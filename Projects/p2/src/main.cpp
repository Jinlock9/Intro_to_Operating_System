//
// Created by liu on 18-10-21.
//

#include <getopt.h>

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "query/QueryBuilders.h"
#include "query/QueryManager.h"
#include "query/QueryParser.h"

struct {
  std::string listen;
  int64_t threads = 0;
} parsedArgs;

void parseArgs(int argc, char *argv[]) {
  const option longOpts[] = {{"listen", required_argument, nullptr, 'l'},
                             {"threads", required_argument, nullptr, 't'},
                             {nullptr, no_argument, nullptr, 0}};
  const char *shortOpts = "l:t:";
  int opt = 0;
  int longIndex = 0;
  while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &longIndex)) !=
         -1) {
    if (opt == 'l') {
      parsedArgs.listen = optarg;
    } else if (opt == 't') {
      parsedArgs.threads = std::strtol(optarg, nullptr, 10);
    } else {
      std::cerr << "lemondb: warning: unknown argument "
                << longOpts[longIndex].name << std::endl;
    }
  }
}

std::string extractQueryString(std::istream &is) {
  std::string buf;
  do {
    int const ch = is.get();
    if (ch == ';')
      return buf;
    if (ch == EOF)
      throw std::ios_base::failure("End of input");
    buf.push_back((char)ch);
  } while (true);
}

int main(int argc, char *argv[]) {
  // Assume only C++ style I/O is used in lemondb
  // Do not use printf/fprintf in <cstdio> with this line
  std::ios_base::sync_with_stdio(false);

  parseArgs(argc, argv);

  std::ifstream fin;
  if (!parsedArgs.listen.empty()) {
    fin.open(parsedArgs.listen);
    if (!fin.is_open()) {
      std::cerr << "lemondb: error: " << parsedArgs.listen
                << ": no such file or directory" << std::endl;
      exit(-1);
    }
  }
  std::istream is(fin.rdbuf());

#ifdef NDEBUG
  // In production mode, listen argument must be defined
  if (parsedArgs.listen.empty()) {
    std::cerr << "lemondb: error: --listen argument not found, not allowed in "
                 "production mode"
              << std::endl;
    exit(-1);
  }
#else
  // In debug mode, use stdin as input if no listen file is found
  if (parsedArgs.listen.empty()) {
    std::cerr << "lemondb: warning: --listen argument not found, use stdin "
                 "instead in debug mode"
              << std::endl;
    is.rdbuf(std::cin.rdbuf());
  }
#endif

  if (parsedArgs.threads < 0) {
    std::cerr << "lemondb: error: threads num can not be negative value "
              << parsedArgs.threads << std::endl;
    exit(-1);
  } else if (parsedArgs.threads == 0) {
    // @TODO Auto detect the thread num
    std::cerr << "lemondb: info: auto detect thread num" << std::endl;
    parsedArgs.threads = std::thread::hardware_concurrency();
  } else {
    std::cerr << "lemondb: info: running in " << parsedArgs.threads
              << " threads" << std::endl;
  }
  parsedArgs.threads = std::max(parsedArgs.threads, int64_t(8));

  QueryParser p;

  p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
  p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
  p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());

  // manual over-subscription, since there are some blocking operations
  ThreadPool::init(static_cast<size_t>(parsedArgs.threads));
  QueryManager qm(static_cast<size_t>(parsedArgs.threads) - 1, &is, &p);
  do {
    while (is) {
      try {
        // A very standard REPL
        // REPL: Read-Evaluate-Print-Loop
        std::string const queryStr = extractQueryString(is);
        qm.execute(queryStr);
      } catch (const std::ios_base::failure &e) {
        // End of input
        break;
      } catch (const std::exception &e) {
        std::cout.flush();
        std::cerr << e.what() << std::endl;
      }
    }
    std::streambuf *buffer = qm.get_buf();
    if (buffer) {
      is.clear();
      is.rdbuf(buffer);
    } else {
      break;
    }
  } while (is);
  return 0;
}
