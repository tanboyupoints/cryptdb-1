#include <main/ddl_handler.hh>

#include <parser/lex_util.hh>
#include <main/rewrite_util.hh>
#include <main/List_helpers.hh>
#include <main/cdb_rewrite.hh>
#include <main/enum_text.hh>

std::map<enum_sql_command, DDLHandler *> DDLHandler::handlers;

// TODO: Remove rewrite_create_field from cbd_rewrite.cc
// TODO: Remove rewrite_key from cdb_rewrite.cc

// Prototypes.
// TODO: Remove from cdb_rewrite.cc
static bool
do_add_field(TableMeta *tm, const Analysis &a, std::string dbname,
             std::string table, unsigned long long *tid=NULL);

// TODO: Remove from cdb_rewrite.cc
static void
do_field_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a);

static void
create_table_meta(Analysis & a, const string & table, LEX *lex,
                  bool encByDefault);

static std::string
bool_to_string(bool b);

// TODO: Write a dispatcher.
class AlterHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(false);
    }
};

class CreateHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        update(q, lex, a);
        return rewrite(lex, a, out_lex_count);
    }

    void update(const string &q, LEX *lex, Analysis &a) const {
        char* dbname = lex->select_lex.table_list.first->db;
        char* table  = lex->select_lex.table_list.first->table_name;

        // If we are only creating the table if it doesn't exist,
        // we must forgo the duplication of meta objects and such.
        if (lex->create_info.options & HA_LEX_CREATE_IF_NOT_EXISTS) {
            if (a.tableMetaExists(table)) {
                return;
            }
        }

        // TODO(burrows): This should be a seperate step.
        // Create *Meta objects.
        create_table_meta(a, std::string(table), lex, a.ps->encByDefault);

        // Add table to embedded database.
        assert(a.ps->e_conn->execute(q));

        TableMeta *tm = a.ps->schema->tableMetaMap[table];
        assert(tm != NULL);

        a.ps->e_conn->execute("START TRANSACTION");

        {
            ostringstream s;
            s << " INSERT INTO pdb.table_info VALUES ("
              << " " << tm->tableNo << ", "
              << " '" << tm->anonTableName << "', "
              << " '" << table << "', "
              << " " << bool_to_string(tm->hasSensitive) << ", "
              << " " << bool_to_string(tm->has_salt) << ", "
              << " '" << tm->salt_name << "', "
              << " '" << dbname << "',"
              << " 0"
              << " );";

            assert(a.ps->e_conn->execute(s.str()));
        }

        // Add field.
        unsigned long long tableID = a.ps->e_conn->last_insert_id();
        do_add_field(tm, a, dbname, table, &tableID);

        a.ps->e_conn->execute("COMMIT");
    }

    LEX **rewrite(LEX *lex, Analysis &a, unsigned *out_lex_count) const {
        LEX * new_lex = copy(lex);

        // table name
        const string &table =
            lex->select_lex.table_list.first->table_name;

        new_lex->select_lex.table_list =
            rewrite_table_list(lex->select_lex.table_list, a);
        
        //TODO: support for "create table like"
        if (lex->create_info.options & HA_LEX_CREATE_TABLE_LIKE) {
            cryptdb_err() << "No support for create table like yet. " <<
                       "If you see this, please implement me";
        } else {
            // If we are only creating the table if it doesn't exist,
            // we must forgo the duplication of meta objects and such.
            if (!(lex->create_info.options & HA_LEX_CREATE_IF_NOT_EXISTS &&
                a.tableMetaExists(table))) {
                do_field_rewriting(lex, new_lex, table, a);
            }
        }

        LEX **out_lex = new LEX*[1];
        out_lex[0] = new_lex;
        *out_lex_count = 1;
        return out_lex;
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

        LEX **out_lex = new LEX*[1];
        out_lex[0] = new_lex;
        *out_lex_count = 1;
        return out_lex;;
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

class ChangeDBHandler : public DDLHandler {
    virtual LEX **rewriteAndUpdate(LEX *lex, Analysis &a, const string &q,
                                   unsigned *out_lex_count) const {
        assert(lex->select_lex.db);
        char* dbname = lex->select_lex.db;

        // new dbname is saved for next queries
        a.ps->conn->setCurDBName(dbname);
        a.ps->e_conn->setCurDBName(dbname);

        LEX **out_lex = new LEX*[1];
        out_lex[0] = lex;
        *out_lex_count = 1;
        return out_lex;
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

LEX** DDLHandler::transformLex(LEX *lex, Analysis &analysis,
                                      const string &q, unsigned *out_lex_count) {
    const DDLHandler *handler = DDLHandler::dispatch(lex->sql_command);
    if (!handler) {
        return NULL;
    }

    return handler->rewriteAndUpdate(lex, analysis, q, out_lex_count);
}

// @tid: defaults to NULL.
static bool
do_add_field(TableMeta *tm, const Analysis &a, std::string dbname,
             std::string table, unsigned long long *tid)
{
    unsigned long long table_id;
    if (NULL == tid) {
        DBResult *dbres;
        ostringstream s;
        s << " SELECT id FROM pdb.table_info "
          << " WHERE pdb.table_info.database_name = '" << dbname << "'"
          << "   AND pdb.table_info.name = '" << table << "';";

        assert(a.ps->e_conn->execute(s.str(), dbres));

        ScopedMySQLRes r(dbres->n);
        MYSQL_ROW row;

        if (1 != mysql_num_rows(r.res())) {
            return false;
        }

        while ((row = mysql_fetch_row(r.res()))) {
            unsigned long *l = mysql_fetch_lengths(r.res());
            assert(l != NULL);

            string table_id(row[0], l[0]);
            table_id = (unsigned long long)atoi(table_id.c_str());
        }
    } else {
        table_id = *tid;
    }

    for (std::pair<std::string, FieldMeta *> fm_pair: tm->fieldMetaMap) {
        FieldMeta *fm = fm_pair.second;
        ostringstream s;
        s << " INSERT INTO pdb.field_info VALUES ("
          << " " << table_id << ", "
          << " '" << fm->fname << "', "
          << " " << fm->index << ", "
          << " " << bool_to_string(fm->has_salt) << ", "
          << " '" << fm->salt_name << "',"
          << " '" << TypeText<onionlayout>::toText(fm->onion_layout)<< "',"
          << " 0"
          << " );";

        assert(a.ps->e_conn->execute(s.str()));

        unsigned long long fieldID = a.ps->e_conn->last_insert_id();

        for (std::pair<onion, OnionMeta *> onion_pair: fm->onions) {
            OnionMeta *om = onion_pair.second;
            onion o = onion_pair.first;
            ostringstream s;

            SECLEVEL current_sec_level = fm->getOnionLevel(o);
            assert(current_sec_level != SECLEVEL::INVALID);
            std::string str_seclevel =
                TypeText<SECLEVEL>::toText(current_sec_level);
            std::string str_onion  = TypeText<onion>::toText(o);
            s << " INSERT INTO pdb.onion_info VALUES ("
              << " " << std::to_string(fieldID) << ", "
              << " '" << om->onionname << "', "
              << " '" << str_onion << "', "
              << " '" << str_seclevel << "', "
              << " '" << TypeText<enum enum_field_types>::toText(om->sql_type) << "', "
              << " 0);";

            assert(a.ps->e_conn->execute(s.str()));

            unsigned long long onionID = a.ps->e_conn->last_insert_id();
            for (unsigned int i = 0; i < onion_pair.second->layers.size(); ++i) {
                SECLEVEL level = fm->onion_layout[o][i];
                std::string str_level =
                    TypeText<SECLEVEL>::toText(level);

                std::string crypto_key = onion_pair.second->layers[i]->serialize();

                unsigned int escaped_length = crypto_key.size() * 2 + 1;
                char escaped_key[escaped_length];
                a.ps->e_conn->real_escape_string(escaped_key,
                                                 crypto_key.c_str(),
                                                 escaped_length);

                ostringstream s;
                s << " INSERT INTO pdb.layer_key VALUES ("
                  << " " << onionID << ", "
                  << " '" << escaped_key << "', "
                  << " '" << str_onion << "', "
                  << " '" << str_level << "', "
                  << " '" << crypto_key.size() << "', "
                  << " 0"
                  << " );";

                assert(a.ps->e_conn->execute(s.str()));
                // The last iteration should get us to the current
                // security level.
                if (current_sec_level == level) {
                    assert(i == onion_pair.second->layers.size() - 1);
                }
            }
        }
    }

    return true;
}

//TODO: no need to pass create_field to this
static void rewrite_create_field(const string &table_name,
                                 Create_field *f,
                                 const Analysis &a,
                                 vector<Create_field *> &l)
{
    LOG(cdb_v) << "in rewrite create field for " << *f;

    FieldMeta *fm = a.getFieldMeta(table_name, f->field_name);

    if (!fm->isEncrypted()) {
        // Unencrypted field
        l.push_back(f);
        return;
    }

    // Encrypted field

    //check if field is not encrypted
    if (fm->onions.empty()) {
        l.push_back(f);
        //cerr << "onions were empty" << endl;
        return;
    }

    // create each onion column
    for (auto oit = fm->onions.begin();
         oit != fm->onions.end();
         ++oit) {
	EncLayer * last_layer = oit->second->layers.back();
	//create field with anonymous name
	Create_field * new_cf = last_layer->newCreateField(oit->second->onionname.c_str());

        l.push_back(new_cf);
    }

    // create salt column
    if (fm->has_salt) {
        //cerr << fm->salt_name << endl;
        assert(!fm->salt_name.empty());
        THD *thd         = current_thd;
        Create_field *f0 = f->clone(thd->mem_root);
        f0->field_name   = thd->strdup(fm->salt_name.c_str());
	f0->flags = f0->flags | UNSIGNED_FLAG;//salt is unsigned
        f0->sql_type     = MYSQL_TYPE_LONGLONG;
	f0->length       = 8;
        l.push_back(f0);
    }
}

static void
rewrite_key(const string &table_name, Key *k, Analysis &a, vector<Key*> &l)
{
    //TODO
    l.push_back(k);
}

static void
do_field_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a)
{
    // TODO(stephentu): template this pattern away
    // (borrowed from rewrite_select_lex())
    auto cl_it = List_iterator<Create_field>(lex->alter_info.create_list);
    List<Create_field> newList;
    new_lex->alter_info.create_list =
        reduceList<Create_field>(cl_it, newList, [table, &a] (List<Create_field> out_list, Create_field *cf) {
            vector<Create_field *> l;
            rewrite_create_field(table, cf, a, l);
            List<Create_field> temp_list = vectorToList(l);
            out_list.concat(&temp_list);
            return out_list; /* lambda */
         });

    auto k_it = List_iterator<Key>(lex->alter_info.key_list);
    List<Key> newList0;
    new_lex->alter_info.key_list =
        reduceList<Key>(k_it, newList0, [table, &a] (List<Key> out_list,
                                                     Key *k) {
            vector<Key *> l;
            rewrite_key(table, k, a, l);
            List<Key> temp_list = vectorToList(l);
            out_list.concat(&temp_list);
            return out_list; /* lambda */
        });
}

static void
check_table_not_exists(Analysis & a, LEX * lex, string table) {
    auto it = a.ps->schema->tableMetaMap.find(table);
    if (it != a.ps->schema->tableMetaMap.end()) {
        if (!(lex->create_info.options & HA_LEX_CREATE_IF_NOT_EXISTS)) {
            LOG(warn) << "ERROR: Table exists. Embedded DB possibly"
		"out of sync with regular DB (or, just programmer error)";
        }
        return;
    }
}

// If mkey == NULL, the field is not encrypted
static void
init_onions_layout(AES_KEY * mKey, FieldMeta * fm, uint index, Create_field * cf, onionlayout ol) {

    fm->onions.clear();

    // This additional reflection is needed as we must rebuild the
    // OnionMeta's (and their layers) after a restart.
    fm->onion_layout = ol;

    for (auto it: ol) {
        onion o = it.first;
        OnionMeta * om = new OnionMeta();
        fm->onions[o] = om;

        om->onionname = anonymizeFieldName(index, o, fm->fname, false);
        // HACK(burrows)
        om->sql_type = cf->sql_type;

        if (mKey) {
            //generate enclayers for encrypted field
            for (auto l: it.second) {
                string key;

                // TODO(burrows): This can be pulled out of loop.
                string uniqueFieldName = fullName(om->onionname,
                                                  fm->tm->anonTableName);
                key = getLayerKey(mKey, uniqueFieldName, l);
                om->layers.push_back(EncLayerFactory<string>::encLayer(o, l, cf, key));
            }
        }

        LOG(cdb_v) << "adding onion layer " << om->onionname << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

static void
init_onions(AES_KEY * mKey, FieldMeta * fm, Create_field * cf, uint index = 0) {
    if (!mKey) {
        // unencrypted field
        init_onions_layout(NULL, fm, 0, cf, PLAIN_ONION_LAYOUT);
        fm->has_salt = false;
        return;
    }

    // Encrypted field

    fm->has_salt = true;
    fm->salt_name = getFieldSalt(index, fm->tm->anonTableName);

    if (IsMySQLTypeNumeric(cf->sql_type)) {
        init_onions_layout(mKey, fm, index, cf, NUM_ONION_LAYOUT);
    } else {
        init_onions_layout(mKey, fm, index, cf, STR_ONION_LAYOUT);
    }
}

static bool
create_field_meta(TableMeta *tm, Create_field *field,
                  const Analysis a, bool encByDefault)
{
    FieldMeta * fm = new FieldMeta();

    fm->tm            = tm;
    fm->sql_field     = field->clone(current_thd->mem_root);
    fm->fname         = string(fm->sql_field->field_name);
    fm->index         = tm->fieldNames.size();

    if (encByDefault) {
        init_onions(a.ps->masterKey, fm, field, fm->index);
    } else {
        init_onions(NULL, fm, field);
    }

    // FIXME: Use exists function.
    if (tm->fieldMetaMap.find(fm->fname) != tm->fieldMetaMap.end()) {
        return false;
    }

    tm->fieldMetaMap[fm->fname] = fm;
    tm->fieldNames.push_back(fm->fname);//TODO: do we need fieldNames?

    return true;
}


static void
create_table_meta(Analysis & a, const string & table, LEX *lex,
                  bool encByDefault) {
    assert(lex->sql_command == SQLCOM_CREATE_TABLE);

    LOG(cdb_v) << "add_table encByDefault " << encByDefault;

    // FIXME: Stop using this function.
    check_table_not_exists(a, lex, table);

    // FIXME: Use SchemaInfo::createTableMeta.
    // What is the role of has_salt, has_sensitive and salt_name?
    TableMeta *tm = new TableMeta();
    a.ps->schema->tableMetaMap[table] = tm;

    if (encByDefault) { //anonymize name
	// such increment may cause problem with multiple proxies
        tm->tableNo = a.ps->schema->totalTables++;
        tm->anonTableName = anonymizeTableName(tm->tableNo, table);
    } else {
        tm->anonTableName = table;
    }

    auto it = List_iterator<Create_field>(lex->alter_info.create_list);
    eachList<Create_field>(it, [tm, a, encByDefault] (Create_field *cf) {
        create_field_meta(tm, cf, a, encByDefault);
    });
}

static std::string
bool_to_string(bool b)
{
    if (true == b) {
        return "TRUE";
    } else {
        return "FALSE";
    }
}
