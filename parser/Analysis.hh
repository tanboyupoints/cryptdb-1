#pragma once

#include <crypto-old/CryptoManager.hh>
#include <util/onions.hh>

#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <edb/MultiPrinc.hh>
//#include <util/schema.hh>

using namespace std;

/**
 * Used to keep track of encryption constraints during
 * analysis
 */
class EncSet {
public:
    EncSet(OnionLevelFieldMap input) : osl(input) {}
    EncSet(); // TODO(stephentu): move ctor here

    /**
     * decides which encryption scheme to use out of multiple in a set
     */
    EncSet chooseOne() const;

    EncSet intersect(const EncSet & es2) const;

    inline bool empty() const { return osl.empty(); }

    inline bool singleton() const { return osl.size() == 1; }

    inline OnionLevelFieldPair extract_singleton() const {
        assert(singleton());
        auto it = osl.begin();
        return OnionLevelFieldPair(it->first, it->second);
    }

    void setFieldForOnion(onion o, FieldMeta * fm);
    
    OnionLevelFieldMap osl; //max level on each onion
};


const EncSet EQ_EncSet = {
        {
            {oDET, LevelFieldPair(SECLEVEL::DET, NULL)},
            {oOPE, LevelFieldPair(SECLEVEL::OPE, NULL)},
        }
};

const EncSet ORD_EncSet = {
        {
            {oOPE, LevelFieldPair(SECLEVEL::OPE, NULL)},
        }
};

//todo: there should be a map of FULL_EncSets depending on item type
const EncSet FULL_EncSet = {
        {
            {oDET, LevelFieldPair(SECLEVEL::RND, NULL)},
            {oOPE, LevelFieldPair(SECLEVEL::RND, NULL)},
            {oAGG, LevelFieldPair(SECLEVEL::HOM, NULL)},
            {oSWP, LevelFieldPair(SECLEVEL::SEARCH, NULL)},
        }
};

const EncSet Search_EncSet = {
        {
            {oSWP, LevelFieldPair(SECLEVEL::SEARCH, NULL)},
        }
};

const EncSet ADD_EncSet = {
        {
            {oAGG, LevelFieldPair(SECLEVEL::HOM, NULL)},
        }
};

const EncSet EMPTY_EncSet = {
        {{}}
};


/***************************************************/

// metadata for field analysis                                                        
class FieldAMeta {
public:
    EncDesc exposedLevels; //field identifier to max sec level allowed to process a query
    FieldAMeta(const EncDesc & ed) : exposedLevels(ed) {}
};

//Metadata about how to encrypt an item
class ItemMeta {
public:
    onion o;
    FieldMeta * basefield;
    std::string stringify();
};
extern "C" void *create_embedded_thd(int client_flag);

typedef struct ReturnField {//TODO: isn't FieldMeta more fit than ItemMeta?
    bool is_salt;
    std::string field_called;
    ItemMeta *im;
    int pos_salt; //position of salt of this field in the query results,
                  // or -1 if such salt was not requested
    string stringify();
} ReturnField;

typedef struct ReturnMeta {
    map<int, ReturnField> rfmeta;
    string stringify();
} ReturnMeta;

typedef struct AdjustInfo {
    SECLEVEL l;
    Item_field * itf;
    AdjustInfo() : l(SECLEVEL::INVALID), itf(NULL) {}
    AdjustInfo(SECLEVEL _l, Item_field * _itf) : l(_l), itf(_itf) {}
    AdjustInfo(const AdjustInfo &  ai): l(ai.l), itf(ai.itf) {}
} AdjustInfo;

class Analysis {
public:
    Analysis(Connect * e_conn, Connect * conn, SchemaInfo * schema, AES_KEY *key, MultiPrinc *mp)
        : pos(0), schema(schema), masterKey(key), conn(conn), e_conn(e_conn), mp(mp) {}
    
    Analysis(): pos(0), schema(NULL), masterKey(NULL), conn(NULL), e_conn(NULL), mp(NULL) {}
 

    unsigned int pos; //a counter indicating how many projection fields have been analyzed so far                                                                    
    std::map<std::string, FieldAMeta *> fieldToAMeta;						     

    //maps an Item to metadata about that item
    std::map<Item*, ItemMeta *>         itemToMeta;
    std::map<Item_field*, FieldMeta*>   itemToFieldMeta;
    std::set<Item*>                     itemHasRewrite;
    
    SchemaInfo *                        schema;
    AES_KEY *                           masterKey;
    
    Connect *                           conn;
    Connect *                           e_conn;

    MultiPrinc *                        mp;
    TMKM                                tmkm;

    //fields that needs onions adjusted
    std::map<onion, std::map<FieldMeta *, AdjustInfo> > onionAdjust;
   
    ReturnMeta rmeta;

private:
    MYSQL * m;

};

typedef struct ConnectionData {
    std::string server;
    std::string user;
    std::string psswd;
    std::string dbname;
    uint port;

    ConnectionData() {}

    ConnectionData(std::string serverarg, std::string userarg, std::string psswdarg, std::string dbnamearg, uint portarg = 0) {
        server = serverarg;
        user = userarg;
        psswd = psswdarg;
        dbname = dbnamearg;
        port = portarg;
    }

} ConnectionData;
