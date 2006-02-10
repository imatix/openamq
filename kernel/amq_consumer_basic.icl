<?xml?>
<class
    name    = "amq_consumer_basic"
    comment = "Basic consumer"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements a Basic consumer.  For now, an empty object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_consumer_t
        *consumer;                      //  Parent consumer
</context>

<method name = "new">
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    //
    self->consumer = consumer;
</method>

<method name = "selftest" />

</class>
