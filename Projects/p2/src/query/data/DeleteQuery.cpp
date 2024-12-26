#include "DeleteQuery.h"

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "../QueryManager.h"

QueryResult::Ptr DeleteQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() != 0)
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  try {
    Table::SizeType counter = 0;
    auto result = initCondition(table);
    auto &threadPool = ThreadPool::getInstance();
    if (result.second) {
#ifdef SINGLE_THREAD
#ifdef SHOW_TIME
      auto start = std::chrono::high_resolution_clock::now();
#endif

      std::vector<bool> delete_flag;
      delete_flag.reserve(table.size());
      delete_flag.assign(table.size(), false);
      for (auto it = table.begin(); it != table.end(); ++it) {
        if (this->evalCondition(*it, table)) {
          // Delete
          delete_flag[static_cast<size_t>(it - table.begin())] = true;
        }
      }

#ifdef SHOW_TIME
      auto search_end = std::chrono::high_resolution_clock::now();
#endif

      counter = table.DeleteRows(delete_flag);

#ifdef SHOW_TIME
      auto end = std::chrono::high_resolution_clock::now();
      auto search_duration =
          std::chrono::duration_cast<std::chrono::microseconds>(search_end -
                                                                start);
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "Search: " << search_duration.count() << "us" << std::endl;
      std::cout << "Total: " << duration.count() << "us" << std::endl;
#endif
#else
#ifdef SHOW_TIME
      auto start = std::chrono::high_resolution_clock::now();
#endif
      std::vector<uint8_t> delete_flag(table.size(), 0);

      std::vector<std::future<void>> futures;
      auto thread_num = threadPool.get_subtask_limit();
      auto chunk_size = static_cast<unsigned int>(table.size()) / thread_num;

      for (unsigned int i = 0; i < thread_num; i++) {
        auto begin = table.begin() + static_cast<int>(i * chunk_size);
        auto end = (i == thread_num - 1) ? table.end()
                                         : begin + static_cast<int>(chunk_size);
        auto offset = static_cast<size_t>(begin - table.begin());
        futures.push_back(threadPool.enqueue_subtask(
            [offset, begin, end, &delete_flag, this, &table] {
              for (auto it = begin; it != end; ++it) {
                // Delete only if the entry is not already marked as deleted
                if (this->evalCondition(*it, table)) {
                  delete_flag[offset + static_cast<size_t>(it - begin)] = 1;
                }
              }
            }));
      }
      for (auto &f : futures) {
        f.get();
      }

#ifdef SHOW_TIME
      auto search_end = std::chrono::high_resolution_clock::now();
#endif

      counter = table.DeleteRows(delete_flag);

#ifdef SHOW_TIME
      auto end = std::chrono::high_resolution_clock::now();
      auto search_duration =
          std::chrono::duration_cast<std::chrono::microseconds>(search_end -
                                                                start);
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "Search: " << search_duration.count() << "us" << std::endl;
      std::cout << "Total: " << duration.count() << "us" << std::endl;
#endif
#endif
    }
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

std::string DeleteQuery::toString() {
  return "QUERY = DELETE " + this->targetTable + "\"";
}
