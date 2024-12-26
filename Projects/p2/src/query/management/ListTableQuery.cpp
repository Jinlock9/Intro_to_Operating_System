//
// Created by liu on 18-10-25.
//

#include "ListTableQuery.h"

#include <memory>

#include "../../db/Database.h"

QueryResult::Ptr ListTableQuery::execute(Table &table) {
  Database &db = Database::getInstance();
  db.printAllTable();
  return std::make_unique<SuccessMsgResult>(qname);
}

std::string ListTableQuery::toString() { return "QUERY = LIST"; }
