#pragma once

#include <main/Analysis.hh>
#include <util/yield.hpp>

#include <sql_lex.h>

#include <map>

#define CR_QUERY_AGAIN(value)                                               \
    std::make_pair(AbstractQueryExecutor::ResultType::QUERY_COME_AGAIN,     \
                   newAnything(value))

#define CR_QUERY_RESULTS(value)                                             \
    std::make_pair(AbstractQueryExecutor::ResultType::QUERY_USE_RESULTS,    \
                   newAnything(value))

#define CR_RESULTS(value)                                                   \
    std::make_pair(AbstractQueryExecutor::ResultType::RESULTS,              \
                   newAnything(value))

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

template <typename Type>
Anything<Type> *
newAnything(const Type &t)
{
    return new Anything<Type>(t);
}

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
    coroutine corot;

public:
    enum class ResultType {RESULTS, QUERY_COME_AGAIN, QUERY_USE_RESULTS};

    AbstractQueryExecutor() {}
    virtual ~AbstractQueryExecutor();
    virtual std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, NextParams &nparams) = 0;

    static void genericPreamble(bool staleness, NextParams &nparams);
};

class SimpleExecutor : public AbstractQueryExecutor {
    const std::string query;

public:
    SimpleExecutor(const std::string &query)
        : query(query) {}
    ~SimpleExecutor() {}

    std::pair<ResultType, AbstractAnything *>
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

#include <util/unyield.hpp>
