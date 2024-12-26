#include "SubQuery.h"

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "../../db/Table.h"
#include "../QueryManager.h"

QueryResult::Ptr SubQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() < 2) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  }
  try {
    auto result = initCondition(table);
    auto &threadPool = ThreadPool::getInstance();
    int counts = 0;
    if (result.second) {
      const size_t threadNum = threadPool.get_subtask_limit();
      // if (threadNum > static_cast<unsigned int>(table.size()))
      //   threadNum = static_cast<unsigned int>(table.size());
      auto chunkSize = table.size() / threadNum;

      std::vector<std::future<int>> futures;

      auto numFields = this->operands.size();
      std::vector<Table::FieldIndex> fieldId(numFields);
      for (size_t i = 0; i < numFields; i++) {
        fieldId[i] = table.getFieldIndex(this->operands[i]);
      }
      auto task = [&table, chunkSize, &fieldId, this, threadNum](size_t i) {
        Table::Iterator it_begin =
            table.begin() + (static_cast<int>(chunkSize) * static_cast<int>(i));
        const Table::Iterator it_end =
            (i == threadNum - 1) ? table.end() : it_begin + (int)chunkSize;
        int count = 0;
        auto dest = fieldId.back();
        for (auto it = it_begin; it != it_end; ++it) {
          if (this->evalCondition(*it, table)) {
            auto itId = fieldId.begin();
            auto temp = (*it)[*itId];
            for (++itId; itId != fieldId.end() - 1; ++itId) {
              temp -= (*it)[*itId];
            }
            (*it)[dest] = temp;
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

std::string SubQuery::toString() {
  return "QUERY = SUB" + this->targetTable + "\"";
}
