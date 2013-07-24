#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <typeinfo>

#include <main/rewrite_main.hh>
#include <main/rewrite_util.hh>
#include <util/cryptdb_log.hh>
#include <main/CryptoHandlers.hh>
#include <parser/lex_util.hh>
#include <main/enum_text.hh>
#include <main/sql_handler.hh>
#include <main/dml_handler.hh>
#include <main/ddl_handler.hh>
#include <main/List_helpers.hh>

#include "field.h"

extern CItemTypesDir itemTypes;
extern CItemFuncDir funcTypes;
extern CItemSumFuncDir sumFuncTypes;
extern CItemFuncNameDir funcNames;

#define ANON                ANON_NAME(__anon_id_)


//TODO: potential inconsistency problem because we update state,
//but only the proxy is responsible for WRT to updateMeta

//TODO: use getAssert in more places
//TODO: replace table/field with FieldMeta * for speed and conciseness

//TODO: rewrite_proj may not need to be part of each class;
// it just does gather, choos and then rewrite

static Item_field *
stringToItemField(std::string field, std::string table, Item_field * itf) {

    THD * thd = current_thd;
    assert(thd);
    Item_field * res = new Item_field(thd, itf);
    res->name = NULL; //no alias
    res->field_name = make_thd_string(field);
    res->table_name = make_thd_string(table);

    return res;
}

static inline std::string
extract_fieldname(Item_field *i)
{
    std::stringstream fieldtemp;
    fieldtemp << *i;
    return fieldtemp.str();
}


//TODO: remove this at some point
static inline void
mysql_query_wrapper(MYSQL *m, const std::string &q)
{
    if (mysql_query(m, q.c_str())) {
        cryptdb_err() << "query failed: " << q
                << " reason: " << mysql_error(m);
    }

    // HACK(stephentu):
    // Calling mysql_query seems to have destructive effects
    // on the current_thd. Thus, we must call create_embedded_thd
    // again.
    void* ret = create_embedded_thd(0);
    if (!ret) assert(false);
}

bool
sanityCheck(FieldMeta *fm)
{
    for (auto it : fm->children) {
        OnionMeta *om = it.second;
        onion o = it.first->getValue();
        const std::vector<SECLEVEL> &secs = fm->onion_layout.at(o);
        for (unsigned int i = 0; i < om->layers.size(); ++i) {
            EncLayer *layer = om->layers[i];
            assert(layer->level() == secs[i]);
        }
    }
    return true;
}

static bool
sanityCheck(TableMeta *tm)
{
    for (auto it : tm->children) {
        FieldMeta *fm = it.second;
        assert(sanityCheck(fm));
    }
    return true;
}

static bool
sanityCheck(SchemaInfo *schema)
{
    for (auto it : schema->children) {
        TableMeta *tm = it.second;
        assert(sanityCheck(tm));
    }
    return true;
}

// FIXME: This function will not build all of our tables when it is run
// on an empty database.  If you don't have a parent, your table won't be
// built.  We probably want to seperate our database logic into 3 parts.
//  1> Schema buildling (CREATE TABLE IF NOT EXISTS...)
//  2> INSERTing
//  3> SELECTing
static SchemaInfo *
loadSchemaInfo(Connect *e_conn)
{
    SchemaInfo *schema = new SchemaInfo(); 
    // Recursively rebuild the AbstractMeta<Whatever> and it's children.
    std::function<DBMeta*(DBMeta *)> loadChildren =
        [&loadChildren, &e_conn](DBMeta *parent) {
            auto kids = parent->fetchChildren(e_conn);
            for (auto it : kids) {
                loadChildren(it);
            }

            return parent;  /* lambda */
        };

     loadChildren(schema);
     assert(sanityCheck(schema));
     return schema;
}

static void
printEC(Connect * e_conn, const std::string & command) {
    DBResult * dbres;
    assert_s(e_conn->execute(command, dbres), "command failed");
    ResType res = dbres->unpack();
    printRes(res);
}

