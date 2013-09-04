#include <algorithm>
#include <functional>
#include <memory>

#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/schema.hh>
#include <main/rewrite_main.hh>
#include <main/rewrite_util.hh>
#include <main/dbobject.hh>
#include <main/metadata_tables.hh>

std::vector<std::shared_ptr<DBMeta>>
DBMeta::doFetchChildren(const std::unique_ptr<Connect> &e_conn,
                        std::function<std::shared_ptr<DBMeta>
                            (const std::string &, const std::string &,
                             const std::string &)>
                            deserialHandler)
{
    const std::string table_name = MetaDataTables::Name::metaObject();

    // Now that we know the table exists, SELECT the data we want.
    std::vector<std::shared_ptr<DBMeta>> out_vec;
    DBResult *db_res;
    const std::string parent_id = std::to_string(this->getDatabaseID());
    const std::string serials_query = 
        " SELECT pdb." + table_name + ".serial_object,"
        "        pdb." + table_name + ".serial_key,"
        "        pdb." + table_name + ".id"
        " FROM pdb." + table_name + 
        " WHERE pdb." + table_name + ".parent_id"
        "   = " + parent_id + ";";
    assert(e_conn.get()->execute(serials_query, db_res));
    ScopedMySQLRes r(db_res->n);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long * const l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        const std::string child_serial_object(row[0], l[0]);
        const std::string child_key(row[1], l[1]);
        const std::string child_id(row[2], l[2]);

        std::shared_ptr<DBMeta> new_old_meta =
            deserialHandler(child_key, child_serial_object, child_id);
        out_vec.push_back(new_old_meta);
    }

    return out_vec;
}

