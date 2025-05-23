#include "QueryParser.h"

#include "Query.h"
#include "QueryBuilders.h"
#include <sstream>
#include <utility>

QueryParser::QueryParser() : first(nullptr), last(nullptr) {}

Query::Ptr QueryParser::parseQuery(const std::string &queryString) {
  if (first == nullptr)
    throw QueryBuilderMatchFailed(queryString);
  auto t = tokenizeQueryString(queryString);
  if (t.token.empty())
    throw QueryBuilderMatchFailed("");
  first->clear();
  return first->tryExtractQuery(&t);
}

void QueryParser::registerQueryBuilder(QueryBuilder::Ptr &&qBuilder) {
  if (first == nullptr) {
    first = std::move(qBuilder);
    last = first.get();
    last->setNext(FailedQueryBuilder::getDefault());
  } else {
    QueryBuilder *temp = last;
    last = qBuilder.get();
    temp->setNext(std::move(qBuilder));
    last->setNext(FailedQueryBuilder::getDefault());
  }
}

TokenizedQueryString
QueryParser::tokenizeQueryString(const std::string &queryString) {
  TokenizedQueryString t;
  t.rawQeuryString = queryString;
  std::stringstream s;
  s.str(queryString);
  std::string tStr;
  while (s >> tStr)
    t.token.push_back(tStr);
  return t;
}
