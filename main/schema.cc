#include <algorithm>
#include <functional>

#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/schema.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <main/rewrite_util.hh>
#include <main/dbobject.hh>

using namespace std;

std::vector<DBMeta *>
DBMeta::doFetchChildren(Connect *e_conn, DBWriter dbw,
                        std::function<DBMeta *(std::string, std::string,
                                               std::string)>
                            deserialHandler)
{
    // Ensure the tables exist.
    assert(create_tables(e_conn, dbw));

    // Now that we know the table exists, SELECT the data we want.
    std::vector<DBMeta *> out_vec;
    DBResult *db_res;
    const std::string parent_id = std::to_string(this->getDatabaseID());
    const std::string serials_query = 
        " SELECT pdb." + dbw.table_name() + ".serial_object,"
        "        pdb." + dbw.table_name() + ".serial_object_len,"
        "        pdb." + dbw.table_name() + ".id,"
        "        pdb." + dbw.join_table_name() + ".serial_key,"
        "        pdb." + dbw.join_table_name() + ".serial_key_len"
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
        std::string child_serial_length(row[1], l[1]);
        std::string child_id(row[2], l[2]);
        std::string child_key(row[3], l[3]);
        std::string child_key_length(row[4], l[4]);

        if (child_serial.size() >
            (unsigned int)atoi(child_serial_length.c_str())) {
            child_serial.erase(atoi(child_serial_length.c_str()),
                               std::string::npos);
        }

        if (child_key.size() >
            (unsigned int)atoi(child_key_length.c_str())) {
            child_key.erase(atoi(child_key_length.c_str()),
                            std::string::npos);
        }

        DBMeta *new_old_meta =
            deserialHandler(child_key, child_serial, child_id);
        out_vec.push_back(new_old_meta);
    }

    return out_vec;
}

