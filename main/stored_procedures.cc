#include <string>

#include <main/stored_procedures.hh>
// FIXME: Path.

static std::string
readFile(const std::string &path)
{
    std::ifstream file(path);
    std::stringstream stream_contents;
    stream_contents << file.rdbuf();

    return stream_contents.str();
}

static bool
addStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    // FIXME: path.
    const std::string path = "/home/burrows/code/cryptdb/main/transaction.sql";
    const std::string contents = readFile(path);

    return conn->execute(contents);
}

static bool
dropStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    // FIXME: path.
    const std::string path =
        "/home/burrows/code/cryptdb/main/transaction_drop.sql";
    const std::string contents = readFile(path);

    return conn->execute(contents);
}

// FIXME: Replace assert with RETURN_FALSE_IF_FALSE once we've debugged.
bool
loadStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    assert(dropStoredProcedures(conn));
    assert(addStoredProcedures(conn));

    return true;
}

