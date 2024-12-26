#include "SwapQuery.h"

#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../db/Table.h"
#include "../QueryManager.h"

QueryResult::Ptr SwapQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() != 2)
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  try {
    auto &threadPool = ThreadPool::getInstance();
    auto result = initCondition(table);
    int counts = 0;
    if (result.second) {
      const size_t threadNum = threadPool.get_subtask_limit();
      auto chunkSize = table.size() / threadNum;

      std::vector<std::future<int>> futures;

      const Table::FieldIndex field1 = table.getFieldIndex(this->operands[0]);
      const Table::FieldIndex field2 = table.getFieldIndex(this->operands[1]);
      auto task = [&table, chunkSize, field1, field2, this,
                   threadNum](size_t i) {
        Table::Iterator it_begin =
            table.begin() + (static_cast<int>(chunkSize) * static_cast<int>(i));
        const Table::Iterator it_end =
            (i == threadNum - 1) ? table.end() : it_begin + (int)chunkSize;
        int count = 0;
        for (auto it = it_begin; it != it_end; ++it) {
          if (this->evalCondition(*it, table)) {
            std::swap((*it)[field1], (*it)[field2]);
            count++;
          }
        }
        return count;
      };
      for (unsigned int i = 0; i < threadNum - 1; i++) {
        futures.push_back(
            threadPool.enqueue_subtask([&task, i] { return task(i); }));
      }
      counts = task(threadNum - 1);
      for (auto &f : futures) {
        counts += f.get();
      }
    }
    return std::make_unique<RecordCountResult>(counts);
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

std::string SwapQuery::toString() {
  return "QUERY = SWAP \"" + this->targetTable + "\"";
}