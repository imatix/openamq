<?xml?>
<class
    name      = "amq_smessage_list"
    comment   = "Implements list head container for amq_smessage"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >
<doc>
    This class implements list head container for amq_smessage
</doc>

<inherit class = "ipr_list_head" />

<import class = "amq_smessage"  />
<option name = "childname" value = "amq_smessage" />

<context overlay = "amq_smessage">
</context>

<method name = "commit" template = "function">
    <doc>
    Commits and dispatches all messages waiting on the list.
    </doc>
    <local>
    amq_smessage_t
        *previous,
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        amq_queue_accept   (message->queue, NULL, message);
        amq_queue_dispatch (message->queue);
        previous = message;
        message = amq_smessage_list_first (self);
        ASSERT (message != previous);
    }
</method>

<method name = "rollback" template = "function">
    <doc>
    Destroys all messages waiting on the list.
    </doc>
    <local>
    amq_smessage_t
        *message;
    </local>
    message = amq_smessage_list_first (self);
    while (message) {
        amq_smessage_destroy (&message);
        message = amq_smessage_list_first (self);
    }
</method>

<method name = "selftest" />

</class>