static void
printEmbeddedState(ProxyState & ps) {
    printEC(ps.e_conn, "show databases;");
    printEC(ps.e_conn, "show tables from pdb;");
    printEC(ps.e_conn, "selecT * from pdb.tableMeta_schemaInfo;");
    printEC(ps.e_conn, "select * from pdb.tableMeta;");
    printEC(ps.e_conn, "selecT * from pdb.fieldMeta_tableMeta;");
    printEC(ps.e_conn, "select * from pdb.fieldMeta;");
    printEC(ps.e_conn, "selecT * from pdb.onionMeta_fieldMeta;");
    printEC(ps.e_conn, "select * from pdb.onionMeta;");
    printEC(ps.e_conn, "selecT * from pdb.encLayer_onionMeta;");
    printEC(ps.e_conn, "select * from pdb.encLayer;");
}

template <typename type> static void
translatorHelper(const char **texts, type *enums, int count)
{
    std::vector<type> vec_enums(count);
    std::vector<std::string> vec_texts(count);

    for (int i = 0; i < count; ++i) {
        vec_texts[i] = texts[i];
        vec_enums[i] = enums[i];
    }

    TypeText<type>::addSet(vec_enums, vec_texts);
}

#define arraysize(a) (sizeof(a)/sizeof(a[0]))

static void
buildTypeTextTranslator()
{
    // Onions.
    const char *onion_chars[] = {"oPLAIN", "oDET", "oOPE", "oAGG", "oSWP"};
    onion onions[] = {oPLAIN, oDET, oOPE, oAGG, oSWP};
    assert(arraysize(onion_chars) == arraysize(onions));
    int count = arraysize(onion_chars);
    translatorHelper((const char **)onion_chars, (onion *)onions, count);

    // SecLevels.
    const char *seclevel_chars[] = {"RND", "DET", "DETJOIN", "OPE", "HOM",
                                    "SEARCH", "PLAINVAL"};
    SECLEVEL seclevels[] = {SECLEVEL::RND, SECLEVEL::DET,
                            SECLEVEL::DETJOIN, SECLEVEL::OPE,
                            SECLEVEL::HOM, SECLEVEL::SEARCH,
                            SECLEVEL::PLAINVAL};
    assert(arraysize(seclevel_chars) == arraysize(seclevels));
    count = arraysize(seclevel_chars);
    translatorHelper((const char **)seclevel_chars, (SECLEVEL *)seclevels,
                     count);

    // MYSQL types.
    const char *mysql_type_chars[] =
    {
        "MYSQL_TYPE_BIT", "MYSQL_TYPE_BLOB", "MYSQL_TYPE_DATE",
        "MYSQL_TYPE_DATETIME", "MYSQL_TYPE_DECIMAL", "MYSQL_TYPE_DOUBLE",
        "MYSQL_TYPE_ENUM", "MYSQL_TYPE_FLOAT", "MYSQL_TYPE_GEOMETRY",
        "MYSQL_TYPE_INT24", "MYSQL_TYPE_LONG", "MYSQL_TYPE_LONG_BLOB",
        "MYSQL_TYPE_LONGLONG", "MYSQL_TYPE_MEDIUM_BLOB",
        "MYSQL_TYPE_NEWDATE", "MYSQL_TYPE_NEWDECIMAL", "MYSQL_TYPE_NULL",
        "MYSQL_TYPE_SET", "MYSQL_TYPE_SHORT", "MYSQL_TYPE_STRING",
        "MYSQL_TYPE_TIME", "MYSQL_TYPE_TIMESTAMP", "MYSQL_TYPE_TINY",
        "MYSQL_TYPE_TINY_BLOB", "MYSQL_TYPE_VAR_STRING",
        "MYSQL_TYPE_VARCHAR", "MYSQL_TYPE_YEAR"
    };
    enum enum_field_types mysql_types[] =
    {
        MYSQL_TYPE_BIT, MYSQL_TYPE_BLOB, MYSQL_TYPE_DATE,
        MYSQL_TYPE_DATETIME, MYSQL_TYPE_DECIMAL, MYSQL_TYPE_DOUBLE,
        MYSQL_TYPE_ENUM, MYSQL_TYPE_FLOAT, MYSQL_TYPE_GEOMETRY,
        MYSQL_TYPE_INT24, MYSQL_TYPE_LONG, MYSQL_TYPE_LONG_BLOB,
        MYSQL_TYPE_LONGLONG, MYSQL_TYPE_MEDIUM_BLOB,
        MYSQL_TYPE_NEWDATE, MYSQL_TYPE_NEWDECIMAL, MYSQL_TYPE_NULL,
        MYSQL_TYPE_SET, MYSQL_TYPE_SHORT, MYSQL_TYPE_STRING,
        MYSQL_TYPE_TIME, MYSQL_TYPE_TIMESTAMP, MYSQL_TYPE_TINY,
        MYSQL_TYPE_TINY_BLOB, MYSQL_TYPE_VAR_STRING,
        MYSQL_TYPE_VARCHAR, MYSQL_TYPE_YEAR
    };
    assert(arraysize(mysql_type_chars) == arraysize(mysql_types));
    count = arraysize(mysql_type_chars);
    translatorHelper((const char **)mysql_type_chars,
                     (enum enum_field_types *)mysql_types, count);

    // Onion Layouts.
    const char *onion_layout_chars[] =
    {
        "PLAIN_ONION_LAYOUT", "NUM_ONION_LAYOUT", "MP_NUM_ONION_LAYOUT",
        "STR_ONION_LAYOUT"
    };
    onionlayout onion_layouts[] =
    {
        PLAIN_ONION_LAYOUT, NUM_ONION_LAYOUT, MP_NUM_ONION_LAYOUT,
        STR_ONION_LAYOUT
    };
    assert(arraysize(onion_layout_chars) == arraysize(onion_layouts));
    count = arraysize(onion_layout_chars);
    translatorHelper((const char **)onion_layout_chars,
                     (onionlayout *)onion_layouts, count);

    // Geometry type.
    const char *geometry_type_chars[] =
    {
        "GEOM_GEOMETRY", "GEOM_POINT", "GEOM_LINESTRING", "GEOM_POLYGON",
        "GEOM_MULTIPOINT", "GEOM_MULTILINESTRING", "GEOM_MULTIPOLYGON",
        "GEOM_GEOMETRYCOLLECTION"
    };
    Field::geometry_type geometry_types[] =
    {
        Field::GEOM_GEOMETRY, Field::GEOM_POINT, Field::GEOM_LINESTRING,
        Field::GEOM_POLYGON, Field::GEOM_MULTIPOINT,
        Field::GEOM_MULTILINESTRING, Field::GEOM_MULTIPOLYGON,
        Field::GEOM_GEOMETRYCOLLECTION
    };
    assert(arraysize(geometry_type_chars) == arraysize(geometry_types));
    count = arraysize(geometry_type_chars);
    translatorHelper((const char **)geometry_type_chars,
                    (Field::geometry_type *)geometry_types, count);

    return;
}

