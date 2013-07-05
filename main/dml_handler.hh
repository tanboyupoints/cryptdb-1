#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/dispatcher.hh>

#include <sql_lex.h>

// Abstract base class for query handler.
class DMLHandler : public SQLHandler {
public:
    virtual LEX** transformLex(LEX *lex, Analysis &analysis,
                               const string &q, unsigned *out_lex_count) const;

private:
    virtual void gather(LEX *lex, Analysis &a) const = 0;
    virtual LEX **rewrite(LEX *lex, Analysis &a,
                          unsigned *out_lex_count) const = 0;
     
protected:
    DMLHandler() {;}
    virtual ~DMLHandler() {;}
};

SQLDispatcher *buildDMLDispatcher();

