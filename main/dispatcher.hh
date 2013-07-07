#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>
#include <main/alter_sub_handler.hh>

#include <sql_lex.h>

template <typename Input, typename FetchMe>
class Dispatcher {
public:
    bool addHandler(long long cmd, FetchMe h) {
        auto it = handlers.find(cmd);
        if (handlers.end() != it) {
            return false;
        }

        handlers[cmd] = h;
        return true;
    }

    bool canDo(Input lex) const {
        return handlers.end() != handlers.find(extract(lex));
    }

    const FetchMe dispatch(Input lex) const {
        auto it = handlers.find(extract(lex));
        if (handlers.end() == it) {
            return NULL;
        } else {
            return it->second;
        }
    }

    std::map<long long, FetchMe> handlers; 

protected:
    virtual ~Dispatcher() {
        auto cp = handlers;
        handlers.clear();

        for (auto it : cp) {
            delete it.second;
        }
    }

private:
    virtual long long extract(Input lex) const = 0;
};

class SQLDispatcher : public Dispatcher<LEX*, SQLHandler*> {
    virtual long long extract(LEX* lex) const;
public:
    virtual ~SQLDispatcher() {;}
};

class AlterDispatcher : public Dispatcher<LEX*, AlterSubHandler*> {
    virtual long long extract(LEX* lex) const;
    long calculateMask() const;
public:
    virtual ~AlterDispatcher() {;}
};

