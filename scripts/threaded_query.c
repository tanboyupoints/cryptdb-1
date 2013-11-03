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

#define NO_THREAD               -1

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

enum RESTART_STATUS {RESTARTED, ONLY_SANE_STOP, FUBAR};
enum Command {QUERY, RESULTS, KILL};

// SUCCESS          : a thread was running and we killed it
// NOTHING_TO_STOP  : tried to stop a thread; but none was running.
// PENDING_SELF_DESTRUCTION : a thread was running and we sent it a kill
//                            signal, but it has not yet entered asynch
//                            kill mode. the thread must exit at the next
//                            opportunity and clean up it's own metadata.
// FAILURE          : a thread was running, but we failed to kill it.
enum STOP_TYPE {SUCCESS, NOTHING_TO_STOP, PENDING_SELF_DESTRUCTION,
                FAILURE};

struct HostData {
    const char *host;
    const char *user;
    const char *passwd;
    unsigned int port;
};

// survives restarts
// > must not be modified by commandHandler; these values can always be
//   meaningfully read after running issueCommand.
struct PersistentState {
    unsigned int restarts;
    // using a pointer for 'host_data' allows for PersistentState
    // to determine if 'host_data' is valid or not.
    // > non-NULL == valid
    const struct HostData *host_data;       // deep copied

    // parameter to all commands
    lua_State *ell;

    unsigned int wait;
};

struct LuaQuery {
    // internal state
    enum Command command;
    bool command_ready;
    bool completion_signal;
    pthread_t thread;
    struct PersistentState persist;

    // QUERY parameter
    const char *sql;                        // deep copied

    // output
    unsigned int output_count;

    // mysql may take 20 or 30 seconds to timeout on failure and we don't
    // want to kill the whole server when this happens
    bool mysql_connected;
};

static struct HostData *createHostData(const char *const host,
                                       const char *const user,
                                       const char *const passwd,
                                       unsigned int port);
static void destroyHostData(struct HostData **p_host_data);

static struct LuaQuery **createLuaQuery(struct HostData **p_host_data,
                                        unsigned int wait);
static void destroyLuaQuery(struct LuaQuery ***pp_lua_query);
static bool startLuaQueryThread(struct LuaQuery *const lua_query);
static enum STOP_TYPE stopLuaQueryThread(struct LuaQuery *const lua_query);
void clearLuaQuery(struct LuaQuery *const lua_query);
void finishedCommandIssue(struct LuaQuery *const lua_query);
void strangeFinishedCommandIssue(struct LuaQuery *const lua_query,
                                 bool exit_status);
static enum RESTART_STATUS restartLuaQueryThread(struct LuaQuery **lua_query);

static bool zombie(struct LuaQuery *const lua_query);
static void issueCommand(lua_State *L, enum Command command,
                         struct LuaQuery **const p_lua_query);
static bool issueQuery(lua_State *L, enum Command command,
                       const char *const query,
                       struct LuaQuery **const p_lua_query);
static void *commandHandler(void *const lq);

static void nilTheStack(lua_State *const L, int pushed);
static void nilTheStackPlus(struct LuaQuery *const lua_query,
                            int pushed);

static void pushvalue(lua_State *const L, const char *const string,
                      long int len);
static const char *luaToCharp(lua_State *const L, int index);

