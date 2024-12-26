#include "UpdateQuery.h"

#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../QueryManager.h"

QueryResult::Ptr UpdateQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() != 2)
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  std::atomic<Table::SizeType> counter = 0;
  try {
    if (this->operands[0] == "KEY") {
      this->keyValue = this->operands[1];
    } else {
      this->fieldId = table.getFieldIndex(this->operands[0]);
      this->fieldValue =
          (Table::ValueType)strtol(this->operands[1].c_str(), nullptr, 10);
    }
    auto [key, quick_result] = initCondition(table);
    if (quick_result) {
      if (!key.empty()) {
        // std::cout << "Enter here" << std::endl;
        // std::cout.flush();
        auto obj = table[key];
        // std::cout << "key: " << key << std::endl;
        // std::cout.flush();
        //  if (obj){
        //    std::cout << "obj->key(): " << obj->key() << std::endl;
        //    std::cout.flush();
        //    std::cout << "object exits" << std::endl;
        //    std::cout.flush();
        //  }
        //  for (const auto& cond : condition){
        //    std::cout << cond.field << " " << cond.op << " " << cond.value <<
        //    std::endl; std::cout.flush();
        //  }
        if (obj && this->evalCondition(*obj, table)) {
          if (this->keyValue.empty()) {
            obj->get(this->fieldId) = this->fieldValue;
          } else {
            obj->setKey(this->keyValue);
          }
          ++counter;
        }
      } else {
        std::vector<std::future<void>> futures;
        auto &threadPool = ThreadPool::getInstance();
        auto numThreads = threadPool.get_subtask_limit();
        auto chunkSize = static_cast<unsigned int>(table.size()) / numThreads;
        auto task = [&table, numThreads, chunkSize, this, &counter](size_t i) {
          auto begin = table.begin() + static_cast<int>(i * chunkSize);
          auto end = (i == numThreads - 1)
                         ? table.end()
                         : begin + static_cast<int>(chunkSize);
          for (auto it = begin; it != end; ++it) {
            if (this->evalCondition(*it, table)) {
              if (this->keyValue.empty()) {
                (*it)[this->fieldId] = this->fieldValue;
              } else {
                it->setKey(this->keyValue);
              }
              ++counter;
            }
          }
        };
        for (unsigned int i = 0; i < numThreads - 1; i++) {
          futures.push_back(
              threadPool.enqueue_subtask([&task, i] { task(i); }));
        }
        task(numThreads - 1);
        for (auto &f : futures) {
          f.get();
        }
      }
    }
    // else{
    //   printf("Come into else.\n");
    // }

    return std::make_unique<RecordCountResult>(counter);
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

std::string UpdateQuery::toString() {
  return "QUERY = UPDATE " + this->targetTable + "\"";
}
