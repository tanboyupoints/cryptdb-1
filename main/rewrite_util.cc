#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <main/rewrite_main.hh>
#include <parser/lex_util.hh>
#include <List_helpers.hh>

using namespace std;

extern CItemTypesDir itemTypes;

void
optimize(Item **i, Analysis &a) {
   //TODO
/*Item *i0 = itemTypes.do_optimize(*i, a);
    if (i0 != *i) {
        // item i was optimized (replaced) by i0
        if (a.itemRewritePlans.find(*i) != a.itemRewritePlans.end()) {
            a.itemRewritePlans[i0] = a.itemRewritePlans[*i];
            a.itemRewritePlans.erase(*i);
        }
        *i = i0;
    } */
}


// this function should be called at the root of a tree of items
// that should be rewritten
// @context defaults to empty string.
Item *
rewrite(Item *i, const OLK & constr, Analysis &a, string context)
{
    if (context.size()) {
	context = " for " + context;
    }
    RewritePlan * rp = getAssert(a.rewritePlans, i);
    assert(rp);
    if (!rp->es_out.contains(constr)) {
	cerr << "query cannot be supported because " << i << " needs to return " << constr << context << "\n" \
	     << "BUT it can only return " << rp->es_out << " BECAUSE " << rp->r << "\n";
	assert(false);
    }
    return itemTypes.do_rewrite(i, constr, rp, a);
}

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, Analysis &a)
{
    TABLE_LIST * new_t = copy(t);

    // Table name can only be empty when grouping a nested join.
    assert(t->table_name || t->nested_join);
    if (t->table_name) {
        string anon_name = a.getAnonTableName(string(t->table_name,
                                                     t->table_name_length));
        new_t->table_name = make_thd_string(anon_name, &new_t->table_name_length);
        new_t->alias = make_thd_string(anon_name);
        new_t->next_local = NULL;
    }

    return new_t;
}

// @if_exists: defaults to false.
SQL_I_List<TABLE_LIST>
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a,
                   bool if_exists)
{
    if (!tlist.elements) {
	return SQL_I_List<TABLE_LIST>();
    }

    TABLE_LIST * tl;
    if (if_exists && (false == a.tableMetaExists(tlist.first->table_name))) {
       tl = copy(tlist.first);
    } else {
       tl = rewrite_table_list(tlist.first, a);
    }

    SQL_I_List<TABLE_LIST> * new_tlist = oneElemList<TABLE_LIST>(tl);

    TABLE_LIST * prev = tl;
    for (TABLE_LIST *tbl = tlist.first->next_local;
	 tbl; tbl = tbl->next_local) {

        TABLE_LIST * new_tbl;
        if (if_exists && (false == a.tableMetaExists(tbl->table_name))) {
            new_tbl = copy(tbl);
        } else {
            new_tbl = rewrite_table_list(tbl, a);
        }

	prev->next_local = new_tbl;
	prev = new_tbl;
    }
    prev->next_local = NULL;

    return *new_tlist;
}

List<TABLE_LIST>
rewrite_table_list(List<TABLE_LIST> tll, Analysis & a)
{
    List<TABLE_LIST> * new_tll = new List<TABLE_LIST>();

    List_iterator<TABLE_LIST> join_it(tll);

    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t) {
	    break;
	}

        TABLE_LIST * new_t = rewrite_table_list(t, a);
	new_tll->push_back(new_t);

        if (t->nested_join) {
            new_t->nested_join->join_list = rewrite_table_list(t->nested_join->join_list, a);
            return *new_tll;
        }

        if (t->on_expr) {
            new_t->on_expr = rewrite(t->on_expr, PLAIN_OLK, a);
	}

	/* TODO: derived tables
        if (t->derived) {
            st_select_lex_unit *u = t->derived;
            rewrite_select_lex(u->first_select(), a);
        }
	*/
    }

    return *new_tll;
}

/*
 * Helper functions to look up via directory & invoke method.
 */
RewritePlan *
gather(Item *i, reason &tr, Analysis & a)
{
    return itemTypes.do_gather(i, tr, a);
}

//TODO: need to check somewhere that plain is returned
//TODO: Put in gather helpers file.
void
analyze(Item *i, Analysis & a)
{
    assert(i != NULL);
    LOG(cdb_v) << "calling gather for item " << *i;
    reason r;
    a.rewritePlans[i] = gather(i, r, a);
}

LEX *
begin_transaction_lex(Analysis a) {
    string query = "START TRANSACTION;";
    query_parse *begin_parse = new query_parse(a.ps->conn->getCurDBName(),
                                               query);
    return begin_parse->lex();
}

LEX *
commit_transaction_lex(Analysis a) {
    string query = "COMMIT;";
    query_parse *commit_parse = new query_parse(a.ps->conn->getCurDBName(),
                                                query);
    return commit_parse->lex();
}

