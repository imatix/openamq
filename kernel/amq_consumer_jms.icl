<?xml?>
<class
    name      = "amq_consumer_jms"
    comment   = "JMS consumer"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements a JMS consumer.  For now, an empty object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<public name = "include">
#include "amq_server_classes.h"
</public>

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
