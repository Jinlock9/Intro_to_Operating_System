//
// Created by liu on 18-10-23.
//

#ifndef PROJECT_DB_H
#define PROJECT_DB_H

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "Table.h"

class Database {
private:
  std::shared_mutex tableMutex;
  /**
   * A unique pointer to the global database object
   */
  static std::unique_ptr<Database> instance;

  /**
   * The map of tableName -> table unique ptr
   */
  std::unordered_map<std::string, Table::Ptr> tables;

  /**
   * The map of fileName -> tableName
   */
  std::unordered_map<std::string, std::string> fileTableNameMap;

  /**
   * The default constructor is made private for singleton instance
   */
  Database() = default;

public:
  void lockDB() { tableMutex.lock(); }
  void unlockDB() { tableMutex.unlock(); }
  void testDuplicate(const std::string &tableName);

  Table &registerTableName(const std::string &tableName);

  Table::Ptr loadFromStreamAsPtr(std::istream &is, const std::string &source);

  Table &registerTable(Table::Ptr &&table);

  void dropTable(const std::string &tableName);

  void truncateTable(const std::string &tableName);

  void printAllTable();

  Table &operator[](const std::string &tableName);

  const Table &operator[](const std::string &tableName) const;

  Database &operator=(const Database &) = delete;

  Database &operator=(Database &&) = delete;

  Database(const Database &) = delete;

  Database(Database &&) = delete;

  ~Database() = default;

  static Database &getInstance();

  void updateFileTableName(const std::string &fileName,
                           const std::string &tableName);

  std::string getFileTableName(const std::string &fileName);

  /**
   * Load a table from an input stream (i.e., a file)
   * @param is
   * @param source
   * @return reference of loaded table
   */
  Table &loadTableFromStream(std::istream &is, const std::string &source = "");

  Table &copyTable(const std::string &tableNameOld,
                   const std::string &tableNameNew);

  Table &copyTableMulti(const std::string &tableNameOld,
                        const std::string &tableNameNew,
                        const unsigned int numThreads);

  void exit();
};

#endif // PROJECT_DB_H
