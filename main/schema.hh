#pragma once

#include <util/onions.hh>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <main/CryptoHandlers.hh>
#include <main/Translator.hh>
#include <main/enum_text.hh>
#include <string>
#include <map>
#include <list>
#include <iostream>

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

// onion-level-key: all the information needed to know how to encrypt a constant
class OLK {
public:
    OLK(onion o, SECLEVEL l, FieldMeta * key) : o(o), l(l), key(key) {}
    OLK() : o(oINVALID), l(SECLEVEL::INVALID), key(NULL) {}
    onion o;
    SECLEVEL l;
    FieldMeta * key; //a field meta is a key because each encryption key ever
		     //used in CryptDB belongs to a field; a field contains the
		     //encryption and decryption handlers for its keys (see
		     //layers)
    bool operator<(const OLK & olk ) const {
        return (o < olk.o) || ((o == olk.o) && (l < olk.l));
    }
    bool operator==(const OLK & olk ) const {
        return (o == olk.o) && (l == olk.l);
    }
};

std::ostream&
operator<<(std::ostream &out, const OLK &olk);


const OLK PLAIN_OLK = OLK(oPLAIN, SECLEVEL::PLAINVAL, NULL);


std::ostream&
operator<<(std::ostream &out, const OnionLevelFieldPair &p);


/*
 * The name must be unique as it is used as a unique identifier when
 * generating the encryption layers.
 */
typedef struct OnionMeta {
    std::vector<EncLayer *> layers; //first in list is lowest layer

    SECLEVEL getSecLevel() {
        assert(layers.size() > 0);
        return layers.back()->level();
    }

    // New.
    OnionMeta(onion o) 
        : onionname(getpRandomName() + TypeText<onion>::toText(o)) {};
    // Restore.
    OnionMeta(std::string name)
        : onionname(name) {}
    std::string getAnonOnionName() const;

private:
    const std::string onionname;
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef struct FieldMeta {
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
    ~FieldMeta();

    std::string stringify() const;

    std::string getSaltName() const {
        assert(has_salt);
        return salt_name;
    }

    bool hasOnion(onion o) const {
        return onions.find(o) !=
            onions.end();
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

    void removeOnion(onion o) {
        onions.erase(o);
    }

    bool isEncrypted() {
        return ((onions.size() != 1) ||  (onions.find(oPLAIN) == onions.end()));
    }
} FieldMeta;

typedef struct TableMeta {
    std::list<std::string> fieldNames;     //in order field names
    std::map<std::string, FieldMeta *> fieldMetaMap;
    const bool hasSensitive;
    const bool has_salt;
    const std::string salt_name;
    const std::string anon_table_name;

    // Restore old TableMeta.
    TableMeta(bool has_sensitive, bool has_salt, std::string salt_name,
              std::string anon_table_name)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name(salt_name), anon_table_name(anon_table_name) {}

    // New TableMeta.
    TableMeta(bool has_sensitive, bool has_salt)
        : hasSensitive(has_sensitive), has_salt(has_salt),
          salt_name("tableSalt_" + getpRandomName()),
          anon_table_name("table_" + getpRandomName()) {}
    ~TableMeta();

    bool fieldMetaExists(std::string name);
    bool addFieldMeta(FieldMeta *fm);
    FieldMeta *getFieldMeta(std::string field);
    std::string getAnonTableName() const;

    friend class Analysis;

protected:
    bool destroyFieldMeta(std::string field);
    std::string getAnonIndexName(std::string index_name) const;
} TableMeta;


// AWARE: Table/Field aliases __WILL NOT__ be looked up when calling from
// this level or below. Use Analysis::* if you need aliasing.
typedef struct SchemaInfo {
    SchemaInfo() {;}
    ~SchemaInfo();
    bool addTableMeta(std::string name, TableMeta *tm);

    friend class Analysis;

private:
    std::map<std::string, TableMeta *> tableMetaMap;

    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    TableMeta * getTableMeta(const std::string & table) const;
    FieldMeta * getFieldMeta(const std::string & table,
                             const std::string & field) const;
    bool tableMetaExists(std::string table) const;
    bool destroyTableMeta(std::string table);
} SchemaInfo;

