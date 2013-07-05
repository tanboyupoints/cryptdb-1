#include <main/alter_sub_handler.hh>

std::map<long, AlterSubHandler *> AlterSubHandler::handlers;

class AddColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class DropColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class ChangeColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class ForeignKeySubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class AddIndexSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class DropIndexSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};


void AlterSubHandler::buildAll() {
    AlterSubHandler *h;
    
    h = new AddColumnSubHandler();
    handlers[ALTER_ADD_COLUMN] = h;

    h = new DropColumnSubHandler();
    handlers[ALTER_DROP_COLUMN] = h;

    h = new ChangeColumnSubHandler();
    handlers[ALTER_CHANGE_COLUMN] = h;

    h = new ForeignKeySubHandler();
    handlers[ALTER_FOREIGN_KEY] = h;

    h = new AddIndexSubHandler();
    handlers[ALTER_ADD_INDEX] = h;

    h = new DropIndexSubHandler();
    handlers[ALTER_DROP_INDEX] = h;
}

void AlterSubHandler::destroyAll() {
    auto cp = handlers;
    handlers.clear();

    for (auto it : cp) {
        delete it.second; 
    } 
}

long AlterSubHandler::calculateMask() {
    long mask = 0;
    for (auto it : handlers) {
        mask |= it.first;
    }

    return mask;
}

const AlterSubHandler *AlterSubHandler::dispatch(long flags) {
    // AWARE: You can't add handlers later on in the programs lifecycle.
    static long mask = calculateMask();

    auto it = handlers.find(flags & mask);
    if (handlers.end() == it) {
        return NULL;
    }

    return it->second;
}

LEX **AlterSubHandler::transformLex(LEX *lex, Analysis &a, const string &q,
                                    unsigned *out_lex_count)
{
    const AlterSubHandler *handler =
        AlterSubHandler::dispatch(lex->alter_info.flags);
    if (!handler) {
        return NULL;
    }

    return handler->rewriteAndUpdate(lex, a, q, out_lex_count);
}

