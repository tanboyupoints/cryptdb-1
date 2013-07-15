#pragma once

#include <util/onions.hh>
#include <parser/embedmysql.hh>
#include <parser/stringify.hh>
#include <util/cryptdb_log.hh>
#include <main/schema.hh>

using namespace std;

/**
 * Used to keep track of encryption constraints during
 * analysis
 */
class EncSet {
public:
    EncSet(OnionLevelFieldMap input) : osl(input) {}
    EncSet(); // TODO(stephentu): move ctor here
    EncSet(FieldMeta * fm);
    EncSet(const OLK & olk);

    /**
     * decides which encryption scheme to use out of multiple in a set
     */
    OLK chooseOne() const;

    bool contains(const OLK & olk) const;

    EncSet intersect(const EncSet & es2) const;

    bool empty() const { return osl.empty(); }

    bool singleton() const { return osl.size() == 1; }

    OLK extract_singleton() const {
        assert_s(singleton(), string("encset has size ") + StringFromVal(osl.size()));
	auto it = osl.begin();
	return OLK(it->first, it->second.first, it->second.second);
    }

    void setFieldForOnion(onion o, FieldMeta * fm);

    OnionLevelFieldMap osl; //max level on each onion
};


ostream&
operator<<(ostream &out, const EncSet & es);

const EncSet EQ_EncSet = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oDET,   LevelFieldPair(SECLEVEL::DET, NULL)},
        {oOPE,   LevelFieldPair(SECLEVEL::OPE, NULL)},
    }
};

const EncSet JOIN_EncSet = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oDET,   LevelFieldPair(SECLEVEL::DETJOIN, NULL)},
    }
};

const EncSet ORD_EncSet = {
    {
	{oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
	{oOPE, LevelFieldPair(SECLEVEL::OPE, NULL)},
    }
};

const EncSet PLAIN_EncSet = {
    {
	{oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
    }
};

//todo: there should be a map of FULL_EncSets depending on item type
const EncSet FULL_EncSet = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oDET, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oOPE, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oAGG, LevelFieldPair(SECLEVEL::HOM, NULL)},
        {oSWP, LevelFieldPair(SECLEVEL::SEARCH, NULL)},
    }
};

const EncSet FULL_EncSet_Str = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oDET, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oOPE, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oSWP, LevelFieldPair(SECLEVEL::SEARCH, NULL)},
    }
};

const EncSet FULL_EncSet_Int = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oDET, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oOPE, LevelFieldPair(SECLEVEL::RND, NULL)},
        {oAGG, LevelFieldPair(SECLEVEL::HOM, NULL)},
    }
};

const EncSet Search_EncSet = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oSWP, LevelFieldPair(SECLEVEL::SEARCH, NULL)},
    }
};

const EncSet ADD_EncSet = {
    {
        {oPLAIN, LevelFieldPair(SECLEVEL::PLAINVAL, NULL)},
        {oAGG, LevelFieldPair(SECLEVEL::HOM, NULL)},
    }
};

const EncSet EMPTY_EncSet = {
    {{}}
};


// returns true if any of the layers in ed
// need salt
bool
needsSalt(EncSet ed);

/***************************************************/

extern "C" void *create_embedded_thd(int client_flag);

typedef struct ReturnField {
    bool is_salt;
    std::string field_called;
    OLK olk; // if !olk.key, field is not encrypted
    int pos_salt; //position of salt of this field in the query results,
                  // or -1 if such salt was not requested
    string stringify();
} ReturnField;

typedef struct ReturnMeta {
    map<int, ReturnField> rfmeta;
    string stringify();
} ReturnMeta;


class OnionAdjustExcept {
public:
    OnionAdjustExcept(onion o, FieldMeta * fm, SECLEVEL l, Item_field * itf) :
	o(o), fm(fm), tolevel(l), itf(itf) {}

    onion o;
    FieldMeta * fm;
    SECLEVEL tolevel;
    Item_field * itf;
};


class reason {
public:
    reason(const EncSet & es, const std::string &why_t_arg,
                Item *why_t_item_arg)
	:  encset(es), why_t(why_t_arg), why_t_item(why_t_item_arg)
	{ childr = new std::list<reason>();}
    reason(): encset(EMPTY_EncSet), why_t(""), why_t_item(NULL), childr(NULL) {}
    void add_child(const reason & ch) {
	childr->push_back(ch);
    }

    EncSet encset;

    std::string why_t;
    Item *why_t_item;

    std::list<reason> * childr;
};

ostream&
operator<<(ostream &out, const reason &r);

// The rewrite plan of a lex node: the information a
// node remembers after gather, to be used during rewrite
// Other more specific RewritePlan-s inherit from this class
class RewritePlan {
public:
    reason r;
    EncSet es_out; // encset that this item can output

    RewritePlan(const EncSet & es, reason r) : r(r), es_out(es) {};
    RewritePlan() {};

    //only keep plans that have parent_olk in es
//    void restrict(const EncSet & es);

};


//rewrite plan in which we only need to remember one olk
// to know how to rewrite
class RewritePlanOneOLK: public RewritePlan {
public:
    OLK olk;
    // the following store how to rewrite children
    RewritePlan ** childr_rp;

    RewritePlanOneOLK(const EncSet & es_out,
		      const OLK & olk,
		      RewritePlan ** childr_rp,
		      reason r) : RewritePlan(es_out, r),
				  olk(olk),
				  childr_rp(childr_rp) {}
};

// TODO: Maybe we want a database name argument/member.
typedef class ConnectionInfo {
public:
    std::string server;
    uint port;
    std::string user;
    std::string passwd;

    ConnectionInfo(std::string s, std::string u, std::string p, uint port = 0) :
	server(s), port(port), user(u), passwd(p) {};
    ConnectionInfo() : server(""), port(0), user(""), passwd("") {};

} ConnectionInfo;

