#include "schema.hh"
#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/rewrite_main.hh>

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

string FieldMeta::stringify() {
    string res = " [FieldMeta " + fname + "]";
    return res;
}

FieldMeta::FieldMeta()
{
    fname = "";
    has_salt = true;
}

FieldMeta::~FieldMeta()
{
    for (auto onion_it : onions) {
        delete onion_it.second;
    }
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

FieldMeta *TableMeta::getFieldMeta(std::string field)
{
    auto it = fieldMetaMap.find(field);
    if (fieldMetaMap.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

unsigned int TableMeta::getIndexCounter() const
{
    return index_counter;
}

// If mkey == NULL, the field is not encrypted
static void
init_onions_layout(AES_KEY * mKey, FieldMeta * fm, uint index,
                   Create_field * cf, onionlayout ol) {

    fm->onions.clear();

    // This additional reflection is needed as we must rebuild the
    // OnionMeta's (and their layers) after a restart.
    fm->onion_layout = ol;

    for (auto it: ol) {
        onion o = it.first;
        OnionMeta * om = new OnionMeta(o, index, fm->fname);
        fm->onions[o] = om;

        if (mKey) {
	    Create_field * newcf = cf;
            //generate enclayers for encrypted field
            string uniqueFieldName = fm->fullName(o);
            for (auto l: it.second) {
                string key;
                key = getLayerKey(mKey, uniqueFieldName, l);

		EncLayer * el = EncLayerFactory::encLayer(o, l, newcf, key);

		Create_field * oldcf = newcf;
		newcf = el->newCreateField();
		
                om->layers.push_back(el);

		if (oldcf != cf) {
		    delete oldcf;
		}
            }
        }

        LOG(cdb_v) << "adding onion layer " << om->getAnonOnionName() << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

// @index: default value 0
static void
init_onions(AES_KEY * mKey, FieldMeta * fm, Create_field * cf,
            uint index=0) {
    if (!mKey) {
        // unencrypted field
        init_onions_layout(NULL, fm, 0, cf, PLAIN_ONION_LAYOUT);
        fm->has_salt = false;
        return;
    }

    // Encrypted field

    fm->has_salt = true;

    if (IsMySQLTypeNumeric(cf->sql_type)) {
        init_onions_layout(mKey, fm, index, cf, NUM_ONION_LAYOUT);
    } else {
        init_onions_layout(mKey, fm, index, cf, STR_ONION_LAYOUT);
    }
}

FieldMeta *
TableMeta::createFieldMeta(Create_field *field, const Analysis &a,
                           bool encByDefault)
{
    FieldMeta * fm = new FieldMeta();

    fm->tm            = this;
    fm->fname         = string(field->field_name);
    // FIXME: This does not play nice with field deletion.
    fm->uniq          = this->fieldNames.size();

    if (encByDefault) {
        init_onions(a.ps->masterKey, fm, field, fm->uniq);
    } else {
        init_onions(NULL, fm, field);
    }

    // FIXME: Use exists function.
    if (this->fieldMetaMap.find(fm->fname) != this->fieldMetaMap.end()) {
        return NULL;
    }

    this->fieldMetaMap[fm->fname] = fm;
    this->fieldNames.push_back(fm->fname);//TODO: do we need fieldNames?

    return fm;
}

TableMeta::TableMeta() {
    tableNo = 0;
    index_counter = 0;
}

TableMeta::~TableMeta()
{
    for (auto i = fieldMetaMap.begin(); i != fieldMetaMap.end(); i++)
        delete i->second;

}

// FIXME: May run into problems where a plaintext table expects the regular
// name, but it shouldn't get that name from 'getAnonTableName' anyways.
std::string TableMeta::getAnonTableName() const {
    return std::string("table") + strFromVal((uint32_t)tableNo);
}

std::string TableMeta::addIndex(std::string index_name)
{
    std::string anon_name =
        std::string("index") + std::to_string(index_counter++) +
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

// table_no: defaults to NULL indicating we are to generate it ourselves.
TableMeta *
SchemaInfo::createTableMeta(std::string table_name,
                            bool has_sensitive, bool has_salt,
                            std::string salt_name,
                            std::map<std::string, std::string> index_map,
                            unsigned int index_counter,
                            const unsigned int *table_no)
{
    assert(index_map.empty() || index_counter > 0);

    // Make sure a table with this name does not already exist.
    std::map<std::string, TableMeta *>::iterator it =
        tableMetaMap.find(table_name);
    if (tableMetaMap.end() != it) {
        return NULL;
    }

    unsigned int table_number;
    if (NULL == table_no) {
        table_number = ++totalTables;
    } else {
        // TODO: Make sure no other tables with this number exist.
        ++totalTables;
        table_number = *table_no;
    }
    TableMeta *tm = new TableMeta(table_number, has_sensitive, has_salt,
                                  salt_name, index_map, index_counter);
    tableMetaMap[table_name] = tm;
    return tm;
}

TableMeta *
SchemaInfo::getTableMeta(const string & table) {
    auto it = tableMetaMap.find(table);
    if (tableMetaMap.end() == it) {
        return NULL;
    } else {
        return it->second;
    }
}

FieldMeta *
SchemaInfo::getFieldMeta(const string & table, const string & field) {
    TableMeta * tm = getTableMeta(table);
    if (NULL == tm) {
        return NULL;
    }

    return tm->getFieldMeta(field);
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

