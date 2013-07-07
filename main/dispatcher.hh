#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>

#include <sql_lex.h>

// TODO: Template LEX* and SQLHandler *.
class Dispatcher {
public:
    bool addHandler(long long cmd, SQLHandler *h);
    bool canDo(LEX *lex) const;
    const SQLHandler *dispatch(LEX *lex) const;


    std::map<long long, SQLHandler *> handlers; 

protected:
    virtual ~Dispatcher();

private:
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

