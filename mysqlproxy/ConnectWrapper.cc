#include <sstream>
#include <fstream>
#include <assert.h>
#include <lua5.1/lua.hpp>

#include <util/ctr.hh>
#include <util/cryptdb_log.hh>
#include <util/scoped_lock.hh>

#include <main/rewrite_main.hh>
#include <parser/sql_utils.hh>

class WrapperState {
public:
    std::string last_query;
    bool considered;
    std::ofstream * PLAIN_LOG;
    ReturnMeta * rmeta;
    std::string cur_db;
};

static Timer t;

//static EDBProxy * cl = NULL;
static Rewriter * r = NULL;
static pthread_mutex_t big_lock;

static bool DO_CRYPT = true;

static bool EXECUTE_QUERIES = true;

static std::string TRAIN_QUERY ="";

static bool LOG_PLAIN_QUERIES = false;
static std::string PLAIN_BASELOG = "";


static int counter = 0;

static std::map<std::string, WrapperState*> clients;

static Item *
make_item(std::string value, enum_field_types type)
{
    Item * i;

    switch(type) {
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_LONGLONG:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_TINY:
        i = new Item_int((long long) valFromStr(value));
        break;

    case MYSQL_TYPE_VARCHAR:
    case MYSQL_TYPE_VAR_STRING:
        i = new Item_string(make_thd_string(value), value.length(), &my_charset_bin);
        break;

    default:
        thrower() << "unknown data type " << type;
    }
    return i;
}

static std::string
xlua_tolstring(lua_State *l, int index)
{
    size_t len;
    const char *s = lua_tolstring(l, index, &len);
    return std::string(s, len);
}

static void
xlua_pushlstring(lua_State *l, const std::string &s)
{
    lua_pushlstring(l, s.data(), s.length());
}

// FIXME: Specify database name.
static int
connect(lua_State *L)
{
    ANON_REGION(__func__, &perf_cg);
    scoped_lock l(&big_lock);
    assert(0 == mysql_thread_init());

    std::string client = xlua_tolstring(L, 1);
    std::string server = xlua_tolstring(L, 2);
    uint port = luaL_checkint(L, 3);
    std::string user = xlua_tolstring(L, 4);
    std::string psswd = xlua_tolstring(L, 5);
    std::string embed_dir = xlua_tolstring(L, 6);

    ConnectionInfo ci = ConnectionInfo(server, user, psswd, port);

    WrapperState *ws = new WrapperState();

    if (clients.find(client) != clients.end()) {
           LOG(warn) << "duplicate client entry";
    }

    clients[client] = ws;

    if (!r) {
        std::cerr << "starting proxy\n";
        //cryptdb_logger::setConf(string(getenv("CRYPTDB_LOG")?:""));

        LOG(wrapper) << "connect " << client << "; "
                     << "server = " << server << ":" << port << "; "
                     << "user = " << user << "; "
                     << "password = " << psswd;

        // HACK: This code may require the support of databaseless 'Connect'
        // objects.  If so, use a derived type to make it clear that the 
        // Connect object supports this property.
        std::string dbname = "cryptdbtest";
        std::string mode = getenv("CRYPTDB_MODE")?:"";
        if (mode == "single") {
            std::string encbydefault = getenv("ENC_BY_DEFAULT");
	    if (encbydefault == "false") {
                std::cerr << "\n\n enc by default false " << "\n\n";
		r = new Rewriter(ci, embed_dir, dbname, false, false);
	    } else {
                std::cerr << "\n\nenc by default true" << "\n\n";
		r = new Rewriter(ci, embed_dir, dbname, false, true);
	    }

	} else {
            throw CryptDBError("Unsupported cryptdb mode!");
        }

        uint64_t mkey = 113341234;  // XXX do not change as it's used for tpcc exps
        r->setMasterKey(BytesFromInt(mkey, AES_KEY_BYTES));

        //may need to do training
        char * ev = getenv("TRAIN_QUERY");
        if (ev) {
            std::string trainQuery = ev;
            LOG(wrapper) << "proxy trains using " << trainQuery;
            if (trainQuery != "") {
                // TODO: May need to set current database in ProxyState.
                r->rewrite(trainQuery);
            } else {
                std::cerr << "empty training!\n";
            }
        }

        ev = getenv("DO_CRYPT");
        if (ev) {
            std::string useCryptDB = std::string(ev);
            if (useCryptDB == "false") {
                LOG(wrapper) << "do not crypt queries/results";
                DO_CRYPT = false;
            } else {
                LOG(wrapper) << "crypt queries/result";
            }
        }


        ev = getenv("EXECUTE_QUERIES");
        if (ev) {
            std::string execQueries = std::string(ev);
            if (execQueries == "false") {
                LOG(wrapper) << "do not execute queries";
                EXECUTE_QUERIES = false;
            } else {
                LOG(wrapper) << "execute queries";
            }
        }

        ev = getenv("LOAD_ENC_TABLES");
        if (ev) {
            std::cerr << "No current functionality for loading tables\n";
            //cerr << "loading enc tables\n";
            //cl->loadEncTables(string(ev));
        }

        ev = getenv("LOG_PLAIN_QUERIES");
        if (ev) {
            std::string logPlainQueries = std::string(ev);
            if (logPlainQueries != "") {
                LOG_PLAIN_QUERIES = true;
                PLAIN_BASELOG = logPlainQueries;
                logPlainQueries += StringFromVal(++counter);

                assert_s(system(("rm -f" + logPlainQueries + "; touch " + logPlainQueries).c_str()) >= 0, "failed to rm -f and touch " + logPlainQueries);

                std::ofstream * PLAIN_LOG =
                    new std::ofstream(logPlainQueries, std::ios_base::app);
                LOG(wrapper) << "proxy logs plain queries at " << logPlainQueries;
                assert_s(PLAIN_LOG != NULL, "could not create file " + logPlainQueries);
                clients[client]->PLAIN_LOG = PLAIN_LOG;
            } else {
                LOG_PLAIN_QUERIES = false;
            }
        }



    } else {
        if (LOG_PLAIN_QUERIES) {
            std::string logPlainQueries =
                PLAIN_BASELOG+StringFromVal(++counter);
            assert_s(system((" touch " + logPlainQueries).c_str()) >= 0, "failed to remove or touch plain log");
            LOG(wrapper) << "proxy logs plain queries at " << logPlainQueries;

            std::ofstream * PLAIN_LOG =
                new std::ofstream(logPlainQueries, std::ios_base::app);
            assert_s(PLAIN_LOG != NULL, "could not create file " + logPlainQueries);
            clients[client]->PLAIN_LOG = PLAIN_LOG;
        }
    }



    return 0;
}

