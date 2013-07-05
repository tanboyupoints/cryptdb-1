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

#include "field.h"

#include <main/MultiPrinc.hh>

using namespace std;

extern CItemTypesDir itemTypes;
extern CItemFuncDir funcTypes;
extern CItemSumFuncDir sumFuncTypes;
extern CItemFuncNameDir funcNames;

#define ANON                ANON_NAME(__anon_id_)


//TODO: potential inconsistency problem because we update state,
//but only the proxy is responsible for WRT to updateMeta

//TODO: use getAssert in more places
//TODO: replace table/field with FieldMeta * for speed and conciseness

// FIXME: Placement.
template <typename T> List<T>
vectorToList(std::vector<T*> v);

template <typename T, typename F> void
eachList(List_iterator<T> it, F op);

template <typename T, typename F> List<T>
mapList(List_iterator<T> it, F op);

template <typename T, typename F, typename O> O
reduceList(List_iterator<T> it, O init, F op);

static void buildSqlHandlers();

static void
buildTableMeta(ProxyState &ps);

static void
buildFieldMeta(ProxyState &ps, TableMeta *tm, string database_name);

static void
buildOnionMeta(ProxyState &ps, FieldMeta *fm, int field_id);

//TODO: rewrite_proj may not need to be part of each class;
// it just does gather, choos and then rewrite

static Item *
stringToItemField(string field, string table, Item_field * itf) {

    THD * thd = current_thd;
    assert(thd);
    Item_field * res = new Item_field(thd, itf);
    res->name = NULL; //no alias
    res->field_name = make_thd_string(field);
    res->table_name = make_thd_string(table);

    return res;
}

static inline string
extract_fieldname(Item_field *i)
{
    stringstream fieldtemp;
    fieldtemp << *i;
    return fieldtemp.str();
}


//TODO: remove this at some point
static inline void
mysql_query_wrapper(MYSQL *m, const string &q)
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

static void
createMetaTablesIfNotExists(ProxyState & ps)
{
    ostringstream s;

    assert(ps.e_conn->execute("CREATE DATABASE IF NOT EXISTS pdb"));

    assert(ps.e_conn->execute(
                " CREATE TABLE IF NOT EXISTS pdb.table_info"
                " (number bigint NOT NULL UNIQUE,"
                "  anonymous_name varchar(64) NOT NULL UNIQUE,"
                "  name varchar(64) NOT NULL UNIQUE,"
                "  has_sensitive boolean,"
                "  has_salt boolean,"
                "  salt_name varchar(64) NOT NULL,"
                "  database_name varchar(64) NOT NULL,"
                "  id SERIAL PRIMARY KEY)"
                " ENGINE=InnoDB;"));

    s << " CREATE TABLE IF NOT EXISTS pdb.field_info"
      << " (table_info_id bigint NOT NULL," // Foreign key.
      << "  name varchar(64) NOT NULL,"
      << "  ndex bigint NOT NULL,"
      << "  has_salt boolean,"
      << "  salt_name varchar(64),"
      << "  onion_layout enum"
      << " " << TypeText<onionlayout>::parenList().c_str() << " NOT NULL,"
      << "  id SERIAL PRIMARY KEY)"
      << " ENGINE=InnoDB;";

    assert(ps.e_conn->execute(s.str()));
    s.str("");
    s.clear();

    s << " CREATE TABLE IF NOT EXISTS pdb.onion_info"
      << " (field_info_id bigint NOT NULL," // Foreign key.
      << "  name varchar(64) NOT NULL,"
      << "  type enum"
      << " " << TypeText<onion>::parenList() << " NOT NULL,"
      << "  current_level enum"
      << " " << TypeText<SECLEVEL>::parenList() << " NOT NULL,"
      << " sql_type enum"
      << " " << TypeText<enum enum_field_types>::parenList() <<" NOT NULL,"
      << "  id SERIAL PRIMARY KEY)"
      << " ENGINE=InnoDB;";

    assert(ps.e_conn->execute(s.str()));
    s.str("");
    s.clear();

    s << " CREATE TABLE IF NOT EXISTS pdb.layer_key"
      << " (onion_info_id bigint NOT NULL," // Foreign key.
      << "  lkey varbinary(64) NOT NULL,"
      << "  type enum"
      << " " << TypeText<onion>::parenList() << " NOT NULL,"
      << "  level enum"
      << " " << TypeText<SECLEVEL>::parenList() << " NOT NULL,"
      << "  len bigint NOT NULL,"
      << "  id SERIAL PRIMARY KEY)"
      << " ENGINE=InnoDB;";

    assert(ps.e_conn->execute(s.str()));
    return;
}

