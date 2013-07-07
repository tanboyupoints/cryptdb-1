#include <main/dispatcher.hh>

bool Dispatcher::addHandler(long long cmd, SQLHandler *h) {
    auto it = handlers.find(cmd);
    if (handlers.end() != it) {
        return false;
    }

    handlers[cmd] = h;
    return true;
}

bool Dispatcher::canDo(LEX *lex) const {
    return handlers.end() != handlers.find(extract(lex));
}

Dispatcher::~Dispatcher()
{
    auto cp = handlers;
    handlers.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

const SQLHandler *Dispatcher::dispatch(LEX *lex) const
{
    auto it = handlers.find(extract(lex));
    if (handlers.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

long long SQLDispatcher::extract(LEX *lex) const
{
    return lex->sql_command;
}

long long AlterDispatcher::extract(LEX *lex) const
{
    static long mask = calculateMask();
    return lex->alter_info.flags & mask;
}

long AlterDispatcher::calculateMask() const {
    long mask = 0;
    for (auto it : handlers) {
        mask |= it.first;
    }

    return mask;
}

