#define COLOR_END       "\033[0m"
#define GREEN_BEGIN     "\033[1;92m"
#define RED_BEGIN       "\033[1;31m"

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//            Simple Testing DSL
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

static unsigned __total_tests       = 0;
static unsigned __passed_tests      = 0;

static unsigned __total_asserts     = 0;
static unsigned __passed_asserts    = 0;

#define DEFINED_NAME

#define TEST_SUCCESS                                \
{                                                   \
   printf("%s%s succeeded!%s\n", GREEN_BEGIN,       \
                                (__test_name),      \
                                COLOR_END);         \
}

#define TEST_FAILURE                                \
{                                                   \
   printf("%s%s failed!%s\n", RED_BEGIN,            \
                              (__test_name),        \
                              COLOR_END);           \
}

#define TEST_ASSERT(expr)                           \
{                                                   \
    ++__total_asserts;                              \
    if (!(expr)) {                                  \
        TEST_FAILURE                                \
        return 0;                                   \
    }                                               \
    ++__passed_asserts;                             \
}

#define TEST(name)                                  \
    static int                                      \
    name(struct lua_State *const L)                 \
    {                                               \
        const char *const __test_name = #name;      \
        ++__total_tests;

#define END_TEST                                    \
        TEST_SUCCESS                                \
        ++__passed_tests;                           \
    }

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                Unit Tests
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TEST(test_pushvalue)
    const char *s0 = "hello";
    const char *s1 = "world\0w0rld"; unsigned int s1_len = 11;
    const char *s2 = "paradise";
    pushvalue(L, s0, strlen(s0));
    pushvalue(L, NULL, -1);
    pushvalue(L, s1, s1_len);
    pushvalue(L, s1, strlen(s1));
    pushvalue(L, NULL, 1);
    pushvalue(L, s2, strlen(s2));

    TEST_ASSERT(!strcmp(s2, lua_tolstring(L, -1, NULL)));
    TEST_ASSERT(NULL == lua_tolstring(L, -2, NULL));
    TEST_ASSERT(!strcmp(s1, lua_tolstring(L, -3, NULL)));
    TEST_ASSERT(!memcmp(s1, lua_tolstring(L, -4, NULL), s1_len));
    TEST_ASSERT(NULL == lua_tolstring(L, -5, NULL));
    TEST_ASSERT(!strcmp(s0, lua_tolstring(L, -6, NULL)));
END_TEST

TEST(test_luaToCharp)
    const unsigned int number = 10;
    const char *const s0      = "red";
    const char *const s1      = "green\0purple"; unsigned s1_len = 11;
    const char *const s2      = "blue";

    lua_pushstring(L, s0);
    lua_pushlstring(L, s1, s1_len);
    lua_pushstring(L, s1);
    lua_pushnumber(L, number);

    lua_pushstring(L, s2);
    TEST_ASSERT(!strcmp(luaToCharp(L, -1), s2));

    lua_pushstring(L, s0);
    TEST_ASSERT(!strcmp(luaToCharp(L, -1), s0));

    TEST_ASSERT(number == lua_tointeger(L, -3));
    TEST_ASSERT(!strcmp(luaToCharp(L, -4), s1));
    TEST_ASSERT(!memcmp(luaToCharp(L, -5), s1, s1_len));
    TEST_ASSERT(!strcmp(luaToCharp(L, -6), s0));
END_TEST

TEST(test_waitForCommand)
    struct LuaQuery lua_query;
    lua_query.persist.ell = (void *)0x01;
    lua_query.command_ready = true;
    waitForCommand(&lua_query);
END_TEST

TEST(test_completeLuaQuery)
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
        .sql                = NULL,
        .output_count       = -1
    };

    completeLuaQuery(&lua_query, 0);
    TEST_ASSERT(init_command         == lua_query.command);
    TEST_ASSERT(false                == lua_query.command_ready);
    TEST_ASSERT(true                 == lua_query.completion_signal);
    TEST_ASSERT(init_thread          == lua_query.thread);
    TEST_ASSERT(init_restarts        == lua_query.persist.restarts);
    TEST_ASSERT(init_host_data       == lua_query.persist.host_data);
    TEST_ASSERT(L                    == lua_query.persist.ell);
    TEST_ASSERT(NULL                 == lua_query.sql);
    TEST_ASSERT(COMMAND_OUTPUT_COUNT == lua_query.output_count);
