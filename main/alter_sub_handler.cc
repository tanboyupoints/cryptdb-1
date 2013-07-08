#include <main/alter_sub_handler.hh>
#include <main/List_helpers.hh>
#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <parser/lex_util.hh>
#include <main/dispatcher.hh>

// Prototypes.
static LEX **single_lex_output(LEX *out_me, unsigned *out_lex_count);

class AddColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        update(q, lex, a);
        return this->rewrite(lex, a, q, out_lex_count);
    }

    void update(const string &q, LEX *lex, Analysis &a) const {
        const string &table =
            lex->select_lex.table_list.first->table_name;
        const string &dbname = lex->select_lex.table_list.first->db;

        TableMeta *tm = a.getTableMeta(table);

        // FIXME: This does not properly support multiple column adds.
        // Create *Meta objects.
        auto add_it =
            List_iterator<Create_field>(lex->alter_info.create_list);
        eachList<Create_field>(add_it, [tm, a] (Create_field *cf) {
            create_field_meta(tm, cf, a, a.ps->encByDefault);
        });

        // Add field to embedded database.
        assert(a.ps->e_conn->execute(q));

        // Add metadata to embedded database.
        do_add_field(tm, a, dbname, table);
    }

    LEX **rewrite(LEX *lex, Analysis &a, const string &q,
                  unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        do_field_rewriting(lex, new_lex, table, a);
        return single_lex_output(new_lex, out_lex_count);
    }
};

class DropColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        LEX **out_lex = this->rewrite(lex, a, q, out_lex_count);
        update(q, lex, a);

        return out_lex;
    }

    LEX **rewrite(LEX *lex, Analysis &a, const string &q,
                  unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        auto drop_it = List_iterator<Alter_drop>(lex->alter_info.drop_list);
        new_lex->alter_info.drop_list =
            reduceList<Alter_drop>(drop_it, List<Alter_drop>(),
                [table, a] (List<Alter_drop> out_list, Alter_drop *adrop) {
                    // FIXME: Possibly this should be an assert as mixed
                    // clauses are not supported?
                    if (adrop->type == Alter_drop::COLUMN) {
                        FieldMeta *fm = a.getFieldMeta(table, adrop->name);
                        THD *thd = current_thd;

                        for (auto onion_pair : fm->onions) {
                            Alter_drop *new_adrop =
                                adrop->clone(thd->mem_root);
                            new_adrop->name =
                                thd->strdup(onion_pair.second->onionname.c_str());
                            out_list.push_back(new_adrop);
                        }

                        if (fm->has_salt) {
                            Alter_drop *new_adrop =
                                adrop->clone(thd->mem_root);
                            new_adrop->name =
                                thd->strdup(fm->salt_name.c_str());
                            out_list.push_back(new_adrop);
                        }

                    }
                    return out_list; /* lambda */
                });

        return single_lex_output(new_lex, out_lex_count);
    }

    void update(const string &q, LEX *lex, Analysis &a) const {
        const string &table =
            lex->select_lex.table_list.first->table_name;
        const string &dbname = lex->select_lex.table_list.first->db;

        auto drop_it = List_iterator<Alter_drop>(lex->alter_info.drop_list);
        eachList<Alter_drop>(drop_it,
            [table, dbname, &a, q] (Alter_drop *adrop) {
                // FIXME: Possibly this should be an assert as mixed clauses
                // are not supported?
                assert(adrop->type == Alter_drop::COLUMN);
                // Remove metadata from embedded database.
                ostringstream s;
                s << " DELETE FROM pdb.field_info, pdb.onion_info, "
                  << "             pdb.layer_key"
                  << " USING pdb.table_info INNER JOIN pdb.field_info "
                  << "       INNER JOIN pdb.onion_info INNER JOIN "
                  << "       pdb.layer_key"
                  << " ON  pdb.table_info.id = pdb.field_info.table_info_id"
                  << " AND pdb.field_info.id = pdb.onion_info.field_info_id"
                  << " AND pdb.onion_info.id = pdb.layer_key.onion_info_id "
                  << " WHERE pdb.table_info.name = '" << table << "' "
                  << " AND pdb.table_info.database_name = '" << dbname << "';";

                assert(a.ps->e_conn->execute(s.str()));


                // Remove from *Meta structures.
                assert(a.destroyFieldMeta(table, adrop->name));});

        // Remove column from embedded database.
        assert(a.ps->e_conn->execute(q));
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
        this->update(q, lex, a);
        return this->rewrite(lex, a, q, out_lex_count);
    }

    // TODO: Add index metadata to persistency.
    // TODO: Add index to embedded shallow mirror.
    void update(const string &q, LEX *lex, Analysis &a) const {
        return;
    }

    LEX **rewrite(LEX *lex, Analysis &a, const string &q,
                  unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        // Rewrite the index names and choose the onion to apply it too.
        auto key_it = List_iterator<Key>(lex->alter_info.key_list);
        new_lex->alter_info.key_list =
            reduceList<Key>(key_it, List<Key>(),
                [table, a] (List<Key> out_list, Key *key) {
                    // TODO: key->name;
                    auto col_it =
                        List_iterator<Key_part_spec>(key->columns);
                    key->columns = 
                        reduceList<Key_part_spec>(col_it,
                                                  List<Key_part_spec>(),
                            [table, a] (List<Key_part_spec> out_field_list,
                                        Key_part_spec *key_part) {
                                string field_name =
                                    convert_lex_str(key_part->field_name);
                                FieldMeta *fm =
                                    a.getFieldMeta(table, field_name);
                                // TODO: Algorithm for determimining which
                                // onion to apply the index to.
                                key_part->field_name = 
                                    string_to_lex_str(fm->onions[oOPE]->onionname);
                                out_field_list.push_back(key_part);
                                return out_field_list;
                            }); 
                    out_list.push_back(key);
                    return out_list;
                });

        return single_lex_output(new_lex, out_lex_count);
    }
};

class DropIndexSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

LEX **AlterSubHandler::transformLex(LEX *lex, Analysis &a, const string &q,
                                    unsigned *out_lex_count) const {
    return this->rewriteAndUpdate(lex, a, q, out_lex_count);
}

// TODO: Use this in other handlers.
static LEX **single_lex_output(LEX *out_me, unsigned *out_lex_count)
{
    LEX **out_lex = new LEX*[1];
    out_lex[0] = out_me;
    *out_lex_count = 1;
    return out_lex;
}

AlterDispatcher *buildAlterSubDispatcher() {
    AlterDispatcher *dispatcher = new AlterDispatcher();
    AlterSubHandler *h;
    
    h = new AddColumnSubHandler();
    dispatcher->addHandler(ALTER_ADD_COLUMN, h);

    h = new DropColumnSubHandler();
    dispatcher->addHandler(ALTER_DROP_COLUMN, h);

    h = new ChangeColumnSubHandler();
    dispatcher->addHandler(ALTER_CHANGE_COLUMN, h);

    h = new ForeignKeySubHandler();
    dispatcher->addHandler(ALTER_FOREIGN_KEY, h);

    h = new AddIndexSubHandler();
    dispatcher->addHandler(ALTER_ADD_INDEX, h);

    h = new DropIndexSubHandler();
    dispatcher->addHandler(ALTER_DROP_INDEX, h);

    return dispatcher;
}

