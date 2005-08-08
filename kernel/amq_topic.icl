<?xml?>
<class
    name      = "amq_topic"
    comment   = "Topic instance class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a topic instance, used when rebuilding a subscriber index.
    The topic is just a destination name used by a publisher at some time
    in the past.  Topics are not predefined.  When a publisher uses a
    topic that is new to the vhost, all subscribers are updated for that
    topic, if they match it.  When a new subscriber joins, it is compared
    to all known topics for potential matches.
</doc>

<inherit class = "icl_object">
    <option name = "cache"  value = "1" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "ipr_array_item" />

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    // TODO stick hash index here to save a hash recalc
    ipr_shortstr_t
        dest_name;                      /*  Topic destination name           */
</context>

<method name = "new">
    <argument name = "dest name" type = "char *">Topic destination name</argument>
    assert (dest_name);
    ipr_shortstr_cpy (self->dest_name, dest_name);
</method>

<method name = "selftest">
    <local>
    amq_topic_array_t
        *array;
    amq_topic_t
        *item;
    int
        index;
    </local>
    array = amq_topic_array_new ();
    for (index = 0; index < 5000; index++) {
        item = self_new (array, index, "this is a test blah blah");
        amq_topic_unlink (&item);
    }
    amq_topic_array_destroy (&array);
</method>

</class>
