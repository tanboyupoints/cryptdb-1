#include <main/Analysis.hh>
#include <main/rewrite_util.hh>
#include <main/rewrite_main.hh>
#include <main/metadata_tables.hh>

#define ROLLBACK_AND_RETURN_ON_FAIL(status, c, ret)     \
    {                                                   \
        if (!status) {                                  \
            assert(c->execute("ROLLBACK;"));            \
            return ret;                                 \
        }                                               \
    }                                                   

// FIXME: Memory leaks when we allocate MetaKey<...>, use smart pointer.

static std::string
escapeString(Connect *e_conn, const std::string &escape_me)
{
    unsigned int escaped_length = escape_me.size() * 2 + 1;
    char *escaped = new char[escaped_length];
    e_conn->real_escape_string(escaped, escape_me.c_str(),
                               escape_me.size());

    std::string out = std::string(escaped);
    delete[] escaped;

    return out;
}

// FIXME: Wrong interfaces.
EncSet::EncSet(Analysis &a, FieldMeta * fm) {
    // FIXME: Safe to throw exception in constructor?
    if (0 == fm->children.size()) {
        throw CryptDBError("FieldMeta has no children!");
    }

    osl.clear();
    for (auto pair : fm->children) {
        OnionMeta *om = pair.second;
        OnionMetaKey *key = pair.first;
        osl[key->getValue()] = LevelFieldPair(a.getOnionLevel(om), fm);
    }
}

EncSet::EncSet(const OLK & olk) {
    osl[olk.o] = LevelFieldPair(olk.l, olk.key);
}

EncSet
EncSet::intersect(const EncSet & es2) const
{
    OnionLevelFieldMap m;
    for (auto it2 = es2.osl.begin();
            it2 != es2.osl.end(); ++it2) {
        auto it = osl.find(it2->first);

        if (it != osl.end()) {
            FieldMeta *fm = it->second.second;
            FieldMeta *fm2 = it2->second.second;

            onion o = it->first;
            onion o2 = it2->first;
            
            // AWARE: While there isn't a reason why we can't share
            // keys across onions, as of now it seems like unintentional
            // behavior.
            if (o != o2) {
                continue;
            }

            SECLEVEL sl = (SECLEVEL)min((int)it->second.first, 
                                        (int)it2->second.first);

            if (fm == NULL) {
                m[o] = LevelFieldPair(sl, fm2);
            } else if (fm2 == NULL) {
                m[it->first] = LevelFieldPair(sl, fm);
            } else {
                // This can hypothetically succeed in two cases.
                // 1> Same field, so same key.
                // 2> Different fields, but SECLEVEL is PLAINVAL,
                //    HOM or DETJOIN so same key.
                OnionMeta *om = fm->getOnionMeta(o);
                OnionMeta *om2 = fm2->getOnionMeta(o);
                // HACK: To determine if the keys are the same.
                if (om->getLayer(sl)->doSerialize() ==
                    om2->getLayer(sl)->doSerialize()) {
                    m[o] = LevelFieldPair(sl, fm);
                }
            }
        }
    }
    return EncSet(m);
}

std::ostream&
operator<<(std::ostream &out, const EncSet & es)
{
    if (es.osl.size() == 0) {
        out << "empty encset";
    }
    for (auto it : es.osl) {
        out << "(onion " << it.first
            << ", level " << TypeText<SECLEVEL>::toText(it.second.first)
            << ", field `" << (it.second.second == NULL ? "*" : it.second.second->fname) << "`"
            << ") ";
    }
    return out;
}


void
EncSet::setFieldForOnion(onion o, FieldMeta * fm) {
    LevelFieldPair lfp = getAssert(osl, o);

    osl[o] = LevelFieldPair(lfp.first, fm);
}

