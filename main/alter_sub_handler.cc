#include <main/alter_sub_handler.hh>
#include <main/List_helpers.hh>
#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <parser/lex_util.hh>

std::map<long, AlterSubHandler *> AlterSubHandler::handlers;

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

        List<Alter_drop> new_drop_list;
        auto drop_it = List_iterator<Alter_drop>(lex->alter_info.drop_list);
        new_lex->alter_info.drop_list =
            reduceList<Alter_drop>(drop_it, new_drop_list,
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

static LEX **single_lex_output(LEX *out_me, unsigned *out_lex_count)
{
    LEX **out_lex = new LEX*[1];
    out_lex[0] = out_me;
    *out_lex_count = 1;
    return out_lex;
}

