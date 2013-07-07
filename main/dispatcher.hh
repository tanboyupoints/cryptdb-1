#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>

#include <sql_lex.h>

// TODO: Template on Function and remove arguments to call, to generalize.
//       Also on type of handlers it supports.
// template <typename InputSelector, typename Function>
class Dispatcher {
public:
    LEX **call(LEX *lex, Analysis &a, const string &q,
               unsigned *out_lex_count) const;
    // FIXME: Generalize with an extract(LEX *) function.
    bool addHandler(long long cmd, SQLHandler *h);
    bool canDo(LEX *lex) const;


    std::map<long long, SQLHandler *> handlers; 

protected:
    virtual ~Dispatcher();

private:
    const SQLHandler *dispatch(LEX *lex) const;
    virtual long long extract(LEX *lex) const = 0;
};

class SQLDispatcher : public Dispatcher {
    virtual long long extract(LEX *lex) const;
public:
    virtual ~SQLDispatcher() {;}
};

class AlterDispatcher : public Dispatcher {
    virtual long long extract(LEX *lex) const;
    long calculateMask() const;
public:
    virtual ~AlterDispatcher() {;}
};

