<?xml?>
<class
    name      = "amq_subsc"
    comment   = "Subscription class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Defines a subscription class.  This is an interim implementation of
topic pub/sub subscriptions, slow but simple.  Subscriptions are
defined per vhost.
</doc>

<inherit class = "ipr_list_item" />

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    amq_consumer_t
        *consumer;                      /*  Consumer for this subscription   */
    ipr_shortstr_t
        dest_name;                      /*  Topic name for subscription      */
</context>

<method name = "new">
    <doc>
    Creates a new subscription as specified.
    </doc>
    <argument name = "dest_name" type = "char *"    >Parent channel</argument>
    <argument name = "consumer"  type = "amq_handle_open_t *">Handle open command</argument>
</method>

<method name = "destroy">
</method>

<method name = "selftest">
</method>

</class>
