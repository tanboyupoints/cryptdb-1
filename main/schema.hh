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

class AbstractMetaKey {
public:
    AbstractMetaKey() {;}
    virtual ~AbstractMetaKey() {;}
    virtual bool operator <(const AbstractMetaKey &rhs) const = 0;
    virtual bool operator ==(const AbstractMetaKey &rhs) const = 0;
    virtual std::string toString() const = 0;
};

template <typename KeyType>
class MetaKey : public AbstractMetaKey {
public:
    KeyType key_data;

    MetaKey(KeyType key_data) : key_data(key_data) {}
    // FIXME: Implement.
    MetaKey(int dummy, std::string serial) {;}

    bool operator <(const AbstractMetaKey &rhs) const
    {
        MetaKey rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data < rhs_key.key_data;
    }

    bool operator ==(const AbstractMetaKey &rhs) const
    {
        MetaKey rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data == rhs_key.key_data;
    }

    static MetaKey<KeyType> *deserialize(std::string serial)
    {
        int dummy = 1;
        return new MetaKey<KeyType>(dummy, serial);
    }

    // FIXME.
    std::string toString() const
    {
        std::ostringstream s;
        s << key_data;
        return s.str();
    }
};

struct DBMeta : public DBObject {
    DBMeta() {}
    virtual ~DBMeta() {}

    virtual bool addChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool replaceChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool destroyChild(AbstractMetaKey *key);

    // Helpers.
    std::map<AbstractMetaKey *, DBMeta *>::const_iterator
        findChild(AbstractMetaKey *key) const;
    bool childExists(AbstractMetaKey * key) const;
    DBMeta *getChild(AbstractMetaKey * key) const;
    AbstractMetaKey *getKey(const DBMeta *const child) const;

    // FIXME: Use rtti.
    virtual std::string typeName() const = 0;
    virtual std::vector<std::pair<AbstractMetaKey *, DBMeta *>>
        fetchChildren(Connect *e_conn) = 0;

    std::map<AbstractMetaKey *, DBMeta *> children;
};

// > TODO: Mage getDatabaseID() protected by templating on the Concrete type
//   and making it a friend.
template <typename ChildType, typename KeyType>
struct AbstractMeta : public DBMeta {
    // TODO: Remove default constructor.
    AbstractMeta() {}
    virtual ~AbstractMeta()
    {
        auto cp = children;
        children.clear();

        for (auto it : cp) {
            delete it.second;
        }
    }
    // Virtual constructor to deserialize from embedded database.
    template <typename ConcreteMeta>
        static ConcreteMeta *deserialize(std::string serial);
    std::vector<std::pair<AbstractMetaKey *, DBMeta *>>
        fetchChildren(Connect *e_conn);
};

/*
 * The name must be unique as it is used as a unique identifier when
 * generating the encryption layers.
 */
// TODO: Fix the children.
typedef struct OnionMeta : AbstractMeta<OnionMeta, std::string> {
    // TODO: Private.
    std::vector<EncLayer *> layers; //first in list is lowest layer

    // New.
    OnionMeta(onion o) 
        : onionname(getpRandomName() + TypeText<onion>::toText(o)) {};
    // Restore.
    /*
    OnionMeta(std::string name)
        : onionname(name) {}
    */
    OnionMeta(std::string serial);
    std::string serialize(const DBObject &parent) const;
    std::string getAnonOnionName() const;
    // FIXME: Use rtti.
    std::string typeName() const {return "onionMeta";}

    SECLEVEL getSecLevel() {
        assert(layers.size() > 0);
        return layers.back()->level();
    }

private:
    const std::string onionname;
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef struct FieldMeta : public AbstractMeta<OnionMeta, onion> {
    const std::string fname;
    bool has_salt; //whether this field has its own salt
    const std::string salt_name;
    onionlayout onion_layout;

    std::map<onion, OnionMeta *> onions;

    // New field.
    FieldMeta(std::string name, Create_field *field, AES_KEY *mKey);
    // Recovering field from proxy db.
    FieldMeta(std::string name, bool has_salt, 
              std::string salt_name, onionlayout onion_layout)
        : fname(name), has_salt(has_salt), salt_name(salt_name),
          onion_layout(onion_layout) {}
    FieldMeta(std::string serial);
    ~FieldMeta();

    std::string serialize(const DBObject &parent) const;
    std::string stringify() const;

    std::string getSaltName() const {
        assert(has_salt);
        return salt_name;
    }

    SECLEVEL getOnionLevel(onion o) const {
        auto it = onions.find(o);
        if (it == onions.end()) return SECLEVEL::INVALID;

        return it->second->getSecLevel();
    }

    bool setOnionLevel(onion o, SECLEVEL maxl) {
        SECLEVEL current_sec_level = onions[o]->getSecLevel();
        if (current_sec_level > maxl) {
            while (onions[o]->layers.size() != 0 && onions[o]->layers.back()->level() != maxl) {
                onions[o]->layers.pop_back();
            }
            return true;
        }
        return false;
    }

    bool isEncrypted() {
        return ((onions.size() != 1) ||  (onions.find(oPLAIN) == onions.end()));
    }

    // FIXME: Use rtti.
    std::string typeName() const {return "fieldMeta";}
} FieldMeta;

// TODO: Put const back.
typedef struct TableMeta : public AbstractMeta<FieldMeta, std::string> {
    std::list<AbstractMetaKey *> fieldNames;  //in order field names
    bool hasSensitive;
    bool has_salt;
    std::string salt_name;
    std::string anon_table_name;

    // Restore old TableMeta.
    TableMeta(bool has_sensitive, bool has_salt, std::string salt_name,
              std::string anon_table_name,
              std::map<std::string, std::string> index_map)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name(salt_name), anon_table_name(anon_table_name),
          index_map(index_map) {}

    // New TableMeta.
    TableMeta(bool has_sensitive, bool has_salt,
              std::map<std::string, std::string> index_map)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name("tableSalt_" + getpRandomName()),
          anon_table_name("table_" + getpRandomName()),
          index_map(index_map) {}
    TableMeta(std::string serial);

    std::string serialize(const DBObject &parent) const;
    bool addChild(AbstractMetaKey *key, DBMeta *meta);
    std::string getAnonTableName() const;
    bool destroyChild(AbstractMetaKey *key);
    // FIXME: Use rtti.
    std::string typeName() const {return "tableMeta";}

    friend class Analysis;

protected:
    std::string addIndex(std::string index_name); 
    std::string getAnonIndexName(std::string index_name) const;
    std::string getIndexName(std::string anon_index_name) const;
    bool destroyIndex(std::string index_name);
   
private:
    std::map<std::string, std::string> index_map;
} TableMeta;


// FIXME: Inherit from AbstractMeta.
// AWARE: Table/Field aliases __WILL NOT__ be looked up when calling from
// this level or below. Use Analysis::* if you need aliasing.
typedef struct SchemaInfo : public AbstractMeta<TableMeta, std::string> {
    SchemaInfo() {;}
    ~SchemaInfo();

    std::string typeName() const {return "SchemaInfo";}

    friend class Analysis;

private:
    std::map<std::string, TableMeta *> tableMetaMap;

    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    FieldMeta * getFieldMeta(std::string & table,
                             std::string & field) const;
    std::string serialize(const DBObject &parent) const {
        throw CryptDBError("SchemaInfo can not be serialized!");
    }
} SchemaInfo;

