#include <string>
#include <memory>
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
#include <main/metadata_tables.hh>

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

/*
    Other interesting error codes
    > ER_DUP_KEY
    > ER_KEY_DOES_NOT_EXIST
*/
static bool
recoverableDeltaError(unsigned int err)
{
    bool ret =
        ER_TABLE_EXISTS_ERROR == err ||     // Table already exists.
        ER_DUP_FIELDNAME == err ||          // Column already exists.
        ER_DUP_KEYNAME == err ||            // Key already exists.
        ER_BAD_TABLE_ERROR == err ||        // Table doesn't exist.
        ER_CANT_DROP_FIELD_OR_KEY == err;   // Key/Col doesn't exist.

    return ret;
}

static bool
fixDelta(Connect *conn, Connect *e_conn, unsigned long delta_output_id)
{
    std::list<std::string> local_queries;
    bool expect_ddl;

    const std::string query_table_name = MetaDataTables::Name::query();

    // Get local queries (should only be one).
    DBResult *dbres;
    const std::string get_local_query_query =
        " SELECT query FROM pdb." + query_table_name +
        "  WHERE delta_output_id = " + std::to_string(delta_output_id) +
        "    AND local = TRUE;";
    assert(e_conn->execute(get_local_query_query, dbres));

    // Onion adjustment queries do not have local.
    ScopedMySQLRes local_r(dbres->n);
    const unsigned long long local_row_count =
        mysql_num_rows(local_r.res());
    if (1 == local_row_count) {
        expect_ddl = true;
        const MYSQL_ROW local_row = mysql_fetch_row(local_r.res());
        const unsigned long * const local_l =
            mysql_fetch_lengths(local_r.res());
        const std::string local_query(local_row[0], local_l[0]);
        local_queries.push_back(local_query);
    } else if (0 == local_row_count) {
        expect_ddl = false;
    } else {
        throw CryptDBError("Too many local queries!");
    }


    // Get remote queries (ORDER matters).
    const std::string remote_query =
        " SELECT query, ddl FROM pdb." + query_table_name +
        "  WHERE delta_output_id = " + std::to_string(delta_output_id) +
        "    AND local = FALSE;"
        "  ORDER BY ASC id";
    assert(e_conn->execute(remote_query, dbres));

    ScopedMySQLRes remote_r(dbres->n);
    MYSQL_ROW remote_row;
    std::list<std::string> remote_queries;
    while ((remote_row = mysql_fetch_row(remote_r.res()))) {
        const unsigned long * const remote_l =
            mysql_fetch_lengths(remote_r.res());
        const std::string remote_query(remote_row[0], remote_l[0]);
        const std::string remote_ddl(remote_row[1], remote_l[1]);
        const bool ddl = string_to_bool(remote_ddl);
        if (ddl != expect_ddl) {
            throw CryptDBError("Expectations of DDLness are unmatched!");
        }

        remote_queries.push_back(remote_query);
    }

    // Do sanity check.
    assert((expect_ddl && remote_queries.size() == 1 &&
            local_queries.size() == 1) ||
           (!expect_ddl && remote_queries.size() >= 1 &&
            local_queries.size() == 0));

    if (expect_ddl) {  // Handle single DDL query.
        if (false == conn->execute(remote_queries.back())) {
            unsigned int err = conn->get_mysql_errno();
            if (false == recoverableDeltaError(err)) {
                throw CryptDBError("Unrecoverable error in Delta "
                                   " recovery!");
            }
        }
    } else {        // Handle one or more DML queries.
        DBResult *dbres;
        const std::string dml_table =
            MetaDataTables::Name::dmlCompletion();
        const std::string dml_query =
            " SELECT * FROM " + dml_table +
            "  WHERE delta_output_id = " +
            " " +    std::to_string(delta_output_id) + ";";
        assert(conn->execute(dml_query, dbres));
        
        ScopedMySQLRes r(dbres->n);
        const unsigned long long dml_row_count =
            mysql_num_rows(r.res());
        if (0 == dml_row_count) {
            assert(conn->execute("START TRANSACTION;"));
            for (auto it : remote_queries) {
                assert(conn->execute(it));
            }
            assert(saveDMLCompletion(conn, delta_output_id));
            assert(conn->execute("COMMIT"));
        } else if (1 > dml_row_count) {
            throw CryptDBError("Too many DML table results!");
        }
    }

    // Cleanup database and do local query.
    {
        assert(e_conn->execute("START TRANSACTION;"));
        if (false == setRegularTableToBleedingTable(e_conn)) {
            assert(e_conn->execute("ROLLBACK;"));
            throw CryptDBError("regular=bleeding fail!");
        }

        if (false == cleanupDeltaOutputAndQuery(e_conn,delta_output_id)) {
            assert(e_conn->execute("ROLLBACK;"));
            throw CryptDBError("cleaning up delta fail!");
        }
        assert(e_conn->execute("COMMIT;"));
    }

    // FIXME: local_query can be DDL.
    // > This can be fixed with a bleeding table.
    assert(local_queries.size() <= 1);
    for (auto it : local_queries) {
        assert(e_conn->execute(it));
    }

    return true;
}

