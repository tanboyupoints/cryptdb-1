#include "schema.hh"

using namespace std;

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


bool
EncDesc::restrict(onion o, SECLEVEL maxl)
{
    //TODO:
    //assert(maxl is on onion o);

    auto it = olm.find(o);
    assert(it != olm.end());

    if (it->second > maxl) {
        it->second = maxl;
        return true;
    }

    return false;
}

EncDesc
EncDesc::intersect(EncDesc & ed) {
    OnionLevelMap new_olm = OnionLevelMap();
    for (auto it: ed.olm) {
	auto jt = olm.find(it.first);
	if (jt != olm.end()) {
	    new_olm[it.first] = min(it.second, jt->second);
	}
    }
    return EncDesc(new_olm);
}
