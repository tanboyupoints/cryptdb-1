// gcc -shared -fpic main.c -o main.so --llua5.1
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <pthread.h>

#include <mysql/mysql.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

int
do_query(lua_State *const L, const char *const query);

static int
pass_table(lua_State *const L)
{
    lua_newtable(L);

    lua_pushstring(L, "key0");
    lua_pushstring(L, "value0");
    lua_settable(L, -3);

    lua_pushstring(L, "key1");
    lua_pushstring(L, "value1");
    lua_settable(L, -3);

    return 1;
}

// taken from luasql-mysql
static void
pushvalue(lua_State *const L, void *const row, long int len)
{
    if (row == NULL) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, row, len);
    }
}

static int
query(lua_State *const L)
{
    // FIXME: Get real query.
    const char *const query = "SELECT * FROM lua.t";
    // const char *const query = "CREATE TABLE lua.something6(x integer)";
    return do_query(L, query);
}

int
do_query(lua_State *const L, const char *const query)
{
    MYSQL *const init = mysql_init(NULL);
    if (!init) {
        fprintf(stderr, "mysql_init failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    const char *const host      = "localhost";
    const char *const user      = "root";
    const char *const passwd    = "letmein";
    const unsigned int port     = 3306;

    MYSQL *const conn =
        mysql_real_connect(init, host, user, passwd, NULL, port, NULL, 0);
    if (!conn) {
        fprintf(stderr, "mysql_real_connect failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    if (mysql_query(conn, query)) {
        mysql_close(conn);
        fprintf(stderr, "mysql_query failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    MYSQL_RES *const result = mysql_store_result(conn);
    const int field_count = mysql_field_count(conn);
    if (NULL == result) {
        if (0 == field_count) {
            assert(0 == mysql_errno(conn));

            lua_pushboolean(L, true);
            lua_pushnumber(L, mysql_affected_rows(conn));
            return 2;
        }

        assert(mysql_errno(conn));
        fprintf(stderr, "mysql_store_result failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }
    assert(0 == mysql_errno(conn));

    // resultset
    lua_pushboolean(L, true);
    lua_newtable(L);

    MYSQL_ROW row;
    int row_index = 0;
    while ((row = mysql_fetch_row(result))) {
        unsigned long *const l = mysql_fetch_lengths(result);

        // row
        lua_pushnumber(L, row_index + 1);
        lua_newtable(L);
        int col_index = 0;
        for (; col_index < field_count; ++col_index) {
            lua_pushnumber(L, col_index + 1);
            pushvalue(L, row[col_index], l[col_index]);
            lua_settable(L, -3);
        }
        lua_settable(L, -3);

        row_index += 1;
    }

    return 2;
}

static const struct luaL_reg
main_lib[] = {
#define F(n) { #n, n }
    F(pass_table),
    F(query),
    {0, 0},
};

extern int
lua_main_init(lua_State *const L)
{
    luaL_openlib(L, "Main", main_lib, 0);
    return 1;
}
