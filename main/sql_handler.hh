#pragma once

#include <main/Analysis.hh>

#include <sql_lex.h>

#include <map>

// only call yield one time per block
#define crStartBlock                                \
    if (0 == state) {

#define crEndBlock                                  \
        assert(false);                              \
    }                                               \
    assert(state > 0);                              \
    --state; ++upstate;

#define crYield(value)                                          \
{                                                               \
    state   = ++upstate;                                        \
    upstate = 0;                                                \
    {                                                           \
        auto temp = (value);                                    \
        return std::make_pair(true,                             \
                    new Anything<decltype(temp)>(temp));        \
    }                                                           \
}

#define crFinish(value)                                             \
{                                                                   \
    assert(0 == state);                                             \
    state = upstate;                                                \
    {                                                               \
        auto temp = (value);                                        \
        return std::make_pair(false,                                \
                    new Anything<decltype(temp)>(temp));            \
    }                                                               \
    assert(false);                                                  \
}

template <typename Type>
class Anything;

class AbstractAnything {
public:
    virtual ~AbstractAnything() = 0;

    template <typename Type>
    Type
    extract() const
    {
        return static_cast<const Anything<Type> *>(this)->get();
    }
};

template <typename Type>
class Anything : public AbstractAnything {
    const Type value;

public:
    Anything(Type value) : value(value) {}
    Type get() const {return value;}
};

struct NextParams {
    SchemaCache *const schema_cache;
    const std::string &default_db;
    const std::unique_ptr<Connect> &e_conn;
    const std::unique_ptr<Connect> &conn;

    NextParams(ProxyState &ps, const std::string &default_db)
        : schema_cache(&ps.getSchemaCache()), default_db(default_db),
          e_conn(ps.getEConn()), conn(ps.getConn())
    {
        assert(this->schema_cache && this->e_conn && this->conn);
    }
};

class AbstractQueryExecutor {
protected:
    unsigned state, upstate;

public:
    AbstractQueryExecutor()
        : state(0), upstate(0) {}
    virtual ~AbstractQueryExecutor();
    virtual std::pair<bool, AbstractAnything *>
        next(const ResType &res, NextParams &nparams) = 0;

    static void genericPreamble(bool staleness, NextParams &nparams);
};

class SimpleExecutor : public AbstractQueryExecutor {
    const std::string query;

public:
    SimpleExecutor(const std::string &query)
        : query(query) {}
    ~SimpleExecutor() {}

    std::pair<bool, AbstractAnything *>
        next(const ResType &res, NextParams &nparams);
};

inline SimpleExecutor *
noopExecutor()
{
    return new SimpleExecutor("do 0;");
}

class SQLHandler {
public:
    SQLHandler() {;}
    virtual AbstractQueryExecutor *
        transformLex(Analysis &a, LEX *lex, const ProxyState &ps)
        const = 0;
    virtual ~SQLHandler() {;}
};

struct Preamble {
    Preamble(const std::string &dbname, const std::string &table)
        : dbname(dbname), table(table) {}
    const std::string dbname;
    const std::string table;
};

