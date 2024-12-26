#include "ListenQuery.h"

#include <iostream>
#include <memory>
#include <string>

std::string ListenQuery::toString() { return "QUERY = Quit"; }

QueryResult::Ptr ListenQuery::execute(Table &table) {
  throw std::logic_error("This function should not be called");
}

QueryResult::Ptr ListenQuery::execute_noarg() {
  std::string file_name;
  const size_t pos = file_to_listen.find_last_of('/');
  if (pos != std::string::npos) {
    file_name = file_to_listen.substr(pos + 1);
  } else {
    file_name = file_to_listen;
  }
  if (success) {
    const std::string result = "ANSWER = ( listening from ? )"_f % file_name;
    return std::make_unique<SuccessMsgResult>(result);
  }

  return std::make_unique<ErrorMsgResult>(qname, "Failed to listen to file");
}