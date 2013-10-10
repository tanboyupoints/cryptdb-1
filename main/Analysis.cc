#include <main/Analysis.hh>
#include <main/rewrite_util.hh>
#include <main/rewrite_main.hh>
#include <main/metadata_tables.hh>
#include <main/macro_util.hh>
#include <main/stored_procedures.hh>

// FIXME: Memory leaks when we allocate MetaKey<...>, use smart pointer.

// FIXME: Wrong interfaces.
EncSet::EncSet(Analysis &a, FieldMeta * const fm) {
    TEST_TextMessageError(0 != fm->children.size(),
                         "FieldMeta has no children!");

    osl.clear();
    for (auto pair = fm->children.begin(); pair != fm->children.end();
         pair++) {
        OnionMeta *const om = (*pair).second.get();
        OnionMetaKey const &key = (*pair).first;
        osl[key.getValue()] = LevelFieldPair(a.getOnionLevel(*om), fm);
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
            FieldMeta * const fm = it->second.second;
            FieldMeta * const fm2 = it2->second.second;

            const onion o = it->first;
            const onion o2 = it2->first;

            assert(o == o2);

            const SECLEVEL sl =
                static_cast<SECLEVEL>(
                        min(static_cast<int>(it->second.first),
                            static_cast<int>(it2->second.first)));

            /*
             * FIXME: Each clause of this if statement should make sure
             * that it's OnionMeta actually has the SecLevel.
             */
            if (fm == NULL) {
                m[o] = LevelFieldPair(sl, fm2);
            } else if (fm2 == NULL) {
                m[it->first] = LevelFieldPair(sl, fm);
            } else {
                // This can succeed in three cases.
                // 1> Same field, so same key.
                // 2> Different fields, but SECLEVEL is PLAINVAL
                //    or DETJOIN so same key.
                // 3> Differt fields, and SECLEVEL is HOM so
                //    we will do computation client side if necessary.
                const OnionMeta * const om = fm->getOnionMeta(o);
                const OnionMeta * const om2 = fm2->getOnionMeta(o);
                // HACK: To determine if the keys are the same.
                if ((om->hasEncLayer(sl) && om2->hasEncLayer(sl)
                     && om->getLayer(sl)->doSerialize() ==
                        om2->getLayer(sl)->doSerialize())) {
                    m[o] = LevelFieldPair(sl, fm);
                }
            }
        }
    }
    return EncSet(m);
}

std::ostream&
operator<<(std::ostream &out, const EncSet &es)
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
        oPLAIN,
    };

    static size_t onion_size =
        sizeof(onion_order) / sizeof(onion_order[0]);
    for (size_t i = 0; i < onion_size; i++) {
        const onion o = onion_order[i];
        const auto it = osl.find(o);
        if (it != osl.end()) {
            if (SECLEVEL::INVALID == it->second.first) {
                continue;
            }
            if (0 == it->second.second
                && (it->second.first != SECLEVEL::PLAINVAL
                    && o != oPLAIN)) {
                /*
                 * If no key, skip this OLK.
                 */
                continue;
            }

            return OLK(o, it->second.first, it->second.second);
        }
    }

    return OLK::invalidOLK();
}

bool
EncSet::contains(const OLK &olk) const {
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
        if (it.second.first == level) {
            return true;
        }
    }

    return false;
}

SECLEVEL
EncSet::onionLevel(onion o) const
{
    for (auto it : osl) {
        if (it.first == o) {
            return it.second.first;
        }
    }

    assert(false);
}

bool
EncSet::available() const
{
    return OLK::isNotInvalid(this->chooseOne());
}

bool EncSet::single_crypted_and_or_plainvals() const
{
    unsigned int crypted = 0;
    unsigned int plain = 0;
    for (auto it : osl) {
        if (SECLEVEL::PLAINVAL == it.second.first) {
            ++plain;
        } else {
            ++crypted;
        }
    }

    return 1 >= crypted || plain > 0;
}

