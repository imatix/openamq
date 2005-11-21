<?xml version="1.0"?>
<class
    name    = "basic"
    handler = "channel"
    index   = "6"
  >
  work with basic content
<doc>
  The basic operational model is designed to provide the simplest possible
  message store and forward model.  Basic messages are not acknowledged,
  cannot be pushed back onto a queue, and are not covered by transactions.
</doc>

<doc name = "grammar">
    basic               = C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:PUBLISH content
                        / S:BOUNCE content
                        / S:DELIVER content
                        / C:GET ( S:GET-OK content / S:GET-EMPTY )
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

<doc name = "rule">
  The server SHOULD respect the persistent property of basic messages
  and SHOULD make a best-effort to hold persistent basic messages on a
  reliable storage mechanism.
</doc>
<doc name = "rule">
  The server MUST NOT discard a persistent basic message in case of a
  queue overflow. The server MAY use the Channel.Flow method to slow
  or stop a basic message publisher when necessary.
</doc>
<doc name = "rule">
  The server MAY overflow non-persistent basic messages to persistent
  storage and MAY discard or dead-letter non-persistent basic messages
  on a priority basis if the queue size exceeds some configured limit.
</doc>
<doc name = "rule">
  The server MUST implement at least 2 priority levels for basic
  messages, where priorities 0-4 and 5-9 are treated as two distinct
  levels. The server MAY implement up to 10 priority levels.
</doc>
<doc name = "rule">
  The server MUST deliver messages of the same priority in order
  irrespective of their individual persistence. 
</doc>
<doc name = "rule">
  The server MUST implement automatic acknowledgements on all Basic
  messages.  That is, as soon as a message is delivered to a client
  via a Deliver or a Get-Ok method, the server must remove it from
  the queue.
</doc>

<!--  These are the properties for a Basic content  -->

<field name = "content type" type = "shortstr">
    MIME content type
</field>
<field name = "content encoding" type = "shortstr">
    MIME content encoding
</field>
<field name = "headers" type = "table">
    Message header field table
</field>
<field name = "message id" type = "shortstr">
    The application message identifier
</field>
<field name = "reply to" type = "shortstr">
    The destination to reply to
</field>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "consume" synchronous = "1">
  start a queue consumer
  <doc>
    This method asks the server to start a "consumer", which is a
    transient request for messages from a specific queue. Consumers
    last as long as the channel they were created on, or until the
    client cancels them.
  </doc>
  <doc name = "rule" test = "amq_basic_01">
    The server MUST allow at least 16 consumers per queue, unless the
    queue was declared as private.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "consume-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read" access
      rights to the realm for the queue.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to consume from.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "no local" domain = "no local" />

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc name = "rule" test = "amq_basic_02">
      Request exclusive consumer access.  If the server cannot grant
      this - because there are other consumers active - it MUST raise a
      channel exception.
    </doc>
  </field>
</method>

<method name = "consume-ok" synchronous = "1">
  confirm a new consumer
  <doc name = "rule" test = "amq_basic_03">
    The server MUST provide the client with a consumer tag, which is used by
    the client for methods called on the consumer at a later stage.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel" synchronous = "1">
  end a queue consumer
  <doc>
    This method cancels a consumer. This does not affect already
    delivered messages, but it does mean the server will not send any
    more messages for that consumer.  The client may receive an
    abitrary number of messages in between sending the cancel method
    and receiving the cancel-ok reply.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "cancel-ok" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>

