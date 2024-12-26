//
// Created by jiayin on 18-10-25.
//
#include "SelectQuery.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

QueryResult::Ptr SelectQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  if (this->operands.size() < 1) // There must have KEY
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  try {
    std::vector<std::string> output;
    auto initResult = initCondition(table);
    if (initResult.second) {
      auto numFields = this->operands.size();
      std::vector<Table::FieldIndex> queryIndex(numFields - 1);
      std::map<Table::KeyType, std::vector<Table::ValueType>> result;
      for (size_t i = 0; i < numFields - 1; i++) {
        // Key must be included, but no need to be in index
        queryIndex[i] = table.getFieldIndex(this->operands[i + 1]);
      }
      for (auto it = table.begin(); it != table.end(); ++it) {
        if (this->evalCondition(*it, table)) {
          std::vector<Table::ValueType> selectedFields;
          for (size_t i = 0; i < numFields - 1; i++) {
            selectedFields.push_back((*it)[queryIndex[i]]);
          }
          result[it->key()] = std::move(selectedFields);
        }
      }

      if (!result.empty()) {
        for (const auto &entry : result) {
          std::string line = "( " + entry.first + " ";
          for (const auto &value : entry.second) {
            line += std::to_string(value) + " ";
          }
          line += ")";
          output.push_back(line);
        }
      }
    }
    return std::make_unique<SelectResult>(std::move(output));
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

std::string SelectQuery::toString() {
  return "QUERY = SELECT " + this->targetTable + "\"";
}
