<?xml?>
<class
    name      = "amq_exchange_dest_name"
    comment   = "Dest-name exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the dest-name exchange, which routes messages
based on their "destination" property.
</doc>

<inherit class = "amq_exchange_base" />

<import class = "amq_hash" />

<context>
    amq_hash_table_t
        *binding_hash;                  //  Bindings hashed by destination
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
        *destination;                   //  Destination value
    amq_hash_t
        *hash;                          //  Hash entry
    </local>
    //
    fields = asl_field_list_new (binding->arguments);    
    if (fields) {
        destination = asl_field_list_search (fields, "destination");
        if (destination) {
            hash = amq_hash_new (self->binding_hash, asl_field_string (destination), binding);
            if (hash)
                amq_hash_unlink (&hash);
            else {
                rc = 1;
                amq_server_channel_close (
                    channel, ASL_COMMAND_INVALID, "Duplicate binding");
            }
            asl_field_unlink (&destination);
        }
        else {
            rc = 1;
            amq_server_channel_close (
                channel, ASL_COMMAND_INVALID, "No destination field specified");
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
    hash = amq_hash_table_search (self->binding_hash, destination);
    if (hash) {
        binding = hash->data;
        amq_binding_publish (binding, channel, class_id, content, mandatory, immediate);
        amq_hash_unlink (&hash);
        delivered = TRUE;
    }
</method>

</class>
