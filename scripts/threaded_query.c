// gcc -shared -fpic main.c -o main.so --llua5.1
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <pthread.h>
#include <sys/types.h>

#include <mysql/mysql.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#define HAPPY_THREAD_EXIT       (void *)101
#define SAD_THREAD_EXIT         (void *)0x5AD

#define NO_KILLING(stmt)                                                \
{                                                                       \
        assert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));  \
        (stmt);                                                         \
        assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));   \
}

#define MAX_RESTARTS            3
#define COMMAND_OUTPUT_COUNT    2

enum Command {QUERY, RESULTS, KILL};

struct LuaQuery {
    // internal state
    enum Command command;
    bool command_ready;
    bool completion_signal;
    pthread_t thread;
    unsigned int restarts;

    // parameter to all commands
    lua_State *ell;

    // QUERY parameter
    const char *query;

    // output
    unsigned int output_count;
};

static struct LuaQuery *createLuaQuery();

static void destroyLuaQuery(struct LuaQuery **lua_query);

bool stopLuaQuery(struct LuaQuery *const lua_query);

static bool restartLuaQuery(struct LuaQuery *lua_query);

static bool zombie(struct LuaQuery *const lua_query);

void issueCommand(lua_State *L, enum Command command,
                  struct LuaQuery *const lua_query, unsigned int wait);

bool issueQuery(lua_State *L, enum Command command,
                const char *const query, struct LuaQuery *const lua_query,
                unsigned int wait);

void *commandHandler(void *const lq);