OLK EncSet::extract_singleton() const
{
    assert_s(singleton(), std::string("encset has size ") +
                            StringFromVal(osl.size()));
    const auto it = osl.begin();
    return OLK(it->first, it->second.first, it->second.second);
}

// needsSaltz must have consistent semantics.
static bool
needsSalt(SECLEVEL l)
{
    return l == SECLEVEL::RND;
}

bool
needsSalt(OLK olk)
{
    return olk.key && olk.key->getHasSalt() && needsSalt(olk.l);
}

bool
needsSalt(EncSet es)
{
    for (auto pair : es.osl) {
        OLK olk(pair.first, pair.second.first, pair.second.second);
        if (needsSalt(olk)) {
            return true;
        }
    }

    return false;
}

std::ostream&
operator<<(std::ostream &out, const reason &r)
{
    out << r.item << " PRODUCES encset " << r.encset << std::endl
        << " BECAUSE " << r.why << std::endl;

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
operator<<(std::ostream &out, const RewritePlan * const rp)
{
    if (!rp) {
        out << "NULL RewritePlan";
        return out;
    }

    out << " RewritePlan: \n---> out encset " << rp->es_out << "\n---> reason " << rp->r << "\n";

    return out;
}

// This function should not be used after intitialization.
bool
lowLevelGetCurrentDatabase(Connect *const c, std::string *const out_db)
{
    const std::string query = "SELECT DATABASE();";
    std::unique_ptr<DBResult> db_res;
    RFIF(c->execute(query, &db_res));
    assert(1 == mysql_num_rows(db_res->n));

    MYSQL_ROW row = mysql_fetch_row(db_res->n);
    unsigned long *const l = mysql_fetch_lengths(db_res->n);
    assert(l != NULL);

    *out_db = std::string(row[0], l[0]);
    if (out_db->size() == 0) {
        assert(0 == l[0] && NULL == row[0]);
        return false;
    }

    return true;
}

// This function should not be used after intitialization.
static bool
lowLevelSetCurrentDatabase(Connect *const c, const std::string &db)
{
    const std::string query = "USE " + db + ";";
    RFIF(c->execute(query));

    return true;
}

static void
dropAll(const std::unique_ptr<Connect> &conn)
{
    for (const udf_func * const u: udf_list) {
        const std::string s =
            "DROP FUNCTION IF EXISTS " + convert_lex_str(u->name) + ";";
        assert_s(conn->execute(s), s);
    }
}

static void
createAll(const std::unique_ptr<Connect> &conn)
{
    for (const udf_func * const u: udf_list) {
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
        assert_s(conn.get()->execute(ss.str()), ss.str());
    }
}

static void
loadUDFs(const std::unique_ptr<Connect> &conn) {
    const std::string udf_db = "cryptdb_udf";
    assert_s(conn.get()->execute("DROP DATABASE IF EXISTS " + udf_db), "cannot drop db for udfs even with 'if exists'");
    assert_s(conn.get()->execute("CREATE DATABASE " + udf_db), "cannot create db for udfs");

    std::string saved_db;
    assert(lowLevelGetCurrentDatabase(conn.get(), &saved_db));
    assert(lowLevelSetCurrentDatabase(conn.get(), udf_db));
    dropAll(conn);
    createAll(conn);
    assert(lowLevelSetCurrentDatabase(conn.get(), saved_db));

    LOG(cdb_v) << "Loaded CryptDB's UDFs.";
}

static bool
synchronizeDatabases(Connect *const conn, Connect *const e_conn)
{
    std::string current_db;
    if (lowLevelGetCurrentDatabase(conn, &current_db)) {
        RFIF(lowLevelSetCurrentDatabase(e_conn, current_db));
        return true;
    }

    RFIF(lowLevelSetCurrentDatabase(conn,
                                    MetaData::DB::purgatory()));
    RFIF(lowLevelSetCurrentDatabase(e_conn,
                                    MetaData::DB::purgatory()));
    return true;
}

ProxyState::ProxyState(ConnectionInfo ci, const std::string &embed_dir,
                       const std::string &master_key,
                       SECURITY_RATING default_sec_rating)
    : masterKey(std::unique_ptr<AES_KEY>(getKey(master_key))),
      mysql_dummy(ProxyState::db_init(embed_dir)), // HACK: Allows
                                                   // connections in init
                                                   // list.
      conn(new Connect(ci.server, ci.user, ci.passwd, ci.port)),
      e_conn(Connect::getEmbedded(embed_dir)), 
      default_sec_rating(default_sec_rating)
{
    assert(conn && e_conn);

    // Must be done before database synchronization.
    const std::string &prefix = 
        getenv("CRYPTDB_NAME") ? getenv("CRYPTDB_NAME")
                               : "generic_prefix_";
    assert(MetaData::initialize(conn, e_conn, prefix));

    TEST_TextMessageError(synchronizeDatabases(conn.get(), e_conn.get()),
                          "Failed to synchronize embedded and remote"
                          " databases!");

    
    loadUDFs(conn);

    assert(loadStoredProcedures(conn));
}

ProxyState::~ProxyState()
{
    // mysql_library_end();
}

int ProxyState::db_init(const std::string &embed_dir)
{
    init_mysql(embed_dir);
    return 1;
}

std::string Delta::tableNameFromType(TableType table_type) const
{
    switch (table_type) {
        case REGULAR_TABLE: {
            return MetaData::Table::metaObject();
        }
        case BLEEDING_TABLE: {
            return MetaData::Table::bleedingMetaObject();
        }
        default: {
            FAIL_TextMessageError("Unrecognized table type!");
        }
    }
}

// TODO: Remove asserts.
// Recursive.
bool CreateDelta::apply(const std::unique_ptr<Connect> &e_conn,
                        TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);
    std::function<void(const std::unique_ptr<Connect> &e_conn,
                       const DBMeta &, const DBMeta &,
                       const AbstractMetaKey * const,
                       const unsigned int * const)> helper =
        [&helper, table_name] (const std::unique_ptr<Connect> &e_conn,
                               const DBMeta &object,
                               const DBMeta &parent,
                               const AbstractMetaKey * const k,
                               const unsigned int * const ptr_parent_id)
    {
        const std::string child_serial = object.serialize(parent);
        assert(0 == object.getDatabaseID());
        unsigned int parent_id;
        if (ptr_parent_id) {
            parent_id = *ptr_parent_id;
        } else {
            parent_id = parent.getDatabaseID();
        }

        std::function<std::string(const DBMeta &, const DBMeta &,
                                  const AbstractMetaKey *const)>
            getSerialKey =
                [] (const DBMeta &p, const DBMeta &o,
                    const AbstractMetaKey *const keee)
            {
                if (NULL == keee) {
                    return p.getKey(o).getSerial();  /* lambda */
                }

                return keee->getSerial();      /* lambda */
            };

        const std::string serial_key = getSerialKey(parent, object, k);
        const std::string esc_serial_key =
            escapeString(e_conn, serial_key);

        // ------------------------
        //    Build the queries.
        // ------------------------

        // On CREATE, the database generates a unique ID for us.
        const std::string esc_child_serial =
            escapeString(e_conn, child_serial);

        const std::string query =
            " INSERT INTO " + table_name + 
            "    (serial_object, serial_key, parent_id) VALUES (" 
            " '" + esc_child_serial + "',"
            " '" + esc_serial_key + "',"
            " " + std::to_string(parent_id) + ");";
        assert(e_conn->execute(query));

        const unsigned int object_id = e_conn->last_insert_id();

        std::function<void(const DBMeta &)> localCreateHandler =
            [&e_conn, &object, object_id, &helper]
                (const DBMeta &child)
            {
                helper(e_conn, child, object, NULL, &object_id);
            };
        object.applyToChildren(localCreateHandler);
    };

    helper(e_conn, *meta.get(), parent_meta, &key, NULL);
    return true;
}