static int
start(lua_State *const L)
{
    const char *host        = luaToCharp(L, 1);
    const char *user        = luaToCharp(L, 2);
    const char *passwd      = luaToCharp(L, 3);
    const unsigned int port = lua_tointeger(L, 4);

    struct HostData *host_data =
        createHostData(host, user, passwd, port);
    if (!host_data) {
        fprintf(stderr, "createHostData failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    struct LuaQuery **const p_lua_query = createLuaQuery(&host_data, 1);
    if (!p_lua_query) {
        destroyHostData(&host_data);
        fprintf(stderr, "createLuaQuery failed!\n");
        lua_pushboolean(L, false);
        lua_pushnil(L);
        return 2;
    }

    lua_pushboolean(L, true);
    lua_pushnumber(L, (unsigned long)p_lua_query);
    return 2;
}

static int
query(lua_State *const L)
{
    struct LuaQuery **p_lua_query =
        (struct LuaQuery **)lua_tointeger(L, 1);
    assert(p_lua_query && *p_lua_query);

    const char *const q = luaToCharp(L, 2);

    if (false == issueQuery(L, QUERY, q, p_lua_query)) {
        fprintf(stderr, "issueQuery failed!\n");
        lua_pushboolean(L, false);
        nilTheStackPlus(*p_lua_query, 1);
        return COMMAND_OUTPUT_COUNT;
    }

    assert(COMMAND_OUTPUT_COUNT == (*p_lua_query)->output_count);
    return (*p_lua_query)->output_count;
}

static int
results(lua_State *const L)
{
    struct LuaQuery **p_lua_query =
        (struct LuaQuery **)lua_tointeger(L, 1);
    assert(p_lua_query && *p_lua_query);

    issueCommand(L, RESULTS, p_lua_query);
    assert(COMMAND_OUTPUT_COUNT == (*p_lua_query)->output_count);
    return (*p_lua_query)->output_count;
}

static int
kill(lua_State *const L)
{
    struct LuaQuery **p_lua_query =
        (struct LuaQuery **)lua_tointeger(L, 1);
    assert(p_lua_query && *p_lua_query);

    // soft kill
    issueCommand(L, KILL, p_lua_query);

    const int output_count = (*p_lua_query)->output_count;
    assert(COMMAND_OUTPUT_COUNT == output_count);

    // hard kill
    destroyLuaQuery(&p_lua_query);
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

// > guarenteed to return meaningful (safe to access)
//   LuaQuery::PresistentData and LuaQuery::output_count.
void
issueCommand(lua_State *const L, enum Command command,
             struct LuaQuery **const p_lua_query)
{
    assert(L && p_lua_query && *p_lua_query);

    assert(NULL == (*p_lua_query)->persist.ell);
    assert(false == (*p_lua_query)->completion_signal);

    // execute command
    (*p_lua_query)->command       = command;
    (*p_lua_query)->persist.ell   = L;
    (*p_lua_query)->command_ready = true;

    const time_t test = time(NULL) + (*p_lua_query)->persist.wait;
    while (false == (*p_lua_query)->completion_signal) {
        if (difftime(test, time(NULL)) < 0) {
            break;
        }
    }

    // handle result
    if (false == (*p_lua_query)->completion_signal) {
        // the worker thread did not complete the operation
        // > a bad state because the thread might signal completion
        // after we reset completion

        // if we were trying to kill, issue a hard kill
        // > if the hard kill fails, we can no longer effectively
        // communicate with this thread. this is problematic because
        // a rogue thread has a pointer to the lua_State.
        if (KILL == command) {
            const enum STOP_TYPE stop_type =
                stopLuaQueryThread(*p_lua_query);
            if (FAILURE == stop_type) {
                fprintf(stderr, "panicing: failed to stop thread after"
                                " KILL failed");
                exit(0);
            }

            strangeFinishedCommandIssue(*p_lua_query, true);
            return;
        }

        if (zombie(*p_lua_query)) {
            fprintf(stderr, "zombie, no restart!\n");
            strangeFinishedCommandIssue(*p_lua_query, false);
            return;
        }

        fprintf(stderr, "no completion signal, restarting!\n");

        const enum RESTART_STATUS rstatus =
            restartLuaQueryThread(p_lua_query);
        switch (rstatus) {
            case RESTARTED:
                // managed to get a replacement thread running
            case ONLY_SANE_STOP:
                // killed the original thread; but no replacement thread
                assert(L        == (*p_lua_query)->persist.ell);
                assert(false    == (*p_lua_query)->completion_signal);
                assert(false    == (*p_lua_query)->command_ready);

                strangeFinishedCommandIssue(*p_lua_query, false);
                return;

            case FUBAR:
                // rogue thread has lua_State pointer
            default:
                fprintf(stderr, "panicing: rogue worker thread!\n");
                exit(0);
        }
    }

    assert(L        == (*p_lua_query)->persist.ell);
    assert(true     == (*p_lua_query)->completion_signal);
    assert(false    == (*p_lua_query)->command_ready);

    finishedCommandIssue((*p_lua_query));
    return;
}

bool
issueQuery(lua_State *const L, enum Command command,
           const char *const query,
           struct LuaQuery **const p_lua_query)
{
    // HACK.
    (*p_lua_query)->sql = strdup(query);
    if (NULL == (*p_lua_query)->sql) {
        perror("strdup failed!\n");
        return false;
    }

    issueCommand(L, command, p_lua_query);
    return true;
}

static void
waitForCommand(struct LuaQuery *const lua_query)
{
    while (false == lua_query->command_ready);
    assert(lua_query->persist.ell);
    return;
}

static void
freeSQL(struct LuaQuery *const lua_query)
{
    assert(!!lua_query->sql == !!(QUERY == lua_query->command));
    if (lua_query->sql) {
        free((void *)lua_query->sql);
        lua_query->sql = NULL;
    }
}

// commandHandler signals back that it has finished
void
completeLuaQuery(struct LuaQuery *const lua_query,
                 unsigned output_count)
{
    assert(true     == lua_query->command_ready);
    assert(false    == lua_query->completion_signal);
    assert(lua_query->persist.ell);

    freeSQL(lua_query);

    nilTheStackPlus(lua_query, output_count);
    assert(COMMAND_OUTPUT_COUNT == lua_query->output_count);

    lua_query->command_ready     = false;
    lua_query->completion_signal = true;
}

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//    issueCommand puts things back to a neutral state
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
void
finishedCommandIssue(struct LuaQuery *const lua_query)
{
    lua_query->command_ready        = false;
    lua_query->completion_signal    = false;
    lua_query->command              = -1;
    lua_query->persist.ell          = NULL;
}

void
strangeFinishedCommandIssue(struct LuaQuery *const lua_query,
                            bool exit_status)
{
    // we may be in an abnormal state; as we likely came here after an
    // asynch kill.
    assert(lua_query->persist.ell);

    freeSQL(lua_query);

    lua_pushboolean(lua_query->persist.ell, exit_status);
    nilTheStackPlus(lua_query, 1);
    assert(COMMAND_OUTPUT_COUNT == lua_query->output_count);

    finishedCommandIssue(lua_query);
}

// close the mysql connection if we receive an asynch kill order
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
        perror("mysql_init failed!\n");
        return SAD_THREAD_EXIT;
    }

    // mysql does not respect the timeout to the precise quantity
    unsigned int connect_timeout = 1;
    if (mysql_options(init, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout)) {
        perror("mysql_options failed!\n");
        return SAD_THREAD_EXIT;
    }

    // hackery, we don't want to receive asynchronous cancellations until
    // after we acquire resources.  we also need to clean up our mysql
    // connection if it succeeds; given all exit paths.
    MYSQL *const conn =
        mysql_real_connect(init, lua_query->persist.host_data->host,
                           lua_query->persist.host_data->user,
                           lua_query->persist.host_data->passwd, NULL,
                           lua_query->persist.host_data->port, NULL, 0);
    if (!conn) {
        perror("mysql_real_connect failed!\n");
        return SAD_THREAD_EXIT;
    }
    lua_query->mysql_connected = true;

    // if the caller tried to kill us before this point, we should
    // immediately die after entering ASYNCH KILL mode.
    pthread_cleanup_push(&cleanup_handler, conn);
    assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
    // debug variable
    bool queried = false;
    while (true) {
        // blocking
        waitForCommand(lua_query);

        assert(false == lua_query->completion_signal);
        if (KILL == lua_query->command) {
            assert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));
            lua_pushboolean(lua_query->persist.ell, true);
            completeLuaQuery(lua_query, 1);
            // mysql_close(conn);
            return HAPPY_THREAD_EXIT;
        } else if (QUERY == lua_query->command) {
            assert(false == queried);
            queried = true;

            if (mysql_query(conn, lua_query->sql)) {
                assert(mysql_errno(conn));
                lua_pushboolean(lua_query->persist.ell, false);
            } else {
                lua_pushboolean(lua_query->persist.ell, true);
            }
            NO_KILLING(completeLuaQuery(lua_query, 1));
            continue;
        }

        // ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        //            fetch results
        // ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        assert(RESULTS == lua_query->command);
        if (false == queried) {
            // trying to get results after thread was killed during QUERY
            lua_pushboolean(lua_query->persist.ell, false);
            lua_pushnil(lua_query->persist.ell);

            NO_KILLING(completeLuaQuery(lua_query, 2));
            continue;
        }
        queried = false;

        MYSQL_RES *const result = mysql_store_result(conn);
        const int field_count = mysql_field_count(conn);
        if (NULL == result) {
            if (mysql_errno(conn)) {
                // query failed
                lua_pushboolean(lua_query->persist.ell, false);
                lua_pushnil(lua_query->persist.ell);

                NO_KILLING(completeLuaQuery(lua_query, 2));
                continue;
            }

            lua_pushboolean(lua_query->persist.ell, true);
            lua_pushnumber(lua_query->persist.ell,
                           mysql_affected_rows(conn));

            NO_KILLING(completeLuaQuery(lua_query, 2));
            continue;
        }
        assert(0 == mysql_errno(conn));

        // resultset
        lua_pushboolean(lua_query->persist.ell, true);
        lua_newtable(lua_query->persist.ell);

        MYSQL_ROW row;
        int row_index = 0;
        while ((row = mysql_fetch_row(result))) {
            const unsigned long *const l = mysql_fetch_lengths(result);

            // row
            lua_pushnumber(lua_query->persist.ell, row_index + 1);
            lua_newtable(lua_query->persist.ell);
            int col_index = 0;
            for (; col_index < field_count; ++col_index) {
                lua_pushnumber(lua_query->persist.ell, col_index + 1);
                pushvalue(lua_query->persist.ell, row[col_index],
                          l[col_index]);
                lua_settable(lua_query->persist.ell, -3);
            }
            lua_settable(lua_query->persist.ell, -3);

            row_index += 1;
        }

        mysql_free_result(result);
        NO_KILLING(completeLuaQuery(lua_query, 2));
    }

    // should never reach this point.
    assert(false);
    pthread_cleanup_pop(0);
}

