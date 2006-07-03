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

<class name = "queue">
  <defaults method = "declare">
    <field name = "ticket"       default = "0" />
    <field name = "passive"      default = "0" />
    <field name = "durable"      default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </defaults>
  <defaults method = "bind">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </defaults>
  <defaults method = "purge">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "delete">
    <field name = "ticket"       default = "0" />
    <field name = "if unused"    default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
</class>

<class name = "exchange">
  <defaults method = "declare">
    <field name = "ticket"       default = "0" />
    <field name = "passive"      default = "0" />
    <field name = "auto delete"  default = "0" />
    <field name = "internal"     default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </defaults>
  <defaults method = "delete">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
</class>

<class name = "basic">
  <defaults method = "consume">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "no ack"       default = "1" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "cancel">
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "publish">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </defaults>
  <defaults method = "get">
    <field name = "ticket"       default = "0" />
    <field name = "no ack"       default = "0" />
  </defaults>

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
  <defaults method = "consume">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "cancel">
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "publish">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </defaults>
  <action name = "deliver" sameas = "basic" />
  <action name = "return"  sameas = "basic" />
</class>

<class name = "stream">
  <defaults method = "consume">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "cancel">
    <field name = "nowait"       default = "0" />
  </defaults>
  <defaults method = "publish">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </defaults>
</class>

</protocol>
