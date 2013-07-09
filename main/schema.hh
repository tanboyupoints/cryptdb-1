#pragma once

#include <util/onions.hh>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <main/CryptoHandlers.hh>
#include <main/Translator.hh>
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


typedef struct OnionMeta {
    std::vector<EncLayer *> layers; //first in list is lowest layer

    SECLEVEL getSecLevel() {
        assert(layers.size() > 0);
        return layers.back()->level();
    }

    OnionMeta(onion o, unsigned int field_index, std::string field_name) 
        : onionname(anonymizeFieldName(field_index, o, field_name))  {};
    std::string getAnonOnionName() const;

private:
    const std::string onionname;
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
// FIXME: Make uniq const.
typedef struct FieldMeta {
    TableMeta * tm; //point to table belonging in
    std::string fname;
    int uniq;
    bool has_salt; //whether this field has its own salt
    onionlayout onion_layout;

    std::map<onion, OnionMeta *> onions;

    // New field.
    FieldMeta(TableMeta *tm, std::string name, unsigned int uniq,
              Create_field *field, AES_KEY *mKey);
    // Recovering field from proxy db.
    FieldMeta(TableMeta *tm, std::string name, unsigned int uniq,
              bool has_salt, onionlayout onion_layout)
        : tm(tm), fname(name), uniq(uniq), has_salt(has_salt),
          onion_layout(onion_layout) {}
    ~FieldMeta();

    std::string saltName() const;
    std::string fullName(onion o) const;
    std::string stringify() const;

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


// TODO: tableNo, hasSalt, salt_name should be const.
typedef struct TableMeta {
    std::list<std::string> fieldNames;     //in order field names
    unsigned int tableNo;

    std::map<std::string, FieldMeta *> fieldMetaMap;

    bool hasSensitive;

    bool has_salt;
    std::string salt_name;

    TableMeta()
        : tableNo(0), uniq_counter(0) {}
    TableMeta(unsigned int table_no, bool has_sensitive,
              bool has_salt, std::string salt_name,
              std::map<std::string, std::string> index_map,
              unsigned int uniq_counter=0)
        : tableNo(table_no), hasSensitive(has_sensitive),
          has_salt(has_salt), salt_name(salt_name),
          index_map(index_map), uniq_counter(uniq_counter) {}
    ~TableMeta();

    // TODO: Make FieldMeta a friend and deal with the other uses of this
    // function.
    FieldMeta *createFieldMeta(Create_field *field, const Analysis &a,
                               bool encByDefault);
    FieldMeta *getFieldMeta(std::string field);
    unsigned int getUniqCounter() const;
    std::string getAnonTableName() const;

    friend class Analysis;

protected:
    bool destroyFieldMeta(std::string field);
    std::string addIndex(std::string index_name); 
    std::string getAnonIndexName(std::string index_name) const;
    std::string getIndexName(std::string anon_index_name) const;
    bool destroyIndex(std::string index_name);
   
private:
    std::map<std::string, std::string> index_map;
    unsigned int uniq_counter;

    unsigned int leaseUniqCounter();
} TableMeta;


// AWARE: Table/Field aliases __WILL NOT__ be looked up when calling from
// this level or below. Use Analysis::* if you need aliasing.
typedef struct SchemaInfo {
    std::map<std::string, TableMeta *> tableMetaMap;
    unsigned int totalTables;
    embedmysql * embed_db;

    SchemaInfo():totalTables(0) {};
    ~SchemaInfo() { tableMetaMap.clear();}

    // Parameters should match TableMeta constructor, except for tableNo
    // which we derive from SchemaInfo and the addition of the plaintext
    // table name.
    TableMeta *createTableMeta(std::string table_name,
                               bool has_sensitive, bool has_salt,
                               std::string salt_name,
                               std::map<std::string, std::string> index_map,
                               unsigned int uniq_counter,
                               const unsigned int *table_no=NULL);
    friend class Analysis;

private:
    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    TableMeta * getTableMeta(const std::string & table);
    FieldMeta * getFieldMeta(const std::string & table,
                             const std::string & field);
    bool destroyTableMeta(std::string table);
} SchemaInfo;

