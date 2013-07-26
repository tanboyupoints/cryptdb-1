#include <main/Analysis.hh>
#include <main/rewrite_util.hh>
#include <main/rewrite_main.hh>

// FIXME: Memory leaks when we allocate MetaKey<...>, use smart pointer.

inline std::string
escapeString(Connect *e_conn, std::string escape_me)
{
    unsigned int escaped_length = escape_me.size() * 2 + 1;
    char *escaped = new char[escaped_length];
    e_conn->real_escape_string(escaped, escape_me.c_str(),
                               escape_me.size());

    std::string out = std::string(escaped);
    delete[] escaped;

    return out;
}

EncSet::EncSet() : osl(FULL_EncSet.osl) {}

// FIXME: Wrong interfaces.
EncSet::EncSet(Analysis &a, FieldMeta * fm) {
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

        FieldMeta *fm = it->second.second;
        FieldMeta *fm2 = it2->second.second;

        if (it != osl.end()) {
            SECLEVEL sl = (SECLEVEL)min((int)it->second.first,
                    (int)it2->second.first);
            onion o = it->first;

            if (fm == NULL) {
                m[o] = LevelFieldPair(sl, fm2);
            } else if (fm2 == NULL) {
                m[it->first] = LevelFieldPair(sl, fm);
            } else if (fm != NULL && fm2 != NULL) {
                // TODO(burrows): Guarentee that the keys are the same.
                if (sl == SECLEVEL::DETJOIN) {
                    m[o] = LevelFieldPair(sl, fm);
                } else if (sl == SECLEVEL::HOM) {
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
            << ", level " << levelnames[(int)it.second.first]
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
        oPLAIN,
    };

    static size_t onion_size = sizeof(onion_order) / sizeof(onion_order[0]);
    for (size_t i = 0; i < onion_size; i++) {
        onion o = onion_order[i];
        auto it = osl.find(o);
        if (it != osl.end()) {
            if (it->second.second == 0 &&
                it->second.first != SECLEVEL::PLAINVAL) {
                /*
                 * If no key, skip this OLK.
                 * What are the semantics of chooseOne() anyway?
                 */
                continue;
            }

            return OLK(o,  it->second.first, it->second.second);
        }
    }
    return OLK();
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

// FIXME: Implement serial number.
// > key default argument is NULL
bool Delta::singleSave(Connect *e_conn, const DBMeta * const object,
                       const DBMeta * const parent,
                       const AbstractMetaKey * const key)
{
    const std::string serial_object = object->serialize(*parent);
    const std::string parent_id =
        std::to_string(parent->getDatabaseID());
    std::string serial_key;
    if (NULL == key) {
        serial_key = parent->getKey(object)->getSerial();
    } else {
        serial_key = key->getSerial();
    }
    const std::string table_name = "Delta";

    // Make sure the table exists.
    const std::string create_table_query =
        " CREATE TABLE IF NOT EXISTS pdb." + table_name +
        "   (serial_object VARBINARY(200) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    serial_key VARBINARY(200) NOT NULL)"
        " ENGINE=InnoDB;";
    assert(e_conn->execute(create_table_query));

    // TODO: Maybe we want to join on object_id as well.
    const std::string esc_serial_object =
        escapeString(e_conn, serial_object);
    const std::string esc_serial_key =
        escapeString(e_conn, serial_key);
    const std::string query =
        " INSERT INTO pdb." + table_name +
        "    (serial_object, parent_id, serial_key) VALUES (" +
        " '" + esc_serial_object + "', " +
        " "  + parent_id + ", " +
        " '" + esc_serial_key + "');";

    assert(e_conn->execute(query));

    return true;
}

bool Delta::singleDestroy(Connect *e_conn, const DBMeta * const object,
                          const DBMeta * const parent,
                          const AbstractMetaKey * const key)
{
    const std::string serial_object = object->serialize(*parent);
    const std::string parent_id =
        std::to_string(parent->getDatabaseID());
    std::string serial_key;
    if (NULL == key) {
        serial_key = parent->getKey(object)->getSerial();
    } else {
        serial_key = key->getSerial();
    }
    const std::string table_name = "Delta";

    const std::string query =
        " DELETE pdb." + table_name +
        "   FROM pdb." + table_name +
        "  WHERE pdb." + table_name + ".serial_object" +
        "      = '" + serial_object + "' "
        "    AND pdb." + table_name + ".parent_id" +
        "      = '" + parent_id + "' "
        "    AND pdb." + table_name + ".serial_key" +
        "      = '" + serial_key + "';";

    return e_conn->execute(query);
}

// FIXME: Duplicates code with CreateDelta::destroyRecord
bool CreateDelta::save(Connect *e_conn)
{
    // Must provide the key because we have not yet associated the child
    // with the parent.
    assert(Delta::singleSave(e_conn, meta, parent_meta, key));
    assert(saveNewChildrenRecords(e_conn));
    
    return true;
}

bool CreateDelta::saveNewChildrenRecords(Connect *e_conn)
{
    // FIXME: This should use the recursive application in
    // DeleteDelta::apply
    return false;
}

// TODO: Remove asserts.
// Recursive.
bool CreateDelta::apply(Connect *e_conn)
{
    static std::function<void(Connect *e_conn, const DBMeta * const,
                              const DBMeta * const,
                              const AbstractMetaKey * const,
                              const unsigned int * const)> helper =
        [&helper] (Connect *e_conn, const DBMeta * const object,
                   const DBMeta * const parent,
                   const AbstractMetaKey * const k,
                   const unsigned int * const ptr_parent_id)
    {
        DBWriter dbw(object, parent);
        
        // Ensure the tables exist.
        assert(create_tables(e_conn, dbw));
        
        const std::string child_serial = object->serialize(*parent);
        assert(0 == object->getDatabaseID());
        unsigned int parent_id;
        if (ptr_parent_id) {
            parent_id = *ptr_parent_id;
        } else {
            parent_id = parent->getDatabaseID();
        }

        // ------------------------
        //    Build the queries.
        // ------------------------

        // On CREATE, the database generates a unique ID for us.
        const std::string esc_child_serial =
            escapeString(e_conn, child_serial);
        const unsigned int esc_child_serial_len = esc_child_serial.size();
        const std::string query =
            " INSERT INTO pdb." + dbw.table_name() + 
            "    (serial_object, serial_object_len) VALUES (" 
            " '" + esc_child_serial + "', "
            " "  + std::to_string(esc_child_serial_len) + "); ";

        assert(e_conn->execute(query));

        const unsigned int object_id = e_conn->last_insert_id();
        std::string serial_key;
        if (NULL == k) {
            AbstractMetaKey *ck = parent->getKey(object);
            assert(ck);
            serial_key = ck->getSerial();
        } else {
            serial_key = k->getSerial();
        }
        const std::string esc_serial_key = escapeString(e_conn, serial_key);
        const unsigned int esc_serial_key_len = 
            esc_serial_key.size();
        const std::string join_query =
            " INSERT INTO pdb." + dbw.join_table_name() +
            "  (object_id, parent_id, serial_key, serial_key_len) VALUES ("
            " "  + std::to_string(object_id) + ", " 
            " "  + std::to_string(parent_id) + ", "
            " '" + esc_serial_key + "', " +
            " "  + std::to_string(esc_serial_key_len) + ");";

        assert(e_conn->execute(join_query));

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

bool CreateDelta::destroyRecord(Connect *e_conn)
{
    // Must provide key.
    assert(Delta::singleDestroy(e_conn, meta, parent_meta, key));
    assert(destroyNewChildrenRecords(e_conn));
    
    return true;
}

bool CreateDelta::destroyNewChildrenRecords(Connect *e_conn)
{
    // FIXME: This should use the recursive application in
    // CreateDelta::apply
    // > Create orphan tables.
    return false;
}

bool ReplaceDelta::save(Connect *e_conn)
{
    return Delta::singleSave(e_conn, meta, parent_meta);
}

bool ReplaceDelta::apply(Connect *e_conn)
{
    DBWriter dbw(meta, parent_meta);

    const std::string child_serial = meta->serialize(*parent_meta);
    const unsigned int child_id = meta->getDatabaseID();
    const unsigned int parent_id = parent_meta->getDatabaseID();
    
    const std::string esc_child_serial =
        escapeString(e_conn, child_serial);
    const unsigned int esc_child_serial_len = esc_child_serial.size();
    const std::string query = 
        " UPDATE pdb." + dbw.table_name() +
        "    SET serial_object='" + esc_child_serial + "', "
        "        serial_object_len=" + std::to_string(esc_child_serial_len) +
        "  WHERE id=" + std::to_string(child_id) + ";";

    assert(e_conn->execute(query));

    const std::string serial_key = key->getSerial();
    const std::string esc_serial_key = escapeString(e_conn, serial_key);
    const unsigned int esc_serial_key_len = esc_serial_key.size();
    const std::string join_query =
        " UPDATE pdb." + dbw.join_table_name() +
        "    SET serial_key='" + esc_serial_key + "', "
        "        serial_key_len=" + std::to_string(esc_serial_key_len) +
        "  WHERE object_id=" + std::to_string(child_id) +
        "    AND parent_id=" + std::to_string(parent_id) + "; ";
    
    assert(e_conn->execute(join_query)); 

    return true;
}

bool ReplaceDelta::destroyRecord(Connect *e_conn)
{
    return Delta::singleDestroy(e_conn, meta, parent_meta);
}

bool DeleteDelta::save(Connect *e_conn)
{
    return Delta::singleSave(e_conn, meta, parent_meta);
}

bool DeleteDelta::apply(Connect *e_conn)
{
    std::function<void(Connect *, const DBMeta * const,
                       const DBMeta * const)> helper =
        [&helper](Connect *e_conn, const DBMeta * const object,
                  const DBMeta * const parent)
    {
        DBWriter dbw(object, parent);
        const unsigned int object_id = object->getDatabaseID();
        const unsigned int parent_id = parent->getDatabaseID();

        const std::string query =
            " DELETE pdb." + dbw.table_name() + ", "
            "        pdb." + dbw.join_table_name() +
            "   FROM pdb." + dbw.table_name() + 
            " INNER JOIN pdb." + dbw.join_table_name() +
            "  WHERE pdb." + dbw.table_name() + ".id" +
            "      = pdb." + dbw.join_table_name() + ".object_id" +
            "    AND pdb." + dbw.table_name() + ".id" +
            "      = "     + std::to_string(object_id) + 
            "    AND pdb." + dbw.join_table_name() + ".parent_id" +
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

bool DeleteDelta::destroyRecord(Connect *e_conn)
{
    return Delta::singleDestroy(e_conn, meta, parent_meta);
}

RewriteOutput::~RewriteOutput()
{;}

static void
prettyPrintQuery(std::string query)
{
    std::cout << std::endl << RED_BEGIN
              << "QUERY: " << COLOR_END << query << std::endl;
}

ResType *RewriteOutput::doQuery(Connect *conn, Connect *e_conn,
                                Rewriter *rewriter)
{
    if (false == queryAgain()) {
        prettyPrintQuery(query_from_lex);
        return sendQuery(conn, query_from_lex);
    }
    return NULL;
}

bool RewriteOutput::queryAgain()
{
    return false;
}

std::string RewriteOutput::getQuery(LEX *lex)
{
    if (NULL == lex) {
        return std::string("");
    } else {
        std::ostringstream ss;
        ss << *lex;
        return ss.str();
    }
}

ResType *RewriteOutput::sendQuery(Connect *c, std::string q)
{
    DBResult *dbres;
    assert(c->execute(q, dbres));
    ResType *res = new ResType(dbres->unpack());
    if (!res->ok) {
        throw CryptDBError("Failed to unpack query results!");
    }

    return res;
}

ResType *DMLOutput::doQuery(Connect *conn, Connect *e_conn,
                            Rewriter *rewriter)
{
    return RewriteOutput::doQuery(conn, e_conn, rewriter);
}

SpecialUpdate::SpecialUpdate(const std::string &original_query,
                             LEX *lex, const ProxyState &ps)
    : RewriteOutput(original_query, lex), ps(ps)
{
    this->plain_table =
        lex->select_lex.top_join_list.head()->table_name; 
    if (lex->select_lex.where) {
        std::ostringstream where_stream;
        where_stream << " " << *lex->select_lex.where << " ";
        this->where_clause = where_stream.str();
    } else {    // HACK: Handle empty WHERE clause.
        this->where_clause = " TRUE ";
    }
}

// FIXME: Implement locking.
ResType *SpecialUpdate::doQuery(Connect *conn, Connect *e_conn,
                                Rewriter *rewriter)
{
    assert(rewriter);
    // Retrieve rows from database.
    std::ostringstream select_stream;
    select_stream << " SELECT * FROM " << this->plain_table
                  << " WHERE " << this->where_clause << ";";
    const ResType * const select_res_type =
        executeQuery(*rewriter, this->ps, select_stream.str());
    assert(select_res_type);
    if (select_res_type->rows.size() == 0) { // No work to be done.
        return RewriteOutput::doQuery(conn, e_conn, rewriter);
    }

    const auto itemJoin = [](std::vector<Item*> row) {
        return "(" + vector_join<Item*>(row, ",", ItemToString) + ")";
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

    // FIXME(burrows): Use general join.
    ScopedMySQLRes r(dbres->n);
    MYSQL_ROW row;
    std::string output_rows = " ";
    const unsigned long field_count = r.res()->field_count;
    while ((row = mysql_fetch_row(r.res()))) {
        unsigned long *l = mysql_fetch_lengths(r.res());
        assert(l != NULL);

        output_rows.append(" ( ");
        for (unsigned long field_index = 0; field_index < field_count;
             ++field_index) {
            std::string field_data;
            if (row[field_index]) {
                field_data = std::string(row[field_index],
                                         l[field_index]);
            } else {    // Handle NULL values.
                field_data = std::string("NULL");
            }
            output_rows.append(field_data);
            if (field_index + 1 < field_count) {
                output_rows.append(", ");
            }
        }
        output_rows.append(" ) ,");
    }
    output_rows = output_rows.substr(0, output_rows.length() - 1);

    // Cleanup the embedded database.
    std::ostringstream cleanup_stream;
    cleanup_stream << "DELETE FROM " << this->plain_table << ";";
    assert(e_conn->execute(cleanup_stream.str()));

    // DELETE the rows matching the WHERE clause from the database.
    std::ostringstream delete_stream;
    delete_stream << " DELETE FROM " << this->plain_table
                  << " WHERE " << this->where_clause << ";";
    assert(executeQuery(*rewriter, this->ps, delete_stream.str()));

    // > Add each row from the embedded database to the data database.
    std::ostringstream push_results_stream;
    push_results_stream << " INSERT INTO " << this->plain_table
                        << " VALUES " << output_rows << ";";
    return executeQuery(*rewriter, this->ps, push_results_stream.str());
}

DeltaOutput::~DeltaOutput()
{;}

// FIXME: Implement.
static bool saveQuery(Connect *e_conn, std::string query)
{
    return true;
}

// FIXME: Implement.
static bool destroyQueryRecord(Connect *e_conn, std::string query)
{
    return true;
}

// FIXME: Test DB by reading out of it for later ops.
ResType *DeltaOutput::doQueryHelper(Connect *conn, Connect *e_conn,
                                    Rewriter *rewriter, bool do_original,
                                    std::function<ResType *()> primary)
{
    // Store deltas and original query in embedded db.
    {
        assert(e_conn->execute("START TRANSACTION;"));
        for (auto it : deltas) {
            assert(it->save(e_conn));
        }
        if (true == do_original) {
            assert(saveQuery(e_conn, original_query));
        }
        assert(e_conn->execute("COMMIT;"));
    }

    // Execute query @ remote.
    ResType *result = primary();

    // > Apply deltas and original query.
    // > Remove deltas and original query from embedded db.
    {
        assert(e_conn->execute("START TRANSACTION;"));
        for (auto it : deltas) {
            assert(it->apply(e_conn));
            assert(it->destroyRecord(e_conn));
        }
        if (true == do_original) {
            assert(e_conn->execute(original_query));
            destroyQueryRecord(e_conn, original_query);
        }
        assert(e_conn->execute("COMMIT;"));
    }

    return result;
}

ResType *DDLOutput::doQuery(Connect *conn, Connect *e_conn,
                            Rewriter *rewriter)
{
    std::function<ResType *()> primary =
        [this, &conn, &e_conn, &rewriter]()
        {
            return RewriteOutput::doQuery(conn, e_conn, rewriter);
        };
    return DeltaOutput::doQueryHelper(conn, e_conn, rewriter, true,
                                      primary);
}

bool AdjustOnionOutput::queryAgain()
{
    return true;
}

ResType *AdjustOnionOutput::doQuery(Connect *conn, Connect *e_conn,
                                    Rewriter *rewriter)
{
    std::function<ResType *()> primary =
        [this, &conn]() -> ResType*
        {
            for (auto it : adjust_queries) {
                assert(conn->execute(it));
            }
            return NULL;
        };
    return DeltaOutput::doQueryHelper(conn, e_conn, rewriter, false,
                                      primary);
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

// FIXME: Field aliasing.
FieldMeta *Analysis::getFieldMeta(const std::string &table,
                                  const std::string &field) const
{
    std::string real_table_name = unAliasTable(table);
    FieldMeta *fm = this->schema->getFieldMeta(real_table_name, field);
    assert(fm);
    return fm;
}

TableMeta *Analysis::getTableMeta(const std::string &table) const
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
                                 
    TableMeta *tm = this->schema->getChild(key);
    assert(tm);
    return tm;
}

bool Analysis::tableMetaExists(const std::string &table) const
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
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
        EncLayer *out_layer = to_adjust_enc_layers[om].back();
        to_adjust_enc_layers[om].pop_back();
        return out_layer;
    } else { // Second onion adjustment for this query.
        // FIXME: Maybe we want to support this case.
        throw CryptDBError("Trying to adjust onion twice in same round!");
    }
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
