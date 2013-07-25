#pragma once

#include <algorithm>
#include <util/onions.hh>
#include <util/cryptdb_log.hh>
#include <main/schema.hh>
#include <parser/embedmysql.hh>
#include <parser/stringify.hh>

/**
 * Used to keep track of encryption constraints during
 * analysis
 */
class EncSet {
public:
    EncSet(OnionLevelFieldMap input) : osl(input) {}
    EncSet(); // TODO(stephentu): move ctor here
    EncSet(Analysis &a, FieldMeta * fm);
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
        assert_s(singleton(), std::string("encset has size ") +
                              StringFromVal(osl.size()));
        auto it = osl.begin();
        return OLK(it->first, it->second.first, it->second.second);
    }

    void setFieldForOnion(onion o, FieldMeta * fm);

    OnionLevelFieldMap osl; //max level on each onion
};


std::ostream&
operator<<(std::ostream &out, const EncSet & es);

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
    std::string stringify();
} ReturnField;

typedef struct ReturnMeta {
    std::map<int, ReturnField> rfmeta;
    std::string stringify();
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
    reason()
        : encset(EMPTY_EncSet), why_t(""), why_t_item(NULL),
          childr(NULL) {}
    void add_child(const reason & ch) {
        childr->push_back(ch);
    }

    EncSet encset;

    std::string why_t;
    Item *why_t_item;

    std::list<reason> * childr;
};

std::ostream&
operator<<(std::ostream &out, const reason &r);

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

    RewritePlanOneOLK(const EncSet & es_out, const OLK & olk,
                      RewritePlan ** childr_rp, reason r)
        : RewritePlan(es_out, r), olk(olk), childr_rp(childr_rp) {}
};

// TODO: Maybe we want a database name argument/member.
typedef class ConnectionInfo {
public:
    std::string server;
    uint port;
    std::string user;
    std::string passwd;

    ConnectionInfo(std::string s, std::string u, std::string p,
                   uint port = 0)
        : server(s), port(port), user(u), passwd(p) {};
    ConnectionInfo() : server(""), port(0), user(""), passwd("") {};

} ConnectionInfo;

std::ostream&
operator<<(std::ostream &out, const RewritePlan * rp);


// state maintained at the proxy
typedef struct ProxyState {
    ProxyState()
        : conn(NULL), e_conn(NULL), encByDefault(true), masterKey(NULL) {}
    ~ProxyState();
    std::string dbName() const {return dbname;}

    ConnectionInfo ci;

    // connection to remote and embedded server
    Connect*       conn;
    Connect*       e_conn;

    bool           encByDefault;
    AES_KEY*       masterKey;

private:
    // FIXME: Remove once cryptdb supports multiple databases.
    constexpr static const char *dbname = "cryptdbtest";
} ProxyState;


// FIXME: Use RTTI as we are going to need a way to determine what the
// parent is.
// For REPLACE and DELETE we are duplicating the MetaKey information.
class Delta : public DBObject {
public:
    enum Action {CREATE, REPLACE, DELETE};

    // New Delta.
    Delta(Action action, const DBMeta * const meta,
          const DBMeta * const parent_meta,
          const AbstractMetaKey * const key)
        : action(action), meta(meta), parent_meta(parent_meta), key(key) {}
    // FIXME: Unserialize old Delta.
    /*
    Delta(unsigned int id, std::string serial)
        : DBObject(id)
    {
        // FIXME: Determine key.
        // return Delta(CREATE, NULL, NULL);
        std::vector<std::string> split_serials = unserialize_string(serial);
        throw CryptDBError("implement!");
    }
    */

    /*
     * This function is responsible for writing our Delta to the database.
     */
    bool save(Connect *e_conn);

    /*
     * Take the update action against the database. Contains high level
     * serialization semantics.
     */
    bool apply(Connect *e_conn);
    bool destroyRecord(Connect *e_conn);
    void createHandler(Connect *e_conn, const DBMeta * const object,
                       const DBMeta * const parent,
                       const AbstractMetaKey * const k = NULL,
                       const unsigned int * const ptr_parent_id = NULL);
    void deleteHandler(Connect *e_conn, const DBMeta * const object,
                       const DBMeta * const parent);
    void replaceHandler(Connect *e_conn, const DBMeta * const object,
                        const DBMeta * const parent,
                        const AbstractMetaKey * const k);

private:
    Action action;
    // Can't use references because of deserialization.
    const DBMeta * const meta;
    const DBMeta * const parent_meta;
    const AbstractMetaKey * const key;

    std::string serialize(const DBObject &parent) const 
    {
        throw CryptDBError("Calling Delta::serialize with a parent"
                           " argument is nonsensical!");
    }
};

class Rewriter;

