<?xml version="1.0"?>
<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "client"
    >
<inherit name = "amq" />
<inherit name = "asl_client" />
<option name = "product_name" value = "OpenAMQ Kernel Client" />

<class name = "basic">
  <action name = "browse-ok">
    amq_content_$(class.name)_possess (self->content);
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    ipr_looseref_queue (session->arrived_$(class.name)_list, self->content);
  </action>

  <action name = "deliver">
    amq_content_$(class.name)_possess (self->content);
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    ipr_looseref_queue (session->arrived_$(class.name)_list, self->content);
  </action>

  <action name = "bounce">
    amq_content_$(class.name)_possess (self->content);
    amq_content_$(class.name)_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    ipr_looseref_queue (session->bounced_$(class.name)_list, self->content);
    self->content->bounced = TRUE;

    if (!session->silent)
        icl_console_print ("W: $(class.name) message was bounced: %d - %s",
            session->reply_code, session->reply_text);
  </action>
</class>

<class name = "jms">
  <action name = "browse-ok" sameas = "basic" />
  <action name = "deliver"   sameas = "basic" />
  <action name = "bounce"    sameas = "basic" />
</class>

</protocol>
