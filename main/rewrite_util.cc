#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
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
    // Table name can only be empty when grouping a nested join.
    assert(t->table_name || t->nested_join);
    if (t->table_name) {
        string anon_name = a.getAnonTableName(string(t->table_name,
                                                     t->table_name_length));
        return rewrite_table_list(t, anon_name);
    } else {
        return copy(t);
    }
}

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, std::string anon_name)
{
    TABLE_LIST *new_t = copy(t);
    new_t->table_name =
        make_thd_string(anon_name, &new_t->table_name_length);
    new_t->alias = make_thd_string(anon_name);
    new_t->next_local = NULL;

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

//TODO(raluca) : figure out how to create Create_field from scratch
// and avoid this chaining and passing f as an argument
static Create_field *
get_create_field(Create_field * f, vector<EncLayer*> & v, const string & name) {

    Create_field * new_cf = f;
    
    for (auto l : v) {
	Create_field * old_cf = new_cf;
	new_cf = l->newCreateField(old_cf, name);

	if (old_cf != f) {
	    delete old_cf;
	}
    }

    return new_cf;
    
}

vector<Create_field *>
rewrite_create_field(FieldMeta *fm, Create_field *f, const Analysis &a)
{
    LOG(cdb_v) << "in rewrite create field for " << *f;

    vector<Create_field *> output_cfields;

    // FIXME: This sequence checking for encryption is broken.
    if (!fm->isEncrypted()) {
        // Unencrypted field
        output_cfields.push_back(f);
        return output_cfields;
    }

    // Encrypted field

    //check if field is not encrypted
    if (fm->children.empty()) {
        output_cfields.push_back(f);
        //cerr << "onions were empty" << endl;
        return output_cfields;
    }

    // create each onion column
    for (auto oit : fm->orderedOnionMetas()) {
        OnionMeta *om = oit.second;
	Create_field * new_cf =
            get_create_field(f, om->layers, om->getAnonOnionName());
	/*
	EncLayer * last_layer = oit->second->layers.back();
	//create field with anonymous name
	Create_field * new_cf =
            last_layer->newCreateField(f, oit->second->getAnonOnionName().c_str());
	*/
        output_cfields.push_back(new_cf);
    }

    // create salt column
    if (fm->has_salt) {
        //cerr << fm->salt_name << endl;
        THD *thd         = current_thd;
        Create_field *f0 = f->clone(thd->mem_root);
        f0->field_name   = thd->strdup(fm->getSaltName().c_str());
	f0->flags = f0->flags | UNSIGNED_FLAG;//salt is unsigned
        f0->sql_type     = MYSQL_TYPE_LONGLONG;
	f0->length       = 8;
        output_cfields.push_back(f0);
    }

    return output_cfields;
}

// TODO: Add Key for oDET onion as well.
vector<Key*>
rewrite_key(const string &table, Key *key, Analysis &a)
{
    vector<Key*> output_keys;
    Key *new_key = key->clone(current_thd->mem_root);    
    auto col_it =
        List_iterator<Key_part_spec>(key->columns);
    // FIXME: Memleak.
    new_key->name =
        string_to_lex_str(a.getAnonIndexName(table, convert_lex_str(key->name)));
    new_key->columns = 
        reduceList<Key_part_spec>(col_it, List<Key_part_spec>(),
            [table, a] (List<Key_part_spec> out_field_list,
                        Key_part_spec *key_part) {
                string field_name =
                    convert_lex_str(key_part->field_name);
                FieldMeta *fm =
                    a.getFieldMeta(table, field_name);
                OnionMeta *om = fm->getOnionMeta(oOPE);
                key_part->field_name =
                    string_to_lex_str(om->getAnonOnionName());
                out_field_list.push_back(key_part);
                return out_field_list; /* lambda */
            });
    output_keys.push_back(new_key);

    return output_keys;
}

bool
do_add_field(FieldMeta *fm, Analysis &a, std::string dbname,
             std::string table)
{
    // Add the field data to the proxy db.
    // TODO: hasSensitive
    ostringstream s;
    s << " INSERT INTO pdb.field_info VALUES ("
      << "       (SELECT id FROM pdb.table_info "
      << "         WHERE pdb.table_info.database_name = '" << dbname << "'"
      << "           AND pdb.table_info.name = '" << table << "'),"
      << " '" << fm->fname << "', "
      << " "  << bool_to_string(fm->has_salt) << ", "
      << " '" << fm->getSaltName() << "', "
      << " '" << TypeText<onionlayout>::toText(fm->onion_layout)<< "',"
      << " "  << fm->getUniqCount() << ", "
      << " 0"
      << " );";

    assert(a.ps->e_conn->execute(s.str()));
    
    unsigned long long fieldID = a.ps->e_conn->last_insert_id();

    // Add the onion data to the proxy db.
    for (std::pair<AbstractMetaKey *, DBMeta *> onion_pair :
            fm->children) {
        // FIXME: dynamic_cast
        const onion o =
            static_cast<OnionMetaKey *>(onion_pair.first)->getValue();
        OnionMeta * const om = static_cast<OnionMeta *>(onion_pair.second);
        ostringstream s;

        SECLEVEL current_sec_level = fm->getOnionLevel(o);
        assert(current_sec_level != SECLEVEL::INVALID);
        std::string str_seclevel =
            TypeText<SECLEVEL>::toText(current_sec_level);
        std::string str_onion = TypeText<onion>::toText(o);
        s << " INSERT INTO pdb.onion_info VALUES ("
          << " " << std::to_string(fieldID) << ", "
          << " '" << om->getAnonOnionName() << "', "
          << " '" << str_onion << "', "
          << " '" << str_seclevel << "', "
          << " 0);";

        assert(a.ps->e_conn->execute(s.str()));

        // Add the encryption layer data to the proxy db.
        unsigned long long onionID = a.ps->e_conn->last_insert_id();
        for (unsigned int i = 0; i < om->layers.size(); ++i) {
            SECLEVEL level = fm->onion_layout[o][i];
            std::string str_level =
                TypeText<SECLEVEL>::toText(level);

            std::string crypto_key = "whatever";
                // EncLayerFactory::serializeLayer(om->layers[i], om);

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
                assert(i == om->layers.size() - 1);
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

bool
string_to_bool(std::string s)
{
    if (s == std::string("TRUE") || s == std::string("1")) {
        return true;
    } else if (s == std::string("FALSE") || s == std::string("0")) {
        return false;
    } else {
        throw "unrecognized string in string_to_bool!";
    }
}

LEX **
single_lex_output(LEX *out_me, unsigned *out_lex_count)
{
    LEX **out_lex = new LEX*[1];
    out_lex[0] = out_me;
    *out_lex_count = 1;
    return out_lex;
}

List<Create_field>
createAndRewriteField(Create_field *cf, TableMeta *tm,
                      const std::string &table, const std::string &dbname,
                      Analysis &a, bool new_table,
                      List<Create_field> &rewritten_cfield_list)
{
    // -----------------------------
    //         Update FIELD       
    // -----------------------------
    std::string name = std::string(cf->field_name);
    FieldMeta *fm =
        new FieldMeta(name, cf, a.ps->masterKey, tm->leaseIncUniq());
    // Here we store the key name for the first time. It will be applied
    // after the Delta is read out of the database.
    if (true == new_table) {
        tm->addChild(new IdentityMetaKey(name), fm);
    } else {
        Delta d(Delta::CREATE, fm, tm, new IdentityMetaKey(name));
        a.deltas.push_back(d);
    }
    // FIXME: Remove.
    // assert(tm->addChild(name, fm));
    // assert(do_add_field(fm, a, dbname, table));
    // -----------------------------
    //         Rewrite FIELD       
    // -----------------------------
    auto new_fields = rewrite_create_field(fm, cf, a);
    rewritten_cfield_list.concat(vectorToList(new_fields));

    return rewritten_cfield_list;
}

