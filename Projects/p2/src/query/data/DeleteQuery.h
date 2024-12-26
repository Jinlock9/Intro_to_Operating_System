#ifndef PROJECT_DELETEQUERY_H
#define PROJECT_DELETEQUERY_H

#include <string>

#include "../Query.h"

class DeleteQuery : public ComplexQuery {
  static constexpr const char *qname = "DELETE";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }
};

#endif
