<?xml version="1.0"?>
<!--
    Copyright (c) 2007 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
-->

<protocol
    comment  = "OpenAMQ peer client protocol methods"
    script   = "asl_gen"
    chassis  = "client"
    basename = "amq_peer"
    caller   = "amq_peering"
    broker   = "amq_broker"
    >
<!--
    Specifies the processing for AMQP methods that come in from a peering
    connection.
    -->

<inherit name = "amq" />

<option name = "product_name"   value = "OpenAMQ peering client" />
<option name = "import_classes" value = "amq_server_classes" />

<class name = "connection">
  <action name = "open-ok">
    amq_peering_peer_connection_open_ok (caller, self);
  </action>
  <action name = "close">
    amq_peering_peer_connection_close (caller, self);
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

</protocol>
