#include <memory>

#include <main/rewrite_util.hh>
#include <main/enum_text.hh>
#include <main/rewrite_main.hh>
#include <main/macro_util.hh>
#include <parser/lex_util.hh>
#include <parser/stringify.hh>

extern CItemTypesDir itemTypes;

void
optimize(Item ** const i, Analysis &a) {
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
rewrite(Item *const i, const EncSet &req_enc, Analysis &a)
{
    RewritePlan *const rp = getAssert(a.rewritePlans, i);
    assert(rp);
    const EncSet solution = rp->es_out.intersect(req_enc);
    // FIXME: Use version that takes reason, expects 0 children,
    // and lets us indicate what our EncSet does have.
    TEST_NoAvailableEncSet(solution, i->type(), req_enc, rp->r.why_t,
                           NULL, 0);

    return itemTypes.do_rewrite(i, solution.chooseOne(), rp, a);
}

TABLE_LIST *
rewrite_table_list(const TABLE_LIST * const t, const Analysis &a)
{
    // Table name can only be empty when grouping a nested join.
    assert(t->table_name || t->nested_join);
    if (t->table_name) {
        const std::string plain_name =
            std::string(t->table_name, t->table_name_length);
        // Don't use Analysis::getAnonTableName(...) as it respects
        // aliases and if a table has been aliased with 'plain_name'
        // it will give us the wrong name.
        const std::string anon_name =
            a.translateNonAliasPlainToAnonTableName(plain_name);
        return rewrite_table_list(t, anon_name);
    } else {
        return copyWithTHD(t);
    }
}

TABLE_LIST *
rewrite_table_list(const TABLE_LIST * const t,
                   const std::string &anon_name)
{
    TABLE_LIST *const new_t = copyWithTHD(t);
    new_t->table_name = make_thd_string(anon_name);
    new_t->table_name_length = anon_name.size();
    if (false == new_t->is_alias) {
        new_t->alias = make_thd_string(anon_name);
    }
    new_t->next_local = NULL;

    return new_t;
}

// @if_exists: defaults to false; it is necessary to facilitate
//   'DROP TABLE IF EXISTS'
SQL_I_List<TABLE_LIST>
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a,
                   bool if_exists)
{
    if (!tlist.elements) {
        return SQL_I_List<TABLE_LIST>();
    }

    TABLE_LIST * tl;
    if (if_exists && !a.nonAliasTableMetaExists(tlist.first->table_name)) {
       tl = copyWithTHD(tlist.first);
    } else {
       tl = rewrite_table_list(tlist.first, a);
    }

    const SQL_I_List<TABLE_LIST> * const new_tlist =
        oneElemListWithTHD<TABLE_LIST>(tl);

    TABLE_LIST * prev = tl;
    for (TABLE_LIST *tbl = tlist.first->next_local; tbl;
         tbl = tbl->next_local) {
        TABLE_LIST * new_tbl;
        if (if_exists && !a.nonAliasTableMetaExists(tbl->table_name)) {
            new_tbl = copyWithTHD(tbl);
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
rewrite_table_list(List<TABLE_LIST> tll, Analysis &a)
{
    List<TABLE_LIST> * const new_tll = new List<TABLE_LIST>();

    List_iterator<TABLE_LIST> join_it(tll);

    for (;;) {
        const TABLE_LIST * const t = join_it++;
        if (!t) {
            break;
        }

        TABLE_LIST * const new_t = rewrite_table_list(t, a);
        new_tll->push_back(new_t);

        if (t->nested_join) {
            new_t->nested_join->join_list =
                rewrite_table_list(t->nested_join->join_list, a);
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
gather(Item * const i, reason &tr, Analysis &a)
{
    return itemTypes.do_gather(i, tr, a);
}

//TODO: need to check somewhere that plain is returned
//TODO: Put in gather helpers file.
void
analyze(Item * const i, Analysis &a)
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
get_create_field(const Analysis &a, Create_field * const f,
                 const OnionMeta &om)
{
    const std::string name = om.getAnonOnionName();
    Create_field *new_cf = f;

    // Default value is handled during INSERTion.
    auto save_default = f->def;
    f->def = NULL;

    const auto &enc_layers = a.getEncLayers(om);
    assert(enc_layers.size() > 0);
    for (auto it = enc_layers.begin(); it != enc_layers.end(); it++) {
        const Create_field * const old_cf = new_cf;
        new_cf = (*it)->newCreateField(old_cf, name);

        if (old_cf != f) {
            delete old_cf;
        }
    }

    // Restore the default so we don't memleak it.
    f->def = save_default;
    return new_cf;
}

// NOTE: The fields created here should have NULL default pointers
// as such is handled during INSERTion.
std::vector<Create_field *>
rewrite_create_field(const FieldMeta * const fm,
                     Create_field * const f, const Analysis &a)
{
    LOG(cdb_v) << "in rewrite create field for " << *f;

    assert(fm->children.size() > 0);

    std::vector<Create_field *> output_cfields;

    // Don't add the default value to the schema.
    Item *const save_def = f->def;
    f->def = NULL;

    // create each onion column
    for (auto oit : fm->orderedOnionMetas()) {
        OnionMeta * const om = oit.second;
        Create_field * const new_cf = get_create_field(a, f, *om);

        output_cfields.push_back(new_cf);
    }

    // create salt column
    if (fm->has_salt) {
        THD * const thd         = current_thd;
        Create_field * const f0 = f->clone(thd->mem_root);
        f0->field_name          = thd->strdup(fm->getSaltName().c_str());
        // Salt is unsigned and is not AUTO_INCREMENT.
        // > NOT_NULL_FLAG is useful for debugging if mysql strict mode
        //   (ie, STRICT_ALL_TABLES) is turned on.
        f0->flags               =
            (f0->flags | UNSIGNED_FLAG | NOT_NULL_FLAG)
            & ~AUTO_INCREMENT_FLAG;
        f0->sql_type            = MYSQL_TYPE_LONGLONG;
        f0->length              = 8;

        output_cfields.push_back(f0);
    }

    // Restore the default to the original Create_field parameter.
    f->def = save_def;

    return output_cfields;
}

std::vector<onion>
getOnionIndexTypes()
{
    return std::vector<onion>({oOPE, oDET, oPLAIN});
}

std::vector<Key *>
rewrite_key(const TableMeta &tm, Key *const key, const Analysis &a)
{
    std::vector<Key *> output_keys;

    const std::vector<onion> key_onions = getOnionIndexTypes();
    for (auto onion_it : key_onions) {
        const onion o = onion_it;
        Key *const new_key = key->clone(current_thd->mem_root);

        // Set anonymous name.
        const std::string new_name =
            a.getAnonIndexName(tm, convert_lex_str(key->name), o);
        new_key->name = string_to_lex_str(new_name);

        // Set anonymous columns.
        const auto col_it =
            List_iterator<Key_part_spec>(key->columns);
        // HACK: Determine if we succeed in creating the INDEX on
        // each onion.
        bool fail = false;
        new_key->columns =
            accumList<Key_part_spec>(col_it,
                [o, &tm, &a, &fail] (List<Key_part_spec> out_field_list,
                                     Key_part_spec *const key_part)
                {
                    Key_part_spec *const new_key_part =
                        copyWithTHD(key_part);
                    const std::string field_name =
                        convert_lex_str(new_key_part->field_name);
                    const FieldMeta &fm = a.getFieldMeta(tm, field_name);
                    const OnionMeta *const om = fm.getOnionMeta(o);
                    if (NULL == om) {
                        fail = true;
                        return out_field_list;  /* lambda */
                    }

                    new_key_part->field_name =
                        string_to_lex_str(om->getAnonOnionName());
                    out_field_list.push_back(new_key_part);
                    return out_field_list; /* lambda */
                });
        if (false == fail) {
            output_keys.push_back(new_key);
        }
    }

    // Only create one PRIMARY KEY.
    if (Key::PRIMARY == key->type) {
        if (output_keys.size() > 0) {
            return std::vector<Key *>({output_keys.front()});
        } else {
            return std::vector<Key *>();
        }
    }

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
string_to_bool(const std::string &s)
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
                      Create_field * const cf,
                      TableMeta *const tm, bool new_table,
                      List<Create_field> &rewritten_cfield_list)
{
    const std::string name = std::string(cf->field_name);
    auto buildFieldMeta =
        [] (const std::string name, Create_field * const cf,
            const ProxyState &ps, TableMeta *const tm)
    {
        return new FieldMeta(name, cf, ps.masterKey,
                             ps.defaultSecurityRating(),
                             tm->leaseIncUniq());
    };
    std::unique_ptr<FieldMeta> fm(buildFieldMeta(name, cf, ps, tm));

    // -----------------------------
    //         Rewrite FIELD       
    // -----------------------------
    const auto new_fields = rewrite_create_field(fm.get(), cf, a);
    rewritten_cfield_list.concat(vectorToListWithTHD(new_fields));

    // -----------------------------
    //         Update FIELD       
    // -----------------------------

    // Here we store the key name for the first time. It will be applied
    // after the Delta is read out of the database.
    if (true == new_table) {
        tm->addChild(IdentityMetaKey(name), std::move(fm));
    } else {
        a.deltas.push_back(new CreateDelta(std::move(fm), *tm,
                                           new IdentityMetaKey(name)));
        a.deltas.push_back(new ReplaceDelta(*tm, a.getSchema()));
    }

    return rewritten_cfield_list;
}

//TODO: which encrypt/decrypt should handle null?
Item *
encrypt_item_layers(Item * const i, onion o, const OnionMeta &om,
                    const Analysis &a, uint64_t IV) {
    assert(!i->is_null());

    const auto &enc_layers = a.getEncLayers(om);
    assert_s(enc_layers.size() > 0, "onion must have at least one layer");
    Item * enc = i;
    Item * prev_enc = NULL;
    for (auto it = enc_layers.begin(); it != enc_layers.end(); it++) {
        LOG(encl) << "encrypt layer "
                  << TypeText<SECLEVEL>::toText((*it)->level()) << "\n";
        enc = (*it)->encrypt(enc, IV);
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
rewriteAndGetSingleQuery(const ProxyState &ps, const std::string &q,
                         SchemaInfo const &schema)
{
    const QueryRewrite qr(Rewriter::rewrite(ps, q, schema));
    assert(false == qr.output->stalesSchema());
    assert(false == qr.output->queryAgain());

    std::list<std::string> out_queryz;
    if (!qr.output->getQuery(&out_queryz, schema)) {
        throw CryptDBError("Failed to retrieve query!");
    }
    assert(out_queryz.size() == 1);

    return out_queryz.back();
}

std::string
escapeString(const std::unique_ptr<Connect> &c,
             const std::string &escape_me)
{
    const unsigned int escaped_length = escape_me.size() * 2 + 1;
    std::unique_ptr<char, void (*)(void *)>
        escaped(new char[escaped_length], &operator delete []);
    c->real_escape_string(escaped.get(), escape_me.c_str(),
                          escape_me.size());

    return std::string(escaped.get());
}

void
encrypt_item_all_onions(Item *i, FieldMeta *fm,
                        uint64_t IV, std::vector<Item*> &l,
                        Analysis &a)
{
    for (auto it : fm->orderedOnionMetas()) {
        const onion o = it.first->getValue();
        OnionMeta * const om = it.second;
        l.push_back(encrypt_item_layers(i, o, *om, a, IV));
    }
}

void
typical_rewrite_insert_type(Item *const i, Analysis &a,
                            std::vector<Item *> &l, FieldMeta *const fm)
{
    const uint64_t salt = fm->has_salt ? randomValue() : 0;

    encrypt_item_all_onions(i, fm, salt, l, a);

    if (fm->has_salt) {
        l.push_back(new Item_int(static_cast<ulonglong>(salt)));
    }
}

std::string
mysql_noop()
{
    return "do 0;";
}

PREAMBLE_STATUS
queryPreamble(const ProxyState &ps, const std::string &q,
              std::unique_ptr<QueryRewrite> *const qr,
              std::list<std::string> *const out_queryz,
              SchemaInfo const &schema)
{
    *qr = std::unique_ptr<QueryRewrite>(
            new QueryRewrite(Rewriter::rewrite(ps, q, schema)));

    assert((*qr)->output->beforeQuery(ps.getConn(), ps.getEConn()));

    if (!(*qr)->output->getQuery(out_queryz, schema)) {
        (*qr)->output->handleQueryFailure(ps.getEConn());
        return PREAMBLE_STATUS::FAILURE;
    }

    MaxOneReadPerAssign<bool> did_rollback(false);
    switch ((*qr)->output->queryChannel()) {
        // Must detect the side channel deadlock and reissue the side
        // query before the transaction it is deadlocking with.
        case RewriteOutput::Channel::SIDE: {
            unsigned int const max_attempts = 5;
            for (unsigned int attempts = 0; attempts < max_attempts;
                 ++attempts) {
                MaxOneReadPerAssign<bool> side_channel_success(true);
                for (auto it : *out_queryz) {
                    if (!ps.getSideChannelConn()->execute(it)) {
                        unsigned int const err =
                            ps.getSideChannelConn()->get_mysql_errno();
                        // Force the regular mysql connection to ROLLBACK
                        // and retry onion adjustment if we timed out.
                        if (ER_LOCK_WAIT_TIMEOUT == err) {
                            assert(false == did_rollback.get());
                            // > Possibly violates
                            //   innodb_rollback_on_timeout=FALSE (default)
                            // > Doesn't work for proxy.
                            assert(ps.getSideChannelConn()->execute("ROLLBACK;"));
                            assert((*qr)->output->handleQueryFailure(ps.getEConn()));
                            did_rollback = true;
                            goto side_channel_epilogue;
                        }

                        (*qr)->output->handleQueryFailure(ps.getEConn());
                        return PREAMBLE_STATUS::FAILURE;
                    }
                }
                break;
            }

side_channel_epilogue:
            out_queryz->clear();
            // HACK: The caller will use the metadata
            // associated with AdjustOnionOutput.
            // > Considering that the metadata will tell us to
            // issue the query again, all is well. But if such
            // weren't the case we _could_ get a mismatch
            // between the metadata and the noop query.
            if (true == did_rollback.get()) {
                out_queryz->push_back("ROLLBACK");
                return PREAMBLE_STATUS::ROLLBACK;
            }
            
            out_queryz->push_back(mysql_noop());
            break;
        }
        case RewriteOutput::Channel::REGULAR:
            break;
        default:
            return PREAMBLE_STATUS::FAILURE;
    }

    return PREAMBLE_STATUS::SUCCESS;
}

bool
queryHandleRollback(const ProxyState &ps, const std::string &query,
                    SchemaInfo const &schema)
{
    std::unique_ptr<QueryRewrite> qr;
    std::list<std::string> out_queryz;
    PREAMBLE_STATUS const preamble_status =
        queryPreamble(ps, query, &qr, &out_queryz, schema);
    assert(qr);
    if (PREAMBLE_STATUS::FAILURE == preamble_status) {
        return false;
    }

    assert(PREAMBLE_STATUS::SUCCESS == preamble_status);
    assert(qr->output->afterQuery(ps.getEConn()));

    return true;
}

/*
static void
printEC(std::unique_ptr<Connect> e_conn, const std::string & command) {
    DBResult * dbres;
    assert_s(e_conn->execute(command, dbres), "command failed");
    ResType res = dbres->unpack();
    printRes(res);
}
*/

static void
printEmbeddedState(const ProxyState &ps) {
/*
    printEC(ps.e_conn, "show databases;");
    printEC(ps.e_conn, "show tables from pdb;");
    std::cout << "regular" << std::endl << std::endl;
    printEC(ps.e_conn, "select * from pdb.MetaObject;");
    std::cout << "bleeding" << std::endl << std::endl;
    printEC(ps.e_conn, "select * from pdb.BleedingMetaObject;");
    printEC(ps.e_conn, "select * from pdb.Query;");
    printEC(ps.e_conn, "select * from pdb.DeltaOutput;");
*/
}


void
prettyPrintQuery(const std::string &query)
{
    std::cout << std::endl << RED_BEGIN
              << "QUERY: " << COLOR_END << query << std::endl;
}

static void
prettyPrintQueryResult(ResType res)
{
    std::cout << std::endl << RED_BEGIN
              << "RESULTS: " << COLOR_END << std::endl;
    printRes(res);
    std::cout << std::endl;
}

ResType *
queryEpilogue(const ProxyState &ps, const QueryRewrite &qr,
              ResType *const res, const std::string &query, bool pp)
{
    assert(qr.output->afterQuery(ps.getEConn()));

    if (qr.output->queryAgain()) {
        return executeQuery(ps, query);
    }

    if (pp) {
        printEmbeddedState(ps);
        prettyPrintQueryResult(*res);
    }

    if (qr.output->doDecryption()) {
        ResType *const dec_res =
            Rewriter::decryptResults(*res, qr.rmeta);
        if (pp) {
            prettyPrintQueryResult(*dec_res);
        }

        return dec_res;
    }

    return res;
}

const SchemaInfo &
SchemaCache::getSchema(const std::unique_ptr<Connect> &conn,
                       const std::unique_ptr<Connect> &e_conn)
{
    if (true == staleness) {
        this->schema.reset(loadSchemaInfo(conn, e_conn));
    }
    assert(this->schema);

    return *this->schema.get();
}

void SchemaCache::updateStaleness(bool staleness)
{
    this->staleness = staleness;
}

