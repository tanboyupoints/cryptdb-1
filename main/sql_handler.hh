#pragma once

#include <main/Analysis.hh>

#include <sql_lex.h>

#include <map>

// only call yield one time per block
#define crStartBlock                                                        \
    if (0 == state_) {

#define crEndBlock                                                          \
        assert(false);                                                      \
    }                                                                       \
    assert(state_ > 0);                                                     \
    --state_; ++upstate_;

#define crYield(value)                                                      \
{                                                                           \
    assert(false == finished_);                                             \
    assert(0 == state_);                                                    \
    state_   = ++upstate_;                                                  \
    upstate_ = 0;                                                           \
    {                                                                       \
        auto temp = (value);                                                \
        return std::make_pair(                                              \
            AbstractQueryExecutor::ResultType::QUERY_COME_AGAIN,            \
            new Anything<decltype(temp)>(temp));                            \
    }                                                                       \
    assert(false);                                                          \
}

#define crFinishWithQuery(value)                                            \
{                                                                           \
    assert(false == finished_);                                             \
    assert(0 == state_);                                                    \
    finished_ = true;                                                       \
    {                                                                       \
        auto temp = (value);                                                \
        return std::make_pair(                                              \
            AbstractQueryExecutor::ResultType::QUERY_USE_RESULTS,           \
            new Anything<decltype(temp)>(temp));                            \
    }                                                                       \
    assert(false);                                                          \
}

#define crFinish(value)                                                     \
{                                                                           \
    assert(false == finished_);                                             \
    assert(0 == state_);                                                    \
    finished_ = true;                                                       \
    {                                                                       \
        auto temp = (value);                                                \
        return std::make_pair(                                              \
            AbstractQueryExecutor::ResultType::RESULTS,                     \
            new Anything<decltype(temp)>(temp));                            \
    }                                                                       \
    assert(false);                                                          \
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
    unsigned state_, upstate_;
    bool finished_;

public:
    enum class ResultType {RESULTS, QUERY_COME_AGAIN, QUERY_USE_RESULTS};

    AbstractQueryExecutor()
        : state_(0), upstate_(0), finished_(false) {}
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

