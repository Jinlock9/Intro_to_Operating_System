#include "QueryManager.h"

#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <semaphore>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../db/Database.h"
#include "management/CopyTableQuery.h"
#include "management/ListenQuery.h"
#include "management/LoadTableQuery.h"

// auto detect if threadLimit is 0
// inline ThreadPool::ThreadPool(size_t threadLimit) : stop(false) {
//   if (threadLimit == 0) {
//     threadLimit = std::thread::hardware_concurrency();
//   }
//   for (size_t i = 0; i < threadLimit; ++i) {
//       threads.emplace_back([this](std::stop_token stop_token) {
//           work(stop_token);
//       });
//   }
// }
std::unique_ptr<ThreadPool> ThreadPool::instance = nullptr;

size_t ThreadPool::subtask_limit = 1;

void ThreadPool::init(size_t threadLimit) {
  if (threadLimit == 0) {
    threadLimit = std::thread::hardware_concurrency();
  }
  auto &instance = getInstance();
  for (size_t i = 0; i < threadLimit; ++i) {
    instance.threads.emplace_back(
        [&instance](const std::stop_token &stop_token) {
          instance.work(stop_token);
        });
  }
  subtask_limit = std::max(threadLimit - 2ul, 1ul);
}

ThreadPool &ThreadPool::getInstance() {
  if (instance == nullptr) {
    instance = std::make_unique<ThreadPool>();
  }
  return *instance;
}

void ThreadPool::work(const std::stop_token &stop_token) {
  while (!stop_token.stop_requested()) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      condition.wait(
          lock, [this] { return stop || !subtasks.empty() || !tasks.empty(); });
      if (stop && subtasks.empty() && tasks.empty())
        return;
      if (!subtasks.empty()) {
        task = std::move(subtasks.front());
        subtasks.pop();
      } else {
        task = std::move(tasks.front());
        tasks.pop();
      }
    }
    task();
  }
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F &&f,
                         Args &&...args) -> std::future<decltype(f(args...))> {
  using return_type = decltype(f(args...));

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      [args = std::make_tuple(std::forward<Args>(args)...),
       f = std::move(f)]() { return std::apply(f, args); });

  {
    const std::lock_guard<std::mutex> lock(queue_mutex);
    if (stop) {
      throw std::runtime_error("ThreadPool stopped");
    }
    tasks.emplace([task]() { (*task)(); });
  }

  condition.notify_one();
  return task->get_future();
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    const std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  threads.clear(); // nice feature of jthread
}

void QueryManager::processResult(QueryResult::Ptr &&result) {
  std::cout << ++counter << "\n";
  if (result->success()) {
    if (result->display()) {
      std::cout << *result;
    } else {
#ifndef NDEBUG
      std::cout.flush();
      std::cerr << *result;
#endif
    }
  } else {
    std::cout.flush();
    std::cerr << "QUERY FAILED:\n\t" << *result;
  }
}

void QueryManager::processResult(std::future<QueryResult::Ptr> *future) {
  auto result = future->get();
  processResult(std::move(result));
}

void QueryManager::executeRead(Query::Ptr &&query) {
  auto &threadPool = ThreadPool::getInstance();
  auto tableName = query->getTargetTable();
  auto &sem = table_write.at(tableName);
  sem.acquire();
  auto &db = Database::getInstance();
  auto &table = db[query->getTargetTable()];
  table.lock_read();
  resultQueue.push(threadPool.enqueue([query = std::move(query), &table] {
    auto res = query->execute(table);
    table.unlock_read();
    return res;
  }));
  sem.release();
}

void QueryManager::executeWrite(Query::Ptr &&query) {
  auto tableName = query->getTargetTable();
  auto &sem = table_write.at(tableName);
  auto &threadPool = ThreadPool::getInstance();
  sem.acquire();
  auto &db = Database::getInstance();
  auto &table = db[tableName];
  // DANGEROUS! the mutex is locked and unlocked in different threads!
  resultQueue.push(threadPool.enqueue([query = std::move(query), &sem, &table] {
    table.lock_write(); // wait for read to finish
    auto res = query->execute(table);
    sem.release();
    table.unlock_write(); // no actual use
    return res;
  }));
}

void QueryManager::executeLoad(Query::Ptr &&query) {
  auto tableName = query->getTargetTable();
  auto &sem = table_write.emplace(tableName, 1).first->second;
  auto &threadPool = ThreadPool::getInstance();
  sem.acquire();
  auto &db = Database::getInstance();
  auto &table = db.registerTableName(tableName);
  resultQueue.push(threadPool.enqueue([query = std::move(query), &sem, &table] {
    auto res = query->execute(table);
    // std::cerr << "LOAD " << query->getTargetTable() << std::endl;
    sem.release();
    return res;
  }));
}

