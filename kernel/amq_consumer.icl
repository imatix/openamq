<?xml?>
<class
    name    = "amq_consumer"
    comment = "Polymorphic queue consumer"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements a polymorphic consumer. This is an object that
ties the channel to the queue, and envelopes class-specific consumers
for Basic, File, and Stream content classes.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "4001" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_queue" />
    <option name = "count"  value = "1" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_channel" />
</inherit>
<inherit class = "icl_tracker" />

<import class = "amq_server_classes" />

<context>
    int
        class_id;                       //  Consumer content class id
    amq_server_channel_t
        *channel;                       //  Parent channel
    amq_queue_t
        *queue;                         //  Parent queue
    icl_shortstr_t
        tag,                            //  Consumer tag
        cluster_id;                     //  Cluster id for connection
    amq_consumer_basic_t
        *consumer_basic;                //  Basic consumer
    qbyte
        prefetch_size;                  //  Prefetch size
    dbyte
        prefetch_count;                 //  Prefetch count
    Bool
        no_local,                       //  Don't want own messages
        no_ack,                         //  Auto acknowledge messages
        exclusive,                      //  Exclusive access?
        clustered;                      //  Is consumer clustered?
    qbyte
        busy;                           //  Is consumer sending stuff?
</context>

<method name = "new">
    <argument name = "connection" type = "amq_server_connection_t *">Parent connection</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    <argument name = "method" type = "amq_server_method_t *">Consume method</argument>
    <dismiss argument = "table" value = "connection->consumer_table" />
    <dismiss argument = "key"   value = "self->tag" />
    //
    <local>
    amq_server_basic_consume_t
        *basic_consume;
    </local>
    //
    self->channel  = amq_server_channel_link (channel);
    self->queue    = amq_queue_link (queue);
    self->class_id = method->class_id;

    //  Class-dependent properties
    assert (method->class_id == AMQ_SERVER_BASIC);

    basic_consume = &method->payload.basic_consume;
    self->prefetch_size  = basic_consume->prefetch_size;
    self->prefetch_count = basic_consume->prefetch_count;
    self->no_local       = basic_consume->no_local;
    self->no_ack         = basic_consume->no_ack;
    self->exclusive      = basic_consume->exclusive;
    self->consumer_basic = amq_consumer_basic_new (self);

    icl_shortstr_cpy (self->tag, basic_consume->consumer_tag);
    if (strnull (self->tag))
        icl_shortstr_fmt (self->tag, "%d",
            icl_atomic_inc32 ((volatile qbyte *) &(connection->consumer_tag)));

    //  Broadcast consume method to cluster using our cluster_id
    if (connection->group != AMQ_CONNECTION_GROUP_CLUSTER
    &&  self->queue->clustered) {
        icl_shortstr_fmt (self->cluster_id,
            "%s/%s", connection->cluster_id, self->tag);
        icl_shortstr_cpy (
            method->payload.basic_consume.consumer_tag, self->cluster_id);
        amq_cluster_tunnel_out (amq_cluster,
            AMQ_CLUSTER_ALL, method, AMQ_CLUSTER_DURABLE, channel);
        self->clustered = TRUE;
    }
</method>

<method name = "destroy">
    <local>
    amq_proxy_method_t
        *method;
    </local>
    //
    amq_server_channel_unlink (&self->channel);
    amq_queue_unlink (&self->queue);

    if (self->class_id == AMQ_SERVER_BASIC) {
        amq_consumer_basic_destroy (&self->consumer_basic);

        //  Broadcast cancel method to cluster using our cluster_id
        if (amq_cluster && amq_cluster->enabled && self->clustered) {
            method = amq_proxy_method_new_basic_cancel (self->cluster_id);
            amq_cluster_tunnel_out (amq_cluster,
                AMQ_CLUSTER_ALL,
                (amq_server_method_t *) method,
                AMQ_CLUSTER_DURABLE,
                NULL);
            amq_proxy_method_destroy (&method);
        }
    }
</method>

<method name = "cluster search" return = "consumer">
    <doc>
    Lookups up a cluster consumer tag, returns the consumer reference
    if found, else null. The caller must unlink the returned reference
    when finished with it.  The cluster consumer tag is formatted thus:
    serverid/connectionid/tag.
    </doc>
    <argument name = "cluster id" type = "char *">Cluster consumer tag</argument>
    <declare name = "consumer" type = "amq_consumer_t *">Consumer to return</declare>
    <local>
    icl_shortstr_t
        string;                         //  Copy of cluster consumer tag
    char
        *connection_id,                 //  Connection id value
        *consumer_tag;                  //  Consumer tag value
    amq_server_connection_t
        *connection;                    //  Connection
    </local>
    //
    icl_shortstr_cpy (string, cluster_id);

    //  String must start with our own server id
    connection_id = strchr (string, '/');
    assert (connection_id);
    connection_id++;

    consumer_tag = strchr (connection_id, '/');
    assert (consumer_tag);
    *consumer_tag++ = 0;

    //  Lookup connection, channel, and consumer if necessary
    connection = amq_server_connection_table_search (
        amq_broker->connections, connection_id);
    if (connection) {
        consumer = amq_consumer_table_search (connection->consumer_table, consumer_tag);
        amq_server_connection_unlink (&connection);
        assert (streq (consumer->cluster_id, cluster_id));
    }
    else
        consumer = NULL;
</method>

<method name = "selftest" />

</class>
