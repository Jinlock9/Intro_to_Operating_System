#ifndef ADD_QUERY_H
#define ADD_QUERY_H

#include <string>
#include <vector>

#include "../../db/Table.h"
#include "../Query.h"

class AddQuery : public ComplexQuery {
  static constexpr const char *qname = "ADD";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }
};

#endif // ADD_QUERY_H