static void
createInMemoryTables(ProxyState & ps)
{
    buildTableMeta(ps);
    return;
}

static bool
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

static void
buildTableMeta(ProxyState &ps)
{

    DBResult *dbres;
    assert(ps.e_conn->execute(
                " SELECT number, anonymous_name, name, has_sensitive,"
                "        has_salt, salt_name, database_name"
                " FROM pdb.table_info", dbres));
    ScopedMySQLRes r(dbres->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string table_number(row[0], l[0]);
        string table_anonymous_name(row[1], l[1]);
        string table_name(row[2], l[2]);
        string table_has_sensitive(row[3], l[3]);
        string table_has_salt(row[4], l[4]);
        string table_salt_name(row[5], l[5]);
        string table_database_name(row[6], l[6]);

        // FIXME: Signed to unsigned conversion.
        unsigned int int_table_number = atoi(table_number.c_str());
        TableMeta *tm =
            ps.schema->createTableMeta(table_name,
                                       table_anonymous_name,
                                       string_to_bool(table_has_sensitive),
                                       string_to_bool(table_has_salt),
                                       table_salt_name,
                                       &int_table_number);

        buildFieldMeta(ps, tm, table_database_name);
    }

    return;
}

static void
buildFieldMeta(ProxyState &ps, TableMeta *tm, string database_name)
{

    string q = " SELECT f.name, f.ndex, f.has_salt, f.salt_name,"
               "        f.onion_layout, f.id"
               " FROM pdb.table_info t, pdb.field_info f"
               " WHERE t.database_name = '" + database_name + "' "
               "   AND t.number = " + std::to_string(tm->tableNo) +
               "   AND t.id = f.table_info_id;";

    DBResult *dbRes;
    assert(ps.e_conn->execute(q, dbRes));

    ScopedMySQLRes r(dbRes->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string field_name(row[0], l[0]);
        string field_ndex(row[1], l[1]);
        string field_has_salt(row[2], l[2]);
        string field_salt_name(row[3], l[3]);
        string field_onion_layout(row[4], l[4]);
        string field_id(row[5], l[5]);

        FieldMeta *fm = new FieldMeta;
        fm->tm = tm;
        fm->fname = field_name;
        fm->index = atoi(field_ndex.c_str());
        fm->has_salt = string_to_bool(field_has_salt);
        fm->salt_name = field_salt_name;
        fm->onion_layout =
            TypeText<onionlayout>::toType(field_onion_layout);

        tm->fieldMetaMap[fm->fname] = fm;
        // Guarentee order.
        assert(tm->fieldNames.size() == (unsigned long)fm->index);
        tm->fieldNames.push_back(fm->fname);

        buildOnionMeta(ps, fm, atoi(field_id.c_str()));
    }
    return;
}

static std::map<SECLEVEL, std::string>
get_layer_keys(ProxyState &ps, onion o, int onion_id) {
    string q = " SELECT l.lkey, l.type, l.level, l.len"
               " FROM pdb.layer_key l, pdb.onion_info o"
               " WHERE l.onion_info_id = " + std::to_string(onion_id) +
               "    AND o.type = l.type;";

    DBResult *dbRes;
    assert(ps.e_conn->execute(q, dbRes));

    ScopedMySQLRes r(dbRes->n);
    MYSQL_ROW row;
    std::map<SECLEVEL, std::string> layer_keys;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string layer_lkey(row[0], l[0]);
        string layer_type(row[1], l[1]);
        string layer_level(row[2], l[2]);
        string layer_len(row[3], l[3]);

        layer_lkey.erase(atoi(layer_len.c_str()), std::string::npos);

        SECLEVEL level = TypeText<SECLEVEL>::toType(layer_level);
        std::pair<SECLEVEL, std::string> key(level, layer_lkey);
        layer_keys.insert(key);
    }

    return layer_keys;
}