static int
disconnect(lua_State *L)
{
    ANON_REGION(__func__, &perf_cg);
    scoped_lock l(&big_lock);
    assert(0 == mysql_thread_init());

    std::string client = xlua_tolstring(L, 1);
    if (clients.find(client) == clients.end())
        return 0;

    LOG(wrapper) << "disconnect " << client;
    delete clients[client];
    clients.erase(client);

    return 0;
}

static int
rewrite(lua_State *L)
{
    ANON_REGION(__func__, &perf_cg);
    scoped_lock l(&big_lock);
    assert(0 == mysql_thread_init());

    std::string client = xlua_tolstring(L, 1);
    if (clients.find(client) == clients.end())
        return 0;

    std::string query = xlua_tolstring(L, 2);

    //clients[client]->considered = true;

    std::list<std::string> new_queries;


    t.lap_ms();
    if (EXECUTE_QUERIES) {
        if (!DO_CRYPT) {
            new_queries.push_back(query);
        } else {
            try {
                // TODO: May need to set current db in ProxyState.
                QueryRewrite rew = r->rewrite(query);
		new_queries = rew.queries;
		clients[client]->rmeta = rew.rmeta;
		clients[client]->considered = rew.wasRew;
                //cerr << "query: " << *new_queries.begin() << " considered ? " << clients[client]->considered << "\n";
            } catch (CryptDBError &e) {
                LOG(wrapper) << "cannot rewrite " << query << ": " << e.msg;
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }
        }
    }

    if (LOG_PLAIN_QUERIES) {
        *(clients[client]->PLAIN_LOG) << query << "\n";
    }

    lua_pushboolean(L, clients[client]->considered);

    lua_createtable(L, (int) new_queries.size(), 0);
    int top = lua_gettop(L);
    int index = 1;
    for (auto it = new_queries.begin(); it != new_queries.end(); it++) {
        xlua_pushlstring(L, *it);
        lua_rawseti(L, top, index);
        index++;
    }

    clients[client]->last_query = query;
    return 2;
}

