#include <main/Analysis.hh>

using namespace std;

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
EncSet::EncSet(FieldMeta * fm) {
    osl.clear();
    for (auto pair : fm->children) {
        // FIXME: dynamic_cast
        OnionMeta *om = static_cast<OnionMeta *>(pair.second);
        OnionMetaKey *key = static_cast<OnionMetaKey *>(pair.first);
	osl[key->getValue()] = LevelFieldPair(om->getSecLevel(), fm);
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

ostream&
operator<<(ostream &out, const EncSet & es)
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
            if (it->second.second == 0 && it->second.first != SECLEVEL::PLAINVAL) {
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


ostream&
operator<<(ostream &out, const reason &r)
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

ostream&
operator<<(ostream &out, const RewritePlan * rp)
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
    // HACK: A danger to myself and others, can remove once we have
    // load working.
    MappedDBMeta *keyed_parent_meta =
        static_cast<MappedDBMeta *>(parent_meta);

    switch (action) {
        case CREATE: {
            // FIXME: Remove this once we are doing a Load after DDL
            // queries.
            assert(keyed_parent_meta->addChild(key, meta));

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
            return keyed_parent_meta->replaceChild(key, meta);
            break;
        } case DELETE: {
            /*
             * Recursively delete everything from this top level
             */
            return keyed_parent_meta->destroyChild(key);
            break;
        } default: {
            throw CryptDBError("Unknown Delta::Action!");
        }
    }
}

// TODO: Remove asserts.
// Recursive.
void Delta::createHandler(Connect *e_conn, DBMeta *object,
                          DBMeta *parent, AbstractMetaKey *k)
{
    // Ensure the tables exist.
    DBWriter dbw(object, parent);
    assert(create_tables(e_conn, dbw));
    
    const std::string child_serial = object->serialize(*parent);
    assert(0 == object->getDatabaseID());
    const std::string parent_id =
        std::to_string(parent->getDatabaseID());

    // ------------------------
    //    Build the queries.
    // ------------------------

    // On CREATE, the database generates a unique ID for us.
    std::string query =
        " INSERT INTO pdb." + dbw.table_name() + 
        "    (serial_object) VALUES ("
        " '" + escapeString(e_conn, child_serial) + "'); ";

    assert(e_conn->execute(query));

    const std::string object_id =
        std::to_string(e_conn->last_insert_id());
    std::string serial_key;
    if (NULL == k) {
        AbstractMetaKey *ck = parent->getKey(object);
        assert(ck);
        serial_key = ck->getSerial();
    } else {
        serial_key = k->getSerial();
    }
    std::string join_query =
        " INSERT INTO pdb." + dbw.join_table_name() +
        "   (object_id, parent_id, serial_key) VALUES ("
        " "  + object_id + ", " +
        " "  + parent_id + ", " +
        " '" + escapeString(e_conn, serial_key) + "'); ";

    assert(e_conn->execute(join_query));

    std::function<void(DBMeta *)> localCreateHandler =
        [&e_conn, &object, this] (DBMeta *child) {
            this->createHandler(e_conn, child, object);
        };
    object->applyToChildren(localCreateHandler);
}


bool Analysis::addAlias(std::string alias, std::string table)
{
    auto alias_pair = table_aliases.find(alias);
    if (table_aliases.end() != alias_pair) {
        return false;
    }

    table_aliases[alias] = table;
    return true;
}

FieldMeta *Analysis::getFieldMeta(std::string table, std::string field) const
{
    string real_table_name = unAliasTable(table);
    FieldMeta *fm = ps->schema->getFieldMeta(real_table_name, field);
    assert(fm);
    return fm;
}

TableMeta *Analysis::getTableMeta(std::string table) const
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
                                 
    TableMeta *tm =
        static_cast<TableMeta *>(ps->schema->getChild(key));
    assert(tm);
    return tm;
}

// FIXME: Field aliasing.
bool Analysis::destroyFieldMeta(std::string table, std::string field)
{
    IdentityMetaKey *table_key = new IdentityMetaKey(unAliasTable(table));
    TableMeta *tm = 
        static_cast<TableMeta *>(ps->schema->getChild(table_key));
    if (!tm) {
        return false;
    }

    IdentityMetaKey *field_key = new IdentityMetaKey(field);
    return tm->destroyChild(field_key);
}

bool Analysis::destroyTableMeta(std::string table)
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
    return ps->schema->destroyChild(key);
}

bool Analysis::tableMetaExists(std::string table) const
{
    IdentityMetaKey *key = new IdentityMetaKey(unAliasTable(table));
    return ps->schema->childExists(key);
}

std::string Analysis::getAnonTableName(const string &table) const
{
    return this->getTableMeta(table)->getAnonTableName();
}

std::string Analysis::getAnonIndexName(std::string table,
                                       std::string index_name) const
{
    return this->getTableMeta(table)->getAnonIndexName(index_name); 
}

std::string Analysis::unAliasTable(std::string table) const
{
    auto alias_pair = table_aliases.find(table);
    if (table_aliases.end() != alias_pair) {
        return alias_pair->second;
    } else {
        return table;
    }
}

