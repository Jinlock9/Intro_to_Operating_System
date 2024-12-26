//
// Created by liu on 18-10-25.
//

#include "DumpTableQuery.h"

#include <fstream>
#include <memory>
#include <string>

QueryResult::Ptr DumpTableQuery::execute(Table &table) {
  try {
    std::ofstream outfile(this->fileName);
    if (!outfile.is_open()) {
      return std::make_unique<ErrorMsgResult>(qname, "Cannot open file '?'"_f %
                                                         this->fileName);
    }
    outfile << table;
    outfile.close();
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

std::string DumpTableQuery::toString() {
  return "QUERY = Dump TABLE, FILE = \"" + this->fileName + "\"";
}
