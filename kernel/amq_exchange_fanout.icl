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
    <local>
    amq_binding_t
        *binding;
    </local>
    //
    binding = amq_binding_list_first (self->exchange->binding_list);
    if (binding) {
        if (amq_binding_publish (binding, channel, method))
            delivered = TRUE;
        amq_binding_unlink (&binding);
    }
</method>

</class>
