// gcc -shared -fpic main.c -o main.so --llua5.1
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <sys/types.h>

#include <mysql/mysql.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

enum Command {QUERY, KILL};

struct LuaQuery {
    enum Command command;
    bool command_ready;
    bool completion_signal;
    void **magic_ptr;

    const char *query;
    unsigned int output_count;
};

static struct LuaQuery *
createLuaQuery(const char *const query);

static void
destroyLuaQuery(struct LuaQuery **lua_query);

void
issueCommand(lua_State *L, enum Command command,
             struct LuaQuery *const lua_query, const pthread_t thread,
             unsigned int wait);

void *
commandHandler(void *const lq);

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
    // NOTE: Will not handle embedded NUL characters.
    size_t length;
    struct LuaQuery *lua_query =
        createLuaQuery(lua_tolstring(L, 1, &length));
    if (!lua_query) {
        fprintf(stderr, "createLuaQuery failed!\n");
        goto error_exit;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, commandHandler, (void *)lua_query)) {
        fprintf(stderr, "pthread_create failed!\n");
        goto error_exit;
    }

    lua_pushboolean(L, true);
    lua_pushnumber(L, (unsigned long)lua_query);
    lua_pushnumber(L, thread);
    return 3;

error_exit:
    if (lua_query) {
        destroyLuaQuery(&lua_query);
    }
    lua_pushboolean(L, false);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

static int
results(lua_State *const L)
{
    struct LuaQuery *lua_query =
        (struct LuaQuery *)lua_tointeger(L, 1);
    const pthread_t thread = (pthread_t)lua_tointeger(L, 2);

    issueCommand(L, QUERY, lua_query, thread, 1);
    return lua_query->output_count;
}

static int
kill(lua_State *const L)
{
    struct LuaQuery *lua_query =
        (struct LuaQuery *)lua_tointeger(L, 1);
    const pthread_t thread = (pthread_t)lua_tointeger(L, 2);

    issueCommand(L, KILL, lua_query, thread, 1);

    const int output_count = lua_query->output_count;
    destroyLuaQuery(&lua_query);
    return output_count;
}

static const struct luaL_reg
main_lib[] = {
#define F(n) { #n, n }
    F(pass_table),
    F(query),
    F(results),
    F(kill),
    {0, 0},
};

extern int
lua_main_init(lua_State *const L)
{
    luaL_openlib(L, "Main", main_lib, 0);
    return 1;
}


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                  Helpers
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

void
issueCommand(lua_State *const L, enum Command command,
             struct LuaQuery *lua_query, const pthread_t thread,
             unsigned int wait)
{
    assert(false == lua_query->completion_signal);

    lua_query->command       = command;
    *lua_query->magic_ptr    = L;
    lua_query->command_ready = true;

    const time_t test = time(NULL) + wait;
    while (false == lua_query->completion_signal) {
        if (difftime(test, time(NULL)) < 0) {
            break;
        }
    }

    if (false == lua_query->completion_signal) {
        // bad state because the thread might signal completion after
        // we reset completion
        fprintf(stderr, "thread did not self kill successfully!\n");
        goto panic_exit;
    }
    lua_query->completion_signal = false;

    assert(false == lua_query->command_ready);
    return;

panic_exit:
    // we don't know what happened to the lua state
    fprintf(stderr, "panicing!\n");
    destroyLuaQuery(&lua_query);
    exit(1);
}

static lua_State *
handleMagicPointer(struct LuaQuery *lua_query)
{
    while (false == lua_query->command_ready);
    assert(*lua_query->magic_ptr);
    return (lua_State *)*lua_query->magic_ptr;
}

void
completeLuaQuery(struct LuaQuery *lua_query, unsigned output_count)
{
    assert(true == lua_query->command_ready &&
           false == lua_query->completion_signal);

    lua_query->command_ready     = false;
    *lua_query->magic_ptr        = NULL;
    lua_query->output_count      = output_count;
    lua_query->completion_signal = true;
}

void *
commandHandler(void *const lq)
{
    struct LuaQuery *lua_query = (struct LuaQuery *)lq;
    // do we need to do this for each thread?
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

    while (true) {
        // blocking
        lua_State *const ell = handleMagicPointer(lua_query);

        assert(false == lua_query->completion_signal);
        if (KILL == lua_query->command) {
            lua_pushboolean(ell, true);
            completeLuaQuery(lua_query, 1);
            return (void *)0;
        }

        assert(QUERY == lua_query->command);

        MYSQL_RES *const result = mysql_store_result(conn);
        const int field_count = mysql_field_count(conn);
        if (NULL == result) {
            if (0 != field_count) {
                // query failed
                assert(mysql_errno(conn));
                mysql_close(conn);
                fprintf(stderr, "mysql_store_result failed!\n");
                goto error_exit;
            }
            assert(0 == mysql_errno(conn));

            lua_pushboolean(ell, true);
            lua_pushnumber(ell, mysql_affected_rows(conn));

            completeLuaQuery(lua_query, 2);
            continue;
        }
        assert(0 == mysql_errno(conn));

        // resultset
        lua_pushboolean(ell, true);
        lua_newtable(ell);

        MYSQL_ROW row;
        int row_index = 0;
        while ((row = mysql_fetch_row(result))) {
            const unsigned long *const l = mysql_fetch_lengths(result);

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

        completeLuaQuery(lua_query, 2);
    }

error_exit:
    ;
    lua_State *const ell_err = handleMagicPointer(lua_query);
    lua_pushboolean(ell_err, false);
    lua_pushnil(ell_err);

    completeLuaQuery(lua_query, 2);
    return (void *)1;
}

static struct LuaQuery *
createLuaQuery(const char *const query)
{
    struct LuaQuery *const lua_query = malloc(sizeof(struct LuaQuery));
    if (!lua_query) {
        fprintf(stderr, "malloc failed!\n");
        return NULL;
    }

    lua_query->magic_ptr = malloc(sizeof(void *));
    if (!lua_query->magic_ptr) {
        free(lua_query);
        fprintf(stderr, "malloc failed!\n");
        return NULL;
    }
    *lua_query->magic_ptr = NULL;

    lua_query->query = strdup(query);
    if (NULL == lua_query->query) {
        free(lua_query->magic_ptr);
        free(lua_query);
        fprintf(stderr, "strdup failed!\n");
        return NULL;
    }

    lua_query->command_ready     = false;
    lua_query->completion_signal = false;
    return lua_query;
}

static void
destroyLuaQuery(struct LuaQuery **lua_query)
{
    assert(lua_query && *lua_query && (*lua_query)->query &&
           (*lua_query)->magic_ptr);
    free((char *)(*lua_query)->query);
    free((*lua_query)->magic_ptr);
    free(*lua_query);

    *lua_query = NULL;
}
