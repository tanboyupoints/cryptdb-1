-- This code is ugly; but it is largely an attempt to deal with disparate
-- datastructures that are essentially isomorphic with a minimal amount
-- of looping.

-- wordpress testing hack
local new_session = true

package.cpath = package.cpath .. ";/usr/local/lib/lua/5.1/?.so"
local luasql = assert(require("luasql.mysql"))
local proto = assert(require("mysql.proto"))

local LOG_FILE = "logs/double.log"
local env = nil
local cryptdb_connection = nil

function connect_server()
    print("Double Connection.")
    -- initialize cryptdb connection
    env = luasql.mysql()
    if env then
        cryptdb_connection = env:connect("", "root", "letmein", "127.0.0.1", 3307)
    end

    -- open log file
    io.output(LOG_FILE)
end

function disconnect_client()
    if cryptdb_connection then
        cryptdb_connection:close()
    end

    if env then
        env:close()
    end
end

function read_query(packet)
    local query = string.sub(packet, 2)
    if string.byte(packet) == proxy.COM_INIT_DB then
        query = "USE " .. query
    end
    packet = string.char(proxy.COM_QUERY) .. query

    -- HACK: that turns off strict mode for wordpress because
    -- the testing database runs in strict mode.
    proxy.queries:append(42, packet, {resultset_is_needed = true})
    if true == new_session then
        mode = "SET @@session.sql_mode := ''"
        proxy.queries:append(1337, string.char(proxy.COM_QUERY) .. mode,
                             {resultset_is_needed = true})
        new_session = false
    end

    return proxy.PROXY_SEND_QUERY
end

function read_query_result(inj)
    local client_name = proxy.connection.client.src.name
    local query = string.sub(inj.query, 2)

    if nil == cryptdb_connection then
        io.write(create_log_entry(client_name, query, false, false,
                                  "no cryptb connection"))
        io.flush()
        return
    end

    -- > somemtimes this is a cursor (ie SELECT), sometimes it's nil (query
    --   error), sometimes it's number of rows affected by command
    local out_status = nil
    cursor = cryptdb_connection:execute(query)
    local cryptdb_error = not cursor
    local regular_error =  proxy.MYSQLD_PACKET_ERR ==
                                inj.resultset.query_status

    if regular_error or cryptdb_error then
        out_status = "error"
    elseif "number" == type(cursor) then
        -- WARN: this is always going to give a nonsensical result
        -- for UPDATE and DDL queries.
        out_status = get_match_text(cursor == inj.resultset.affected_rows)
    else
        -- compare field names size
        -- > can get false match where fields with different names have
        --   same elements
        if (#cursor:getcolnames() ~= #inj.resultset.fields) then
            out_status = get_match_text(false)
        else
            -- do the naive comparison while gathering the results,
            -- then if it fails, do the slow comparison
            local cryptdb_results = {}
            local regular_results = {}

            -- HACK/CARE: double iteration
            local index = 1
            local no_match = false
            local no_fast_match = false
            for regular_row in inj.resultset.rows do
                -- will only be nil after last (not strictly true)
                local cryptdb_row = cursor:fetch({}, "n")
                if nil == cryptdb_row then
                    no_match = true
                    no_fast_match = true
                    break
                elseif false == no_fast_match then
                    -- don't do table_test if we already know we dont have
                    -- a fast match
                    if false == table_test(cryptdb_row, regular_row) then
                        no_fast_match = true
                    end
                end

                regular_results[index] = regular_row
                cryptdb_results[index] = cryptdb_row
                index = index + 1
            end

            if true == no_match then
                out_status = get_match_text(false)
            elseif false == no_fast_match then
                out_status = get_match_text(true)
            else
                -- do slow, unordered matching
                local test = slow_test(regular_results, cryptdb_results)
                out_status = get_match_text(test)
            end
        end
    end

    io.write(create_log_entry(client_name, query, cryptdb_error,
                              regular_error, out_status))
    io.flush()
end

function create_log_entry(client, query, cryptdb_error, regular_error,
                          status)
    return client .. "," .. csv_escape(query) .. "," .. os.date("%c") .. "," .. ppbool(cryptdb_error) .. "," .. ppbool(regular_error) .. "," .. status .. "\n"
end

function table_test(results_a, results_b)
    if table.getn(results_a) ~= table.getn(results_b) then
        return false
    end

    for i = 1, #results_a do
        if results_a[i] ~= results_b[i] then
            return false
        end
    end

    return true
end

-- FIXME: Can get 2x speed up by removing matched elements from the inner
-- lookup array.
function slow_test(results_a, results_b)
    if table.getn(results_a) ~= table.getn(results_b) then
        return false
    end

    for a_index = 1, #results_a do
        local matched = false
        for b_index = 1, #results_b do
            if table_test(results_a[a_index], results_b[b_index]) then
                matched = true
                break
            end
        end

        if false == matched then
            return false
        end
    end

    return true
end

function get_match_text(b)
    if true == b then
        return "matched"
    else
        return "diverged"
    end
end

function ppbool(b)
    if true == b then
        return "true"
    else
        return "false"
    end
end

-- FIXME: Implement this if it actually matters; will make code slower.
-- VAPORWARE
function csv_escape(string)
    return string
end
