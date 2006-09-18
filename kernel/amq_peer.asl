<?xml version="1.0"?>
<protocol
    comment  = "OpenAMQ peer client protocol methods"
    script   = "asl_gen"
    chassis  = "client"
    basename = "amq_peer"
    caller   = "amq_peering"
    >
<!--
    Specifies the processing for AMQP methods that come in from a peering
    connection.
    -->

<inherit name = "amq" />

<option name = "product_name"   value = "OpenAMQ peering client" />
<option name = "import_classes" value = "amq_server_classes" />

<class name = "connection">
  <action name = "start">
    amq_peering_peer_connection_start (caller, self);
  </action>
</class>

<class name = "channel">
  <action name = "open-ok">
    amq_peering_peer_channel_open_ok (caller, self);
  </action>
</class>

<class name = "basic">
  <!-- We use only asynchronous consumers to get messages -->
  <action name = "deliver">
    amq_peering_peer_basic_deliver (caller, self);
  </action>
  <action name = "return">
    amq_peering_peer_basic_return (caller, self);
  </action>
</class>

<class name = "file">
</class>

</protocol>