OLK
EncSet::chooseOne() const
{
    // Order of selection is encoded in this array.
    // The onions appearing earlier are the more preferred ones.
    static const onion onion_order[] = {
        oDET,
        oOPE,
        oAGG,
        oSWP,
        oPLAIN
    };

    static size_t onion_size =
        sizeof(onion_order) / sizeof(onion_order[0]);
    for (size_t i = 0; i < onion_size; i++) {
        onion o = onion_order[i];
        auto it = osl.find(o);
        if (it != osl.end()) {
            // HACK.
            if (it->second.second == 0 &&
                (it->second.first != SECLEVEL::PLAINVAL &&
                 o != oPLAIN)) {
                /*
                 * If no key, skip this OLK.
                 */
                continue;
            }

            return OLK(o, it->second.first, it->second.second);
        }
    }

    throw CryptDBError("No OLK to choose!");
}

bool
EncSet::contains(const OLK & olk) const {
    auto it = osl.find(olk.o);
    if (it == osl.end()) {
        return false;
    }
    if (it->second.first == olk.l) {
        return true;
    }
    return false;
}

bool
EncSet::hasSecLevel(SECLEVEL level) const
{
    for (auto it : osl) {
        if (level == it.second.first) {
            return true;
        }
    }

    return false;
}

bool
needsSalt(EncSet es) {
    for (auto pair : es.osl) {
        if (pair.second.first == SECLEVEL::RND) {
            return true;
        }
    }

    return false;
}

std::ostream&
operator<<(std::ostream &out, const reason &r)
{
    out << r.why_t_item << " PRODUCES encset " << r.encset << "\n" \
        << " BECAUSE " << r.why_t << "\n";

    if (r.childr->size()) {
        out << " AND CHILDREN: {" << "\n";
        for (reason ch : *r.childr) {
            out << ch;
        }
        out << "} \n";
    }
    return out;
}


/*
void
RewritePlan::restrict(const EncSet & es) {
    es_out = es_out.intersect(es);
    assert_s(!es_out.empty(), "after restrict rewrite plan has empty encset");

    if (plan.size()) { //node has children
        for (auto pair = plan.begin(); pair != plan.end(); pair++) {
            if (!es.contains(pair->first)) {
            plan.erase(pair);
            }
        }
    }
}
*/

std::ostream&
operator<<(std::ostream &out, const RewritePlan * rp)
{
    if (!rp) {
        out << "NULL RewritePlan";
        return out;
    }

    out << " RewritePlan: \n---> out encset " << rp->es_out << "\n---> reason " << rp->r << "\n";

    return out;
}

static void
dropAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        std::stringstream ss;
        ss << "DROP FUNCTION IF EXISTS " << convert_lex_str(u->name) << ";";
        assert_s(conn->execute(ss.str()), ss.str());
    }
}

static void
createAll(Connect * conn)
{
    for (udf_func* u: udf_list) {
        std::stringstream ss;
        ss << "CREATE ";
        if (u->type == UDFTYPE_AGGREGATE) ss << "AGGREGATE ";
        ss << "FUNCTION " << u->name.str << " RETURNS ";
        switch (u->returns) {
            case INT_RESULT:    ss << "INTEGER"; break;
            case STRING_RESULT: ss << "STRING";  break;
            default:            thrower() << "unknown return " << u->returns;
        }
        ss << " SONAME 'edb.so';";
        assert_s(conn->execute(ss.str()), ss.str());
    }
}

static void
loadUDFs(Connect * conn) {
    //need a database for the UDFs
    assert_s(conn->execute("DROP DATABASE IF EXISTS cryptdb_udf"), "cannot drop db for udfs even with 'if exists'");
    assert_s(conn->execute("CREATE DATABASE cryptdb_udf;"), "cannot create db for udfs");
    assert_s(conn->execute("USE cryptdb_udf;"), "cannot use db");
    dropAll(conn);
    createAll(conn);
    LOG(cdb_v) << "Loaded CryptDB's UDFs.";
}

