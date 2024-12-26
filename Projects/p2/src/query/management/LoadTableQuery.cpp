//
// Created by liu on 18-10-25.
//

#include "LoadTableQuery.h"

#include <fstream>
#include <memory>
#include <string>

#include "../../db/Database.h"

QueryResult::Ptr LoadTableQuery::execute(Table &table) {
  try {
    std::ifstream infile(this->fileName);
    if (!infile.is_open()) {
      return std::make_unique<ErrorMsgResult>(qname, "Cannot open file '?'"_f %
                                                         this->fileName);
    }
    Database &db = Database::getInstance();
    auto ptr = db.loadFromStreamAsPtr(infile, this->fileName);
    table.copyFrom(std::move(*ptr));
    infile.close();
    return std::make_unique<NullQueryResult>();
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

std::string LoadTableQuery::toString() {
  return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}
