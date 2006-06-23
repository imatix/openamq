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
  <method name = "declare" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "passive"      default = "0" />
    <field name = "durable"      default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </method>
  <method name = "bind" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </method>
  <method name = "purge" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "delete" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "if unused"    default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
</class>

<class name = "exchange">
  <method name = "declare" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "passive"      default = "0" />
    <field name = "auto delete"  default = "0" />
    <field name = "internal"     default = "0" />
    <field name = "nowait"       default = "0" />
    <field name = "arguments"    default = ""  />
  </method>
  <method name = "delete" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
</class>

<class name = "basic">
  <method name = "consume" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "no ack"       default = "1" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "cancel" overlay = "1">
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "publish" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </method>
  <method name = "get" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "no ack"       default = "0" />
  </method>

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
  <method name = "consume" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "cancel" overlay = "1">
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "publish" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </method>
  <action name = "deliver" sameas = "basic" />
  <action name = "return"  sameas = "basic" />
</class>

<class name = "stream">
  <method name = "consume" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "consumer tag" default = ""  />
    <field name = "no local"     default = "0" />
    <field name = "exclusive"    default = "0" />
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "cancel" overlay = "1">
    <field name = "nowait"       default = "0" />
  </method>
  <method name = "publish" overlay = "1">
    <field name = "ticket"       default = "0" />
    <field name = "mandatory"    default = "0" />
    <field name = "immediate"    default = "0" />
  </method>
</class>

</protocol>