OnionMeta::OnionMeta(onion o, std::vector<SECLEVEL> levels,
                     const AES_KEY * const m_key,
                     Create_field * const cf, unsigned long uniq_count)
    : onionname(getpRandomName() + TypeText<onion>::toText(o)),
      uniq_count(uniq_count)
{
    Create_field * newcf = cf;
    //generate enclayers for encrypted field
    const std::string uniqueFieldName = this->getAnonOnionName();
    for (auto l: levels) {
        const std::string key =
            m_key ? getLayerKey(m_key, uniqueFieldName, l)
                  : "plainkey";
        const std::shared_ptr<EncLayer>
            el(EncLayerFactory::encLayer(o, l, newcf, key));

        Create_field * const oldcf = newcf;
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

std::unique_ptr<OnionMeta>
OnionMeta::copyWithNewName(const OnionMeta * const om,
                           const std::string &name)
{
    return std::unique_ptr<OnionMeta>(new OnionMeta(om->getDatabaseID(),
                                                    name, om->getUniq()));
}

std::unique_ptr<OnionMeta>
OnionMeta::deserialize(unsigned int id, const std::string &serial)
{
    assert(id);
    const auto vec = unserialize_string(serial); 

    const std::string onionname = vec[0];
    const unsigned int uniq_count = atoi(vec[1].c_str());

    return std::unique_ptr<OnionMeta>(new OnionMeta(id, onionname,
                                                    uniq_count));
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

std::vector<std::shared_ptr<DBMeta>>
OnionMeta::fetchChildren(const std::unique_ptr<Connect> &e_conn)
{
    std::function<std::shared_ptr<DBMeta>(const std::string &,
                                          const std::string &,
                                          const std::string &)>
        deserialHelper =
    [this] (const std::string &key, const std::string &serial,
            const std::string &id) -> std::shared_ptr<EncLayer>
    {
        std::function<unsigned int(std::string)> strToInt =
            [](std::string s) {return atoi(s.c_str());};
        // > Probably going to want to use indexes in AbstractMetaKey
        // for now, otherwise you will need to abstract and rederive
        // a keyed and nonkeyed version of Delta.
        const std::unique_ptr<UIntMetaKey>
            meta_key(AbstractMetaKey::factory<UIntMetaKey>(key));
        const unsigned int index = meta_key->getValue();
        if (index >= this->layers.size()) {
            this->layers.resize(index + 1);
        }
        std::shared_ptr<EncLayer>
            layer(EncLayerFactory::deserializeLayer(atoi(id.c_str()),
                                                    serial));
        this->layers[index] = layer;

        return layer;
    };

    return DBMeta::doFetchChildren(e_conn, deserialHelper);
}

void OnionMeta::applyToChildren(std::function<void(std::shared_ptr<DBMeta>)>
    fn) const
{
    for (auto it : layers) {
        fn(it);
    }
}

AbstractMetaKey *OnionMeta::getKey(const DBMeta *const child) const
{
    for (std::vector<EncLayer *>::size_type i = 0; i< layers.size(); ++i) {
        // FIXME: PTR.
        if (child == layers[i].get()) {
            return new UIntMetaKey(i);
        }
    }

    return NULL;
}

EncLayer *OnionMeta::getLayerBack() const
{
    if (layers.size() == 0) {
        throw CryptDBError("Tried getting EncLayer when there are none!");
    }
    // FIXME: PTR.
    return layers.back().get();
}

bool OnionMeta::hasEncLayer(const SECLEVEL &sl) const
{
    for (auto it : layers) {
        if (it->level() == sl) {
            return true;
        }
    }

    return false;
}

EncLayer *OnionMeta::getLayer(const SECLEVEL &sl) const
{
    if (layers.size() == 0) {
        throw CryptDBError("No EncLayerz!");
    }

    AssignOnce<EncLayer *> out;
    for (auto it : layers) {
        if (it->level() == sl) {
            // FIXME: PTR.
            out = it.get();
        }
    }

    assert(out.assigned());
    return out.get();
}

void OnionMeta::removeLayerBack()
{
    if (layers.size() == 0) {
        throw CryptDBError("Tried to remove EncLayer when there are none!");
    }
    layers.pop_back();
}

SECLEVEL OnionMeta::getSecLevel()
{
    assert(layers.size() > 0);
    return layers.back()->level();
}

std::unique_ptr<FieldMeta>
FieldMeta::deserialize(unsigned int id, const std::string &serial)
{
    const auto vec = unserialize_string(serial);

    const std::string fname = vec[0];
    const bool has_salt = string_to_bool(vec[1]);
    const std::string salt_name = vec[2];
    const bool plain_number = string_to_bool(vec[3]);
    const onionlayout onion_layout = TypeText<onionlayout>::toType(vec[4]);
    const SECURITY_RATING sec_rating =
        TypeText<SECURITY_RATING>::toType(vec[5]);
    const unsigned int uniq_count = atoi(vec[6].c_str());
    const unsigned int counter = atoi(vec[7].c_str());

    return std::unique_ptr<FieldMeta>
        (new FieldMeta(id, fname, has_salt, salt_name, plain_number,
                       onion_layout, sec_rating, uniq_count, counter));
}

// If mkey == NULL, the field is not encrypted
static bool
init_onions_layout(const AES_KEY *const m_key,
                   FieldMeta *const fm, Create_field *const cf)
{
    if (fm->has_salt != static_cast<bool>(m_key)) {
        return false;
    }

    if (0 != fm->children.size()) {
        return false;
    }

    for (auto it: fm->onion_layout) {
        const onion o = it.first;
        const std::vector<SECLEVEL> levels = it.second;
        // A new OnionMeta will only occur with a new FieldMeta so
        // we never have to build Deltaz for our OnionMetaz.
        std::unique_ptr<OnionMeta>
            om(new OnionMeta(o, levels, m_key, cf, fm->leaseIncUniq()));
        const std::string onion_name = om->getAnonOnionName();
        fm->addChild(new OnionMetaKey(o), std::move(om));

        LOG(cdb_v) << "adding onion layer " << onion_name
                   << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }

    return true;
}

FieldMeta::FieldMeta(const std::string &name, Create_field * const field,
                     const AES_KEY * const m_key,
                     SECURITY_RATING sec_rating,
                     unsigned long uniq_count)
    : fname(name), has_salt(static_cast<bool>(m_key)),
      salt_name(BASE_SALT_NAME + getpRandomName()), 
      onion_layout(getOnionLayout(m_key, field, sec_rating,
                                  &plain_number)),
      sec_rating(sec_rating), uniq_count(uniq_count), counter(0)
{
    assert(init_onions_layout(m_key, this, field));
}

std::string FieldMeta::serialize(const DBObject &parent) const
{
    const std::string serialized_salt_name =
        true == this->has_salt ? serialize_string(getSaltName())
                               : serialize_string("");
    const std::string serial =
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialized_salt_name +
        serialize_string(bool_to_string(plain_number)) +
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
        // FIXME: PTR.
        v.push_back(std::make_pair(it.first, it.second.get()));
    }

    std::sort(v.begin(), v.end(),
              [] (std::pair<OnionMetaKey *, OnionMeta *> a,
                  std::pair<OnionMetaKey *, OnionMeta *> b) {
                return a.second->getUniq() <
                       b.second->getUniq();
              });

    return v;
}

std::string FieldMeta::getSaltName() const
{
    assert(has_salt);
    return salt_name;
}

SECLEVEL FieldMeta::getOnionLevel(onion o) const
{
    std::unique_ptr<OnionMetaKey> key(new OnionMetaKey(o));
    const auto om = getChild(key.get());
    if (om == NULL) {
        return SECLEVEL::INVALID;
    }

    return om->getSecLevel();
}

bool FieldMeta::setOnionLevel(onion o, SECLEVEL maxl)
{
    OnionMeta * const om = getOnionMeta(o);
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
    const std::unique_ptr<OnionMetaKey> key(new OnionMetaKey(o));
    const std::shared_ptr<OnionMeta> om = getChild(key.get());
    // FIXME: PTR.
    return om.get();
}

onionlayout FieldMeta::getOnionLayout(const AES_KEY * const m_key,
                                      const Create_field * const f,
                                      SECURITY_RATING sec_rating,
                                      bool * const plain_number)
{
    if (sec_rating == SECURITY_RATING::PLAIN) {
        // assert(!m_key);
        return PLAIN_ONION_LAYOUT;
    }

    if (!m_key) {
        throw CryptDBError("Should be using SECURITY_RATING::PLAIN!");
    }

    onionlayout basic_layout;
    if (SECURITY_RATING::SENSITIVE == sec_rating) {
        *plain_number = false;
        if (true == IsMySQLTypeNumeric(f->sql_type)) {
            return NUM_ONION_LAYOUT;
        } else {
            return STR_ONION_LAYOUT;
        }
    } else if (SECURITY_RATING::BEST_EFFORT == sec_rating) {
        if (true == IsMySQLTypeNumeric(f->sql_type)) {
            *plain_number = true;
            return BEST_EFFORT_NUM_ONION_LAYOUT;
        } else {
            *plain_number = false;
            return BEST_EFFORT_STR_ONION_LAYOUT;
        }
    } else {
        throw CryptDBError("Bad SECURITY_RATING in getOnionLayout!");
    }
}

bool FieldMeta::needExtraPlainColumn() const
{
    return plain_number;
}

std::string FieldMeta::getToPlainName() const
{
    // FIXME.
    assert(this->needExtraPlainColumn());
    return "plainname";
}

bool FieldMeta::hasOnion(onion o) const
{
    const std::unique_ptr<OnionMetaKey> key(new OnionMetaKey(o));
    return childExists(key.get());
}

std::unique_ptr<TableMeta>
TableMeta::deserialize(unsigned int id, const std::string &serial)
{
    const auto vec = unserialize_string(serial);
    
    const std::string anon_table_name = vec[0];
    const bool hasSensitive = string_to_bool(vec[1]);
    const bool has_salt = string_to_bool(vec[2]);
    const std::string salt_name = vec[3];
    const unsigned int counter = atoi(vec[4].c_str());

    return std::unique_ptr<TableMeta>
        (new TableMeta(id, anon_table_name, hasSensitive, has_salt,
                       salt_name, counter));
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
std::string TableMeta::getAnonTableName() const
{
    return anon_table_name;
}

// FIXME: Slow.
// > Code is also duplicated with FieldMeta::orderedOnionMetas.
std::vector<FieldMeta *> TableMeta::orderedFieldMetas() const
{
    std::vector<FieldMeta *> v;
    for (auto it : children) {
        // FIXME: PTR.
        v.push_back(it.second.get());
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
                         const std::string &field) const
{
    std::unique_ptr<IdentityMetaKey>
        table_key(new IdentityMetaKey(table));
    std::shared_ptr<TableMeta> tm = getChild(table_key.get());
    if (NULL == tm) {
        return NULL;
    }

    std::unique_ptr<IdentityMetaKey>
        field_key(new IdentityMetaKey(field));
    std::shared_ptr<FieldMeta> fm = tm->getChild(field_key.get());

    // FIXME: PTR.
    return fm.get();
}

// FIXME: Slow.
std::string
SchemaInfo::getTableNameFromFieldMeta(const FieldMeta * const fm) const
{
    for (auto it : children) {
        const AbstractMetaKey * const key = it.second->getKey(fm);
        if (key) {
            return it.first->getValue();
        }
    }

    throw CryptDBError("Failed to get table name from FieldMeta");
}

