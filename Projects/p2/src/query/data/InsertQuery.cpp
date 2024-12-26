//
// Created by liu on 18-10-25.
//

#include "InsertQuery.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../QueryManager.h"
#include "../QueryResult.h"

QueryResult::Ptr InsertQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.empty())
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),
                                            "No operand (? operands)."_f %
                                                operands.size());
  try {
    const auto &key = this->operands.front();
    std::vector<Table::ValueType> data;
    data.reserve(this->operands.size() - 1);
    for (auto it = ++this->operands.begin(); it != this->operands.end(); ++it) {
      data.emplace_back(strtol(it->c_str(), nullptr, 10));
    }
    table.insertByIndex(key, std::move(data));
    return std::make_unique<NullQueryResult>();
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

std::string InsertQuery::toString() {
  return "QUERY = INSERT " + this->targetTable + "\"";
}