class RewriteOutput { 
public:
    RewriteOutput(const std::string &original_query, LEX *lex)
        : original_query(original_query),
          query_from_lex(getQuery(lex)) {}
    virtual ~RewriteOutput() = 0;

    virtual ResType *doQuery(Connect *conn, Connect *e_conn,
                             Rewriter *rewriter = NULL);
    virtual bool queryAgain();

protected:
    const std::string original_query;
    const std::string query_from_lex;

    static std::string getQuery(LEX *lex);
    static ResType *sendQuery(Connect *c, std::string q);
};

class SimpleOutput : public RewriteOutput {
public:
    SimpleOutput(const std::string &original_query)
        : RewriteOutput(original_query, NULL) {}
};

class DMLOutput : public RewriteOutput {
public:
    DMLOutput(const std::string &original_query, LEX *lex)
        : RewriteOutput(original_query, lex) {}
    ~DMLOutput() {;}

    ResType *doQuery(Connect *conn, Connect *e_conn,
                    Rewriter *rewriter = NULL);
};

// Special case of DML query.
class SpecialUpdate : public RewriteOutput {
public:
    // Requires the original lex.
    SpecialUpdate(const std::string &original_query, LEX *lex,
                  const ProxyState &ps);
    ~SpecialUpdate() {;}

    ResType *doQuery(Connect *conn, Connect *e_conn,
                    Rewriter *rewriter = NULL);

private:
    std::string plain_table;
    std::string where_clause;
    const ProxyState &ps;
};

class DeltaOutput : public RewriteOutput {
public:
    DeltaOutput(const std::string &original_query, LEX *lex,
                std::list<Delta> deltas)
        : RewriteOutput(original_query, lex), deltas(deltas) {}
    virtual ~DeltaOutput() = 0;
    virtual ResType *doQuery(Connect *conn, Connect *e_conn,
                            Rewriter *rewriter = NULL)
    {
        throw CryptDBError("DeltaOutput::doQuery should not be called!");
    }
    ResType *doQueryHelper(Connect *conn, Connect *e_conn,
                           Rewriter *rewriter, bool do_original,
                           std::function<ResType *()> primary);

private:
    const std::list<Delta> deltas;
};

class DDLOutput : public DeltaOutput {
public:
    DDLOutput(const std::string &original_query, LEX *lex,
              std::list<Delta> deltas)
        : DeltaOutput(original_query, lex, deltas) {}
    ~DDLOutput() {;}

    ResType *doQuery(Connect *conn, Connect *e_conn,
                    Rewriter *rewriter = NULL);
};

class AdjustOnionOutput : public DeltaOutput {
public:
    AdjustOnionOutput(const std::string &original_query,
                      std::list<Delta> deltas,
                      std::list<std::string> adjust_queries)
        : DeltaOutput(original_query, NULL, deltas),
          adjust_queries(adjust_queries) {}
    ~AdjustOnionOutput() {;}

    bool queryAgain();
    ResType *doQuery(Connect *conn, Connect *e_conn,
                    Rewriter *rewriter = NULL);

private:
    const std::list<std::string> adjust_queries;
};

class Analysis {
public:
    Analysis(const SchemaInfo * const schema)
        : pos(0), rmeta(new ReturnMeta()), special_update(false),
          schema(schema) {}

    unsigned int pos; // > a counter indicating how many projection
                      // fields have been analyzed so far
    std::map<FieldMeta *, salt_type>    salts;
    std::map<Item *, RewritePlan *>     rewritePlans;
    std::map<std::string, std::string>  table_aliases;

    // information for decrypting results
    ReturnMeta * rmeta;
    
    bool special_update;

    // These functions are prefered to their lower level counterparts.
    bool addAlias(const std::string &alias, const std::string &table);
    OnionMeta *getOnionMeta(const std::string &table,
                            const std::string &field, onion o) const;
    FieldMeta *getFieldMeta(const std::string &table,
                            const std::string &field) const;
    TableMeta *getTableMeta(const std::string &table) const;
    bool tableMetaExists(const std::string &table) const;
    std::string getAnonTableName(const std::string &table) const;
    std::string getAnonIndexName(const std::string &table,
                                 const std::string &index_name) const;
    EncLayer *getBackEncLayer(OnionMeta * const om) const;
    EncLayer *popBackEncLayer(OnionMeta * const om);
    SECLEVEL getOnionLevel(OnionMeta * const om) const;
    std::vector<EncLayer *> getEncLayers(OnionMeta * const om) const;
    // HACK.
    const SchemaInfo *getSchema() {return schema;}

    // TODO: Make private.
    std::map<OnionMeta *, std::vector<EncLayer *>> to_adjust_enc_layers;
    
    std::list<Delta> deltas;

private:
    const SchemaInfo * const schema;
    std::string unAliasTable(const std::string &table) const;
};

