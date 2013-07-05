#pragma once

#include <map>

#include <main/Analysis.hh>

#include <sql_lex.h>

class AlterSubHandler {
public:
    static const AlterSubHandler *dispatch(long flags);
    static void buildAll();
    static void destroyAll();
    static LEX **transformLex(LEX *lex, Analysis &a, const string &q,
                              unsigned *out_lex_count);

private:
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const = 0;
    static long calculateMask();

protected:
    AlterSubHandler() {;}
    virtual ~AlterSubHandler() {;}

    // AWARE: Stateful.
    static std::map<long, AlterSubHandler *> handlers;

    typedef LEX **(*subHandler)(LEX *lex, Analysis &a, const string &q,   
                                unsigned *out_lex_count);
};
