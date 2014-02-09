#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/dispatcher.hh>

#include <sql_lex.h>

class DDLQueryExecutor : public AbstractQueryExecutor {
public:
    DDLQueryExecutor(const LEX &original_lex,
                     const LEX &new_lex,
                     std::vector<std::unique_ptr<Delta> > &&deltas)
        : original_query(lexToQuery(original_lex)),
          new_query(lexToQuery(new_lex)), deltas(std::move(deltas)) {}
    ~DDLQueryExecutor() {}
    std::pair<ResultType, AbstractAnything *>
        next(const ResType &res, const NextParams &nparams);

private:
    const std::string original_query;
    const std::string new_query;
    const std::vector<std::unique_ptr<Delta> > deltas;

    AssignOnce<ResType> ddl_res;
    AssignOnce<uint64_t> embedded_completion_id;
};

// Abstract base class for command handler.
class DDLHandler : public SQLHandler {
public:
    virtual AbstractQueryExecutor *transformLex(Analysis &analysis, LEX *lex)
        const;

private:
    virtual AbstractQueryExecutor *
        rewriteAndUpdate(Analysis &a, LEX *lex, const Preamble &pre) const = 0;

protected:
    DDLHandler() {;}
    virtual ~DDLHandler() {;}
};

SQLDispatcher *buildDDLDispatcher();

