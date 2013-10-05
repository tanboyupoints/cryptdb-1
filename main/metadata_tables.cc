#include <string>
#include <memory>

#include <main/metadata_tables.hh>
#include <main/Connect.hh>
#include <main/macro_util.hh>

std::string MetaDataTables::Name::delta()
{
    return Internal::embeddedDB() + "." + Internal::getPrefix() +
           "DeltaOutput";
}

std::string MetaDataTables::Name::query()
{
    return Internal::embeddedDB() + "." + Internal::getPrefix() +
           "Query";
}

std::string MetaDataTables::Name::dmlCompletion()
{
    return Internal::remoteDB() + "." + Internal::getPrefix() +
           "DMLCompletion";
}

std::string MetaDataTables::Name::metaObject()
{
    return Internal::embeddedDB() + "." + Internal::getPrefix() +
           "MetaObject";
}

std::string MetaDataTables::Name::bleedingMetaObject()
{
    return Internal::embeddedDB() + "." + Internal::getPrefix() +
           "BleedingMetaObject";
}

std::string MetaDataTables::Name::purgatoryDB()
{
    return "purgatory";
}

bool static
hasWhitespace(const std::string &s)
{
    for (auto it : s) {
        if (isspace(it)) {
            return true;
        }
    }

    return false;
}

bool MetaDataTables::initialize(const std::unique_ptr<Connect> &conn,
                                const std::unique_ptr<Connect> &e_conn,
                                const std::string &prefix)
{
    // HACK: prevents multiple initialization
    static bool initialized = false;
    if (initialized) {
        return false;
    }

    // Prefix handling must be done first.
    if (hasWhitespace(prefix)) {
        return false;
    }
    MetaDataTables::Internal::initPrefix(prefix);

    // Embedded database.
    const std::string create_db =
        " CREATE DATABASE IF NOT EXISTS " + Internal::embeddedDB() + ";";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_db));

    const std::string create_delta_table =
        " CREATE TABLE IF NOT EXISTS " + Name::delta() +
        "    (remote_complete BOOLEAN NOT NULL,"
        "     id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_delta_table));

    const std::string create_query_table =
        " CREATE TABLE IF NOT EXISTS " + Name::query() +
        "   (query VARCHAR(500) NOT NULL,"
        "    delta_output_id BIGINT NOT NULL,"
        "    local BOOLEAN NOT NULL,"
        "    ddl BOOLEAN NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_query_table));

    const std::string create_meta_table =
        " CREATE TABLE IF NOT EXISTS " + Name::metaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_meta_table));

    const std::string create_bleeding_table =
        " CREATE TABLE IF NOT EXISTS " + Name::bleedingMetaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_bleeding_table));

    // Remote database.
    const std::string create_remote_db =
        " CREATE DATABASE IF NOT EXISTS " + Internal::remoteDB() + ";";
    RETURN_FALSE_IF_FALSE(conn.get()->execute(create_remote_db));

    const std::string create_dml_table =
        " CREATE TABLE IF NOT EXISTS " + Name::dmlCompletion() +
        "   (delta_output_id BIGINT NOT NULL,"
        "    id SERIAL)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(conn.get()->execute(create_dml_table));

    // Initialize synchronization database.
    const std::string create_purgatory_db =
        " CREATE DATABASE IF NOT EXISTS " + Name::purgatoryDB() + ";";
    RETURN_FALSE_IF_FALSE(conn.get()->execute(create_purgatory_db));
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_purgatory_db));

    initialized = true;
    return true;
}

void
MetaDataTables::Internal::initPrefix(const std::string &s)
{
    lowLevelPrefix(s.c_str());
}

const std::string &
MetaDataTables::Internal::getPrefix()
{
    return lowLevelPrefix(NULL);
}

const std::string &
MetaDataTables::Internal::lowLevelPrefix(const char *const p)
{
    static const std::string prefix = (assert(p), p);
    return prefix;
}

const std::string &MetaDataTables::Internal::embeddedDB()
{
    static const std::string name = "pdb";
    return name;
}

const std::string &MetaDataTables::Internal::remoteDB()
{
    static const std::string name = "remote_db";
    return name;
}
