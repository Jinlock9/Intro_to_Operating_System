#include "CountQuery.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../QueryManager.h"

QueryResult::Ptr CountQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() != 0)
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  try {
    auto &threadPool = ThreadPool::getInstance();
    // task-based multi-thread version
    auto result = initCondition(table);
    int counter = 0;
    if (result.second) {
      std::vector<std::future<int>> futures;
      // will be replaced with parsedArgs.threads later. auto detect now
      auto numThreads = threadPool.get_subtask_limit();
      auto chunkSize = static_cast<unsigned int>(table.size()) / numThreads;
      auto task = [&table, numThreads, chunkSize, this](size_t i) {
        int subCounter = 0;
        auto begin = table.begin() + static_cast<int>(i * chunkSize);
        auto end = (i == numThreads - 1) ? table.end()
                                         : begin + static_cast<int>(chunkSize);
        for (auto it = begin; it != end; ++it) {
          const auto &row = *it;
          if (this->evalCondition(row, table)) {
            ++subCounter;
          }
        }
        return subCounter;
      };
      for (unsigned int i = 0; i < numThreads - 1; i++) {
        futures.push_back(
            threadPool.enqueue_subtask([&task, i] { return task(i); }));
      }
      counter = task(numThreads - 1);
      for (auto &f : futures) {
        auto subCounter = f.get();
        counter += subCounter;
      }
      return std::make_unique<SuccessMsgResult>(counter);
    }
    return std::make_unique<SuccessMsgResult>(counter);
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

std::string CountQuery::toString() {
  return "QUERY = COUNT " + this->targetTable + "\"";
}