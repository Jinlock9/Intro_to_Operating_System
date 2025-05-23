//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_DROPTABLEQUERY_H
#define PROJECT_DROPTABLEQUERY_H

#include <string>

#include "../Query.h"

class DropTableQuery : public Query {
  static constexpr const char *qname = "DROP";

public:
  using Query::Query;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::DROP; }
};

#endif // PROJECT_DROPTABLEQUERY_H
