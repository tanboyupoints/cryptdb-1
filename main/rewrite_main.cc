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
SchemaInfo *
loadSchemaInfo(Connect *e_conn);

/*
static void
buildTableMeta(ProxyState &ps);

static void
buildFieldMeta(ProxyState &ps, TableMeta *tm, string database_name);

static void
buildOnionMeta(ProxyState &ps, FieldMeta *fm, int field_id);
*/

//TODO: rewrite_proj may not need to be part of each class;
// it just does gather, choos and then rewrite

static Item_field *
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

/*
static void
createMetaTablesIfNotExists(ProxyState & ps)
{
    ostringstream s;

    assert(ps.e_conn->execute("CREATE DATABASE IF NOT EXISTS pdb"));

    assert(ps.e_conn->execute(
                " CREATE TABLE IF NOT EXISTS pdb.table_info"
                " (name varchar(64) NOT NULL UNIQUE,"
                "  anon_name varchar(64) NOT NULL UNIQUE,"
                "  has_sensitive boolean,"
                "  has_salt boolean,"
                "  salt_name varchar(64) NOT NULL,"
                "  database_name varchar(64) NOT NULL,"
                "  uniq_counter bigint NOT NULL,"
                "  id SERIAL PRIMARY KEY)"
                " ENGINE=InnoDB;"));

    s << " CREATE TABLE IF NOT EXISTS pdb.field_info"
      << " (table_info_id bigint NOT NULL," // Foreign key.
      << "  name varchar(64) NOT NULL,"
      << "  has_salt boolean,"
      << "  salt_name varchar(64) NOT NULL,"
      << "  onion_layout enum"
      << " " << TypeText<onionlayout>::parenList().c_str() << " NOT NULL,"
      << "  uniq_count bigint NOT NULL,"
      << "  id SERIAL PRIMARY KEY)"
      << " ENGINE=InnoDB;";

    assert(ps.e_conn->execute(s.str()));
    s.str("");
    s.clear();

    s << " CREATE TABLE IF NOT EXISTS pdb.onion_info"
      << " (field_info_id bigint NOT NULL," // Foreign key.
      << "  name varchar(64) NOT NULL UNIQUE,"
      << "  type enum"
      << " " << TypeText<onion>::parenList() << " NOT NULL,"
      << "  current_level enum"
      << " " << TypeText<SECLEVEL>::parenList() << " NOT NULL,"
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
    s.str("");
    s.clear();

    return;
}
*/

static bool
sanityCheck(FieldMeta *fm)
{
    for (auto it : fm->children) {
        // FIXME: dynamic_cast
        OnionMeta *om = static_cast<OnionMeta *>(it.second);
        onion o = static_cast<OnionMetaKey *>(it.first)->getValue();
        std::vector<SECLEVEL> secs = fm->onion_layout[o];
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
        // FIXME: dynamic_cast
        FieldMeta *fm = static_cast<FieldMeta *>(it.second);
        assert(sanityCheck(fm));
    }
    return true;
}

