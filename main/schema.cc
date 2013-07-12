#include <algorithm>

#include "schema.hh"
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <main/rewrite_util.hh>

using namespace std;

static std::string
serialize_string(std::string str)
{
    return std::string(std::to_string(str.length()) + "_" + str);
}

// TESTME.
// Must perserve order.
static std::vector<std::string>
unserialize_string(std::string serial)
{
    std::vector<std::string> output;
    std::size_t start = 0;
    std::size_t under_pos = serial.find_first_of("_");
    while (under_pos != std::string::npos) {
        std::size_t length =
            atoi(serial.substr(start, under_pos-start).c_str());
        output.push_back(serial.substr(under_pos+1, length)); 
        start = under_pos + 1 + length;
        under_pos = serial.find_first_of("_", start);
    }

    // TODO: Sanity check no leftover characters.

    return output;
}

template <typename ConcreteMeta> ConcreteMeta *
AbstractMeta::deserialize(std::string serial)
{
    return new ConcreteMeta(serial);
}

bool AbstractMeta::childExists(const MetaKey &key) const
{
    return children.find(key) != children.end();
}

AbstractMeta *AbstractMeta::getChild(const MetaKey &key) const
{
    auto it = children.find(key);
    if (children.end() == it) {
        return NULL;
    }

    return it->second;
}

bool AbstractMeta::addChild(const MetaKey &key, AbstractMeta *meta)
{
    if (childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool AbstractMeta::replaceChild(const MetaKey &key, AbstractMeta *meta)
{
    if (!childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool AbstractMeta::destroyChild(const MetaKey &key)
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

// TODO: Implement.
AbstractMeta ** AbstractMeta::doFetchChildren(unsigned int *count)
{
    return NULL;
}

// TODO: Implement deserialization.
OnionMeta::OnionMeta(std::string serial)
{
   unserialize_string(serial); 
}

MetaSerial OnionMeta::serialize(AbstractMeta *parent) const
{
    // FIXME: Get onion from parent.
    onion o = oDET;
    SECLEVEL seclevel = ((FieldMeta*)parent)->getOnionLevel(o);
    assert(seclevel != SECLEVEL::INVALID);

    std::string serial =
        serialize_string(std::to_string(parent->getDatabaseID())) +
        serialize_string(getAnonOnionName()) +
        serialize_string(TypeText<onion>::toText(o)) +
        serialize_string(TypeText<SECLEVEL>::toText(seclevel));

    return MetaSerial(serial, this->getDatabaseID());
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
MetaSerial FieldMeta::serialize(AbstractMeta *parent) const
{
    std::string serial =
        serialize_string(std::to_string(parent->getDatabaseID())) +
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(getSaltName()) +
        serialize_string(TypeText<onionlayout>::toText(onion_layout));

   return MetaSerial(serial, this->getDatabaseID());
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

MetaSerial TableMeta::serialize(AbstractMeta *parent) const
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
    
    return MetaSerial(serial, this->getDatabaseID());
}

// TODO: @fieldNames is a blight. Use a counter.
bool TableMeta::addChild(const MetaKey &key, AbstractMeta *meta)
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

bool TableMeta::destroyChild(const MetaKey &key)
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

bool
SchemaInfo::addTableMeta(std::string name, TableMeta *tm)
{
    if (this->tableMetaExists(name)) {
        return false;
    }

    tableMetaMap[name] = tm;
    return true;
}

TableMeta *
SchemaInfo::getTableMeta(const string & table) const
{
    auto it = tableMetaMap.find(table);
    if (tableMetaMap.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

FieldMeta *
SchemaInfo::getFieldMeta(const string & table, const string & field) const
{
    TableMeta * tm = getTableMeta(table);
    if (NULL == tm) {
        return NULL;
    }

    return static_cast<FieldMeta*>(tm->getChild(field));
}

bool
SchemaInfo::tableMetaExists(std::string table) const
{
    return tableMetaMap.find(table) != tableMetaMap.end();
}

bool
SchemaInfo::destroyTableMeta(std::string table)
{
    TableMeta *tm = this->getTableMeta(table);
    if (NULL == tm) {
        return false;
    }

    if (1 == tableMetaMap.erase(table)) {
        delete tm;
        return true;
    }

    return false;
}

