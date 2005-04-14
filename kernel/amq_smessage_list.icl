<?xml?>
<class
    name      = "amq_smessage_list"
    comment   = "Implements list head container for amq_smessage"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    This class implements list head container for amq_smessage
</doc>

<inherit class = "ipr_list_head" />

<import class = "amq_smessage"  />
<option name = "childname" value = "amq_smessage" />
<option name = "childtype" value = "amq_smessage_t" />

<context>
    amq_smessage_t
        amq_smessage;                   /*  Self starts with child object    */
</context>

<method name = "commit" template = "function">
    <doc>
    Commits a set of messages waiting on the list.  Should be done inside
    a database transaction and before a full dispatch of all queues.
    </doc>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <local>
    amq_smessage_t
        *current,
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        /*  TODO
            topics  */
        amq_queue_accept (message->queue, NULL, message, txn);
        current = message;              /*  Double-check accept worked       */
        amq_smessage_destroy (&message);
        message = amq_smessage_list_first (self);
        assert (message != current);
    }
</method>

<method name = "rollback" template = "function">
    <doc>
    Rolls-back all messages waiting to be written for the transaction.
    </doc>
    <local>
    amq_smessage_t
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        amq_smessage_purge (message);
        amq_smessage_destroy (&message);
        message = amq_smessage_list_first (self);
    }
</method>

<method name = "selftest" />

</class>
