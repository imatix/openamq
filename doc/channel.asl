<?xml version="1.0"?>

<class
    name = "channel"
    handler = "channel"
  >
  work with channels

<doc>
  The channel class provides methods for a client to establish a virtual
  connection - a channel - to a server and for both peers to operate the
  virtual connection thereafter.
</doc>

<doc name = "grammar">
    channel             = open-channel *use-channel close-channel
    open-channel        = C:OPEN S:OPEN_OK
    use-channel         = C:FLOW
                        / S:FLOW
                        / S:SYNCH
                        / access
                        / destination
                        / subscription
                        / jms
                        / file
                        / stream
                        / tx
                        / dtx
                        / test
    close-channel       = C:CLOSE S:CLOSE_OK
                        / S:CLOSE C:CLOSE_OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "open" synchronous = "1">
  open a channel for use
  <doc>
    Client asks server to open a new channel.
  </doc>
  <doc name = "rule">
    This method is not allowed when the channel is already open.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "open ok" />

  <field name = "prefetch max" type = "long">
    maximum prefetch size for channel
    <doc>
      This value governs the total amount of prefetch data that the
      client is willing to accept per channel.  Prefetching is the
      technique of sending messages to a client in advance, which
      reduces the latency of message processing.  Normally each
      comnsumer specifies its own prefetch window.  The prefetch-max
      field specifies an overall limit at the channel level.  If it
      is zero, the server does not impose any channel-level limit.
    </doc>
    <doc name = "rule">
      The server MUST NOT send a client more data in advance than
      this value allows.  If sending a specific message in advance
      would exhaust the channel prefetch window, it MUST NOT send
      the message.  Setting this field to a very low non-zero value
      - e.g. 1 - effectively disables all prefetching on the channel.
    </doc>
  </field>

  <field name = "out of band" type = "shortstr">
    out-of-band settings for channel
    <doc>
      Configures out-of-band transfers on this channel.  The syntax and
      meaning of this field will be formally defined at a later date.
    </doc>
    <assert check = "null" />
  </field>
    
  <field name = "tx mode" type = "octet">
    transaction mode for channel
    <doc>
      Configures the transaction mode for the channel.  AMQP/Fast
      supports three transactional models: automatic, standard, and
      distributed (e.g. XA).
    </doc>
    <see name = "transaction modes"/>
    <assert check = "enum">
      <value name = "automatic"   >automatic transaction around every operation</value>
      <value name = "standard"    >standard commit and rollback</value>
      <value name = "distributed" >distributed 2-phase transactions</value>
    </assert>
  </field>
</method>

<method name = "open ok" synchronous = "1">
  signal that the channel is ready
  <doc>
    This method signals to the client that the channel is ready for use.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "flow">
  enable/disable flow from peer
  <doc>
    This method asks the peer to pause or restart the flow of content
    data. This is a simple flow-control mechanism that a peer can use
    to avoid oveflowing its queues or otherwise finding itself receiving
    more messages than it can process.  Note that this method is not
    intended for window control.
  </doc>
  <doc name = "rule">
    When sending content data in multiple frames, a peer SHOULD monitor
    the channel for incoming methods and respond to a Channel.Flow as
    rapidly as possible.
  </doc>
  <doc name = "rule">
    A peer MAY use the Channel.Flow method to throttle incoming content
    data for internal reasons, for example, when proxying data over a
    slower connection.
  </doc>
  <doc name = "rule">
    The peer that requests a Channel.Flow method MAY disconnect and/or
    ban a peer that does not respect the request.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />

  <field name = "flow pause" type = "bit">
    start/stop content frames
    <doc>
      If 1, the peer stops sending content frames.  If 0, the peer
      restarts sending content frames.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "close" synchronous = "1">
  request a channel close
  <doc>
    This method indicates that the sender wants to close the channel.
    This may be due to internal conditions (e.g. a forced shut-down) or
    due to an error handling a specific method, i.e. an exception.  When
    a close is due to an exception, the sender provides the class,
    method id, and synchtag of the method which caused the exception.
  </doc>
  <doc name = "rule">
    After sending this method any received method except
    Channel.Close_ok MUST be discarded.
  </doc>
  <doc name = "rule">
    The peer sending this method MAY use a counter or timeout to detect
    failure of the other peer to respond correctly with Channel.Close_ok..
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "close ok" />

  <field name = "reply code" domain = "reply code">
    reply code
  </field>

  <field name = "reply text" domain = "reply text">
    localised reply text 
  </field>

  <field name = "class" type = "octet">
    failing method class
    <doc>
      When the close is provoked by a method exception, this is the
      class of the method.
    </doc>
  </field>

  <field name = "method" type = "octet">
    failing method ID
    <doc>
      When the close is provoked by a method exception, this is the
      ID of the method.
    </doc>
  </field>

  <field name = "synchtag" type = "short">
    failing method synchtag
    <doc>
      When the close is provoked by a method exception, this is the
      synchtag of the method.
    </doc>
  </field>
</method>

<method name = "close ok" synchronous = "1">
  confirm a channel close
  <doc>
    This method confirms a Channel.Close method and tells the recipient
    that it is safe to release resources for the channel and close the
    socket.
  </doc>
  <doc name = "rule">
    A peer that detects a socket closure without having received a
    Channel.Close_ok handshake method SHOULD log the error.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
</method>

</class>