ProxyState::ProxyState(ConnectionInfo ci, const std::string &embed_dir,
                       const std::string &dbname,
                       const std::string &master_key,
                       SECURITY_RATING default_sec_rating)
    : masterKey(getKey(master_key)), dbname(dbname),
      default_sec_rating(default_sec_rating), previous_schema(NULL),
      schema_staleness(true)
{
    init_mysql(embed_dir);

    e_conn = Connect::getEmbedded(embed_dir, dbname);
    conn = new Connect(ci.server, ci.user, ci.passwd, dbname, ci.port);

    MetaDataTables::initialize(conn, e_conn);

    loadUDFs(conn);

    // HACK: This is necessary as above functions use a USE statement.
    // ie, loadUDFs.
    assert(conn->execute("USE cryptdbtest;"));
    assert(e_conn->execute("USE cryptdbtest;"));
}

ProxyState::~ProxyState()
{
    if (conn) {
        delete conn;
        conn = NULL;
    }
    if (e_conn) {
        delete e_conn;
        e_conn = NULL;
    }
}

std::string Delta::tableNameFromType(TableType table_type) const
{
    switch (table_type) {
        case REGULAR_TABLE: {
            return MetaDataTables::Name::metaObject();
        }
        case BLEEDING_TABLE: {
            return MetaDataTables::Name::bleedingMetaObject();
        }
        default: {
            throw CryptDBError("Unrecognized table type!");
        }
    }
}

// TODO: Remove asserts.
// Recursive.
bool CreateDelta::apply(Connect *e_conn, TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);
    std::function<void(Connect *e_conn, const DBMeta * const,
                       const DBMeta * const,
                       const AbstractMetaKey * const,
                       const unsigned int * const)> helper =
        [&helper, table_name] (Connect *e_conn,
                               const DBMeta * const object,
                               const DBMeta * const parent,
                               const AbstractMetaKey * const k,
                               const unsigned int * const ptr_parent_id)
    {
        const std::string child_serial = object->serialize(*parent);
        assert(0 == object->getDatabaseID());
        unsigned int parent_id;
        if (ptr_parent_id) {
            parent_id = *ptr_parent_id;
        } else {
            parent_id = parent->getDatabaseID();
        }

        std::string serial_key;
        if (NULL == k) {
            AbstractMetaKey *ck = parent->getKey(object);
            assert(ck);
            serial_key = ck->getSerial();
        } else {
            serial_key = k->getSerial();
        }
        const std::string esc_serial_key =
            escapeString(e_conn, serial_key);

        // ------------------------
        //    Build the queries.
        // ------------------------

        // On CREATE, the database generates a unique ID for us.
        const std::string esc_child_serial =
            escapeString(e_conn, child_serial);

        const std::string query =
            " INSERT INTO pdb." + table_name + 
            "    (serial_object, serial_key, parent_id) VALUES (" 
            " '" + esc_child_serial + "',"
            " '" + esc_serial_key + "',"
            " " + std::to_string(parent_id) + ");";
        assert(e_conn->execute(query));

        const unsigned int object_id = e_conn->last_insert_id();

        std::function<void(const DBMeta * const)> localCreateHandler =
            [&e_conn, &object, object_id, &helper]
                (const DBMeta * const child)
            {
                helper(e_conn, child, object, NULL, &object_id);
            };
        object->applyToChildren(localCreateHandler);
    };

    helper(e_conn, meta, parent_meta, key, NULL);
    return true;
}

bool ReplaceDelta::apply(Connect *e_conn, TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);

    const unsigned int child_id = meta->getDatabaseID();
    
    const std::string child_serial = meta->serialize(*parent_meta);
    const std::string esc_child_serial =
        escapeString(e_conn, child_serial);
    const std::string serial_key = key->getSerial();
    const std::string esc_serial_key = escapeString(e_conn, serial_key);

    const std::string query = 
        " UPDATE pdb." + table_name +
        "    SET serial_object = '" + esc_child_serial + "', "
        "        serial_key = '" + esc_serial_key + "'"
        "  WHERE id = " + std::to_string(child_id) + ";";

    assert(e_conn->execute(query));

    return true;
}

