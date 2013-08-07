#include <algorithm>
#include <functional>

#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/schema.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <main/rewrite_util.hh>
#include <main/dbobject.hh>
#include <main/metadata_tables.hh>

std::vector<DBMeta *>
DBMeta::doFetchChildren(Connect *e_conn,
                        std::function<DBMeta *(std::string, std::string,
                                               std::string)>
                            deserialHandler)
{
    const std::string table_name = MetaDataTables::Name::metaObject();

    // Now that we know the table exists, SELECT the data we want.
    std::vector<DBMeta *> out_vec;
    DBResult *db_res;
    const std::string parent_id = std::to_string(this->getDatabaseID());
    const std::string serials_query = 
        " SELECT pdb." + table_name + ".serial_object,"
        "        pdb." + table_name + ".serial_key,"
        "        pdb." + table_name + ".id"
        " FROM pdb." + table_name + 
        " WHERE pdb." + table_name + ".parent_id"
        "   = " + parent_id + ";";
    assert(e_conn->execute(serials_query, db_res));
    ScopedMySQLRes r(db_res->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        const std::string child_serial_object(row[0], l[0]);
        const std::string child_key(row[1], l[1]);
        const std::string child_id(row[2], l[2]);

        DBMeta *new_old_meta =
            deserialHandler(child_key, child_serial_object, child_id);
        out_vec.push_back(new_old_meta);
    }

    return out_vec;
}

