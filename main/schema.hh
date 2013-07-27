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

// > FIXME: We are Memleaking
// SchemaInfo/TableMeta/FieldMeta/OnionMeta/EncLayer and the keys.

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
typedef class OnionMeta : public DBMeta {
public:
    // New.
    OnionMeta(onion o, std::vector<SECLEVEL> levels, AES_KEY *m_key,
              Create_field *cf, unsigned long uniq_count);
    // Restore.
    static OnionMeta *deserialize(unsigned int id,
                                  const std::string &serial);
    OnionMeta(unsigned int id, std::string onionname,
              unsigned long uniq_count)
        : DBMeta(id), onionname(onionname), uniq_count(uniq_count) {}

    std::string serialize(const DBObject &parent) const;
    std::string getAnonOnionName() const;
    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}
    std::vector<DBMeta *> fetchChildren(Connect *e_conn);
    void applyToChildren(std::function<void(const DBMeta * const)>) const;
    AbstractMetaKey *getKey(const DBMeta *const child) const;
    unsigned long getUniq() const {return uniq_count;}

    // Need access to layers.
    friend class Analysis;
    friend class FieldMeta;
    friend bool sanityCheck(FieldMeta *);
    friend Item *decrypt_item_layers(Item *, onion, OnionMeta *, uint64_t,
                                     FieldMeta *,
                                     const std::vector<Item *> &);

private:
    std::vector<EncLayer *> layers; //first in list is lowest layer
    constexpr static const char *type_name = "onionMeta";
    const std::string onionname;
    unsigned long uniq_count;

    SECLEVEL getSecLevel();
    void addLayerBack(EncLayer *layer);
    EncLayer *getLayerBack() const;
    void removeLayerBack();
    void replaceLayerBack(EncLayer *layer);
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef class FieldMeta : public MappedDBMeta<OnionMeta, OnionMetaKey> {
public:
    const std::string fname;
    const bool has_salt; //whether this field has its own salt
    const std::string salt_name;
    const onionlayout onion_layout;

    // New.
    FieldMeta(std::string name, Create_field *field, AES_KEY *mKey,
              unsigned long uniq_count);
    // Restore (WARN: Creates an incomplete type as it will not have it's
    // OnionMetas until they are added by the caller).
    static FieldMeta *deserialize(unsigned int id,
                                  const std::string &serial);
    FieldMeta(unsigned int id, std::string fname, bool has_salt,
              std::string salt_name, onionlayout onion_layout,
              unsigned long uniq_count, unsigned long counter)
        : MappedDBMeta(id), fname(fname), has_salt(has_salt),
          salt_name(salt_name), onion_layout(onion_layout),
          uniq_count(uniq_count) {}
    ~FieldMeta() {;}

    std::string serialize(const DBObject &parent) const;
    std::string stringify() const;
    std::vector<std::pair<OnionMetaKey *, OnionMeta *>>
        orderedOnionMetas() const;
    std::string getSaltName() const;
    unsigned long getUniq() const {return uniq_count;}

    bool isEncrypted();
    OnionMeta *getOnionMeta(onion o) const;
    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}

    unsigned long leaseIncUniq() {return counter++;}
    // FIXME: Change name.
    unsigned long getCurrentUniqCounter() {return counter;}

private:
    constexpr static const char *type_name = "fieldMeta";
    unsigned long uniq_count;
    unsigned long counter;

    SECLEVEL getOnionLevel(onion o) const;
    bool setOnionLevel(onion o, SECLEVEL maxl);
    static onionlayout getOnionLayout(AES_KEY *m_key, Create_field *f);
} FieldMeta;

typedef class TableMeta : public MappedDBMeta<FieldMeta, IdentityMetaKey> {
public:
    const bool hasSensitive;
    const bool has_salt;
    const std::string salt_name;
    const std::string anon_table_name;

    // New TableMeta.
    TableMeta(bool has_sensitive, bool has_salt)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name("tableSalt_" + getpRandomName()),
          anon_table_name("table_" + getpRandomName()),
          counter(0) {}
    // Restore.
    static TableMeta *deserialize(unsigned int id,
                                  const std::string &serial);
    TableMeta(unsigned int id, std::string anon_table_name,
              bool has_sensitive, bool has_salt, std::string salt_name,
              unsigned int counter)
        : MappedDBMeta(id), hasSensitive(has_sensitive),
          has_salt(has_salt), salt_name(salt_name),
          anon_table_name(anon_table_name), counter(counter) {}
    ~TableMeta() {;}

    std::string serialize(const DBObject &parent) const;
    std::string getAnonTableName() const;
    std::vector<FieldMeta *> orderedFieldMetas() const;
    // FIXME: Use rtti.
    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}
    unsigned long leaseIncUniq() {return counter++;}
    unsigned long getCurrentUniqCounter() {return counter;}

    friend class Analysis;

private:
    constexpr static const char *type_name = "tableMeta";
    unsigned int counter;

    std::string getAnonIndexName(const std::string &index_name) const;
} TableMeta;


// AWARE: Table/Field aliases __WILL NOT__ be looked up when calling from
// this level or below. Use Analysis::* if you need aliasing.
typedef class SchemaInfo : public MappedDBMeta<TableMeta, IdentityMetaKey> {
public:
    SchemaInfo() : MappedDBMeta(0), db_name("cryptdbtest") {}
    ~SchemaInfo() {}

    std::string typeName() const {return type_name;}
    static std::string instanceTypeName() {return type_name;}
    std::string getTableNameFromFieldMeta(FieldMeta *fm) const;

    friend class Analysis;

private:
    constexpr static const char *type_name = "schemaInfo";
    const char *db_name;

    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    FieldMeta * getFieldMeta(const std::string & table,
                             const std::string & field) const;
    std::string serialize(const DBObject &parent) const {
        throw CryptDBError("SchemaInfo can not be serialized!");
    }
} SchemaInfo;

bool create_tables(Connect *e_conn);