bool DeleteDelta::apply(Connect *e_conn, TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);
    std::function<void(Connect *, const DBMeta * const,
                       const DBMeta * const)> helper =
        [&helper, table_name](Connect *e_conn,
                              const DBMeta * const object,
                              const DBMeta * const parent)
    {
        const unsigned int object_id = object->getDatabaseID();
        const unsigned int parent_id = parent->getDatabaseID();

        const std::string query =
            " DELETE pdb." + table_name + " "
            "   FROM pdb." + table_name + 
            "  WHERE pdb." + table_name + ".id" +
            "      = "     + std::to_string(object_id) + 
            "    AND pdb." + table_name + ".parent_id" +
            "      = "     + std::to_string(parent_id) + ";";

        assert(e_conn->execute(query));

        std::function<void(const DBMeta * const)> localDestroyHandler =
            [&e_conn, &object, &helper] (const DBMeta * const child) {
                helper(e_conn, child, object);
            };
        object->applyToChildren(localDestroyHandler);
    };

    helper(e_conn, meta, parent_meta); 
    return true;
}

RewriteOutput::~RewriteOutput()
{;}

bool RewriteOutput::queryAgain() const
{
    return false;
}

bool RewriteOutput::doDecryption() const
{
    return true;
}

bool RewriteOutput::stalesSchema() const
{
    return false;
}

ResType *RewriteOutput::sendQuery(Connect *c, const std::string &q)
{
    DBResult *dbres;
    assert(c->execute(q, dbres));
    ResType *res = new ResType(dbres->unpack());

    return res;
}

bool SimpleOutput::beforeQuery(Connect *conn, Connect *e_conn)
{
    return true;
}

bool SimpleOutput::getQuery(std::list<std::string> *queryz) const
{
    queryz->clear();
    queryz->push_back(original_query);

    return true;
}

bool SimpleOutput::handleQueryFailure(Connect *e_conn) const
{
    return true;
}

bool SimpleOutput::afterQuery(Connect *e_conn) const
{
    return true;
}

bool SimpleOutput::doDecryption() const
{
    return false;
}

bool DMLOutput::beforeQuery(Connect *conn, Connect *e_conn)
{
    return true;
}

bool DMLOutput::getQuery(std::list<std::string> *queryz) const
{
    queryz->clear();
    queryz->push_back(new_query);

    return true;
}

bool DMLOutput::handleQueryFailure(Connect *e_conn) const
{
    return true;
}

bool DMLOutput::afterQuery(Connect *e_conn) const
{
    return true;
}

bool SpecialUpdate::beforeQuery(Connect *conn, Connect *e_conn)
{
    // Retrieve rows from database.
    std::ostringstream select_stream;
    select_stream << " SELECT * FROM " << this->plain_table
                  << " WHERE " << this->where_clause << ";";
    // FIXME: const_cast
    const ResType * const select_res_type =
        executeQuery(const_cast<ProxyState &>(this->ps),
                     select_stream.str());
    assert(select_res_type);
    if (select_res_type->rows.size() == 0) { // No work to be done.
        return sendQuery(conn, new_query);
    }

    const auto itemJoin = [](std::vector<Item*> row) {
        return "(" +
               vector_join<Item*>(row, ",", ItemToStringWithQuotes) +
               ")";
    };
    const std::string values_string =
        vector_join<std::vector<Item*>>(select_res_type->rows, ",",
                                        itemJoin);
    delete select_res_type;

    // Push the plaintext rows to the embedded database.
    std::ostringstream push_stream;
    push_stream << " INSERT INTO " << this->plain_table
                << " VALUES " << values_string << ";";
    assert(e_conn->execute(push_stream.str()));

    // Run the original (unmodified) query on the data in the embedded
    // database.
    assert(e_conn->execute(this->original_query));

    // > Collect the results from the embedded database.
    // > This code relies on single threaded access to the database
    //   and on the fact that the database is cleaned up after
    //   every such operation.
    DBResult *dbres;
    std::ostringstream select_results_stream;
    select_results_stream << " SELECT * FROM " << this->plain_table << ";";
    assert(e_conn->execute(select_results_stream.str(), dbres));
    ResType * const interim_res = new ResType(dbres->unpack());
    this->output_values = 
        vector_join<std::vector<Item*>>(interim_res->rows, ",",
                                        itemJoin);
    // Cleanup the embedded database.
    std::ostringstream cleanup_stream;
    cleanup_stream << "DELETE FROM " << this->plain_table << ";";
    assert(e_conn->execute(cleanup_stream.str()));

    return true;
}

