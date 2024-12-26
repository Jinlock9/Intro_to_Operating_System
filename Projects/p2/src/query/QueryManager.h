#ifndef PROJECT_QUERYMANAGER_H
#define PROJECT_QUERYMANAGER_H

#include <fstream>
#include <future>
#include <memory>
#include <queue>
#include <semaphore>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "./QueryParser.h"
#include "Query.h"

constexpr size_t SUBTASK_LIMIT = 4u;

class ThreadPool {
public:
  // should be called before any other function if want to specify threadLimit
  static void init(size_t threadLimit);
  static ThreadPool &getInstance();
  size_t get_subtask_limit() { return subtask_limit; }
  ThreadPool() = default;
  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;

  template <class F, class... Args>
  auto enqueue_subtask(F &&f,
                       Args &&...args) -> std::future<decltype(f(args...))>;
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ~ThreadPool();

private:
  static std::unique_ptr<ThreadPool> instance;
  void work(const std::stop_token &stop_token);
  static size_t subtask_limit;
  std::queue<std::function<void()>> subtasks;
  std::queue<std::function<void()>> tasks;
  std::vector<std::jthread> threads;
  std::mutex queue_mutex;
  std::condition_variable_any condition;
  bool stop;
};

class QueryManager {
  QueryParser *parser;
  std::istream *in;       // for LISTEN
                          //   size_t curThreads = 0;
  std::ifstream base_fin; // store the base buffer of in
  std::istringstream *buf;
  size_t result_size_limit;
  size_t counter = 0;
  std::mutex result_mutex;
  std::queue<std::future<QueryResult::Ptr>> resultQueue;
  void executeRead(Query::Ptr &&query);
  void executeWrite(Query::Ptr &&query);
  void executeLoad(Query::Ptr &&query);
  void executeQuit(Query::Ptr &&query);
  void executeListen(Query::Ptr &&query);
  void executeReadWrite(Query::Ptr &&query);
  void executeDrop(Query::Ptr &&query);
  void executeDebug(Query::Ptr &&query);
  void processResult(std::future<QueryResult::Ptr> *future);
  void processResult(QueryResult::Ptr &&result);
  std::unordered_map<std::string, std::binary_semaphore> table_write;

public:
  explicit QueryManager(size_t result_size_limit, std::istream *in,
                        QueryParser *parser)
      : parser(parser), in(in), buf(new std::istringstream()),
        result_size_limit(result_size_limit) {}

  void execute(const std::string &queryStr);

  std::streambuf *get_buf() const { return buf->rdbuf(); }

  ~QueryManager() { delete buf; }
};

template <class F, class... Args>
auto ThreadPool::enqueue_subtask(F &&f, Args &&...args)
    -> std::future<decltype(f(args...))> {
  using return_type = decltype(f(args...));

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      [args = std::make_tuple(std::forward<Args>(args)...),
       f = std::move(f)]() { return std::apply(f, args); });

  {
    const std::lock_guard<std::mutex> lock(queue_mutex);
    if (stop) {
      throw std::runtime_error("ThreadPool stopped");
    }
    subtasks.emplace([task]() { (*task)(); });
  }

  condition.notify_one();
  return task->get_future();
}

#endif // PROJECT_QUERYMANAGER_H
