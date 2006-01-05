<?xml?>
<class
    name      = "amq_consumer"
    comment   = "Polymorphic queue consumer"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements a polymorphic consumer.  This is an object that
ties the channel to the queue, and envelopes class-specific consumers
for Basic, File, and Stream content classes.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_queue" />
    <option name = "count"  value = "1" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_channel" />
</inherit>

<import class = "amq_server_classes" />

<context>
    int
        class_id;                       //  Consumer content class id
    amq_server_channel_t
        *channel;                       //  Parent channel
    amq_queue_t
        *queue;                         //  Parent queue
    dbyte
        tag;                            //  External id for consumer
    icl_shortstr_t
        client_key;                     //  Client key for consumer
    amq_consumer_basic_t
        *consumer_basic;                //  Basic consumer
    qbyte
        prefetch_size;                  //  Prefetch size
    dbyte
        prefetch_count;                 //  Prefetch count
    Bool
        no_local,                       //  Don't want own messages
        auto_ack,                       //  Auto acknowledge messages
        exclusive;                      //  Exclusive access?
</context>

<method name = "new">
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "queue"   type = "amq_queue_t *">Parent queue</argument>
    <argument name = "method"  type = "amq_server_method_t *">Consume method</argument>
    //
    <local>
    amq_server_basic_consume_t
        *basic_consume;
    </local>
    self->channel  = channel;
    self->queue    = queue;
    self->class_id = method->class_id;

    //  Class-dependent properties
    if (method->class_id == AMQ_SERVER_BASIC) {
        basic_consume = &method->payload.basic_consume;
        self->prefetch_size  = basic_consume->prefetch_size;
        self->prefetch_count = basic_consume->prefetch_count;
        self->no_local       = basic_consume->no_local;
        self->auto_ack       = basic_consume->auto_ack;
        self->exclusive      = basic_consume->exclusive;
        icl_shortstr_cpy (self->client_key, basic_consume->client_key);
        self->consumer_basic = amq_consumer_basic_new (self);
    }
</method>

<method name = "destroy">
    if (self->class_id == AMQ_SERVER_BASIC)
        amq_consumer_basic_destroy (&self->consumer_basic);
</method>

<method name = "selftest" />

</class>
