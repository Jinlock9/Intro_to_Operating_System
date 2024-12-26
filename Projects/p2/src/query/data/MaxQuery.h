#ifndef PROJECT_MAXQUERY_H
#define PROJECT_MAXQUERY_H

#include <string>

#include "../Query.h"

class MaxQuery : public ComplexQuery {
  static constexpr const char *qname = "MAX";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::READ; }
};

#endif // PROJECT_MAXQUERY_H