END_TEST

TEST(test_newLuaQuery)
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

    TEST_ASSERT(newLuaQuery(&lua_query, &host_data));

    TEST_ASSERT(-1                   == lua_query.command);
    TEST_ASSERT(false                == lua_query.command_ready);
    TEST_ASSERT(false                == lua_query.completion_signal);
    TEST_ASSERT(NO_THREAD            != lua_query.thread);
    TEST_ASSERT(0                    == lua_query.persist.restarts);
    TEST_ASSERT(&host_data           == lua_query.persist.host_data);
    TEST_ASSERT(NULL                 == lua_query.persist.ell);
    TEST_ASSERT(NULL                 == lua_query.sql);
    TEST_ASSERT(-1                   == lua_query.output_count);

    // the thread is using data allocated from this thread and will
    // segfault and shutdown if we do not manually kill.
    TEST_ASSERT(stopLuaQueryThread(&lua_query));
END_TEST

TEST(test_createLuaQuery)
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

    TEST_ASSERT(lua_query);
    TEST_ASSERT(-1                   == lua_query->command);
    TEST_ASSERT(false                == lua_query->command_ready);
    TEST_ASSERT(false                == lua_query->completion_signal);
    TEST_ASSERT(NO_THREAD            != lua_query->thread);
    TEST_ASSERT(0                    == lua_query->persist.restarts);
    TEST_ASSERT(&host_data           == lua_query->persist.host_data);
    TEST_ASSERT(NULL                 == lua_query->persist.ell);
    TEST_ASSERT(NULL                 == lua_query->sql);
    TEST_ASSERT(-1                   == lua_query->output_count);

    TEST_ASSERT(NULL                 == p_host_data);
    TEST_ASSERT(host_data.host       == lua_query->persist.host_data->host);
    TEST_ASSERT(host_data.user       == lua_query->persist.host_data->user);
    TEST_ASSERT(host_data.passwd     == lua_query->persist.host_data->passwd);
    TEST_ASSERT(host_data.port       == lua_query->persist.host_data->port);

    TEST_ASSERT(stopLuaQueryThread(lua_query));
END_TEST

TEST(test_stopLuaQueryThread)
    struct LuaQuery lua_query, test_lua_query;
    memset(&lua_query, 0x13371337, sizeof(struct LuaQuery));
    memcpy(&test_lua_query, &lua_query, sizeof(struct LuaQuery));

    test_lua_query.thread = lua_query.thread = (pthread_t)malloc(20);
    TEST_ASSERT(false == stopLuaQueryThread(&lua_query));
    TEST_ASSERT(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));
END_TEST

TEST(test_stopAndStartLuaQueryThread)
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
    TEST_ASSERT(startLuaQueryThread(&lua_query));
    TEST_ASSERT(stopLuaQueryThread(&lua_query));
    test_lua_query.thread = lua_query.thread;
    TEST_ASSERT(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));
END_TEST

TEST(test_destroyLuaQuery)
    struct HostData *host_data =
        createHostData("something", "else", "isunderthebed!", 199);
    TEST_ASSERT(host_data);

    struct LuaQuery *lua_query = createLuaQuery(&host_data);
    TEST_ASSERT(lua_query);

    destroyLuaQuery(&lua_query);
    TEST_ASSERT(NULL == lua_query);
END_TEST

