#pragma once

/*
 * cdb_rewrite.hh
 *
 *
 *  TODO: need to integrate it with util.h: some declarations are repeated
 */

#include <main/Translator.hh>
#include <main/Connect.hh>
#include <main/MultiPrinc.hh>

#include <sql_select.h>
#include <sql_delete.h>
#include <sql_insert.h>
#include <sql_update.h>

#include <main/Analysis.hh>
#include <parser/Annotation.hh>
#include <parser/stringify.hh>

#include <util/errstream.hh>
#include <util/cleanup.hh>
#include <util/rob.hh>


using namespace std;


class FieldReturned {
public:
    bool encrypted;
    bool includeInResult;
    std::string key;
    unsigned int SaltIndex;
    std::string nameForReturn;
};

void
printRes(const ResType & r);

//contains the results of a query rewrite:
// - rewritten queries
// - data structure needed to decrypt results
class QueryRewrite {
public:
    QueryRewrite() : wasRew(true) {}
    bool wasRew; //if query was rewritten
    std::list<std::string> queries;
    ReturnMeta * rmeta;
};

// Main class processing rewriting
class Rewriter {
public:
    Rewriter(ConnectionInfo ci,
             const std::string &embed_dir,
             bool MultiPrinc = false,
             bool encByDefault = true);
    ~Rewriter();

    void setMasterKey(const std::string &mkey);
    QueryRewrite rewrite(const std::string &q, std::string *cur_db);
    ResType decryptResults(ResType & dbres, ReturnMeta * rm);

private:
    ProxyState ps;
};

class ScopedMySQLRes {
public:
    ScopedMySQLRes(MYSQL_RES *r) : r(r) {
        if (!r) fatal() << "input result set was NULL";
    }
    ~ScopedMySQLRes() {
        assert(r);
        mysql_free_result(r);
    }
    MYSQL_RES *res() { return r; }
private:
    MYSQL_RES *r;
};

ResType *
executeQuery(Connect &conn, Rewriter &r, const string &q, bool show=false);

const string BOLD_BEGIN = "\033[1m";
const string RED_BEGIN = "\033[1;31m";
const string GREEN_BEGIN = "\033[1;92m";
const string COLOR_END = "\033[0m";

