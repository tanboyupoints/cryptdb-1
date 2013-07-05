#pragma once

#include <map>

#include <main/Analysis.hh>

#include <sql_lex.h>

// Abstract base class for command handler.
class DDLHandler {
public:
    static const DDLHandler *dispatch(enum_sql_command sql_cmd);
    static void buildAll();
    static void destroyAll();
    static LEX** transformLex(LEX *lex, Analysis &analysis,
                              const string &q, unsigned *out_lex_count);

private:
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const = 0;

protected:
    DDLHandler() {;}
    virtual ~DDLHandler() {;}

    // AWARE: Stateful.
    static std::map<enum_sql_command, DDLHandler *> handlers;
};

