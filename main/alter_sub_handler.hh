#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/dispatcher.hh>

#include <sql_lex.h>

class AlterSubHandler : public SQLHandler {
public:
    virtual LEX **transformLex(LEX *lex, Analysis &a, const string &q,
                               unsigned *out_lex_count) const;

private:
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const = 0;

protected:
    AlterSubHandler() {;}
    virtual ~AlterSubHandler() {;}
};

AlterDispatcher *buildAlterSubDispatcher();
