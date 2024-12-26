#include "MaxQuery.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../QueryManager.h"
#include "../QueryResult.h"

std::string MaxQuery::toString() {
  return "QUERY = MAX " + this->targetTable + "\"";
}

QueryResult::Ptr MaxQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  try {
    auto &threadPool = ThreadPool::getInstance();
    // task-based multi-thread version
    auto result = initCondition(table);
    if (result.second) {
      auto numFields = this->operands.size();
      std::vector<std::future<std::vector<Table::ValueType>>> futures;
      // will be replaced with parsedArgs.threads later. auto detect now
      auto numThreads = threadPool.get_subtask_limit();
      auto chunkSize = static_cast<unsigned int>(table.size()) / numThreads;
      std::vector<Table::FieldIndex> queryIndex(numFields);
      for (size_t i = 0; i < numFields; i++) {
        queryIndex[i] = table.getFieldIndex(this->operands[i]);
      }
      auto task = [&queryIndex, &table, numFields, numThreads, chunkSize,
                   this](size_t i) {
        std::vector<Table::ValueType> maxVals(
            numFields, std::numeric_limits<Table::ValueType>::min());
        auto begin = table.begin() + static_cast<int>(i * chunkSize);
        auto end = (i == numThreads - 1) ? table.end()
                                         : begin + static_cast<int>(chunkSize);
        for (auto it = begin; it != end; ++it) {
          const auto &row = *it;
          if (this->evalCondition(row, table)) {
            for (size_t j = 0; j < numFields; j++) {
              maxVals[j] = std::max(maxVals[j], row[queryIndex[j]]);
            }
          }
        }
        return maxVals;
      };
      for (unsigned int i = 0; i < numThreads - 1; i++) {
        futures.push_back(
            threadPool.enqueue_subtask([&task, i] { return task(i); }));
      }
      std::vector<Table::ValueType> maxVals = task(numThreads - 1);
      for (auto &f : futures) {
        auto maxValsPartial = f.get();
        for (size_t i = 0; i < numFields; i++) {
          maxVals[i] = std::max(maxVals[i], maxValsPartial[i]);
        }
      }
      if (maxVals[0] == std::numeric_limits<Table::ValueType>::min()) {
        return std::make_unique<NullQueryResult>();
      }
      return std::make_unique<SuccessMsgResult>(maxVals);
    }
    return std::make_unique<NullQueryResult>();
    ;
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "No such table."s);
  } catch (const IllFormedQueryCondition &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const std::invalid_argument &e) {
    // Cannot convert operand to string
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unkonwn error '?'."_f % e.what());
  }
}