void QueryManager::executeListen(Query::Ptr &&query) {
  auto listenQuery = dynamic_cast<ListenQuery *>(query.get());
  if (listenQuery) {
    auto fileToListen = listenQuery->get_file_to_listen();
    base_fin.close();
    base_fin.open(fileToListen);
    if (!base_fin.is_open()) {
      listenQuery->fail();
      std::cerr << "Error: could not open " << fileToListen << std::endl;
    } else {
      std::ostringstream oss;
      oss << in->rdbuf();
      buf->str(oss.str());
      buf->clear();
      in->rdbuf(base_fin.rdbuf());
      in->clear();
    }
    auto listen_query = dynamic_cast<ListenQuery *>(query.get());
    if (listen_query) {
      auto res = listen_query->execute_noarg();
      processResult(std::move(res));
      return;
    }
    std::cerr << "Invalid query type for executeListen\n";
  } else {
    std::cerr << "Invalid query type for executeListen\n";
  }
}

void QueryManager::executeReadWrite(Query::Ptr &&query) {
  auto copyQuery = dynamic_cast<CopyTableQuery *>(query.get());
  if (copyQuery) {
    auto &threadPool = ThreadPool::getInstance();
    auto readTableName = copyQuery->getTargetTable();
    auto newTableName = copyQuery->getFileName();
    auto &sem = table_write.at(readTableName);
    sem.acquire();
    auto &db = Database::getInstance();
    auto &table = db[query->getTargetTable()];
    table.lock_read();
    auto &sem_w = table_write.emplace(newTableName, 1).first->second;
    sem_w.acquire();
    auto &destTable = db.registerTableName(newTableName);
    resultQueue.push(threadPool.enqueue(
        [query = std::move(query), &sem_w, &table, &destTable] {
          auto copy_query = dynamic_cast<CopyTableQuery *>(query.get());
          if (copy_query) {
            auto res = copy_query->execute_rw(table, destTable);
            table.unlock_read();
            sem_w.release();
            return res;
          }
          throw std::runtime_error("Invalid query type for executeReadWrite");
        }));
    sem.release();
  }
}

void QueryManager::executeQuit(Query::Ptr &&query) {
  while (!resultQueue.empty()) {
    processResult(&resultQueue.front());
    resultQueue.pop();
  }
  std::cout.flush();
  std::cerr.flush();
  exit(0);
}

void QueryManager::executeDrop(Query::Ptr &&query) {
  auto tableName = query->getTargetTable();
  auto &sem = table_write.at(tableName);
  sem.acquire();
  auto &db = Database::getInstance();
  auto &table = db[tableName];
  table.lock_write();
  auto p_res = query->execute(table);
  std::promise<QueryResult::Ptr> promise;
  resultQueue.push(promise.get_future());
  promise.set_value(std::move(p_res));
  table.unlock_write();
  sem.release();
}

// debug queries have to be executed saperately
// such that the output will not be messed up
void QueryManager::executeDebug(Query::Ptr &&query) {
  while (!resultQueue.empty()) {
    processResult(&resultQueue.front());
    resultQueue.pop();
  }
  std::cout.flush();
  std::cerr.flush();
  auto &db = Database::getInstance();
  auto &table = db[query->getTargetTable()];
  auto p_res = query->execute(table);
  processResult(std::move(p_res));
}

void QueryManager::execute(const std::string &queryStr) {
  if (resultQueue.size() > result_size_limit) {
    processResult(&resultQueue.front());
    resultQueue.pop();
  }
  Query::Ptr query = parser->parseQuery(queryStr);
  auto type = query->getQueryType();
  if (type == QueryType::READ) {
    executeRead(std::move(query));
  } else if (type == QueryType::WRITE) {
    executeWrite(std::move(query));
  } else if (type == QueryType::LOAD) {
    executeLoad(std::move(query));
  } else if (type == QueryType::COPYTBL) {
    executeReadWrite(std::move(query));
  } else if (type == QueryType::LISTEN) {
    executeListen(std::move(query));
  } else if (type == QueryType::QUIT) {
    executeQuit(std::move(query));
  } else if (type == QueryType::DROP) {
    executeDrop(std::move(query));
  } else if (type == QueryType::DEBUG) {
    executeDebug(std::move(query));
  } else {
    std::cerr << "Invalid query type\n";
  }
  //   auto result = query->execute();
  //   std::cout << ++counter << "\n";
  //   if (result->success()) {
  //     if (result->display()) {
  //       std::cout << *result;
  //     } else {
  // #ifndef NDEBUG
  //       std::cout.flush();
  //       std::cerr << *result;
  // #endif
  //     }
  //   } else {
  //     std::cout.flush();
  //     std::cerr << "QUERY FAILED:\n\t" << *result;
  //   }
}