static bool
deltaSanityCheck(Connect *conn, Connect *e_conn)
{
    const std::string table_name = MetaDataTables::Name::delta();

    DBResult *dbres;
    const std::string get_deltas =
        " SELECT id FROM pdb." + table_name + ";";
    assert(e_conn->execute(get_deltas, dbres));

    ScopedMySQLRes r(dbres->n);
    const unsigned long long row_count = mysql_num_rows(r.res());

    std::cerr << "There are " << row_count << " DeltaOutputz!"
              << std::endl;
    if (0 == row_count) {
        return true;
    } else if (1 == row_count) {
        MYSQL_ROW row = mysql_fetch_row(r.res());
        const unsigned long * const l = mysql_fetch_lengths(r.res());
        const std::string string_delta_output_id(row[0], l[0]);
        const unsigned long delta_output_id =
            atoi(string_delta_output_id.c_str());
        return fixDelta(conn, e_conn, delta_output_id);
    } else {
        throw CryptDBError("Too many DeltaOutputz!");
    }
}

// This function will not build all of our tables when it is run
// on an empty database.  If you don't have a parent, your table won't be
// built.  We probably want to seperate our database logic into 3 parts.
//  1> Schema buildling (CREATE TABLE IF NOT EXISTS...)
//  2> INSERTing
//  3> SELECTing
static SchemaInfo *
loadSchemaInfo(Connect *conn, Connect *e_conn)
{
    // Must be done before loading the children.
    assert(deltaSanityCheck(conn, e_conn));

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
    // FIXME: Ideally we would do this before loading the schema.
    // But first we must decide on a place to create the database from.
    assert(sanityCheck(schema));
    
    return schema;
}

/*
static void
printEC(Connect * e_conn, const std::string & command) {
    DBResult * dbres;
    assert_s(e_conn->execute(command, dbres), "command failed");
    ResType res = dbres->unpack();
    printRes(res);
}
*/

