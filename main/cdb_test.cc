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

#define QDIR "./qdir"

using namespace std;

class testBatchCommands {
    public:
       map<int, char*> files;
       DIR *dir;
       struct dirent *dentry;

       vector<string>populate(string filename);
};

vector<string>
testBatchCommands::populate(string filename)
{
    vector<string>vec;
    string tmp = QDIR;
    tmp += "/" + filename;

    //TODO: open file and fill up vec
    
    return vec;
}


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

static const string BOLD_BEGIN = "\033[1m";
static const string RED_BEGIN = "\033[1;31m";
static const string GREEN_BEGIN = "\033[1;92m";
static const string COLOR_END = "\033[0m";

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

  DBResult * dbres;
  QueryRewrite qr;

  try {
    // static initialized database name
    string curdb(conn.getCurDBName());
    if(curdb.size() == 0)
        throw runtime_error("database name not set");

    qr = r.rewrite(q, &curdb);
    //only last query should return anything
    if (qr.queries.size() == 0) {
      return true;
    }
    for (auto new_q = qr.queries.begin(); new_q != qr.queries.end(); new_q++) {
      cerr << endl
           << RED_BEGIN << "ENCRYPTED QUERY:" << COLOR_END << endl
           << *new_q << endl;
      assert(conn.execute(*new_q, dbres));
    }
    if (!dbres) {
      return true;
    }

    ResType res = dbres->unpack();

    if (!res.ok) {
      return true;
    }
    cerr << endl << RED_BEGIN << "ENCRYPTED RESULTS FROM DB:" << COLOR_END << endl;
    printRes(res);
    cerr << endl;
    ResType dec_res = r.decryptResults(res, qr.rmeta);

    cerr << endl << RED_BEGIN << "DECRYPTED RESULTS:" << COLOR_END << endl;
    printRes(dec_res);

  } catch (runtime_error &e) {
    cout << "Unexpected Error: " << e.what() << " in query " << q << endl;
  } catch (CryptDBError &e) {
    cout << "Internal Error: " << e.msg << " in query " << q << endl;
  }

  return true;
}


static string parse_args(const char *msg)
{
    string ret;
    static char cmd[64];
    memset(cmd, 0, 64);

    cerr << GREEN_BEGIN << msg << COLOR_END;
    fgets(cmd, 64, stdin);
    
    cmd[strlen(cmd)-1] = '\0';
    ret = cmd;

    return ret;
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

    testBatchCommands batch;

    batch.dir = opendir(QDIR);
    if(!batch.dir)
    {
        cout << "Warning: Could not open dir: " <<  QDIR <<endl;
    } else
    {
        while((batch.dentry = readdir(batch.dir)) != NULL)
        {
            static int i = 0;
            // Fill up files names
            string tmp = batch.dentry->d_name;
            if(tmp != "." && tmp != "..")
                //batch.files.push_back(batch.dentry->d_name);
                batch.files.insert ( std::pair<int, char*>(i++,batch.dentry->d_name) );
        }
    }

    for (;;) {
        char *input = readline(prompt.c_str());
        if (!input) break;
        string q(input);
        if (q.empty()) continue;

        /*
         * Alternative options
         */
        if(q == "!help")
        {
            cout << "!help: Show this help" << endl;
            cout << "!file: Load mysql commands from file" << endl;
            continue;
        } else if(q == "!list")
        {
            if(batch.files.size() == 0)
                continue;

            for(map<int, char*>::iterator it = batch.files.begin(); 
                    it != batch.files.end(); ++it)
                cout << "[" << it->first << "] " << it->second << endl; 

        } else if(q == "!file")
        {
            string arg = parse_args("CryptDB [Enter id number]=# ");
            if(arg.size() > 0)
            {
                map<int, char*>::iterator it = batch.files.find(atoi(arg.c_str()));
                if(it != batch.files.end())
                {
                    vector<string>vaec = batch.populate(it->second);
                    //TODO: execute line by line
                }
            }

            if(batch.files.size() == 0)
                continue;

            continue;
        } else
        {
            if (!handle_line(conn, r, q))
                break;
        }
    }
}
