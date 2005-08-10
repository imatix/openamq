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
for JMS, Basic, and other content classes.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "ipr_list_item" >
    <option name = "prefix" value = "by_queue" />
    <option name = "count"  value = "1" />
</inherit>
<inherit class = "ipr_list_item" >
    <option name = "prefix" value = "by_channel" />
</inherit>

<public name = "include">
#include "amq_server_classes.h"
</public>

<context>
    int
        class_id;                       //  Consumer content class id
    amq_server_channel_t
        *channel;                       //  Parent channel
    amq_queue_t
        *queue;                         //  Parent queue
    dbyte
        tag;                            //  Consumer tag
    amq_consumer_jms_t
        *consumer_jms;                  //  JMS consumer
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
    <argument name = "channel"        type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "queue"          type = "amq_queue_t *">Parent queue</argument>
    <argument name = "class id"       type = "int"   >The content class</argument>
    <argument name = "prefetch size"  type = "qbyte" >Prefetch size</argument>
    <argument name = "prefetch count" type = "dbyte" >Prefetch count</argument>
    <argument name = "no local"       type = "Bool"  >Don't want own messages</argument>
    <argument name = "auto ack"       type = "Bool"  >Auto acknowledge messages</argument>
    <argument name = "exclusive"      type = "Bool"  >Exclusive access?</argument>
    //
    self->channel        = channel;
    self->queue          = queue;
    self->class_id       = class_id;
    self->prefetch_size  = prefetch_size;
    self->prefetch_count = prefetch_count;
    self->no_local       = no_local;
    self->auto_ack       = auto_ack;
    self->exclusive      = exclusive;

    if (self->class_id == AMQ_SERVER_JMS)
        self->consumer_jms = amq_consumer_jms_new (self);
    else
    if (self->class_id == AMQ_SERVER_BASIC)
        self->consumer_basic = amq_consumer_basic_new (self);
</method>

<method name = "destroy">
    if (self->class_id == AMQ_SERVER_JMS)
        amq_consumer_jms_destroy (&self->consumer_jms);
    else
    if (self->class_id == AMQ_SERVER_BASIC)
        amq_consumer_basic_destroy (&self->consumer_basic);
</method>

<method name = "selftest" />

</class>