struct HostData *
createHostData(const char *const host, const char *const user,
               const char *const passwd, unsigned int port)
{
    struct HostData *const host_data = malloc(sizeof(struct HostData));
    if (!host_data) {
        perror("createHostData failed!\n");
        return NULL;
    }

    host_data->host = strdup(host);
    host_data->user = strdup(user);
    host_data->passwd = strdup(passwd);
    if (!host_data->host || !host_data->user || !host_data->passwd) {
        free(host_data);
        perror("strdup failed!\n");
        return NULL;
    }

    host_data->port = port;

    return host_data;
}

void
destroyHostData(struct HostData **p_host_data)
{
    assert(p_host_data && *p_host_data);

    free((void *)(*p_host_data)->host);
    free((void *)(*p_host_data)->user);
    free((void *)(*p_host_data)->passwd);
    free((void *)(*p_host_data));

    *p_host_data = NULL;
}

// doesn't modify persistent data
void
clearLuaQuery(struct LuaQuery *const lua_query)
{
    lua_query->command_ready        = false;
    lua_query->completion_signal    = false;
    lua_query->command              = -1;
    lua_query->sql                  = NULL;
    lua_query->thread               = NO_THREAD;
    lua_query->output_count         = -1;
    lua_query->mysql_connected      = false;
}

