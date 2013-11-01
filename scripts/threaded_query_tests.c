#define COLOR_END       "\033[0m"
#define GREEN_BEGIN     "\033[1;92m"

#define TEST_SUCCESS(name)                          \
{                                                   \
   printf("%s%s succeeded!%s\n", GREEN_BEGIN,       \
                                (name),             \
                                COLOR_END);         \
}

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                Unit Tests
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

static int
test_pushvalue(struct lua_State *const L)
{
    const char *s0 = "hello";
    const char *s1 = "world\0w0rld"; unsigned int s1_len = 11;
    const char *s2 = "paradise";
    pushvalue(L, s0, strlen(s0));
    pushvalue(L, NULL, -1);
    pushvalue(L, s1, s1_len);
    pushvalue(L, s1, strlen(s1));
    pushvalue(L, NULL, 1);
    pushvalue(L, s2, strlen(s2));

    assert(!strcmp(s2, lua_tolstring(L, -1, NULL)));
    assert(NULL == lua_tolstring(L, -2, NULL));
    assert(!strcmp(s1, lua_tolstring(L, -3, NULL)));
    assert(!memcmp(s1, lua_tolstring(L, -4, NULL), s1_len));
    assert(NULL == lua_tolstring(L, -5, NULL));
    assert(!strcmp(s0, lua_tolstring(L, -6, NULL)));

    TEST_SUCCESS("test_pushvalue");
    return 0;
}

static int
test_luaToCharp(lua_State *const L)
{
    const unsigned int number = 10;
    const char *const s0      = "red";
    const char *const s1      = "green\0purple"; unsigned s1_len = 11;
    const char *const s2      = "blue";

    lua_pushstring(L, s0);
    lua_pushlstring(L, s1, s1_len);
    lua_pushstring(L, s1);
    lua_pushnumber(L, number);

    lua_pushstring(L, s2);
    assert(!strcmp(luaToCharp(L, -1), s2));

    lua_pushstring(L, s0);
    assert(!strcmp(luaToCharp(L, -1), s0));

    assert(number == lua_tointeger(L, -3));
    assert(!strcmp(luaToCharp(L, -4), s1));
    assert(!memcmp(luaToCharp(L, -5), s1, s1_len));
    assert(!strcmp(luaToCharp(L, -6), s0));

    TEST_SUCCESS("test_luaToCharp");
    return 0;
}

static int
test_waitForCommand(struct lua_State *const L)
{
    struct LuaQuery lua_query;
    lua_query.persist.ell = (void *)0x01;
    lua_query.command_ready = true;
    waitForCommand(&lua_query);

    TEST_SUCCESS("test_waitForCommand");
    return 0;
}

static int
test_completeLuaQuery(struct lua_State *const L)
{
    const unsigned int init_command     = RESULTS;
    const pthread_t init_thread         = (pthread_t)0x1;
    const void *const init_host_data    = (void *)0x2;
    const unsigned int init_restarts    = 0x3;

    struct LuaQuery lua_query = {
        .command            = init_command,
        .command_ready      = true,
        .completion_signal  = false,
        .thread             = (pthread_t)init_thread,
        .persist            = {
            .restarts           = init_restarts,
            .host_data          = init_host_data,
            .ell                = L
        },
        .query              = NULL,
        .output_count       = -1
    };

    completeLuaQuery(&lua_query, 0);
    assert(init_command         == lua_query.command);
    assert(false                == lua_query.command_ready);
    assert(true                 == lua_query.completion_signal);
    assert(init_thread          == lua_query.thread);
    assert(init_restarts        == lua_query.persist.restarts);
    assert(init_host_data       == lua_query.persist.host_data);
    assert(L                    == lua_query.persist.ell);
    assert(NULL                 == lua_query.query);
    assert(COMMAND_OUTPUT_COUNT == lua_query.output_count);

    TEST_SUCCESS("test_completeLuaQuery");
    return 0;
}

static int
test_newLuaQuery(struct lua_State *const L)
{
    struct LuaQuery lua_query;
    memset(&lua_query, 0xDEADBEEF, sizeof(struct LuaQuery));

    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "martianinvader";
    const char *const init_passwd   = "lophtcrak";
    const unsigned int init_port    = 0x1111;

    struct HostData host_data = {
        .host       = init_host,
        .user       = init_user,
        .passwd     = init_passwd,
        .port       = init_port
    };

    assert(newLuaQuery(&lua_query, &host_data));

    assert(-1                   == lua_query.command);
    assert(false                == lua_query.command_ready);
    assert(false                == lua_query.completion_signal);
    assert(NO_THREAD            != lua_query.thread);
    assert(0                    == lua_query.persist.restarts);
    assert(&host_data           == lua_query.persist.host_data);
    assert(NULL                 == lua_query.persist.ell);
    assert(NULL                 == lua_query.query);
    assert(-1                   == lua_query.output_count);

    // the thread is using data allocated from this thread and will
    // segfault and shutdown if we do not manually kill.
    assert(stopLuaQueryThread(&lua_query));
    TEST_SUCCESS("test_newLuaQuery");

    return 0;
}

