<?xml?>
<class
    name    = "amq_exchange_fanout"
    comment = "Fanout exchange class"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements the fanout exchange.  All messages are
routed to all bound queues and exchanges.
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    //  Until we actually do something here, stop the compiler complaining
    //  about self not being used in this method...
    if (self);
</method>

<method name = "publish">
    if (!amq_binding_list_empty (self->exchange->binding_list)) {
        if (amq_binding_publish (
              *amq_binding_list_begin (self->exchange->binding_list),
              channel, method))
            delivered = TRUE;
    }
</method>

</class>