bool SpecialUpdate::getQuery(std::list<std::string> *queryz) const
{
    queryz->clear();
    queryz->push_back("START TRANSACTION; ");

    // FIXME: Broken, these queries must be rewritten.

    // DELETE the rows matching the WHERE clause from the database.
    std::ostringstream delete_stream;
    delete_stream << " DELETE FROM " << this->plain_table
                  << " WHERE " << this->where_clause << ";";
    const std::string re_delete =
        rewriteAndGetSingleQuery(ps, delete_stream.str());
    queryz->push_back(re_delete);

    // > Add each row from the embedded database to the data database.
    std::ostringstream push_results_stream;
    push_results_stream << " INSERT INTO " << this->plain_table
                        << " VALUES " << this->output_values.get()
                        << ";";
    const std::string re_push =
        rewriteAndGetSingleQuery(ps, push_results_stream.str());
    queryz->push_back(re_push);

    queryz->push_back("COMMIT; ");

    return true;
}

// FIXME: Implement.
bool SpecialUpdate::handleQueryFailure(Connect *e_conn) const
{
    return false;
}

bool SpecialUpdate::afterQuery(Connect *e_conn) const
{
    return true;
}

DeltaOutput::~DeltaOutput()
{;}

bool DeltaOutput::stalesSchema() const
{
    return true;
}

bool DeltaOutput::save(Connect *e_conn, unsigned long *delta_output_id)
{
    const std::string table_name = "DeltaOutput";
    const std::string query =
        " INSERT INTO pdb." + table_name +
        "    () VALUES ();";
    assert(e_conn->execute(query));

    *delta_output_id = e_conn->last_insert_id();

    return true;
}

bool DeltaOutput::destroyRecord(Connect *e_conn,
                                unsigned long delta_output_id)
{
    const std::string table_name = "DeltaOutput";
    const std::string delete_query =
        " DELETE pdb." + table_name +
        "   FROM pdb." + table_name +
        "  WHERE pdb." + table_name + ".id" +
        "      = "     + std::to_string(delta_output_id) + ";";
    assert(e_conn->execute(delete_query));

    return true;
}

static bool saveQuery(Connect *e_conn, const std::string &query,
                      unsigned long delta_output_id, bool local, bool ddl)
{
    const std::string table_name = MetaDataTables::Name::query();
    const std::string insert_query =
        " INSERT INTO pdb." + table_name +
        "   (query, delta_output_id, local, ddl) VALUES ("
        " '" + escapeString(e_conn, query) + "', "
        " "  + std::to_string(delta_output_id) + ","
        " "  + bool_to_string(local) + ","
        " "  + bool_to_string(ddl) + ");";
    assert(e_conn->execute(insert_query));

    return true;
}

static bool destroyQueryRecord(Connect *e_conn,
                               unsigned long delta_output_id)
{
    const std::string table_name = MetaDataTables::Name::query();
    const std::string delete_query =
        " DELETE pdb." + table_name +
        "   FROM pdb." + table_name +
        "  WHERE pdb." + table_name + ".delta_output_id"
        "      = "     + std::to_string(delta_output_id) + ";";
    assert(e_conn->execute(delete_query));

    return true;
}