bool ReplaceDelta::apply(const std::unique_ptr<Connect> &e_conn,
                         TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);

    const unsigned int child_id = meta.getDatabaseID();

    const std::string child_serial = meta.serialize(parent_meta);
    const std::string esc_child_serial =
        escapeString(e_conn, child_serial);
    const std::string serial_key = key.getSerial();
    const std::string esc_serial_key = escapeString(e_conn, serial_key);

    const std::string query = 
        " UPDATE " + table_name +
        "    SET serial_object = '" + esc_child_serial + "', "
        "        serial_key = '" + esc_serial_key + "'"
        "  WHERE id = " + std::to_string(child_id) + ";";

    assert(e_conn->execute(query));

    return true;
}

bool DeleteDelta::apply(const std::unique_ptr<Connect> &e_conn,
                        TableType table_type)
{
    const std::string table_name = tableNameFromType(table_type);
    Connect * const e_c = e_conn.get();
    std::function<void(const DBMeta &, const DBMeta &)> helper =
        [&e_c, &helper, table_name](const DBMeta &object,
                                    const DBMeta &parent)
    {
        const unsigned int object_id = object.getDatabaseID();
        const unsigned int parent_id = parent.getDatabaseID();

        const std::string query =
            " DELETE " + table_name + " "
            "   FROM " + table_name + 
            "  WHERE " + table_name + ".id" +
            "      = "     + std::to_string(object_id) + 
            "    AND " + table_name + ".parent_id" +
            "      = "     + std::to_string(parent_id) + ";";

        assert(e_c->execute(query));

        std::function<void(const DBMeta &)> localDestroyHandler =
            [&e_c, &object, &helper] (const DBMeta &child) {
                helper(child, object);
            };
        object.applyToChildren(localDestroyHandler);
    };

    helper(meta, parent_meta); 
    return true;
}

