#include "schema.hh"
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>

using namespace std;

ostream&
operator<<(ostream &out, const OnionLevelFieldPair &p)
{
    out << "(onion " << p.first
        << ", level " << levelnames[(int)p.second.first]
        << ", field `" << (p.second.second == NULL ? "*" : p.second.second->fname) << "`"
        << ")";
    return out;
}

std::ostream&
operator<<(std::ostream &out, const OLK &olk)
{
    out << "( onion " << olk.o << " level " << levelnames[(uint)olk.l] << " fieldmeta ";
    if (olk.key == NULL) {
	out << " NULL ";
    } else {
	out << olk.key->fname;
    }
    out << ")";
    return out;
}

std::string OnionMeta::getAnonOnionName() const
{
    return onionname;
}

FieldMeta::FieldMeta(TableMeta *tm, std::string name, unsigned int uniq,
                     Create_field *field, AES_KEY *mKey)
    : tm(tm), fname(name), uniq(uniq)
{
    if (mKey) {
        init_onions(mKey, this, field, this->uniq);
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

std::string FieldMeta::saltName() const
{
    assert(has_salt);

    return BASE_SALT_NAME + "_f_" + StringFromVal(uniq) + "_" +
           tm->getAnonTableName();
}

std::string FieldMeta::fullName(onion o) const
{
    auto it = onions.find(o);
    OnionMeta *om = it->second;
    assert(om);
    return tm->getAnonTableName() + "." + om->getAnonOnionName();
}

string FieldMeta::stringify() const
{
    string res = " [FieldMeta " + fname + "]";
    return res;
}

TableMeta::~TableMeta()
{
    auto cp = fieldMetaMap;
    fieldMetaMap.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

FieldMeta *
TableMeta::createFieldMeta(Create_field *field, const Analysis &a,
                           bool encByDefault)
{
    FieldMeta * fm = new FieldMeta(this, string(field->field_name),
                                   this->leaseUniqCounter(), field,
                                   a.ps->masterKey); 

    // FIXME: Use exists function.
    if (this->fieldMetaMap.find(fm->fname) != this->fieldMetaMap.end()) {
        return NULL;
    }

    this->fieldMetaMap[fm->fname] = fm;
    this->fieldNames.push_back(fm->fname);//TODO: do we need fieldNames?

    return fm;
}

FieldMeta *TableMeta::getFieldMeta(std::string field)
{
    auto it = fieldMetaMap.find(field);
    if (fieldMetaMap.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

unsigned int TableMeta::getUniqCounter() const
{
    return uniq_counter;
}

// FIXME: May run into problems where a plaintext table expects the regular
// name, but it shouldn't get that name from 'getAnonTableName' anyways.
std::string TableMeta::getAnonTableName() const {
    return std::string("table") + strFromVal((uint32_t)tableNo);
}

bool TableMeta::destroyFieldMeta(std::string field)
{
    FieldMeta *fm = this->getFieldMeta(field);
    if (NULL == fm) {
        return false;
    }

    auto erase_count = fieldMetaMap.erase(field);
    fieldNames.remove(field);

    delete fm;
    return 1 == erase_count;
}

std::string TableMeta::addIndex(std::string index_name)
{
    std::string anon_name =
        std::string("index") + std::to_string(leaseUniqCounter()) +
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

unsigned int TableMeta::leaseUniqCounter()
{
    return uniq_counter++;
}

SchemaInfo::~SchemaInfo()
{
    auto cp = tableMetaMap;
    tableMetaMap.clear();

    for (auto it : cp) {
        delete it.second;
    }
}

// table_no: defaults to NULL indicating we are to generate it ourselves.
TableMeta *
SchemaInfo::createTableMeta(std::string table_name,
                            bool has_sensitive, bool has_salt,
                            std::string salt_name,
                            std::map<std::string, std::string> index_map,
                            unsigned int uniq_counter,
                            const unsigned int *table_no)
{
    // Make sure a table with this name does not already exist.
    if (this->tableMetaExists(table_name)) {
        return NULL;
    }

    unsigned int table_number;
    if (NULL == table_no) {
        table_number = totalTables++;
    } else {
        // TODO: Make sure no other tables with this number exist.
        ++totalTables;
        table_number = *table_no;
    }
    TableMeta *tm = new TableMeta(table_number, has_sensitive, has_salt,
                                  salt_name, index_map, uniq_counter);
    tableMetaMap[table_name] = tm;
    return tm;
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

    return tm->getFieldMeta(field);
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

    if (totalTables <= 0) {
        throw CryptDBError("SchemaInfo::totalTables can't be less than zero");
    }

    --totalTables;
    delete tm;

    return 1 == tableMetaMap.erase(table);
}

