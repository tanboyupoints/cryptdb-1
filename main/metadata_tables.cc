#include <string>
#include <memory>

#include <main/metadata_tables.hh>
#include <main/Connect.hh>
#include <main/macro_util.hh>

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

std::string MetaDataTables::Name::transactionHelper()
{
    return "TransactionHelper";
}

bool MetaDataTables::initialize(const std::unique_ptr<Connect> &conn,
                                const std::unique_ptr<Connect> &e_conn)
{
    const std::string create_db =
        " CREATE DATABASE IF NOT EXISTS pdb;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_db));

    const std::string create_delta_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::delta() +
        "    (remote_complete BOOLEAN NOT NULL,"
        "     id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_delta_table));

    const std::string create_query_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::query() +
        "   (query VARCHAR(500) NOT NULL,"
        "    delta_output_id BIGINT NOT NULL,"
        "    local BOOLEAN NOT NULL,"
        "    ddl BOOLEAN NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_query_table));

    const std::string create_dml_table =
        " CREATE TABLE IF NOT EXISTS " + Name::dmlCompletion() +
        "   (delta_output_id BIGINT NOT NULL,"
        "    id SERIAL)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(conn.get()->execute(create_dml_table));

    const std::string create_meta_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::metaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_meta_table));

    const std::string create_bleeding_table =
        " CREATE TABLE IF NOT EXISTS pdb." + Name::bleedingMetaObject() +
        "   (serial_object VARBINARY(500) NOT NULL,"
        "    serial_key VARBINARY(500) NOT NULL,"
        "    parent_id BIGINT NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(e_conn.get()->execute(create_bleeding_table));

    const std::string create_transaction_helper_table =
        " CREATE TABLE IF NOT EXISTS "
        "       cryptdbtest." + Name::transactionHelper() +
        "   (thread_id INTEGER NOT NULL,"
        "    trx_id VARCHAR(20),"
        "    do_commit BOOLEAN NOT NULL,"
        "    id SERIAL PRIMARY KEY)"
        " ENGINE=InnoDB;";
    RETURN_FALSE_IF_FALSE(conn.get()->execute(create_transaction_helper_table));

    return true;
}

