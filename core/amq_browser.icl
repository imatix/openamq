<?xml?>
<class
    name      = "amq_browser"
    comment   = "Message browser class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a message reference that points to either a persistent
    queued message or a memory queued message.  Used to support
    message browsing.
</doc>

<inherit class = "ipr_array_item" />

<import class = "ipr_classes" />
<import class = "amq_global"  />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    amq_queue_t
        *queue;                         /*  Queue holding message            */
    qbyte
        item_id;                        /*  Queue ID if persistent           */
    amq_smessage_t
        *message;                       /*  Memory ID if not persistent      */
</context>

<method name = "new">
    <argument name = "queue"   type = "amq_queue_t *"   >Queue holding message</argument>
    <argument name = "item id" type = "qbyte"           >Key for persistent storage</argument>
    <argument name = "message" type = "amq_smessage_t *">Key for memory storage</argument>
    self->queue   = queue;
    self->item_id = item_id;
    self->message = message;
</method>

<method name = "selftest">
    <local>
    amq_browser_array_t
        *array;
    int
        index;
    </local>
    array = amq_browser_array_new ();
    for (index = 0; index < 5000; index++)
        self_new (array, index, NULL, 0, NULL);
    amq_browser_array_destroy (&array);
</method>

</class>
