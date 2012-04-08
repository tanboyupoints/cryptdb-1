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

#include <edb/Connect.hh>

#include <unistd.h>

#include <parser/cdb_rewrite.hh>

#include <readline/readline.h>
#include <readline/history.h>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>

#include <crypto/ecjoin.hh>

#include <util/errstream.hh>
#include <util/cryptdb_log.hh>

using namespace std;

// if true, uses MultiPrinc mode
static bool Multi = true;

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
    

    ConnectionInfo ci("localhost", "root", "letmein", "cryptdbtest");

    Rewriter r(ci, av[1], Multi, encByDefault);
    //TODO: conn creation has to occur after rewriter creation
    //because rewriter inits mysql library; fix this
    Connect conn("localhost", "root", "letmein", "cryptdbtest");

    r.setMasterKey("2392834");

    DBResult * dbres;

    for (;;) {
        char *input = readline("CryptDB=# ");

        if (!input) break;

        string q(input);
        if (q.empty()) continue;

        if (q == "\\q") {
            cerr << "Goodbye!\n";
            break;
        }
        add_history(input);
        string new_q;
	Analysis analysis;
        try {

            list<string> new_queries = r.rewrite(q, analysis);
            //only last query should return anything
            for (auto new_q = new_queries.begin(); new_q != new_queries.end(); new_q++) {
                cerr << "SUCCESS: " << *new_q << endl;
                assert(conn.execute(*new_q, dbres));
            }
            if (!dbres) {
                continue;
            }

            ResType res = dbres->unpack();

	    if (!res.ok) {
                cerr << "issue with query \n";
                continue;
            }
            ResType dec_res = r.decryptResults(res, analysis);
            cerr << "decrypted results are: \n"; printRes(dec_res);

        } catch (std::runtime_error &e) {
            cout << "Unexpected Error: " << e.what() << " in query " << q << endl;
        } catch (CryptDBError &e) {
            cout << "Interal Error: " << e.msg << " in query " << q << endl;
        }
    }
}