static int
decrypt(lua_State *L)
{
    ANON_REGION(__func__, &perf_cg);
    scoped_lock l(&big_lock);
    assert(0 == mysql_thread_init());

    THD *thd = (THD*) create_embedded_thd(0);
    auto thd_cleanup = cleanup([&]
        {
            thd->clear_data_list();
            thd->store_globals();
            thd->unlink();
            delete thd;
        });

    std::string client = xlua_tolstring(L, 1);
    if (clients.find(client) == clients.end())
        return 0;

    ResType res;

    /* iterate over the fields argument */
    lua_pushnil(L);
    while (lua_next(L, 2)) {
        if (!lua_istable(L, -1))
            LOG(warn) << "mismatch";

        lua_pushnil(L);
        while (lua_next(L, -2)) {
            std::string k = xlua_tolstring(L, -2);
            if (k == "name")
                res.names.push_back(xlua_tolstring(L, -1));
            else if (k == "type")
                res.types.push_back((enum_field_types) luaL_checkint(L, -1));
            else
                LOG(warn) << "unknown key " << k;
            lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }

    assert(res.names.size() == res.types.size());

    /* iterate over the rows argument */
    lua_pushnil(L);
    while (lua_next(L, 3)) {
        if (!lua_istable(L, -1))
            LOG(warn) << "mismatch";

        /* initialize all items to NULL, since Lua skips nil array entries */
        std::vector<Item *> row(res.types.size());

        lua_pushnil(L);
        while (lua_next(L, -2)) {
            int key = luaL_checkint(L, -2) - 1;
            assert(key >= 0 && (uint) key < res.types.size());
            std::string data = xlua_tolstring(L, -1);
            Item * value = make_item(data, res.types[key]);
            row[key] = value;
            lua_pop(L, 1);
        }

        res.rows.push_back(row);
        lua_pop(L, 1);
    }

    ResType rd;
    std::cerr << "do crypt is " << DO_CRYPT << " and considered is " << clients[client]->considered << "\n";
    if (!DO_CRYPT || !clients[client]->considered) {
        rd = res;
    } else {
        try {
            rd = r->decryptResults(res, clients[client]->rmeta);
        }
        catch(CryptDBError e) {
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }

    /* return decrypted result set */
    lua_createtable(L, (int) rd.names.size(), 0);
    int t_fields = lua_gettop(L);
    for (uint i = 0; i < rd.names.size(); i++) {
        lua_createtable(L, 0, 2);
        int t_field = lua_gettop(L);

        /* set name for field */
        xlua_pushlstring(L, rd.names[i]);
        lua_setfield(L, t_field, "name");

        /* set type for field */
        lua_pushinteger(L, rd.types[i]);
        lua_setfield(L, t_field, "type");

        /* insert field element into fields table at i+1 */
        lua_rawseti(L, t_fields, i+1);
    }

    lua_createtable(L, (int) rd.rows.size(), 0);
    int t_rows = lua_gettop(L);
    for (uint i = 0; i < rd.rows.size(); i++) {
        lua_createtable(L, (int) rd.rows[i].size(), 0);
        int t_row = lua_gettop(L);

        for (uint j = 0; j < rd.rows[i].size(); j++) {
            if (rd.rows[i][j] == NULL) {
                lua_pushnil(L);
            } else {
                xlua_pushlstring(L, ItemToString(rd.rows[i][j]));
            }
            lua_rawseti(L, t_row, j+1);
        }

        lua_rawseti(L, t_rows, i+1);
    }

    //cerr << clients[client]->last_query << " took (too long) " << t.lap_ms() << endl;;
    return 2;
}

static const struct luaL_reg
cryptdb_lib[] = {
#define F(n) { #n, n }
    F(connect),
    F(disconnect),
    F(rewrite),
    F(decrypt),
    { 0, 0 },
};

extern "C" int lua_cryptdb_init(lua_State *L);

int
lua_cryptdb_init(lua_State *L)
{
    luaL_openlib(L, "CryptDB", cryptdb_lib, 0);
    return 1;
}
