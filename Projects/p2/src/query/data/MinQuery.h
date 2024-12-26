#ifndef PROJECT_MINQUERY_H
#define PROJECT_MINQUERY_H

#include <string>

#include "../Query.h"

class MinQuery : public ComplexQuery {
  static constexpr const char *qname = "MIN";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::READ; }
};

#endif // PROJECT_MINQUERY_H