#include <main/ddl_handler.hh>

#include <parser/lex_util.hh>
#include <main/rewrite_util.hh>
#include <main/List_helpers.hh>
#include <main/rewrite_main.hh>
#include <main/alter_sub_handler.hh>
#include <main/dispatcher.hh>

// > TODO: mysql permits a single ALTER TABLE command to invoke _multiple_
//   and _different_ subcommands.
//   ie, ALTER TABLE t ADD COLUMN x integer, ADD INDEX i (z);
//   Currently we do not support mixed operations.
//   > Must guarentee that rewrite_table_list is only called one time.
//   > If we drop Keys and Columns in the same query the order is probably
//     going to get changed.
class AlterHandler : public DDLHandler {
    virtual LEX *rewriteAndUpdate(Analysis &a, LEX *lex,
                                  const ProxyState &ps) const
    {
        const AlterSubHandler *handler;
        assert(sub_dispatcher->canDo(lex));
        assert(handler =
            (const AlterSubHandler*)sub_dispatcher->dispatch(lex));
        return handler->transformLex(a, lex, ps);
    }
    
    AlterDispatcher *sub_dispatcher;
    
public:
    AlterHandler()
    {
        sub_dispatcher = buildAlterSubDispatcher();
    }
};

class CreateHandler : public DDLHandler {
    virtual LEX *rewriteAndUpdate(Analysis &a, LEX *lex,
                                  const ProxyState &ps) const
    {
        char* table  = lex->select_lex.table_list.first->table_name;
        LEX *new_lex = copy(lex);
        
        //TODO: support for "create table like"
        if (lex->create_info.options & HA_LEX_CREATE_TABLE_LIKE) {
            cryptdb_err() << "No support for create table like yet. "
                          << "If you see this, please implement me";
        }

        // Create the table regardless of 'IF NOT EXISTS' if the table
        // doesn't exist.
        if (false == a.tableMetaExists(table)) {
            // -----------------------------
            //         Update TABLE       
            // -----------------------------
            // TODO: Use appropriate values for has_sensitive and has_salt.
            TableMeta *tm = new TableMeta(true, true);
            IdentityMetaKey *key = new IdentityMetaKey(table);
            Delta delta(Delta::CREATE, tm, a.getSchema(), key);
            a.deltas.push_back(delta);
           
            // -----------------------------
            //         Rewrite TABLE       
            // -----------------------------
            // HACK.
            // > We know that there is only one table.
            // > We also know that rewrite_table_list is going to fail to
            // find this table in 'a'.
            // > And we know that the table we want is tm with name table.
            // > This will _NOT_ gracefully handle a malformed CREATE TABLE
            // query.
            assert(1 == new_lex->select_lex.table_list.elements);
            TABLE_LIST *tbl =
                rewrite_table_list(new_lex->select_lex.table_list.first,
                                   tm->getAnonTableName());
            new_lex->select_lex.table_list = *oneElemList<TABLE_LIST>(tbl);

            auto it =
                List_iterator<Create_field>(lex->alter_info.create_list);
            new_lex->alter_info.create_list = 
                reduceList<Create_field>(it, List<Create_field>(),
                    [&a, &ps, &tm] (List<Create_field> out_list,
                                    Create_field *cf) {
                        return createAndRewriteField(a, ps, cf, tm, 
                                                     true, out_list);
                });
        } else { // Table already exists.

            // Make sure we aren't trying to create a table that
            // already exists.
            assert(lex->create_info.options & HA_LEX_CREATE_IF_NOT_EXISTS);
            // -----------------------------
            //         Rewrite TABLE       
            // -----------------------------
            new_lex->select_lex.table_list =
                rewrite_table_list(lex->select_lex.table_list, a);
            // > We do not rewrite the fields because presumably the caller
            // can do a CREATE TABLE IF NOT EXISTS for a table that already
            // exists, but with fields that do not actually exist.
            // > This would cause problems when trying to look up FieldMeta
            // for these non-existant fields.
            // > We may want to do some additional non-deterministic
            // anonymization of the fieldnames to prevent information leaks.
            // (ie, server gets compromised, server logged all sql queries,
            // attacker can see that the admin creates the account table
            // with the credit card field every time the server boots)
        }

        return new_lex;
    }
};

class DropHandler : public DDLHandler {
    virtual LEX *rewriteAndUpdate(Analysis &a, LEX *lex,
                                  const ProxyState &ps) const
    {

        LEX *final_lex = rewrite(a, lex, ps);
        update(a, lex, ps);

        return final_lex;
    }
    
    LEX *rewrite(Analysis &a, LEX *lex, const ProxyState &ps) const
    {
        LEX *new_lex = copy(lex);
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a, true);

        return new_lex;
    }

    void update(Analysis &a, LEX *lex, const ProxyState &ps) const
    {
        TABLE_LIST *tbl = lex->select_lex.table_list.first;
        for (; tbl; tbl = tbl->next_local) {
            char* table  = tbl->table_name;

            if (lex->drop_if_exists) {
                if (false == a.tableMetaExists(table)) {
                    continue;
                }
            }

            // Remove from *Meta structures.
            TableMeta *tm = a.getTableMeta(table);
            // FIXME: Key only necessary for CREATE.
            Delta delta(Delta::DELETE, tm, a.getSchema(),
                        new IdentityMetaKey(table));
            a.deltas.push_back(delta);
        }
    }
};

// TODO: Implement.
class ChangeDBHandler : public DDLHandler {
    virtual LEX *rewriteAndUpdate(Analysis &a, LEX *lex,
                                  const ProxyState &ps) const
    {

        throw CryptDBError("cryptdb does not support changing the db!");
    }
};

LEX *DDLHandler::transformLex(Analysis &a, LEX *lex,
                              const ProxyState &ps) const
{
    return this->rewriteAndUpdate(a, lex, ps);
}

// FIXME: Add test to make sure handler added successfully.
SQLDispatcher *buildDDLDispatcher()
{
    DDLHandler *h;
    SQLDispatcher *dispatcher = new SQLDispatcher();

    h = new AlterHandler();
    dispatcher->addHandler(SQLCOM_ALTER_TABLE, h);

    h = new CreateHandler();
    dispatcher->addHandler(SQLCOM_CREATE_TABLE, h);
    
    h = new DropHandler();
    dispatcher->addHandler(SQLCOM_DROP_TABLE, h);

    h = new ChangeDBHandler();
    dispatcher->addHandler(SQLCOM_CHANGE_DB, h);

    return dispatcher;
}