TEST(test_restartLuaQuery)
    TEST_ASSERT(MAX_RESTARTS > 0);

    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "where";
    const char *const init_passwd   = "vacation?";
    const unsigned int init_port    = 0x9999;

    struct HostData *host_data =
        createHostData(init_host, init_user, init_passwd, init_port);
    TEST_ASSERT(host_data);

    struct LuaQuery *lua_query = createLuaQuery(&host_data);
    TEST_ASSERT(lua_query);

    TEST_ASSERT(RESTARTED == restartLuaQueryThread(lua_query));
    TEST_ASSERT(false            == lua_query->command_ready);
    TEST_ASSERT(false            == lua_query->completion_signal);
    TEST_ASSERT(-1               == lua_query->command);
    TEST_ASSERT(NULL             == lua_query->sql);
    TEST_ASSERT(-1               == lua_query->output_count);
    TEST_ASSERT(!strcmp(init_host, lua_query->persist.host_data->host));
    TEST_ASSERT(!strcmp(init_user, lua_query->persist.host_data->user));
    TEST_ASSERT(!strcmp(init_passwd, lua_query->persist.host_data->passwd));
    TEST_ASSERT(init_port        == lua_query->persist.host_data->port);
    TEST_ASSERT(1                == lua_query->persist.restarts);
    TEST_ASSERT(NULL             == lua_query->persist.ell);

    TEST_ASSERT(stopLuaQueryThread(lua_query));
END_TEST

TEST(test_fubarRestartLuaQuery)
    TEST_ASSERT(MAX_RESTARTS > 0)

    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "saysomething";
    const char *const init_passwd   = "quickly?";
    const unsigned int init_port    = 0xFFFF;

    struct HostData *host_data =
        createHostData(init_host, init_user, init_passwd, init_port);
    TEST_ASSERT(host_data);

    struct LuaQuery *lua_query = createLuaQuery(&host_data);
    TEST_ASSERT(lua_query);

    // This should cause stopLuaQueryThread to fail.
    lua_query->thread = (pthread_t)malloc(20);
    memset((void *)lua_query->thread, 0x12345678, 4);

    TEST_ASSERT(FUBAR == restartLuaQueryThread(lua_query));
END_TEST

void *
dummyThread(void *const unused)
{
    assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
    assert(!pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));

    while (1);
}

TEST(test_badIssueCommand)
    TEST_ASSERT(MAX_RESTARTS > 0)

    const char *const init_host     = "127.0.0.1";
    const char *const init_user     = "going";
    const char *const init_passwd   = "tophail?";
    const unsigned int init_port    = 0xEEEE;

    struct HostData *host_data =
        createHostData(init_host, init_user, init_passwd, init_port);
    TEST_ASSERT(host_data);

    struct LuaQuery *lua_query = createLuaQuery(&host_data);
    TEST_ASSERT(lua_query);

    // soft kill should fail, then hard kill should succeed
    TEST_ASSERT(!pthread_create(&lua_query->thread, NULL, dummyThread,
                                NULL));
    issueCommand(L, KILL, lua_query, 1);
    TEST_ASSERT(true           == lua_toboolean(L, -COMMAND_OUTPUT_COUNT));
    TEST_ASSERT(NULL                    == lua_query->persist.ell);
    TEST_ASSERT(false                   == lua_query->command_ready);
    TEST_ASSERT(false                   == lua_query->completion_signal);
    TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);

    // test restart attempts
    size_t i = 0;
    for (; i < MAX_RESTARTS; ++i) {
        issueCommand(L, QUERY, lua_query, 1);
        TEST_ASSERT(false      == lua_toboolean(L, -COMMAND_OUTPUT_COUNT));
        TEST_ASSERT(NO_THREAD               != lua_query->thread);
        TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);
        TEST_ASSERT(NULL                    == lua_query->persist.ell);

        TEST_ASSERT(stopLuaQueryThread(lua_query));
        TEST_ASSERT(NO_THREAD               == lua_query->thread);
        TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);

        TEST_ASSERT(i+1                     ==lua_query->persist.restarts);
    }

    // test for sane zombie
    lua_query->sql = strdup("wordly");
    issueCommand(L, QUERY, lua_query, 1);
    TEST_ASSERT(false          == lua_toboolean(L, -COMMAND_OUTPUT_COUNT));
    TEST_ASSERT(NO_THREAD               == lua_query->thread)
    TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);
    TEST_ASSERT(MAX_RESTARTS            == lua_query->persist.restarts);
    TEST_ASSERT(NULL                    == lua_query->persist.ell);

    TEST_ASSERT(stopLuaQueryThread(lua_query));
    TEST_ASSERT(NO_THREAD               == lua_query->thread)
    TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);
    TEST_ASSERT(MAX_RESTARTS            == lua_query->persist.restarts);

    issueCommand(L, KILL, lua_query, 1);
    TEST_ASSERT(true           == lua_toboolean(L, -COMMAND_OUTPUT_COUNT));
    TEST_ASSERT(NO_THREAD               == lua_query->thread);
    TEST_ASSERT(COMMAND_OUTPUT_COUNT    == lua_query->output_count);
    TEST_ASSERT(MAX_RESTARTS            == lua_query->persist.restarts);
    TEST_ASSERT(NULL                    == lua_query->persist.ell);

    TEST_ASSERT(!strcmp(init_host, lua_query->persist.host_data->host));
    TEST_ASSERT(!strcmp(init_user, lua_query->persist.host_data->user));
    TEST_ASSERT(!strcmp(init_passwd,lua_query->persist.host_data->passwd));
    TEST_ASSERT(init_port == lua_query->persist.host_data->port);
