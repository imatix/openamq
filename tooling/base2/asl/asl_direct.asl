<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
<class
    name    = "direct"
    handler = "connection"
    index   = "61500"
  >
  Direct Protocol class.

<doc>
  Implements the Direct Message Protocol specifications, 4-DMP@wiki.amqp.org.
  see http://wiki.amqp.org.
</doc>

<doc name = "grammar">
    direct              = C:PUT S:PUT-OK
                        / C:GET S:GET-OK
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "put" synchronous = "1" index = "10">
  request a connection to a sink
  <doc>
    Requests non-exclusive access to the specified sink.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "put-ok" />

  <field name = "sink" type = "shortstr" >
    Sink name
    <doc>
    Specifies the name of the sink, which must exist on the server.
    </doc>
  </field>
</method>

<method name = "put-ok" synchronous = "1" index = "11">
  grant access to the sink
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "lease" type = "shortstr">
    Lease for connection
    <doc>
    Holds a lease which the client must use when opening the direct 
    protocol connection.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "get" synchronous = "1" index = "20">
  request a connection to a feed
  <doc>
    Requests exclusive access to the specified feed.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "get-ok" />

  <field name = "feed" type = "shortstr" >
    Feed name
    <doc>
    Specifies the name of the feed, which must exist on the server and be
    already owned by the current connection.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "get-ok" synchronous = "1" index = "21">
  grant access to the feed
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "lease" type = "shortstr">
    Lease for connection
    <doc>
    Holds a lease which the client must use when opening the direct 
    protocol connection.
    </doc>
  </field>
</method>

</class>