void nilTheStack(lua_State *const L, int pushed);
void nilTheStackPlus(struct LuaQuery *const lua_query, int pushed);

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
start(lua_State *const L)
{
    struct LuaQuery *const lua_query = createLuaQuery();
    if (!lua_query) {
        fprintf(stderr, "createLuaQuery failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    lua_pushboolean(L, true);
    lua_pushnumber(L, (unsigned long)lua_query);
    return 2;
}

static int
query(lua_State *const L)
{
    struct LuaQuery *const lua_query =
        (struct LuaQuery *)lua_tointeger(L, 1);
    assert(lua_query);

    // NOTE: Will not handle embedded NUL characters.
    size_t length;
    const char *const q = lua_tolstring(L, 2, &length);

    if (false == issueQuery(L, QUERY, q, lua_query, 1)) {
        fprintf(stderr, "issueQuery failed!\n");
        lua_pushboolean(L, false);
        nilTheStackPlus(lua_query, 1);
        return COMMAND_OUTPUT_COUNT;
    }

    assert(COMMAND_OUTPUT_COUNT == lua_query->output_count);
    return lua_query->output_count;
}

static int
results(lua_State *const L)
{
    struct LuaQuery *const lua_query =
        (struct LuaQuery *)lua_tointeger(L, 1);
    assert(lua_query);

    issueCommand(L, RESULTS, lua_query, 1);
    assert(COMMAND_OUTPUT_COUNT == lua_query->output_count);
    return lua_query->output_count;
}

static int
kill(lua_State *const L)
{
    struct LuaQuery *lua_query = (struct LuaQuery *)lua_tointeger(L, 1);
    assert(lua_query);

    // soft kill
    issueCommand(L, KILL, lua_query, 1);

    const int output_count = lua_query->output_count;
    assert(COMMAND_OUTPUT_COUNT == output_count);

    // hard kill
    destroyLuaQuery(&lua_query);
    lua_pushboolean(L, true);
    nilTheStack(L, 1);
    return COMMAND_OUTPUT_COUNT;
}

static const struct luaL_reg
main_lib[] = {
#define F(n) { #n, n }
    F(start),
    F(query),
    F(results),
    F(kill),
    {0, 0},
};

extern int
lua_main_init(lua_State *const L)
{
    luaL_openlib(L, "ThreadedQuery", main_lib, 0);
    return 1;
}


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                  Helpers
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

void
issueCommand(lua_State *const L, enum Command command,
             struct LuaQuery *lua_query, unsigned int wait)
{
    assert(L && lua_query);

    if (zombie(lua_query)) {
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return;
    }

    assert(false == lua_query->completion_signal);

    // execute command
    lua_query->command       = command;
    lua_query->ell           = L;
    lua_query->command_ready = true;

    const time_t test = time(NULL) + wait;
    while (false == lua_query->completion_signal) {
        if (difftime(test, time(NULL)) < 0) {
            break;
        }
    }

    // handle result
    if (false == lua_query->completion_signal) {
        if (KILL == command) {
            if (false == stopLuaQuery(lua_query)) {
                fprintf(stderr, "failed to stop thread after KILL failed");
                exit(0);
            }

            lua_pushboolean(L, true);
            nilTheStackPlus(lua_query, 1);
            return;
        }

        // bad state because the thread might signal completion after
        // we reset completion
        fprintf(stderr, "no completion signal, restarting!\n");

        // if need be we can handle cases where we at least stopped the
        // thread.
        if (false == restartLuaQuery(lua_query)) {
            fprintf(stderr, "panic: failed to restart thread!\n");
            exit(0);
        }

        assert(false == lua_query->completion_signal
               && false == lua_query->command_ready);

        // return failure to the caller
        lua_pushboolean(L, false);
        nilTheStackPlus(lua_query, 1);
        return;
    }

    lua_query->completion_signal = false;

    assert(false == lua_query->command_ready);
    return;
}

bool
issueQuery(lua_State *const L, enum Command command,
           const char *const query, struct LuaQuery *lua_query,
           unsigned int wait)
{
    lua_query->query = strdup(query);
    if (NULL == lua_query->query) {
        fprintf(stderr, "strdup failed!\n");
        return false;
    }

    issueCommand(L, command, lua_query, wait);
    return true;
}

static void
waitForCommand(struct LuaQuery *const lua_query)
{
    while (false == lua_query->command_ready);
    assert(lua_query->ell);
    return;
}

void
completeLuaQuery(struct LuaQuery *lua_query, unsigned output_count)
{
    assert(true == lua_query->command_ready &&
           false == lua_query->completion_signal);

    assert(!!lua_query->query == !!(QUERY == lua_query->command));
    if (lua_query->query) {
        free((void *)lua_query->query);
        lua_query->query = NULL;
    }

    nilTheStackPlus(lua_query, output_count);
    assert(COMMAND_OUTPUT_COUNT == lua_query->output_count);

    lua_query->command_ready     = false;
    lua_query->ell               = NULL;
    lua_query->completion_signal = true;
}

static void
cleanup_handler(void *const mysql_conn)
{
    assert(mysql_conn);
    mysql_close(mysql_conn);
}

void *
commandHandler(void *const lq)
{
    assert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));
    assert(!pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));

    struct LuaQuery *lua_query = (struct LuaQuery *)lq;

    // do we need to do this for each thread?
    MYSQL *const init = mysql_init(NULL);
    if (!init) {
        fprintf(stderr, "mysql_init failed!\n");
        goto no_killing_error_exit;
    }

    const char *const host      = "127.0.0.1";
    const char *const user      = "root";
    const char *const passwd    = "letmein";
    const unsigned int port     = 3306;

    // hackery, we don't want to receive asynchronous cancellations until
    // after we acquire resources.  we also need to clean up our mysql
    // connection if it succeeds.
    MYSQL *const conn =
        mysql_real_connect(init, host, user, passwd, NULL, port, NULL, 0);
    if (!conn) {
        fprintf(stderr, "mysql_real_connect failed!\n");
        goto no_killing_error_exit;
    }

    pthread_cleanup_push(&cleanup_handler, conn);
    assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
    // debug variable
    bool queried = false;
    while (true) {
        // blocking
        waitForCommand(lua_query);
        assert(!!queried == !!(lua_query->command == RESULTS));

        assert(false == lua_query->completion_signal);
        if (KILL == lua_query->command) {
            assert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));
            lua_pushboolean(lua_query->ell, true);
            completeLuaQuery(lua_query, 1);
            mysql_close(conn);
            return HAPPY_THREAD_EXIT;
        } else if (QUERY == lua_query->command) {
            if (mysql_query(conn, lua_query->query)) {
                fprintf(stderr, "mysql_query failed!\n");
                lua_pushboolean(lua_query->ell, false);
            } else {
                lua_pushboolean(lua_query->ell, true);
            }
            NO_KILLING(completeLuaQuery(lua_query, 1));
            queried = true;
            continue;
        }

        assert(RESULTS == lua_query->command);
        queried = false;

        MYSQL_RES *const result = mysql_store_result(conn);
        const int field_count = mysql_field_count(conn);
        if (NULL == result) {
            if (mysql_errno(conn)) {
                // query failed
                lua_pushboolean(lua_query->ell, false);
                lua_pushnil(lua_query->ell);

                NO_KILLING(completeLuaQuery(lua_query, 2));
                continue;
            }

            lua_pushboolean(lua_query->ell, true);
            lua_pushnumber(lua_query->ell, mysql_affected_rows(conn));

            NO_KILLING(completeLuaQuery(lua_query, 2));
            continue;
        }
        assert(0 == mysql_errno(conn));

        // resultset
        lua_pushboolean(lua_query->ell, true);
        lua_newtable(lua_query->ell);

        MYSQL_ROW row;
        int row_index = 0;
        while ((row = mysql_fetch_row(result))) {
            const unsigned long *const l = mysql_fetch_lengths(result);

            // row
            lua_pushnumber(lua_query->ell, row_index + 1);
            lua_newtable(lua_query->ell);
            int col_index = 0;
            for (; col_index < field_count; ++col_index) {
                lua_pushnumber(lua_query->ell, col_index + 1);
                pushvalue(lua_query->ell, row[col_index], l[col_index]);
                lua_settable(lua_query->ell, -3);
            }
            lua_settable(lua_query->ell, -3);

            row_index += 1;
        }

        NO_KILLING(completeLuaQuery(lua_query, 2));
    }

    // should never reach this point.
    assert(false);
    pthread_cleanup_pop(0);

