#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/dispatcher.hh>

#include <sql_lex.h>

class DMLQueryExecutor : public AbstractQueryExecutor {
public:
    DMLQueryExecutor(const LEX &lex, const ReturnMeta &rmeta)
        : query(lexToQuery(lex)), rmeta(rmeta) {}
    ~DMLQueryExecutor() {}
    std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, NextParams &nparams);

private:
    const std::string query;
    const ReturnMeta rmeta;
};

class SpecialUpdateExecutor : public AbstractQueryExecutor {
public:
    SpecialUpdateExecutor(const std::string &original_query,
                          const std::string &plain_table,
                          const std::string &crypted_table,
                          const std::string &where_clause,
                          const ProxyState &ps)     // FIXME: remove with rewrite
        : original_query(original_query), plain_table(plain_table),
          crypted_table(crypted_table), where_clause(where_clause),
          ps(ps) {}
    ~SpecialUpdateExecutor() {}
    std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, NextParams &nparams);

private:
    const std::string original_query;
    const std::string plain_table;
    const std::string crypted_table;
    const std::string where_clause;
    const ProxyState &ps;               // FIXME: eliminate after rewrite

    AssignOnce<DBResult *> original_query_dbres;
    AssignOnce<std::string> escaped_output_values;
    AssignOnce<ReturnMeta> select_rmeta;
};

class ShowDirectiveExecutor : public AbstractQueryExecutor {
    const SchemaInfo &schema;

public:
    ShowDirectiveExecutor(const SchemaInfo &schema)
        : schema(schema) {}
    ~ShowDirectiveExecutor() {}

    std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, NextParams &nparams);


private:
    static bool
    deleteAllShowDirectiveEntries(const std::unique_ptr<Connect> &e_conn);

    static bool
    addShowDirectiveEntry(const std::unique_ptr<Connect> &e_conn,
                          const std::string &database,
                          const std::string &table,
                          const std::string &field,
                          const std::string &onion,
                          const std::string &level);

    static bool
    getAllShowDirectiveEntries(const std::unique_ptr<Connect> &e_conn,
                               std::unique_ptr<DBResult> *db_res);
};

class SensitiveDirectiveExecutor : public AbstractQueryExecutor {
    const std::vector<std::unique_ptr<Delta> > deltas;

public:
    SensitiveDirectiveExecutor(std::vector<std::unique_ptr<Delta> > &&deltas)
        : deltas(std::move(deltas)) {}
    ~SensitiveDirectiveExecutor() {}

    std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, NextParams &nparams);
};

// Abstract base class for query handler.
class DMLHandler : public SQLHandler {
public:
    virtual AbstractQueryExecutor *
        transformLex(Analysis &a, LEX *lex,
                     const ProxyState &ps) const;

private:
    virtual void gather(Analysis &a, LEX *lex, const ProxyState &ps)
        const = 0;
    virtual AbstractQueryExecutor *
        rewrite(Analysis &a, LEX *lex, const ProxyState &ps)
        const = 0;

protected:
    DMLHandler() {;}
    virtual ~DMLHandler() {;}
};

SQLDispatcher *buildDMLDispatcher();

