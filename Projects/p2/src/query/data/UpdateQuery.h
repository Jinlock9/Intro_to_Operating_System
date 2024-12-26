//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_UPDATEQUERY_H
#define PROJECT_UPDATEQUERY_H

#include <string>
#include <utility>
#include <vector>

#include "../Query.h"

class UpdateQuery : public ComplexQuery {
  static constexpr const char *qname = "UPDATE";
  Table::ValueType
      fieldValue; // = (operands[0]=="KEY")? 0 :std::stoi(operands[1]);
  Table::FieldIndex fieldId;
  Table::KeyType keyValue;

public:
  using ComplexQuery::ComplexQuery;

  QueryResult::Ptr execute(Table &table) override;

  std::string toString() override;

  QueryType getQueryType() const override { return QueryType::WRITE; }

  UpdateQuery(std::string targetTable, std::vector<std::string> operands,
              std::vector<QueryCondition> condition)
      : ComplexQuery(std::move(targetTable), std::move(operands),
                     std::move(condition)),
        fieldValue(), fieldId() {}
};

#endif // PROJECT_UPDATEQUERY_H