static bool
sanityCheck(SchemaInfo *schema)
{
    for (auto it : schema->children) {
        // FIXME: dynamic_cast
        TableMeta *tm = static_cast<TableMeta *>(it.second);
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
SchemaInfo *
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

/*
static void
createInMemoryTables(ProxyState & ps)
{
    buildTableMeta(ps);
    return;
}

static void
buildTableMeta(ProxyState &ps)
{

    DBResult *dbres;
    assert(ps.e_conn->execute(
                " SELECT name, anon_name, has_sensitive, has_salt," 
                "        salt_name, database_name, uniq_counter"
                " FROM pdb.table_info", dbres));
    ScopedMySQLRes r(dbres->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string table_name(row[0], l[0]);
        string table_anon_name(row[1], l[1]);
        string table_has_sensitive(row[2], l[2]);
        string table_has_salt(row[3], l[3]);
        string table_salt_name(row[4], l[4]);
        string table_database_name(row[5], l[5]);
        string table_uniq_counter(row[6], l[6]);

        unsigned long counter = atoi(table_uniq_counter.c_str());

        TableMeta *tm = new TableMeta(string_to_bool(table_has_sensitive),
                                      string_to_bool(table_has_salt),
                                      table_salt_name, table_anon_name,
                                      counter);
        IdentityMetaKey *key = new IdentityMetaKey(table_name);
        assert(ps.schema->addChild(key, tm));

        buildFieldMeta(ps, tm, table_database_name);
    }

    return;
}

static void
buildFieldMeta(ProxyState &ps, TableMeta *tm, string database_name)
{

    string q = " SELECT f.name, f.has_salt, f.salt_name, f.onion_layout,"
               "        f.uniq_count, f.id"
               " FROM pdb.table_info t, pdb.field_info f"
               " WHERE t.database_name = '" + database_name + "' "
               "   AND t.id = f.table_info_id;";

    DBResult *dbRes;
    assert(ps.e_conn->execute(q, dbRes));

    ScopedMySQLRes r(dbRes->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        string field_name(row[0], l[0]);
        string field_has_salt(row[1], l[1]);
        string field_salt_name(row[2], l[2]);
        string field_onion_layout(row[3], l[3]);
        string field_uniq_count(row[4], l[4]);
        string field_id(row[5], l[5]);

        bool has_salt = string_to_bool(field_has_salt);
        onionlayout onion_layout = 
            TypeText<onionlayout>::toType(field_onion_layout);
        long unsigned uniq_count = atoi(field_uniq_count.c_str());

        FieldMeta *fm =
            new FieldMeta(field_name, has_salt, field_salt_name,
                          onion_layout, uniq_count);

        IdentityMetaKey *key = new IdentityMetaKey(field_name);
        assert(tm->addChild(key, fm));

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

    string q = " SELECT o.name, o.type, o.current_level, o.id"
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
        string onion_id(row[3], l[3]);

        onion o = TypeText<onion>::toType(onion_type);
        OnionMeta *om = new OnionMeta(onion_name);
        OnionMetaKey *key = new OnionMetaKey(o);
        fm->addChild(key, om);

        // Add elements to OnionMeta.layers starting with the bottom layer
        // and stopping at the current level.
        std::map<SECLEVEL, std::string> layer_serial =
            get_layer_keys(ps, o, atoi(onion_id.c_str()));
        std::vector<SECLEVEL> layers = fm->onion_layout[o];
        SECLEVEL current_level =
            TypeText<SECLEVEL>::toType(onion_current_level);
        for (auto it: layers) {
            EncLayer *enc_layer =
		EncLayerFactory::deserializeLayer(layer_serial[it]);

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
*/

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
    printEC(ps.e_conn, "selecT * from pdb.tableMeta_schemaInfo;");
    printEC(ps.e_conn, "select * from pdb.tableMeta;");
    printEC(ps.e_conn, "selecT * from pdb.fieldMeta_tableMeta;");
    printEC(ps.e_conn, "select * from pdb.fieldMeta;");
    printEC(ps.e_conn, "selecT * from pdb.onionMeta_fieldMeta;");
    printEC(ps.e_conn, "select * from pdb.onionMeta;");
    printEC(ps.e_conn, "selecT * from pdb.encLayer_onionMeta;");
    printEC(ps.e_conn, "select * from pdb.encLayer;");
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


/*
static void
initSchema(ProxyState & ps)
{
    createMetaTablesIfNotExists(ps);

    printEmbeddedState(ps);

    createInMemoryTables(ps);

    return;
}
*/

//l gets updated to the new level
static void
removeOnionLayer(FieldMeta * fm, Item_field * itf, Analysis & a, onion o, SECLEVEL & new_level, const string & cur_db) {

    OnionMeta *om = fm->getOnionMeta(o);
    assert(om);
    string fieldanon  = om->getAnonOnionName();
    string tableanon  = a.getTableMeta(itf->table_name)->getAnonTableName();

    //removes onion layer at the DB
    stringstream query;
    query << "UPDATE " << tableanon << " SET " << fieldanon  << " = ";

    Item_field *field = stringToItemField(fieldanon, tableanon, itf);
    Item_field *salt = stringToItemField(fm->getSaltName(), tableanon, itf);
    Item * decUDF = om->layers.back()->decryptUDF(field, salt);

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
    return decrypt_item_layers(i, o, fm->getOnionMeta(o)->layers, IV, a,
                               fm, res);
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

    // Must be called before initSchema.
    buildTypeTextTranslator();
    // initSchema(ps);
    ps.schema = loadSchemaInfo(ps.e_conn);
    // printEmbeddedState(ps);

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
    const SQLHandler *handler;
    if (dml_dispatcher->canDo(lex)) {
        handler = dml_dispatcher->dispatch(lex);
    } else if (ddl_dispatcher->canDo(lex)) {
        handler = ddl_dispatcher->dispatch(lex);
    } else {
        throw CryptDBError("Rewriter can not dispatch bad lex");
    }

    return handler->transformLex(lex, a, q, out_lex_count);
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
    // TODO: Support ENC keyword in query.
    assert(false);
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
        analysis.rewriter->dispatchAndTransformOnLex(lex, analysis, q,
                                                     &out_lex_count);
    assert(new_lexes && out_lex_count != 0);

    // FIXME: This subsection needs to be moved around to fit our new
    // execution scheme.
    // --------------------------------------
    // HACK: To determine if we have a DDL.
    /*
    if (analysis.deltas.size() > 0) {
        assert(analysis.ps->e_conn->execute("use pdb;"));
        assert(analysis.ps->e_conn->execute(q));
    }
    */
    for (auto it : analysis.deltas) {
        assert(it.apply(analysis.ps->e_conn));
    }

    analysis.ps->schema = loadSchemaInfo(analysis.ps->e_conn);
    printEmbeddedState(*analysis.ps);
    // --------------------------------------

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
	 }  catch (CryptDBError &e) {
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


