/*
 * Connect.cpp
 *
 *  Created on: Dec 1, 2010
 *      Author: raluca
 */

#include <stdexcept>
#include <assert.h>
#include <edb/Connect.hh>
#include <util/cryptdb_log.hh>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

Connect::Connect(string server, string user, string passwd,
                 string dbname, uint port)
  : conn( nullptr)
  , close_on_destroy( true )
{
#if MYSQL_S
    const char *dummy_argv[] =
        {
            "progname",
            "--skip-grant-tables",
            "--skip-innodb",
            "--default-storage-engine=MEMORY",
            "--character-set-server=utf8",
            "--language=" MYSQL_BUILD_DIR "/sql/share/"
        };
    assert(0 == mysql_server_init(sizeof(dummy_argv) / sizeof(*dummy_argv),
                                  (char**) dummy_argv, 0));

    conn = mysql_init(NULL);

    /* Make sure we always connect via TCP, and not via Unix domain sockets */
    uint proto = MYSQL_PROTOCOL_TCP;
    mysql_options(conn, MYSQL_OPT_PROTOCOL, &proto);

    /* Connect to the real server even if linked against embedded libmysqld */
    mysql_options(conn, MYSQL_OPT_USE_REMOTE_CONNECTION, 0);

    /* Connect to database */
    if (!mysql_real_connect(conn, server.c_str(), user.c_str(),
                            passwd.c_str(), dbname.c_str(), port, 0, 0)) {
        LOG(warn) << "connecting to server " << server
                  << " user " << user
                  << " pwd " << passwd
                  << " dbname " << dbname
                  << " port " << port;
        LOG(warn) << "mysql_real_connect: " << mysql_error(conn);
        throw runtime_error("cannot connect");
    }

#else /* postgres */
    string conninfo = " dbname = " + dbname;
    conn = PQconnectdb(conninfo.c_str());

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit(1);
    }
#endif

}

Connect * Connect::getEmbedded() {
    MYSQL * m = mysql_init(0);
    assert(m);
    
    mysql_options(m, MYSQL_OPT_USE_EMBEDDED_CONNECTION, 0);
    
    if (!mysql_real_connect(m, 0, 0, 0, 0, 0, 0, CLIENT_MULTI_STATEMENTS)) {
        mysql_close(m);
        cryptdb_err() << "mysql_real_connect: " << mysql_error(m);
    }

    Connect * conn = new Connect(m);
    conn->close_on_destroy = true;

    return conn;
}

bool
Connect::execute(const string &query, DBResult * & res)
{
    //silently ignore empty queries
    if (query.length() == 0) {
        LOG(warn) << "empty query";
        res = 0;
        return true;
    }
    bool success = true;
    if (mysql_query(conn, query.c_str())) {
        LOG(warn) << "mysql_query: " << mysql_error(conn);
        LOG(warn) << "on query: " << query;
        res = 0;
        success = false;
    } else {
        res = DBResult::wrap(mysql_store_result(conn));
    }

    // HACK taken from stephentu:
    // Calling mysql_query seems to have destructive effects
    // on the current_thd. Thus, we must call create_embedded_thd
    // again.
    // TODO: mysql examples for embed server do not need to recreate thd,
    // what are we doing different?
    void* ret = create_embedded_thd(0);
    if (!ret) assert(false);

    
    return success;
}


bool
Connect::execute(const string &query)
{
    DBResult *aux;
    bool r = execute(query, aux);
    if (r)
        delete aux;
    return r;
}

string
Connect::getError()
{
#if MYSQL_S
    return mysql_error(conn);
#else
    return PQerrorMessage(conn);
#endif
}


my_ulonglong
Connect::last_insert_id()
{
    return mysql_insert_id(conn);
}

Connect::~Connect()
{
    if (close_on_destroy) {
#if MYSQL_S
        mysql_close(conn);
#else /*postgres */
        PQfinish(conn);
#endif
    }
}

DBResult::DBResult()
{
}

DBResult *
DBResult::wrap(DBResult_native *n)
{
    DBResult *r = new DBResult();
    r->n = n;
    return r;
}

DBResult::~DBResult()
{
#if MYSQL_S
    mysql_free_result(n);
#else
    PQclear(n);
#endif
}

static Item *
getItem(char * content, enum_field_types type, uint len) {
    if (content == NULL) {
	return new Item_null();
    }
    Item * i;
    if (IsMySQLTypeNumeric(type)) {
	ulonglong val = valFromStr(string(content, len));
	i = new Item_int(val);
    } else {
	i = new Item_string(content, len, &my_charset_bin);
    }
    
    return i;
}

// returns the data in the last server response
// TODO: to optimize return pointer to avoid overcopying large result sets?
ResType
DBResult::unpack()
{
#if MYSQL_S
    if (n == NULL)
        return ResType();

    size_t rows = (size_t)mysql_num_rows(n);
    int cols  = -1;
    if (rows > 0) {
        cols = mysql_num_fields(n);
    } else {
        return ResType();
    }

    ResType res;

    for (int j = 0;; j++) {
        MYSQL_FIELD *field = mysql_fetch_field(n);
        if (!field)
            break;

        res.names.push_back(field->name);
        res.types.push_back(field->type);
    }

    for (int index = 0;; index++) {
        MYSQL_ROW row = mysql_fetch_row(n);
        if (!row)
            break;
        unsigned long *lengths = mysql_fetch_lengths(n);

        vector<Item *> resrow;

        for (int j = 0; j < cols; j++) {
            Item * it = getItem(row[j], res.types[j], lengths[j]);
	    resrow.push_back(it);
        }

        res.rows.push_back(resrow);
    }

    return res;

#else /* postgres */

    unsigned int cols = PQnfields(n);
    unsigned int rows = PQntuples(n);

    ResType *res = new vector<vector<string> >[rows+1];

    // first, fill up first row with names
    (*res)[0] = new vector<string>[cols];
    for (uint i = 0; i < cols; i++)
        (*res)[0][i] = string(PQfname(dbAnswer, i));

    // fill up values
    for (uint i = 0; i < rows; i++)
        for (uint j = 0; j < cols; j++)
            (*res)[i+1][j] = string(PQgetvalue(n, i, j));

    return res;
#endif

}

