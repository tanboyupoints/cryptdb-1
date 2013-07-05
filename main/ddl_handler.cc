#include <main/ddl_handler.hh>

std::map<enum_sql_command, DDLHandler *> DDLHandler::handlers;

class AlterHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class CreateHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class DropHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class ChangeDBHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

const DDLHandler *DDLHandler::dispatch(enum_sql_command sql_cmd)
{
    std::map<enum_sql_command, DDLHandler *>::iterator it =
        handlers.find(sql_cmd);
    if (handlers.end() == it) {
        return NULL;
    }

    return it->second;
}

// AWARE: If you want your new handler to be accessible, you must add it
// to this function.
void DDLHandler::buildAll()
{
    DDLHandler *h;

    h = new AlterHandler();
    handlers[SQLCOM_ALTER_TABLE] = h;

    h = new CreateHandler();
    handlers[SQLCOM_CREATE_TABLE] = h;
    
    h = new DropHandler();
    handlers[SQLCOM_DROP_TABLE] = h;

    h = new ChangeDBHandler();
    handlers[SQLCOM_CHANGE_DB] = h;
}

void DDLHandler::destroyAll()
{
    for (auto it : handlers) {
        delete it.second;
    }

    handlers.clear();
}