// Should basically mirror init_onions_layout()
static void
buildOnionMeta(ProxyState &ps, FieldMeta *fm, int field_id)
{

    string q = " SELECT o.name, o.type, o.current_level,"
               "        o.sql_type, o.id"
               " FROM pdb.onion_info o, pdb.field_info f"
               " WHERE o.field_info_id = " + std::to_string(field_id) +";";

    DBResult *dbRes;
    assert(ps.e_conn->execute(q, dbRes));

    ScopedMySQLRes r(dbRes->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string onion_name(row[0], l[0]);
        string onion_type(row[1], l[1]);
        string onion_current_level(row[2], l[2]);
        string onion_sql_type(row[3], l[3]);
        string onion_id(row[4], l[4]);

        OnionMeta *om = new OnionMeta();
        om->onionname = onion_name;
        om->sql_type  =
            TypeText<enum enum_field_types>::toType(onion_sql_type);

        onion o = TypeText<onion>::toType(onion_type);
        fm->onions[o] = om;

        // Then, build EncLayer subclasses.
        string uniqueFieldName = fullName(om->onionname,
                                         fm->tm->anonTableName);

        // Add elements to OnionMeta.layers starting with the bottom layer
        // and stopping at the current level.
        std::map<SECLEVEL, std::string> layer_serial =
            get_layer_keys(ps, o, atoi(onion_id.c_str()));
        std::vector<SECLEVEL> layers = fm->onion_layout[o];
        SECLEVEL current_level =
            TypeText<SECLEVEL>::toType(onion_current_level);
        for (auto it: layers) {
            EncLayer *enc_layer;
            string uniqueFieldName = fullName(om->onionname,
                                              fm->tm->anonTableName);

	    enc_layer =
		EncLayerFactory<std::string>::encLayerFromSerial(o, it, om->sql_type, layer_serial[it]);

            om->layers.push_back(enc_layer);
            SECLEVEL onion_level = fm->getOnionLevel(o);
            assert(onion_level != SECLEVEL::INVALID);
            if (it == current_level) {
                assert(it == layers.back());
                break;
            }
        }
     }

     return;
}

static void
printEC(Connect * e_conn, const string & command) {
    DBResult * dbres;
    assert_s(e_conn->execute(command, dbres), "command failed");
    ResType res = dbres->unpack();
    printRes(res);
}

static void
printEmbeddedState(ProxyState & ps) {
    printEC(ps.e_conn, "use pdb;");
    printEC(ps.e_conn, "show databases;");
    printEC(ps.e_conn, "show tables;");
    // printEC(ps.e_conn, "select * from pdb.table_info;");
    // printEC(ps.e_conn, "select * from pdb.field_info;");
    // printEC(ps.e_conn, "select * from pdb.onion_info;");
    // printEC(ps.e_conn, "select * from pdb.layer_key;");
}