//l gets updated to the new level
static void
removeOnionLayer(Analysis &a, const ProxyState &ps, FieldMeta * fm,
                 Item_field *itf, onion o, SECLEVEL *new_level,
                 const std::string &cur_db) {
    OnionMeta *om = fm->getOnionMeta(o);
    assert(om);
    std::string fieldanon  = om->getAnonOnionName();
    std::string tableanon  =
        a.getTableMeta(itf->table_name)->getAnonTableName();

    //removes onion layer at the DB
    std::stringstream query;
    query << " UPDATE " << cur_db << "." << tableanon
          << "    SET " << fieldanon  << " = ";

    Item_field *field = stringToItemField(fieldanon, tableanon, itf);
    Item_field *salt = stringToItemField(fm->getSaltName(), tableanon, itf);
    Item * decUDF = a.getBackEncLayer(om)->decryptUDF(field, salt);

    query << *decUDF << ";";

    std::cerr << "\nADJUST: \n" << query.str() << "\n";

    //execute decryption query
    assert_s(ps.conn->execute(query.str()), "failed to execute onion decryption query");

    LOG(cdb_v) << "adjust onions: \n" << query.str() << "\n";

    //remove onion layer in schema
    Delta d(Delta::DELETE, a.popBackEncLayer(om), om, NULL);	
    a.deltas.push_back(d);

    *new_level = a.getOnionLevel(om);
}

