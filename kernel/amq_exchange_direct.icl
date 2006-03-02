<?xml?>
<class
    name    = "amq_exchange_direct"
    comment = "Direct routing exchange class"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements the dest exchange, which routes messages based
on the routing_key.
</doc>

<inherit class = "amq_exchange_base" />

<import class = "amq_hash" />

<method name = "compile">
    //  We don't do anything, since we use the exchange's binding
    //  hash table for our lookups.
    if (amq_server_config_debug_route (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "X: compile  routing_key=%s", binding->routing_key);
</method>

<method name = "publish">
    <local>
    amq_binding_t
        *binding;
    amq_hash_t
        *hash;                          //  Entry into hash table
     </local>
    //
    if (amq_server_config_debug_route (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "X: route    routing_key=%s", routing_key);

    hash = amq_hash_table_search (self->exchange->binding_hash, routing_key);
    if (hash) {
        binding = hash->data;
        delivered += amq_binding_publish (binding, channel, method);
        amq_hash_unlink (&hash);
    }
</method>

<method name = "unbind">
    <local>
    amq_hash_t
        *hash;                          //  Entry into hash table
     </local>
    //
    //  We know there is exactly one entry in the hash table
    //  so we can safely delete the binding completely.
    //
    hash = amq_hash_table_search (self->exchange->binding_hash, binding->routing_key);
    if (hash) {
        amq_hash_table_remove (hash);
        amq_hash_unlink (&hash);
    }
</method>

</class>
