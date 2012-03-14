#pragma once

/*
 * cdb_rewrite.hh
 *
 *
 *  TODO: need to integrate it with util.h: some declarations are repeated
 */

#include <edb/Translator.hh>
#include <edb/Connect.hh>
#include <edb/MultiPrinc.hh>

#include <sql_select.h>
#include <sql_delete.h>
#include <sql_insert.h>
#include <sql_update.h>

#include <parser/Analysis.hh>
#include <parser/Annotation.hh>

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


class constraints {
public:
    constraints(EncSet t_arg,
                const std::string &why_t_arg,
                Item *why_t_item_arg,
                const constraints *parent_arg)
    : encset(t_arg), soft(false),
      why_t(why_t_arg), why_t_item(why_t_item_arg),
      parent(parent_arg)
    {
        if (parent && parent->soft)
            soft = true;
    }

    constraints(EncSet t_arg,
                const std::string &why_t_arg,
                Item *why_t_item_arg,
                const constraints *parent_arg,
                bool soft_arg)
    : encset(t_arg), soft(soft_arg),
      why_t(why_t_arg), why_t_item(why_t_item_arg),
      parent(parent_arg)
    {
    }

    inline constraints
    clone_with(const EncSet &e) const
    {
        return constraints(e, why_t, why_t_item, parent, soft);
    }

    EncSet encset;
    SchemaInfo * schema;

    bool soft;      /* can be evaluated at proxy */

    std::string why_t;
    Item *why_t_item;

    const constraints *parent;
};


class Rewriter {
public:
    //Rewriter(const std::string & db);
    Rewriter(ConnectionData db,
             bool MultiPrinc = false,
             bool encByDefault = true);
    ~Rewriter();

    void setMasterKey(const std::string &mkey);
    list<std::string> rewrite(const std::string &q, Analysis & a);
    ResType decryptResults(ResType & dbres, Analysis & a);
    inline MYSQL* connect() {
        mysql_thread_init();
        return m;
    }
private:
    void initSchema();
    void createMetaTablesIfNotExists();
    list<string> processAnnotation(Annotation annot, Analysis &a);
    //initialize multi-principal data structures
    void mp_init(Analysis &a);
    
    std::string    db;
    SchemaInfo*    schema;
    CryptoManager* cm;
    unsigned int   totalTables;
    MYSQL*         m;
    Connect*       c;
    MultiPrinc*    mp;
    TMKM           tmkm;
    bool           encByDefault;
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
    inline MYSQL_RES *res() { return r; }
private:
    MYSQL_RES *r;
};
