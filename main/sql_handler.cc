#include <main/sql_handler.hh>

AbstractAnything::~AbstractAnything() {}

AbstractQueryExecutor::~AbstractQueryExecutor() {}

void AbstractQueryExecutor::
genericPreamble(bool staleness, NextParams &nparams)
{
    // We handle before any queries because a failed query
    // may stale the database during recovery and then
    // we'd have to handle there as well.
    nparams.schema_cache->updateStaleness(nparams.e_conn, staleness);

    // FIXME: add flag so we only set this if the query actually needs the
    // embedded database
    TEST_TextMessageError(
        lowLevelSetCurrentDatabase(nparams.e_conn, nparams.default_db),
        "failed to set the embedded database to " + nparams.default_db + ";"
        " your client may be in an unrecoverable bad loop"
        " so consider restarting just the _client_. this can happen"
        " if you tell your client to connect to a database that exists"
        " but was not created through cryptdb.");

    return;
}

std::pair<AbstractQueryExecutor::ResultType, AbstractAnything *> SimpleExecutor::
next(const ResType &res, NextParams &nparams)
{
    genericPreamble(false, nparams);

    crFinishWithQuery(this->query);
}