RewriteOutput::~RewriteOutput()
{;}

bool RewriteOutput::queryAgain(const std::unique_ptr<Connect> &conn)
    const
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

bool RewriteOutput::multipleResultSets() const
{
    return false;
}

bool SimpleOutput::beforeQuery(const std::unique_ptr<Connect> &conn,
                               const std::unique_ptr<Connect> &e_conn)
{
    return true;
}

bool SimpleOutput::getQuery(std::list<std::string> *const queryz,
                            SchemaInfo const &) const
{
    queryz->clear();
    queryz->push_back(original_query);

    return true;
}

bool
SimpleOutput::handleQueryFailure(const std::unique_ptr<Connect> &e_conn)
    const
{
    return true;
}

bool SimpleOutput::afterQuery(const std::unique_ptr<Connect> &e_conn)
    const
{
    return true;
}

bool SimpleOutput::doDecryption() const
{
    return false;
}

bool DMLOutput::beforeQuery(const std::unique_ptr<Connect> &conn,
                            const std::unique_ptr<Connect> &e_conn)
{
    return true;
}

bool DMLOutput::getQuery(std::list<std::string> * const queryz,
                         SchemaInfo const &) const
{
    queryz->clear();
    queryz->push_back(new_query);

    return true;
}

bool DMLOutput::handleQueryFailure(const std::unique_ptr<Connect> &e_conn)
    const
{
    return true;
}

bool DMLOutput::afterQuery(const std::unique_ptr<Connect> &e_conn) const
{
    return true;
}

