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
    comment  = "Zyre peering client protocol handler"
    script   = "asl_gen"
    chassis  = "client"
    basename = "zyre_peer"
    caller   = "zyre_peering"
    broker   = "http_server"
    >
<!--
    Specifies the processing for AMQP methods that come in from a peering
    connection. -->

<inherit name = "amq" />

<option name = "product_name"   value = "Zyre peering client" />
<option name = "import_classes" value = "zyre_classes" />

<class name = "connection">
  <action name = "open-ok">
    zyre_peering_connection_open_ok (caller, self);
  </action>
  <action name = "close">
    zyre_peering_connection_close (caller, self);
  </action>
</class>

<class name = "channel">
  <action name = "open-ok">
    zyre_peering_channel_open_ok (caller, self);
  </action>
</class>

<class name = "basic">
  <action name = "deliver">
    zyre_peering_basic_deliver (caller, self);
  </action>
  <action name = "return">
    zyre_peering_basic_return (caller, self);
  </action>
</class>

</protocol>
