<?xml version="1.0"?>
<protocol
    comment  = "OpenAMQ server protocol methods"
    script   = "asl_gen"
    chassis  = "client"
    basename = "amq_proxy"
    >
<!--
    Defines the proxy client protocol methods and how they are
    processed.
    -->
    
<inherit name = "amq_cluster" />
<inherit name = "amq" />

<option name = "product_name"   value = "OpenAMQ Kernel Proxy" />
<option name = "import_classes" value = "amq_server_classes" />

<class name = "connection">
  <action name = "start">
    <local>
    asl_field_list_t
        *fields;                        //  Decoded responses
    icl_shortstr_t
        spid;
    </local>
    //
    strclr (spid);
    fields = asl_field_list_new (method->server_properties);
    assert (fields);
    asl_field_list_cpy (fields, spid, "spid");
    asl_field_list_destroy (&fields);
    assert (*spid);

    //  Tell the cluster that the node we're talking to is now alive
    if (amq_cluster)
        amq_cluster_joined (amq_cluster, session, spid);
  </action>
</class>

<class name = "basic">
  <action name = "deliver">
    //  The client_key in the delivered message holds the connection
    //  id, channel key, and consumer tag, delimited by spaces.
    char
        *char_ptr,                      //  Pointer into client key string
        *connection_id;                 //  Connection id value
    dbyte
        channel_key,                    //  Channel key value
        consumer_tag;                   //  Consumer tag value
    amq_server_connection_t
        *connection = NULL;             //  Connection to send message to
    amq_server_channel_t
        *channel = NULL;                //  Channel to send message to
    amq_consumer_t
        *consumer = NULL;               //  Consumer to send message to
    amq_content_$(class.name)_t
        *content;                       //  Current content

    //  Collect connection id from start of client key
    connection_id = method->client_key;
    char_ptr = strchr (connection_id, ' ');
    assert (char_ptr);
    *char_ptr++ = 0;

    //  Collect channel key as next integer
    channel_key = atoi (char_ptr);
    char_ptr = strchr (char_ptr, ' ');
    assert (char_ptr);
    *char_ptr++ = 0;

    //  Collect consumer tag as next integer
    consumer_tag = atoi (char_ptr);

    //  Lookup connection, channel, and consumer if necessary
    connection = amq_server_connection_table_search (amq_broker->connections, connection_id);
    if (connection) {
        channel = amq_server_channel_table_search (connection->channels, channel_key);
        if (amq_server_channel_alive (channel))
            consumer = amq_server_channel_consumer_search (channel, consumer_tag);
    }
        
    if (consumer) {
        content = (amq_content_$(class.name)_t *) self->content;
        amq_server_agent_$(class.name)_deliver (
            connection->thread,
            channel_key,
            content,
            consumer_tag,
            consumer->client_key,
            0,                          //  Delivery tag
            FALSE,                      //  Redelivered
            content->exchange,
            content->routing_key);
    }
    else
        icl_console_print ("E: no consumer for proxied message, discarding");
    
    amq_consumer_unlink          (&consumer);
    amq_server_channel_unlink    (&channel);
    amq_server_connection_unlink (&connection);
  </action>
</class>

<class name = "file">
<!--
    to handle file deliveries, which are staged, we should probably make a
    channel method that accepts the delivery and handles it according to
    the class type.

    question: where do we put the file-related delivery mechanism? Maybe
    amq_consumer_file.icl?
  -->
</class>

</protocol>
