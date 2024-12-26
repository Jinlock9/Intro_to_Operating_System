#include "SumQuery.h"

#include <algorithm>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "../QueryManager.h"
#include "../QueryResult.h"

std::string SumQuery::toString() {
  return "QUERY = SUM " + this->targetTable + "\"";
}

QueryResult::Ptr SumQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  try {
    auto &threadPool = ThreadPool::getInstance();
    // task-based multi-thread version
    auto result = initCondition(table);
    auto numFields = this->operands.size();
    if (result.second) {
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
        std::vector<Table::ValueType> sumVals(numFields, 0);
        auto begin = table.begin() + static_cast<int>(i * chunkSize);
        auto end = (i == numThreads - 1) ? table.end()
                                         : begin + static_cast<int>(chunkSize);
        for (auto it = begin; it != end; ++it) {
          const auto &row = *it;
          if (this->evalCondition(row, table)) {
            for (size_t j = 0; j < numFields; j++) {
              sumVals[j] += row[queryIndex[j]];
            }
          }
        }
        return sumVals;
      };
      for (unsigned int i = 0; i < numThreads - 1; i++) {
        futures.push_back(
            threadPool.enqueue_subtask([&task, i] { return task(i); }));
      }
      std::vector<Table::ValueType> sumVals = task(numThreads - 1);
      for (auto &f : futures) {
        auto maxValsPartial = f.get();
        for (size_t i = 0; i < numFields; i++) {
          sumVals[i] += maxValsPartial[i];
        }
      }
      return std::make_unique<SuccessMsgResult>(sumVals);
    }
    return std::make_unique<SuccessMsgResult>(
        std::vector<Table::ValueType>(numFields));
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