static int
test_createLuaQuery(struct lua_State *const L)
{
    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "rainbow";
    const char *const init_passwd   = "jetplane";
    const unsigned int init_port    = 0x2222;

    const struct HostData host_data = {
        .host       = init_host,
        .user       = init_user,
        .passwd     = init_passwd,
        .port       = init_port
    };
    const struct HostData *p_host_data = (struct HostData *)&host_data;

    struct LuaQuery *const lua_query =
        createLuaQuery((struct HostData **)&p_host_data);

    assert(lua_query);
    assert(-1                   == lua_query->command);
    assert(false                == lua_query->command_ready);
    assert(false                == lua_query->completion_signal);
    assert(NO_THREAD            != lua_query->thread);
    assert(0                    == lua_query->persist.restarts);
    assert(&host_data           == lua_query->persist.host_data);
    assert(NULL                 == lua_query->persist.ell);
    assert(NULL                 == lua_query->query);
    assert(-1                   == lua_query->output_count);

    assert(NULL                 == p_host_data);
    assert(host_data.host       == lua_query->persist.host_data->host);
    assert(host_data.user       == lua_query->persist.host_data->user);
    assert(host_data.passwd     == lua_query->persist.host_data->passwd);
    assert(host_data.port       == lua_query->persist.host_data->port);

    assert(stopLuaQueryThread(lua_query));
    TEST_SUCCESS("test_createLuaQuery");
}

static int
test_stopLuaQueryThread(struct lua_State *const L)
{
    struct LuaQuery lua_query, test_lua_query;
    memset(&lua_query, 0x13371337, sizeof(struct LuaQuery));
    memcpy(&test_lua_query, &lua_query, sizeof(struct LuaQuery));

    test_lua_query.thread = lua_query.thread = (pthread_t)malloc(20);
    assert(false == stopLuaQueryThread(&lua_query));
    assert(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));

    TEST_SUCCESS("test_StopLuaQueryThread");
    return 0;
}

static int
test_stopAndStartLuaQueryThread(struct lua_State *const L)
{
    struct LuaQuery lua_query, test_lua_query;
    memset(&lua_query, 0xDEADBEEF, sizeof(struct LuaQuery));
    memcpy(&test_lua_query, &lua_query, sizeof(struct LuaQuery));

    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "buildings";
    const char *const init_passwd   = "cars";
    const unsigned int init_port    = 0x3333;

    const struct HostData host_data = {
        .host       = init_host,
        .user       = init_user,
        .passwd     = init_passwd,
        .port       = init_port
    };

    lua_query.thread = NO_THREAD;
    test_lua_query.persist.host_data =
        lua_query.persist.host_data =   &host_data;
    assert(startLuaQueryThread(&lua_query));
    assert(stopLuaQueryThread(&lua_query));
    test_lua_query.thread = lua_query.thread;
    assert(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));

    TEST_SUCCESS("test_StopAndStartLuaQuery");
    return 0;
}

static int
test_destroyLuaQuery(struct lua_State *const L)
{
    // stub: pita
    return 0;
}

static int
test_restartLuaQuery(struct lua_State *const L)
{
    // stub: pita
    return 0;
}

static int
test_zombie(struct lua_State *const L)
{
    struct LuaQuery lua_query, test_lua_query;
    memset(&lua_query, 0x11111111, sizeof(struct LuaQuery));
    memcpy(&test_lua_query, &lua_query, sizeof(struct LuaQuery));

    assert(MAX_RESTARTS >= 0);
    if (MAX_RESTARTS > 0) {
        test_lua_query.persist.restarts =
            lua_query.persist.restarts =        MAX_RESTARTS - 1;
        assert(false == zombie(&lua_query));
        assert(!memcmp(&lua_query, &test_lua_query,
                       sizeof(struct LuaQuery)));
    }

    test_lua_query.persist.restarts =
        lua_query.persist.restarts =            MAX_RESTARTS;
    assert(true == zombie(&lua_query));
    assert(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));

    // will cause abort
    /*
    test_lua_query.persist.restarts =
        lua_query.persist.restarts =            MAX_RESTARTS + 1;
    assert(true == zombie(&lua_query));
    assert(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));
    */

    TEST_SUCCESS("test_zombie");
    return 0;
}

static int
test_createHostData(struct lua_State *const L)
{

    TEST_SUCCESS("test_createHostData");
    return 0;
}

static int
test_destroyHostData(struct lua_State *const L)
{

    TEST_SUCCESS("test_createHostData");
    return 0;
}

static int
all(struct lua_State *const L)
{
    test_pushvalue(L);
    test_luaToCharp(L);
    test_waitForCommand(L);
    test_completeLuaQuery(L);
    test_newLuaQuery(L);
    test_createLuaQuery(L);
    test_stopLuaQueryThread(L);
    test_stopAndStartLuaQueryThread(L);

    test_zombie(L);

    return 0;
}

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//             Integration Tests
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                 Helpers
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                 Exports
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

static const struct luaL_reg
test_lib[] = {
#define F(n) { #n, n }
    F(test_pushvalue),
    F(all),
    {0, 0},
};

extern int
lua_test_init(lua_State *const L)
{
    luaL_openlib(L, "TestThreadedQuery", test_lib, 0);

    return 1;
}

