#include <util/onions.hh>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <main/CryptoHandlers.hh>


#include <string>
#include <map>
#include <list>
#include <iostream>

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
    std::string onionname;
    enum enum_field_types sql_type;
    std::vector<EncLayer *> layers; //first in list is lowest layer

    SECLEVEL getSecLevel() {
        assert(layers.size() > 0);
        return layers.back()->level();
    }

    OnionMeta(): onionname("")  {};
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef struct FieldMeta {
    TableMeta * tm; //point to table belonging in
    std::string fname;
    Create_field * sql_field;
    onionlayout onion_layout;
    int index;

    std::map<onion, OnionMeta *> onions;

    bool has_salt; //whether this field has its own salt
    std::string salt_name;

    FieldMeta();

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

    std::string stringify();
    
    bool isEncrypted() {
        return ((onions.size() != 1) ||  (onions.find(oPLAIN) == onions.end()));
    }
} FieldMeta;



typedef struct TableMeta {

    std::list<std::string> fieldNames;     //in order field names                     
    unsigned int tableNo;
    std::string anonTableName;

    std::map<std::string, FieldMeta *> fieldMetaMap;

    bool hasSensitive;

    bool has_salt;
    std::string salt_name;

    TableMeta();
    ~TableMeta();
} TableMeta;


typedef struct SchemaInfo {
    std::map<std::string, TableMeta *> tableMetaMap;
    unsigned int totalTables;
    embedmysql * embed_db;

    SchemaInfo():totalTables(0) {};
    ~SchemaInfo() { tableMetaMap.clear();}
    friend class Analysis;

private:
    // These functions do not support Aliasing, use Analysis::getTableMeta
    // and Analysis::getFieldMeta.
    TableMeta * getTableMeta(const std::string & table);
    FieldMeta * getFieldMeta(const std::string & table, const std::string & field);
} SchemaInfo;