/*
 * Adjusts the onion for a field fm/itf to level: tolevel.
 *
 * Issues queries for decryption to the DBMS.
 *
 * Adjusts the schema metadata at the proxy about onion layers. Propagates the
 * changed schema to persistent storage.
 *
 */
static void
adjustOnion(Analysis &a, const ProxyState &ps, onion o, FieldMeta * fm,
            SECLEVEL tolevel, Item_field *itf, const std::string &cur_db)
{
    OnionMeta *om = fm->getOnionMeta(o);
    SECLEVEL newlevel = a.getOnionLevel(om);
    assert(newlevel != SECLEVEL::INVALID);

    while (newlevel > tolevel) {
        removeOnionLayer(a, ps, fm, itf, o, &newlevel, cur_db);
    }
    assert(newlevel == tolevel);
}
//TODO: propagate these adjustments in the embedded database?

static inline bool
FieldQualifies(const FieldMeta * restriction,
               const FieldMeta * field)
{
    return !restriction || restriction == field;
}

template <class T>
static Item *
do_optimize_const_item(T *i, Analysis &a) {

    return i;

    /* TODO for later
    if (i->const_item()) {
        // ask embedded DB to eval this const item,
        // then replace this item with the eval-ed constant
        //
        // WARNING: we must make sure that the primitives like
        // int literals, string literals, override this method
        // and not ask the server.

        // very hacky...
        stringstream buf;
        buf << "SELECT " << *i;
        string q(buf.str());
        LOG(cdb_v) << q;

	DBResult * dbres = NULL;
	assert(a.ps->e_conn->execute(q, dbres));

        THD *thd = current_thd;
        assert(thd != NULL);

        MYSQL_RES *r = dbres->n;
        if (r) {
            Item *rep = NULL;

            assert(mysql_num_rows(r) == 1);
            assert(mysql_num_fields(r) == 1);

            MYSQL_FIELD *field = mysql_fetch_field_direct(r, 0);
            assert(field != NULL);

            MYSQL_ROW row = mysql_fetch_row(r);
            assert(row != NULL);

            char *p = row[0];
            unsigned long *lengths = mysql_fetch_lengths(r);
            assert(lengths != NULL);
            if (p) {

                LOG(cdb_v) << "p: " << p;
                LOG(cdb_v) << "field->type: " << field->type;

                switch (field->type) {
                    case MYSQL_TYPE_SHORT:
                    case MYSQL_TYPE_LONG:
                    case MYSQL_TYPE_LONGLONG:
                    case MYSQL_TYPE_INT24:
                        rep = new Item_int((long long) strtoll(p, NULL, 10));
                        break;
                    case MYSQL_TYPE_FLOAT:
                    case MYSQL_TYPE_DOUBLE:
                        rep = new Item_float(p, lengths[0]);
                        break;
                    case MYSQL_TYPE_DECIMAL:
                    case MYSQL_TYPE_NEWDECIMAL:
                        rep = new Item_decimal(p, lengths[0], i->default_charset());
                        break;
                    case MYSQL_TYPE_VARCHAR:
                    case MYSQL_TYPE_VAR_STRING:
                        rep = new Item_string(thd->strdup(p),
                                              lengths[0],
                                              i->default_charset());
                        break;
                    default:
                        // TODO(stephentu): implement the rest of the data types
                        break;
                }
            } else {
                // this represents NULL
                rep = new Item_null();
            }
            mysql_free_result(r);
            if (rep != NULL) {
                rep->name = i->name;
                return rep;
            }
        } else {
            // some error in dealing with the DB
            LOG(warn) << "could not retrieve result set";
        }
    }
    return i;

    */
}

Item *
decrypt_item_layers(Item * i, onion o, OnionMeta *om, uint64_t IV,
                    FieldMeta *fm, const std::vector<Item *> &res)
{
    assert(!i->is_null());

    if (o == oPLAIN) {// Unencrypted item
        return i;
    }

    // Encrypted item

    Item * dec = i;
    Item * prev_dec = NULL;

    auto enc_layers = om->layers;
    for (auto it = enc_layers.rbegin(); it != enc_layers.rend(); ++it) {
        dec = (*it)->decrypt(dec, IV);
        LOG(cdb_v) << "dec okay";
        //need to free space for all decs except last
        if (prev_dec) {
            delete prev_dec;
        }
        prev_dec = dec;
    }

    return dec;
}

