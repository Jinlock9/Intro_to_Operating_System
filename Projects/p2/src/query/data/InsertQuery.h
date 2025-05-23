//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_INSERTQUERY_H
#define PROJECT_INSERTQUERY_H

#include <string>

#include "../Query.h"

class InsertQuery : public ComplexQuery {
  static constexpr const char *qname = "INSERT";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }
};

#endif // PROJECT_INSERTQUERY_H
