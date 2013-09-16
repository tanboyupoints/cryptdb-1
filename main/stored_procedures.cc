#include <string>

#include <main/stored_procedures.hh>
#include <main/macro_util.hh>

static bool
addStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    const std::vector<std::string> add_procs({
        // ---------------------------------------
        //   def currentTransactionID(integer id)
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

        // ---------------------------------------
        //       def lazyTransactionBegin()
        // ---------------------------------------
        /*
            The corner case for this procedure is a call graph like this.
            >>> START TRANSACTION;
            >>> UPDATE t SET x = x + 1;
                > lazyTransactionBegin();
                > ...
                > lazyTransactionCommit();
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
        " CREATE PROCEDURE cryptdbtest.lazyTransactionBegin ()"
        " BEGIN"
        "   DECLARE status BOOLEAN;"
        "   DECLARE old_transaction_id VARCHAR(20);"
        "   DECLARE new_transaction_id VARCHAR(20);"

        "   CALL currentTransactionID(@old_transaction_id);"

            // Start a transaction if necessary and record it's origin"
            // (this proc or the user)"
        "   IF @old_transaction_id IS NULL THEN"
        "       START TRANSACTION;"

                // This propagates transaction metadata into
                // INFORMATION_SCHEMA
        "       SELECT NULL FROM TransactionHelper;"

                // We can't set the transaction id here because
                // INFORMATION_SCHEMA.INNODB_TRXs doesn't reflect our
                // efforts until we return from this procedure.
        "       INSERT INTO TransactionHelper (thread_id, do_commit)"
        "            VALUES ((SELECT CONNECTION_ID()), TRUE);"
        "   ELSE"
        "       INSERT INTO TransactionHelper (thread_id, do_commit)"
        "            VALUES ((SELECT CONNECTION_ID()), FALSE);"
        "   END IF;"
        " END",

        // ---------------------------------------
        //         def hackTransaction();
        // ---------------------------------------
        " CREATE PROCEDURE cryptdbtest.hackTransaction ()"
        " BEGIN"
        "    DECLARE transaction_id VARCHAR(20);"

        "    CALL currentTransactionID(@transaction_id);"
             // This UPDATE will potentially modify failure artefacts
             // as lazyTransactionCommit() may not have had the
             // opportunity to remove them from the table.
        "    UPDATE TransactionHelper SET trx_id = @transaction_id"
        "     WHERE thread_id = (SELECT CONNECTION_ID())"
        "       AND trx_id IS NULL;"
        " END",

        // ---------------------------------------
        //      def lazyTransactionCommit()
        // ---------------------------------------
        " CREATE PROCEDURE cryptdbtest.lazyTransactionCommit ()"
        " BEGIN"
        "    DECLARE status BOOLEAN;"
        "    DECLARE transaction_id INTEGER;"

             // ORDER BY + LIMIT is necessary as we must deal with
             // artefacts if lazyTransactionCommit() fails to execute
             // for a prior query.
        "    CALL currentTransactionID(@transaction_id);"
        "    SELECT do_commit INTO status FROM TransactionHelper"
        "     WHERE trx_id = @transaction_id"
        "       AND thread_id = (SELECT CONNECTION_ID())"
        "  ORDER BY id DESC"
        "     LIMIT 1;"

        "    DELETE FROM TransactionHelper"
        "          WHERE trx_id = @transaction_id;"

        "   IF TRUE = status THEN"
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
                    "   cryptdbtest.lazyTransactionBegin;",
                    "DROP PROCEDURE IF EXISTS"
                    "   cryptdbtest.hackTransaction;",
                    "DROP PROCEDURE IF EXISTS"
                    "   cryptdbtest.lazyTransactionCommit;"});

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