static bool
newLuaQuery(struct LuaQuery *const lua_query,
            struct HostData *const host_data,
            unsigned int wait)
{
    memset(lua_query, sizeof(struct LuaQuery), 0);

    clearLuaQuery(lua_query);

    // persistent data should only be initialized at object creation
    lua_query->persist.host_data = host_data;
    lua_query->persist.restarts  = 0;
    lua_query->persist.ell       = NULL;
    lua_query->persist.wait      = wait;

    if (false == startLuaQueryThread(lua_query)) {
        fprintf(stderr, "startLuaQueryThread failed!\n");
        return false;
    }

    return true;
}

// on success, we take ownership of @p_host_data
static struct LuaQuery **
createLuaQuery(struct HostData **p_host_data, unsigned int wait)
{
    assert(p_host_data && *p_host_data);

    struct LuaQuery **p_lua_query = malloc(sizeof(struct LuaQuery *));
    if (!p_lua_query) {
        perror("malloc failed!\n");
        return NULL;
    }

    struct LuaQuery *const lua_query = malloc(sizeof(struct LuaQuery));
    if (!lua_query) {
        free(p_lua_query);
        perror("malloc failed!\n");
        return NULL;
    }
    *p_lua_query = lua_query;

    if (false == newLuaQuery(lua_query, *p_host_data, wait)) {
        free(p_lua_query);
        free(lua_query);
        fprintf(stderr, "newLuaQuery failed!\n");
        return NULL;
    }

    // caller loses ownership
    *p_host_data = NULL;

    return p_lua_query;
}
bool
startLuaQueryThread(struct LuaQuery *const lua_query)
{
    assert(NO_THREAD == lua_query->thread);

    if (pthread_create(&lua_query->thread, NULL, commandHandler,
                       (void *)lua_query)) {
        lua_query->thread = NO_THREAD;
        perror("pthread_create failed!\n");
        return false;
    }

    assert(NO_THREAD != lua_query->thread);

    return true;
}

