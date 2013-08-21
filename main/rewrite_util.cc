#include <memory>

#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <List_helpers.hh>

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
// @item_cache defaults to NULL
Item *
rewrite(Item *i, const EncSet &req_enc, Analysis &a)
{
    RewritePlan * const rp = getAssert(a.rewritePlans, i);
    assert(rp);
    EncSet solution = rp->es_out.intersect(req_enc);
    if (solution.empty()) {
        // FIXME: Error message;
        assert(false);
    }
    
    return itemTypes.do_rewrite(i, solution.chooseOne(), rp, a);
}

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, Analysis &a)
{
    // Table name can only be empty when grouping a nested join.
    assert(t->table_name || t->nested_join);
    if (t->table_name) {
        std::string anon_name =
            a.getAnonTableName(std::string(t->table_name,
                               t->table_name_length));
        return rewrite_table_list(t, anon_name);
    } else {
        return copy(t);
    }
}

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, std::string anon_name)
{
    TABLE_LIST * const new_t = copy(t);
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
    for (TABLE_LIST *tbl = tlist.first->next_local; tbl;
         tbl = tbl->next_local) {
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
    List<TABLE_LIST> * const new_tll = new List<TABLE_LIST>();

    List_iterator<TABLE_LIST> join_it(tll);

    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t) {
            break;
        }

        TABLE_LIST * const new_t = rewrite_table_list(t, a);
        new_tll->push_back(new_t);

        if (t->nested_join) {
            new_t->nested_join->join_list = rewrite_table_list(t->nested_join->join_list, a);
            return *new_tll;
        }

        if (t->on_expr) {
            new_t->on_expr = rewrite(t->on_expr, PLAIN_EncSet, a);
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
begin_transaction_lex(const std::string &dbname) {
    static const std::string query = "START TRANSACTION;";
    query_parse *begin_parse = new query_parse(dbname, query);
    return begin_parse->lex();
}

LEX *
commit_transaction_lex(const std::string &dbname) {
    static const std::string query = "COMMIT;";
    query_parse *commit_parse = new query_parse(dbname, query);
    return commit_parse->lex();
}

//TODO(raluca) : figure out how to create Create_field from scratch
// and avoid this chaining and passing f as an argument
static Create_field *
get_create_field(const Analysis &a, Create_field * f, OnionMeta *om,
                 const std::string & name)
{
    Create_field * new_cf = f;
    
    auto enc_layers = a.getEncLayers(om);
    assert(enc_layers.size() > 0);
    for (auto l : enc_layers) {
        Create_field * old_cf = new_cf;
        new_cf = l->newCreateField(old_cf, name);

        if (old_cf != f) {
            delete old_cf;
        }
    }

    return new_cf;
}

static Item *
makeNiceDefault(Create_field *cf)
{
    assert(cf->def);

    switch (cf->sql_type) {
        case MYSQL_TYPE_DECIMAL:
            throw CryptDBError("implement default decimal type!");

        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG: {
            const std::string val = ItemToString(cf->def);
            return new Item_int(atoi(val.c_str()));
        }
        case MYSQL_TYPE_DATE:
            throw CryptDBError("handle MYSQL_TYPE_DATE");
        case MYSQL_TYPE_TIME:
            throw CryptDBError("handle MYSQL_TYPE_TIME");
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
            return make_item(static_cast<Item_string *>(cf->def));

        case MYSQL_TYPE_NULL:
            return make_item(static_cast<Item_null *>(cf->def));
        default:
            throw CryptDBError("unrecognized default type!");
    }
}

std::vector<Create_field *>
rewrite_create_field(FieldMeta *fm, Create_field *f, const Analysis &a)
{
    LOG(cdb_v) << "in rewrite create field for " << *f;

    std::vector<Create_field *> output_cfields;

    //check if field is not encrypted
    if (fm->children.empty()) {
        output_cfields.push_back(f);
        //cerr << "onions were empty" << endl;
        return output_cfields;
    }

    const bool has_default = f->def != NULL;
    const uint64_t default_salt =
        fm->has_salt && has_default ? randomValue() : 0;

    // create each onion column
    for (auto oit : fm->orderedOnionMetas()) {
        const onion o = oit.first->getValue();
        OnionMeta * const om = oit.second;
        Create_field * const new_cf =
            get_create_field(a, f, om, om->getAnonOnionName());
        assert(has_default == static_cast<bool>(new_cf->def));
        if (has_default) {
            const std::unique_ptr<Item> def(makeNiceDefault(new_cf));
            new_cf->def =
                encrypt_item_layers(def.get(), o, om, a, default_salt);
        }
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

        if (has_default) {
            f0->def = new Item_int(static_cast<ulonglong>(default_salt));
        }

        output_cfields.push_back(f0);
    }

    return output_cfields;
}

static
const OnionMeta *
getKeyOnionMeta(const FieldMeta *fm)
{
    std::vector<onion> onions({oOPE, oDET, oPLAIN});
    for (auto it : onions) {
        const OnionMeta * const om = fm->getOnionMeta(it);
        if (NULL != om) {
            return om;
        }
    }

    assert(false);
}

// TODO: Add Key for oDET onion as well.
std::vector<Key*>
rewrite_key(const TableMeta *tm, Key *key, Analysis &a)
{
    std::vector<Key*> output_keys;
    Key * const new_key = key->clone(current_thd->mem_root);
    auto col_it =
        List_iterator<Key_part_spec>(key->columns);
    // FIXME: Memleak.
    const std::string new_name =
        a.getAnonIndexName(tm, convert_lex_str(key->name));
    new_key->name = string_to_lex_str(new_name);
    new_key->columns = 
        reduceList<Key_part_spec>(col_it, List<Key_part_spec>(),
            [tm, a] (List<Key_part_spec> out_field_list,
                        Key_part_spec *key_part) {
                const std::string field_name =
                    convert_lex_str(key_part->field_name);
                const FieldMeta * const fm =
                    a.getFieldMeta(tm, field_name);
                // HACK: Should return multiple onions.
                const OnionMeta * const om = getKeyOnionMeta(fm);
                key_part->field_name =
                    string_to_lex_str(om->getAnonOnionName());
                out_field_list.push_back(key_part);
                return out_field_list; /* lambda */
            });
    output_keys.push_back(new_key);

    return output_keys;
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

List<Create_field>
createAndRewriteField(Analysis &a, const ProxyState &ps,
                      Create_field *cf, TableMeta *tm,
                      bool new_table,
                      List<Create_field> &rewritten_cfield_list)
{
    // -----------------------------
    //         Update FIELD       
    // -----------------------------
    const std::string name = std::string(cf->field_name);
    auto buildFieldMeta =
        [] (const std::string name, Create_field *cf,
            const ProxyState &ps, TableMeta *tm)
    {
        if (Field::NEXT_NUMBER == cf->unireg_check) {
            return new FieldMeta(name, cf, NULL, SECURITY_RATING::PLAIN,
                                 tm->leaseIncUniq());
        } else {
            return new FieldMeta(name, cf, ps.masterKey,
                                 ps.defaultSecurityRating(),
                                 tm->leaseIncUniq());
        }
    };
    FieldMeta * const fm = buildFieldMeta(name, cf, ps, tm);
    // Here we store the key name for the first time. It will be applied
    // after the Delta is read out of the database.
    if (true == new_table) {
        tm->addChild(new IdentityMetaKey(name), fm);
    } else {
        a.deltas.push_back(new CreateDelta(fm, tm,
                                           new IdentityMetaKey(name)));
        a.deltas.push_back(new ReplaceDelta(tm, a.getSchema(),
                                            a.getSchema()->getKey(tm)));
    }

    // -----------------------------
    //         Rewrite FIELD       
    // -----------------------------
    auto new_fields = rewrite_create_field(fm, cf, a);
    rewritten_cfield_list.concat(vectorToList(new_fields));

    return rewritten_cfield_list;
}

//TODO: which encrypt/decrypt should handle null?
Item *
encrypt_item_layers(Item * i, onion o, OnionMeta * const om,
                    const Analysis &a, uint64_t IV) {
    assert(!i->is_null());

    const auto enc_layers = a.getEncLayers(om);
    assert_s(enc_layers.size() > 0, "onion must have at least one layer");
    Item * enc = i;
    Item * prev_enc = NULL;
    auto stringEscaper = a.getStringEscaper();
    for (auto layer : enc_layers) {
        LOG(encl) << "encrypt layer "
                  << TypeText<SECLEVEL>::toText(layer->level()) << "\n";
        enc = layer->encrypt(enc, IV, stringEscaper);
        //need to free space for all enc
        //except the last one
        if (prev_enc) {
            delete prev_enc;
        }
        prev_enc = enc;
    }

    return enc;
}

std::string
rewriteAndGetSingleQuery(const ProxyState &ps, const std::string &q)
{
    Rewriter r;
    SchemaInfo *out_schema;
    QueryRewrite qr = r.rewrite(ps, q, &out_schema);
    assert(false == qr.output->stalesSchema());
    assert(false == qr.output->queryAgain());
    
    std::list<std::string> out_queryz;
    if (!qr.output->getQuery(&out_queryz)) {
        throw CryptDBError("Failed to retrieve query!");
    }
    assert(out_queryz.size() == 1);

    return out_queryz.back();
}

std::string
escapeString(Connect * const e_conn, const std::string &escape_me)
{
    const unsigned int escaped_length = escape_me.size() * 2 + 1;
    std::unique_ptr<char> escaped(new char[escaped_length]);
    e_conn->real_escape_string(escaped.get(), escape_me.c_str(),
                               escape_me.size());

    const std::string out = std::string(escaped.get());

    return out;
}

