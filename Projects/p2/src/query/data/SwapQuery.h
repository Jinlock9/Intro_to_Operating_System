#ifndef SWAP_QUERY_H
#define SWAP_QUERY_H

#include <string>

#include "../Query.h"

class SwapQuery : public ComplexQuery {
  static constexpr const char *qname = "SWAP";

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }
};

#endif // SWAP_QUERY_H
