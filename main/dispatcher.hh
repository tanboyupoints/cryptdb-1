#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>

#include <sql_lex.h>

class Dispatcher {
public:
    LEX **call(LEX *lex, Analysis &a, const string &q,
               unsigned *out_lex_count) const;
    bool addHandler(long long cmd, SQLHandler *h);
    bool canDo(long long cmd);


    std::map<long long, SQLHandler *> handlers; 

protected:
    virtual ~Dispatcher();

private:
    virtual const SQLHandler *dispatch(LEX *lex) const = 0;
};

class SQLDispatcher : public Dispatcher {
    virtual const SQLHandler *dispatch(LEX *lex) const;
public:
    virtual ~SQLDispatcher() {;}
};

class AlterDispatcher : public Dispatcher {
    virtual const SQLHandler *dispatch(LEX *lex) const;
    long calculateMask() const;
public:
    virtual ~AlterDispatcher() {;}
};

