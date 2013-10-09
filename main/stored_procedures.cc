#include <string>

#include <main/stored_procedures.hh>
#include <main/macro_util.hh>
#include <main/metadata_tables.hh>

static bool
addStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    const std::string current_transaction_id =
        MetaData::Proc::currentTransactionID();
    const std::string hom_addition_transaction =
        MetaData::Proc::homAdditionTransaction();
    const std::string adjust_onion =
        MetaData::Proc::adjustOnion();
    const std::string remote_completion_table =
        MetaData::Table::remoteQueryCompletion();

    const std::vector<std::string> add_procs({
        // ---------------------------------------
        //      def currentTransactionID(id)
        // ---------------------------------------
        " CREATE PROCEDURE " + current_transaction_id +
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
            homAdditionTransction(). This should lead to consistent,
            expected behavior provided (*) is correct rationale.
        */
        " CREATE PROCEDURE " + hom_addition_transaction +
        "       (IN delete_query VARCHAR(50000),"
        "        IN insert_query VARCHAR(50000))"
        " BEGIN"
        "   DECLARE old_transaction_id VARCHAR(20);"

        "   CALL " + current_transaction_id + " (@old_transaction_id);"

            // Start a transaction if necessary.
        "   IF @old_transaction_id IS NULL THEN"
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
        " END",

        // ---------------------------------------
        //         def adjustOnion(id)
        // ---------------------------------------
        // NOTE: If we need N queries, we will have to use an additional
        // table + cursors.
        " CREATE PROCEDURE " + adjust_onion +
        "       (IN embedded_completion_id INTEGER,"
        "        IN adjust_query0 VARCHAR(500),"
        "        IN adjust_query1 VARCHAR(500))"
        " BEGIN"
        "   DECLARE old_transaction_id VARCHAR(20);"
        "   DECLARE reissue BOOLEAN;"

            // Are we in a transaction?
        "   CALL " + current_transaction_id + "(@old_transaction_id);"

            // if not, we will want to reissue the original query
        "   IF @old_transaction_id IS NULL THEN"
        "       SET @reissue = TRUE;"
        "   ELSE"
        "       SET @reissue = FALSE;"
        "   END IF;"

            // cancel pending transaction
        "   ROLLBACK;"

            // start a new one for this proc call
        "   START TRANSACTION;"

            // first onion adjustment
        "   SET @query = adjust_query0;"
        "   PREPARE aq0 FROM @query;"
        "   EXECUTE aq0;"

            // (possibly) second onion adjustment
        "   SET @query = adjust_query1;"
        "   PREPARE aq1 FROM @query;"
        "   EXECUTE aq1;"

            // update metadata used for recovery
        "   INSERT INTO " + remote_completion_table +
        "       (complete, embedded_completion_id,  reissue) VALUES"
        "       (TRUE,     embedded_completion_id, @reissue);"

        "   COMMIT;"
        " END"});

    for (auto it : add_procs) {
        RETURN_FALSE_IF_FALSE(conn->execute(it));
    }

    return true;
}

static bool
dropStoredProcedures(const std::unique_ptr<Connect> &conn)
{
    const std::string current_transaction_id =
        MetaData::Proc::currentTransactionID();
    const std::string hom_addition_transaction =
        MetaData::Proc::homAdditionTransaction();
    const std::string adjust_onion =
        MetaData::Proc::adjustOnion();

    const std::vector<std::string>
        drop_procs({"DROP PROCEDURE IF EXISTS " +
                        current_transaction_id,
                    "DROP PROCEDURE IF EXISTS " +
                        hom_addition_transaction,
                    "DROP PROCEDURE IF EXISTS " +
                        adjust_onion});

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

