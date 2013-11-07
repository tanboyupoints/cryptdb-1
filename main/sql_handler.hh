#pragma once

#include <main/Analysis.hh>

#include <sql_lex.h>

#include <map>

class SQLHandler {
public:
    SQLHandler() {;}
    virtual LEX *transformLex(Analysis &a, LEX *lex, const ProxyState &ps)
        const = 0;
    virtual ~SQLHandler() {;}
};

struct Preamble {
    Preamble(const std::string &dbname, const std::string &table)
        : dbname(dbname), table(table) {}
    const std::string dbname;
    const std::string table;
};

