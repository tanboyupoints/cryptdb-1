#pragma once

#include <main/Analysis.hh>

#include <sql_lex.h>

#include <map>

class SQLHandler {
public:
    SQLHandler() {;}
    virtual LEX **transformLex(LEX *lex, Analysis &a,const string &q,
                               unsigned *out_lex_count) const = 0;
    virtual ~SQLHandler() {;}
};

