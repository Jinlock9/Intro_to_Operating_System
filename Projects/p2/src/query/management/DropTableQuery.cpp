//
// Created by liu on 18-10-25.
//

#include "DropTableQuery.h"

#include <memory>
#include <string>

#include "../../db/Database.h"

QueryResult::Ptr DropTableQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  try {
    auto &db = Database::getInstance();
    db.dropTable(this->targetTable);
    return std::make_unique<SuccessMsgResult>(qname);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "No such table."s);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

std::string DropTableQuery::toString() {
  return "QUERY = DROP, Table = \"" + targetTable + "\"";
}