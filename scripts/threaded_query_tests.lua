-- script will likely require root privileges

local COLOR_END         = string.char(27) .. "[0m"
local GREEN             = string.char(27) .. "[1;92m"
local RED               = string.char(27) .. "[1;31m"

local unit_lib =
    assert(package.loadlib("/home/burrows/code/cryptdb/scripts/threaded_query.so",
                           "lua_test_init"))
unit_lib()

local main_lib =
    assert(package.loadlib("/home/burrows/code/cryptdb/scripts/threaded_query.so",
                           "lua_main_init"))
main_lib()

-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
--                Run Tests
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
function main()
    -- unit tests
    TestThreadedQuery.all()

    --
    -- integration tests
    --
    os.execute("service mysql stop")
    os.execute("mysqld --bind-address=127.0.0.1 &")
    os.execute("sleep 3")

    os.execute("mysql -uroot -pletmein -e \"drop database if exists lua_test\"")
    os.execute("mysql -uroot -pletmein -e \"create database if not exists lua_test\"")

    total_integrations      = 0
    passed_integrations     = 0

    total_integrations = total_integrations + 1
    if test_normalQueryExecution() then
        print(GREEN .. "test_normalQueryExecution succeeded!" .. COLOR_END)
        passed_integrations = passed_integrations + 1
    else
        print(RED .. "test_normalQueryExecution failed!" .. COLOR_END)
    end

    total_integrations = total_integrations + 1
    if test_restartedQueryExecution() then
        print(GREEN .."test_restartedQueryExection succeeded!".. COLOR_END)
        passed_integrations = passed_integrations + 1
    else
        print(RED .. "test_restartedQueryExection succeeded!" .. COLOR_END)
    end

    print("\n" ..
          "################################\n" ..
          "  Passed " .. passed_integrations .."/" .. total_integrations ..
                " Integration Tests\n" ..
          "################################\n")

    os.execute("mysql -uroot -pletmein -e \"drop database lua_test\"")
    os.execute("pkill -9 mysqld")
    os.execute("service mysql start")
end

-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
--             Integration Tests
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
-- ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


function test_normalQueryExecution()
    os.execute("mysql -uroot -pletmein -e \"create table lua_test.t (x integer, y integer)\"")
    os.execute("mysql -uroot -pletmein -e \"insert into lua_test.t VALUES (1, 2), (3, 4), (4, 3)\"")

    status, lua_query =
        ThreadedQuery.start("127.0.0.1", "root", "letmein", 3306)
    if not (status and lua_query) then
        return false
    end

    -- do a query
    status = ThreadedQuery.query(lua_query, "SELECT * FROM lua_test.t")
    if not status then
        return false
    end

    status, result = ThreadedQuery.results(lua_query)
    if not (status and type(result) == "table") then
        return false
    end

    -- do it again
    status = ThreadedQuery.query(lua_query, "SELECT * FROM lua_test.t")
    if not assert(status) then
        return false
    end

    status, result = ThreadedQuery.results(lua_query)
    if not (status and type(result) == "table") then
        return false
    end

    -- different query
    status = ThreadedQuery.query(lua_query, "UPDATE lua_test.t SET x = 15")
    if not (status) then
        return false
    end

    status, result = ThreadedQuery.results(lua_query)
    if not (status and type(result) == "number") then
        return false
    end

    -- issue a malformed query
    status = ThreadedQuery.query(lua_query, "SELECT * FROM lua_test.tt")
    if not (not status) then
        return false
    end

    status, result = ThreadedQuery.results(lua_query)
    if not (not status and result == nil) then
        return false
    end

    status = ThreadedQuery.kill(lua_query)
    if not (status) then
        return false
    end

    return true
end

function test_restartedQueryExecution()
    os.execute("mysql -uroot -pletmein -e \"create table lua_test.t2 (x integer, y integer)\"")
    os.execute("mysql -uroot -pletmein -e \"insert into lua_test.t2 VALUES (1, 2), (3, 4), (4, 3)\"")

    os.execute("pkill -9 mysqld")
    -- should fail to connect
    status, lua_query =
        ThreadedQuery.start("127.0.0.1", "root", "letmein", 3306)
    assert(status and lua_query)

    -- try reconnect and fail
    status = ThreadedQuery.query(lua_query, "SELECT * FROM lua_test.t2")
    assert(not status)

    os.execute("mysqld --bind-address=127.0.0.1 &")
    os.execute("sleep 3")

    -- reconnect and succeed, but no data
    status, result = ThreadedQuery.results(lua_query)
    assert(not status and result == nil)

    -- successfully execute query
    status = ThreadedQuery.query(lua_query, "SELECT * FROM lua_test.t2")
    assert(status)

    -- successfully fetch data
    status, result = ThreadedQuery.results(lua_query)
    assert(status and type(result) == "table")

    status = ThreadedQuery.kill(lua_query)
    assert(status)

    return true
end

main()