// NOTE: Callers are apt to fault if this function does not complete
// successfully.
enum STOP_TYPE
stopLuaQueryThread(struct LuaQuery *const lua_query)
{
    // kill(...) issues a soft kill that then a hardkill, so we don't
    // do anything if the soft kill succeeded
    if (NO_THREAD == lua_query->thread) {
        return NOTHING_TO_STOP;
    }

    // try to cancel the thread if it hasn't already exited
    // > not safe to cancel a thread after joining it as system may reuse
    //   id
    const int why = pthread_cancel(lua_query->thread);
    if (why != 0 && why != ESRCH) {
        perror("pthread_cancel did something weird!!\n");
        return FAILURE;
    }

    // Don't wait for mysql to connect (or fail to connect)
    // > Once it's done connecting the thread will either exit because
    //   it failed to connect, or because pthread_cancel has queued
    //   a kill request for it.
    if (false == lua_query->mysql_connected) {
        lua_query->thread = NO_THREAD;
        return PENDING_SELF_DESTRUCTION;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        fprintf(stderr, "clock_gettime failed!\n");
        return FAILURE;
    }

    ts.tv_sec += lua_query->persist.wait;

    void *exit_code;
    if (pthread_timedjoin_np(lua_query->thread, &exit_code, &ts)) {
        perror("pthread_timedjoin_np failed!\n");
        return FAILURE;
    }

    if (exit_code != PTHREAD_CANCELED && exit_code != HAPPY_THREAD_EXIT
        && exit_code != SAD_THREAD_EXIT) {
        fprintf(stderr, "unclear why thread exited: %p\n", exit_code);
        return FAILURE;
    }

    lua_query->thread = NO_THREAD;
    return SUCCESS;
}

static struct LuaQuery *
deepCopyLuaQuery(struct LuaQuery *const lua_query)
{
    assert(lua_query);

    struct LuaQuery *const new_lua_query = malloc(sizeof(struct LuaQuery));
    if (NULL == new_lua_query) {
        perror("malloc failed!\n");
        return NULL;
    }

    memcpy(new_lua_query, lua_query, sizeof(struct LuaQuery));

    // do deep copies
    if (lua_query->sql) {
        new_lua_query->sql = strdup(lua_query->sql);
    } else {
        new_lua_query->sql = NULL;
    }
    new_lua_query->persist.host_data =
        createHostData(lua_query->persist.host_data->host,
                       lua_query->persist.host_data->user,
                       lua_query->persist.host_data->passwd,
                       lua_query->persist.host_data->port);

    if (!new_lua_query->persist.host_data) {
        free(new_lua_query);
        fprintf(stderr, "createHostData failed!\n");
        return NULL;
    }

    return new_lua_query;
}

static void
undoDeepCopyLuaQuery(struct LuaQuery **p_lua_query)
{
    assert(p_lua_query && *p_lua_query);
    freeSQL(*p_lua_query);
    destroyHostData((struct HostData **)&(*p_lua_query)->persist.host_data);
    *p_lua_query = NULL;
}

