// gcc -shared -fpic main.c -o main.so --llua5.1
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <pthread.h>
#include <sys/types.h>

#include <mysql/mysql.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

struct LuaQuery {
    const char *query;      // remove
    void **magic_ptr;
};

void *
do_query(void *lq);

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
    size_t length;
    // FIXME:
    char *const query = calloc(200, sizeof(char));
    strcpy(query, lua_tolstring(L, 1, &length));

    struct LuaQuery *const lua_query = malloc(sizeof(struct LuaQuery));
    if (!lua_query) {
        fprintf(stderr, "malloc failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_query->magic_ptr = malloc(sizeof(void *));
    if (!lua_query->magic_ptr) {
        free(lua_query);
        fprintf(stderr, "malloc failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }
    *lua_query->magic_ptr = NULL;
    lua_query->query = query;

    pthread_t thread;
    // const char *const query = "CREATE TABLE lua.something6(x integer)";
    if (pthread_create(&thread, NULL, do_query, (void *)lua_query)) {
        free(lua_query->magic_ptr);
        free(lua_query);
        fprintf(stderr, "pthread_create failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushboolean(L, true);
    lua_pushnumber(L, (unsigned long)lua_query);
    lua_pushnumber(L, thread);
    return 3;
}

static int
results(lua_State *const L)
{
    struct LuaQuery *const lua_query =
        (struct LuaQuery *)lua_tointeger(L, 1);
    pthread_t thread = (pthread_t)lua_tointeger(L, 2);

    *lua_query->magic_ptr = L;

    int thread_output;
    if (pthread_join(thread, (void **)&thread_output)) {
        free(lua_query->magic_ptr);
        free(lua_query);
        fprintf(stderr, "pthread_join failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    printf("thread out: %d\n", thread_output);
    free(lua_query->magic_ptr);
    free(lua_query);
    return thread_output;
}

static lua_State *
handleMagicPointer(struct LuaQuery *lua_query)
{
    while (NULL == *lua_query->magic_ptr);
    return (lua_State *)*lua_query->magic_ptr;
}

void *
do_query(void *lq)
{
    struct LuaQuery *lua_query = (struct LuaQuery *)lq;
    void **const magic_ptr = lua_query->magic_ptr;
    MYSQL *const init = mysql_init(NULL);
    if (!init) {
        fprintf(stderr, "mysql_init failed!\n");
        goto error_exit;
    }

    const char *const host      = "localhost";
    const char *const user      = "root";
    const char *const passwd    = "letmein";
    const unsigned int port     = 3306;

    MYSQL *const conn =
        mysql_real_connect(init, host, user, passwd, NULL, port, NULL, 0);
    if (!conn) {
        fprintf(stderr, "mysql_real_connect failed!\n");
        goto error_exit;
    }

    if (mysql_query(conn, lua_query->query)) {
        mysql_close(conn);
        fprintf(stderr, "mysql_query failed!\n");
        goto error_exit;
    }

    MYSQL_RES *const result = mysql_store_result(conn);
    const int field_count = mysql_field_count(conn);
    if (NULL == result) {
        if (0 == field_count) {
            assert(0 == mysql_errno(conn));

            lua_State *const ell = handleMagicPointer(lua_query);
            lua_pushboolean(ell, true);
            lua_pushnumber(ell, mysql_affected_rows(conn));
            return (void *)2;
        }

        assert(mysql_errno(conn));
        mysql_close(conn);
        fprintf(stderr, "mysql_store_result failed!\n");
        goto error_exit;
    }
    assert(0 == mysql_errno(conn));

    lua_State *const ell = handleMagicPointer(lua_query);
    // resultset
    lua_pushboolean(ell, true);
    lua_newtable(ell);
    MYSQL_ROW row;
    int row_index = 0;
    while ((row = mysql_fetch_row(result))) {
        unsigned long *const l = mysql_fetch_lengths(result);

        // row
        lua_pushnumber(ell, row_index + 1);
        lua_newtable(ell);
        int col_index = 0;
        for (; col_index < field_count; ++col_index) {
            lua_pushnumber(ell, col_index + 1);
            pushvalue(ell, row[col_index], l[col_index]);
            lua_settable(ell, -3);
        }
        lua_settable(ell, -3);

        row_index += 1;
    }

    return (void *)2;

error_exit:
    fprintf(stderr, "error exit!\n");
    ;
    lua_State *const ell_err = handleMagicPointer(lua_query);

    lua_pushboolean(ell_err, false);
    lua_pushnil(ell_err);

    return (void *)2;
}

static const struct luaL_reg
main_lib[] = {
#define F(n) { #n, n }
    F(pass_table),
    F(query),
    F(results),
    {0, 0},
};

extern int
lua_main_init(lua_State *const L)
{
    luaL_openlib(L, "Main", main_lib, 0);
    return 1;
}
