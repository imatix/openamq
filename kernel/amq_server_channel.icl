<?xml?>
<class
    name      = "amq_server_channel"
    comment   = "AMQ server channel class"
    version   = "1.0"
    script    = "smt_object_gen"
    >
<doc>
This class implements the AMQ server channel class.  The channel holds
a set of consumers managed both as a list and as a lookup table.  The
maximum number of consumers per channel is set at compile time.
</doc>

<inherit class = "asl_server_channel" />
<option name = "basename" value = "amq_server" />

<context>
    amq_consumer_by_channel_t
        *consumer_list;                 //  List of consumers for channel
    ipr_index_t
        *consumer_table;                //  Fuzzy table of consumers
</context>

<method name = "new">
    self->consumer_table = ipr_index_new ();
    self->consumer_list  = amq_consumer_by_channel_new ();
</method>

<method name = "destroy">
    <action>
    amq_consumer_t
        *consumer;                      //  Consumer in index table
    int
        consumer_nbr;

    //  Index table has a link on each consumer, so unlink these
    for (consumer_nbr = 1; consumer_nbr < IPR_INDEX_MAX; consumer_nbr++) {
        consumer = self->consumer_table->data [consumer_nbr];
        if (consumer)
            amq_consumer_unlink (&consumer);
    }
    //  We destroy consumers by asking the respective queues
    while ((consumer = amq_consumer_by_channel_pop (self->consumer_list))) {
        if (amq_queue_cancel (consumer->queue, &consumer, FALSE))
            amq_consumer_destroy (&consumer);
    }
    //  Now destroy containers
    ipr_index_destroy (&self->consumer_table);
    amq_consumer_by_channel_destroy (&self->consumer_list);
    </action>
</method>

<method name = "flow" template = "async function" async = "1">
    <doc>
    Implements the channel.flow method.  When a channel is restarted,
    we dispatch the channel's virtual host.
    </doc>
    <argument name = "active" type = "Bool">Active consumer?</argument>
    //
    <action>
    amq_consumer_t
        *consumer;                      //  Consumer object reference

    self->active = active;
    consumer = amq_consumer_by_channel_first (self->consumer_list);
    while (consumer) {
        amq_queue_flow (consumer->queue, consumer, active);
        consumer = amq_consumer_by_channel_next (&consumer);
    }
    if (self->active)
        amq_vhost_dispatch (self->connection->vhost);
    </action>
</method>

<method name = "consume" template = "async function" async = "1">
    <doc>
    Creates a new channel consumer as specified.  Mechanism is as
    follows: server_channel creates consumer and inserts into index
    table. It then attaches to own consumer list and sends to queue
    so that queue can add consumer to its consumer list.
    </doc>
    <argument name = "queue name"     type = "char *">Queue name</argument>
    <argument name = "class id"       type = "int"   >Consumer content class</argument>
    <argument name = "prefetch size"  type = "qbyte" >Prefetch size</argument>
    <argument name = "prefetch count" type = "dbyte" >Prefetch count</argument>
    <argument name = "no local"       type = "Bool"  >Don't want own messages</argument>
    <argument name = "auto ack"       type = "Bool"  >Auto acknowledge messages</argument>
    <argument name = "exclusive"      type = "Bool"  >Exclusive access?</argument>
    //
    <action>
    amq_queue_t
        *queue;
    amq_consumer_t
        *consumer;
    int
        tag;                            //  New consumer tag

    //  Look for queue as specified, if it exists, create consumer
    queue = amq_queue_table_search (amq_vhost->queue_table, queue_name);
    if (queue) {
        //  Create and configure the consumer object
        consumer = amq_consumer_new (
            self, queue, class_id,
            prefetch_size, prefetch_count, no_local, auto_ack, exclusive);

        tag = ipr_index_insert (self->consumer_table, consumer);
        if (tag) {
            consumer->tag = (dbyte) tag;
            amq_consumer_by_channel_queue (self->consumer_list, consumer);
            amq_queue_consume (queue, consumer, self->active);
        }
        else {
            amq_server_connection_exception (self->connection, ASL_RESOURCE_ERROR,
                "Too many consumers for channel");
            amq_consumer_destroy (&consumer);
        }
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (self, ASL_NOT_FOUND, "No such queue defined");
    </action>
</method>

<method name = "cancel" template = "async function" async = "1">
    <doc>
    Cancels channel consumer specified by tag.  May be called either
    from method handler - sync true - or from queue agent - sync false.
    Mechanism is as follows: removes consumer from index table and from
    own consumer list.  If sync, then sends to queue so that queue can
    remove from own consumer list.  If not sync, destroys consumer.
    </doc>
    <argument name = "tag"  type = "int" >Consumer tag</argument>
    <argument name = "sync" type = "Bool">Are we talking to a client?</argument>
    //
    <action>
    amq_consumer_t
        *consumer = NULL;               //  Consumer reference

    if (tag > 0 && tag <= IPR_INDEX_MAX)
        consumer = self->consumer_table->data [tag];

    if (consumer) {
        ipr_index_delete (self->consumer_table, tag);
        amq_consumer_by_channel_remove (consumer);
        if (sync) {
            //  Pass to queue to do the final honours
            amq_queue_cancel (consumer->queue, &consumer, TRUE);
        }
        else {
            //  Consumer must have been removed from its per-queue list
            assert (consumer->by_queue_next == consumer);
            amq_consumer_destroy (&consumer);
        }
    }
    else
    if (sync)
        amq_server_channel_close (self, ASL_NOT_FOUND, "Not a valid consumer tag");
    </action>
</method>

<method name = "alive" return = "rc">
    <argument name = "self" type = "amq_server_channel_t *">Reference to channel</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <doc>
    Returns TRUE if the channel appears to be alive.  Accepts
    a null channel reference (which is considered as 'not alive').
    </doc>
    //
    if (self
    &&  self->zombie == FALSE
    &&  self->connection
    &&  self->connection->thread
    &&  self->connection->suspended == FALSE)
        rc = TRUE;
    else
        rc = FALSE;
</method>

<method name = "selftest">
    <local>
    amq_server_channel_table_t
        *table;
    amq_server_channel_t
        *channel;
    int
        count,
        channel_nbr;
    </local>

    smt_os_thread_initialise ();
    table = amq_server_channel_table_new ();

    //  Let's try some edge cases
    channel = amq_server_channel_new (table, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE - 1, NULL);
    amq_server_channel_destroy (&channel);
    smt_os_thread_wait (0);

    channel = amq_server_channel_new (table, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE, NULL);
    amq_server_channel_destroy (&channel);
    smt_os_thread_wait (0);

    channel = amq_server_channel_new (table, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE + 1, NULL);
    amq_server_channel_destroy (&channel);

    //  Now some random table bashing
    for (count = 0; count < 2000; count++) {
        channel_nbr = randomof (0xffff);
        channel = amq_server_channel_new (table, channel_nbr, NULL);
        amq_server_channel_destroy (&channel);
        smt_os_thread_wait (0);
    }
    amq_server_channel_table_destroy (&table);
</method>

</class>