static Item *
decrypt_item(FieldMeta * fm, onion o, Item * i, uint64_t IV,
             std::vector<Item *> &res)
{
    assert(!i->is_null());
    return decrypt_item_layers(i, o, fm->getOnionMeta(o), IV, fm, res);
}


// returns the intersection of the es and fm.encdesc
// by also taking into account what onions are stale
// on fm
/*static OnionLevelFieldMap
intersect(const EncSet & es, FieldMeta * fm) {
    OnionLevelFieldMap res;

    for (auto it : es.osl) {
        onion o = it.first;
        auto ed_it = fm->encdesc.olm.find(o);
        if ((ed_it != fm->encdesc.olm.end()) && (!fm->onions[o]->stale)) {
            //an onion to keep
            res[o] = LevelFieldPair(min(it.second.first, ed_it->second), fm);
        }
    }

    return res;
}
*/
/*
 * Actual item handlers.
 */
static void optimize_select_lex(st_select_lex *select_lex, Analysis & a);

static class ANON : public CItemSubtypeIT<Item_subselect, Item::Type::SUBSELECT_ITEM> {
    virtual RewritePlan * do_gather_type(Item_subselect *i, reason &tr, Analysis & a) const
    {
        /*
        st_select_lex *select_lex = i->get_select_lex();
        process_select_lex(select_lex, a);
        return tr.encset;*/
        UNIMPLEMENTED;
        return NULL;
    }
    virtual Item * do_optimize_type(Item_subselect *i, Analysis & a) const {
        optimize_select_lex(i->get_select_lex(), a);
        return i;
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_cache, Item::Type::CACHE_ITEM> {
    virtual RewritePlan * do_gather_type(Item_cache *i, reason &tr, Analysis & a) const
    {
        /*
        Item *example = i->*rob<Item_cache, Item*, &Item_cache::example>::ptr();
        if (example)
            return gather(example, tr, a);
        return tr.encset;*/
        UNIMPLEMENTED;
        return NULL;
    }
    virtual Item * do_optimize_type(Item_cache *i, Analysis & a) const
    {
        // TODO(stephentu): figure out how to use rob here
        return i;
    }
} ANON;

/*
 * Some helper functions.
 */

static void
optimize_select_lex(st_select_lex *select_lex, Analysis & a)
{
    auto item_it = List_iterator<Item>(select_lex->item_list);
    for (;;) {
        if (!item_it++)
            break;
        optimize(item_it.ref(), a);
    }

    if (select_lex->where)
        optimize(&select_lex->where, a);

    if (select_lex->join &&
        select_lex->join->conds &&
        select_lex->where != select_lex->join->conds)
        optimize(&select_lex->join->conds, a);

    if (select_lex->having)
        optimize(&select_lex->having, a);

    for (ORDER *o = select_lex->group_list.first; o; o = o->next)
        optimize(o->item, a);
    for (ORDER *o = select_lex->order_list.first; o; o = o->next)
        optimize(o->item, a);
}

static void
optimize_table_list(List<TABLE_LIST> *tll, Analysis &a)
{
    List_iterator<TABLE_LIST> join_it(*tll);
    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t)
            break;

        if (t->nested_join) {
            optimize_table_list(&t->nested_join->join_list, a);
            return;
        }

        if (t->on_expr)
            optimize(&t->on_expr, a);

        if (t->derived) {
            st_select_lex_unit *u = t->derived;
            optimize_select_lex(u->first_select(), a);
        }
    }
}

static void
dropAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        std::stringstream ss;
        ss << "DROP FUNCTION IF EXISTS " << convert_lex_str(u->name) << ";";
        assert_s(conn->execute(ss.str()), ss.str());
    }
}

