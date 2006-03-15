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
    icl_shortstr_t
        cluster_id;                     //  Cluster id for channel
</context>

<method name = "new">
    self->consumer_list = amq_consumer_by_channel_new ();
    icl_shortstr_fmt (self->cluster_id,
        "%s/%s/%d",
        amq_broker? amq_broker->name: "-",
        connection? connection->id: "-",
        self->number);
</method>

<method name = "destroy">
    <action>
    {
    amq_consumer_t
        *consumer;                      //  Consumer in index table

    //  We destroy consumers by asking the respective queues
    while ((consumer = amq_consumer_by_channel_pop (self->consumer_list))) {
        if (amq_queue_cancel (consumer->queue, consumer, FALSE))
            //  If the async cancel failed, destroy the consumer ourselves
            amq_consumer_destroy (&consumer);
    }
    //  Now destroy containers
    amq_consumer_by_channel_destroy (&self->consumer_list);
    }
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
    amq_server_connection_t
        *connection;

    self->active = active;
    consumer = amq_consumer_by_channel_first (self->consumer_list);
    while (consumer) {
        amq_queue_flow (consumer->queue, consumer, active);
        consumer = amq_consumer_by_channel_next (&consumer);
    }
    connection = amq_server_connection_link (self->connection);
    amq_server_agent_channel_flow_ok (connection->thread, self->number, self->active);
    amq_server_connection_unlink (&connection);
    </action>
</method>

<method name = "consume" template = "async function" async = "1">
    <doc>
    Creates a new channel consumer as specified.  Mechanism is as
    follows: server_channel creates consumer, then attaches it to
    own consumer list and sends to queue so that queue can add
    consumer to its consumer list.
    </doc>
    <argument name = "queue"  type = "amq_queue_t *">Queue to consume from</argument>
    <argument name = "method" type = "amq_server_method_t *">Consume method</argument>
    //
    <possess>
    queue  = amq_queue_link (queue);
    method = amq_server_method_link (method);
    </possess>
    <release>
    amq_queue_unlink (&queue);
    amq_server_method_unlink (&method);
    </release>
    //
    <action>
    amq_consumer_t
        *consumer = NULL;
    amq_server_connection_t
        *connection;

    //  Create and configure the consumer object
    connection = amq_server_connection_link (self->connection);
    if (connection) {
        consumer = amq_consumer_new (connection, self, queue, method);
        if (consumer) {
            amq_consumer_by_channel_queue (self->consumer_list, consumer);
            amq_queue_consume (queue, consumer, self->active);
            amq_consumer_unlink (&consumer);
        }
        else
            asl_log_print (amq_broker->alert_log,
                "W: duplicate consumer requested, tag='%s'", method->payload.basic_consume.consumer_tag);
        amq_server_connection_unlink (&connection);
    }
    </action>
</method>

<method name = "cancel" template = "async function" async = "1" on_shutdown = "1">
    <doc>
    Cancels channel consumer specified by tag.  May be called either
    from method handler - sync true - or from queue agent - sync false.
    Mechanism is as follows: removes consumer from index table and from
    own consumer list.  If sync, then sends to queue so that queue can
    remove from own consumer list.  If not sync, destroys consumer.
    </doc>
    <argument name = "tag"  type = "char *">Consumer tag</argument>
    <argument name = "sync" type = "Bool">Are we talking to a client?</argument>
    //
    <action>
    amq_consumer_t
        *consumer = NULL;               //  Consumer reference
    amq_server_connection_t
        *connection;

    connection = amq_server_connection_link (self->connection);
    consumer = amq_consumer_table_search (connection->consumer_table, tag);
    amq_server_connection_unlink (&connection);

    if (consumer) {
        amq_consumer_by_channel_remove (consumer);
        if (sync) {
            //  Pass to queue to do the final honours
            amq_queue_cancel (consumer->queue, consumer, TRUE);
        }
        else {
            //  Consumer must have been removed from its per-queue list
            assert (consumer->by_queue_next == consumer);
            amq_consumer_destroy (&consumer);
        }
    }
    else
    if (sync)
        amq_server_channel_error (self, ASL_NOT_FOUND, "Not a valid consumer tag");
    </action>
</method>

<method name = "error">
    <doc>
    If the channel is alive, closes the channel with the specified
    reply code/text, otherwise prints it to the console.
    </doc>
    <argument name = "self" type = "amq_server_channel_t *">Reference to channel</argument>
    <argument name = "reply code" type = "dbyte" >Error code</argument>
    <argument name = "reply text" type = "char *">Error text</argument>
    if (self)
        amq_server_channel_close (self, reply_code, reply_text);
    else
        asl_log_print (amq_broker->alert_log,
            "E: channel exception: (%d) %s", reply_code, reply_text);
</method>

<method name = "cluster search" return = "channel">
    <doc>
    Lookups up a cluster channel tag, returns the channel reference if
    found, else null. The caller must unlink the returned reference
    when finished with it.  The cluster channel tag is formatted thus:
    serverid/connectionid/channelnbr.
    </doc>
    <argument name = "cluster id" type = "char *">Cluster consumer tag</argument>
    <declare name = "channel" type = "amq_server_channel_t *">channel to return</declare>
    <local>
    icl_shortstr_t
        string;                         //  Copy of cluster channel tag
    char
        *connection_id,                 //  Connection id value
        *channel_nbr;                   //  Channel number string
    amq_server_connection_t
        *connection;                    //  Connection
    </local>
    //
    icl_shortstr_cpy (string, cluster_id);

    //  String must start with our own id
    connection_id = strchr (string, '/');
    assert (connection_id);
    connection_id++;

    channel_nbr = strchr (connection_id, '/');
    assert (channel_nbr);
    *channel_nbr++ = 0;

    //  Lookup connection, channel, and channel if necessary
    connection = amq_server_connection_table_search (amq_broker->connections, connection_id);
    if (connection) {
        channel = amq_server_channel_table_search (connection->channels, atoi (channel_nbr));
        amq_server_connection_unlink (&connection);
    }
    else
        channel = NULL;
</method>

<method name = "selftest">
    <local>
    amq_server_channel_table_t
        *table;
    amq_server_channel_t
        *channel;
    int
        count;
    dbyte
        channel_nbr;
    </local>

    smt_initialise ();
    table = amq_server_channel_table_new ();

    //  Let's try some edge cases
    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE - 1);
    amq_server_channel_destroy (&channel);
    smt_wait (0);

    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE);
    amq_server_channel_destroy (&channel);
    smt_wait (0);

    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_MAXSIZE + 1);
    amq_server_channel_destroy (&channel);

    //  Now some random table bashing
    for (count = 0; count < 2000; count++) {
        channel_nbr = randomof (0xffff);
        channel = amq_server_channel_new (table, NULL, channel_nbr);
        amq_server_channel_destroy (&channel);
        smt_wait (0);
    }
    amq_server_channel_table_destroy (&table);
</method>

</class>
