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
    name    = "connection"
    handler = "connection"
    index   = "10"
  >
  work with socket connections
<doc>
  The connection class provides methods for a client to establish a
  network connection to a server, and for both peers to operate the
  connection thereafter.
</doc>

<doc name = "grammar">
    connection          = open-connection *use-connection close-connection
    open-connection     = C:protocol-header
                          S:START C:START-OK
                          *challenge
                          S:TUNE C:TUNE-OK
                          C:OPEN S:OPEN-OK 
    challenge           = S:SECURE C:SECURE-OK
    use-connection      = *channel
    close-connection    = C:CLOSE S:CLOSE-OK
                        / S:CLOSE C:CLOSE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "start" synchronous = "1" index = "10">
  start connection negotiation
  <doc>
    This method starts the connection negotiation process by telling
    the client the protocol version that the server proposes, along
    with a list of security mechanisms which the client can use for
    authentication.
  </doc>
  <doc name = "rule">
    If the client cannot handle the protocol version suggested by the
    server it MUST close the socket connection.
  </doc>
  <doc name = "rule">
    The server MUST provide a protocol version that is lower than or
    equal to that requested by the client in the protocol header. If
    the server cannot support the specified protocol it MUST NOT send
    this method, but MUST close the socket connection.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <response name = "start-ok" />

  <field name = "version major" type = "octet">
    protocol major version
    <doc>
      The protocol major version that the server agrees to use, which
      cannot be higher than the client's major version.
    </doc>
  </field>

  <field name = "version minor" type = "octet">
    protocol major version
    <doc>
      The protocol minor version that the server agrees to use, which
      cannot be higher than the client's minor version.
    </doc>
  </field>

  <field name = "server properties" domain = "peer properties">
    server properties
  </field>

  <field name = "mechanisms" type = "longstr">
    available security mechanisms
    <doc>
      A list of the security mechanisms that the server supports, delimited
      by spaces.  Currently ASL supports these mechanisms: PLAIN.
    </doc>
    <see name = "security mechanisms"/>
    <assert check = "notnull" />
  </field>

  <field name = "locales" type = "longstr">
    available message locales
    <doc>
      A list of the message locales that the server supports, delimited
      by spaces.  The locale defines the language in which the server
      will send reply texts.
    </doc>
    <doc name = "rule">
      All servers MUST support at least the en_US locale.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "start-ok" synchronous = "1" index = "11">
  select security mechanism and locale
  <doc>
    This method selects a SASL security mechanism. ASL uses SASL
    (RFC2222) to negotiate authentication and encryption.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "client properties" domain = "peer properties">
    client properties
  </field>

  <field name = "mechanism" type = "shortstr">
    selected security mechanism
    <doc>
      A single security mechanisms selected by the client, which must be
      one of those specified by the server.
    </doc>
    <doc name = "rule">
      The client SHOULD authenticate using the highest-level security
      profile it can handle from the list provided by the server.
    </doc>
    <doc name = "rule">
    The mechanism field MUST contain one of the security mechanisms
    proposed by the server in the Start method. If it doesn't, the
    server MUST close the socket.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "response" type = "longstr">
    security response data
    <doc>
      A block of opaque data passed to the security mechanism. The contents
      of this data are defined by the SASL security mechanism.  For the
      PLAIN security mechanism this is defined as a field table holding
      two fields, LOGIN and PASSWORD.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "locale" type = "shortstr">
    selected message locale
    <doc>
      A single message local selected by the client, which must be one
      of those specified by the server.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "secure" synchronous = "1" index = "20">
  security mechanism challenge
  <doc>
    The SASL protocol works by exchanging challenges and responses until
    both peers have received sufficient information to authenticate each
    other.  This method challenges the client to provide more information.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <response name = "secure-ok" />

  <field name = "challenge" type = "longstr">
    security challenge data
    <doc>
      Challenge information, a block of opaque binary data passed to
      the security mechanism.
    </doc>
    <see name = "security mechanisms"/>
  </field>
</method>

<method name = "secure-ok" synchronous = "1" index = "21">
  security mechanism response
  <doc>
    This method attempts to authenticate, passing a block of SASL data
    for the security mechanism at the server side.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "response" type = "longstr">
    security response data
    <doc>
      A block of opaque data passed to the security mechanism.  The contents
      of this data are defined by the SASL security mechanism.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "tune" synchronous = "1" index = "30">
  propose connection tuning parameters
  <doc>
    This method proposes a set of connection configuration values
    to the client.  The client can accept and/or adjust these.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <response name = "tune-ok" />

  <field name = "channel max" type = "short">
    proposed maximum channels
    <doc>
      The maximum total number of channels that the server allows
      per connection. Zero means that the server does not impose a
      fixed limit, but the number of allowed channels may be limited
      by available server resources.
    </doc>
  </field>

  <field name = "frame max" type = "long">
    proposed maximum frame size
    <doc>
      The largest frame size that the server proposes for the
      connection. The client can negotiate a lower value.  Zero means
      that the server does not impose any specific limit but may reject
      very large frames if it cannot allocate resources for them.
    </doc>
    <doc name = "rule">
      Until the frame-max has been negotiated, both peers MUST accept
      frames of up to 4096 octets large. The minimum non-zero value for
      the frame-max field is 4096.
    </doc>
  </field>

  <field name = "heartbeat" type = "short">
    desired heartbeat delay
    <doc>
      The delay, in seconds, of the connection heartbeat that the server
      wants.  Zero means the server does not want a heartbeat.
    </doc>
  </field>
