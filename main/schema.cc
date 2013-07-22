#include <algorithm>
#include <functional>

#include <parser/lex_util.hh>
#include <parser/stringify.hh>
#include <main/schema.hh>
#include <main/rewrite_main.hh>
#include <main/init_onions.hh>
#include <main/rewrite_util.hh>
#include <main/dbobject.hh>

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

OnionMeta::OnionMeta(onion o, std::vector<SECLEVEL> levels, AES_KEY *m_key,
                     Create_field *cf, unsigned long uniq_count)
    : onionname(getpRandomName() + TypeText<onion>::toText(o)),
      uniq_count(uniq_count)
{
    if (m_key) {         // Don't encrypt if we don't have a key.
        Create_field * newcf = cf;
        //generate enclayers for encrypted field
        std::string uniqueFieldName = this->getAnonOnionName();
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

OnionMeta *OnionMeta::deserialize(unsigned int id, std::string serial)
{
    auto vec = unserialize_string(serial); 

    std::string onionname = vec[0];
    unsigned int uniq_count = atoi(vec[1].c_str());

    return new OnionMeta(id, onionname, uniq_count);
}

std::string OnionMeta::serialize(const DBObject &parent) const
{
    std::string serial =
        serialize_string(this->onionname) +
        serialize_string(std::to_string(this->uniq_count));

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

AbstractMetaKey *OnionMeta::getKey(const DBMeta *const child) const
{
	for (std::vector<EncLayer *>::size_type i = 0; i < layers.size(); ++i) {
		if (child == layers[i]) {
			return new UIntMetaKey(i);
		}
	}

	return NULL;
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

FieldMeta *FieldMeta::deserialize(unsigned int id, std::string serial)
{
    auto vec = unserialize_string(serial);

    std::string fname = vec[0];
    bool has_salt = string_to_bool(vec[1]);
    std::string salt_name = vec[2];
    onionlayout onion_layout = TypeText<onionlayout>::toType(vec[3]);
    unsigned int uniq_count = atoi(vec[4].c_str());
    unsigned int counter = atoi(vec[5].c_str());

    return new FieldMeta(id, fname, has_salt, salt_name, onion_layout,
                         uniq_count, counter);
}

FieldMeta::FieldMeta(std::string name, Create_field *field, AES_KEY *m_key,
                     unsigned long uniq_count)
    : fname(name), has_salt(static_cast<bool>(m_key)),
      salt_name(BASE_SALT_NAME + getpRandomName()), 
      onion_layout(getOnionLayout(m_key, field)), uniq_count(uniq_count),
      counter(0)
{
    init_onions_layout(m_key, this, field);
}

std::string FieldMeta::serialize(const DBObject &parent) const
{
    std::string serial =
        serialize_string(fname) +
        serialize_string(bool_to_string(has_salt)) +
        serialize_string(getSaltName()) +
        serialize_string(TypeText<onionlayout>::toText(onion_layout)) +
        serialize_string(std::to_string(uniq_count)) +
        serialize_string(std::to_string(counter));

   return serial;
}

std::string FieldMeta::stringify() const
{
    std::string res = " [FieldMeta " + fname + "]";
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
	auto om = getChild(key);
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

// FIXME: This is a HACK.
bool FieldMeta::isEncrypted() {
	OnionMetaKey *key = new OnionMetaKey(oPLAIN);
	bool status =  ((children.size() != 1) ||
					(children.find(key) == children.end()));
	delete key;
	return status;
}

OnionMeta *FieldMeta::getOnionMeta(onion o) {
	OnionMetaKey *key = new OnionMetaKey(o);
	DBMeta *om = getChild(key);
	delete key;
	// FIXME: dynamic_cast
	return static_cast<OnionMeta *>(om);
}

onionlayout FieldMeta::getOnionLayout(AES_KEY *m_key, Create_field *f)
{
    if (NULL == m_key) {
        return PLAIN_ONION_LAYOUT;
    } else if (true == IsMySQLTypeNumeric(f->sql_type)) {
        return NUM_ONION_LAYOUT;
    } else {
        return STR_ONION_LAYOUT;
    }
}

TableMeta *TableMeta::deserialize(unsigned int id, std::string serial)
{
    auto vec = unserialize_string(serial);
    
    std::string anon_table_name = vec[0];
    bool hasSensitive = string_to_bool(vec[1]);
    bool has_salt = string_to_bool(vec[2]);
    std::string salt_name = vec[3];
    unsigned int counter = atoi(vec[4].c_str());

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
std::string TableMeta::getAnonIndexName(std::string index_name) const
{
    std::string hash_input = anon_table_name + index_name;
    std::size_t hsh = std::hash<std::string>()(hash_input);

    return std::string("index_") + std::to_string(hsh);
}

FieldMeta *
SchemaInfo::getFieldMeta(std::string & table, std::string & field) const
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

