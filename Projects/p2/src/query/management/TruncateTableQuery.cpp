//
// Created by Jiayin on 24-10-31.
//

#include "TruncateTableQuery.h"

#include <memory>
#include <string>

#include "../../db/Database.h"

QueryResult::Ptr TruncateTableQuery::execute(Table &table) {
  using std::literals::string_literals::operator""s;
  try {
    table.clear();
    return std::make_unique<SuccessMsgResult>(qname);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "No such table."s);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

std::string TruncateTableQuery::toString() {
  return "QUERY = TRUNCATE, Table = \"" + targetTable + "\"";
}