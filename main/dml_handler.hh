#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/rewrite_util.hh>

#include <sql_lex.h>

// Abstract base class for query handler.
class DMLHandler {
public:
    static const DMLHandler *dispatch(enum_sql_command sql_cmd);
    static void buildAll();
    static LEX** rewriteLex(LEX *lex, Analysis &analysis,
                            const string &q, unsigned *out_lex_count);

private:
    virtual void gather(LEX *lex, Analysis &a) const = 0;
    virtual LEX **rewrite(LEX *lex, Analysis &a,
                          unsigned *out_lex_count) const = 0;
     
protected:
    DMLHandler() {;}
    ~DMLHandler() {;}

    // AWARE: Stateful.
    static std::map<enum_sql_command, DMLHandler *> handlers;
};

