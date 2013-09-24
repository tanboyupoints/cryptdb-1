#pragma once

#include <algorithm>
#include <util/onions.hh>
#include <util/cryptdb_log.hh>
#include <main/schema.hh>
#include <main/rewrite_ds.hh>
#include <parser/embedmysql.hh>
#include <parser/stringify.hh>

/***************************************************/

extern "C" void *create_embedded_thd(int client_flag);

class ReturnField {
public:
    ReturnField(bool is_salt, const std::string &field_called,
                const OLK &olk, int salt_pos)
        : is_salt(is_salt), field_called(field_called), olk(olk),
          salt_pos(salt_pos) {}

    bool getIsSalt() const {return is_salt;}
    std::string fieldCalled() const {return field_called;}
    const OLK getOLK() const {return olk;}
    int getSaltPosition() const {return salt_pos;}
    std::string stringify();

private:
    const bool is_salt;
    const std::string field_called;
    const OLK olk;      // if !olk.key, field is not encrypted
    const int salt_pos; // position of salt of this field in
                        // the query results, or -1 if such
                        // salt was not requested
};

typedef struct ReturnMeta {
    std::map<int, ReturnField> rfmeta;
    std::string stringify();
} ReturnMeta;


class OnionAdjustExcept {
public:
    OnionAdjustExcept(const TableMeta &tm, const FieldMeta &fm, onion o,
                      SECLEVEL l)
        : tm(tm), fm(fm), o(o), tolevel(l) {}

    const TableMeta &tm;
    const FieldMeta &fm;
    const onion o;
    const SECLEVEL tolevel;
};

// TODO: Maybe we want a database name argument/member.
typedef class ConnectionInfo {
public:
    std::string server;
    uint port;
    std::string user;
    std::string passwd;

    ConnectionInfo(const std::string &s, const std::string &u,
                   const std::string &p, uint port = 0)
        : server(s), port(port), user(u), passwd(p) {};
    ConnectionInfo() : server(""), port(0), user(""), passwd("") {};

} ConnectionInfo;

// state maintained at the proxy
typedef struct ProxyState {
    // FIXME: Make private.
    const AES_KEY * const masterKey;

    ProxyState(ConnectionInfo ci, const std::string &embed_dir,
               const std::string &dbname, const std::string &master_key,
               SECURITY_RATING default_sec_rating =
                SECURITY_RATING::BEST_EFFORT);
    ~ProxyState();
    std::string dbName() const {return dbname;}
    SECURITY_RATING defaultSecurityRating() const
    {
        return default_sec_rating;
    }

    const std::unique_ptr<Connect> &getConn() const {return conn;}
    const std::unique_ptr<Connect> &getSideChannelConn() const
    {
        return side_channel_conn;
    }
    const std::unique_ptr<Connect> &getEConn() const {return e_conn;}

    static int db_init(const std::string &embed_dir);

private:
    const int mysql_dummy;
    // connection to remote and embedded server
    const std::unique_ptr<Connect> conn;
    const std::unique_ptr<Connect> side_channel_conn;
    const std::unique_ptr<Connect> e_conn;
    // FIXME: Remove once cryptdb supports multiple databases.
    const std::string dbname;
    const SECURITY_RATING default_sec_rating;
} ProxyState;


// For REPLACE and DELETE we are duplicating the MetaKey information.
class Delta {
public:
    enum TableType {REGULAR_TABLE, BLEEDING_TABLE};

    Delta(const DBMeta &parent_meta) : parent_meta(parent_meta) {}

    /*
     * Take the update action against the database. Contains high level
     * serialization semantics.
     */
    virtual bool apply(const std::unique_ptr<Connect> &e_conn,
                       TableType table_type) = 0;

protected:
    const DBMeta &parent_meta;

    std::string tableNameFromType(TableType table_type) const;
};

// CreateDelta calls must provide the key.  meta and
// parent_meta have not yet been associated such that the key can be
// functionally derived.
class CreateDelta : public Delta {
public:
    CreateDelta(std::unique_ptr<DBMeta> meta,
                const DBMeta &parent_meta,
                AbstractMetaKey *const key)
        : Delta(parent_meta), meta(std::move(meta)), key(key) {}

    bool save(const std::unique_ptr<Connect> &e_conn,
              unsigned long * const delta_output_id);
    bool apply(const std::unique_ptr<Connect> &e_conn,
               TableType table_type);
    bool destroyRecord(const std::unique_ptr<Connect> &e_conn);

private:
    const std::unique_ptr<DBMeta> meta;
    const std::unique_ptr<AbstractMetaKey> key;
};

