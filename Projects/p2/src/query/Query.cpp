//
// Created by liu on 18-10-25.
//

#include "Query.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <utility>

#include "../db/Database.h"

std::pair<std::string, bool> ComplexQuery::initCondition(const Table &table) {
  const std::unordered_map<std::string, int> opmap{
      {">", '>'}, {"<", '<'}, {"=", '='}, {">=", 'g'}, {"<=", 'l'},
  };
  std::pair<std::string, bool> result = {"", true};
  for (auto &cond : condition) {
    if (cond.field == "KEY") {
      if (cond.op != "=") {
        throw IllFormedQueryCondition("Can only compare equivalence on KEY");
      } else if (result.first.empty()) {
        result.first = cond.value;
      } else if (result.first != cond.value) {
        result.second = false;
        return result;
      }
    } else {
      cond.fieldId = table.getFieldIndex(cond.field);
      cond.valueParsed =
          (Table::ValueType)std::strtol(cond.value.c_str(), nullptr, 10);
      int op = 0;
      try {
        op = opmap.at(cond.op);
      } catch (const std::out_of_range &e) {
        throw IllFormedQueryCondition(
            R"("?" is not a valid condition operator.)"_f % cond.op);
      }
      switch (op) {
      case '>':
        cond.comp = std::greater<>();
        break;
      case '<':
        cond.comp = std::less<>();
        break;
      case '=':
        cond.comp = std::equal_to<>();
        break;
      case 'g':
        cond.comp = std::greater_equal<>();
        break;
      case 'l':
        cond.comp = std::less_equal<>();
        break;
      default:
        assert(0); // should never be here
      }
    }
  }
  return result;
}

bool ComplexQuery::evalCondition(const Table::Object &object,
                                 const Table &table) {
  // evalCondition is only called for objects in the table
  // So no need to check existence
  if (table.isDeleted(object.key())) {
    return false;
  }
  bool ret = true;
  for (const auto &cond : condition) {
    if (cond.field != "KEY") {
      if (cond.fieldId >= 200u) {
        std::cerr << "idx out of range" << cond.fieldId << std::endl;
        throw IllFormedQueryCondition("Field index out of range");
      }
      ret = ret && cond.comp(object[cond.fieldId], cond.valueParsed);
    } else {
      ret = ret && (object.key() == cond.value);
    }
  }
  return ret;
}

// bool ComplexQuery::evalConditionNoDeleteCheck(const Table::Object &object) {
//   bool ret = true;
//   for (const auto &cond : condition) {
//     if (cond.field != "KEY") {
//       ret = ret && cond.comp(object[cond.fieldId], cond.valueParsed);
//     } else {
//       ret = ret && (object.key() == cond.value);
//     }
//   }
//   return ret;
// }

// bool ComplexQuery::testKeyCondition(
//     Table &table,
//     const std::function<void(bool, Table::Object::Ptr &&)> &function) {
//   auto condResult = initCondition(table);
//   if (!condResult.second) {
//     function(false, nullptr);
//     return true;
//   }
//   if (!condResult.first.empty()) {
//     auto object = table[condResult.first];
//     if (object != nullptr && evalCondition(*object)) {
//       function(true, std::move(object));
//     } else {
//       function(false, nullptr);
//     }
//     return true;
//   }
//   return false;
// }