OnionMeta::OnionMeta(onion o, std::vector<SECLEVEL> levels,
                     const AES_KEY * const m_key,
                     Create_field *cf, unsigned long uniq_count)
    : onionname(getpRandomName() + TypeText<onion>::toText(o)),
      uniq_count(uniq_count)
{
    if (m_key) {         // Don't encrypt if we don't have a key.
        Create_field * newcf = cf;
        //generate enclayers for encrypted field
        const std::string uniqueFieldName = this->getAnonOnionName();
        for (auto l: levels) {
            std::string key;
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

OnionMeta *OnionMeta::deserialize(unsigned int id,
                                  const std::string &serial)
{
    const auto vec = unserialize_string(serial); 

    const std::string onionname = vec[0];
    const unsigned int uniq_count = atoi(vec[1].c_str());

    return new OnionMeta(id, onionname, uniq_count);
}

std::string OnionMeta::serialize(const DBObject &parent) const
{
    const std::string serial =
        serialize_string(this->onionname) +
        serialize_string(std::to_string(this->uniq_count));

    return serial;
}

std::string OnionMeta::getAnonOnionName() const
{
    return onionname;
}

std::vector<DBMeta *> OnionMeta::fetchChildren(Connect *e_conn)
{
    std::function<DBMeta *(std::string, std::string, std::string)> deserialize =
        [this] (std::string key, std::string serial, std::string id)
            -> DBMeta*
        {
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
    return DBMeta::doFetchChildren(e_conn, deserialize);
}

void OnionMeta::applyToChildren(std::function<void(const DBMeta * const)> fn) const
{
    for (auto it : layers) {
        fn(it);
    }
}

AbstractMetaKey *OnionMeta::getKey(const DBMeta *const child) const
{
    for (std::vector<EncLayer *>::size_type i = 0; i< layers.size(); ++i) {
        if (child == layers[i]) {
            return new UIntMetaKey(i);
        }
    }

    return NULL;
}

EncLayer *OnionMeta::deserializeChild(unsigned int id,
                                      const std::string &serial_child)
    const
{
    return EncLayerFactory::deserializeLayer(id, serial_child);
}

UIntMetaKey *OnionMeta::deserializeKey(const std::string &serial_key)
    const
{
    return AbstractMetaKey::factory<UIntMetaKey>(serial_key);
}

void OnionMeta::addLayerBack(EncLayer *layer) {
    layers.push_back(layer);
}

EncLayer *OnionMeta::getLayerBack() const
{
    if (layers.size() == 0) {
        throw CryptDBError("Tried getting EncLayer when there are none!");
    }
    return layers.back();
}

void OnionMeta::removeLayerBack()
{
    if (layers.size() == 0) {
        throw CryptDBError("Tried to remove EncLayer when there are none!");
    }
    layers.pop_back();
}

void OnionMeta::replaceLayerBack(EncLayer *layer)
{
    if (layers.size() == 0) {
        throw CryptDBError("Tried to remove EncLayer when there are none!");
    }
    layers.pop_back();
    layers.push_back(layer);
}

SECLEVEL OnionMeta::getSecLevel()
{
    assert(layers.size() > 0);
    return layers.back()->level();
}

FieldMeta *FieldMeta::deserialize(unsigned int id,
                                  const std::string &serial)
{
    const auto vec = unserialize_string(serial);

    const std::string fname = vec[0];
    const bool has_salt = string_to_bool(vec[1]);
    const std::string salt_name = vec[2];
    const onionlayout onion_layout = TypeText<onionlayout>::toType(vec[3]);
    const SECURITY_RATING sec_rating =
        TypeText<SECURITY_RATING>::toType(vec[4]);
    const unsigned int uniq_count = atoi(vec[5].c_str());
    const unsigned int counter = atoi(vec[6].c_str());

    return new FieldMeta(id, fname, has_salt, salt_name, onion_layout,
                         sec_rating, uniq_count, counter);
}

FieldMeta::FieldMeta(std::string name, Create_field *field,
                     const AES_KEY * const m_key,
                     SECURITY_RATING sec_rating,
                     unsigned long uniq_count)
    : fname(name), has_salt(static_cast<bool>(m_key)),
      salt_name(BASE_SALT_NAME + getpRandomName()), 
      onion_layout(getOnionLayout(m_key, field, sec_rating)),
      sec_rating(sec_rating), uniq_count(uniq_count), counter(0)
{
    init_onions_layout(m_key, this, field);
}

std::string FieldMeta::serialize(const DBObject &parent) const
{
    const std::string serial =
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(getSaltName()) +
        serialize_string(TypeText<onionlayout>::toText(onion_layout)) +
        serialize_string(TypeText<SECURITY_RATING>::toText(sec_rating)) +
        serialize_string(std::to_string(uniq_count)) +
        serialize_string(std::to_string(counter));

   return serial;
}

std::string FieldMeta::stringify() const
{
    const std::string res = " [FieldMeta " + fname + "]";
    return res;
}

std::vector<std::pair<OnionMetaKey *, OnionMeta *>>
FieldMeta::orderedOnionMetas() const
{
    std::vector<std::pair<OnionMetaKey *, OnionMeta *>> v;
    for (auto it : children) {
        v.push_back(it);
    }

    std::sort(v.begin(), v.end(),
              [] (std::pair<OnionMetaKey *, OnionMeta *> a,
                  std::pair<OnionMetaKey *, OnionMeta *> b) {
                return a.second->getUniq() <
                       b.second->getUniq();
              });

    return v;
}

std::string FieldMeta::getSaltName() const {
    assert(has_salt);
    return salt_name;
}

SECLEVEL FieldMeta::getOnionLevel(onion o) const {
    OnionMetaKey *key = new OnionMetaKey(o);
    const auto om = getChild(key);
    delete key;
    if (om == NULL) {
        return SECLEVEL::INVALID;
    }

    return om->getSecLevel();
}

bool FieldMeta::setOnionLevel(onion o, SECLEVEL maxl) {
    OnionMeta *om = getOnionMeta(o);
    if (NULL == om) {
        return false;
    }
    SECLEVEL current_sec_level = om->getSecLevel();
    if (current_sec_level > maxl) {
        while (om->getSecLevel() != maxl) {
            om->removeLayerBack();
        }
        return true;
    }
    return false;
}

OnionMeta *FieldMeta::getOnionMeta(onion o) const
{
    const OnionMetaKey * const key = new OnionMetaKey(o);
    OnionMeta * const om = getChild(key);
    delete key;
    return om;
}

onionlayout FieldMeta::getOnionLayout(const AES_KEY * const m_key,
                                      Create_field *f,
                                      SECURITY_RATING sec_rating)
{
    if (sec_rating == SECURITY_RATING::PLAIN) {
        assert(!m_key);
        return PLAIN_ONION_LAYOUT;
    }

    if (!m_key) {
        throw CryptDBError("Should be using SECURITY_RATING::PLAIN!");
    }

    onionlayout basic_layout;
    if (SECURITY_RATING::SENSITIVE == sec_rating) {
        if (true == IsMySQLTypeNumeric(f->sql_type)) {
            return NUM_ONION_LAYOUT;
        } else {
            return STR_ONION_LAYOUT;
        }
    } else if (SECURITY_RATING::BEST_EFFORT == sec_rating) {
        if (true == IsMySQLTypeNumeric(f->sql_type)) {
            return BEST_EFFORT_NUM_ONION_LAYOUT;
        } else {
            return BEST_EFFORT_STR_ONION_LAYOUT;
        }
    } else {
        throw CryptDBError("Bad SECURITY_RATING in getOnionLayout!");
    }
}

TableMeta *TableMeta::deserialize(unsigned int id,
                                  const std::string &serial)
{
    const auto vec = unserialize_string(serial);
    
    const std::string anon_table_name = vec[0];
    const bool hasSensitive = string_to_bool(vec[1]);
    const bool has_salt = string_to_bool(vec[2]);
    const std::string salt_name = vec[3];
    const unsigned int counter = atoi(vec[4].c_str());

    return new TableMeta(id, anon_table_name, hasSensitive, has_salt,
                         salt_name, counter);
}

std::string TableMeta::serialize(const DBObject &parent) const
{
    std::string serial = 
        serialize_string(getAnonTableName()) +
        serialize_string(bool_to_string(hasSensitive)) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(salt_name) +
        serialize_string(std::to_string(counter));
    
    return serial;
}

// FIXME: May run into problems where a plaintext table expects the regular
// name, but it shouldn't get that name from 'getAnonTableName' anyways.
std::string TableMeta::getAnonTableName() const {
    return anon_table_name;
}

// FIXME: Slow.
// > Code is also duplicated with FieldMeta::orderedOnionMetas.
std::vector<FieldMeta *> TableMeta::orderedFieldMetas() const
{
    std::vector<FieldMeta *> v;
    for (auto it : children) {
        v.push_back(it.second);
    }

    std::sort(v.begin(), v.end(),
              [] (FieldMeta *a, FieldMeta *b) {
                return a->getUniq() < b->getUniq();
              }); 

    return v;
}

// TODO: Add salt.
std::string TableMeta::getAnonIndexName(const std::string &index_name) const
{
    const std::string hash_input = anon_table_name + index_name;
    const std::size_t hsh = std::hash<std::string>()(hash_input);

    return std::string("index_") + std::to_string(hsh);
}

FieldMeta *
SchemaInfo::getFieldMeta(const std::string &table,
                         const std::string & field) const
{
    IdentityMetaKey *table_key = new IdentityMetaKey(table);
    TableMeta * tm = getChild(table_key);
    if (NULL == tm) {
        return NULL;
    }
    delete table_key;

    IdentityMetaKey *field_key = new IdentityMetaKey(field);
    FieldMeta *fm = tm->getChild(field_key);
    delete field_key;

    return fm;
}

// FIXME: Slow.
std::string
SchemaInfo::getTableNameFromFieldMeta(FieldMeta *fm) const
{
    for (auto it : children) {
        AbstractMetaKey *key = it.second->getKey(fm);
        if (key) {
            return it.first->getValue();
        }
    }

    throw CryptDBError("Failed to get table name from FieldMeta");
}

