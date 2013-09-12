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
#include <main/macro_util.hh>

#include "field.h"

extern CItemTypesDir itemTypes;
extern CItemFuncDir funcTypes;
extern CItemSumFuncDir sumFuncTypes;
extern CItemFuncNameDir funcNames;

#define ANON                ANON_NAME(__anon_id_)

//TODO: use getAssert in more places
//TODO: replace table/field with FieldMeta * for speed and conciseness

/*
static Item_field *
stringToItemField(const std::string &field,
                  const std::string &table, Item_field *const itf)
{
    THD *const thd = current_thd;
    assert(thd);
    Item_field *const res = new Item_field(thd, itf);
    res->name = NULL; //no alias
    res->field_name = make_thd_string(field);
    res->table_name = make_thd_string(table);

    return res;
}
*/

static inline std::string
extract_fieldname(Item_field *const i)
{
    std::stringstream fieldtemp;
    fieldtemp << *i;
    return fieldtemp.str();
}


//TODO: remove this at some point
static inline void
mysql_query_wrapper(MYSQL *const m, const std::string &q)
{
    if (mysql_query(m, q.c_str())) {
        cryptdb_err() << "query failed: " << q
                << " reason: " << mysql_error(m);
    }

    // HACK(stephentu):
    // Calling mysql_query seems to have destructive effects
    // on the current_thd. Thus, we must call create_embedded_thd
    // again.
    void *const ret = create_embedded_thd(0);
    if (!ret) assert(false);
}

bool
sanityCheck(FieldMeta *const fm)
{
    for (auto it : fm->children) {
        // FIXME: PTR.
        std::shared_ptr<OnionMeta> om = it.second;
        const onion o = it.first->getValue();
        const std::vector<SECLEVEL> &secs = fm->onion_layout.at(o);
        for (unsigned int i = 0; i < om.get()->layers.size(); ++i) {
            std::shared_ptr<EncLayer> layer = om.get()->layers[i];
            // FIXME: PTR.
            assert(layer.get()->level() == secs[i]);
        }
    }
    return true;
}

static bool
sanityCheck(TableMeta *const tm)
{
    for (auto it : tm->children) {
        std::shared_ptr<FieldMeta> fm = it.second;
        // FIXME: PTR.
        assert(sanityCheck(fm.get()));
    }
    return true;
}

