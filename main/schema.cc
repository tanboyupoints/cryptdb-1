#include <algorithm>

#include "schema.hh"
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <main/rewrite_util.hh>
#include <main/dbobject.hh>

using namespace std;

bool DBMeta::addChild(AbstractMetaKey *key, DBMeta *meta)
{
    if (childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool DBMeta::replaceChild(AbstractMetaKey *key, DBMeta *meta)
{
    if (!childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool DBMeta::destroyChild(AbstractMetaKey *key)
{
    if (!childExists(key)) {
        return false;
    }

    auto child = children[key];
    auto erase_count = children.erase(key);
    if (1 == erase_count) {
        delete child;
        return true;
    } else if (0 == erase_count) {
        return false;
    } else {
        throw CryptDBError("Bad erase amount in destroyChild!");
    }
}

std::map<AbstractMetaKey *, DBMeta *>::const_iterator
DBMeta::findChild(AbstractMetaKey *key) const
{
    auto it = 
        std::find_if(children.begin(), children.end(),
            [&key](const std::pair<AbstractMetaKey *, DBMeta *> child) {
                return *child.first == *key;
            });

    return it;
}

bool DBMeta::childExists(AbstractMetaKey *key) const
{
    auto it = this->findChild(key);
    return children.end() != it;
}

DBMeta *DBMeta::getChild(AbstractMetaKey *key) const
{
    auto it = this->findChild(key);
    if (children.end() == it) {
        return NULL;
    }

    return it->second;
}

AbstractMetaKey *DBMeta::getKey(const DBMeta * const child) const
{
    for (auto it : children) {
        if (it.second == child) {
            return it.first;
        }
    }

    throw CryptDBError("reverse lookup failed to find the child's key!");
}

template <typename ChildType, typename KeyType>
template <typename ConcreteMeta> ConcreteMeta *
AbstractMeta<ChildType, KeyType>::deserialize(std::string serial)
{
    return new ConcreteMeta(serial);
}

// TODO: Debug.
template <typename ChildType, typename KeyType>
std::vector<std::pair<AbstractMetaKey *, DBMeta *>>
AbstractMeta<ChildType, KeyType>::fetchChildren(Connect *e_conn)
{
    DBWriter dbw = DBWriter::factory<ChildType>(this);
    // Ensure the tables exist.
    assert(create_tables(e_conn, dbw));

    // Now that we know the table exists, SELECT the data we want.
    std::vector<std::pair<AbstractMetaKey *, DBMeta *>> out_vec;
    DBResult *db_res;
    const std::string parent_id = std::to_string(this->getDatabaseID());
    const std::string serials_query = 
        " SELECT pdb." + dbw.table_name() + ".serial_object,"
        "        pdb." + dbw.join_table_name() + ".serial_key"
        " FROM pdb." + dbw.table_name() + 
        "   INNER JOIN pdb." + dbw.join_table_name() +
        "       ON (pdb." + dbw.table_name() + ".id"
        "       =   pdb." + dbw.join_table_name() + ".object_id)"
        " WHERE pdb." + dbw.join_table_name() + ".parent_id"
        "   = " + parent_id + ";";
    assert(e_conn->execute(serials_query, db_res));
    ScopedMySQLRes r(db_res->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        std::string child_serial(row[0], l[0]);
        std::string child_key(row[1], l[1]);

        AbstractMetaKey *key = MetaKey<KeyType>::deserialize(child_key);
        DBMeta *new_old_meta =
            AbstractMeta::deserialize<ChildType>(child_serial);
        out_vec.push_back(std::pair<AbstractMetaKey *, DBMeta *>(key, new_old_meta));
    }

    return out_vec;
}

// TODO: Implement deserialization.
OnionMeta::OnionMeta(std::string serial)
{
   unserialize_string(serial); 
}

std::string OnionMeta::serialize(const DBObject &parent) const
{
    // FIXME: Get onion from parent.
    onion o = oDET;
    SECLEVEL seclevel =
        static_cast<const FieldMeta&>(parent).getOnionLevel(o);
    assert(seclevel != SECLEVEL::INVALID);

    std::string serial =
        serialize_string(std::to_string(parent.getDatabaseID())) +
        serialize_string(getAnonOnionName()) +
        serialize_string(TypeText<onion>::toText(o)) +
        serialize_string(TypeText<SECLEVEL>::toText(seclevel));

    return serial;
}

std::string OnionMeta::getAnonOnionName() const
{
    return onionname;
}

// TODO: Implement deserialization.
FieldMeta::FieldMeta(std::string serial)
{
    
}

// TODO: Implement serialization.
std::string FieldMeta::serialize(const DBObject &parent) const
{
    std::string serial =
        serialize_string(std::to_string(parent.getDatabaseID())) +
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(getSaltName()) +
        serialize_string(TypeText<onionlayout>::toText(onion_layout));

   return serial;
}

FieldMeta::FieldMeta(std::string name, Create_field *field, AES_KEY *mKey)
    : fname(name), salt_name(BASE_SALT_NAME + getpRandomName())
{
    if (mKey) {
        init_onions(mKey, this, field);
    } else {
        init_onions(NULL, this, field);
    }
}

FieldMeta::~FieldMeta()
{
    auto cp = onions;
    onions.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

string FieldMeta::stringify() const
{
    string res = " [FieldMeta " + fname + "]";
    return res;
}

// TODO: Implement deserialization.
TableMeta::TableMeta(std::string serial)
{

}

std::string TableMeta::serialize(const DBObject &parent) const
{
    // HACK: Need to get this information from parent.
    std::string dbname = "cryptdbtest";

    std::string serial = 
        // FIXME: Do anon_table_name => table_name translation at parent.
        // serialize_string(table) +
        serialize_string(getAnonTableName()) +
        serialize_string(bool_to_string(hasSensitive)) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(salt_name) +
        serialize_string(dbname);
    
    return serial;
}

// TODO: @fieldNames is a blight. Use a counter.
bool TableMeta::addChild(AbstractMetaKey *key, DBMeta *meta)
{
    bool status = AbstractMeta::addChild(key, meta);
    if (false == status) {
        return false;
    }
    
    this->fieldNames.push_back(key);//TODO: do we need fieldNames?

    return true;
}

// FIXME: May run into problems where a plaintext table expects the regular
// name, but it shouldn't get that name from 'getAnonTableName' anyways.
std::string TableMeta::getAnonTableName() const {
    return anon_table_name;
}

bool TableMeta::destroyChild(AbstractMetaKey *key)
{
    fieldNames.remove(key);
    bool status = AbstractMeta::destroyChild(key);
    if (false == status) {
        throw CryptDBError("Failed to destroy FieldMeta!");
    }

    return true;
}

std::string TableMeta::addIndex(std::string index_name)
{
    std::string anon_name =
        std::string("index") + getpRandomName() +
        getAnonTableName();
    auto it = index_map.find(index_name);
    assert(index_map.end() == it);

    return index_map[index_name] = anon_name;
}

std::string TableMeta::getAnonIndexName(std::string index_name) const
{
    auto it = index_map.find(index_name);
    assert(index_map.end() != it);
    return it->second;
}

// Reverse lookup.
std::string TableMeta::getIndexName(std::string anon_name) const
{
    for (auto it : index_map) {
        if (it.second == anon_name) {
            return it.first;
        }
    }

    assert(false);
}

bool TableMeta::destroyIndex(std::string index_name)
{
    auto it = index_map.find(index_name);
    assert(index_map.end() != it);

    return 1 == index_map.erase(index_name);
}

SchemaInfo::~SchemaInfo()
{
    auto cp = tableMetaMap;
    tableMetaMap.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

FieldMeta *
SchemaInfo::getFieldMeta(std::string & table, std::string & field) const
{
    AbstractMetaKey *table_key = new MetaKey<std::string>(table);
    TableMeta * tm = static_cast<TableMeta *>(getChild(table_key));
    if (NULL == tm) {
        return NULL;
    }

    AbstractMetaKey *field_key = new MetaKey<std::string>(field);
    return static_cast<FieldMeta *>(tm->getChild(field_key));
}

bool create_tables(Connect *e_conn, DBWriter dbw)
{
    // FIXME: Elsewhere.
    const std::string create_db =
        " CREATE DATABASE IF NOT EXISTS pdb;";
    
    assert(e_conn->execute(create_db));

    const std::string create_query =
        " CREATE TABLE IF NOT EXISTS pdb." + dbw.table_name() +
        "   (serial_object VARCHAR(100) NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";

    assert(e_conn->execute(create_query));

    // Do the same for the JOIN table.
    const std::string join_create_query = 
        " CREATE TABLE IF NOT EXISTS pdb." + dbw.join_table_name() +
        "   (object_id bigint NOT NULL,"
        "    parent_id bigint NOT NULL,"
        "    serial_key varchar(100) NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";

    assert(e_conn->execute(join_create_query));

    return true;
}