ostream&
operator<<(ostream &out, const RewritePlan * rp);


// state maintained at the proxy
typedef struct ProxyState {

    ProxyState(): conn(NULL), e_conn(NULL), encByDefault(true),
		  masterKey(NULL), schema(NULL) {}
    ConnectionInfo ci;

    // connection to remote and embedded server
    Connect*       conn;
    Connect*       e_conn;

    bool           encByDefault;
    AES_KEY*       masterKey;

    SchemaInfo*    schema;

    ~ProxyState();
} ProxyState;


// FIXME: Use RTTI as we are going to need a way to determine what the
// parent is.
// For REPLACE and DELETE we are duplicating the MetaKey information.
class Delta : public DBObject {
public:
    enum Action {CREATE, REPLACE, DELETE};

    // New Delta.
    Delta(Action action, DBMeta *meta, DBMeta *parent_meta,
          AbstractMetaKey *key)
        : action(action), meta(meta), parent_meta(parent_meta), key(key) {}
    // FIXME: Unserialize old Delta.
    Delta(std::string serial) : key(new MetaKey<std::string>("implement me!"))
    {
        // return Delta(CREATE, NULL, NULL);
        std::vector<std::string> split_serials = unserialize_string(serial);
        throw CryptDBError("implement!");
    }

    /*
     * This function is responsible for writing our Delta to the database.
     */
    bool save(Connect *e_conn)
    {
        std::string serial_object = meta->serialize(*parent_meta);
        std::string parent_id =
            std::to_string(parent_meta->getDatabaseID());
        std::string table_name = "Delta";

        // TODO: Maybe we want to join on object_id as well.
        std::string query =
            " INSERT INTO pdb." + table_name +
            "    (action, serial_object, parent_id) VALUES (" +
            " "  + std::to_string(action) + ", " +
            " '" + serial_object + "', " +
            " '" + parent_id + "');";

        return e_conn->execute(query);
    }

    /*
     * Take the update action against the database. Contains high level
     * serialization semantics.
     */
    bool apply(Connect *e_conn) 
    {
        switch (action) {
            case CREATE: {
                // FIXME: Remove this once we are doing a Load after DDL
                // queries.
                assert(parent_meta->addChild(key, meta));
                
                // TODO: Encapsulate this behavior somewhere.
                const std::string child_serial =
                    meta->serialize(*parent_meta);
                assert(0 == meta->getDatabaseID());
                const std::string parent_id =
                    std::to_string(parent_meta->getDatabaseID());
                // FIXME: rtti.
                const std::string table_name = meta->typeName();
                const std::string join_table_name =
                    parent_meta->typeName() + "_" + meta->typeName();

                // Build the queries.

                // On CREATE, the database generates a unique ID for us.
                std::string query =
                    " INSERT INTO pdb." + table_name + 
                    "    (serial_object) VALUES ("
                    " '" + child_serial + "'); ";
                // TODO: Remove assert.
                assert(e_conn->execute(query));

                const std::string object_id =
                    std::to_string(e_conn->last_insert_id());
                std::string join_query =
                    " INSERT INTO pdb." + join_table_name +
                    "   (object_id, parent_id, serial_key) VALUES ("
                    " "  + object_id + ", " +
                    " "  + parent_id + ", " +
                    // FIXME: Serialize.
                    " '" + key->toString() + "'); ";

                // TODO: Remove assert.
                assert(e_conn->execute(join_query));

                return true;
                break;
            } case REPLACE: {
                return parent_meta->replaceChild(key, meta);
                break;
            } case DELETE: {
                return parent_meta->destroyChild(key);
                break;
            } default: {
                throw CryptDBError("Unknown Delta::Action!");
            }
        }
    }

private:
    Action action;
    // Can't use references because of deserialization.
    DBMeta * meta;
    DBMeta * parent_meta;
    AbstractMetaKey *key;
    bool used;

    std::string serialize(const DBObject &parent) const 
    {
        throw CryptDBError("Calling Delta::serialize with a parent argument"
                           " is nonsensical!");
    }
};

class Rewriter;

class Analysis {
public:
    Analysis(ProxyState * ps) : ps(ps), pos(0), rmeta(new ReturnMeta()) {}

    Analysis(): ps(NULL), pos(0), rmeta(new ReturnMeta()) {}

    // pointer to proxy metadata
    ProxyState * ps;

    /* Temporary structures for processing one query */

    unsigned int pos; //a counter indicating how many projection fields have been analyzed so far
    std::map<FieldMeta *, salt_type>    salts;
    std::map<Item *, RewritePlan *>     rewritePlans;
    std::map<std::string, std::string>  table_aliases;

    // information for decrypting results
    ReturnMeta * rmeta;

    // These functions are prefered to their lower level counterparts.
    bool addAlias(std::string alias, std::string table);
    FieldMeta *getFieldMeta(std::string table, std::string field) const;
    TableMeta *getTableMeta(std::string table) const;
    bool destroyFieldMeta(std::string table, std::string field);
    bool destroyTableMeta(std::string table);
    bool tableMetaExists(std::string table) const;
    std::string getAnonTableName(const string &table) const;
    std::string addIndex(std::string table,
                         std::string index_name);
    std::string getAnonIndexName(std::string table,
                                 std::string index_name) const;
    std::string getIndexName(std::string table,
                             std::string anon_index_name) const;
    bool destroyIndex(std::string table, std::string index_name);

    // HACK(burrows): This is a temporary solution until I redesign.
    Rewriter *rewriter;

    // TODO: Make private.
    std::vector<Delta> deltas;

private:
    std::string unAliasTable(std::string table) const;
};

