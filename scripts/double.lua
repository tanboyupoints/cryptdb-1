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

    proxy.queries:append(42, packet, {resultset_is_needed = true})
    return proxy.PROXY_SEND_QUERY
end

function read_query_result(inj)
    local client_name = proxy.connection.client.src.name
    local query = string.sub(inj.query, 2)

    proxy.response.type          = proxy.MYSQLD_PACKET_OK
    proxy.response.affected_rows = inj.resultset.affected_rows
    proxy.response.insert_id     = inj.resultset.insert_id

    if nil == cryptdb_connection then
        io.write(create_log_entry(client_name, query, false, false,
                                  "no cryptb connection"))
        io.flush()
        return proxy.PROXY_SEND_RESULT
    end

    -- > somemtimes this is a cursor (ie SELECT), sometimes it's nil (query
    --   error), sometimes it's number of rows affected by command
    local out_status = nil
    cursor = assert(cryptdb_connection:execute(query))
    local cryptdb_error = not cursor
    local regular_error
    if proxy.MYSQLD_PACKET_ERR == inj.resultset.query_status then
        regular_error = true
    else
        regular_error = false
    end

    if cryptdb_error or regular_error then
        proxy.response.type = proxy.MYSQLD_PACKET_ERR
        local err = proto.from_err_packet(inj.resultset.raw)
        proxy.response.errmsg = err.msg
        proxy.response.errcode = err.errcode
        proxy.response.sqlstate = err.sqlstate

        out_status = "error"
    elseif "number" == type(cursor) then
        -- WARN: this is always going to give the wrong result
        -- for UPDATE.
        out_status = get_match_text(cursor == inj.resultset.affected_rows)
    else
        -- do the naive comparison while gathering the results,
        -- then if it fails, do the slow comparison

        local cryptdb_fields = cursor:getcolnames()
        local regular_fields = {}
        for i = 1, #inj.resultset.fields do
            regular_fields[i] = inj.resultset.fields[i]
        end

        -- FIXME: Does this comparison do as desired?
        if cryptdb_fields ~= regular_fields then
            out_status = get_match_text(false)
        else
            local cryptdb_results = {}
            local regular_results = {}

            local index = 1
            -- HACK/CARE: double iteration.
            for regular_row in inj.resultset.rows do
                -- will be nil after last
                local cryptdb_row = cursor:fetch(cryptdb_results, "n")
                -- FIXME: This comparison is suspect as well.
                if nil == cryptdb_row or cryptdb_row ~= regular_row then
                    io.write(create_log_entry(client_name, query,
                                              cryptdb_error, regular_error,
                                              get_match_text(false)))
                    io.flush()
                    return proxy.PROXY_SEND_RESULT
                end

                regular_results[index] = regular_row
                index = index + 1
            end

            local after_row = cursor:fetch(cryptdb_results, "n")
            if nil ~= after_row then
                -- cryptdb had more rows than the regular database
                out_status = get_match_text(false)
            else
                -- no errors, same fields, same number of rows, but
                -- rows in different orders; do the slow unordered
                -- comparison
                local test = slow_test(regular_results, cryptdb_results)
                out_status = get_match_text(test)
            end
        end
    end

    io.write(create_log_entry(client_name, query, cryptdb_error,
                              regular_error, out_status))
    io.flush()

    return proxy.PROXY_SEND_RESULT
end

function create_log_entry(client, query, cryptdb_error, regular_error,
                          status)
    return client .. "," .. query .. "," .. os.date("%c") .. "," .. ppbool(cryptdb_error) .. "," .. ppbool(regular_error) .. "," .. status .. "\n"
end

function naive_test(results_a, results_b)
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
    for a in results_a do
        local matched = false
        for b in results_b do
            if a == b then
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

function table_length(t)
    local count = 0
    for _ in paris(t) do count = count + 1 end
    return count
end

