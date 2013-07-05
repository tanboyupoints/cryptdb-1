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

#include <main/cdb_rewrite.hh>
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

#include <main/MultiPrinc.hh>

using namespace std;

extern CItemTypesDir itemTypes;
extern CItemFuncDir funcTypes;
extern CItemSumFuncDir sumFuncTypes;
extern CItemFuncNameDir funcNames;

//TODO: potential inconsistency problem because we update state,
//but only the proxy is responsible for WRT to updateMeta

//TODO: use getAssert in more places
//TODO: replace table/field with FieldMeta * for speed and conciseness

// FIXME: Placement.
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

//TODO: which encrypt/decrypt should handle null?
static Item *
encrypt_item_layers(Item * i, onion o, std::vector<EncLayer *> & layers, Analysis &a, FieldMeta *fm = 0, uint64_t IV = 0) {
    assert(!i->is_null());

    if (o == oPLAIN) {//Unencrypted item
	return i;
    }

    // Encrypted item

    assert_s(layers.size() > 0, "field must have at least one layer");
    Item * enc = i;
    Item * prev_enc = NULL;
    for (auto layer : layers) {
        LOG(encl) << "encrypt layer " << levelnames[(int)layer->level()] << "\n";
	enc = layer->encrypt(enc, IV);
        //need to free space for all enc
        //except the last one
        if (prev_enc) {
            delete prev_enc;
        }
        prev_enc = enc;
    }

    return enc;
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

// encrypts a constant item based on the information in a
static Item *
encrypt_item(Item * i, const OLK & olk, Analysis & a)
{
    assert(!i->is_null());

    if (olk.l == SECLEVEL::PLAINVAL)
        return i;

    FieldMeta * fm = olk.key;
    assert(fm);

    onion o        = olk.o;
    LOG(cdb_v) << fm->fname << " " << fm->onions.size();

    auto it = a.salts.find(fm);
    salt_type IV = 0;
    if (it != a.salts.end()) {
	IV = it->second;
    }
    return encrypt_item_layers(i, o, fm->onions[o]->layers, a, fm, IV);
}

static void
encrypt_item_all_onions(Item * i, FieldMeta * fm,
                        uint64_t IV, vector<Item*> & l, Analysis &a) {

    for (auto it : fm->onions) {
        l.push_back(encrypt_item_layers(i, it.first, it.second->layers, a, fm, IV));
    }
}

static Item *
decrypt_item(FieldMeta * fm, onion o, Item * i, uint64_t IV, Analysis &a, vector<Item *> &res) {
    assert(!i->is_null());
    return decrypt_item_layers(i, o, fm->onions[o]->layers, IV, a, fm, res);
}

// An implementation of gather for the common case
// Works for Item_func with two arguments, solution encset is intersect of
// children and my_es
// Only works with nodes with one outgoing encset, which could be other_encset
// if encset_from_intersection is false, or else is the intersection with the children
static RewritePlan *
typical_gather(Analysis & a, Item_func * i,
	     const EncSet & my_es, string why, reason & my_r,
	     bool encset_from_intersection, const EncSet & other_encset = PLAIN_EncSet)
{

    Item **args = i->arguments();
    assert(i->argument_count() == 2);

    reason r1, r2;
    RewritePlan ** childr_rp = new RewritePlan*[2];
    childr_rp[0] = gather(args[0], r1, a);
    childr_rp[1] = gather(args[1], r2, a);

    EncSet solution = my_es.intersect(childr_rp[0]->es_out).
	                    intersect(childr_rp[1]->es_out);

    if (solution.empty()) {
	cerr << "crypto schemes does not support this query BECAUSE " << i << "NEEDS " << my_es << "\n" \
	     << " BECAUSE " << why << "\n" \
	     << " AND children have:  " << r1 << r2 << "\n";
	assert(false);
    }

    EncSet out_es;
    if (encset_from_intersection) {
	assert_s(solution.singleton(), "cannot use basic_gather with more outgoing encsets");
	out_es = solution;
    } else {
	out_es = PLAIN_EncSet;
    }

    my_r = reason(out_es, why, i);
    my_r.add_child(r1);
    my_r.add_child(r2);

    return new RewritePlanOneOLK(out_es.extract_singleton(),
				 solution.chooseOne(), childr_rp,
				 my_r);
}

static inline void
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


template <class T>
static Item *
do_optimize_type_self_and_args(T *i, Analysis &a) {
    UNIMPLEMENTED;
    /*
    Item *i0 = do_optimize_const_item(i, a);
    if (i0 == i) {
        // no optimizations done at top level
        // try children
        Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            optimize(&args[x], a);
        }
        return i;
    } else {
        return i0;
	}*/
}

// in place rewriting of arguments
template <class T>
static void
rewrite_args_FN(T * i, const OLK & constr,
		const RewritePlanOneOLK * rp, Analysis &a) {

    Item **args = i->arguments();
    uint count = i->argument_count();
    for (uint x = 0; x < count; x++) {
        args[x] = itemTypes.do_rewrite(args[x],
				       rp->olk, rp->childr_rp[x],
				       a);
        args[x]->name = NULL; // args should never have aliases...
    }
}

// rewrites the arguments of aggregators
// no_args specifies a certain number of arguments that I must have
// if negative, i can have any no. of arguments
static list<Item *>
rewrite_agg_args(Item_sum * oldi, const OLK & constr, const RewritePlanOneOLK * rp,
		 Analysis & a, int no_args = -1) {
    if (no_args >= 0) {
	assert_s(oldi->get_arg_count() == (uint)no_args,
		 "support for aggregation with this number of arguments not currently implemented");
    } else {
	no_args = oldi->get_arg_count();
    }

    list<Item *> res = list<Item *>();
    for (int j = 0; j < no_args; j++) {
	Item * child_item = oldi->get_arg(j);
	child_item = itemTypes.do_rewrite(child_item, rp->olk, rp->childr_rp[j], a);
	res.push_back(child_item);
    }

    return res;
}

template <typename ItemType>
static void
typical_rewrite_insert_type(ItemType *i, Analysis &a, vector<Item *> &l,
                            FieldMeta *fm)
{
    if (!fm->isEncrypted()) {
        l.push_back(make_item(i));
        return;
    }

    // Encrypted

    uint64_t salt = 0;

    if (fm->has_salt) {
        salt = randomValue();
    } else {
        //TODO: need to use table salt in this case
    }

    encrypt_item_all_onions(i, fm, salt, l, a);

    if (fm->has_salt) {
        l.push_back(new Item_int((ulonglong) salt));
    }
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

static class ANON : public CItemSubtypeIT<Item_string, Item::Type::STRING_ITEM> {
    virtual RewritePlan * do_gather_type(Item_string *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << " String item do_gather " << *i;
        /* constant strings are always ok */
	tr = reason(FULL_EncSet, "is a constant", i);
	return new RewritePlan(FULL_EncSet_Str, tr);

    }
    virtual Item * do_optimize_type(Item_string *i, Analysis & a) const {
        return i;
    }

    virtual Item * do_rewrite_type(Item_string *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type String item " << *i;

        return encrypt_item(i, constr, a);
    }

    virtual void
    do_rewrite_insert_type(Item_string *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_num, Item::Type::INT_ITEM> {
    virtual RewritePlan * do_gather_type(Item_num *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT (L966) num do_gather " << *i;
        /* constant ints are always ok */
	tr = reason(FULL_EncSet, "is a constant", i);
	return new RewritePlan(FULL_EncSet_Int, tr);

    }

    virtual Item * do_optimize_type(Item_num *i, Analysis & a) const {
        return i;
    }

    virtual Item * do_rewrite_type(Item_num *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type " << *i << endl;

        return encrypt_item(i, constr, a);
    }

    virtual void
    do_rewrite_insert_type(Item_num *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_decimal, Item::Type::DECIMAL_ITEM> {
    virtual RewritePlan * do_gather_type(Item_decimal *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT (L1024) decimal do_gather " << *i;
	UNIMPLEMENTED;
        /* constant decimals are always ok */
        return NULL;
    }
        virtual Item * do_optimize_type(Item_decimal *i, Analysis & a) const {
        return i;
    }
    virtual Item * do_rewrite_type(Item_decimal *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type L1028";
        double n = i->val_real();
        char buf[sizeof(double) * 2];
        sprintf(buf, "%x", (unsigned int)n);
        // TODO(stephentu): Do some actual encryption of the double here
        return new Item_hex_string(buf, sizeof(buf));
    }
    virtual void
    do_rewrite_insert_type(Item_decimal *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        assert(fm != NULL);
        double n = i->val_real();
        char buf[sizeof(double) * 2];
        sprintf(buf, "%x", (unsigned int)n);
        for (auto it = fm->onions.begin();
             it != fm->onions.end();
             ++it) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
        if (fm->has_salt) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
        //if no onions, add field as is
        if (l.empty()) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
    }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_neg, Item_func::Functype::NEG_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_neg *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_neg *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_not, Item_func::Functype::NOT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_not *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_not *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

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

extern const char str_in_optimizer[] = "<in_optimizer>";
static class ANON : public CItemSubtypeFN<Item_in_optimizer, str_in_optimizer> {
    virtual RewritePlan * do_gather_type(Item_in_optimizer *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeFN (L1107) do_gather " << *i;

	UNIMPLEMENTED;
        //Item **args = i->arguments();
        //analyze(args[0], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        //analyze(args[1], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        return NULL;
    }
    virtual Item * do_optimize_type(Item_in_optimizer *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
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


template<Item_func::Functype FT, class IT>
class CItemCompare : public CItemSubtypeFT<Item_func, FT> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const {
	LOG(cdb_v) << "CItemCompare (L1139) do_gather func " << *i;

        EncSet my_es;
	string why = "";

        if (FT == Item_func::Functype::EQ_FUNC ||
            FT == Item_func::Functype::EQUAL_FUNC ||
            FT == Item_func::Functype::NE_FUNC) {
            my_es = EQ_EncSet;
	    why = "compare equality";
        } else {
            my_es = ORD_EncSet;
	    why = "compare order";
        }

	Item ** args = i->arguments();
	assert_s(i->argument_count() == 2, "expected two arguments for comparison");
        if (!args[0]->const_item() && !args[1]->const_item()) {
            why = why + "; join";
	    cerr << "join";
	    my_es = JOIN_EncSet;
	}

	return typical_gather(a, i, my_es, why, tr, false, PLAIN_EncSet);

    }

    virtual Item * do_optimize_type(Item_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func *i, const OLK & constr,
				   const RewritePlan * rp, Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type Item_func " << *i << " constr " << constr;
	Item ** args = i->arguments();
	assert_s(i->argument_count() == 2, "compare function does not have two arguments as expected");
	IT *res = new IT(args[0], args[1]);
	rewrite_args_FN(res, constr, (const RewritePlanOneOLK *)rp, a);
        LOG(cdb_v) << "return 1171 " << *res;
        return res;
    }
};

static CItemCompare<Item_func::Functype::EQ_FUNC,    Item_func_eq>    ANON;
static CItemCompare<Item_func::Functype::EQUAL_FUNC, Item_func_equal> ANON;
static CItemCompare<Item_func::Functype::NE_FUNC,    Item_func_ne>    ANON;
static CItemCompare<Item_func::Functype::GT_FUNC,    Item_func_gt>    ANON;
static CItemCompare<Item_func::Functype::GE_FUNC,    Item_func_ge>    ANON;
static CItemCompare<Item_func::Functype::LT_FUNC,    Item_func_lt>    ANON;
static CItemCompare<Item_func::Functype::LE_FUNC,    Item_func_le>    ANON;

template<Item_func::Functype FT, class IT>
class CItemCond : public CItemSubtypeFT<Item_cond, FT> {
    virtual RewritePlan * do_gather_type(Item_cond *i, reason &tr, Analysis & a) const {
	/*   auto it = List_iterator<Item>(*i->argument_list());
        for (;;) {
            Item *argitem = it++;
            if (!argitem)
                break;
	    reason new_tr(tr.encset.intersect(EQ_EncSet), "and/or", i);
            RewritePlan * rp = gather(argitem, tr, a);
            if (!rp->es_out.contains(PLAIN_OLK))
                thrower() << "cannot obtain PLAIN for " << *argitem;
        }

        return rp;*/
	UNIMPLEMENTED;
    }

    virtual Item * do_optimize_type(Item_cond *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_cond *i,
				   const OLK & olk, const RewritePlan * rp, Analysis & a) const {
	List<Item> newlist;
	auto it = List_iterator<Item>(*i->argument_list());
	for (;;) {
	    Item *argitem = it++;
	    if (!argitem) {
		break;
	    }
	    newlist.push_back(rewrite(argitem, olk, a));
	}

	IT * res = new IT(newlist);
	return res;
    }
};

static CItemCond<Item_func::Functype::COND_AND_FUNC, Item_cond_and> ANON;
static CItemCond<Item_func::Functype::COND_OR_FUNC,  Item_cond_or>  ANON;

template<Item_func::Functype FT>
class CItemNullcheck : public CItemSubtypeFT<Item_bool_func, FT> {
    virtual RewritePlan * do_gather_type(Item_bool_func *i, reason &tr, Analysis & a) const {
        Item **args = i->arguments();
        assert(i->argument_count() == 1);

        reason r;
        RewritePlan **child_rp = new RewritePlan*[2];
        child_rp[0] = gather(args[0], r, a);

        EncSet solution = child_rp[0]->es_out;
        EncSet out_es = PLAIN_EncSet;

        tr = reason(out_es, "nullcheck", i);
        tr.add_child(r);

        return new RewritePlanOneOLK(out_es.extract_singleton(),
                                     solution.chooseOne(),
                                     child_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_bool_func * i, const OLK & constr,
                                   const RewritePlan * _rp, Analysis & a)
                                  const {
        rewrite_args_FN(i, constr, (const RewritePlanOneOLK *)_rp, a);
        return i;
    }

    virtual Item * do_optimize_type(Item_bool_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

};

static CItemNullcheck<Item_func::Functype::ISNULL_FUNC> ANON;
static CItemNullcheck<Item_func::Functype::ISNOTNULL_FUNC> ANON;

static class ANON : public CItemSubtypeFT<Item_func_get_system_var, Item_func::Functype::GSYSVAR_FUNC> {

    virtual RewritePlan * do_gather_type(Item_func_get_system_var *i, reason &tr, Analysis & a) const {
	reason r = reason(PLAIN_EncSet, "system var", i);
	return new RewritePlan(PLAIN_EncSet, r);
    }

    virtual Item * do_rewrite_type(Item_func_get_system_var * i,
	                           const OLK & constr, const RewritePlan * _rp,
	                           Analysis & a) const {
	return i;
    }
} ANON;


template<const char *NAME>
class CItemAdditive : public CItemSubtypeFN<Item_func_additive_op, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_additive_op *i, reason &tr, Analysis & a) const {
	return typical_gather(a, i, ADD_EncSet, "additive", tr, true);
    }
    virtual Item * do_optimize_type(Item_func_additive_op *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func_additive_op *i,
				   const OLK & constr, const RewritePlan * _rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type Item_func_additive_op" << *i << " with constr " << constr;

	//rewrite children
	assert_s(i->argument_count() == 2, " expecting two arguments for additive operator ");
	Item **args = i->arguments();

	RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

        cerr << "Rewrite plan is " << rp << "\n";

	Item * arg0 = itemTypes.do_rewrite(args[0],
					   rp->olk, rp->childr_rp[0], a);
	Item * arg1 = itemTypes.do_rewrite(args[1],
					   rp->olk, rp->childr_rp[1], a);

	EncLayer *el = getAssert(constr.key->onions, oAGG)->layers.back();
	assert_s(el->level() == SECLEVEL::HOM, "incorrect onion level on onion oHOM");
	return ((HOM*)el)->sumUDF(arg0, arg1);

	}
};

extern const char str_plus[] = "+";
static CItemAdditive<str_plus> ANON;

extern const char str_minus[] = "-";
static CItemAdditive<str_minus> ANON;

template<const char *NAME>
class CItemMath : public CItemSubtypeFN<Item_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_func *i, /* TODO reason not necessary */ reason &tr, Analysis & a) const {
	 Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
	    return a.rewritePlans.find(i)->second;
    }
    virtual Item * do_optimize_type(Item_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_mul[] = "*";
static CItemMath<str_mul> ANON;

extern const char str_div[] = "/";
static CItemMath<str_div> ANON;

extern const char str_idiv[] = "div";
static CItemMath<str_idiv> ANON;

extern const char str_sqrt[] = "sqrt";
static CItemMath<str_sqrt> ANON;

extern const char str_round[] = "round";
static CItemMath<str_round> ANON;

extern const char str_sin[] = "sin";
static CItemMath<str_sin> ANON;

extern const char str_cos[] = "cos";
static CItemMath<str_cos> ANON;

extern const char str_acos[] = "acos";
static CItemMath<str_acos> ANON;

extern const char str_pow[] = "pow";
static CItemMath<str_pow> ANON;

extern const char str_log[] = "log";
static CItemMath<str_log> ANON;

extern const char str_radians[] = "radians";
static CItemMath<str_radians> ANON;

extern const char str_if[] = "if";
static class ANON : public CItemSubtypeFN<Item_func_if, str_if> {
    virtual RewritePlan * do_gather_type(Item_func_if *i, reason &tr, Analysis & a) const {
        /*
        Item **args = i->arguments();
        assert(i->argument_count() == 3);
        analyze(args[0], a);
        analyze(args[1], a);
        analyze(args[2], a);
        return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_if *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

extern const char str_nullif[] = "nullif";
static class ANON : public CItemSubtypeFN<Item_func_nullif, str_nullif> {
    virtual RewritePlan * do_gather_type(Item_func_nullif *i, reason &tr, Analysis & a) const {
        /*
	    Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
        return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_nullif *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

extern const char str_coalesce[] = "coalesce";
static class ANON : public CItemSubtypeFN<Item_func_coalesce, str_coalesce> {
    virtual RewritePlan * do_gather_type(Item_func_coalesce *i, reason &tr, Analysis & a) const {
        /*
	 Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
	    return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_coalesce *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

extern const char str_case[] = "case";
static class ANON : public CItemSubtypeFN<Item_func_case, str_case> {
    virtual RewritePlan * do_gather_type(Item_func_case *i, reason &tr, Analysis & a) const {
	/*     Item **args = i->arguments();
        int first_expr_num = i->*rob<Item_func_case, int,
                &Item_func_case::first_expr_num>::ptr();
        int else_expr_num = i->*rob<Item_func_case, int,
                &Item_func_case::else_expr_num>::ptr();
        uint ncases = i->*rob<Item_func_case, uint,
                &Item_func_case::ncases>::ptr();

        if (first_expr_num >= 0)
            analyze(args[first_expr_num],
                    reason(EQ_EncSet, "case_first", i, &tr), a);
        if (else_expr_num >= 0)
            analyze(args[else_expr_num], tr, a);

        for (uint x = 0; x < ncases; x += 2) {
            if (first_expr_num < 0)
            analyze(args[x],
                    reason(EMPTY_EncSet, "case_nofirst", i, &tr), a);
            else
            analyze(args[x],
                    reason(EQ_EncSet, "case_w/first", i, &tr), a);
            analyze(args[x+1], tr, a);
        }
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_case *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<const char *NAME>
class CItemStrconv : public CItemSubtypeFN<Item_str_conv, NAME> {
    virtual RewritePlan * do_gather_type(Item_str_conv *i, reason & tr, Analysis & a) const {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EMPTY_EncSet, "strconv", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_str_conv *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_lcase[] = "lcase";
static CItemStrconv<str_lcase> ANON;

extern const char str_ucase[] = "ucase";
static CItemStrconv<str_ucase> ANON;

extern const char str_length[] = "length";
static CItemStrconv<str_length> ANON;

extern const char str_char_length[] = "char_length";
static CItemStrconv<str_char_length> ANON;

extern const char str_substr[] = "substr";
static CItemStrconv<str_substr> ANON;

extern const char str_concat[] = "concat";
static CItemStrconv<str_concat> ANON;

extern const char str_concat_ws[] = "concat_ws";
static CItemStrconv<str_concat_ws> ANON;

extern const char str_md5[] = "md5";
static CItemStrconv<str_md5> ANON;

extern const char str_left[] = "left";
static CItemStrconv<str_left> ANON;

extern const char str_regexp[] = "regexp";
static CItemStrconv<str_regexp> ANON;

template<const char *NAME>
class CItemLeafFunc : public CItemSubtypeFN<Item_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const {
	UNIMPLEMENTED;
    }
};

extern const char str_found_rows[] = "found_rows";
static CItemLeafFunc<str_found_rows> ANON;

extern const char str_last_insert_id[] = "last_insert_id";
static CItemLeafFunc<str_last_insert_id> ANON;

extern const char str_rand[] = "rand";
static CItemLeafFunc<str_rand> ANON;

extern const char str_database[] = "database";
static CItemLeafFunc<str_database> ANON;

static class ANON : public CItemSubtypeFT<Item_extract, Item_func::Functype::EXTRACT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_extract *i, reason &tr, Analysis & a) const {
	/* analyze(i->arguments()[0], reason(EMPTY_EncSet, "extract", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_extract *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<const char *NAME>
class CItemDateExtractFunc : public CItemSubtypeFN<Item_int_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_int_func *i, reason &tr, Analysis & a) const {
        /*Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            // assuming we separately store different date components
            analyze(args[x], tr, a);
        }
        return tr.encset;*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_int_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_second[] = "second";
static CItemDateExtractFunc<str_second> ANON;

extern const char str_minute[] = "minute";
static CItemDateExtractFunc<str_minute> ANON;

extern const char str_hour[] = "hour";
static CItemDateExtractFunc<str_hour> ANON;

extern const char str_to_days[] = "to_days";
static CItemDateExtractFunc<str_to_days> ANON;

extern const char str_year[] = "year";
static CItemDateExtractFunc<str_year> ANON;

extern const char str_month[] = "month";
static CItemDateExtractFunc<str_month> ANON;

extern const char str_dayofmonth[] = "dayofmonth";
static CItemDateExtractFunc<str_dayofmonth> ANON;

extern const char str_unix_timestamp[] = "unix_timestamp";
static CItemDateExtractFunc<str_unix_timestamp> ANON;

extern const char str_date_add_interval[] = "date_add_interval";
static class ANON : public CItemSubtypeFN<Item_date_add_interval, str_date_add_interval> {
    virtual RewritePlan * do_gather_type(Item_date_add_interval *i, reason &tr, Analysis & a) const {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            // XXX perhaps too conservative
            analyze(args[x], reason(EMPTY_EncSet, "date_add", i, &tr), a);
        }
        return tr.encset;
        */
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_date_add_interval *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<const char *NAME>
class CItemDateNow : public CItemSubtypeFN<Item_func_now, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_now *i, reason &tr, Analysis & a) const {
        UNIMPLEMENTED;
    }
};

extern const char str_now[] = "now";
static CItemDateNow<str_now> ANON;

extern const char str_utc_timestamp[] = "utc_timestamp";
static CItemDateNow<str_utc_timestamp> ANON;

extern const char str_sysdate[] = "sysdate";
static CItemDateNow<str_sysdate> ANON;

template<const char *NAME>
class CItemBitfunc : public CItemSubtypeFN<Item_func_bit, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_bit *i, reason &tr, Analysis & a) const {
        /*Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EMPTY_EncSet, "bitfunc", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_bit *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_bit_not[] = "~";
static CItemBitfunc<str_bit_not> ANON;

extern const char str_bit_or[] = "|";
static CItemBitfunc<str_bit_or> ANON;

extern const char str_bit_xor[] = "^";
static CItemBitfunc<str_bit_xor> ANON;

extern const char str_bit_and[] = "&";
static CItemBitfunc<str_bit_and> ANON;

static class ANON : public CItemSubtypeFT<Item_func_like, Item_func::Functype::LIKE_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_like *i, reason &tr,
					 Analysis & a) const {

	assert_s(false, "LIKE needs to be adapted to new RewritePlans");
	/*
	LOG(cdb_v) << "Item_func do_gather_type " << *i;

	Item **args = i->arguments();
	assert_s(i->argument_count() == 2, "expecting LIKE to have two arguments");

	if ((args[0]->type() == Item::Type::FIELD_ITEM) && (args[1]->type() == Item::Type::STRING_ITEM)) {

	    string s(args[1]->str_value.ptr(), args[1]->str_value.length());

	    if (s.find('%') == s.npos && s.find('_') == s.npos) {
                // some queries actually use LIKE as an equality check..
		return typical_gather(a, i, EQ_EncSet, "LIKE equality", tr, false, PLAIN_EncSet);

            } else {

		// XXX check if pattern is one we can support?

		reason r1;
		RewritePlan * rp1 = gather(args[0], r1, a);

		EncSet solution = rp1->es_out.intersect(Search_EncSet);

		if (solution.empty()) {
		    cerr << "cannot support " << i << " BECAUSE it needs any of " << Search_EncSet << "\n" \
			 << " but field only supports  " << rp1->es_out << "\n";
		}

		stringstream ss;
		ss << "like:'" << s << "'";
		tr = reason(PLAIN_EncSet, ss.str(), i);
		tr.add_child(r1);

		return new RewritePlan(PLAIN_OLK, solution.chooseOne(), tr);
            }
        } else {
            // we cannot support non-constant search patterns
	    assert_s(false, "we cannot support search patterns not of the form (field like const string)");
	    //for (uint x = 0; x < i->argument_count(); x++)
            //    analyze(args[x], reason(EMPTY_EncSet, "like-non-const", i, &tr), a);
        }


	*/
	return NULL;
    }
    virtual Item * do_optimize_type(Item_func_like *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
    virtual Item * do_rewrite_type(Item_func_like *i,
				   const OLK & constr, const RewritePlan *rp,
				   Analysis & a) const {
/*	LOG(cdb_v) << "Item_func_like do_rewrite_type " << *i;

	assert_s(i->argument_count() == 2, "expecting LIKE to have two arguements");
	Item **args = i->arguments();

	if ((args[0]->type() == Item::Type::FIELD_ITEM) && (args[1]->type() == Item::Type::STRING_ITEM)) {

	    string s(args[1]->str_value.ptr(), args[1]->str_value.length());

	    if (s.find('%') == s.npos && s.find('_') == s.npos) {
                // some queries actually use LIKE as an equality check..

		Item_func_like * res = new Item_func_like(args[0], args[1], NULL, false);
		do_rewrite_type_args(i, res, constr, a);

		return res;

            } else {
		// XXX check if pattern is one we can support?

		RewritePlan * plan = getAssert(a.itemRewritePlans, (Item*) i);
		auto childr_plan = getAssert(plan->plan, constr);
		OLK child_OLK = getAssert(childr_plan, args[0]);

		if (child_OLK == PLAIN_OLK) {
		    return new Item_func_like(args[0], args[1], NULL, false);
		}

		Item * field = itemTypes.do_rewrite(args[0], child_OLK, a);
		args[0]->name = NULL; //no alias

		Item * expr = args[1];
		FieldMeta * fm = child_OLK.key;

		EncLayer * el = getAssert(fm->onions, oSWP)->layers.back();
		assert_s(el->level() == SECLEVEL::SEARCH, "incorrect onion  level on onion oSWP");

		Item * res = ((Search *) el)->searchUDF(field, expr);
		cerr << "result is " << *res << "\n";

		return res;
            }
        }

	// we cannot support non-constant search patterns
	assert_s(false, "we cannot support search patterns not of the form (field like constant string)");
*/
	assert_s(false, "LIKE needs to be updated to new RewritePlan interface");
	return NULL;

    }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func, Item_func::Functype::FUNC_SP> {
    void error(Item_func *i) const __attribute__((noreturn)) {
        thrower() << "unsupported store procedure call " << *i;
    }

    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const __attribute__((noreturn)) { error(i); }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_in, Item_func::Functype::IN_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "in", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_in, Item_func::Functype::BETWEEN> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(ORD_EncSet, "between", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

//TODO: use this func in other places as well
/*static List<Item> *
getArgs(Item_func * itf) {
    List<Item> * res = new List<Item>();
    Item ** args = itf->arguments();
    for (uint j = 0; j < itf->argument_count(); j++)  {
	res->push_back(args[j]);
    }
    return res;
}
*/

template<const char *FN, class IT>
class CItemMinMax : public CItemSubtypeFN<Item_func_min_max, FN> {
    virtual RewritePlan * do_gather_type(Item_func_min_max *i, reason &tr, Analysis & a) const {
	Item **args = i->arguments();
        uint argcount = i->argument_count();
	if (argcount != 2) {
	    cerr << "expected two arguments in " << *i << "\n";
	    assert(false);
	}
	//at least one has to be a constant as we don't support join now
	assert_s(args[0]->const_item() || args[1]->const_item(), "ope join not yet implemented");

	reason r1, r2;
	RewritePlan ** childr_rp = new RewritePlan*[2];
	childr_rp[0] = gather(args[0], r1, a);
        childr_rp[1] = gather(args[1], r2, a);
	EncSet es1 = childr_rp[0]->es_out;
	EncSet es2 = childr_rp[1]->es_out;

	EncSet needed_es = ORD_EncSet;

	EncSet supported_es = needed_es.intersect(es1).intersect(es2);

	if (supported_es.empty()) {
	    cerr << "cannot support " << i << " BECAUSE it needs any of " << needed_es << "\n" \
		 << "BUT children only have (" << r1 << "\n" << r2 << ")\n";
	    assert(false);
	}

	EncSet out_es = es1.intersect(es2);

	tr = reason(out_es, "min_max func", i);
	tr.add_child(r1);
	tr.add_child(r2);

	//prepare rewrite plans
	return new RewritePlanOneOLK(out_es,
				     supported_es.chooseOne(),
				     childr_rp, tr);
    }
    virtual Item * do_optimize_type(Item_func_min_max *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func_min_max *i,
				   const OLK & constr, const RewritePlan * _rp,
				   Analysis & a) const {
	RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

	if (rp->olk == PLAIN_OLK) { // no change
	    return i;
	}

	// replace with IF( cond_arg0 cond cond_arg1, args0, args1)
	Item ** args = i->arguments();
	Item * cond_arg0 = itemTypes.do_rewrite(args[0],
						rp->olk, rp->childr_rp[0], a);
	Item * cond_arg1 = itemTypes.do_rewrite(args[1],
						rp->olk, rp->childr_rp[1], a);

	int cmp_sign = i->*rob<Item_func_min_max, int, &Item_func_min_max::cmp_sign>::ptr();

	Item * cond;
	if (cmp_sign) {
	    cond = new Item_func_gt(cond_arg0, cond_arg1);
	} else {
	    cond = new Item_func_lt(cond_arg0, cond_arg1);
	}

	return new Item_func_if(cond,
				itemTypes.do_rewrite(args[0], constr,
						     rp->childr_rp[0], a),
				itemTypes.do_rewrite(args[1], constr,
						     rp->childr_rp[1], a));
    }
};

//TODO: do we still need the file analyze.cc?

extern const char str_greatest[] = "greatest";
static CItemMinMax<str_greatest, Item_func_max> ANON;

extern const char str_least[] = "least";
static CItemMinMax<str_least, Item_func_min> ANON;


extern const char str_strcmp[] = "strcmp";
static class ANON : public CItemSubtypeFN<Item_func_strcmp, str_strcmp> {
    virtual RewritePlan * do_gather_type(Item_func_strcmp *i, reason &tr, Analysis & a) const {
        //cerr << "do_a_t Item_func_strcmp reason " << tr << "\n";
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "strcmp", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_strcmp *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<Item_sum::Sumfunctype SFT>
class CItemCount : public CItemSubtypeST<Item_sum_count, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_count *i, reason &tr, Analysis & a) const {
	/*
        if (i->has_with_distinct()) {
	    reason new_tr(tr.encset.intersect(EQ_EncSet), "count distinct", i);
            EncSet e = gather(i->get_arg(0), new_tr, a);
            if (e.intersect(EQ_EncSet).empty())
                thrower() << "count distinct";
	}
	return PLAIN_EncSet;*/
	UNIMPLEMENTED;

    }
    virtual Item * do_rewrite_type(Item_sum_count *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
/*	Item_sum_count * res = new Item_sum_count(current_thd, i);

	if (i->has_with_distinct()) {
	    rewrite_agg_args(res, a);
	}
	ItemMeta *im = getAssert(a.itemToMeta, (Item *)i);

	addToReturn(a.rmeta, a.pos++, im, false, i->name);

	return res;
*/
	UNIMPLEMENTED;
    }
};

static CItemCount<Item_sum::Sumfunctype::COUNT_FUNC> ANON;
static CItemCount<Item_sum::Sumfunctype::COUNT_DISTINCT_FUNC> ANON;

static void
check_if_empty(const EncSet & sol, Item * i, const EncSet & my_es, const reason & child_r) {
    if (sol.empty()) {
	cerr << "current crypto schemes do not support this query \n" \
	     << "BECAUSE " << i << " NEEDS " << my_es << "\n"	      \
	     << "AND children have " << child_r << "\n";
	assert(false);
    }
}

template<Item_sum::Sumfunctype SFT, class IT>
class CItemChooseOrder : public CItemSubtypeST<Item_sum_hybrid, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_hybrid *i, reason &tr, Analysis & a) const {
      assert(i->get_arg_count() == 1);
      Item *child = i->get_arg(0);
      RewritePlan **child_rp = new RewritePlan*[1];
      reason r;
      child_rp[0] = gather(child, r, a);
      EncSet es = child_rp[0]->es_out;
      EncSet needed = ORD_EncSet;
      EncSet supported = needed.intersect(es);
      check_if_empty(supported, i, needed, r);
      OLK olk = supported.chooseOne();
      EncSet out = EncSet(olk);
      tr= reason(out, "min/max", i);
      return new RewritePlanOneOLK(out, olk, child_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_sum_hybrid *i,
           const OLK & constr, const RewritePlan * rp,
           Analysis & a) const {
      list<Item *> args =
        rewrite_agg_args(i, constr, (RewritePlanOneOLK *)rp, a, 1);
      return new IT(args.front());
    }
};

static CItemChooseOrder<Item_sum::Sumfunctype::MIN_FUNC, Item_sum_min> ANON;
static CItemChooseOrder<Item_sum::Sumfunctype::MAX_FUNC, Item_sum_max> ANON;

template<Item_sum::Sumfunctype SFT>
class CItemSum : public CItemSubtypeST<Item_sum_sum, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_sum *i, reason &tr, Analysis & a) const {

	LOG(cdb_v) << "gather Item_sum_sum " << *i;
	assert_s(i->get_arg_count() == 1, "expected one argument for sum");
	Item * child_item = i->get_arg(0);
	reason child_r;
	RewritePlan ** childr_rp = new RewritePlan*[1];
	RewritePlan * child_rp = gather(child_item, child_r, a);
	childr_rp[0] = child_rp;
	EncSet child_es = child_rp->es_out;

	if (i->has_with_distinct()) {
	    UNIMPLEMENTED;
	}

	EncSet my_es = ADD_EncSet;

	EncSet solution = my_es.intersect(child_es);

	check_if_empty(solution, i, my_es, child_r);

	OLK olk = solution.chooseOne();

	EncSet return_es = EncSet(olk);
	tr = reason(return_es, "summation", i);

	return new RewritePlanOneOLK(return_es, olk, childr_rp, tr); ;

    }
    virtual Item * do_rewrite_type(Item_sum_sum * i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {

	LOG(cdb_v) << "Item_sum_sum rewrite " << *i;

	list<Item *> args = rewrite_agg_args(i, constr, (RewritePlanOneOLK *)rp, a, 1);

	FieldMeta * fm = constr.key;
	EncLayer * el = getAssert(fm->onions, oAGG, "onion oAGG not in onions")->layers.back();
	assert_s(el->level() == SECLEVEL::HOM, "incorrect onion level on onion oHOM");
	return ((HOM *)el)->sumUDA(args.front());

    }
};

//TODO: field OPE should not be blob for text either

static CItemSum<Item_sum::Sumfunctype::SUM_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::SUM_DISTINCT_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::AVG_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::AVG_DISTINCT_FUNC> ANON;

static class ANON : public CItemSubtypeST<Item_sum_bit, Item_sum::Sumfunctype::SUM_BIT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_sum_bit *i, reason &tr, Analysis & a) const {
	/* LOG(cdb_v) << "do_a_t Item_sum_bit reason " << tr;
        analyze(i->get_arg(0), reason(EMPTY_EncSet, "bitagg", i, &tr, false), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
} ANON;

static class ANON : public CItemSubtypeST<Item_func_group_concat, Item_sum::Sumfunctype::GROUP_CONCAT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_group_concat *i, reason &tr, Analysis & a) const {
	/*  LOG(cdb_v) << "do_a_t Item_func_group reason " << tr;
        uint arg_count_field = i->*rob<Item_func_group_concat, uint,
                &Item_func_group_concat::arg_count_field>::ptr();
        for (uint x = 0; x < arg_count_field; x++) {
            // XXX could perform in the proxy..
            analyze(i->get_arg(x), reason(EMPTY_EncSet, "group_concat", i, &tr), a);
        }

        // XXX order, unused in trace queries..
        return tr.encset;
    */
	UNIMPLEMENTED;
    }
    // TODO(stephentu): figure out how to rob the arg fields for optimization
} ANON;

static class ANON : public CItemSubtypeFT<Item_char_typecast, Item_func::Functype::CHAR_TYPECAST_FUNC> {
    virtual RewritePlan * do_gather_type(Item_char_typecast *i, reason &tr, Analysis & a) const {
        thrower() << "what does Item_char_typecast do?";
        UNIMPLEMENTED;
    }
} ANON;

extern const char str_cast_as_signed[] = "cast_as_signed";
static class ANON : public CItemSubtypeFN<Item_func_signed, str_cast_as_signed> {
    virtual RewritePlan * do_gather_type(Item_func_signed *i, reason &tr, Analysis & a) const {
        /*LOG(cdb_v) << "do_a_t Item_func_signed reason " << tr;
        analyze(i->arguments()[0], tr, a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_signed *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_ref, Item::Type::REF_ITEM> {
    virtual RewritePlan * do_gather_type(Item_ref *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "do_a_t Item_ref reason " << tr;
	/* if (i->ref) {
            analyze(*i->ref, tr, a);
            return tr.encset;
        } else {
            thrower() << "how to resolve Item_ref::ref?";
            UNIMPLEMENTED;
	    }*/
	UNIMPLEMENTED;
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_null, Item::Type::NULL_ITEM> {
    virtual RewritePlan * do_gather_type(Item_null *i, reason &tr, Analysis & a) const {
        tr = reason(FULL_EncSet, "is a constant", i);
        return new RewritePlan(FULL_EncSet, tr);
    }
    virtual Item * do_rewrite_type(Item_null *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        return i;
        // return encrypt_item(i, constr, a);
    }
    virtual void
    do_rewrite_insert_type(Item_null *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        for (uint j = 0; j < fm->onions.size(); ++j) {
            l.push_back(make_item(i));
        }
        if (fm->has_salt) {
            ulonglong salt = randomValue();
            l.push_back(new Item_int((ulonglong) salt));
        }
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

    dml_dispatcher = buildDMLDispatcher();
    ddl_dispatcher = buildDDLDispatcher();

    loadUDFs(ps.conn);

    // HACK: This is necessary because above functions use a USE statement.
    // ie, loadUDFs.
    ps.conn->setCurDBName(dbname);
    ps.e_conn->setCurDBName(dbname);
}

LEX **
Rewriter::dispatchAndTransformOnLex(LEX *lex, Analysis &a, const string &q,
                                    unsigned *out_lex_count) {
    if (dml_dispatcher->canDo(lex->sql_command)) {
        return dml_dispatcher->call(lex, a, q, out_lex_count);
    } else if (ddl_dispatcher->canDo(lex->sql_command)) {
        return ddl_dispatcher->call(lex, a, q, out_lex_count);
    } else {
        throw CryptDBError("Rewriter can not dispatch bad lex");
    }
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
    // FIXME.
    LEX **new_lexes =
        analysis.rewriter->dml_dispatcher->call(lex, analysis, q,
                                                &out_lex_count);
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


