#include <main/dispatcher.hh>

LEX **Dispatcher::call(LEX *lex, Analysis &a, const string &q,
                       unsigned *out_lex_count) const
{
    const SQLHandler *h = dispatch(lex); 
    if (!h) {
        return NULL;
    } else {
        return h->transformLex(lex, a, q, out_lex_count);
    }
}

bool Dispatcher::addHandler(long long cmd, SQLHandler *h) {
    auto it = handlers.find(cmd);
    if (handlers.end() == it) {
        return false;
    }

    handlers[cmd] = h;
    return true;
}

bool Dispatcher::canDo(long long cmd) {
    return handlers.end() != handlers.find(cmd);
}

Dispatcher::~Dispatcher()
{
    auto cp = handlers;
    handlers.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

const SQLHandler *SQLDispatcher::dispatch(LEX *lex) const
{
    auto it = handlers.find(lex->sql_command);
    if (handlers.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}   

const SQLHandler *AlterDispatcher::dispatch(LEX *lex) const
{
    static long mask = calculateMask();
    auto it = handlers.find(lex->alter_info.flags & mask);
    if (handlers.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

long AlterDispatcher::calculateMask() const {
    long mask = 0;
    for (auto it : handlers) {
        mask |= it.first;
    }

    return mask;
}

