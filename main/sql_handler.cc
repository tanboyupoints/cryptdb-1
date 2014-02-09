#include <main/sql_handler.hh>
#include <util/yield.hpp>

AbstractAnything::~AbstractAnything() {}

AbstractQueryExecutor::~AbstractQueryExecutor() {}

void AbstractQueryExecutor::
genericPreamble(bool staleness, const NextParams &nparams)
{
    // We handle before any queries because a failed query
    // may stale the database during recovery and then
    // we'd have to handle there as well.
    nparams.ps.getSchemaCache().updateStaleness(nparams.ps.getEConn(),
                                                staleness);

    // FIXME: add flag so we only set this if the query actually needs the
    // embedded database
    TEST_ErrPkt(
        lowLevelSetCurrentDatabase(nparams.ps.getEConn(), nparams.default_db),
        "failed to set the embedded database to " + nparams.default_db + ";"
        " your client may be in an unrecoverable bad loop"
        " so consider restarting just the _client_. this can happen"
        " if you tell your client to connect to a database that exists"
        " but was not created through cryptdb.");

    return;
}

std::pair<AbstractQueryExecutor::ResultType, AbstractAnything *> SimpleExecutor::
next(const ResType &res, const NextParams &nparams)
{
    reenter(this->corot) {
        genericPreamble(false, nparams);

        yield return CR_QUERY_RESULTS(nparams.original_query);
    }

    assert(false);
}

std::pair<AbstractQueryExecutor::ResultType, AbstractAnything *> NoOpExecutor::
next(const ResType &res, const NextParams &nparams)
{
    reenter(this->corot) {
        genericPreamble(false, nparams);

        yield return CR_QUERY_RESULTS("DO 0;");
    }

    assert(false);
}