class DerivedKeyDelta : public Delta {
public:
    DerivedKeyDelta(const DBMeta &meta,
                    const DBMeta &parent_meta)
        : Delta(parent_meta), meta(meta),
          key(parent_meta.getKey(meta))
    {}

protected:
    const DBMeta &meta;
    const AbstractMetaKey &key;
};

class ReplaceDelta : public DerivedKeyDelta {
public:
    ReplaceDelta(const DBMeta &meta, const DBMeta &parent_meta)
        : DerivedKeyDelta(meta, parent_meta) {}

    bool save(const std::unique_ptr<Connect> &e_conn,
              unsigned long * const delta_output_id);
    bool apply(const std::unique_ptr<Connect> &e_conn,
               TableType table_type);
    bool destroyRecord(const std::unique_ptr<Connect> &e_conn);
};

class DeleteDelta : public DerivedKeyDelta {
public:
    DeleteDelta(const DBMeta &meta, const DBMeta &parent_meta)
        : DerivedKeyDelta(meta, parent_meta) {}

    bool save(const std::unique_ptr<Connect> &e_conn,
              unsigned long * const delta_output_id);
    bool apply(const std::unique_ptr<Connect> &e_conn,
               TableType table_type);
    bool destroyRecord(const std::unique_ptr<Connect> &e_conn);
};

class Rewriter;

class RewriteOutput {
public:
    enum class Channel {REGULAR, SIDE};

    RewriteOutput(const std::string &original_query)
        : original_query(original_query) {}
    virtual ~RewriteOutput() = 0;

    virtual bool beforeQuery(const std::unique_ptr<Connect> &conn,
                             const std::unique_ptr<Connect> &e_conn) = 0;
    virtual bool getQuery(std::list<std::string> *const queryz,
                          SchemaInfo const &schema) const = 0;
    virtual bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn)
        const = 0;
    virtual bool afterQuery(const std::unique_ptr<Connect> &e_conn)
        const = 0;
    // This ASK code is a symptom of returning the rewritten query
    // to the proxy which then issues the query. A more TELL policy
    // would likely lead to cleaner execution of queries.
    virtual bool queryAgain() const;
    virtual bool doDecryption() const;
    virtual bool stalesSchema() const;
    virtual RewriteOutput::Channel queryChannel() const;
    virtual bool multipleResultSets() const;

    static ResType *sendQuery(const std::unique_ptr<Connect> &c,
                              const std::string &q);

protected:
    const std::string original_query;
};

class SimpleOutput : public RewriteOutput {
public:
    SimpleOutput(const std::string &original_query)
        : RewriteOutput(original_query) {}
    ~SimpleOutput() {;}

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);
    bool getQuery(std::list<std::string> * const queryz,
                  SchemaInfo const &schema) const;
    bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const;
    bool doDecryption() const;
};

class DMLOutput : public RewriteOutput {
public:
    DMLOutput(const std::string &original_query,
              const std::string &new_query)
        : RewriteOutput(original_query), new_query(new_query) {}
    ~DMLOutput() {;}

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);
    bool getQuery(std::list<std::string> * const queryz,
                  SchemaInfo const &schema) const;
    bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const;

private:
    const std::string new_query;
};

// Special case of DML query.
class SpecialUpdate : public RewriteOutput {
public:
    SpecialUpdate(const std::string &original_query,
                  const std::string &new_query,
                  const std::string &plain_table,
                  const std::string &crypted_table,
                  const std::string &where_clause,
                  const ProxyState &ps)
    : RewriteOutput(original_query), new_query(new_query),
      plain_table(plain_table), crypted_table(crypted_table),
      where_clause(where_clause), ps(ps) {}
    ~SpecialUpdate() {;}

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);
    bool getQuery(std::list<std::string> * const queryz,
                  SchemaInfo const &schema) const;
    bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const;
    bool multipleResultSets() const;

private:
    const std::string new_query;
    const std::string plain_table;
    const std::string crypted_table;
    const std::string where_clause;
    const ProxyState &ps;

    AssignOnce<std::string> output_values;
    AssignOnce<bool> do_nothing;
};

class DeltaOutput : public RewriteOutput {
public:
    DeltaOutput(const std::string &original_query,
                std::vector<Delta *> deltas)
        : RewriteOutput(original_query), deltas(deltas) {}
    virtual ~DeltaOutput() = 0;

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn) = 0;
    virtual bool getQuery(std::list<std::string> * const queryz,
                          SchemaInfo const &schema) const = 0;
    virtual bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn)
        const = 0;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const = 0;
    // FIXME: final.
    bool stalesSchema() const;

    static bool save(const std::unique_ptr<Connect> &e_conn,
                     unsigned long * const delta_output_id);
    static bool destroyRecord(const std::unique_ptr<Connect> &e_conn,
                              unsigned long delta_output_id);

