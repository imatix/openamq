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
    amq_consumer_t
        *consumer;                      //  Consumer to send message to
    amq_content_$(class.name)_t
        *content;                       //  Current content

    consumer = amq_consumer_cluster_search (method->consumer_tag);
    if (consumer) {
        content = (amq_content_$(class.name)_t *) self->content;
        amq_server_agent_$(class.name)_deliver (
            consumer->channel->connection->thread,
            consumer->channel->number,
            content,
            consumer->tag,
            0,                          //  Delivery tag
            FALSE,                      //  Redelivered
            content->exchange,
            content->routing_key);
        amq_consumer_unlink (&consumer);
    }
    else
        icl_console_print ("E: no consumer for proxied message, discarding");
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
