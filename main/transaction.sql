DELIMITER //

CREATE PROCEDURE currentTransactionID (OUT out_id VARCHAR(20))
BEGIN
    SELECT trx_id INTO out_id FROM INFORMATION_SCHEMA.INNODB_TRX
     WHERE INFORMATION_SCHEMA.INNODB_TRX.TRX_MYSQL_THREAD_ID =
        (SELECT CONNECTION_ID());
END//

CREATE PROCEDURE lazyTransactionBegin ()
BEGIN
    /*
        The corner case for this procedure is a call graph such as this.
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
        INFORMATION_SCHEMA.INNODB_TRX table until a query is issued against
        a table that supports transactions.

        So it follows that in the above sequence the 'START TRANSACTION'
        issued by the user will essentially be ignored in favor of a
        transaction started by lazyTransactionBegin(). This should lead to
        consistent, expected behavior provided (*) is correct
        rationale.
    */
    DECLARE status BOOLEAN;
    DECLARE old_transaction_id VARCHAR(20);
    DECLARE new_transaction_id VARCHAR(20);

    CALL currentTransactionID(@old_transaction_id);
    SELECT trx_state = 'RUNNING' INTO status
                                 FROM INFORMATION_SCHEMA.INNODB_TRX
     WHERE INFORMATION_SCHEMA.INNODB_TRX.TRX_ID = @old_transaction_id;

    -- Start a transaction if necessary and record it's origin (this proc
    -- or the user)
    IF status IS NULL OR FALSE = status THEN
        START TRANSACTION;

        -- This propagates transaction metadata into INFORMATION_SCHEMA
        -- NOTE: Metadata is not always propagated if we do
        --  > SELECT NULL FROM TransactionHelper;
        SELECT NULL FROM TransactionHelper;

        -- We can't set the transaction id here because
        -- INFORMATION_SCHEMA.INNODB_TRXs doesn't reflect our propagation
        -- until we return from this procedure.
        INSERT INTO TransactionHelper (thread_id, do_commit)
             VALUES ((SELECT CONNECTION_ID()), TRUE);
    ELSE
        INSERT INTO TransactionHelper (thread_id, do_commit)
             VALUES ((SELECT CONNECTION_ID()), FALSE);
    END IF;
END//

CREATE PROCEDURE hackTransaction ()
BEGIN
    DECLARE transaction_id VARCHAR(20);

    CALL currentTransactionID(@transaction_id);
    UPDATE TransactionHelper SET trx_id = @transaction_id
     WHERE thread_id = (SELECT CONNECTION_ID());
END//

CREATE PROCEDURE lazyTransactionCommit ()
BEGIN
    DECLARE status BOOLEAN;
    DECLARE transaction_id INTEGER;

    CALL currentTransactionID(@transaction_id);
    SELECT do_commit INTO status FROM TransactionHelper
     WHERE trx_id = @transaction_id AND thread_id = (SELECT CONNECTION_ID());

    -- We must DELETE this row otherwise hackTransaction will update it again
    -- and the above SELECT will also catch it.
    -- > Alternatively we could ORDER BY id AND LIMIT 1 in hackTransaction
    --   and here.
    DELETE FROM TransactionHelper
          WHERE trx_id = @transaction_id;

    IF TRUE = status THEN
        COMMIT;
    END IF;
END//

DELIMITER ;

