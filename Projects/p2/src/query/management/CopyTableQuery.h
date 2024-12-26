//
// Created by Jiayin on 24-10-29.
//

#ifndef PROJECT_COPYTABLEQUERY_H
#define PROJECT_COPYTABLEQUERY_H

#include <string>
#include <utility>

#include "../Query.h"

class CopyTableQuery : public Query {
  static constexpr const char *qname = "COPYTABLE";
  const std::string fileName;

public:
  explicit CopyTableQuery(std::string table, std::string fileName)
      : Query(std::move(table)), fileName(std::move(fileName)) {}

  QueryResult::Ptr execute(Table &table) override {
    throw std::logic_error("This function should not be called");
  }
  QueryResult::Ptr execute_rw(Table &srcTable, Table &destTable);

  std::string toString() override;

  std::string getFileName() { return fileName; }

  QueryType getQueryType() const override { return QueryType::COPYTBL; }
};

#endif // PROJECT_COPYTABLEQUERY_H