bool MappedDBMeta::addChild(AbstractMetaKey *key, DBMeta *meta)
{
    if (childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool MappedDBMeta::replaceChild(AbstractMetaKey *key, DBMeta *meta)
{
    if (!childExists(key)) {
        return false;
    }

    children[key] = meta;
    return true;
}

bool MappedDBMeta::destroyChild(AbstractMetaKey *key)
{
    if (!childExists(key)) {
        return false;
    }

    auto child = getChild(key);
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
MappedDBMeta::findChild(AbstractMetaKey *key) const
{
    auto it = 
        std::find_if(children.begin(), children.end(),
            [&key](const std::pair<AbstractMetaKey *, DBMeta *> child) {
                return *child.first == *key;
            });

    return it;
}

bool MappedDBMeta::childExists(AbstractMetaKey *key) const
{
    auto it = this->findChild(key);
    return children.end() != it;
}

// Slow.
DBMeta *MappedDBMeta::getChild(AbstractMetaKey *key) const
{
    for (auto it : children) {
        if (*it.first == *key) {
            return it.second;
        }
    }

    return NULL;
}

AbstractMetaKey *MappedDBMeta::getKey(const DBMeta * const child) const
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
AbstractMeta<ChildType, KeyType>::deserialize(unsigned int id,
                                              std::string serial)
{
    return new ConcreteMeta(id, serial);
}

// TODO: Debug.
template <typename ChildType, typename KeyType>
std::vector<DBMeta *>
AbstractMeta<ChildType, KeyType>::fetchChildren(Connect *e_conn)
{
    DBWriter dbw = DBWriter::factory<ChildType>(this);

    // Perhaps it's conceptually cleaner to have this lambda return
    // pairs of keys and children and then add the children from local
    // scope.
    std::function<DBMeta *(std::string, std::string, std::string)> deserialize =
        [this] (std::string key, std::string serial, std::string id) {
            AbstractMetaKey *meta_key =
                AbstractMetaKey::factory<KeyType>(key);
            DBMeta *new_old_meta =
                AbstractMeta::deserialize<ChildType>(atoi(id.c_str()),
                                                     serial);

            // Gobble the child.
            this->addChild(meta_key, new_old_meta);

            return new_old_meta;
        };

    return DBMeta::doFetchChildren(e_conn, dbw, deserialize);
}

template <typename ChildType, typename KeyType>
void
AbstractMeta<ChildType, KeyType>::applyToChildren(std::function<void(DBMeta *)> fn)
{
    for (auto it : children) {
        fn(it.second);
    }
}

OnionMeta::OnionMeta(onion o, std::vector<SECLEVEL> levels, AES_KEY *m_key,
                     Create_field *cf)
    : onionname(getpRandomName() + TypeText<onion>::toText(o))
{
    if (m_key) {         // Don't encrypt if we don't have a key.
        Create_field * newcf = cf;
        //generate enclayers for encrypted field
        string uniqueFieldName = this->getAnonOnionName();
        for (auto l: levels) {
            string key;
            key = getLayerKey(m_key, uniqueFieldName, l);

            EncLayer * el = EncLayerFactory::encLayer(o, l, newcf, key);

            Create_field * oldcf = newcf;
            newcf = el->newCreateField(oldcf);
            
            this->layers.push_back(el);

            if (oldcf != cf) {
                delete oldcf;
            }
        }
        if (newcf != cf) {
            delete newcf;
        }
    }
}

OnionMeta::OnionMeta(unsigned int id, std::string serial)
    : DBMeta(id)
{
    auto vec = unserialize_string(serial); 
    std::string parent_id;
    std::string o;
    std::string seclevel;

    parent_id = vec[0];              // ?
    this->onionname = vec[1];
    o = vec[2];                      // ?
    seclevel = vec[3];               // ?
}

std::string OnionMeta::serialize(const DBObject &parent) const
{
    // FIXME: Get onion from parent.
    onion o = oDET;
    SECLEVEL seclevel =
        static_cast<const FieldMeta&>(parent).getOnionLevel(o);
    assert(seclevel != SECLEVEL::INVALID);

    std::string serial =
        serialize_string(std::to_string(parent.getDatabaseID())) +  // ?
        serialize_string(getAnonOnionName()) +
        serialize_string(TypeText<onion>::toText(o)) +              // ?
        serialize_string(TypeText<SECLEVEL>::toText(seclevel));     // ?

    return serial;
}

std::string OnionMeta::getAnonOnionName() const
{
    return onionname;
}

// TODO: TESTME.
std::vector<DBMeta *> OnionMeta::fetchChildren(Connect *e_conn)
{
    DBWriter dbw = DBWriter::factory<EncLayer>(this);
    std::function<DBMeta *(std::string, std::string, std::string)> deserialize =
        [this] (std::string key, std::string serial, std::string id)
        -> DBMeta* {
            std::function<unsigned int(std::string)> strToInt =
                [](std::string s) {return atoi(s.c_str());};
            // > Probably going to want to use indexes in AbstractMetaKey
            // for now, otherwise you will need to abstract and rederive
            // a keyed and nonkeyed version of Delta.
            UIntMetaKey *meta_key =
                AbstractMetaKey::factory<UIntMetaKey>(key);
            const unsigned int index = meta_key->getValue();
            if (index >= this->layers.size()) {
                this->layers.resize(index + 1);
            }
            EncLayer *layer =
                EncLayerFactory::deserializeLayer(atoi(id.c_str()),
                                                  serial);
            this->layers[index] = layer;

            return layer;
        };

    // FIXME: Add sanity check to make sure that onions match
    // OnionMeta::onion_layout.
    return DBMeta::doFetchChildren(e_conn, dbw, deserialize);
}

void OnionMeta::applyToChildren(std::function<void(DBMeta *)> fn)
{
    for (auto it : layers) {
        fn(it);
    }
}

FieldMeta::FieldMeta(unsigned int id, std::string serial)
    : AbstractMeta(id)
{
    auto vec = unserialize_string(serial);
    std::string parent_id;

    parent_id = vec[0];                         // ?
    this->fname = vec[1];
    this->has_salt = string_to_bool(vec[2]);
    this->salt_name = vec[3];
    this->onion_layout = TypeText<onionlayout>::toType(vec[4]);
    this->uniq_count = atoi(vec[5].c_str());
}

FieldMeta::FieldMeta(std::string name, Create_field *field, AES_KEY *m_key,
                     unsigned long uniq_count)
    : fname(name), has_salt(static_cast<bool>(m_key)),
      salt_name(BASE_SALT_NAME + getpRandomName()), uniq_count(uniq_count)
{
    if (NULL == m_key) {
        this->onion_layout = PLAIN_ONION_LAYOUT;
    } else if (true == IsMySQLTypeNumeric(field->sql_type)) {
        this->onion_layout = NUM_ONION_LAYOUT;
    } else {
        this->onion_layout = STR_ONION_LAYOUT;
    }

    init_onions_layout(m_key, this, field);
}

std::string FieldMeta::serialize(const DBObject &parent) const
{
    std::string serial =
        serialize_string(std::to_string(parent.getDatabaseID())) +      // ?
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(getSaltName()) +
        serialize_string(TypeText<onionlayout>::toText(onion_layout)) +
        serialize_string(std::to_string(uniq_count));

   return serial;
}

string FieldMeta::stringify() const
{
    string res = " [FieldMeta " + fname + "]";
    return res;
}

TableMeta::TableMeta(unsigned int id, std::string serial)
    : AbstractMeta(id)
{
    auto vec = unserialize_string(serial);
    std::string dbname;
    
    this->anon_table_name = vec[0];
    this->hasSensitive = string_to_bool(vec[1]);
    this->has_salt = string_to_bool(vec[2]);
    this->salt_name = vec[3];
    dbname = vec[4];                // ?
    this->counter = atoi(vec[5].c_str());
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
        serialize_string(dbname) +                      // ?
        serialize_string(std::to_string(counter));
    
    return serial;
}

// FIXME: May run into problems where a plaintext table expects the regular
// name, but it shouldn't get that name from 'getAnonTableName' anyways.
std::string TableMeta::getAnonTableName() const {
    return anon_table_name;
}

std::vector<FieldMeta *> TableMeta::orderedFieldMetas() const
{
    std::vector<FieldMeta *> v;
    for (auto it : children) {
        // FIXME: Use dynamic_cast.
        v.push_back(static_cast<FieldMeta *>(it.second));
    }

    return v;
}

// TODO: Add salt.
std::string TableMeta::getAnonIndexName(std::string index_name) const
{
    std::string hash_input = anon_table_name + index_name;
    std::size_t hsh = std::hash<std::string>()(hash_input);

    return std::string("index_") + std::to_string(hsh);
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
    AbstractMetaKey *table_key = new IdentityMetaKey(table);
    TableMeta * tm = static_cast<TableMeta *>(getChild(table_key));
    if (NULL == tm) {
        return NULL;
    }

    AbstractMetaKey *field_key = new IdentityMetaKey(field);
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
        "   (serial_object VARBINARY(100) NOT NULL,"
        "    serial_object_len BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";

    assert(e_conn->execute(create_query));

    // Do the same for the JOIN table.
    const std::string join_create_query = 
        " CREATE TABLE IF NOT EXISTS pdb." + dbw.join_table_name() +
        "   (object_id BIGINT NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    serial_key VARBINARY(100) NOT NULL,"
        "    serial_key_len BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";

    assert(e_conn->execute(join_create_query));

    return true;
}

