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
    <option name = "hash_size" value = "65535" />
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
    amq_connection_queue_t
        *mgt_connection_queue;          //  Connection-queue management object
    amq_queue_connection_t
        *mgt_queue_connection;          //  Queue-connection management object
    icl_shortstr_t
        tag;                            //  Consumer tag
    amq_consumer_basic_t
        *consumer_basic;                //  Basic consumer
    Bool
        no_local,                       //  Don't want own messages
        no_ack,                         //  Auto acknowledge messages
        exclusive;                      //  Exclusive access?
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

    //  Interface to console
    self->mgt_connection_queue = amq_connection_queue_new (channel->mgt_connection, self);
    self->mgt_queue_connection = amq_queue_connection_new (queue, self);

    //  Class-dependent properties
    assert (method->class_id == AMQ_SERVER_BASIC);

    basic_consume = &method->payload.basic_consume;
    self->no_local       = basic_consume->no_local;
    self->no_ack         = basic_consume->no_ack;
    self->exclusive      = basic_consume->exclusive;
    self->consumer_basic = amq_consumer_basic_new (self);

    icl_shortstr_cpy (self->tag, basic_consume->consumer_tag);
    if (strnull (self->tag))
        icl_shortstr_fmt (self->tag, "%d",
            icl_atomic_inc32 ((volatile qbyte *) &(connection->consumer_tag)));
</method>

<method name = "destroy">
    amq_server_channel_unlink   (&self->channel);
    amq_queue_unlink            (&self->queue);
    amq_connection_queue_unlink (&self->mgt_connection_queue);
    amq_queue_connection_unlink (&self->mgt_queue_connection);

    if (self->class_id == AMQ_SERVER_BASIC)
        amq_consumer_basic_destroy (&self->consumer_basic);
</method>

<method name = "selftest" />

</class>