bool SpecialUpdate::beforeQuery(const std::unique_ptr<Connect> &conn,
                                const std::unique_ptr<Connect> &e_conn)
{
    // Retrieve rows from database.
    const std::string select_q =
        " SELECT * FROM " + this->plain_table +
        " WHERE " + this->where_clause + ";";
    const ResType select_res_type = executeQuery(this->ps, select_q);
    assert(select_res_type.success());
    if (select_res_type.rows.size() == 0) { // No work to be done.
        this->do_nothing = true;
        return true;
    }
    this->do_nothing = false;

    const auto pullItemPtr = [](std::shared_ptr<Item> p) -> const Item &
    {
        return *p;
    };
    const std::function<std::string(std::shared_ptr<Item>)>
        sharedItemToStringWithQuotes =
            fnCompose<std::shared_ptr<Item>, const Item &,
                      std::string>(ItemToStringWithQuotes, pullItemPtr);
    const auto itemJoin =
        [&sharedItemToStringWithQuotes]
            (std::vector<std::shared_ptr<Item> > row) -> std::string
    {
        return "(" +
               vector_join<std::shared_ptr<Item> >(row, ",",
                                        sharedItemToStringWithQuotes) +
               ")";
    };

    const std::string values_string =
        vector_join<std::vector<std::shared_ptr<Item> > >(
                                            select_res_type.rows, ",",
                                            itemJoin);

    // Do the query on the embedded database inside of a transaction
    // so that we can prevent failure artifacts from populating the
    // embedded dabase.
    RETURN_FALSE_IF_FALSE(e_conn->execute("START TRANSACTION;"));

    // Push the plaintext rows to the embedded database.
    const std::string push_q =
        " INSERT INTO " + this->plain_table +
        " VALUES " + values_string + ";";
    ROLLBACK_AND_RFIF(e_conn->execute(push_q), e_conn);

    // Run the original (unmodified) query on the data in the embedded
    // database.
    ROLLBACK_AND_RFIF(e_conn->execute(this->original_query), e_conn);

    // > Collect the results from the embedded database.
    // > This code relies on single threaded access to the database
    //   and on the fact that the database is cleaned up after
    //   every such operation.
    std::unique_ptr<DBResult> dbres;
    const std::string select_results_q =
        " SELECT * FROM " + this->plain_table + ";";
    ROLLBACK_AND_RFIF(e_conn->execute(select_results_q, &dbres), e_conn);
    const ResType interim_res = ResType(dbres->unpack());
    this->output_values =
        vector_join<std::vector<std::shared_ptr<Item> > >(
                                        interim_res.rows, ",",
                                        itemJoin);
    // Cleanup the embedded database.
    const std::string cleanup_q =
        "DELETE FROM " + this->plain_table + ";";
    ROLLBACK_AND_RFIF(e_conn->execute(cleanup_q), e_conn);

    ROLLBACK_AND_RFIF(e_conn->execute("COMMIT;"), e_conn);

    return true;
}

bool SpecialUpdate::getQuery(std::list<std::string> * const queryz,
                             SchemaInfo const &schema) const
{
    assert(queryz);

    queryz->clear();

    if (true == this->do_nothing.get()) {
        queryz->push_back(mysql_noop());
        return true;
    }

    // This query is necessary to propagate a transaction into
    // INFORMATION_SCHEMA.
    queryz->push_back("SELECT NULL FROM " + this->crypted_table + ";");

    // DELETE the rows matching the WHERE clause from the database.
    const std::string delete_q =
        " DELETE FROM " + this->plain_table +
        " WHERE " + this->where_clause + ";";
    const std::string re_delete =
        rewriteAndGetSingleQuery(ps, delete_q, schema);

    // > Add each row from the embedded database to the data database.
    const std::string insert_q =
        " INSERT INTO " + this->plain_table +
        " VALUES " + this->output_values.get() + ";";
    const std::string re_insert =
        rewriteAndGetSingleQuery(ps, insert_q, schema);

    const std::string hom_addition_transaction =
        MetaData::Proc::homAdditionTransaction();
    queryz->push_back(" CALL " + hom_addition_transaction + " ("
                      " '" + escapeString(ps.getConn(), re_delete) + "', "
                      " '" + escapeString(ps.getConn(),
                                          re_insert) + "');");

    return true;
}

bool
SpecialUpdate::handleQueryFailure(const std::unique_ptr<Connect> &e_conn)
    const
{
    return true;
}

bool SpecialUpdate::afterQuery(const std::unique_ptr<Connect> &e_conn)
    const
{
    return true;
}

bool SpecialUpdate::multipleResultSets() const
{
    return true;
}

DeltaOutput::~DeltaOutput()
{;}

