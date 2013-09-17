#include <string>

#include <main/stored_procedures.hh>
#include <main/macro_util.hh>
#include <main/metadata_tables.hh>

static bool
addStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    const std::vector<std::string> add_procs({
        // ---------------------------------------
        //   def currentTransactionID(id)
        // ---------------------------------------
        " CREATE PROCEDURE cryptdbtest.currentTransactionID"
        "   (OUT out_id VARCHAR(20))"
        " BEGIN"
        "   SELECT trx_id INTO out_id FROM INFORMATION_SCHEMA.INNODB_TRX"
        "    WHERE INFORMATION_SCHEMA.INNODB_TRX.TRX_MYSQL_THREAD_ID ="
        "          (SELECT CONNECTION_ID())"
        "      AND INFORMATION_SCHEMA.INNODB_TRX.TRX_STATE ="
        "          'RUNNING';"
        " END",

        // ----------------------------------------------
        //   def homAdditionTransaction(delete, insert)
        // ----------------------------------------------
        /*
            The corner case for this procedure is a call graph like this.
            >>> START TRANSACTION;
            >>> UPDATE t SET x = x + 1;
                > homAdditionTransaction(...);
            >>> ...
                > ...
                > ...
            >>> COMMIT;

            Where '>>>' are queries issue by user and '>' are CryptDB
            transformations.

            (*) It appers that transactions are not propagated to the
            INFORMATION_SCHEMA.INNODB_TRX table until a query is
            issued against a table that supports transactions.

            So it follows that in the above sequence the
            'START TRANSACTION' issued by the user will essentially be
            ignored in favor of a transaction started by
            lazyTransactionBegin(). This should lead to consistent,
            expected behavior provided (*) is correct rationale.
        */
        " CREATE PROCEDURE cryptdbtest.homAdditionTransaction"
        "       (IN delete_query VARCHAR(50000),"
        "        IN insert_query VARCHAR(50000))"
        " BEGIN"
        "   DECLARE old_transaction_id VARCHAR(20);"
        "   DECLARE where_clause VARCHAR(255);"

        "   CALL currentTransactionID(@old_transaction_id);"

            // Start a transaction if necessary and record it's origin"
            // (this proc or the user)"
        "   IF @old_transaction_id IS NULL THEN"
                // Cancel a transaction like the one described in the
                // corner case, then start our own.
        "       COMMIT;"
        "       START TRANSACTION;"
        "   END IF;"

            // DELETE old values pertaining to WHERE clause
        "   SET @query = delete_query;"
        "   PREPARE dq FROM @query;"
        "   EXECUTE dq;"

            // INSERT new values using subquery from temp table.
        "   SET @query = insert_query;"
        "   PREPARE iq FROM @query;"
        "   EXECUTE iq;"

            // Close our transaction if we started one.
        "   IF @old_transaction_id IS NULL THEN"
        "       COMMIT;"
        "   END IF;"
        " END"});


    for (auto it : add_procs) {
        RETURN_FALSE_IF_FALSE(conn->execute(it));
    }

    return true;
}

static bool
dropStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    const std::vector<std::string>
        drop_procs({"DROP PROCEDURE IF EXISTS"
                    "   cryptdbtest.currentTransactionID;",
                    "DROP PROCEDURE IF EXISTS"
                    "   cryptdbtest.homAdditionTransaction;"});

    for (auto it : drop_procs) {
        RETURN_FALSE_IF_FALSE(conn->execute(it));
    }

    return true;
}

bool
loadStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    RETURN_FALSE_IF_FALSE(dropStoredProcedures(conn));
    RETURN_FALSE_IF_FALSE(addStoredProcedures(conn));

    return true;
}

