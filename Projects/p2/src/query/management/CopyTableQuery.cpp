//
// Created by Jiayin on 24-10-29.
//

#include "CopyTableQuery.h"

#include <fstream>
#include <memory>
#include <string>

#include "../../db/Table.h"

QueryResult::Ptr CopyTableQuery::execute_rw(Table &srcTable, Table &destTable) {
  using std::literals::string_literals::operator""s;
  try {
    // will be replaced with parsedArgs.threads later. auto detect now
    // auto numThreads = thread::hardware_concurrency();
    // db.copyTableMulti(targetTable, this->fileName, numThreads);
    destTable.copyFrom(srcTable);
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "No such table."s);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

std::string CopyTableQuery::toString() {
  return "QUERY = Copy TABLE, FILE = \"" + this->fileName + "\"";
}