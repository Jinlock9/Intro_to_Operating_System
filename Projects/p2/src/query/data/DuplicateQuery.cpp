#include "DuplicateQuery.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../QueryManager.h"

QueryResult::Ptr DuplicateQuery::execute(Table &table) {
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
      std::vector<size_t> duplicate_indices;
      for (auto it = table.begin(); it != table.end(); ++it) {
        if (this->evalCondition(*it, table)) {
          // Duplicate
          auto key = it->key() + "_copy";
          if (table[key] == nullptr) { // avoid overwriting
            duplicate_indices.emplace_back(it - table.begin());
            counter++;
          }
        }
      }

#ifdef SHOW_TIME
      auto search_end = std::chrono::high_resolution_clock::now();
#endif

      for (size_t &duplicate_index : duplicate_indices) {
        table.DuplicateRow(static_cast<size_t>(duplicate_index));
      }

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
      std::vector<uint8_t> duplicate_flags(table.size(), 0);

      std::vector<std::future<void>> futures;
      auto thread_num = threadPool.get_subtask_limit();
      auto chunk_size = table.size() / thread_num;
      auto task = [&table, this, thread_num, chunk_size,
                   &duplicate_flags](size_t i) {
        auto begin = table.begin() + static_cast<int>(i * chunk_size);
        auto end = (i == thread_num - 1) ? table.end()
                                         : begin + static_cast<int>(chunk_size);
        auto tb_begin = table.begin();
        for (auto it = begin; it != end; ++it) {
          if (this->evalCondition(*it, table)) {
            auto new_key = it->key() + "_copy";
            if (this->evalCondition(*it, table) &&
                table[it->key() + "_copy"] == nullptr) { // avoid overwriting
              duplicate_flags[static_cast<size_t>(it - tb_begin)] = 1;
            }
          }
        }
      };

      for (size_t i = 0; i < thread_num - 1; i++) {
        futures.push_back(
            threadPool.enqueue_subtask([&task, i] { return task(i); }));
      }
      task(thread_num - 1);
      for (auto &f : futures) {
        f.get();
      }

#ifdef SHOW_TIME
      auto search_end = std::chrono::high_resolution_clock::now();
#endif

      counter = table.DuplicateRows(duplicate_flags);

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

std::string DuplicateQuery::toString() {
  return "QUERY = DUPLICATE " + this->targetTable + "\"";
}
