<?xml version="1.0"?>
<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "client"
    >
<inherit name = "amq" />
<inherit name = "asl_client" />
<option name = "product_name" value = "OpenAMQ Kernel Client" />
<option name = "syncapi"      value = "1" />

<class name = "basic">
  <action name = "get-ok">
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_$(class.name)_list_push_back (session->arrived_$(class.name)_list, self->content);
  </action>

  <action name = "deliver">
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_$(class.name)_list_push_back (session->arrived_$(class.name)_list, self->content);
  </action>

  <action name = "return">
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_$(class.name)_list_push_back (session->returned_$(class.name)_list, self->content);
    ((amq_content_$(class.name)_t *) self->content)->returned = TRUE;

    if (!session->silent)
        icl_console_print ("W: $(class.name) message was returned: %d - %s",
            session->reply_code, session->reply_text);
  </action>
</class>

<class name = "file">
  <action name = "deliver" sameas = "basic" />
  <action name = "return"  sameas = "basic" />
</class>

</protocol>
