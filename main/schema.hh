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
    bool stale;
    std::list<EncLayer *> layers; //first in list is lowest layer

    OnionMeta(): onionname(""), stale(false) {};
} OnionMeta;

struct TableMeta;
//TODO: FieldMeta and TableMeta are partly duplicates with the original
// FieldMetadata an TableMetadata
// which contains data we want to add to this structure soon
typedef struct FieldMeta {
    TableMeta * tm; //point to table belonging in
    std::string fname;
    Create_field * sql_field;
    int index;

    //TODO: may want to integrate onions with encdesc for clarity
    std::map<onion, OnionMeta *> onions;
    EncDesc encdesc;

    bool has_salt; //whether this field has its own salt
    std::string salt_name;

    FieldMeta();

    bool hasOnion(onion o) const {
        return encdesc.olm.find(o) !=
            encdesc.olm.end();
    }

    SECLEVEL getOnionLevel(onion o) const {
        auto it = encdesc.olm.find(o);
        if (it == encdesc.olm.end()) return SECLEVEL::INVALID;
        return it->second;
    }

    bool setOnionLevel(onion o, SECLEVEL maxl) {
        if (encdesc.restrict(o, maxl)) {
            while (onions[o]->layers.size() != 0 && onions[o]->layers.back()->level() != maxl) {
                onions[o]->layers.pop_back();
            }
            return true;
        }
        return false;
    }

    void removeOnion(onion o) {
        onions.erase(o);
        encdesc.olm.erase(o);
        std::cerr << fname << " encdesc is " << encdesc << std::endl;
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
    TableMeta * getTableMeta(const std::string & table);
    FieldMeta * getFieldMeta(const std::string & table, const std::string & field);
} SchemaInfo;

