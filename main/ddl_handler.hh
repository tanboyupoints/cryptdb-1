#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/dispatcher.hh>

#include <sql_lex.h>

// Abstract base class for command handler.
class DDLHandler : public SQLHandler {
public:
    virtual LEX **transformLex(LEX *lex, Analysis &analysis,
                               const std::string &q,
                               unsigned *out_lex_count) const;

private:
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a,
                                   const std::string &q,
                                   unsigned *out_lex_count) const = 0;

protected:
    DDLHandler() {;}
    virtual ~DDLHandler() {;}
};

SQLDispatcher *buildDDLDispatcher();