static void
createAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        std::stringstream ss;
        ss << "CREATE ";
        if (u->type == UDFTYPE_AGGREGATE) ss << "AGGREGATE ";
        ss << "FUNCTION " << u->name.str << " RETURNS ";
        switch (u->returns) {
            case INT_RESULT:    ss << "INTEGER"; break;
            case STRING_RESULT: ss << "STRING";  break;
            default:            thrower() << "unknown return " << u->returns;
        }
        ss << " SONAME 'edb.so';";
        assert_s(conn->execute(ss.str()), ss.str());
    }
}

static void
loadUDFs(Connect * conn) {
    //need a database for the UDFs
    assert_s(conn->execute("DROP DATABASE IF EXISTS cryptdb_udf"), "cannot drop db for udfs even with 'if exists'");
    assert_s(conn->execute("CREATE DATABASE cryptdb_udf;"), "cannot create db for udfs");
    assert_s(conn->execute("USE cryptdb_udf;"), "cannot use db");
    dropAll(conn);
    createAll(conn);
    LOG(cdb_v) << "Loaded CryptDB's UDFs.";
}

Rewriter::Rewriter(ConnectionInfo ci,
                   const std::string &embed_dir,
                   const std::string &dbname,
                   bool encByDefault)
{
    init_mysql(embed_dir);

    ps.ci = ci;
    ps.encByDefault = encByDefault;

    urandom u;
    ps.masterKey = getKey(u.rand_string(AES_KEY_BYTES));

    ps.e_conn = Connect::getEmbedded(embed_dir, dbname);

    ps.conn = new Connect(ci.server, ci.user, ci.passwd, dbname, ci.port);

    // Must be called before loadSchemaInfo.
    buildTypeTextTranslator();
    // printEmbeddedState(ps);

    dml_dispatcher = buildDMLDispatcher();
    ddl_dispatcher = buildDDLDispatcher();

    loadUDFs(ps.conn);

    // HACK: This is necessary as above functions use a USE statement.
    // ie, loadUDFs.
    ps.conn->execute("USE cryptdbtest;");
    ps.e_conn->execute("USE cryptdbtest;");
}

LEX *
Rewriter::dispatchOnLex(Analysis &a, LEX *lex,
                        const ProxyState &ps)
{
    SQLHandler *handler;
    if (dml_dispatcher->canDo(lex)) {
        handler = dml_dispatcher->dispatch(lex);
    } else if (ddl_dispatcher->canDo(lex)) {
        handler = ddl_dispatcher->dispatch(lex);
    } else {
        throw CryptDBError("Rewriter can not dispatch bad lex");
    }

    return handler->transformLex(a, lex, ps);
}

ProxyState::~ProxyState()
{
    if (conn) {
        delete conn;
        conn = NULL;
    }
    if (e_conn) {
        delete e_conn;
        e_conn = NULL;
    }
}

// TODO: Cleanup resources.
Rewriter::~Rewriter()
{}

void
Rewriter::setMasterKey(const std::string &mkey)
{
    ps.masterKey = getKey(mkey);
}

static bool
noRewrite(LEX * lex) {
    switch (lex->sql_command) {
    case SQLCOM_SHOW_DATABASES:
    case SQLCOM_SET_OPTION:
    case SQLCOM_BEGIN:
    case SQLCOM_COMMIT:
    case SQLCOM_SHOW_TABLES:
        return true;
    case SQLCOM_SELECT: {

    }
    default:
        return false;
    }

    return false;
}

