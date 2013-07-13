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
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        const AlterSubHandler *handler;
        assert(sub_dispatcher->canDo(lex));
        assert(handler = (const AlterSubHandler*)sub_dispatcher->dispatch(lex));
        return handler->transformLex(lex, a, q, out_lex_count);
    }
    
    AlterDispatcher *sub_dispatcher;
    
public:
    AlterHandler() {
        sub_dispatcher = buildAlterSubDispatcher();
    }
};

class CreateHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const
    {
        char* dbname = lex->select_lex.table_list.first->db;
        char* table  = lex->select_lex.table_list.first->table_name;
        LEX *new_lex = copy(lex);

       
        //TODO: support for "create table like"
        if (lex->create_info.options & HA_LEX_CREATE_TABLE_LIKE) {
            cryptdb_err() << "No support for create table like yet. " <<
                       "If you see this, please implement me";
        }

        // Create the table regardless of 'IF NOT EXISTS' if the table
        // doens't exist.
        if (false == a.tableMetaExists(table)) {
            // -----------------------------
            //         Update TABLE       
            // -----------------------------
            auto empty_index_map = std::map<std::string, std::string>();
            // TODO: Use appropriate values for has_sensitive and has_salt.
            TableMeta *tm = new TableMeta(true, true, empty_index_map);
            a.ps->schema->addTableMeta(std::string(table), tm);

           
            // Add table to embedded database.
            assert(a.ps->e_conn->execute(q));

            a.ps->e_conn->execute("START TRANSACTION");

            {
                ostringstream s;
                s << " INSERT INTO pdb.table_info VALUES ("
                  << " '" << table << "', "
                  << " '" << tm->getAnonTableName() << "', "
                  << " " << bool_to_string(tm->hasSensitive) << ", "
                  << " " << bool_to_string(tm->has_salt) << ", "
                  << " '" << tm->salt_name << "', "
                  << " '" << dbname << "',"
                  << " 0"
                  << " );";

                assert(a.ps->e_conn->execute(s.str()));
            }
            
            // -----------------------------
            //         Rewrite TABLE       
            // -----------------------------
            new_lex->select_lex.table_list =
                rewrite_table_list(lex->select_lex.table_list, a);

            auto it =
                List_iterator<Create_field>(lex->alter_info.create_list);
            new_lex->alter_info.create_list = 
                reduceList<Create_field>(it, List<Create_field>(),
                    [tm, a, dbname, table] (List<Create_field> out_list,
                                            Create_field *cf) {
                        // -----------------------------
                        //         Update FIELD       
                        // -----------------------------
                        FieldMeta *fm =
                            new FieldMeta(string(cf->field_name), cf,
                                          a.ps->masterKey);
                        assert(tm->addFieldMeta(fm));
                        assert(do_add_field(fm, a, dbname, table));
                        // -----------------------------
                        //         Rewrite FIELD       
                        // -----------------------------
                        auto new_fields = rewrite_create_field(fm, cf, a);
                        out_list.concat(vectorToList(new_fields));

                        return out_list;
                });
            
            a.ps->e_conn->execute("COMMIT");
        } else {
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

        return single_lex_output(new_lex, out_lex_count);
    }
};

class DropHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        LEX **out_lex = rewrite(lex, a, out_lex_count);
        update(q, lex, a);

        return out_lex;
    }
    
    LEX **rewrite(LEX *lex, Analysis &a, unsigned *out_lex_count) const {
        LEX * new_lex = copy(lex);
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a, true);

        return single_lex_output(new_lex, out_lex_count);
    }

    void update(const string &q, LEX *lex, Analysis &a) const {
        assert(a.ps->e_conn->execute("START TRANSACTION;"));

        TABLE_LIST *tbl = lex->select_lex.table_list.first;
        for (; tbl; tbl = tbl->next_local) {
            char* dbname = tbl->db;
            char* table  = tbl->table_name;

            if (lex->drop_if_exists) {
                if (false == a.tableMetaExists(table)) {
                    continue;
                }
            }

            ostringstream s;

            s << " DELETE FROM pdb.table_info, pdb.field_info, "
              << "             pdb.onion_info, pdb.layer_key"
              << " USING pdb.table_info INNER JOIN pdb.field_info"
              << "       INNER JOIN pdb.onion_info INNER JOIN pdb.layer_key"
              << " WHERE  pdb.table_info.name = '" << table << "' "
              << " AND    pdb.table_info.database_name = '" << dbname << "' "
              << " AND    pdb.table_info.id = pdb.field_info.table_info_id"
              << " AND    pdb.field_info.id = pdb.onion_info.field_info_id"
              << " AND    pdb.onion_info.id = pdb.layer_key.onion_info_id;";

            assert(a.ps->e_conn->execute(s.str()));

            // Remove from *Meta structures.
            assert(a.destroyTableMeta(table));
        }
        
        // Remove table(s) from embedded database.
        assert(a.ps->e_conn->execute(q));

        assert(a.ps->e_conn->execute("COMMIT"));
    }
};

// TODO: FIXME.
class ChangeDBHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(lex->select_lex.db);
        char* dbname = lex->select_lex.db;

        // new dbname is saved for next queries
        a.ps->conn->setCurDBName(dbname);
        a.ps->e_conn->setCurDBName(dbname);

        return single_lex_output(lex, out_lex_count);
    }
};

LEX** DDLHandler::transformLex(LEX *lex, Analysis &analysis,
                                      const string &q, unsigned *out_lex_count) const {
    return this->rewriteAndUpdate(lex, analysis, q, out_lex_count);
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
