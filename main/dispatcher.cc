#include <main/dispatcher.hh>

long long SQLDispatcher::extract(LEX* lex) const
{
    return lex->sql_command;
}

long long AlterDispatcher::extract(LEX* lex) const
{
    static long mask = calculateMask();
    return lex->alter_info.flags & mask;
}

long AlterDispatcher::calculateMask() const {
    long mask = 0;
    for (auto it = handlers.begin(); it != handlers.end(); it++) {
        mask |= (*it).first;
    }

    return mask;
}