bool DeltaOutput::stalesSchema() const
{
    return true;
}
bool
DeltaOutput::beforeQuery(const std::unique_ptr<Connect> &conn,
                         const std::unique_ptr<Connect> &e_conn)
{
    assert(e_conn->execute("START TRANSACTION;"));

    const std::string &q_completion =
        " INSERT INTO " + MetaData::Table::embeddedQueryCompletion() +
        "   (begin, complete, original_query, aborted) VALUES"
        "   (TRUE,  FALSE,"
        "    '" + escapeString(conn, this->original_query) + "', FALSE);";
    e_conn->execute(q_completion);
    this->embedded_completion_id = e_conn->last_insert_id();

    for (auto it = deltas.begin(); it != deltas.end(); it++) {
        const bool b = (*it)->apply(e_conn, Delta::BLEEDING_TABLE);
        ROLLBACK_AND_RFIF(b, e_conn);
    }

    assert(e_conn->execute("COMMIT;"));
    return true;
}

bool
DeltaOutput::afterQuery(const std::unique_ptr<Connect> &e_conn) const
{
    assert(e_conn->execute("START TRANSACTION;"));

    const std::string q_update =
        " UPDATE " + MetaData::Table::embeddedQueryCompletion() +
        "    SET complete = TRUE"
        "  WHERE id=" +
                 std::to_string(this->embedded_completion_id.get()) + ";";
    assert(e_conn->execute(q_update));

    for (auto it = deltas.begin(); it != deltas.end(); it++) {
        const bool b = (*it)->apply(e_conn, Delta::REGULAR_TABLE);
        ROLLBACK_AND_RFIF(b, e_conn);
    }

    assert(e_conn->execute("COMMIT;"));

    return true;
}

unsigned long
DeltaOutput::getEmbeddedCompletionID() const
{
    return this->embedded_completion_id.get();
}

static bool
tableCopy(const std::unique_ptr<Connect> &c, const std::string &src,
          const std::string &dest)
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
setRegularTableToBleedingTable(const std::unique_ptr<Connect> &e_conn)
{
    const std::string src = MetaData::Table::bleedingMetaObject();
    const std::string dest = MetaData::Table::metaObject();
    return tableCopy(e_conn, src, dest);
}

/*
static bool
setBleedingTableToRegularTable(const std::unique_ptr<Connect> &e_conn)
{
    const std::string src = MetaData::Table::metaObject();
    const std::string dest = MetaData::Table::bleedingMetaObject();
    return tableCopy(e_conn, src, dest);
}
*/

/*
static bool
revertAndCleanupEmbedded(const std::unique_ptr<Connect> &e_conn,
                         unsigned long delta_output_id)
{
    assert(e_conn->execute("START TRANSACTION;"));

    TEST_TextMessageError(setBleedingTableToRegularTable(e_conn),
                          "bleedingTable=regularTable failed!");
    TEST_TextMessageError(cleanupDeltaOutputAndQuery(e_conn,
                                                     delta_output_id),
                          "cleaning up delta failed!");

    assert(e_conn->execute("COMMIT;"));

    return true;
}
*/

bool
DDLOutput::getQuery(std::list<std::string> * const queryz,
                    SchemaInfo const &) const
{
    queryz->clear();

    assert(remote_qz().size() == 1);
    const std::string &remote_completion =
        " INSERT INTO " + MetaData::Table::remoteQueryCompletion() +
        "   (complete, embedded_completion_id, reissue) VALUES"
        "   (TRUE, " +
             std::to_string(this->getEmbeddedCompletionID()) +
        "    , FALSE)";
    queryz->push_back(remote_completion);
    queryz->push_back(remote_qz().back());

    return true;
}

bool DDLOutput::handleQueryFailure(const std::unique_ptr<Connect> &e_conn) const
{
    return true;
}

bool
DDLOutput::afterQuery(const std::unique_ptr<Connect> &e_conn) const
{
    // Update embedded database.
    // > This is a DDL query so do not put in transaction.
    assert(e_conn->execute(this->original_query));

    return DeltaOutput::afterQuery(e_conn);
}

const std::list<std::string> DDLOutput::remote_qz() const
{
    return std::list<std::string>({new_query});
}

const std::list<std::string> DDLOutput::local_qz() const
{
    return std::list<std::string>({original_query});
}

