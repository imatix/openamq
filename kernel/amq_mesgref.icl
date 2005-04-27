@<?xml?>
<class
    name      = "amq_mesgref"
    comment   = "Message reference class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a message reference in memory. Queues holds lists of message
    references, which themselves refer to individual messages.  This
    lets us share a message between multiple queues without copying it.
</doc>

<inherit class = "ipr_list_item" />

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    amq_smessage_t
        *message;                       /*  Message referred to              */
    ipr_looseref_list_t
        *browsers;                      /*  List of browsers per message     */
</context>

<method name = "new">
    <argument name = "list"    type = "$(selfname)_list_t *">List to attach to</argument>
    <argument name = "message" type = "amq_smessage_t *"    >Message object</argument>

    self->message  = message;
    self->browsers = ipr_looseref_list_new ();
    amq_smessage_link      (self->message);
    amq_mesgref_list_queue (list, self);
</method>

<method name = "destroy">
    <local>
    ipr_looseref_t
        *browser_ref;                   /*  Browsed message                  */
    </local>

    /*  Invalidate any browsers for this message                             */
    browser_ref = ipr_looseref_list_first (self->browsers);
    while (browser_ref) {
        amq_browser_destroy ((amq_browser_t **) &browser_ref->object);
        browser_ref = ipr_looseref_list_next (self->browsers, browser_ref);
    }
    ipr_looseref_list_destroy (&self->browsers);
    amq_smessage_destroy      (&self->message);
</method>

<method name = "selftest">
</method>

</class>
