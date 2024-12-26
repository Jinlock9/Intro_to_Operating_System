#ifndef SUB_QUERY_H
#define SUB_QUERY_H

#include <string>
#include <vector>

#include "../../db/Table.h"
#include "../Query.h"

class SubQuery : public ComplexQuery {
  static constexpr const char *qname = "SUB";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }
};

#endif // SUB_QUERY_H