//TODO: no need to pass create_field to this
static vector<Create_field *>
rewrite_create_field(const string &table_name, Create_field *f,
                     const Analysis &a)
{
    LOG(cdb_v) << "in rewrite create field for " << *f;

    vector<Create_field *> output_cfields;
    FieldMeta *fm = a.getFieldMeta(table_name, f->field_name);

    if (!fm->isEncrypted()) {
        // Unencrypted field
        output_cfields.push_back(f);
        return output_cfields;
    }

    // Encrypted field

    //check if field is not encrypted
    if (fm->onions.empty()) {
        output_cfields.push_back(f);
        //cerr << "onions were empty" << endl;
        return output_cfields;
    }

    // create each onion column
    for (auto oit = fm->onions.begin();
         oit != fm->onions.end();
         ++oit) {
	EncLayer * last_layer = oit->second->layers.back();
	//create field with anonymous name
	Create_field * new_cf =
            last_layer->newCreateField(oit->second->getAnonOnionName().c_str());

        output_cfields.push_back(new_cf);
    }

    // create salt column
    if (fm->has_salt) {
        //cerr << fm->salt_name << endl;
        THD *thd         = current_thd;
        Create_field *f0 = f->clone(thd->mem_root);
        f0->field_name   = thd->strdup(fm->saltName().c_str());
	f0->flags = f0->flags | UNSIGNED_FLAG;//salt is unsigned
        f0->sql_type     = MYSQL_TYPE_LONGLONG;
	f0->length       = 8;
        output_cfields.push_back(f0);
    }

    return output_cfields;
}

void
do_field_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a)
{
    // TODO(stephentu): template this pattern away
    // (borrowed from rewrite_select_lex())
    auto cl_it = List_iterator<Create_field>(lex->alter_info.create_list);
    List<Create_field> newList;
    new_lex->alter_info.create_list =
        reduceList<Create_field>(cl_it, newList, [table, &a] (List<Create_field> out_list, Create_field *cf) {
            out_list.concat(vectorToList(rewrite_create_field(table, cf, a)));
            return out_list; /* lambda */
         });
}

// TODO: Add Key for oDET onion as well.
// TODO: Anonymize index name (key->name).
static vector<Key*>
rewrite_key(const string &table, Key *key, const Analysis &a)
{
    vector<Key*> output_keys;
    Key *new_key = key->clone(current_thd->mem_root);    
    auto col_it =
        List_iterator<Key_part_spec>(key->columns);
    new_key->columns = 
        reduceList<Key_part_spec>(col_it,
                                  List<Key_part_spec>(),
            [table, a] (List<Key_part_spec> out_field_list,
                        Key_part_spec *key_part) {
                string field_name =
                    convert_lex_str(key_part->field_name);
                FieldMeta *fm =
                    a.getFieldMeta(table, field_name);
                key_part->field_name = 
                    string_to_lex_str(fm->onions[oOPE]->getAnonOnionName());
                out_field_list.push_back(key_part);
                return out_field_list;
            });
    output_keys.push_back(new_key);

    return output_keys;
}

void
do_key_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a)
{
    // Rewrite the index names and choose the onion to apply it too.
    auto key_it = List_iterator<Key>(lex->alter_info.key_list);
    new_lex->alter_info.key_list =
        reduceList<Key>(key_it, List<Key>(),
            [table, a] (List<Key> out_list, Key *key) {
                out_list.concat(vectorToList(rewrite_key(table, key, a)));
                return out_list;
            });
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
        OnionMeta * om = new OnionMeta(o, index, fm->fname);
        fm->onions[o] = om;

        // HACK(burrows)
        om->sql_type = cf->sql_type;

        if (mKey) {
            //generate enclayers for encrypted field
            string uniqueFieldName = fm->fullName(o);
            for (auto l: it.second) {
                string key;
                key = getLayerKey(mKey, uniqueFieldName, l);
                om->layers.push_back(EncLayerFactory::encLayer(o, l, cf, key));
            }
        }

        LOG(cdb_v) << "adding onion layer " << om->getAnonOnionName() << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

// @index: default value 0
void
init_onions(AES_KEY * mKey, FieldMeta * fm, Create_field * cf, uint index) {
    if (!mKey) {
        // unencrypted field
        init_onions_layout(NULL, fm, 0, cf, PLAIN_ONION_LAYOUT);
        fm->has_salt = false;
        return;
    }

    // Encrypted field

    fm->has_salt = true;

    if (IsMySQLTypeNumeric(cf->sql_type)) {
        init_onions_layout(mKey, fm, index, cf, NUM_ONION_LAYOUT);
    } else {
        init_onions_layout(mKey, fm, index, cf, STR_ONION_LAYOUT);
    }
}

// TODO: Should be constructor.
bool
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

// @tid: defaults to NULL.
bool
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

std::string
bool_to_string(bool b)
{
    if (true == b) {
        return "TRUE";
    } else {
        return "FALSE";
    }
}
