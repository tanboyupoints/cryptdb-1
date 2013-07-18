#pragma once

#include <util/onions.hh>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <main/CryptoHandlers.hh>
#include <main/Translator.hh>
#include <main/enum_text.hh>
#include <main/dbobject.hh>
#include <string>
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <functional>

class Analysis;
struct FieldMeta;
/**
 * Field here is either:
 * A) empty string, representing any field or
 * B) the field that the onion is key-ed on. this
 *    only has semantic meaning for DET and OPE
 */
typedef std::pair<SECLEVEL, FieldMeta *> LevelFieldPair;

typedef std::map<SECLEVEL, FieldMeta *> LevelFieldMap;
typedef std::pair<onion, LevelFieldPair> OnionLevelFieldPair;
typedef std::map<onion, LevelFieldPair> OnionLevelFieldMap;

// onion-level-key: all the information needed to know how to encrypt a
// constant
class OLK {
public:
    OLK(onion o, SECLEVEL l, FieldMeta * key) : o(o), l(l), key(key) {}
    OLK() : o(oINVALID), l(SECLEVEL::INVALID), key(NULL) {}
    onion o;
    SECLEVEL l;
    FieldMeta * key; // a field meta is a key because each encryption key
                     // ever used in CryptDB belongs to a field; a field
                     // contains the encryption and decryption handlers
                     // for its keys (see layers)
    bool operator<(const OLK & olk ) const {
        return (o < olk.o) || ((o == olk.o) && (l < olk.l));
    }
    bool operator==(const OLK & olk ) const {
        return (o == olk.o) && (l == olk.l);
    }
};

const OLK PLAIN_OLK = OLK(oPLAIN, SECLEVEL::PLAINVAL, NULL);

/*
 * The name must be unique as it is used as a unique identifier when
 * generating the encryption layers.
 * 
 * OnionMeta is a bit different than the other AbstractMeta derivations.
 * > It's children aren't of the same class.  Each EncLayer does
 *   inherit from EncLayer, but they are still distinct classes. This
 *   is problematic because DBMeta::deserialize<ConcreteClass> relies
 *   on us being able to provide a concrete class.  We can't pick a
 *   specific class for our OnionMeta as it must support multiple classes.
 * > Also note that like FieldMeta, OnionMeta's children have an explicit
 *   order that must be encoded.
 */
// TODO: Fix the children.
typedef class OnionMeta : public DBMeta {
public:
    // TODO: Private.
    std::vector<EncLayer *> layers; //first in list is lowest layer

    // New.
    OnionMeta(onion o, std::vector<SECLEVEL> levels, AES_KEY *m_key,
              Create_field *cf);
    // Restore.
    /*
    OnionMeta(std::string name)
        : onionname(name) {}
    */
    OnionMeta(unsigned int id, std::string serial);

    std::string serialize(const DBObject &parent) const;
    std::string getAnonOnionName() const;
    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}
    std::vector<DBMeta *> fetchChildren(Connect *e_conn);
    void applyToChildren(std::function<void(DBMeta *)>);
    AbstractMetaKey *getKey(const DBMeta *const child) const
    {
        for (std::vector<EncLayer *>::size_type i = 0; i < layers.size(); ++i) {
            if (child == layers[i]) {
                return new UIntMetaKey(i);
            }
        }

        return NULL;
    }

    bool addLayerBack(EncLayer *layer) {
        layers.push_back(layer);
        return true;
    }
    bool removeLayerBack() {
        layers.pop_back();
        return true;
    }
    bool replaceLayerBack(EncLayer *layer) {
        layers.pop_back();
        layers.push_back(layer);
        return true;
    }

    SECLEVEL getSecLevel() {
        assert(layers.size() > 0);
        return layers.back()->level();
    }

