#include <main/Analysis.hh>
#include <main/rewrite_util.hh>

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

bool Delta::save(Connect *e_conn)
{
    std::string serial_object = meta->serialize(*parent_meta);
    std::string parent_id =
        std::to_string(parent_meta->getDatabaseID());
    std::string serial_key = key->getSerial();
    std::string table_name = "Delta";

    // TODO: Maybe we want to join on object_id as well.
    std::string query =
        " INSERT INTO pdb." + table_name +
        "    (action, serial_object, parent_id, serial_key) VALUES (" +
        " "  + std::to_string(action) + ", " +
        " '" + serial_object + "', " +
        " "  + parent_id + ", " +
        " '" + serial_key + "');";

    return e_conn->execute(query);
}

bool Delta::apply(Connect *e_conn) 
{
    switch (action) {
        case CREATE: {
            /*
             * We know that the Delta object is the top level in
             * a hierarchy of all new objects. Therefore we must
             * go through and recursively associate them all with their
             * parents.
             */
            createHandler(e_conn, meta, parent_meta, key);
            return true;
            break;
        } case REPLACE: {
            /*
             * Replace the particular meta.
             */
            replaceHandler(e_conn, meta, parent_meta, key);
            return true;
        } case DELETE: {
            /*
             * Recursively delete everything from this top level
             */
            deleteHandler(e_conn, meta, parent_meta);
            return true;
        } default: {
            throw CryptDBError("Unknown Delta::Action!");
        }
    }
}

// TODO: Remove asserts.
// Recursive.
void Delta::createHandler(Connect *e_conn, const DBMeta * const object,
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
        "   (object_id, parent_id, serial_key, serial_key_len) VALUES ("
        " "  + std::to_string(object_id) + ", " 
        " "  + std::to_string(parent_id) + ", "
        " '" + esc_serial_key + "', " +
        " "  + std::to_string(esc_serial_key_len) + ");";

    assert(e_conn->execute(join_query));

    std::function<void(const DBMeta * const)> localCreateHandler =
        [&e_conn, &object, object_id, this] (const DBMeta * const child) {
            this->createHandler(e_conn, child, object, NULL, &object_id);
        };
    object->applyToChildren(localCreateHandler);
}

void Delta::deleteHandler(Connect *e_conn, const DBMeta * const object,
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
        [&e_conn, &object, this] (const DBMeta * const child) {
            this->deleteHandler(e_conn, child, object);
        };
    object->applyToChildren(localDestroyHandler);
}

void Delta::replaceHandler(Connect *e_conn, const DBMeta * const object,
                           const DBMeta * const parent,
                           const AbstractMetaKey * const k)
{
    DBWriter dbw(object, parent);

    const std::string child_serial = object->serialize(*parent);
    const unsigned int child_id = object->getDatabaseID();
    const unsigned int parent_id = parent->getDatabaseID();
    
    const std::string esc_child_serial =
        escapeString(e_conn, child_serial);
    const unsigned int esc_child_serial_len = esc_child_serial.size();
    const std::string query = 
        " UPDATE pdb." + dbw.table_name() +
        "    SET serial_object='" + esc_child_serial + "', "
        "        serial_object_len=" + std::to_string(esc_child_serial_len) +
        "  WHERE id=" + std::to_string(child_id) + ";";

    assert(e_conn->execute(query));

    const std::string serial_key = k->getSerial();
    const std::string esc_serial_key = escapeString(e_conn, serial_key);
    const unsigned int esc_serial_key_len = esc_serial_key.size();
    const std::string join_query =
        " UPDATE pdb." + dbw.join_table_name() +
        "    SET serial_key='" + esc_serial_key + "', "
        "        serial_key_len=" + std::to_string(esc_serial_key_len) +
        "  WHERE object_id=" + std::to_string(child_id) +
        "    AND parent_id=" + std::to_string(parent_id) + "; ";
    
    assert(e_conn->execute(join_query)); 

    return;
}

RewriteOutput::~RewriteOutput()
{;}

static void
prettyPrintQuery(std::string query)
{
    std::cout << std::endl << RED_BEGIN
              << "QUERY: " << COLOR_END << query << std::endl;
}

// FIXME: Implement.
DBResult *RewriteOutput::doQuery(Connect *conn, Connect *e_conn)
{
    if (false == queryAgain()) {
        DBResult *dbres;
        prettyPrintQuery(new_query);

        assert(conn->execute(new_query, dbres));

        return dbres;
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

DBResult *DMLOutput::doQuery(Connect *conn, Connect *e_conn)
{
    return RewriteOutput::doQuery(conn, e_conn);
}

// FIXME: Implement.
DBResult *SpecialUpdate::doQuery(Connect *conn, Connect *e_conn)
{
    return NULL;
}

DeltaOutput::~DeltaOutput()
{;}

DBResult *DeltaOutput::doQuery(Connect *conn, Connect *e_conn)
{
    for (auto it : deltas) {
        assert(it.apply(e_conn));
    }

    return RewriteOutput::doQuery(conn, e_conn);
}

DBResult *DDLOutput::doQuery(Connect *conn, Connect *e_conn)
{
    assert(e_conn->execute(original_query));
    return DeltaOutput::doQuery(conn, e_conn);
}

bool AdjustOnionOutput::queryAgain()
{
    return true;
}

DBResult *AdjustOnionOutput::doQuery(Connect *conn, Connect *e_conn)
{
    for (auto it : adjust_queries) {
        assert(conn->execute(it));
    }
    return DeltaOutput::doQuery(conn, e_conn);
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

// FIXME: Field aliasing.
bool Analysis::destroyFieldMeta(const std::string &table,
                                const std::string &field)
{
    IdentityMetaKey *table_key = new IdentityMetaKey(unAliasTable(table));
    TableMeta *tm = this->schema->getChild(table_key);
    if (!tm) {
        return false;
    }

    IdentityMetaKey *field_key = new IdentityMetaKey(field);
    return tm->destroyChild(field_key);
}

bool Analysis::destroyTableMeta(const std::string &table)
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
    return this->getSchema()->destroyChild(key);
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
