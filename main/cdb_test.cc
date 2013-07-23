#include <cstdlib>
#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "main/Connect.hh"

#include <readline/readline.h>
#include <readline/history.h>

#include <main/rewrite_main.hh>
#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <crypto/ecjoin.hh>
#include <util/errstream.hh>
#include <util/cryptdb_log.hh>


static bool encByDefault = true;


static inline std::string user_homedir() {
    return getenv("HOME");
}

static inline std::string user_histfile() {
    return user_homedir() + "/.cryptdb-history";
}

static void __write_history() {
    write_history(user_histfile().c_str());
}

static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(std::ptr_fun<int, int>(isspace))));
  return s;
}

static inline std::string &rtrim(std::string &s) {
  s.erase(find_if(s.rbegin(), s.rend(), not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

/** returns true if should stop, to keep looping */
static bool handle_line(Rewriter& r, const std::string& q)
{
  if (q == "\\q") {
    std::cerr << "Goodbye!\n";
    return false;
  }

  add_history(q.c_str());

  // handle meta inputs
  if (q.find(":load") == 0) {
    std::string filename = q.substr(6);
    trim(filename);
    std::cerr << RED_BEGIN << "Loading commands from file: " << filename << COLOR_END << std::endl;
    std::ifstream f(filename.c_str());
    if (!f.is_open()) {
      std::cerr << "ERROR: cannot open file: " << filename << std::endl;
    }
    while (f.good()) {
      std::string line;
      getline(f, line);
      if (line.empty())
        continue;
      std::cerr << GREEN_BEGIN << line << COLOR_END << std::endl;
      if (!handle_line(r, line)) {
        f.close();
        return false;
      }
    }
    f.close();
    return true;
  }


  return (bool)executeQuery(r, q, true);
}

int
main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " embed-db " << std::endl;
        exit(1);
    }

    using_history();
    read_history(user_histfile().c_str());
    atexit(__write_history);

    ConnectionInfo ci("localhost", "root", "letmein");
    Rewriter r(ci, av[1], "cryptdbtest", encByDefault);
    r.setMasterKey("2392834");

    std::string prompt = BOLD_BEGIN + "CryptDB=#" + COLOR_END + " ";

    for (;;) {
        char *input = readline(prompt.c_str());
        if (!input) break;
        std::string q(input);
        if (q.empty()) continue;
        else{
            if (!handle_line(r, q))
                break;
        }
    }
}