private:
    constexpr static const char *type_name = "onionMeta";
    std::string onionname;
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef class FieldMeta : public AbstractMeta<OnionMeta, OnionMetaKey> {
public:
    std::string fname;
    bool has_salt; //whether this field has its own salt
    std::string salt_name;
    onionlayout onion_layout;

    // New field.
    FieldMeta(std::string name, Create_field *field, AES_KEY *mKey,
              unsigned long uniq_count);
    // Recovering field from proxy db.
    FieldMeta(std::string name, bool has_salt, 
              std::string salt_name, onionlayout onion_layout,
              unsigned long uniq_count)
        : fname(name), has_salt(has_salt), salt_name(salt_name),
          onion_layout(onion_layout), uniq_count(uniq_count) {}
    FieldMeta(unsigned int id, std::string serial);
    ~FieldMeta() {;}

    std::string serialize(const DBObject &parent) const;
    std::string stringify() const;

    std::string getSaltName() const {
        assert(has_salt);
        return salt_name;
    }

    unsigned long getUniqCount() const {
        return uniq_count;
    }

    SECLEVEL getOnionLevel(onion o) const {
        AbstractMetaKey *key = new OnionMetaKey(o);
        auto om = getChild(key);
        delete key;
        if (om == NULL) {
            return SECLEVEL::INVALID;
        }

        // FIXME: dynamic_cast
        return static_cast<OnionMeta *>(om)->getSecLevel();
    }

    bool setOnionLevel(onion o, SECLEVEL maxl) {
        OnionMeta *om = getOnionMeta(o);
        if (NULL == om) {
            return false;
        }
        SECLEVEL current_sec_level = om->getSecLevel();
        if (current_sec_level > maxl) {
            while (om->layers.size() != 0 &&
                   om->layers.back()->level() != maxl) {
                om->layers.pop_back();
            }
            return true;
        }
        return false;
    }

    // FIXME: This is a HACK.
    bool isEncrypted() {
        AbstractMetaKey *key = new OnionMetaKey(oPLAIN);
        bool status =  ((children.size() != 1) ||
                        (children.find(key) == children.end()));
        delete key;
        return status;
    }

    OnionMeta *getOnionMeta(onion o) {
        OnionMetaKey *key = new OnionMetaKey(o);
        DBMeta *om = getChild(key);
        delete key;
        // FIXME: dynamic_cast
        return static_cast<OnionMeta *>(om);
    }

    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}

private:
    constexpr static const char *type_name = "fieldMeta";
    unsigned long uniq_count;
} FieldMeta;

// TODO: Put const back.
typedef class TableMeta : public AbstractMeta<FieldMeta, IdentityMetaKey> {
public:
    bool hasSensitive;
    bool has_salt;
    std::string salt_name;
    std::string anon_table_name;

    // Restore old TableMeta.
    TableMeta(bool has_sensitive, bool has_salt, std::string salt_name,
              std::string anon_table_name,
              unsigned long counter)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name(salt_name), anon_table_name(anon_table_name),
          counter(counter) {}

    // New TableMeta.
    TableMeta(bool has_sensitive, bool has_salt)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name("tableSalt_" + getpRandomName()),
          anon_table_name("table_" + getpRandomName()),
          counter(0) {}
    TableMeta(unsigned int id, std::string serial);
    ~TableMeta() {;}

    std::string serialize(const DBObject &parent) const;
    std::string getAnonTableName() const;
    std::vector<FieldMeta *> orderedFieldMetas() const;
    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}
    unsigned long leaseIncUniq() {return counter++;}
    unsigned long getUniqCounter() {return counter;}

    friend class Analysis;

private:
    constexpr static const char *type_name = "tableMeta";
    unsigned int counter;

    std::string getAnonIndexName(std::string index_name) const;
} TableMeta;


// FIXME: Inherit from AbstractMeta.
// AWARE: Table/Field aliases __WILL NOT__ be looked up when calling from
// this level or below. Use Analysis::* if you need aliasing.
typedef class SchemaInfo : public AbstractMeta<TableMeta, IdentityMetaKey> {
public:
    SchemaInfo() : AbstractMeta(0) {}
    ~SchemaInfo();

    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}

    friend class Analysis;

private:
    constexpr static const char *type_name = "schemaInfo";
    // FIXME: Remove.
    std::map<std::string, TableMeta *> tableMetaMap;

    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    FieldMeta * getFieldMeta(std::string & table,
                             std::string & field) const;
    std::string serialize(const DBObject &parent) const {
        throw CryptDBError("SchemaInfo can not be serialized!");
    }
} SchemaInfo;

bool create_tables(Connect *e_conn, DBWriter dbw);