no_killing_error_exit:
    return (void *)SAD_THREAD_EXIT;
}

static bool
initLuaQuery(struct LuaQuery *const lua_query)
{
    lua_query->command_ready     = false;
    lua_query->completion_signal = false;
    lua_query->query             = false;
    lua_query->ell               = NULL;
    lua_query->command           = -1;
    lua_query->restarts          = 0;

    if (pthread_create(&lua_query->thread, NULL, commandHandler,
                       (void *)lua_query)) {
        return false;
    }

    return true;
}

static struct LuaQuery *
createLuaQuery()
{
    struct LuaQuery *const lua_query = malloc(sizeof(struct LuaQuery));
    if (!lua_query) {
        fprintf(stderr, "malloc failed!\n");
        return NULL;
    }

    if (false == initLuaQuery(lua_query)) {
        free(lua_query);
        fprintf(stderr, "initLuaQuery failed!\n");
        return NULL;
    }

    return lua_query;
}

bool
stopLuaQuery(struct LuaQuery *const lua_query)
{
    // try to cancel the thread if it hasn't already exited
    // > not safe to cancel a thread after joining it as system may reuse
    //   id
    const int why = pthread_cancel(lua_query->thread);
    if (why != 0 && why != ESRCH) {
        fprintf(stderr, "pthread_cancel did something weird!!\n");
        return false;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        fprintf(stderr, "clock_gettime failed!\n");
        return false;
    }

    ts.tv_sec += 1;

    void *exit_code;
    if (pthread_timedjoin_np(lua_query->thread, &exit_code, &ts)) {
        fprintf(stderr, "pthread_timedjoin_np failed!\n");
        return false;
    }

    if (exit_code != PTHREAD_CANCELED && exit_code != HAPPY_THREAD_EXIT
        && exit_code != SAD_THREAD_EXIT) {
        fprintf(stderr, "unclear why thread exited!\n");
        return false;
    }

    return true;
}

static void
destroyLuaQuery(struct LuaQuery **lua_query)
{
    assert(lua_query && *lua_query);

    if (false == stopLuaQuery(*lua_query)) {
        fprintf(stderr, "stopLuaQuery failed!\n");
        exit(0);
    }

    if ((*lua_query)->query) {
        free((char *)(*lua_query)->query);
    }
    free(*lua_query);

    *lua_query = NULL;
}

bool
restartLuaQuery(struct LuaQuery *lua_query)
{
    assert(lua_query->restarts < MAX_RESTARTS);

    enum Command command     = lua_query->command;
    unsigned int restarts    = ++lua_query->restarts;
    lua_State *const ell     = lua_query->ell;

    // first stop the thread
    if (false == stopLuaQuery(lua_query)) {
        fprintf(stderr, "stopLuaQuery failed in restart!\n");
        return false;
    }

    // then start a new one
    if (false == initLuaQuery(lua_query)) {
        fprintf(stderr, "initLuaQuery failed in restart!\n");
        return false;
    }

    lua_query->command      = command;
    lua_query->restarts     = restarts;
    lua_query->ell          = ell;

    return true;
}

bool
zombie(struct LuaQuery *const lua_query)
{
    assert(lua_query->restarts <= MAX_RESTARTS);

    return MAX_RESTARTS == lua_query->restarts;
}


void
nilTheStack(lua_State *const L, int pushed)
{
    for (; pushed < COMMAND_OUTPUT_COUNT; ++pushed) {
        lua_pushnil(L);
    }
}

void
nilTheStackPlus(struct LuaQuery *const lua_query, int pushed)
{
    nilTheStack(lua_query->ell, pushed);
    lua_query->output_count = COMMAND_OUTPUT_COUNT;
}
