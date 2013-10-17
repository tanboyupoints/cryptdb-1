luasql = require "luasql.mysql"
local LOG_FILE = "logs/double.log"

function connect_server()
    -- initialize cryptdb connection
    env = assert(luasql.mysql())
    connection = assert(env:connect("", "root", "letmein", "127.0.0.1", 3307))

    -- open log file
    assert(io.output(LOG_FILE))
end

function read_query_result(inj)
    local client_name = proxy.connection.client.src.name

    -- > somemtimes this is a cursor (ie SELECT), sometimes it's nil (query
    --   error), sometimes it's a number indicating the number of rows.
    local out_status = nil
    local cursor = env:execute(inj.query)
    local cryptdb_error = nil == cursor
    local regular_error = proxy.MYSQLD_PACKET_ERR == inj.resultset.query_status
    if cryptdb_error or regular_error then
        out_status = "error"
    elseif "number" == type(cursor) then
        if cursor ~= 0 then
            out_status = "missing cryptdb results"
        else
            -- hopefully the size is zero, otherwise this is slow.
            local zero = true
            for row in inj.resultset.rows do
                zero = false
                break
            end
            out_status = get_match_text(zero)
        end
    else
        -- do the naive comparison while gathering the results,
        -- then if it fails, do the slow comparison

        local cryptdb_fields = cur:getcolnames()
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
                if nil == cryptdb_row or cryptdb_row ~= regular_row then
                    return create_log_entry(client_name, inj.query,
                                            cryptdb_error, regular_error,
                                            get_match_text(false))
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

    io.write(create_log_entry(client_name, inj.query, cryptdb_error,
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

function cleanup()
    connection:close()
    env:close()
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