// TODO: we don't need to pass analysis, enough to pass returnmeta
QueryRewrite
Rewriter::rewrite(const std::string & q)
{
    LOG(cdb_v) << "q " << q;
    assert(0 == mysql_thread_init());
    //assert(0 == create_embedded_thd(0));

    // printEmbeddedState(ps);

    query_parse p(ps.dbName(), q);
    LEX *lex = p.lex();
    QueryRewrite res;

    /*
     * At minimum we must create a valid Analysis object here because we
     * res requires a valid rmeta object.
     *
     * The optimization is dubious however as we may still want to
     * updateMeta or something.
     */
    //optimization: do not process queries that we will not rewrite
    if (noRewrite(p.lex())) {
        // HACK(burrows): This 'Analysis' is dummy as we never call
        // addToReturn. But it works because this optimized cases don't
        // have anything to do in addToReturn anyways.
        Analysis analysis = Analysis(NULL);

        res.wasRew = false;
        res.queries.push_back(q);
        res.rmeta = analysis.rmeta;
        return res;
    }

    //for as long as there are onion adjustments
    // HACK: Because we need to carry EncLayer adjustment information
    // to the next iteration.
    Analysis *old_analysis = NULL;
    while (true) {
        SchemaInfo * const schema = loadSchemaInfo(ps.e_conn);
        Analysis analysis = Analysis(schema);
        // HACK(burrows): Until redesign.
        analysis.rewriter = this;
        // HACK.
        if (old_analysis) {
            analysis.to_adjust_enc_layers =
                old_analysis->to_adjust_enc_layers;
            delete old_analysis;
        }

        LEX *new_lex = NULL;
        try {
            LOG(cdb_v) << "pre-analyze " << *lex;

            new_lex = analysis.rewriter->dispatchOnLex(analysis, lex, ps);
            assert(new_lex);
        } catch (OnionAdjustExcept e) {
            LOG(cdb_v) << "caught onion adjustment";
            std::cout << "Adjusting onion!" << std::endl;
            adjustOnion(analysis, ps, e.o, e.fm, e.tolevel, e.itf,
                        ps.dbName());
            old_analysis = new Analysis(analysis);
            // HACK.
            for (auto it : analysis.deltas) {
                assert(it.apply(ps.e_conn));
            }
            continue;
        }

        // HACK: To determine if we have a DDL.
        if (analysis.deltas.size() > 0) {
            assert(ps.e_conn->execute(q));
        }
        for (auto it : analysis.deltas) {
            assert(it.apply(ps.e_conn));
        }

        std::list<std::string> queries;
        std::ostringstream ss;
        ss << *new_lex;
        res.queries.push_back(ss.str());

        res.wasRew = true;
        res.rmeta = analysis.rmeta;
        SchemaInfo *old_schema = analysis.getSchema();
        SchemaInfo *new_schema = loadSchemaInfo(ps.e_conn);
        // HACK: The rmetas all have olks that refer to an older version
        // of the FieldMeta that will not have the correct number of
        // EncLayerz for each OnionMeta if there was an adjustment.
        for (unsigned int i = 0; i < res.rmeta->rfmeta.size(); ++i) {
            ReturnField *rf = &res.rmeta->rfmeta[i];
            if (false == rf->is_salt) {
                std::string table_name =
                    old_schema->getTableNameFromFieldMeta(rf->olk.key);
                IdentityMetaKey *table_key =
                    new IdentityMetaKey(table_name);
                TableMeta *tm = new_schema->getChild(table_key);
                delete table_key;
                IdentityMetaKey *field_key =
                    new IdentityMetaKey(rf->olk.key->fname);
                FieldMeta *new_fm = tm->getChild(field_key);
                delete field_key;
                rf->olk.key = new_fm;
            }
        }
        printEmbeddedState(ps);
        return res;
    }
}

//TODO: replace stringify with <<
std::string ReturnField::stringify() {
    std::stringstream res;

    res << " is_salt: " << is_salt << " filed_called " << field_called;
    res <<" fm  " << olk.key << " onion " << olk.o;
    res << " pos_salt " << pos_salt;

    return res.str();
}
std::string ReturnMeta::stringify() {
    std::stringstream res;
    res << "rmeta contains " << rfmeta.size() << " elements: \n";
    for (auto i : rfmeta) {
        res << i.first << " " << i.second.stringify() << "\n";
    }
    return res.str();
}