static
std::string
dmlCompletionQuery(unsigned long delta_output_id)
{
    const std::string dml_table =
        MetaDataTables::Name::dmlCompletion();
    const std::string dml_insert_query =
        " INSERT INTO " + dml_table +
        "   (delta_output_id) VALUES ("
        " " + std::to_string(delta_output_id) + ");";

    return dml_insert_query;
}

bool
saveDMLCompletion(Connect *conn, unsigned long delta_output_id)
{
    assert(conn->execute(dmlCompletionQuery(delta_output_id)));

    return true;
}

static bool
tableCopy(Connect *c, const std::string &src, const std::string &dest)
{
    const std::string delete_query =
        " DELETE FROM " + dest + ";";
    assert(c->execute(delete_query));

    const std::string insert_query =
        " INSERT " + dest +
        "   SELECT * FROM " + src + ";";
    assert(c->execute(insert_query));

    return true;
}

bool
setRegularTableToBleedingTable(Connect *e_conn)
{
    const std::string src = "pdb." +
                            MetaDataTables::Name::bleedingMetaObject();
    const std::string dest = "pdb." +
                             MetaDataTables::Name::metaObject();
    return tableCopy(e_conn, src, dest);
}

static bool
setBleedingTableToRegularTable(Connect *e_conn)
{
    const std::string src = "pdb." +
                            MetaDataTables::Name::metaObject();
    const std::string dest = "pdb." +
                             MetaDataTables::Name::bleedingMetaObject();
    return tableCopy(e_conn, src, dest);
}

static bool
revertAndCleanupEmbedded(Connect *e_conn, unsigned long delta_output_id)
{
    assert(e_conn->execute("START TRANSACTION;"));

    if (!setBleedingTableToRegularTable(e_conn)) {
        throw CryptDBError("bleedingTable=regularTable failed!");
    }
    if (!cleanupDeltaOutputAndQuery(e_conn, delta_output_id)) {
        throw CryptDBError("cleaning up delta failed!");
    }

    assert(e_conn->execute("COMMIT;"));

    return true;
}

bool
cleanupDeltaOutputAndQuery(Connect *e_conn,
                           unsigned long delta_output_id)
{
    assert(DeltaOutput::destroyRecord(e_conn, delta_output_id));
    assert(destroyQueryRecord(e_conn, delta_output_id));

    return true;
}

static
bool
handleDeltaBeforeQuery(Connect *conn, Connect *e_conn,
                       std::vector<Delta *> deltas,
                       std::list<std::string> local_qz,
                       std::list<std::string> remote_qz,
                       bool remote_ddl, unsigned long *delta_output_id)
{
    if (remote_ddl) {
        assert(remote_qz.size() == 1);
    }
    assert(local_qz.size() == 0 || local_qz.size() == 1);

    // Write to the bleeding table, store delta and store query.
    bool b;
    assert(e_conn->execute("START TRANSACTION;"));
    for (auto it : deltas) {
        b = it->apply(e_conn, Delta::BLEEDING_TABLE);
        ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);
    }

    b = DeltaOutput::save(e_conn, delta_output_id);
    ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);

    for (auto it : local_qz) {
        b = saveQuery(e_conn, it, *delta_output_id, true, false);
        ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);
    }
    for (auto it : remote_qz) {
        b = saveQuery(e_conn, it, *delta_output_id, false, remote_ddl);
        ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);
    }
    assert(e_conn->execute("COMMIT;"));

    return true;
}
/*
    // -----------------------------------------------------------

    // Execute rewritten query @ remote.
    // > This works because remote_qz will either have ONE ddl query
    //   or multiple DML queries.
    ResType *result;
    {
        if (true == remote_ddl) {
            result = RewriteOutput::sendQuery(conn, remote_qz.back());
            if (!result || !result->ok) {
                revertAndCleanupEmbedded(e_conn, delta_output_id);
                return NULL;
            }
        } else {
            assert(conn->execute("START TRANSACTION;"));
            for (auto it : remote_qz) {
                result = RewriteOutput::sendQuery(conn, it);
                // If the query failed, rollback.
                if (!result || !result->ok) {
                    assert(conn->execute("ROLLBACK;"));
                    revertAndCleanupEmbedded(e_conn, delta_output_id);
                    return NULL;
                }
            }
            b = saveDMLCompletion(conn, delta_output_id);
            ROLLBACK_AND_RETURN_ON_FAIL(b, conn, NULL);

            assert(conn->execute("COMMIT;"));
        }
    }
*/

