#include "schema.hh"

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


string FieldMeta::stringify() {
    string res = " [FieldMeta " + fname + "]";
    return res;
}

FieldMeta::FieldMeta()
{
    fname = "";
    sql_field = NULL;
    salt_name = "";
    has_salt = true;

}

TableMeta::TableMeta() {
    anonTableName = "";
    tableNo = 0;
}

TableMeta::~TableMeta()
{
    for (auto i = fieldMetaMap.begin(); i != fieldMetaMap.end(); i++)
        delete i->second;

}

TableMeta *
SchemaInfo::getTableMeta(const string & table) {
    auto it = tableMetaMap.find(table);
    assert_s(it != tableMetaMap.end(), "could not find table " + table);
    return it->second;
}

FieldMeta *
SchemaInfo::getFieldMeta(const string & table, const string & field) {
    TableMeta * tm = getTableMeta(table);
    auto it = tm->fieldMetaMap.find(field);
    assert_s(it != tm->fieldMetaMap.end(), "could not find field " + field + " in table " +  table );
    assert_s(it->second != NULL, "field " + table + "." + field + " not present in proxy schema ");
    return it->second;
}
