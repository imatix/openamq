<?xml version="1.0"?>
<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "client"
    >
<inherit name = "amq" />
<inherit name = "asl_client" />

<class name = "basic">
  <action name = "browse-ok">
    amq_content_$(class.name)_possess (self->content);
    amq_content_$(class.name)_set_routing (
        self->content, method->exchange, method->destination, 0);
    ipr_looseref_queue (session->arrived_$(class.name)_list, self->content);
  </action>

  <action name = "bounce">
    session->reply_code = method->reply_code;
    ipr_shortstr_cpy (session->reply_text, method->reply_text);
    if (!session->silent)
        icl_console_print ("W: $(class.name) message was bounced: %d - %s",
            session->reply_code, session->reply_text);

    amq_content_$(class.name)_possess (self->content);
    ipr_looseref_queue (session->bounced_$(class.name)_list, self->content);
  </action>
</class>

<class name = "jms">
  <context>
    dbyte
        consumer_tag;                   /*  Returned consumer tag            */
  </context>

  <action name = "browse-ok" sameas = "basic" />

  <action name = "deliver">
    amq_content_jms_possess (self->content);
    amq_content_$(class.name)_set_routing (
        self->content, method->exchange, method->destination, 0);
    ipr_looseref_queue (session->arrived_jms_list, self->content);
  </action>

  <action name = "bounce" sameas = "basic" />

  <action name = "consume-ok">
    session->consumer_tag = method->consumer_tag;
  </action>
</class>

</protocol>