static void
printEmbeddedState(const ProxyState & ps) {
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
    const char *onion_chars[] =
        {"oPLAIN", "oDET", "oOPE", "oAGG", "oSWP"};
    onion onions[] = {oPLAIN, oDET, oOPE, oAGG, oSWP};
    static_assert(arraysize(onion_chars) == arraysize(onions),
                  "onion size mismatch!");
    int count = arraysize(onion_chars);
    translatorHelper((const char **)onion_chars, (onion *)onions, count);

    // SecLevels.
    const char *seclevel_chars[] = {"RND", "DET", "DETJOIN", "OPE", "HOM",
                                    "SEARCH", "PLAINVAL"};
    SECLEVEL seclevels[] = {SECLEVEL::RND, SECLEVEL::DET,
                            SECLEVEL::DETJOIN, SECLEVEL::OPE,
                            SECLEVEL::HOM, SECLEVEL::SEARCH,
                            SECLEVEL::PLAINVAL};
    static_assert(arraysize(seclevel_chars) == arraysize(seclevels),
                  "SECLEVEL size mismatch!");
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
    static_assert(arraysize(mysql_type_chars) == arraysize(mysql_types),
                  "mysql type size mismatch!");
    count = arraysize(mysql_type_chars);
    translatorHelper((const char **)mysql_type_chars,
                     (enum enum_field_types *)mysql_types, count);

    // Onion Layouts.
    const char *onion_layout_chars[] =
    {
        "PLAIN_ONION_LAYOUT", "NUM_ONION_LAYOUT",
        "BEST_EFFORT_NUM_ONION_LAYOUT", "STR_ONION_LAYOUT",
        "BEST_EFFORT_STR_ONION_LAYOUT"
        
    };
    onionlayout onion_layouts[] =
    {
        PLAIN_ONION_LAYOUT, NUM_ONION_LAYOUT,
        BEST_EFFORT_NUM_ONION_LAYOUT, STR_ONION_LAYOUT,
        BEST_EFFORT_STR_ONION_LAYOUT
    };
    static_assert(arraysize(onion_layout_chars) ==
                    arraysize(onion_layouts),
                  "onionlayout size mismatch!");
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
    static_assert(arraysize(geometry_type_chars) ==
                    arraysize(geometry_types),
                  "geometry type size mismatch!");
    count = arraysize(geometry_type_chars);
    translatorHelper((const char **)geometry_type_chars,
                    (Field::geometry_type *)geometry_types, count);

    // Security Rating.
    const char *security_rating_chars[] =
    {
        "SENSITIVE", "BEST_EFFORT", "PLAIN"
    };
    SECURITY_RATING security_rating_types[] =
    {
        SECURITY_RATING::SENSITIVE, SECURITY_RATING::BEST_EFFORT,
        SECURITY_RATING::PLAIN
    };
    static_assert(arraysize(security_rating_chars) ==
                    arraysize(security_rating_types),
                  "security rating size mismatch!");
    count = arraysize(security_rating_chars);
    translatorHelper((const char **)security_rating_chars,
                    (SECURITY_RATING *)security_rating_types, count);

    return;
}

