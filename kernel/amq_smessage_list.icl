<?xml?>
<class
    name      = "amq_smessage_list"
    comment   = "Implements the list container for amq_smessage"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for amq_smessage
</doc>

<inherit class = "ipr_list_head" >
    <option name = "prefix" value = "list"/>
</inherit>
<import class = "amq_smessage" />
<option name = "childname" value = "amq_smessage" />
<option name = "childtype" value = "amq_smessage_t" />
<option name = "rwlock"    value = "1" />

<method name = "commit" template = "function">
    <doc>
    Commits a set of messages waiting on the list.  Should be done inside
    a database transaction and before a full dispatch of all queues.
    </doc>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <local>
    amq_smessage_t
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        amq_smessage_list_remove (message);
        amq_queue_accept (message->queue, NULL, message, FALSE, txn);
        amq_smessage_destroy (&message);
        message = amq_smessage_list_first (self);
    }
</method>

<method name = "rollback" template = "function">
    <doc>
    Rolls-back all messages waiting to be written for the transaction.
    </doc>
    <local>
    amq_smessage_t
        *message_temp,
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        amq_smessage_purge   (message);
        message_temp = message;
        amq_smessage_unlink  (&message_temp);
        amq_smessage_destroy (&message);
        message = amq_smessage_list_first (self);
    }
</method>

<method name = "selftest" />

</class>