bool
AdjustOnionOutput::beforeQuery(const std::unique_ptr<Connect> &conn,
                               const std::unique_ptr<Connect> &e_conn)
{
    assert(deltas.size() > 0);
    return DeltaOutput::beforeQuery(conn, e_conn);
}

bool AdjustOnionOutput::getQuery(std::list<std::string> * const queryz,
                                 SchemaInfo const &)
    const
{
    std::list<std::string> r_qz = remote_qz();
    assert(r_qz.size() == 1 || r_qz.size() == 2);

    if (r_qz.size() == 1) {
        r_qz.push_back(mysql_noop());
    }

    // This query is necessary to propagate a transaction into
    // INFORMATION_SCHEMA.
    // > This allows consistent behavior even when adjustment is first
    // query in transaction.
    const std::string &innodb_table =
        MetaData::Table::remoteQueryCompletion();
    queryz->push_back("SELECT NULL FROM " + innodb_table + ";");

    const std::string q_remote =
        " CALL " + MetaData::Proc::adjustOnion() + " ("
        "   "  + std::to_string(this->getEmbeddedCompletionID()) + ","
        "   '" + hackEscape(r_qz.front()) + "', "
        "   '" + hackEscape(r_qz.back()) + "');";

    queryz->push_back(q_remote);
    return true;
}

// FIXME: If we want the proxy to do something besides dieing when
// a query misfires; implement here.
bool
AdjustOnionOutput::handleQueryFailure(const std::unique_ptr<Connect>
                                         &e_conn)  const
{
    return true;
}

bool
AdjustOnionOutput::afterQuery(const std::unique_ptr<Connect> &e_conn)
    const
{
    assert(deltas.size() > 0);
    return DeltaOutput::afterQuery(e_conn);
}

const std::list<std::string> AdjustOnionOutput::remote_qz() const
{
    return std::list<std::string>(adjust_queries);
}

const std::list<std::string> AdjustOnionOutput::local_qz() const
{
    return std::list<std::string>();
}

bool AdjustOnionOutput::queryAgain(const std::unique_ptr<Connect> &conn)
    const
{
    const std::string q =
        " SELECT reissue "
        "   FROM " + MetaData::Table::remoteQueryCompletion() +
        "  WHERE embedded_completion_id = " +
                 std::to_string(this->getEmbeddedCompletionID()) + ";";

    std::unique_ptr<DBResult> db_res;
    conn->execute(q, &db_res);
    assert(1 == mysql_num_rows(db_res->n));

    MYSQL_ROW row = mysql_fetch_row(db_res->n);
    unsigned long *const l = mysql_fetch_lengths(db_res->n);
    assert(l != NULL);

    return string_to_bool(std::string(row[0], l[0]));
}

// FIXME: Give the user some indication that a ROLLBACK happened.
bool AdjustOnionOutput::doDecryption() const
{
    return false;
}

bool Analysis::addAlias(const std::string &alias,
                        const std::string &db,
                        const std::string &table)
{
    auto db_alias_pair = table_aliases.find(db);
    if (table_aliases.end() == db_alias_pair) {
        table_aliases.insert(
           make_pair(db,
                     std::map<const std::string, const std::string>()));
    }

    std::map<const std::string, const std::string> &
        per_db_table_aliases = table_aliases[db];
    auto alias_pair = per_db_table_aliases.find(alias);
    if (per_db_table_aliases.end() != alias_pair) {
        return false;
    }

    per_db_table_aliases.insert(make_pair(alias, table));
    return true;
}

OnionMeta &Analysis::getOnionMeta(const std::string &db,
                                  const std::string &table,
                                  const std::string &field,
                                  onion o) const
{
    return this->getOnionMeta(this->getFieldMeta(db, table, field), o);
}

OnionMeta &Analysis::getOnionMeta(const FieldMeta &fm,
                                  onion o) const
{
    OnionMeta *const om = fm.getOnionMeta(o);
    TEST_IdentifierNotFound(om, TypeText<onion>::toText(o));

    return *om;
}