//l gets updated to the new level
static std::string
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
    Item_field *salt =
        stringToItemField(fm->getSaltName(), tableanon, itf);
    std::cout << TypeText<onion>::toText(o) << std::endl;
    Item * decUDF = a.getBackEncLayer(om)->decryptUDF(field, salt);

    query << *decUDF << ";";

    std::cerr << "\nADJUST: \n" << query.str() << "\n";

    //execute decryption query

    LOG(cdb_v) << "adjust onions: \n" << query.str() << "\n";

    a.deltas.push_back(new DeleteDelta(a.popBackEncLayer(om), om, NULL));

    *new_level = a.getOnionLevel(om);
    return query.str();
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
static std::list<std::string>
adjustOnion(Analysis &a, const ProxyState &ps, onion o, FieldMeta * fm,
            SECLEVEL tolevel, Item_field *itf, const std::string &cur_db)
{
    OnionMeta *om = fm->getOnionMeta(o);
    SECLEVEL newlevel = a.getOnionLevel(om);
    assert(newlevel != SECLEVEL::INVALID);

    std::list<std::string> adjust_queries;
    while (newlevel > tolevel) {
        auto query =
            removeOnionLayer(a, ps, fm, itf, o, &newlevel, cur_db);
        adjust_queries.push_back(query);
    }
    assert(newlevel == tolevel);

    return adjust_queries;
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
decrypt_item_layers(Item * i, FieldMeta *fm, onion o, uint64_t IV,
                    const std::vector<Item *> &res)
{
    assert(!i->is_null());

    Item * dec = i;
    Item * prev_dec = NULL;

    OnionMeta *om = fm->getOnionMeta(o);
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

Rewriter::Rewriter()
{
    // Must be called before loadSchemaInfo.
    buildTypeTextTranslator();

    dml_dispatcher = buildDMLDispatcher();
    ddl_dispatcher = buildDDLDispatcher();
}

static std::string
lex_to_query(LEX *lex)
{
    std::ostringstream o;
    o << *lex;
    return o.str();
}

RewriteOutput * 
Rewriter::dispatchOnLex(Analysis &a, const ProxyState &ps,
                        const std::string &query)
{
    query_parse p(ps.dbName(), query);
    LEX *lex = p.lex();
    LOG(cdb_v) << "pre-analyze " << *lex;

    // optimization: do not process queries that we will not rewrite
    if (noRewrite(lex)) {
        return new SimpleOutput(query);
    } else if (dml_dispatcher->canDo(lex)) {
        SQLHandler *handler = dml_dispatcher->dispatch(lex);
        try {
            LEX *out_lex = handler->transformLex(a, lex, ps);
            if (true == a.special_update) {
                const auto plain_table =
                    lex->select_lex.top_join_list.head()->table_name;
                const auto crypted_table =
                    out_lex->select_lex.top_join_list.head()->table_name;
                std::string where_clause;
                if (lex->select_lex.where) {
                    std::ostringstream where_stream;
                    where_stream << " " << *lex->select_lex.where << " ";
                    where_clause = where_stream.str();
                } else {
                    where_clause = " TRUE ";
                }
                return new SpecialUpdate(query, lex_to_query(out_lex),
                                         plain_table, crypted_table,
                                         where_clause, ps);
            } else {
                return new DMLOutput(query, lex_to_query(out_lex));
            }
        } catch (OnionAdjustExcept e) {
            LOG(cdb_v) << "caught onion adjustment";
            std::cout << "Adjusting onion!" << std::endl;
            auto adjust_queries = 
                adjustOnion(a, ps, e.o, e.fm, e.tolevel, e.itf,
                            ps.dbName());
            return new AdjustOnionOutput(a.deltas, adjust_queries);
        }
    } else if (ddl_dispatcher->canDo(lex)) {
        SQLHandler *handler = ddl_dispatcher->dispatch(lex);
        LEX *out_lex = handler->transformLex(a, lex, ps);
        return new DDLOutput(query, lex_to_query(out_lex), a.deltas);
    } else {
        throw CryptDBError("Rewriter can not dispatch bad lex");
    }
}

struct DirectiveData {
    std::string table_name;
    std::string field_name;
    SECURITY_RATING sec_rating;

    DirectiveData(const std::string query)
    {
        std::list<std::string> tokens = split(query, " ");
        assert(tokens.size() == 4);
        tokens.pop_front();

        table_name = tokens.front();
        tokens.pop_front();

        field_name = tokens.front();
        tokens.pop_front();

        sec_rating = TypeText<SECURITY_RATING>::toType(tokens.front());
        tokens.pop_front();
    }
};

static
std::string
mysql_noop()
{
    return "do 0;";
}

// FIXME: Implement.
// SYNTAX: DIRECTIVE [table name] [field name] [security rating]
// > FIXME: Make the syntax more sql like.
RewriteOutput * 
Rewriter::handleDirective(Analysis &a, const ProxyState &ps,
                          const std::string &query)
{
    DirectiveData data(query);
    FieldMeta *fm = a.getFieldMeta(data.table_name, data.field_name);
    const SECURITY_RATING current_rating = fm->getSecurityRating();
    if (current_rating < data.sec_rating) {
        throw CryptDBError("cryptdb does not support going to a more "
                           "secure rating!");
    } else if (current_rating == data.sec_rating) {
        return new SimpleOutput(mysql_noop());
    } else {
        // Actually do things.
        throw CryptDBError("implement handleDirective!");
    }
}

static
bool
cryptdbDirective(const std::string query)
{
    std::size_t found = query.find("DIRECTIVE");
    if (std::string::npos == found) {
        return false;
    }
    
    for (std::size_t i = 0; i < found; ++i) {
        if (!std::isspace(query[i])) {
            return false;
        }
    }

    return true;
}

// TODO: we don't need to pass analysis, enough to pass returnmeta
QueryRewrite
Rewriter::rewrite(const ProxyState &ps, const std::string & q)
{
    LOG(cdb_v) << "q " << q;
    assert(0 == mysql_thread_init());
    //assert(0 == create_embedded_thd(0));

    // printEmbeddedState(ps);

    // FIXME: Memleak 'schema'.
    const SchemaInfo * const schema = loadSchemaInfo(ps.conn, ps.e_conn);
    Analysis analysis = Analysis(schema);

    if (cryptdbDirective(q)) {
        RewriteOutput *output = this->handleDirective(analysis, ps, q);
        return QueryRewrite(true, *analysis.rmeta, output);
    } else {
        // FIXME: Memleak return of 'dispatchOnLex()'
        RewriteOutput *output = this->dispatchOnLex(analysis, ps, q);
        return QueryRewrite(true, *analysis.rmeta, output);
    }
}

//TODO: replace stringify with <<
std::string ReturnField::stringify() {
    std::stringstream res;

    res << " is_salt: " << is_salt << " filed_called " << field_called;
    res << " fm  " << olk.key << " onion " << olk.o;
    res << " salt_pos " << salt_pos;

    return res.str();
}
std::string ReturnMeta::stringify() {
    std::stringstream res;
    res << "rmeta contains " << rfmeta.size() << " elements: \n";
    for (auto it : rfmeta) {
        res << it.first << " " << it.second.stringify() << "\n";
    }
    return res.str();
}

ResType *
Rewriter::decryptResults(ResType &dbres, const ReturnMeta &rmeta)
{
    unsigned int rows = dbres.rows.size();
    LOG(cdb_v) << "rows in result " << rows << "\n";
    unsigned int cols = dbres.names.size();

    ResType *res = new ResType();

    // un-anonymize the names
    unsigned int index = 0;
    for (auto it = dbres.names.begin();
        it != dbres.names.end(); it++) {
        const ReturnField &rf = rmeta.rfmeta.at(index);
        if (!rf.getIsSalt()) {
            //need to return this field
            res->names.push_back(rf.fieldCalled());
            // switch types to original ones : TODO

        }
        index++;
    }

    unsigned int real_cols = res->names.size();

    //allocate space in results for decrypted rows
    res->rows = std::vector<std::vector<Item*> >(rows);
    for (unsigned int i = 0; i < rows; i++) {
        res->rows[i] = std::vector<Item*>(real_cols);
    }

    // decrypt rows
    unsigned int col_index = 0;
    for (unsigned int c = 0; c < cols; c++) {
        const ReturnField &rf = rmeta.rfmeta.at(c);
        FieldMeta * fm = rf.getOLK().key;
        if (!rf.getIsSalt()) {
            for (unsigned int r = 0; r < rows; r++) {
                if (!fm || dbres.rows[r][c]->is_null()) {
                    res->rows[r][col_index] = dbres.rows[r][c];
                } else {
                    uint64_t salt = 0;
                    const int salt_pos = rf.getSaltPosition();
                    if (salt_pos >= 0) {
                        Item * salt_item = dbres.rows[r][salt_pos];
                        assert_s(!salt_item->null_value, "salt item is null");
                        salt =
                            ((Item_int *)dbres.rows[r][salt_pos])->value;
                    }

                    res->rows[r][col_index] =
                        decrypt_item_layers(dbres.rows[r][c], fm,
                                            rf.getOLK().o, salt,
                                            res->rows[r]);
                }
            }
            col_index++;
        }
    }

    return res;
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
executeQuery(const ProxyState &ps, const std::string &q)
{
    try {
        Rewriter r;
        QueryRewrite qr = r.rewrite(ps, q);
        std::unique_ptr<ResType> res(qr.output->doQuery(ps.conn,
                                                        ps.e_conn));
        assert(res);
        if (true == qr.output->queryAgain()) { // Onion adjustment.
            return executeQuery(ps, q);
        }
        prettyPrintQueryResult(*res);

        ResType *dec_res = r.decryptResults(*res, qr.rmeta);
        prettyPrintQueryResult(*dec_res);

        printEmbeddedState(ps);
        return dec_res;
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