template <typename type> static void
translatorHelper(const char **texts, type *enums, int count)
{
    vector<type> vec_enums(count);
    vector<std::string> vec_texts(count);

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


static void
initSchema(ProxyState & ps)
{
    createMetaTablesIfNotExists(ps);

    printEmbeddedState(ps);

    createInMemoryTables(ps);

    return;
}

//l gets updated to the new level
static void
removeOnionLayer(FieldMeta * fm, Item_field * itf, Analysis & a, onion o, SECLEVEL & new_level, const string & cur_db) {

    OnionMeta * om    = getAssert(fm->onions, o);
    string fieldanon  = om->onionname;
    string tableanon  = fm->tm->anonTableName;

    //removes onion layer at the DB
    stringstream query;
    query << "UPDATE " << tableanon << " SET " << fieldanon  << " = ";

    Item * decUDF = om->layers.back()->decryptUDF(stringToItemField(fieldanon,     tableanon, itf),
		  				  stringToItemField(fm->salt_name, tableanon, itf));

    query << *decUDF << ";";

    cerr << "\nADJUST: \n" << query.str() << "\n";

    string usedb = "USE " +  cur_db + ";";
    //HACk: make sure right cur_db in other ways
    assert_s(a.ps->conn->execute(usedb),  "failed to execute " + usedb);
    //execute decryption query
    assert_s(a.ps->conn->execute(query.str()), "failed to execute onion decryption query");

    LOG(cdb_v) << "adjust onions: \n" << query.str() << "\n";

    //remove onion layer in schema
    om->layers.pop_back();

    new_level = om->layers.back()->level();
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
adjustOnion(onion o, FieldMeta * fm, SECLEVEL tolevel, Item_field *itf, Analysis & a, const string & cur_db) {

    SECLEVEL newlevel = fm->getOnionLevel(o);
    assert(newlevel != SECLEVEL::INVALID);

    while (newlevel > tolevel) {
	removeOnionLayer(fm, itf, a, o, newlevel, cur_db);
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




static Item *
decrypt_item_layers(Item * i, onion o, vector<EncLayer *> & layers, uint64_t IV, Analysis &a, FieldMeta *fm, const vector<Item *> &res) {
    assert(!i->is_null());

    if (o == oPLAIN) {// Unencrypted item
	return i;
    }

    // Encrypted item

    Item * dec = i;
    Item * prev_dec = NULL;

    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {

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
decrypt_item(FieldMeta * fm, onion o, Item * i, uint64_t IV, Analysis &a, vector<Item *> &res) {
    assert(!i->is_null());
    return decrypt_item_layers(i, o, fm->onions[o]->layers, IV, a, fm, res);
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
    virtual RewritePlan * do_gather_type(Item_subselect *i, reason &tr, Analysis & a) const {
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
    virtual RewritePlan * do_gather_type(Item_cache *i, reason &tr, Analysis & a) const {
	/*
        Item *example = i->*rob<Item_cache, Item*, &Item_cache::example>::ptr();
        if (example)
            return gather(example, tr, a);
	    return tr.encset;*/
	UNIMPLEMENTED;
	return NULL;
    }
    virtual Item * do_optimize_type(Item_cache *i, Analysis & a) const {
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

static void rewrite_key(const string &table_name,
                        Key *k,
                        Analysis &a,
                        vector<Key*> &l)
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

static LEX **
rewrite_create_lex(LEX *lex, Analysis &a, unsigned *out_lex_count)
{
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

// TODO: Write a dispatcher that will guarentee we aren't mixing ALTER
// actions.
static LEX **
rewrite_alter_lex(LEX *lex, Analysis &a, unsigned *out_lex_count)
{
    LEX *new_lex = copy(lex);

    const string &table =
        lex->select_lex.table_list.first->table_name;

    new_lex->select_lex.table_list =
        rewrite_table_list(lex->select_lex.table_list, a);

    // Rewrite create list.
    if (lex->alter_info.flags & ALTER_ADD_COLUMN) {
        do_field_rewriting(lex, new_lex, table, a);
    }

    // Rewrite drop list.
    if (lex->alter_info.flags & ALTER_DROP_COLUMN) {
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
    }

    // TODO: Rewrite alter column list.
    if (lex->alter_info.flags & ALTER_CHANGE_COLUMN) {
        assert(false);
    }

    // TODO: Rewrite key list.
    if (lex->alter_info.flags & ALTER_FOREIGN_KEY) {

    }

    // TODO: Rewrite indices.
    if (lex->alter_info.flags & (ALTER_ADD_INDEX | ALTER_DROP_INDEX)) {

    }

    LEX **out_lex = new LEX*[1];
    out_lex[0] = new_lex;
    *out_lex_count = 1;
    return out_lex;
}

static LEX **
rewrite_drop_table_lex(LEX *lex, Analysis &a, unsigned *out_lex_count)
{
    LEX * new_lex = copy(lex);
    new_lex->select_lex.table_list =
        rewrite_table_list(lex->select_lex.table_list, a, true);

    LEX **out_lex = new LEX*[1];
    out_lex[0] = new_lex;
    *out_lex_count = 1;
    return out_lex;;
}

static inline void
drop_table_update_meta(const string &q,
                       LEX *lex,
                       Analysis &a)
{
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

static std::string
bool_to_string(bool b)
{
    if (true == b) {
        return "TRUE";
    } else {
        return "FALSE";
    }
}

static bool
do_add_field(TableMeta *tm, const Analysis &a, std::string dbname,
             std::string table, unsigned long long *tid=NULL)
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

static inline void
add_table_update_meta(const string &q,
                      LEX *lex,
                      Analysis &a)
{
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

// TODO.
static inline void
alter_table_update_meta(const string &q, LEX *lex, Analysis &a)
{
    const string &table =
        lex->select_lex.table_list.first->table_name;
    const string &dbname = lex->select_lex.table_list.first->db;

    // Rewrite create list.
    if (lex->alter_info.flags & ALTER_ADD_COLUMN) {
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

    // Rewrite drop list.
    if (lex->alter_info.flags & ALTER_DROP_COLUMN) {
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

    // TODO: Rewrite alter column list.
    if (lex->alter_info.flags & ALTER_CHANGE_COLUMN) {
        assert(false);
    }

    // TODO: Rewrite key list.
    if (lex->alter_info.flags & ALTER_FOREIGN_KEY) {
        assert(false);
    }

    // TODO: Rewrite indices.
    if (lex->alter_info.flags & (ALTER_ADD_INDEX | ALTER_DROP_INDEX)) {
        assert(false);
    }

    return;
}

static void
changeDBUpdateMeta(const string &q, LEX *lex, Analysis &a)
{
    assert(lex->select_lex.db);
    char* dbname = lex->select_lex.db;

    // new dbname is saved for next queries
    a.ps->conn->setCurDBName(dbname);
    a.ps->e_conn->setCurDBName(dbname);
}

static void
dropAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        stringstream ss;
        ss << "DROP FUNCTION IF EXISTS " << convert_lex_str(u->name) << ";";
        assert_s(conn->execute(ss.str()), ss.str());
    }
}

static void
createAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        stringstream ss;
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
                   bool multi,
		   bool encByDefault)
{

    init_mysql(embed_dir);

    ps.ci = ci;
    ps.encByDefault = encByDefault;

    urandom u;
    ps.masterKey = getKey(u.rand_string(AES_KEY_BYTES));

    if (multi) {
	ps.encByDefault = false;
    }

    ps.e_conn = Connect::getEmbedded(embed_dir, dbname);

    ps.conn = new Connect(ci.server, ci.user, ci.passwd, dbname, ci.port);

    ps.schema = new SchemaInfo();
    ps.totalTables = 0;

    // Must be called before initSchema.
    buildTypeTextTranslator();
    initSchema(ps);

    buildSqlHandlers();
    DMLHandler::buildAll();

    loadUDFs(ps.conn);

    // HACK: This is necessary because above functions use a USE statement.
    // ie, loadUDFs.
    ps.conn->setCurDBName(dbname);
    ps.e_conn->setCurDBName(dbname);
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
Rewriter::~Rewriter()
{
}

void
Rewriter::setMasterKey(const string &mkey)
{
    ps.masterKey = getKey(mkey);
}

static list<string>
processAnnotation(Annotation annot, Analysis &a)
{
    //TODO: use EncLayer CreateField information
    assert_s(annot.type == SINGLE_ENC || annot.type == ENCFOR,
	     "parser is in single principal mode, but annotations are for multiple principals");
    assert_s(annot.getPrimitive() != "",
	     "enc annotation has no primitive");
    LOG(cdb_v) << "table is " << annot.getPrimitiveTableName() << "; field is " << annot.getPrimitiveFieldName();

    FieldMeta * fm = a.getFieldMeta(annot.getPrimitiveTableName(), annot.getPrimitiveFieldName());

    init_onions(a.ps->masterKey, fm, fm->sql_field, fm->index);

    list<string> query_list;
    string onionname = "";

    for (auto pr : fm->onions) {
        onion o = pr.first;
        onionname = fm->onions[o]->onionname;
        Create_field * cf = fm->onions[o]->layers.back()->newCreateField(onionname);

        stringstream query;
        query << "ALTER TABLE " << fm->tm->anonTableName;

        switch (o) {
        case oDET:
            LOG(cdb_v) << fm->fname << " (" << fm->index << ") gets DET onion";
            query << " CHANGE " << fm->fname << " " <<  *cf << ";";
            break;
        case oOPE:
            LOG(cdb_v) << fm->fname << " (" << fm->index << ") gets OPE onion";
            query << " ADD " << *cf << " AFTER " << fm->onions[oDET]->onionname << ";";
            break;
        case oAGG:
            LOG(cdb_v) << fm->fname << " (" << fm->index << ") gets AGG onion";
            query << " ADD " << *cf <<  " AFTER " << fm->onions[oOPE]->onionname << ";";
            break;
        case oSWP:
            LOG(cdb_v) << fm->fname << " (" << fm->index << ") gets SWP onion";
            query << " ADD " << *cf << " AFTER " << fm->onions[oOPE]->onionname << ";";
            break;
        default:
            assert_s(false, "unknown onion type");
        }

        query_list.push_back(query.str());
    }

    query_list.push_back("ALTER TABLE " + fm->tm->anonTableName  +
                         " ADD " + fm->salt_name + " " + TN_SALT +
                         " AFTER " + onionname + ";");

    return query_list;
}


static list<string>
rewrite_helper(const string & q, Analysis & analysis,
	       query_parse & p) {
    LOG(cdb_v) << "q " << q;

    if (p.annot) {
        return processAnnotation(*p.annot, analysis);
    }

    LEX *lex = p.lex();

    LOG(cdb_v) << "pre-analyze " << *lex;

    unsigned out_lex_count = 0;
    LEX **new_lexes =
        SqlHandler::rewriteLexAndUpdateMeta(lex, analysis, q, &out_lex_count);
    assert(new_lexes && out_lex_count != 0);

    list<string> queries;
    for (unsigned i = 0; i < out_lex_count; ++i) {
        LOG(cdb_v) << "FINAL QUERY [" << i+1 << "/" << out_lex_count
                   << "]: " << new_lexes[i] << endl;
        stringstream ss;
        ss << *new_lexes[i];
        queries.push_back(ss.str());
    }

    return queries;
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
Rewriter::rewrite(const string & q)
{

    assert(0 == mysql_thread_init());
    //assert(0 == create_embedded_thd(0));

    // printEmbeddedState(ps);

    // TODO: Possibly database name should be in Analysis.
    assert(ps.conn->getCurDBName() == ps.e_conn->getCurDBName());
    query_parse p(ps.conn->getCurDBName(), q);
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
	Analysis analysis = Analysis(&ps);

	res.wasRew = false;
	res.queries.push_back(q);
        res.rmeta = analysis.rmeta;
	return res;
    }

    //for as long as there are onion adjustments
    while (true) {
	Analysis analysis = Analysis(&ps);
        // HACK(burrows): Until redesign.
        analysis.rewriter = this;
	try {
	    res.queries = rewrite_helper(q, analysis, p);
	} catch (OnionAdjustExcept e) {
	    LOG(cdb_v) << "caught onion adjustment";
            cout << "Adjusting onion!" << endl;
	    adjustOnion(e.o, e.fm, e.tolevel, e.itf, analysis,
                        ps.conn->getCurDBName());
	    continue;
	}
        res.wasRew = true;
	res.rmeta = analysis.rmeta;
	return res;
    }
}

//TODO: replace stringify with <<
string ReturnField::stringify() {
    stringstream res;

    res << " is_salt: " << is_salt << " filed_called " << field_called;
    res <<" fm  " << olk.key << " onion " << olk.o;
    res << " pos_salt " << pos_salt;

    return res.str();
}
string ReturnMeta::stringify() {
    stringstream res;
    res << "rmeta contains " << rfmeta.size() << " elements: \n";
    for (auto i : rfmeta) {
	res << i.first << " " << i.second.stringify() << "\n";
    }
    return res.str();
}

ResType
Rewriter::decryptResults(ResType & dbres,
			 ReturnMeta * rmeta) {

    Analysis a = Analysis(&ps);
    a.rmeta = rmeta;

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
    res.rows = vector<vector<Item*> >(rows);
    for (unsigned int i = 0; i < rows; i++) {
        res.rows[i] = vector<Item*>(real_cols);
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

		    res.rows[r][col_index] = decrypt_item(fm, rf.olk.o, dbres.rows[r][c], salt, a, res.rows[r]);
                }
            }
            col_index++;
        }
    }


    return res;
}

// @show defaults to false.
ResType *
executeQuery(Rewriter &r, const string &q, bool show)
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
                cerr << endl
                     << RED_BEGIN << "ENCRYPTED QUERY [" << i+1 << "/"
                     << qr.queries.size() << "]:" << COLOR_END << endl
                     << query << endl;
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
            cerr << endl << RED_BEGIN << "ENCRYPTED RESULTS FROM DB:"
                 << COLOR_END << endl;
            printRes(res);
            cerr << endl;
        }

        ResType dec_res = r.decryptResults(res, qr.rmeta);

        if (show) {
            cerr << endl << RED_BEGIN << "DECRYPTED RESULTS:" << COLOR_END << endl;
            printRes(dec_res);
        }

        return new ResType(dec_res);
    } catch (runtime_error &e) {
        cout << "Unexpected Error: " << e.what() << " in query " << q << endl;
        return NULL;
    } catch (CryptDBError &e) {
        cout << "Internal Error: " << e.msg << " in query " << q << endl;
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
	stringstream ss;
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

template <typename _type>
TypeText<_type>::TypeText(std::vector<_type> enums,
                          std::vector<std::string> texts)
{
    theEnums = new std::vector<_type>(enums);
    theTexts = new std::vector<std::string>(texts);
}

// FIXME(burrows): Use destructor.
template <typename _type>
TypeText<_type>::~TypeText()
{
    delete theEnums;
    delete theTexts;
}

template <typename _type> void
TypeText<_type>::addSet(std::vector<_type> enums,
                        std::vector<std::string> texts)
{
    if (enums.size() != texts.size()) {
        throw "enums and text must be the same length!";
    }

    TypeText<_type>::instance = new TypeText<_type>(enums, texts);

    return;
}

template <typename _type> std::vector<std::string>
TypeText<_type>::allText()
{
    return TypeText<_type>::instance->allText();
}

template <typename _type> std::vector<_type>
TypeText<_type>::allEnum()
{
    return TypeText<_type>::instance->allEnum();
}

template <typename _type> std::string
TypeText<_type>::toText(_type e)
{
    return TypeText<_type>::instance->getText(e);
}

template <typename _type> _type
TypeText<_type>::toType(std::string t)
{
    return TypeText<_type>::instance->getEnum(t);
}

template <typename _type> std::string
TypeText<_type>::parenList()
{
    std::vector<std::string> texts = *TypeText<_type>::instance->theTexts;
    std::stringstream s;
    s << "(";
    for (unsigned int i = 0; i < texts.size(); ++i) {
        s << "'" << texts[i] << "'";
        if (i != texts.size() - 1) {
            s << ", ";
        }
    }
    s << ")";

    return s.str();
}

// FIXME(burrows): Should use a functor + find_if.
template <typename _type>
std::string TypeText<_type>::getText(_type e)
{
    for (unsigned int i = 0; i < theEnums->size(); ++i) {
        if ((*theEnums)[i] == e) {
            return (*theTexts)[i];
        }
    }

    throw "enum does not exist!";
}

template <typename _type>
_type TypeText<_type>::getEnum(std::string t)
{
    for (unsigned int i = 0; i < theTexts->size(); ++i) {
        if ((*theTexts)[i] == t) {
            return (*theEnums)[i];
        }
    }

    throw "text does not exist!";
}

/*
 * SQL Handlers
 */

SqlHandler *SqlHandler::getHandler(enum_sql_command cmd)
{
    std::map<enum_sql_command, SqlHandler *>::iterator h =
        handlers.find(cmd);
    if (handlers.end() == h) {
        return NULL;
    }

    return h->second;
}

bool SqlHandler::addHandler(SqlHandler *handler)
{
    enum_sql_command cmd = handler->getSqlCmd();
    std::map<enum_sql_command, SqlHandler *>::iterator h =
        SqlHandler::handlers.find(cmd);
    if (SqlHandler::handlers.end() != h) {
        return false;
    }

    SqlHandler::handlers[cmd] = handler;
    return true;
}

LEX **
SqlHandler::rewriteLexAndUpdateMeta(LEX *lex, Analysis &analysis,
                                    const string &q, unsigned *out_lex_count)
{
    SqlHandler *sql_handler = SqlHandler::getHandler(lex->sql_command);
    if (!sql_handler) {
        return DMLHandler::rewriteLex(lex, analysis, q, out_lex_count);
    }

    // TODO(burrows): Where should this call be?
    // - In each analysis function?
    // - Here?
    // process_table_list(&lex->select_lex.top_join_list, analysis);

    //TODO: is db neededs as param in all these funcs?
    // (*sql_handler->query_analyze)(lex, analysis);

    // HACK: SQLCOM_ALTER_TABLE
    if ((lex->sql_command != SQLCOM_ALTER_TABLE &&
         true == sql_handler->hasUpdateMeta() &&
         false == sql_handler->updateAfter()) ||
        (lex->sql_command == SQLCOM_ALTER_TABLE &&
         lex->alter_info.flags & ALTER_ADD_COLUMN)) {

        (*sql_handler->update_meta)(q, lex, analysis);
    }

    LEX **new_lexes;
    if (sql_handler->lex_rewrite) {
        new_lexes = (*sql_handler->lex_rewrite)(lex, analysis,
                                                out_lex_count);
    } else {
        new_lexes = new LEX*[1];
        new_lexes[0] = lex;
        *out_lex_count = 1;
    }

    if ((lex->sql_command != SQLCOM_ALTER_TABLE &&
         true == sql_handler->hasUpdateMeta() &&
         true == sql_handler->updateAfter()) ||
        (lex->sql_command == SQLCOM_ALTER_TABLE &&
         lex->alter_info.flags & ALTER_DROP_COLUMN)) {

        (*sql_handler->update_meta)(q, lex, analysis);
    }

    return new_lexes;
}

static void buildSqlHandlers()
{
    SqlHandler *h;

    // HACK: This handler requires a hack.
    // * ADDing a COLUMN requires that we updateMeta _before_ we rewriteLex
    // * DROPing a COLUMN requires that we updateMeta _after_ we rewriteLex
    // Our current SqlHandler therefore does not offer control that is
    // fine grain enough.
    h = new SqlHandler(SQLCOM_ALTER_TABLE, NULL,
                       alter_table_update_meta, rewrite_alter_lex, true);
    assert(SqlHandler::addHandler(h));

    // Must rewrite after update, otherwise TableMeta and FieldMeta
    // will not exist during rewrite.
    h = new SqlHandler(SQLCOM_CREATE_TABLE, NULL,
                       add_table_update_meta, rewrite_create_lex);
    assert(SqlHandler::addHandler(h));

    /*
    h = new SqlHandler(SQLCOM_INSERT, process_select_lex, NULL,
                       rewrite_insert_lex, true);
    assert(SqlHandler::addHandler(h));
    */

    /*
    h = new SqlHandler(SQLCOM_REPLACE, process_select_lex, NULL,
                       rewrite_insert_lex, true);
    assert(SqlHandler::addHandler(h));
    */

    // Must update after rewrite, otherwise you will delete TableMeta
    // and FieldMeta that is needed during rewrite.
    h = new SqlHandler(SQLCOM_DROP_TABLE, NULL,
                       drop_table_update_meta, rewrite_drop_table_lex,
                       true);
    assert(SqlHandler::addHandler(h));

    /*
    h = new SqlHandler(SQLCOM_UPDATE, process_update_lex, NULL,
                       rewrite_update_lex, true);
    assert(SqlHandler::addHandler(h));
    */

    /*
    h = new SqlHandler(SQLCOM_DELETE, process_select_lex, NULL,
                       rewrite_delete_lex, true);
    assert(SqlHandler::addHandler(h));
    */

    /*
    h = new SqlHandler(SQLCOM_SELECT, process_select_lex, NULL,
                       rewrite_select_lex, true);
    assert(SqlHandler::addHandler(h));
    */

    // FIXME: This NULL will break.
    h = new SqlHandler(SQLCOM_CHANGE_DB, NULL,
                       changeDBUpdateMeta, NULL, true);
    assert(SqlHandler::addHandler(h));

}

// Helper functions for doing functional things to List<T> structures.
// MySQL may have already implemented these somewhere?
template <typename T, typename F> void
eachList(List_iterator<T> it, F op) {
    T* element = it++;
    for (; element ; element = it++) {
        op(element);
    }

    return;
}

template <typename T, typename F> List<T>
mapList(List_iterator<T> it, F op) {
    List<T> newList;
    T* element = it++;
    for (; element ; element = it++) {
        newList.push_back(op(element));
    }

    return newList;
}

// A bit off.
template <typename T, typename F, typename O> O
reduceList(List_iterator<T> it, O init, F op) {
    List<T> newList;
    O accum = init;
    T* element = it++;

    for (; element ; element = it++) {
        accum = op(accum, element);
    }

    return accum;
}

template <typename T> List<T>
vectorToList(std::vector<T*> v) {
    List<T> lst;
    for (auto it : v) {
        lst.push_back(it);
    }

    return lst;
}
 