static
bool
handleDeltaAfterQuery(Connect *e_conn,
                      std::vector<Delta *> deltas,
                      std::list<std::string> local_qz,
                      unsigned long delta_output_id)
{
    bool b;

    // > Write to regular table and apply original query.
    // > Remove delta and original query from embedded db.
    assert(e_conn->execute("START TRANSACTION;"));
    for (auto it : deltas) {
        b = it->apply(e_conn, Delta::REGULAR_TABLE);
        ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);
    }

    b = cleanupDeltaOutputAndQuery(e_conn, delta_output_id);
    ROLLBACK_AND_RETURN_ON_FAIL(b, e_conn, false);

    assert(e_conn->execute("COMMIT;"));

    // FIXME: local_qz can have DDL.
    // > This can be fixed with a bleeding table.
    assert(local_qz.size() <= 1);
    for (auto it : local_qz) {
        assert(e_conn->execute(it));
    }

    return true;
}

bool DDLOutput::beforeQuery(Connect *conn, Connect *e_conn)
{
    unsigned long delta_id;
    bool b = handleDeltaBeforeQuery(conn, e_conn, deltas, local_qz(),
                                    remote_qz(), true, &delta_id);
    this->delta_output_id = delta_id;

    return b;
}

bool DDLOutput::getQuery(std::list<std::string> *queryz) const
{
    queryz->clear();

    assert(remote_qz().size() == 1);
    queryz->push_back(remote_qz().back());

    return true;
}

bool DDLOutput::handleQueryFailure(Connect *e_conn) const
{
    assert(revertAndCleanupEmbedded(e_conn, this->delta_output_id.get()));
    return true;
}

bool DDLOutput::afterQuery(Connect *e_conn) const
{
    return handleDeltaAfterQuery(e_conn, deltas, local_qz(),
                                 this->delta_output_id.get());
}

const std::list<std::string> DDLOutput::remote_qz() const
{
    return std::list<std::string>({new_query});
}

const std::list<std::string> DDLOutput::local_qz() const
{
    return std::list<std::string>({original_query});
}

bool AdjustOnionOutput::beforeQuery(Connect *conn, Connect *e_conn)
{
    unsigned long delta_id;
    bool b = handleDeltaBeforeQuery(conn, e_conn, deltas, local_qz(),
                                    remote_qz(), false, &delta_id);
    this->delta_output_id = delta_id;

    return b;
}

bool AdjustOnionOutput::getQuery(std::list<std::string> *queryz) const
{
    queryz->clear();
    queryz->push_back("START TRANSACTION; ");

    for (auto it : remote_qz()) {
        queryz->push_back(it);
    }

    queryz->push_back(dmlCompletionQuery(this->delta_output_id.get()));
    queryz->push_back("COMMIT; ");

    return true;
}

bool AdjustOnionOutput::handleQueryFailure(Connect *e_conn) const
{
    assert(revertAndCleanupEmbedded(e_conn, this->delta_output_id.get()));
    return true;
}

bool AdjustOnionOutput::afterQuery(Connect *e_conn) const
{
    return handleDeltaAfterQuery(e_conn, deltas, local_qz(),
                                 this->delta_output_id.get());
}

