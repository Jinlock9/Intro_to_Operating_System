//
// Created by liu on 18-10-23.
//

#include "Table.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../query/QueryManager.h"
#include "Database.h"

void Table::lock_read() {
  const std::lock_guard<std::mutex> lock(access_mutex);
  ++read_count;
}

void Table::unlock_read() {
  const std::lock_guard<std::mutex> lock(access_mutex);
  if (--read_count == 0) {
    read_cv.notify_one();
  }
}

void Table::lock_write() {
  std::unique_lock<std::mutex> lock(access_mutex);
  read_cv.wait(lock, [this] { return read_count == 0; });
}

void Table::unlock_write() {}

Table::FieldIndex
Table::getFieldIndex(const Table::FieldNameType &field) const {
  try {
    return this->fieldMap.at(field);
  } catch (const std::out_of_range &e) {
    throw TableFieldNotFound(R"(Field name "?" doesn't exists.)"_f % (field));
  }
}

void Table::insertByIndex(const KeyType &key, std::vector<ValueType> &&data) {
  if (this->keyMap.find(key) != this->keyMap.end()) {
    // WARNING: Not tested
    // When the key is found, overwrite if it is deleted
    if (this->deleted[this->keyMap[key]]) {
      this->deleted[this->keyMap[key]] = false;
      delete_count--;
      this->data[this->keyMap[key]].datum = std::move(data);
      return;
    } else {
      std::string const err = "In Table \"" + this->tableName + "\" : Key \"" +
                              key + "\" already exists!";
      throw ConflictingKey(err);
    }
  }
  this->keyMap.emplace(key, this->data.size());
  this->data.emplace_back(key, data);
  this->deleted.emplace_back(false);
}

Table::Object::Ptr Table::operator[](const Table::KeyType &key) {
  auto it = keyMap.find(key);
  if (it == keyMap.end() || deleted[it->second]) {
    // not found
    return nullptr;
  } else {
    return createProxy(
        data.begin() +
            static_cast<std::vector<Table::Datum>::difference_type>(it->second),
        this);
  }
}

Table::ConstObject::Ptr Table::operator[](const Table::KeyType &key) const {
  auto it = keyMap.find(key);
  if (it == keyMap.end() || deleted[it->second]) {
    // not found
    return nullptr;
  } else {
    return createProxy(
        data.begin() +
            static_cast<std::vector<Table::Datum>::difference_type>(it->second),
        this);
  }
}

std::ostream &operator<<(std::ostream &os, const Table &table) {
  const int width = 10;
  std::stringstream buffer;
  buffer << table.tableName << "\t" << (table.fields.size() + 1) << "\n";
  buffer << std::setw(width) << "KEY";
  for (const auto &field : table.fields) {
    buffer << std::setw(width) << field;
  }
  buffer << "\n";
  auto numFields = table.fields.size();
  for (const auto &datum : table.data) {
    if (!table.deleted[static_cast<size_t>(&datum - &table.data[0])]) {
      buffer << std::setw(width) << datum.key;
      for (decltype(numFields) i = 0; i < numFields; ++i) {
        buffer << std::setw(width) << datum.datum[i];
      }
      buffer << "\n";
    }
  }
  return os << buffer.str();
}

Table::SizeType Table::DeleteRows(const std::vector<uint8_t> &delete_flag) {
  Table::SizeType counter = 0;
  for (size_t i = 0; i < delete_flag.size(); i++) {
    if (delete_flag[i]) {
      deleted[i] = 1;
      delete_count++;
      counter++;
    }
  }
  if (delete_count > data.size() / 10) {
    RebuildVectors();
  }
  return counter;
}

void Table::RebuildVectors() {
  std::vector<Datum> new_data;
  new_data.reserve(data.size() - delete_count);
  std::unordered_map<KeyType, SizeType> new_keyMap;
  new_keyMap.reserve(data.size() - delete_count);
  size_t new_size = 0;
  const size_t data_size = data.size();
  ThreadPool &threadPool = ThreadPool::getInstance();
  auto fut = threadPool.enqueue_subtask([this, data_size, &new_data] {
    for (size_t i = 0; i < data_size; i++) {
      if (!deleted[i]) {
        new_data.emplace_back(data[i]);
      }
    }
  });
  for (size_t i = 0; i < data_size; i++) {
    if (!deleted[i]) {
      new_keyMap.emplace(data[i].key, new_size++);
    }
  }
  fut.get();
  delete_count = 0;
  std::vector<uint8_t> new_deleted(new_data.size(), 0);
  data = std::move(new_data);
  keyMap = std::move(new_keyMap);
  deleted = std::move(new_deleted);
}

void Table::DuplicateRow(size_t index) {
  auto new_key = data[index].key + "_copy";
  this->keyMap.emplace(new_key, this->data.size());
  this->data.emplace_back(new_key, data[index].datum);
}

Table::SizeType
Table::DuplicateRows(const std::vector<uint8_t> &duplicate_flags) {
  Table::SizeType counter = 0;
  for (size_t i = 0; i < duplicate_flags.size(); i++) {
    if (duplicate_flags[i]) {
      auto new_key = data[i].key + "_copy";
      if (this->keyMap.find(new_key) != this->keyMap.end() &&
          this->isDeleted(new_key)) {
        deleted[keyMap[new_key]] = false;
        delete_count--;
        data[keyMap[new_key]].datum = data[i].datum;
      } else {
        keyMap.emplace(new_key, data.size());
        data.emplace_back(new_key, data[i].datum);
        deleted.emplace_back(false);
      }
      counter++;
    }
  }
  return counter;
}

void Table::copyFrom(const Table &table) {
  fields = table.fields;
  fieldMap = table.fieldMap;
  data = table.data;
  keyMap = table.keyMap;
  deleted = table.deleted;
  delete_count = table.delete_count;
  tableName = table.tableName;
}

void Table::copyFrom(Table &&table) {
  fields = std::move(table.fields);
  fieldMap = std::move(table.fieldMap);
  data = std::move(table.data);
  keyMap = std::move(table.keyMap);
  deleted = std::move(table.deleted);
  delete_count = table.delete_count;
  tableName = std::move(table.tableName);
}