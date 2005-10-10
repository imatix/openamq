<?xml?>
<class
    name      = "amq_exchange_direct"
    comment   = "Direct routing exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the dest exchange, which routes messages
based on their "routing_key" property.
</doc>

<inherit class = "amq_exchange_base" />

<import class = "amq_hash" />

<context>
    amq_hash_table_t
        *binding_hash;                  //  Bindings hashed by routing_key
</context>

<method name = "new">
    self->binding_hash = amq_hash_table_new ();
</method>

<method name = "destroy">
    amq_hash_table_destroy (&self->binding_hash);
</method>

<method name = "compile">
    <local>
    asl_field_list_t
        *fields;                        //  Decoded arguments
    asl_field_t
        *routing_key;                   //  Routing key
    amq_hash_t
        *hash;                          //  Hash entry
    </local>
    //
    fields = asl_field_list_new (binding->arguments);
    if (fields) {
        routing_key = asl_field_list_search (fields, "routing_key");
        if (routing_key) {
            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: compile  routing_key=%s", asl_field_string (routing_key));
            hash = amq_hash_new (self->binding_hash, asl_field_string (routing_key), binding);
            if (hash)
                amq_hash_unlink (&hash);
            else {
                rc = 1;
                amq_server_channel_close (
                    channel, ASL_COMMAND_INVALID, "Duplicate binding");
            }
            asl_field_unlink (&routing_key);
        }
        else {
            rc = 1;
            amq_server_channel_close (
                channel, ASL_COMMAND_INVALID, "No routing_key field specified");
        }
        asl_field_list_destroy (&fields);
    }
    else {
        rc = 1;
        amq_server_channel_close (
            channel, ASL_COMMAND_INVALID, "Invalid binding arguments");
    }
</method>

<method name = "publish">
    <local>
    amq_binding_t
        *binding;
     amq_hash_t
         *hash;                         //  Entry into hash table
     </local>
    //
    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: route    routing_key=%s", routing_key);

    hash = amq_hash_table_search (self->binding_hash, routing_key);
    if (hash) {
        binding = hash->data;
        if (amq_binding_publish (binding, channel, class_id, content, mandatory, immediate))
            delivered = TRUE;
        amq_hash_unlink (&hash);
    }
</method>

</class>