ResType
Rewriter::decryptResults(ResType & dbres, ReturnMeta * rmeta)
{
    unsigned int rows = dbres.rows.size();
    LOG(cdb_v) << "rows in result " << rows << "\n";
    unsigned int cols = dbres.names.size();

    ResType res = ResType();

    unsigned int index = 0;

    // un-anonymize the names
    for (auto it = dbres.names.begin();
        it != dbres.names.end(); it++) {
        ReturnField rf = rmeta->rfmeta[index];
        if (!rf.is_salt) {
            //need to return this field
            res.names.push_back(rf.field_called);
            // switch types to original ones : TODO

        }
        index++;
    }

    unsigned int real_cols = res.names.size();

    //allocate space in results for decrypted rows
    res.rows = std::vector<std::vector<Item*> >(rows);
    for (unsigned int i = 0; i < rows; i++) {
        res.rows[i] = std::vector<Item*>(real_cols);
    }

    // decrypt rows
    unsigned int col_index = 0;
    for (unsigned int c = 0; c < cols; c++) {
        ReturnField rf = rmeta->rfmeta[c];
        FieldMeta * fm = rf.olk.key;
        if (!rf.is_salt) {
            for (unsigned int r = 0; r < rows; r++) {
                if (!fm || !fm->isEncrypted() ||
                    dbres.rows[r][c]->is_null()) {
                    res.rows[r][col_index] = dbres.rows[r][c];
                } else {
                    uint64_t salt = 0;
                    if (rf.pos_salt>=0) {
                        Item * salt_item = dbres.rows[r][rf.pos_salt];
                        assert_s(!salt_item->null_value, "salt item is null");
                        salt = ((Item_int *)dbres.rows[r][rf.pos_salt])->value;
                    }

                    res.rows[r][col_index] =
                        decrypt_item(fm, rf.olk.o, dbres.rows[r][c],
                                     salt, res.rows[r]);
                }
            }
            col_index++;
        }
    }

    return res;
}

// @show defaults to false.
ResType *
executeQuery(Rewriter &r, const std::string &q, bool show)
{
    try {
        DBResult *dbres;

        QueryRewrite qr = r.rewrite(q);
        if (qr.queries.size() == 0) {
          return NULL;
        }

        unsigned i = 0;
        for (auto query : qr.queries) {
            if (show) {
                std::cerr << std::endl
                     << RED_BEGIN << "ENCRYPTED QUERY [" << i+1 << "/"
                     << qr.queries.size() << "]:" << COLOR_END << std::endl
                     << query << std::endl;
            }
            assert(r.getConnection()->execute(query, dbres));
            if (!dbres) {
                return NULL;
            }
            ++i;
        }

        ResType res = dbres->unpack();

        if (!res.ok) {
          return NULL;
        }

        if (show) {
            std::cerr << std::endl << RED_BEGIN
                      << "ENCRYPTED RESULTS FROM DB:" << COLOR_END
                      << std::endl;
            printRes(res);
            std::cerr << std::endl;
        }

        ResType dec_res = r.decryptResults(res, qr.rmeta);

        if (show) {
            std::cerr << std::endl << RED_BEGIN << "DECRYPTED RESULTS:" << COLOR_END << std::endl;
            printRes(dec_res);
        }

        return new ResType(dec_res);
    } catch (std::runtime_error &e) {
        std::cout << "Unexpected Error: " << e.what() << " in query "
                  << q << std::endl;
        return NULL;
    }  catch (CryptDBError &e) {
        std::cout << "Internal Error: " << e.msg << " in query " << q
                  << std::endl;
	return NULL;
    }
}

void
printRes(const ResType & r) {

    //if (!cryptdb_logger::enabled(log_group::log_edb_v))
    //return;

    std::stringstream ssn;
    for (unsigned int i = 0; i < r.names.size(); i++) {
        char buf[400];
        snprintf(buf, sizeof(buf), "%-25s", r.names[i].c_str());
        ssn << buf;
    }
    std::cerr << ssn.str() << std::endl;
    //LOG(edb_v) << ssn.str();

    /* next, print out the rows */
    for (unsigned int i = 0; i < r.rows.size(); i++) {
        std::stringstream ss;
        for (unsigned int j = 0; j < r.rows[i].size(); j++) {
            char buf[400];
            std::stringstream sstr;
            sstr << *r.rows[i][j];
            snprintf(buf, sizeof(buf), "%-25s", sstr.str().c_str());
            ss << buf;
        }
        std::cerr << ss.str() << std::endl;
        //LOG(edb_v) << ss.str();
    }
}

