<?xml?>
<class
    name      = "amq_dispatch_list"
    comment   = "Implements list head container for amq_dispatch"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class holds the list of dispatched messages, i.e. a messages sent to a
client for which acknowledgements are still pending.
</doc>

<inherit class = "ipr_list_head" />

<import class = "amq_dispatch"  />
<option name = "childname" value = "amq_dispatch" />
<option name = "childtype" value = "amq_dispatch_t" />

<context>
    amq_dispatch_t
        amq_dispatch;                   /*  Self starts with child object    */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
</context>

<method name = "new">
    <argument name = "channel" type = "amq_channel_t *">Parent channel</argument>
    self->channel = channel;
</method>

<method name = "ack" template = "function">
    <doc>
    Acknowledge all messages for channel up to & including message_nbr.
    </doc>
    <argument name = "message_nbr" type = "qbyte" />
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>
    dispatch = amq_dispatch_list_first (self);
    while (dispatch) {
        if (dispatch->message_nbr > message_nbr)
            break;
        dispatch = amq_dispatch_ack (dispatch);
    }
</method>

<method name = "unget" template = "function">
    <argument name = "message_nbr" type = "qbyte" />
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>

    rc = -1;                            /*  Unless found...                  */
    dispatch = amq_dispatch_list_first (self);
    while (dispatch) {
        if (dispatch->message_nbr == message_nbr) {
            amq_dispatch_unget    (dispatch);
            amq_dispatch_destroy (&dispatch);
            rc = 0;                     /*  All's well, message exists       */
            break;
        }
        dispatch = amq_dispatch_list_next (self, dispatch);
    }
    if (rc)
        amq_global_set_error (AMQP_MESSAGE_NOT_FOUND, "No such message held");
</method>

<method name = "commit" template = "function">
    <doc>
    Commits all processed (closed) entries in the dispatched message
    list.  Must be done together with a database commit.
    </doc>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>
    dispatch = amq_dispatch_list_first (self);
    while (dispatch)
        dispatch = amq_dispatch_commit (dispatch, txn);
</method>

<method name = "purge" template = "function">
    <doc>
    Removes all processed (closed) entries from the dispatched message
    list.
    </doc>
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>
    dispatch = amq_dispatch_list_first (self);
    while (dispatch)
        dispatch = amq_dispatch_purge (dispatch);
</method>

<method name = "rollback" template = "function">
    <doc>
    Resets all closed entries on the dispatched message list to 'open'
    so that they can be acknowledged again.  Must be done together with
    a database rollback.
    </doc>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>
    dispatch = amq_dispatch_list_first (self);
    while (dispatch)
        dispatch = amq_dispatch_rollback (dispatch, txn);
</method>

<method name = "restore" template = "function">
    <doc>
    Restores all dispatched messages to their queues; should be done when
    the client closes the connection, and after any commit or rollback.
    </doc>
    <local>
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */
    </local>
    dispatch = amq_dispatch_list_first (self);
    while (dispatch) {
        amq_dispatch_unget (dispatch);
        dispatch = amq_dispatch_list_next (self, dispatch);
    }
</method>

<method name = "selftest" />

</class>