END_TEST

TEST(test_zombie)
    struct LuaQuery lua_query, test_lua_query;
    memset(&lua_query, 0x11111111, sizeof(struct LuaQuery));
    memcpy(&test_lua_query, &lua_query, sizeof(struct LuaQuery));

    TEST_ASSERT(MAX_RESTARTS >= 0);
    if (MAX_RESTARTS > 0) {
        test_lua_query.persist.restarts =
            lua_query.persist.restarts =        MAX_RESTARTS - 1;
        TEST_ASSERT(false == zombie(&lua_query));
        TEST_ASSERT(!memcmp(&lua_query, &test_lua_query,
                       sizeof(struct LuaQuery)));
    }

    test_lua_query.persist.restarts =
        lua_query.persist.restarts =            MAX_RESTARTS;
    TEST_ASSERT(true == zombie(&lua_query));
    TEST_ASSERT(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));

    // will cause abort
    /*
    test_lua_query.persist.restarts =
        lua_query.persist.restarts =            MAX_RESTARTS + 1;
    TEST_ASSERT(true == zombie(&lua_query));
    TEST_ASSERT(!memcmp(&lua_query, &test_lua_query, sizeof(struct LuaQuery)));
    */
END_TEST

TEST(test_createHostData)
    const char *const init_host     = "birdsandplains";
    const char *const init_user     = "automobiles";
    const char *const init_passwd   = "sendandreceive";
    const unsigned int init_port    = 0x9898;

    const struct HostData test_host_data = {
        .host           = strdup(init_host),
        .user           = strdup(init_user),
        .passwd         = strdup(init_passwd),
        .port           = init_port
    };

    struct HostData *const host_data =
        createHostData(init_host, init_user, init_passwd, init_port);
    TEST_ASSERT(host_data);

    TEST_ASSERT(!strcmp(test_host_data.host, host_data->host));
    TEST_ASSERT(!strcmp(test_host_data.user, host_data->user));
    TEST_ASSERT(!strcmp(test_host_data.passwd, host_data->passwd));
    TEST_ASSERT(test_host_data.port == host_data->port);
    TEST_ASSERT(init_port           == host_data->port);

    // make sure createHostData didn't mutate values it doesn't own
    TEST_ASSERT(init_host    != host_data->host);
    TEST_ASSERT(init_user    != host_data->user);
    TEST_ASSERT(init_passwd  != host_data->passwd);

    TEST_ASSERT(!strcmp(test_host_data.host, init_host));
    TEST_ASSERT(!strcmp(test_host_data.user, init_user));
    TEST_ASSERT(!strcmp(test_host_data.passwd, init_passwd));
END_TEST

TEST(test_destroyHostData)
    struct HostData *host_data =
        createHostData("some", "data", "willdo", 1005);
    TEST_ASSERT(host_data);

    destroyHostData(&host_data);
    TEST_ASSERT(NULL == host_data);
END_TEST

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//                 Helpers
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
    test_destroyLuaQuery(L);
    test_restartLuaQuery(L);
    test_fubarRestartLuaQuery(L);
    test_badIssueCommand(L);
    test_zombie(L);
    test_createHostData(L);
    test_destroyHostData(L);

    printf("\n"
           "################################\n"
           "     Passed %d/%d Unit Tests\n"
           "       %d/%d Assertions\n"
           "################################\n\n",
           __passed_tests, __total_tests,
           __passed_asserts, __total_asserts);

    return 0;
}


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