static bool
sanityCheck(SchemaInfo *const schema)
{
    for (auto it : schema->children) {
        std::shared_ptr<TableMeta> tm = it.second;
        // FIXME: PTR.
        assert(sanityCheck(tm.get()));
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
    const bool ret =
        ER_TABLE_EXISTS_ERROR == err ||     // Table already exists.
        ER_DUP_FIELDNAME == err ||          // Column already exists.
        ER_DUP_KEYNAME == err ||            // Key already exists.
        ER_BAD_TABLE_ERROR == err ||        // Table doesn't exist.
        ER_CANT_DROP_FIELD_OR_KEY == err;   // Key/Col doesn't exist.

    return ret;
}

static bool
fixDelta(const std::unique_ptr<Connect> &conn,
         const std::unique_ptr<Connect> &e_conn,
         unsigned long delta_output_id)
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
    RETURN_FALSE_IF_FALSE(e_conn->execute(get_local_query_query, dbres));

    // Onion adjustment queries do not have local.
    ScopedMySQLRes local_r(dbres->n);
    const unsigned long long local_row_count =
        mysql_num_rows(local_r.res());
    if (1 == local_row_count) {
        expect_ddl = true;
        const MYSQL_ROW local_row = mysql_fetch_row(local_r.res());
        const unsigned long *const local_l =
            mysql_fetch_lengths(local_r.res());
        const std::string local_query(local_row[0], local_l[0]);
        local_queries.push_back(local_query);
    } else if (0 == local_row_count) {
        expect_ddl = false;
    } else {
        return false;
    }


    // Get remote queries (ORDER matters).
    const std::string remote_query =
        " SELECT query, ddl FROM pdb." + query_table_name +
        "  WHERE delta_output_id = " + std::to_string(delta_output_id) +
        "    AND local = FALSE;"
        "  ORDER BY ASC id";
    RETURN_FALSE_IF_FALSE(e_conn->execute(remote_query, dbres));

    ScopedMySQLRes remote_r(dbres->n);
    MYSQL_ROW remote_row;
    std::list<std::string> remote_queries;
    while ((remote_row = mysql_fetch_row(remote_r.res()))) {
        const unsigned long *const remote_l =
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
    RETURN_FALSE_IF_FALSE((expect_ddl && remote_queries.size() == 1 &&
                           local_queries.size() == 1) ||
                          (!expect_ddl && remote_queries.size() >= 1 &&
                           local_queries.size() == 0));

    if (expect_ddl) {  // Handle single DDL query.
        if (false == conn->execute(remote_queries.back())) {
            unsigned int err = conn->get_mysql_errno();
            if (false == recoverableDeltaError(err)) {
                return false;
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
        RETURN_FALSE_IF_FALSE(conn->execute(dml_query, dbres));

        ScopedMySQLRes r(dbres->n);
        const unsigned long long dml_row_count =
            mysql_num_rows(r.res());
        if (0 == dml_row_count) {
            RETURN_FALSE_IF_FALSE(conn->execute("START TRANSACTION;"));
            for (auto it : remote_queries) {
                RETURN_FALSE_IF_FALSE(conn->execute(it));
            }
            RETURN_FALSE_IF_FALSE(saveDMLCompletion(conn,
                                                    delta_output_id));
            RETURN_FALSE_IF_FALSE(conn->execute("COMMIT"));
        } else if (1 < dml_row_count) {
            throw CryptDBError("Too many DML table results!");
        }
    }

    // Cleanup database and do local query.
    {
        RETURN_FALSE_IF_FALSE(e_conn->execute("START TRANSACTION;"));
        if (false == setRegularTableToBleedingTable(e_conn)) {
            e_conn->execute("ROLLBACK;");
            return false;
        }

        if (false == cleanupDeltaOutputAndQuery(e_conn,delta_output_id)) {
            e_conn->execute("ROLLBACK;");
            return false;
        }
        RETURN_FALSE_IF_FALSE(e_conn->execute("COMMIT;"));
    }

    // FIXME: local_query can be DDL.
    // > This can be fixed with a bleeding table.
    RETURN_FALSE_IF_FALSE(local_queries.size() <= 1);
    for (auto it : local_queries) {
        RETURN_FALSE_IF_FALSE(e_conn->execute(it));
    }

    return true;
}

static bool
deltaSanityCheck(const std::unique_ptr<Connect> &conn,
                 const std::unique_ptr<Connect> &e_conn)
{
    const std::string table_name = MetaDataTables::Name::delta();

    DBResult *dbres;
    const std::string get_deltas =
        " SELECT id FROM pdb." + table_name + ";";
    RETURN_FALSE_IF_FALSE(e_conn->execute(get_deltas, dbres));

    ScopedMySQLRes r(dbres->n);
    const unsigned long long row_count = mysql_num_rows(r.res());

    std::cerr << "There are " << row_count << " DeltaOutputz!"
              << std::endl;
    if (0 == row_count) {
        return true;
    } else if (1 == row_count) {
        MYSQL_ROW row = mysql_fetch_row(r.res());
        const unsigned long *const l = mysql_fetch_lengths(r.res());
        const std::string string_delta_output_id(row[0], l[0]);
        const unsigned long delta_output_id =
            atoi(string_delta_output_id.c_str());
        return fixDelta(conn, e_conn, delta_output_id);
    } else {
        return false;
    }
}

// This function will not build all of our tables when it is run
// on an empty database.  If you don't have a parent, your table won't be
// built.  We probably want to seperate our database logic into 3 parts.
//  1> Schema buildling (CREATE TABLE IF NOT EXISTS...)
//  2> INSERTing
//  3> SELECTing
static SchemaInfo *
loadSchemaInfo(const std::unique_ptr<Connect> &conn,
               const std::unique_ptr<Connect> &e_conn)
{
    // Must be done before loading the children.
    assert(deltaSanityCheck(conn, e_conn));

    SchemaInfo *const schema = new SchemaInfo();
    // Recursively rebuild the AbstractMeta<Whatever> and it's children.
    std::function<DBMeta *(DBMeta *const)> loadChildren =
        [&loadChildren, &e_conn](DBMeta *const parent) {
            auto kids = parent->fetchChildren(e_conn);
            for (auto it : kids) {
                // FIXME: PTR.
                loadChildren(it.get());
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

template <typename Type> static void
translatorHelper(std::vector<std::string> texts,
                 std::vector<Type> enums)
{
    TypeText<Type>::addSet(enums, texts);
}

static bool
buildTypeTextTranslator()
{
    // Onions.
    const std::vector<std::string> onion_strings
    {
        "oINVALID", "oPLAIN", "oDET", "oOPE", "oAGG", "oSWP"
    };
    const std::vector<onion> onions
    {
        oINVALID, oPLAIN, oDET, oOPE, oAGG, oSWP
    };
    RETURN_FALSE_IF_FALSE(onion_strings.size() == onions.size());
    translatorHelper<onion>(onion_strings, onions);

    // SecLevels.
    const std::vector<std::string> seclevel_strings
    {
        "RND", "DET", "DETJOIN", "OPE", "HOM", "SEARCH", "PLAINVAL",
        "INVALID"
    };
    const std::vector<SECLEVEL> seclevels
    {
        SECLEVEL::RND, SECLEVEL::DET, SECLEVEL::DETJOIN, SECLEVEL::OPE,
        SECLEVEL::HOM, SECLEVEL::SEARCH, SECLEVEL::PLAINVAL,
        SECLEVEL::INVALID
    };
    RETURN_FALSE_IF_FALSE(seclevel_strings.size() == seclevels.size());
    translatorHelper(seclevel_strings, seclevels);

    // MYSQL types.
    const std::vector<std::string> mysql_type_strings
    {
        "MYSQL_TYPE_DECIMAL", "MYSQL_TYPE_TINY", "MYSQL_TYPE_SHORT",
        "MYSQL_TYPE_LONG", "MYSQL_TYPE_FLOAT", "MYSQL_TYPE_DOUBLE",
        "MYSQL_TYPE_NULL", "MYSQL_TYPE_TIMESTAMP", "MYSQL_TYPE_LONGLONG",
        "MYSQL_TYPE_INT24", "MYSQL_TYPE_DATE", "MYSQL_TYPE_TIME",
        "MYSQL_TYPE_DATETIME", "MYSQL_TYPE_YEAR", "MYSQL_TYPE_NEWDATE",
        "MYSQL_TYPE_VARCHAR", "MYSQL_TYPE_BIT", "MYSQL_TYPE_NEWDECIMAL",
        "MYSQL_TYPE_ENUM", "MYSQL_TYPE_SET",
        "MYSQL_TYPE_TINY_BLOB", "MYSQL_TYPE_MEDIUM_BLOB",
        "MYSQL_TYPE_LONG_BLOB", "MYSQL_TYPE_BLOB",
        "MYSQL_TYPE_VAR_STRING", "MYSQL_TYPE_STRING",
        "MYSQL_TYPE_GEOMETRY"
    };
    const std::vector<enum enum_field_types> mysql_types
    {
        MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY, MYSQL_TYPE_SHORT,
        MYSQL_TYPE_LONG, MYSQL_TYPE_FLOAT, MYSQL_TYPE_DOUBLE,
        MYSQL_TYPE_NULL, MYSQL_TYPE_TIMESTAMP, MYSQL_TYPE_LONGLONG,
        MYSQL_TYPE_INT24, MYSQL_TYPE_DATE, MYSQL_TYPE_TIME,
        MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR, MYSQL_TYPE_NEWDATE,
        MYSQL_TYPE_VARCHAR, MYSQL_TYPE_BIT,
        MYSQL_TYPE_NEWDECIMAL /* 246 */, MYSQL_TYPE_ENUM /* 247 */,
        MYSQL_TYPE_SET /* 248 */, MYSQL_TYPE_TINY_BLOB /* 249 */,
        MYSQL_TYPE_MEDIUM_BLOB /* 250 */,
        MYSQL_TYPE_LONG_BLOB /* 251 */, MYSQL_TYPE_BLOB /* 252 */,
        MYSQL_TYPE_VAR_STRING /* 253 */, MYSQL_TYPE_STRING /* 254 */,
        MYSQL_TYPE_GEOMETRY /* 255 */
    };
    RETURN_FALSE_IF_FALSE(mysql_type_strings.size() ==
                            mysql_types.size());
    translatorHelper(mysql_type_strings, mysql_types);

    // MYSQL item types.
    const std::vector<std::string> mysql_item_strings
    {
        "FIELD_ITEM", "FUNC_ITEM", "SUM_FUNC_ITEM", "STRING_ITEM",
        "INT_ITEM", "REAL_ITEM", "NULL_ITEM", "VARBIN_ITEM",
        "COPY_STR_ITEM", "FIELD_AVG_ITEM", "DEFAULT_VALUE_ITEM",
        "PROC_ITEM", "COND_ITEM", "REF_ITEM", "FIELD_STD_ITEM",
        "FIELD_VARIANCE_ITEM", "INSERT_VALUE_ITEM",
        "SUBSELECT_ITEM", "ROW_ITEM", "CACHE_ITEM", "TYPE_HOLDER",
        "PARAM_ITEM", "TRIGGER_FIELD_ITEM", "DECIMAL_ITEM",
        "XPATH_NODESET", "XPATH_NODESET_CMP", "VIEW_FIXER_ITEM"
    };
    const std::vector<enum Item::Type> mysql_item_types
    {
        Item::Type::FIELD_ITEM, Item::Type::FUNC_ITEM,
        Item::Type::SUM_FUNC_ITEM, Item::Type::STRING_ITEM,
        Item::Type::INT_ITEM, Item::Type::REAL_ITEM,
        Item::Type::NULL_ITEM, Item::Type::VARBIN_ITEM,
        Item::Type::COPY_STR_ITEM, Item::Type::FIELD_AVG_ITEM,
        Item::Type::DEFAULT_VALUE_ITEM, Item::Type::PROC_ITEM,
        Item::Type::COND_ITEM, Item::Type::REF_ITEM,
        Item::Type::FIELD_STD_ITEM, Item::Type::FIELD_VARIANCE_ITEM,
        Item::Type::INSERT_VALUE_ITEM, Item::Type::SUBSELECT_ITEM,
        Item::Type::ROW_ITEM, Item::Type::CACHE_ITEM,
        Item::Type::TYPE_HOLDER, Item::Type::PARAM_ITEM,
        Item::Type::TRIGGER_FIELD_ITEM, Item::Type::DECIMAL_ITEM,
        Item::Type::XPATH_NODESET, Item::Type::XPATH_NODESET_CMP,
        Item::Type::VIEW_FIXER_ITEM
    };
    RETURN_FALSE_IF_FALSE(mysql_item_strings.size() ==
                            mysql_item_types.size());
    translatorHelper(mysql_item_strings, mysql_item_types);

    // Onion Layouts.
    const std::vector<std::string> onion_layout_strings
    {
        "PLAIN_ONION_LAYOUT", "NUM_ONION_LAYOUT",
        "BEST_EFFORT_NUM_ONION_LAYOUT", "STR_ONION_LAYOUT",
        "BEST_EFFORT_STR_ONION_LAYOUT"
        
    };
    const std::vector<onionlayout> onion_layouts
    {
        PLAIN_ONION_LAYOUT, NUM_ONION_LAYOUT,
        BEST_EFFORT_NUM_ONION_LAYOUT, STR_ONION_LAYOUT,
        BEST_EFFORT_STR_ONION_LAYOUT
    };
    RETURN_FALSE_IF_FALSE(onion_layout_strings.size() ==
                            onion_layouts.size());
    translatorHelper(onion_layout_strings, onion_layouts);

    // Geometry type.
    const std::vector<std::string> geometry_type_strings
    {
        "GEOM_GEOMETRY", "GEOM_POINT", "GEOM_LINESTRING", "GEOM_POLYGON",
        "GEOM_MULTIPOINT", "GEOM_MULTILINESTRING", "GEOM_MULTIPOLYGON",
        "GEOM_GEOMETRYCOLLECTION"
    };
    std::vector<Field::geometry_type> geometry_types
    {
        Field::GEOM_GEOMETRY, Field::GEOM_POINT, Field::GEOM_LINESTRING,
        Field::GEOM_POLYGON, Field::GEOM_MULTIPOINT,
        Field::GEOM_MULTILINESTRING, Field::GEOM_MULTIPOLYGON,
        Field::GEOM_GEOMETRYCOLLECTION
    };
    RETURN_FALSE_IF_FALSE(geometry_type_strings.size() ==
                            geometry_types.size());
    translatorHelper(geometry_type_strings, geometry_types);

    // Security Rating.
    const std::vector<std::string> security_rating_strings
    {
        "SENSITIVE", "BEST_EFFORT", "PLAIN"
    };
    const std::vector<SECURITY_RATING> security_rating_types
    {
        SECURITY_RATING::SENSITIVE, SECURITY_RATING::BEST_EFFORT,
        SECURITY_RATING::PLAIN
    };
    RETURN_FALSE_IF_FALSE(security_rating_strings.size()
                            == security_rating_types.size());
    translatorHelper(security_rating_strings, security_rating_types);

    return true;
}

// Allows us to preserve boolean return values from
// buildTypeTextTranslator, handle it as a static constant in
// Rewriter and panic when it fails.
static bool
buildTypeTextTranslatorHack()
{
    assert(buildTypeTextTranslator());

    return true;
}

//l gets updated to the new level
static std::string
removeOnionLayer(Analysis &a, const ProxyState &ps,
                 const FieldMeta *const fm,
                 const std::string &table_name,
                 onion o, SECLEVEL *const new_level,
                 const std::string &cur_db)
{
    OnionMeta *const om = a.getOnionMeta(fm, o);

    // Remove the EncLayer.
    std::shared_ptr<EncLayer> back_el(a.popBackEncLayer(om));

    // Update the Meta.
    a.deltas.push_back(new DeleteDelta(back_el, om, NULL));
    const SECLEVEL local_new_level = a.getOnionLevel(om);

    //removes onion layer at the DB
    const std::string tableanon =
        a.getTableMeta(table_name)->getAnonTableName();

    const std::string anon_table_name = a.getAnonTableName(table_name);
    Item_field *const salt =
        new Item_field(NULL, ps.dbName().c_str(), anon_table_name.c_str(),
                       fm->getSaltName().c_str());
    std::cout << TypeText<onion>::toText(o) << std::endl;

    const std::string fieldanon = om->getAnonOnionName();
    Item_field *const field =
        new Item_field(NULL, ps.dbName().c_str(), anon_table_name.c_str(),
                       om->getAnonOnionName().c_str());

    Item *const decUDF = back_el.get()->decryptUDF(field, salt);

    std::stringstream query;
    query << " UPDATE " << cur_db << "." << tableanon
          << "    SET " << fieldanon  << " = " << *decUDF
          << ";";

    std::cerr << "\nADJUST: \n" << query.str() << std::endl;

    //execute decryption query

    LOG(cdb_v) << "adjust onions: \n" << query.str() << std::endl;

    *new_level = local_new_level;
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
adjustOnion(Analysis &a, const ProxyState &ps, onion o,
            const FieldMeta *const fm, SECLEVEL tolevel,
            const std::string &table_name, const std::string &cur_db)
{
    OnionMeta *const om = fm->getOnionMeta(o);
    SECLEVEL newlevel = a.getOnionLevel(om);
    assert(newlevel != SECLEVEL::INVALID);

    std::list<std::string> adjust_queries;
    while (newlevel > tolevel) {
        auto query =
            removeOnionLayer(a, ps, fm, table_name, o, &newlevel, cur_db);
        adjust_queries.push_back(query);
    }
    TEST_UnexpectedSecurityLevel(o, tolevel, newlevel);

    return adjust_queries;
}
//TODO: propagate these adjustments in the embedded database?

static inline bool
FieldQualifies(const FieldMeta *const restriction,
               const FieldMeta *const field)
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
decrypt_item_layers(Item *const i, const FieldMeta *const fm, onion o,
                    uint64_t IV, const std::vector<Item *> &res)
{
    assert(!i->is_null());

    Item *dec = i;
    Item *prev_dec = NULL;

    const OnionMeta *const om = fm->getOnionMeta(o);
    assert(om);
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

static Item *getLeftExpr(Item_in_subselect *const i)
{
    Item *const left_expr =
        i->*rob<Item_in_subselect, Item*,
                &Item_in_subselect::left_expr>::ptr();
    assert(left_expr);

    return left_expr;

}

// HACK: Forces query down to PLAINVAL.
static class ANON : public CItemSubtypeIT<Item_subselect, Item::Type::SUBSELECT_ITEM> {
    virtual RewritePlan *do_gather_type(Item_subselect *i, reason &tr,
                                        Analysis &a) const
    {
        const std::string why = "subselect";

        // Gather subquery.
        std::unique_ptr<Analysis>
            subquery_analysis(new Analysis(a.getSchema()));
        st_select_lex *const select_lex = i->get_select_lex();
        process_table_list(&select_lex->top_join_list,
                           *subquery_analysis);
        process_select_lex(select_lex, *subquery_analysis.get());

        // HACK: Forces the subquery to use PLAINVAL for it's
        // projections.
        auto item_it = List_iterator<Item>(select_lex->item_list);
        for (;;) {
            Item *const item = item_it++;
            if (!item) {
                break;
            }

            RewritePlan *const item_rp =
                subquery_analysis.get()->rewritePlans[item];
            TEST_NoAvailableEncSet(item_rp->es_out, i->type(),
                                   PLAIN_EncSet, why, NULL, 0);
            item_rp->es_out = PLAIN_EncSet;
        }

        const EncSet out_es = PLAIN_EncSet;
        tr = reason(out_es, why, i);

        switch (i->substype()) {
            case Item_subselect::subs_type::EXISTS_SUBS:
                assert(false);
            case Item_subselect::subs_type::IN_SUBS: {
                Item *const left_expr = 
                    getLeftExpr(static_cast<Item_in_subselect *>(i));
                reason r;
                RewritePlan *const rp_left_expr =
                    gather(left_expr, r, *subquery_analysis.get());
                tr.add_child(r);
                a.rewritePlans[left_expr] = rp_left_expr;
                break;
            }
            case Item_subselect::subs_type::ALL_SUBS:
                assert(false);
            case Item_subselect::subs_type::ANY_SUBS:
                assert(false);
            default:
                throw CryptDBError("Unknown subquery type!");
        }

        return new RewritePlanWithAnalysis(out_es, tr,
                                           std::move(subquery_analysis));
    }
    virtual Item * do_optimize_type(Item_subselect *i, Analysis & a) const {
        optimize_select_lex(i->get_select_lex(), a);
        return i;
    }
    virtual Item *do_rewrite_type(Item_subselect *i, const OLK &constr,
                                  const RewritePlan *rp, Analysis &a)
        const
    {
        const RewritePlanWithAnalysis *const rp_w_analysis =
            static_cast<const RewritePlanWithAnalysis *>(rp);
        st_select_lex *const select_lex = i->get_select_lex();

        // ------------------------------
        //    General Subquery Rewrite
        // ------------------------------
        st_select_lex *const new_select_lex =
            rewrite_select_lex(select_lex, *rp_w_analysis->a.get());

        // Rewrite table names.
        new_select_lex->top_join_list =
            rewrite_table_list(select_lex->top_join_list,
                               *rp_w_analysis->a.get());

        // Rewrite SELECT params.
        // HACK: The engine inside of the Item_subselect _can_ have a
        // pointer back to the Item_subselect that contains it.
        // > ie, subselect_single_select_engine::join::select_lex
        // > The way this is done varies from engine to engine thus a
        //   general solution seems difficuly.
        // > set_select_lex() attemps to rectify this problem in other
        //   cases
        memcpy(select_lex, new_select_lex, sizeof(st_select_lex));

        // ------------------------------
        //   Specific Subquery Rewrite
        // ------------------------------
        {
            switch (i->substype()) {
                case Item_subselect::subs_type::EXISTS_SUBS:
                    assert(false);
                case Item_subselect::subs_type::IN_SUBS: {
                    Item *const left_expr =
                        getLeftExpr(static_cast<Item_in_subselect *>(i));
                    RewritePlan *const rp_left_expr =
                        getAssert(a.rewritePlans, left_expr);
                    Item *const new_left_expr =
                        itemTypes.do_rewrite(left_expr, constr,
                                             rp_left_expr, a);
                    return new Item_in_subselect(new_left_expr,
                                                 new_select_lex);
                }
                case Item_subselect::subs_type::ALL_SUBS:
                    assert(false);
                case Item_subselect::subs_type::ANY_SUBS:
                    assert(false);
                default:
                    throw CryptDBError("Unknown subquery type!");
            }
        }
    }
} ANON;

// NOTE: Shouldn't be needed unless we allow mysql to rewrite subqueries.
static class ANON : public CItemSubtypeIT<Item_cache, Item::Type::CACHE_ITEM> {
    virtual RewritePlan *do_gather_type(Item_cache *i, reason &tr,
                                        Analysis &a) const
    {
        UNIMPLEMENTED;
        return NULL;

        /*
        TEST_TextMessageError(false ==
                                i->field()->orig_table->alias_name_used,
                              "Can not mix CACHE_ITEM and table alias.");
        const std::string table_name =
            std::string(i->field()->orig_table->alias);
        const std::string field_name =
            std::string(i->field()->field_name);
        OnionMeta *const om =
            a.getOnionMeta(table_name, field_name, oPLAIN);
        if (a.getOnionLevel(om) != SECLEVEL::PLAINVAL) {
            const FieldMeta *const fm =
                a.getFieldMeta(table_name, field_name);

            throw OnionAdjustExcept(oPLAIN, fm, SECLEVEL::PLAINVAL,
                                    table_name);
        }

        const EncSet out_es = PLAIN_EncSet;
        tr = reason(out_es, "is cache item", i);

        return new RewritePlan(out_es, tr);
        */

        /*
        Item *example = i->*rob<Item_cache, Item*, &Item_cache::example>::ptr();
        if (example)
            return gather(example, tr, a);
        return tr.encset;
        UNIMPLEMENTED;
        return NULL;
        */
    }
    virtual Item * do_optimize_type(Item_cache *i, Analysis & a) const
    {
        // TODO(stephentu): figure out how to use rob here
        return i;
    }
    virtual Item *do_rewrite_type(Item_cache *i, const OLK &constr,
                                  const RewritePlan *rp, Analysis &a)
        const
    {
        UNIMPLEMENTED;
        return NULL;
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
noRewrite(const LEX *const lex) {
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

static std::string
lex_to_query(LEX *const lex)
{
    std::ostringstream o;
    o << *lex;
    return o.str();
}

/*
static
bool
mysql_noop_dbres(const std::unique_ptr<Connect> &c, DBResult **dbres)
{
    if (!(c.get()->execute(mysql_noop(), *dbres))) {
        dbres = NULL;
        return false;
    }

    return true;
}
*/

const bool Rewriter::translator_dummy = buildTypeTextTranslatorHack();
const SQLDispatcher *Rewriter::dml_dispatcher = buildDMLDispatcher();
const SQLDispatcher *Rewriter::ddl_dispatcher = buildDDLDispatcher();

RewriteOutput *
Rewriter::dispatchOnLex(Analysis &a, const ProxyState &ps,
                        const std::string &query)
{
    std::unique_ptr<query_parse> p;
    try {
        p =
            std::unique_ptr<query_parse>(new query_parse(ps.dbName(),
                                                         query));
    } catch (std::runtime_error &e) {
        std::cerr << "Bad Query: " << query << std::endl;
        return new SimpleOutput(mysql_noop());
    }
    LEX *const lex = p.get()->lex();

    LOG(cdb_v) << "pre-analyze " << *lex;

    // optimization: do not process queries that we will not rewrite
    if (noRewrite(lex)) {
        return new SimpleOutput(query);
    } else if (dml_dispatcher->canDo(lex)) {
        const SQLHandler *const handler = dml_dispatcher->dispatch(lex);
        AssignOnce<LEX *> out_lex;

        try {
            out_lex = handler->transformLex(a, lex, ps);
        } catch (OnionAdjustExcept e) {
            LOG(cdb_v) << "caught onion adjustment";
            std::cout << "Adjusting onion!" << std::endl;
            auto adjust_queries = 
                adjustOnion(a, ps, e.o, e.fm, e.tolevel, e.table_name,
                            ps.dbName());
            return new AdjustOnionOutput(a.deltas, adjust_queries);
        }

        // Return if it's a regular DML query.
        if (false == a.special_update) {
            return new DMLOutput(query, lex_to_query(out_lex.get()));
        }

        // Handle HOMorphic UPDATE.
        const auto plain_table =
            lex->select_lex.top_join_list.head()->table_name;
        const auto crypted_table =
            out_lex.get()->select_lex.top_join_list.head()->table_name;
        std::string where_clause;
        if (lex->select_lex.where) {
            std::ostringstream where_stream;
            where_stream << " " << *lex->select_lex.where << " ";
            where_clause = where_stream.str();
        } else {
            where_clause = " TRUE ";
        }

        return new SpecialUpdate(query, lex_to_query(out_lex.get()),
                                 plain_table, crypted_table,
                                 where_clause, ps);
    } else if (ddl_dispatcher->canDo(lex)) {
        const SQLHandler *const handler = ddl_dispatcher->dispatch(lex);
        LEX *const out_lex = handler->transformLex(a, lex, ps);
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

// FIXME: Implement.
// SYNTAX
// > DIRECTIVE UPDATE cryptdb_metadata
//                SET <table_name | field_name | rating> = [value]
// > DIRECTIVE SELECT <table_name | field_name | rating>
//               FROM cryptdb_metadata
//              WHERE <table_name | field_name | rating> = [value]
RewriteOutput *
Rewriter::handleDirective(Analysis &a, const ProxyState &ps,
                          const std::string &query)
{
    DirectiveData data(query);
    const FieldMeta *const fm =
        a.getFieldMeta(data.table_name, data.field_name);
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
cryptdbDirective(const std::string &query)
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

QueryRewrite
Rewriter::rewrite(const ProxyState &ps, const std::string &q,
                  SchemaInfo **const out_schema)
{
    LOG(cdb_v) << "q " << q;
    assert(0 == mysql_thread_init());
    //assert(0 == create_embedded_thd(0));

    // printEmbeddedState(ps);

    // FIXME: Memleak 'schema'.
    AssignOnce<SchemaInfo *> schema;
    if (ps.schemaIsStale()) {
        schema = loadSchemaInfo(ps.getConn(), ps.getEConn());
    } else {
        schema = ps.getPreviousSchema();
    }

    assert(schema.get());
    Analysis analysis = Analysis(schema.get());

    RewriteOutput *output;
    try {
        if (cryptdbDirective(q)) {
            output = Rewriter::handleDirective(analysis, ps, q);
        } else {
            // FIXME: Memleak return of 'dispatchOnLex()'
            output = Rewriter::dispatchOnLex(analysis, ps, q);
        }
    } catch (AbstractCryptDBError &e) {
        std::cout << e << std::endl;
        output = new SimpleOutput(mysql_noop());
    }

    *out_schema = schema.get();
    return QueryRewrite(true, *analysis.rmeta, output);
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
Rewriter::decryptResults(const ResType &dbres, const ReturnMeta &rmeta)
{
    const unsigned int rows = dbres.rows.size();
    LOG(cdb_v) << "rows in result " << rows << "\n";
    const unsigned int cols = dbres.names.size();

    ResType *const res = new ResType();

    // un-anonymize the names
    for (auto it = dbres.names.begin();
        it != dbres.names.end(); it++) {
        const unsigned int index = it - dbres.names.begin();
        const ReturnField &rf = rmeta.rfmeta.at(index);
        if (!rf.getIsSalt()) {
            //need to return this field
            res->names.push_back(rf.fieldCalled());
            // switch types to original ones : TODO

        }
    }

    const unsigned int real_cols = res->names.size();

    //allocate space in results for decrypted rows
    res->rows = std::vector<std::vector<Item*> >(rows);
    for (unsigned int i = 0; i < rows; i++) {
        res->rows[i] = std::vector<Item*>(real_cols);
    }

    // decrypt rows
    unsigned int col_index = 0;
    for (unsigned int c = 0; c < cols; c++) {
        const ReturnField &rf = rmeta.rfmeta.at(c);
        if (rf.getIsSalt()) {
            continue;
        }

        FieldMeta *const fm = rf.getOLK().key;
        for (unsigned int r = 0; r < rows; r++) {
            if (!fm || dbres.rows[r][c]->is_null()) {
                res->rows[r][col_index] = dbres.rows[r][c];
            } else {
                uint64_t salt = 0;
                const int salt_pos = rf.getSaltPosition();
                if (salt_pos >= 0) {
                    Item_int *const salt_item =
                        static_cast<Item_int *>(dbres.rows[r][salt_pos]);
                    assert_s(!salt_item->null_value, "salt item is null");
                    salt = salt_item->value;
                }

                res->rows[r][col_index] =
                    decrypt_item_layers(dbres.rows[r][c], fm,
                                        rf.getOLK().o, salt,
                                        res->rows[r]);
            }
        }
        col_index++;
    }

    return res;
}

static void
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

static const std::unique_ptr<Connect> &
deductConnection(RewriteOutput *const output, const ProxyState &ps)
{
    switch (output->queryChannel()) {
        case RewriteOutput::Channel::REGULAR:
            return ps.getConn();
        case RewriteOutput::Channel::SIDE:
            return ps.getSideChannelConn();
        default:
            throw CryptDBError("Unrecognized Channel!");
    }
}

// FIXME: DBResult and ResType memleaks.
// FIXME: Use TELL policy.
ResType *
executeQuery(ProxyState &ps, const std::string &q)
{
    try {
        QueryRewrite *qr = NULL;
        std::list<std::string> out_queryz;
        assert(queryPreamble(ps, q, &qr, &out_queryz));
        assert(qr);

        const std::unique_ptr<Connect>
            &query_conn(deductConnection(qr->output, ps));

        DBResult *dbres = NULL;
        for (auto it : out_queryz) {
            prettyPrintQuery(it);

            if (!query_conn->execute(it, dbres)) {
                qr->output->handleQueryFailure(ps.getEConn());
                throw CryptDBError("Failed to execute query!");
            }
            assert(dbres);
        }

        // Query epilogue.
        assert(qr->output->afterQuery(ps.getEConn()));

        if (qr->output->queryAgain()) {
            return executeQuery(ps, q);
        }

        printEmbeddedState(ps);

        if (dbres) {
            ResType *const res = new ResType(dbres->unpack());

            prettyPrintQueryResult(*res);
            // > Presumably we may want to pass something through here
            // that doesn't need to be decrypted (ie, a no-op).
            // > Such can be accomodated by making this an if-statement.
            assert(qr->output->doDecryption());

            ResType *const dec_res =
                Rewriter::decryptResults(*res, qr->rmeta);
            prettyPrintQueryResult(*dec_res);

            return dec_res;
        } else {
            return NULL;
        }
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
printRes(const ResType &r) {

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

