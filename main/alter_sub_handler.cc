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

        // Create *Meta objects.
        auto add_it =
            List_iterator<Create_field>(lex->alter_info.create_list);
        eachList<Create_field>(add_it,
            [tm, a, dbname, table] (Create_field *cf) {
                FieldMeta *fm =
                    tm->createFieldMeta(cf, a, a.ps->encByDefault);
                assert(fm);

                // Add metadata to embedded database.
                assert(do_add_field(fm, a, dbname, table));
        });

        // Add field to embedded database.
        assert(a.ps->e_conn->execute(q));
    }

    LEX **rewrite(LEX *lex, Analysis &a, const string &q,
                  unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        do_field_rewriting(lex, new_lex, table, a);
        // TODO: Should this be here?
        do_key_rewriting(lex, new_lex, table, a);
        return single_lex_output(new_lex, out_lex_count);
    }
};

class DropColumnSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        const std::string &dbname =
            lex->select_lex.table_list.first->db;

        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);
        
        // Get the column drops.
        auto drop_it =
            List_iterator<Alter_drop>(lex->alter_info.drop_list);
        List<Alter_drop> col_drop_list =
            filterList(drop_it,
                [](Alter_drop *adrop) {
                    return Alter_drop::COLUMN == adrop->type;
                });

        // Rewrite and Remove.
        drop_it = List_iterator<Alter_drop>(col_drop_list);
        new_lex->alter_info.drop_list =
            reduceList<Alter_drop>(drop_it, List<Alter_drop>(),
                [table, dbname, &a, this] (List<Alter_drop> out_list,
                                           Alter_drop *adrop) {
                    FieldMeta *fm = a.getFieldMeta(table, adrop->name);
                    List<Alter_drop> lst = this->rewrite(fm, adrop);
                    out_list.concat(&lst);
                    this->update(a, table, dbname, adrop); 
                    return out_list; /* lambda */
                });

        // Remove column from embedded database.
        assert(a.ps->e_conn->execute(q));

        return single_lex_output(new_lex, out_lex_count);
    }

    List<Alter_drop> rewrite(FieldMeta *fm, Alter_drop *adrop) const
    {
        List<Alter_drop> out_list;
        THD *thd = current_thd;

        // Rewrite each onion column.
        for (auto onion_pair : fm->onions) {
            Alter_drop *new_adrop =
                adrop->clone(thd->mem_root);
            new_adrop->name =
                thd->strdup(onion_pair.second->getAnonOnionName().c_str());
            out_list.push_back(new_adrop);
        }

        // Rewrite the salt column.
        if (fm->has_salt) {
            Alter_drop *new_adrop = adrop->clone(thd->mem_root);
            new_adrop->name =
                thd->strdup(fm->saltName().c_str());
            out_list.push_back(new_adrop);
        }

        return out_list;
    }

    void update(Analysis &a, const std::string &table,
                const std::string &dbname, Alter_drop *adrop) const
    {
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
        assert(a.destroyFieldMeta(table, adrop->name));
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
        LEX **out_lex = this->rewrite(lex, a, q, out_lex_count);
        this->update(q, lex, a);

        return out_lex;
    }

    // NOTE: This must be executed after 'this->rewrite' because it needs
    // the TableMeta to update it's index_counter.
    // TODO: Add index to embedded shallow mirror.
    void update(const string &q, LEX *lex, Analysis &a) const {
        const string &table =
            lex->select_lex.table_list.first->table_name;
        TableMeta *tm = a.getTableMeta(table);
        
        // Update the counter.
        ostringstream s;
        s << " UPDATE pdb.table_info"
          << "    SET index_counter = " << tm->getIndexCounter()
          << "  WHERE number = " << tm->tableNo
          <<"    AND database_name = '"<<a.ps->e_conn->getCurDBName()<<"';";
        assert(a.ps->e_conn->execute(s.str()));

        // Add each new index.
        auto key_it =
            List_iterator<Key>(lex->alter_info.key_list);
        eachList<Key>(key_it, [table, tm, a] (Key *key) {
            std::string index_name = convert_lex_str(key->name);
            std::string anon_name = a.getAnonIndexName(table, index_name);

            ostringstream s;
            s << " INSERT INTO pdb.index_info VALUES ("
              << " (SELECT table_info.id "
              << "    FROM pdb.table_info"
              << "  WHERE number = " << tm->tableNo
              << "    AND database_name = '"
              <<          a.ps->e_conn->getCurDBName() << "'), "
              << " '" << index_name << "', "
              << " '" << anon_name << "', "
              << " 0"
              << " );";

            assert(a.ps->e_conn->execute(s.str()));
        });
 
        return;
    }

    LEX **rewrite(LEX *lex, Analysis &a, const string &q,
                  unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        do_key_rewriting(lex, new_lex, table, a);

        return single_lex_output(new_lex, out_lex_count);
    }
};

class DropIndexSubHandler : public AlterSubHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        LEX *new_lex = copy(lex);
        const string &table =
            lex->select_lex.table_list.first->table_name;
        const string &dbname =
            lex->select_lex.table_list.first->db;
        TableMeta *tm = a.getTableMeta(table);
        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);

        // Get the key drops.
        auto drop_it =
            List_iterator<Alter_drop>(lex->alter_info.drop_list);
        List<Alter_drop> key_drop_list =
            filterList(drop_it,
                [](Alter_drop *adrop) {
                    return Alter_drop::KEY == adrop->type;
                });

        // Update and Rewrite.
        // FIXME: Handle oDET index as well.
        drop_it =
            List_iterator<Alter_drop>(key_drop_list);
        new_lex->alter_info.drop_list =
            reduceList<Alter_drop>(drop_it, List<Alter_drop>(),
                [table, dbname, tm, &a, this] (List<Alter_drop> out_list,
                                               Alter_drop *adrop) {
                    this->update(a, table, dbname, adrop);
                    List<Alter_drop> lst = this->rewrite(a, adrop, table);
                    out_list.concat(&lst); 
                    return out_list;
                });

        return single_lex_output(new_lex, out_lex_count);
    }

    void update(Analysis &a, const std::string &table,
                const std::string &dbname, Alter_drop *adrop) const
    {
        // Remove the index_info record from the proxy db.
        std::string index_name = adrop->name;
        std::string anon_name = a.getAnonIndexName(table, index_name);

        ostringstream s;
        s << " DELETE FROM pdb.index_info "
          << " WHERE table_info_id = "
          << "       (SELECT table_info.id "
          << "          FROM pdb.table_info"
          << "         WHERE name = '" << table << "'"
          << "           AND database_name = '" << dbname << "')"
          << "   AND name = '" << index_name << "'"
          << "   AND anon_name = '" << anon_name << "';";

        assert(a.ps->e_conn->execute(s.str()));
    }

    List<Alter_drop> rewrite(Analysis a, Alter_drop *adrop,
                             const std::string &table) const
    {
        // Rewrite the Alter_drop data structure.
        List<Alter_drop> out_list;
        THD *thd = current_thd;
        Alter_drop *new_adrop = adrop->clone(thd->mem_root);  
        new_adrop->name =
            make_thd_string(a.getAnonIndexName(table, adrop->name));

        // Remove from *Meta.
        a.destroyIndex(table, adrop->name);

        out_list.push_back(new_adrop);
        return out_list;
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

