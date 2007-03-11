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
  <local>
    int
        msg_count;
  </local>

    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_$(class.name)_list_push_back (session->arrived_$(class.name)_list, self->content);
    msg_count = amq_content_$(class.name)_list_size (session->arrived_$(class.name)_list);
    if (amq_client_config_arrived_high_water (amq_client_config) &&
        msg_count == amq_client_config_arrived_high_water (amq_client_config) &&
        icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
            amq_client_session_channel_flow (session, FALSE);
  </action>

  <action name = "deliver">
  <local>
    int
        msg_count;
  </local>

    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_$(class.name)_list_push_back (session->arrived_$(class.name)_list, self->content);
    msg_count = amq_content_$(class.name)_list_size (session->arrived_$(class.name)_list);
    if (amq_client_config_arrived_high_water (amq_client_config) &&
        msg_count == amq_client_config_arrived_high_water (amq_client_config) &&
        icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
            amq_client_session_channel_flow (session, FALSE);
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
