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

<method name = "compile">
    //  We don't do anything, since we use the exchange's binding
    //  hash table for our lookups.
    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: compile  %s: routing_key=%s", self->exchange->name, binding->routing_key);
</method>

<method name = "publish">
    <local>
    amq_binding_t
        *binding;
    ipr_hash_t
        *hash;                          //  Entry into hash table
     </local>
    //
    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: route    %s: routing_key=%s", self->exchange->name, routing_key);

    hash = ipr_hash_table_search (self->exchange->binding_hash, routing_key);
    if (hash) {
        binding = hash->data;
        delivered += amq_binding_publish (binding, channel, method, from_cluster);
        ipr_hash_unlink (&hash);
    }
</method>

<method name = "unbind">
    <local>
    ipr_hash_t
        *hash;                          //  Entry into hash table
     </local>
    //
    //  We know there is exactly one entry in the hash table
    //  so we can safely delete the binding completely.
    //
    hash = ipr_hash_table_search (self->exchange->binding_hash, binding->routing_key);
    if (hash)
        ipr_hash_destroy (&hash);
    //  We are going to destroy the binding that was passed to us
    //  but first we need to grab a link to it...
    binding = amq_binding_link (binding);
    amq_binding_destroy (&binding);
</method>

</class>