protected:
    const std::vector<Delta *> deltas;
};

class DDLOutput : public DeltaOutput {
public:
    DDLOutput(const std::string &original_query,
              const std::string &new_query,
              std::vector<Delta *> deltas)
        : DeltaOutput(original_query, deltas), new_query(new_query) {}
    ~DDLOutput() {;}

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);
    bool getQuery(std::list<std::string> * const queryz,
                  SchemaInfo const &schema) const;
    bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const;

private:
    const std::string new_query;
    AssignOnce<unsigned long> delta_output_id;

    const std::list<std::string> remote_qz() const;
    const std::list<std::string> local_qz() const;
};

class AdjustOnionOutput : public DeltaOutput {
public:
    AdjustOnionOutput(std::vector<Delta *> deltas,
                      std::list<std::string> adjust_queries)
        : DeltaOutput("", deltas),
          adjust_queries(adjust_queries) {}
    ~AdjustOnionOutput() {;}
    ResType *doQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);

    bool beforeQuery(const std::unique_ptr<Connect> &conn,
                     const std::unique_ptr<Connect> &e_conn);
    bool getQuery(std::list<std::string> * const queryz,
                  SchemaInfo const &schema) const;
    bool handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const;
    bool afterQuery(const std::unique_ptr<Connect> &e_conn) const;
    // FIXME: final.
    bool queryAgain() const;
    // FIXME: final.
    bool doDecryption() const __attribute__((noreturn));
    // FIXME: final.
    RewriteOutput::Channel queryChannel() const;

private:
    const std::list<std::string> adjust_queries;
    AssignOnce<unsigned long> delta_output_id;

    const std::list<std::string> remote_qz() const;
    const std::list<std::string> local_qz() const;
};

bool saveDMLCompletion(const std::unique_ptr<Connect> &conn,
                       unsigned long delta_output_id);
bool setRegularTableToBleedingTable(const std::unique_ptr<Connect> &e_conn);
bool cleanupDeltaOutputAndQuery(const std::unique_ptr<Connect> &e_conn,
                                unsigned long delta_output_id);

class RewritePlan;
class Analysis {
public:
    Analysis(const SchemaInfo &schema)
        : pos(0), special_update(false),
          schema(schema) {}

    unsigned int pos; // > a counter indicating how many projection
                      // fields have been analyzed so far
    std::map<FieldMeta *, salt_type>    salts;
    std::map<Item *, RewritePlan *>     rewritePlans;
    std::map<std::string, std::string>  table_aliases;
    std::map<Item_field *, std::pair<Item_field *, OLK>> item_cache;

    // information for decrypting results
    ReturnMeta rmeta;

    bool special_update;

    // These functions are prefered to their lower level counterparts.
    bool addAlias(const std::string &alias, const std::string &table);
    OnionMeta &getOnionMeta(const std::string &table,
                            const std::string &field, onion o) const;
    OnionMeta &getOnionMeta(const FieldMeta &fm, onion o) const;
    FieldMeta &getFieldMeta(const std::string &table,
                            const std::string &field) const;
    FieldMeta &getFieldMeta(const TableMeta &tm,
                            const std::string &field) const;
    TableMeta &getTableMeta(const std::string &table) const;
    bool tableMetaExists(const std::string &table) const;
    bool nonAliasTableMetaExists(const std::string &table) const;
    std::string getAnonTableName(const std::string &table) const;
    std::string
        translateNonAliasPlainToAnonTableName(const std::string &table)
        const;
    std::string getAnonIndexName(const std::string &table,
                                 const std::string &index_name,
                                 onion o) const;
    std::string getAnonIndexName(const TableMeta &tm,
                                 const std::string &index_name,
                                 onion o) const;
    static EncLayer &getBackEncLayer(const OnionMeta &om);
    static SECLEVEL getOnionLevel(const OnionMeta &om);
    static std::vector<std::unique_ptr<EncLayer>> const &
        getEncLayers(const OnionMeta &om);
    const SchemaInfo &getSchema() {return schema;}

    std::vector<Delta *> deltas;

private:
    const SchemaInfo &schema;
    bool isAlias(const std::string &table) const;
    std::string unAliasTable(const std::string &table) const;
};