</method>

<method name = "tune-ok" synchronous = "1" index = "31">
  negotiate connection tuning parameters
  <doc>
    This method sends the client's connection tuning parameters to the
    server. Certain fields are negotiated, others provide capability
    information.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "channel max" type = "short">
    negotiated maximum channels
    <doc>
      The maximum total number of channels that the client will use
      per connection.  May not be higher than the value specified by
      the server.
    </doc>
    <doc name = "rule">
      The server MAY ignore the channel-max value or MAY use it for
      tuning its resource allocation.
    </doc>
    <assert check = "notnull" />
    <assert check = "le" method = "tune" field = "channel max" />
  </field>

  <field name = "frame max" type = "long">
    negotiated maximum frame size
    <doc>
      The largest frame size that the client and server will use for
      the connection.  Zero means that the client does not impose any
      specific limit but may reject very large frames if it cannot
      allocate resources for them.  Note that the frame-max limit
      applies principally to content frames, where large contents
      can be broken into frames of arbitrary size.
    </doc>
    <doc name = "rule">
      Until the frame-max has been negotiated, both peers must accept
      frames of up to 4096 octets large. The minimum non-zero value for
      the frame-max field is 4096.
    </doc>
  </field>

  <field name = "heartbeat" type = "short">
    desired heartbeat delay
    <doc>
      The delay, in seconds, of the connection heartbeat that the client
      wants. Zero means the client does not want a heartbeat.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "open" synchronous = "1" index = "40">
  open connection to virtual host
  <doc>
    This method opens a connection to a virtual host, which is a
    collection of resources, and acts to separate multiple application
    domains within a server.
  </doc>
  <doc name = "rule">
    The client MUST open the context before doing any work on the
    connection.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "open-ok" />

  <field name = "virtual host" domain = "path">
    virtual host name
    <assert check = "regexp" value = "^[a-zA-Z0-9/-_]+$" />
    <doc>
      The name of the virtual host to work with.
    </doc>
    <doc name = "rule">
      If the server supports multiple virtual hosts, it MUST enforce a
      full separation of exchanges, queues, and all associated entities
      per virtual host. An application, connected to a specific virtual
      host, MUST NOT be able to access resources of another virtual host.
    </doc>
    <doc name = "rule">
      The server SHOULD verify that the client has permission to access
      the specified virtual host.
    </doc>
    <doc name = "rule">
      The server MAY configure arbitrary limits per virtual host, such
      as the number of each type of entity that may be used, per
      connection and/or in total.
    </doc>
  </field>

  <field name = "capabilities" type = "shortstr">
    required capabilities
    <doc>
      The client may specify a number of capability names, delimited by
      spaces.  The server can use this string to how to process the 
      client's connection request.
    </doc>
  </field>

  <field name = "insist" type = "bit">
    insist on connecting to server
    <doc>
      In a configuration with multiple load-sharing servers, the server
      may respond to a Connection.Open method with a Connection.Redirect.
      The insist option tells the server that the client is insisting on
      a connection to the specified server.
    </doc>
    <doc name = "rule">
      When the client uses the insist option, the server SHOULD accept
      the client connection unless it is technically unable to do so.
    </doc>
  </field>
</method>

<method name = "open-ok" synchronous = "1" index = "41">
  signal that the connection is ready
  <doc>
    This method signals to the client that the connection is ready for
    use.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "known hosts" domain = "known hosts" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "close" synchronous = "1" index = "50">
  request a connection close
  <doc>
    This method indicates that the sender wants to close the connection.
    This may be due to internal conditions (e.g. a forced shut-down) or
    due to an error handling a specific method, i.e. an exception.  When
    a close is due to an exception, the sender provides the class and
    method id of the method which caused the exception.
  </doc>
  <doc name = "rule">
    After sending this method any received method except the Close-OK
    method MUST be discarded.
  </doc>
  <doc name = "rule">
    The peer sending this method MAY use a counter or timeout to
    detect failure of the other peer to respond correctly with
    the Close-OK method.
  </doc>
  <doc name = "rule">
    When a server receives the Close method from a client it MUST
    delete all server-side resources associated with the client's
    context.  A client CANNOT reconnect to a context after sending
    or receiving a Close method.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "close-ok" />

  <field name = "reply code" domain = "reply code" />
  <field name = "reply text" domain = "reply text" />

  <field name = "class id" domain = "class id">
    failing method class
    <doc>
      When the close is provoked by a method exception, this is the
      class of the method.
    </doc>
  </field>

  <field name = "method id" domain = "class id">
    failing method ID
    <doc>
      When the close is provoked by a method exception, this is the
      ID of the method.
    </doc>
  </field>
</method>

<method name = "close-ok" synchronous = "1" index = "51">
  confirm a connection close
  <doc>
    This method confirms a Connection.Close method and tells the
    recipient that it is safe to release resources for the connection
    and close the socket.
  </doc>
  <doc name = "rule">
    A peer that detects a socket closure without having received a
    Close-Ok handshake method SHOULD log the error.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
</method>

</class>