enum RESTART_STATUS
restartLuaQueryThread(struct LuaQuery **p_lua_query)
{
    assert(p_lua_query && *p_lua_query);

    assert((*p_lua_query)->persist.restarts < MAX_RESTARTS);

    ++(*p_lua_query)->persist.restarts;

    // first make a copy of the metadata in case we lose ownership of
    // if when we try to stop the thread.
    struct LuaQuery *new_lua_query = deepCopyLuaQuery(*p_lua_query);
    if (NULL == new_lua_query) {
        perror("malloc failed!\n");
        return FUBAR;
    }
    assert(new_lua_query != *p_lua_query);
    assert(new_lua_query->sql != (*p_lua_query)->sql
           || !(*p_lua_query)->sql);
    assert(new_lua_query->persist.host_data !=
                (*p_lua_query)->persist.host_data);

    // now we can stop the thread
    const enum STOP_TYPE stop_type = stopLuaQueryThread(*p_lua_query);
    switch (stop_type) {
        case FAILURE:
            fprintf(stderr, "stopLuaQueryThread failed in restart!\n");
            return FUBAR;
        case PENDING_SELF_DESTRUCTION:
            // we lost ownership of original metadata
            // > MEMLEAK
            *p_lua_query = new_lua_query;
            (*p_lua_query)->thread = NO_THREAD;
            break;
        case SUCCESS:
        case NOTHING_TO_STOP:
            // we retained ownership of original metadata
            undoDeepCopyLuaQuery(&new_lua_query);
            break;
        default:
            fprintf(stderr, "unknown stop type!\n");
            exit(0);
    }
    assert(NO_THREAD == (*p_lua_query)->thread);

    // then start a new one and reset state
    clearLuaQuery(*p_lua_query);
    if (false == startLuaQueryThread(*p_lua_query)) {
        fprintf(stderr, "startLuaQueryThread failed in restart!\n");
        return ONLY_SANE_STOP;
    }

    return RESTARTED;
}

static void
destroyLuaQueryNonThreadMetaData(struct LuaQuery ***pp_lua_query)
{
    assert(pp_lua_query && *pp_lua_query && **pp_lua_query);

    freeSQL(**pp_lua_query);
    destroyHostData((struct HostData **)&(**pp_lua_query)->persist.host_data);
    free(**pp_lua_query);
    **pp_lua_query = NULL;

    free(*pp_lua_query);
    *pp_lua_query = NULL;
}

static void
destroyLuaQuery(struct LuaQuery ***pp_lua_query)
{
    assert(pp_lua_query && *pp_lua_query && **pp_lua_query);

    const enum STOP_TYPE stop_type = stopLuaQueryThread(**pp_lua_query);
    switch (stop_type) {
        case FAILURE:
            fprintf(stderr, "stopLuaQueryThread failed!\n");
            exit(0);
        case PENDING_SELF_DESTRUCTION:
            // FIXME: Thread must free it's own metadata.
            // > Currently this MEMLEAKs; but we could fix if necessary.
            // > note that an implementation must respect that potentially
            //   we will try to kill a thread before asynch mode, but we
            //   don't intend on destroyLuaQuery(ing); just restarting the
            //   thread.  this means that we must duplicate the lua_query
            //   data before issuing a kill request because we are
            //   potentially giving the thread ownership of this data.
            *pp_lua_query = NULL;
            return;
        case NOTHING_TO_STOP:
        case SUCCESS:
            destroyLuaQueryNonThreadMetaData(pp_lua_query);
            return;
        default:
            fprintf(stderr, "unknown stop type!\n");
            exit(0);
    }
}

bool
zombie(struct LuaQuery *const lua_query)
{
    assert(lua_query->persist.restarts <= MAX_RESTARTS);

    return MAX_RESTARTS == lua_query->persist.restarts;
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
    nilTheStack(lua_query->persist.ell, pushed);
    lua_query->output_count = COMMAND_OUTPUT_COUNT;
}

// taken from luasql-mysql
void
pushvalue(lua_State *const L, const char *const string, long int len)
{
    if (string == NULL) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, string, len);
    }
}

const char *
luaToCharp(lua_State *const L, int index)
{
    size_t length;
    const char *const p = lua_tolstring(L, index, &length);
    if (!p) {
        return "";
    }

    assert(0 == p[length]);

    return p;
}

#include "threaded_query_tests.c"
