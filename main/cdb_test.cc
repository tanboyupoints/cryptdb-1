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

#include "main/cdb_rewrite.hh"
#include "parser/embedmysql.hh"
#include "parser/stringify.hh"
#include "crypto/ecjoin.hh"
#include "util/errstream.hh"
#include "util/cryptdb_log.hh"

// if true, uses MultiPrinc mode
static bool Multi = false;

static bool encByDefault = true;


static inline string user_homedir() {
    return getenv("HOME");
}

static inline string user_histfile() {
    return user_homedir() + "/.cryptdb-history";
}

static void __write_history() {
    write_history(user_histfile().c_str());
}

static inline string &ltrim(string &s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  return s;
}

static inline string &rtrim(string &s) {
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

static inline string &trim(string &s) {
  return ltrim(rtrim(s));
}

/** returns true if should stop, to keep looping */
static bool handle_line(Connect& conn, Rewriter& r, const string& q)
{
  if (q == "\\q") {
    cerr << "Goodbye!\n";
    return false;
  }

  add_history(q.c_str());

  // handle meta inputs
  if (q.find(":load") == 0) {
    string filename = q.substr(6);
    trim(filename);
    cerr << RED_BEGIN << "Loading commands from file: " << filename << COLOR_END << endl;
    ifstream f(filename.c_str());
    if (!f.is_open()) {
      cerr << "ERROR: cannot open file: " << filename << endl;
    }
    while (f.good()) {
      string line;
      getline(f, line);
      if (line.empty())
        continue;
      cerr << GREEN_BEGIN << line << COLOR_END << endl;
      if (!handle_line(conn, r, line)) {
        f.close();
        return false;
      }
    }
    f.close();
    return true;
  }


  return (bool)executeQuery(conn, r, q, true);
}

int
main(int ac, char **av)
{
    if (ac != 2) {
        cerr << "Usage: " << av[0] << " embed-db " << endl;
        exit(1);
    }

    using_history();
    read_history(user_histfile().c_str());
    atexit(__write_history);


    ConnectionInfo ci("localhost", "root", "letmein");

    Rewriter r(ci, av[1], Multi, encByDefault);
    //TODO: conn creation has to occur after rewriter creation
    //because rewriter inits mysql library; fix this
    Connect conn("localhost", "root", "letmein", "cryptdbtest");

    r.setMasterKey("2392834");

    string prompt = BOLD_BEGIN + "CryptDB=#" + COLOR_END + " ";


    for (;;) {
        char *input = readline(prompt.c_str());
        if (!input) break;
        string q(input);
        if (q.empty()) continue;
        else{
            if (!handle_line(conn, r, q))
                break;
        }
    }
}