FieldMeta &Analysis::getFieldMeta(const std::string &db,
                                  const std::string &table,
                                  const std::string &field) const
{
    FieldMeta * const fm =
        this->getTableMeta(db, table).getChild(IdentityMetaKey(field));
    TEST_IdentifierNotFound(fm, field);

    return *fm;
}

FieldMeta &Analysis::getFieldMeta(const TableMeta &tm,
                                  const std::string &field) const
{
    FieldMeta *const fm = tm.getChild(IdentityMetaKey(field));
    TEST_IdentifierNotFound(fm, field);

    return *fm;
}

TableMeta &Analysis::getTableMeta(const std::string &db,
                                  const std::string &table) const
{
    const DatabaseMeta &dm = this->getDatabaseMeta(db);

    TableMeta *const tm =
        dm.getChild(IdentityMetaKey(unAliasTable(db, table)));
    TEST_IdentifierNotFound(tm, table);

    return *tm;
}

DatabaseMeta &
Analysis::getDatabaseMeta(const std::string &db) const
{
    DatabaseMeta *const dm = this->schema.getChild(IdentityMetaKey(db));
    TEST_IdentifierNotFound(dm, db);

    return *dm;
}

bool Analysis::tableMetaExists(const std::string &db,
                               const std::string &table) const
{
    return this->nonAliasTableMetaExists(db, unAliasTable(db, table));
}

bool Analysis::nonAliasTableMetaExists(const std::string &db,
                                       const std::string &table) const
{
    const DatabaseMeta &dm = this->getDatabaseMeta(db);
    return dm.childExists(IdentityMetaKey(table));
}

std::string Analysis::getAnonTableName(const std::string &db,
                                       const std::string &table) const
{
    if (this->isAlias(db, table)) {
        return table;
    }

    return this->getTableMeta(db, table).getAnonTableName();
}

std::string
Analysis::translateNonAliasPlainToAnonTableName(const std::string &db,
                                                const std::string &table)
    const
{
    TableMeta *const tm =
        this->getDatabaseMeta(db).getChild(IdentityMetaKey(table));
    TEST_IdentifierNotFound(tm, table);

    return tm->getAnonTableName();
}

std::string Analysis::getAnonIndexName(const std::string &db,
                                       const std::string &table,
                                       const std::string &index_name,
                                       onion o)
    const
{
    return this->getTableMeta(db, table).getAnonIndexName(index_name, o);
}

std::string Analysis::getAnonIndexName(const TableMeta &tm,
                                       const std::string &index_name,
                                       onion o)
    const
{
    return tm.getAnonIndexName(index_name, o);
}

bool Analysis::saneDatabaseName() const
{
    return db_name.isSet() && db_name.get().size() > 0;
}

bool Analysis::isAlias(const std::string &db,
                       const std::string &table) const
{
    auto db_alias_pair = table_aliases.find(db);
    if (table_aliases.end() == db_alias_pair) {
        return false;
    }

    return db_alias_pair->second.end() != db_alias_pair->second.find(table);
}

std::string Analysis::unAliasTable(const std::string &db,
                                   const std::string &table) const
{
    auto db_alias_pair = table_aliases.find(db);
    if (table_aliases.end() == db_alias_pair) {
        return table;
    }

    auto alias_pair = db_alias_pair->second.find(table);
    if (db_alias_pair->second.end() == alias_pair) {
        return table;
    }
    
    // We've found an alias!
    return alias_pair->second;
}

EncLayer &Analysis::getBackEncLayer(const OnionMeta &om)
{
    return *om.layers.back().get();
}

SECLEVEL Analysis::getOnionLevel(const OnionMeta &om)
{
    return om.getSecLevel();
}

std::vector<std::unique_ptr<EncLayer>> const &
Analysis::getEncLayers(const OnionMeta &om)
{
    return om.layers;
}

RewritePlanWithAnalysis::RewritePlanWithAnalysis(const EncSet &es_out,
                                                 reason r,
                                            std::unique_ptr<Analysis> a)
    : RewritePlan(es_out, r), a(std::move(a))
{}

