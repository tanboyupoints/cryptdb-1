#include <string>

#include <main/metadata_tables.hh>
#include <main/Connect.hh>

std::string MetaDataTables::Name::delta()
{
    return "DeltaOutput";
}

std::string MetaDataTables::Name::query()
{
    return "Query";
}

std::string MetaDataTables::Name::dmlCompletion()
{
    return "DMLCompletion";
}

std::string MetaDataTables::Name::metaObject()
{
    return "MetaObject";
}

std::string MetaDataTables::Name::bleedingMetaObject()
{
    return "BleedingMetaObject";
}

void MetaDataTables::initialize(Connect *conn, Connect *e_conn)
{
    const std::string create_db =
        " CREATE DATABASE IF NOT EXISTS pdb;";
    assert(e_conn->execute(create_db));

    const std::string create_delta_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::delta() +
        "    (remote_complete BOOLEAN NOT NULL,"
        "     id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    assert(e_conn->execute(create_delta_table));

    const std::string create_query_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::query() +
        "   (query VARCHAR(500) NOT NULL,"
        "    delta_output_id BIGINT NOT NULL,"
        "    local BOOLEAN NOT NULL,"
        "    ddl BOOLEAN NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    assert(e_conn->execute(create_query_table));

    const std::string create_dml_table =
        " CREATE TABLE IF NOT EXISTS " + Name::dmlCompletion() +
        "   (delta_output_id BIGINT NOT NULL,"
        "    id SERIAL)"
        " ENGINE=InnoDB;";
    assert(conn->execute(create_dml_table));

    const std::string create_meta_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::metaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    assert(e_conn->execute(create_meta_table));

    const std::string create_bleeding_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::bleedingMetaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    assert(e_conn->execute(create_bleeding_table));

    return;
}