<method name = "cancel-ok" synchronous = "1">
  confirm a cancelled consumer
  <doc>
    This method confirms that the cancellation was completed.    
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "publish" synchronous = "0" content = "1">
  publish a message
  <doc>
    This method publishes a message to a specific exchange. The message
    will be routed to queues as defined by the exchange configuration
    and distributed to any active consumers as appropriate.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "write"
      access rights to the access realm for the exchange.
    </doc>
  </field>

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange to publish to.  The exchange
      name can be empty, meaning the default exchange.  If the exchange
      name is specified, and that exchange does not exist, the server
      will raise a channel exception.
    </doc>
    <doc name = "rule">
      The server MUST accept a blank exchange name to mean the default
      exchange.
    </doc>
    <doc name = "rule">
      If the exchange was declared as an internal exchange, the server
      MUST respond with a reply code 403 (access refused) and raise a
      channel exception.
    </doc>
    <doc name = "rule">
      The exchange MAY refuse basic content in which case it MUST
      respond with a reply code 540 (not implemented) and raise a
      channel exception.
    </doc>
  </field>

  <field name = "routing key" type = "shortstr">
     Message routing key
    <doc>
      Specifies the routing key for the message.  The routing key is
      used for routing messages depending on the exchange configuration.
    </doc>
  </field>

  <field name = "mandatory" type = "bit">
    indicate mandatory routing
    <doc>
      This flag tells the server how to react if the message cannot be
      routed to a queue.  If this flag is set, the server returns the
      message with a Bounce method.  If this flag is zero, the server
      silently drops the message. The meaning of this bit is not defined
      when a message is routed through multiple exchanges.
    </doc>
  </field>

  <field name = "immediate" type = "bit">
    request immediate delivery
    <doc>
      This flag tells the server how to react if the message cannot be
      routed to a queue consumer immediately.  If this flag is set, the
      server returns the message with a Bounce method.  If this flag is
      zero, the server queues the message, but with no guarantee that it
      will ever be consumed.  The meaning of this bit is not defined
      when a message is routed through multiple exchanges.
    </doc>
  </field>
</method>

<method name = "bounce" content = "1">
  return a failed message
  <doc>
    This method returns an undeliverable message that was published
    with the "immediate" flag set, or an unroutable message published
    with the "mandatory" flag set. The reply code and text provide
    information about the reason that the message was undeliverable.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "reply code" domain = "reply code" />
  <field name = "reply text" domain = "reply text" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was
      originally published to.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "routing key" type = "shortstr">
     Message routing key
    <doc>
      Specifies the routing key name specified when the message was
      published.
    </doc>     
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "deliver" content = "1">
  notify the client of a consumer message
  <doc>
    This method delivers a message to the client, via a consumer.  In
    the asynchronous message delivery model, the client starts a
    consumer using the Consume method, then the server responds with
    Deliver methods as and when messages arrive for that consumer.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was
      originally published to.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "routing key" type = "shortstr">
     Message routing key
    <doc>
      Specifies the routing key name specified when the message was
      published.
    </doc>     
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "get" synchronous = "1">
  direct access to a queue
  <doc>
    This method provides a direct access to the messages in a queue
    using a synchronous dialogue that is designed for specific types of
    application where functionality is more important than performance.
  </doc>
  <response name = "get-ok" />
  <response name = "get-empty" />
  <chassis name = "server" implement = "MUST" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read"
      access rights to the realm for the queue.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to get from.
    </doc>
  </field>
</method>

<method name = "get-ok" synchronous = "1" content = "1">
  provide client with a message
  <doc>
    This method delivers a message to the client following a get
    method.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was originally
      published to.  If empty, the message was published to the default
      exchange.
    </doc>
  </field>

  <field name = "routing key" type = "shortstr">
     Message routing key
    <doc>
      Specifies the routing key name specified when the message was
      published.
    </doc>
  </field>

  <field name = "message count" type = "long" >
    number of messages pending
    <doc>
      This field reports the number of messages pending on the queue,
      excluding the message being delivered.  Note that this figure is
      indicative, not reliable, and can change arbitrarily as messages
      are added to the queue and removed by other clients.
    </doc>
  </field>
</method>


<method name = "get-empty" synchronous = "1">
  indicate no messages available
  <doc>
    This method tells the client that the queue has no messages
    available for the client.
  </doc>
  <chassis name = "client" implement = "MAY" />
</method>

</class>