const std::list<std::string> AdjustOnionOutput::remote_qz() const
{
    return std::list<std::string>(adjust_queries);
}

const std::list<std::string> AdjustOnionOutput::local_qz() const
{
    return std::list<std::string>();
}

bool AdjustOnionOutput::queryAgain() const
{
    return true;
}

bool Analysis::addAlias(const std::string &alias,
                        const std::string &table)
{
    auto alias_pair = table_aliases.find(alias);
    if (table_aliases.end() != alias_pair) {
        return false;
    }

    table_aliases[alias] = table;
    return true;
}

OnionMeta *Analysis::getOnionMeta(const std::string &table,
                                  const std::string &field,
                                  onion o) const
{
    OnionMeta *om = this->getFieldMeta(table, field)->getOnionMeta(o);
    assert(om);

    return om;
}

FieldMeta *Analysis::getFieldMeta(const std::string &table,
                                  const std::string &field) const
{
    const std::string real_table_name = unAliasTable(table);
    FieldMeta * const fm =
        this->schema->getFieldMeta(real_table_name, field);
    assert(fm);
    return fm;
}

FieldMeta *Analysis::getFieldMeta(const TableMeta *tm,
                                  const std::string &field) const
{
    const IdentityMetaKey * const key = new IdentityMetaKey(field);
    FieldMeta *fm = tm->getChild(key);
    assert(fm);
    return fm;
}

TableMeta *Analysis::getTableMeta(const std::string &table) const
{
    const IdentityMetaKey * const key =
        new IdentityMetaKey(unAliasTable(table));

    TableMeta * const tm = this->schema->getChild(key);
    assert(tm);
    return tm;
}

bool Analysis::tableMetaExists(const std::string &table) const
{
    IdentityMetaKey * const key =
        new IdentityMetaKey(unAliasTable(table));
    return this->schema->childExists(key);
}

std::string Analysis::getAnonTableName(const std::string &table) const
{
    return this->getTableMeta(table)->getAnonTableName();
}

std::string Analysis::getAnonIndexName(const std::string &table,
                                       const std::string &index_name) const
{
    return this->getTableMeta(table)->getAnonIndexName(index_name); 
}

std::string Analysis::getAnonIndexName(const TableMeta *tm,
                                       const std::string &index_name) const
{
    return tm->getAnonIndexName(index_name); 
}

std::string Analysis::unAliasTable(const std::string &table) const
{
    auto alias_pair = table_aliases.find(table);
    if (table_aliases.end() != alias_pair) {
        return alias_pair->second;
    } else {
        return table;
    }
}

EncLayer *Analysis::getBackEncLayer(OnionMeta * const om) const
{
    auto it = to_adjust_enc_layers.find(om);
    if (to_adjust_enc_layers.end() == it) {
        return om->layers.back();
    } else { 
        return it->second.back();
    }
}

EncLayer *Analysis::popBackEncLayer(OnionMeta * const om)
{
    auto it = to_adjust_enc_layers.find(om);
    if (to_adjust_enc_layers.end() == it) { // First onion adjustment
        to_adjust_enc_layers[om] = om->layers;
    }

    EncLayer *out_layer = to_adjust_enc_layers[om].back();
    to_adjust_enc_layers[om].pop_back();

    return out_layer;
}

SECLEVEL Analysis::getOnionLevel(OnionMeta * const om) const
{
    auto it = to_adjust_enc_layers.find(om);
    if (to_adjust_enc_layers.end() == it) {
        return om->getSecLevel();
    } else {
        return it->second.back()->level();
    }
}

std::vector<EncLayer *> Analysis::getEncLayers(OnionMeta * const om) const
{
    auto it = to_adjust_enc_layers.find(om);
    if (to_adjust_enc_layers.end() == it) {
        return om->layers;
    } else {
        return it->second;
    }
}

#undef ROLLBACK_AND_RETURN_ON_FAIL

