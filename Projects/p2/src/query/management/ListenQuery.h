#ifndef PROJECT_LISTENQUERY_H
#define PROJECT_LISTENQUERY_H

#include <string>
#include <utility>

#include "../Query.h"

class ListenQuery : public Query {
  static constexpr const char *qname = "QUIT";
  std::string file_to_listen;
  bool success;

public:
  explicit ListenQuery(std::string file_to_listen)
      : file_to_listen(std::move(file_to_listen)), success(true) {}

  QueryResult::Ptr execute(Table &table) override;
  QueryResult::Ptr execute_noarg();

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::LISTEN; }

  const std::string &get_file_to_listen() const { return file_to_listen; }

  void fail() { success = false; }
};

#endif // PROJECT_LISTENQUERY_H