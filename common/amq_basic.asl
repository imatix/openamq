<?xml version="1.0"?>
<!--
    Please see AMQP specification for copyrights.
 
    Links to full AMQP specification:
    =================================
    http://www.amqp.org/
-->

<class
    name    = "basic"
    handler = "channel"
    index   = "60"
  >
  work with basic content
<doc>
  The Basic class provides methods that support an industry-standard
  messaging model.
</doc>

<doc name = "grammar">
    basic               = C:QOS S:QOS-OK
                        / C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:PUBLISH content
                        / S:RETURN content
                        / S:DELIVER content
                        / C:GET ( S:GET-OK content / S:GET-EMPTY )
                        / C:ACK
                        / C:REJECT
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

<doc name = "rule" test = "amq_basic_08">
  The server SHOULD respect the persistent property of basic messages
  and SHOULD make a best-effort to hold persistent basic messages on a
  reliable storage mechanism.
</doc>
<doc name = "rule" test = "amq_basic_09">
  The server MUST NOT discard a persistent basic message in case of a
  queue overflow. The server MAY use the Channel.Flow method to slow
  or stop a basic message publisher when necessary.
</doc>
<doc name = "rule" test = "amq_basic_10">
  The server MAY overflow non-persistent basic messages to persistent
  storage and MAY discard or dead-letter non-persistent basic messages
  on a priority basis if the queue size exceeds some configured limit.
</doc>
<doc name = "rule" test = "amq_basic_11">
  The server MUST implement at least 2 priority levels for basic
  messages, where priorities 0-4 and 5-9 are treated as two distinct
  levels. The server MAY implement up to 10 priority levels.
</doc>
<doc name = "rule" test = "amq_basic_12">
  The server MUST deliver messages of the same priority in order
  irrespective of their individual persistence.
</doc>
<doc name = "rule" test = "amq_basic_13">
  The server MUST support both automatic and explicit acknowledgements
  on Basic content.
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
<field name = "delivery mode" type = "octet">
    Non-persistent (1) or persistent (2)
</field>
<field name = "priority" type = "octet">
    The message priority, 0 to 9
</field>
<field name = "correlation id" type = "shortstr">
    The application correlation identifier
</field>
<field name = "reply to" type = "shortstr">
    The destination to reply to
</field>
<field name = "expiration" type = "shortstr">
    Message expiration specification
</field>
<field name = "message id" type = "shortstr">
    The application message identifier
</field>
<field name = "timestamp" type = "timestamp">
    The message timestamp
</field>
<field name = "type" type = "shortstr">
    The message type name
</field>
<field name = "user id" type = "shortstr">
    The creating user id
</field>
<field name = "app id" type = "shortstr">
    The creating application id
</field>
<field name = "sender id" type = "shortstr">
    Identifier of connection sending the message
</field>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "qos" synchronous = "1" index = "10">
  specify quality of service
  <doc>
    This method requests a specific quality of service.  The QoS can
    be specified for the current channel or for all channels on the
    connection.  The particular properties and semantics of a qos method
    always depend on the content class semantics.  Though the qos method
    could in principle apply to both peers, it is currently meaningful
    only for the server.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "qos-ok" />

  <field name = "prefetch size" type = "long">
    prefetch window in octets
    <doc>
      The client can request that messages be sent in advance so that
      when the client finishes processing a message, the following
      message is already held locally, rather than needing to be sent
      down the channel.  Prefetching gives a performance improvement.
      This field specifies the prefetch window size in octets.  The
      server will send a message in advance if it is equal to or
      smaller in size than the available prefetch size (and also falls
      into other prefetch limits). May be set to zero, meaning "no
      specific limit", although other prefetch limits may still apply.
      The prefetch-size is ignored if the no-ack option is set.
    </doc>
    <doc name = "rule" test = "amq_basic_17">
      The server MUST ignore this setting when the client is not
      processing any messages - i.e. the prefetch size does not limit
      the transfer of single messages to a client, only the sending in
      advance of more messages while the client still has one or more
      unacknowledged messages.
   </doc>
  </field>

  <field name = "prefetch count" type = "short">
    prefetch window in messages
    <doc>
      Specifies a prefetch window in terms of whole messages.  This
      field may be used in combination with the prefetch-size field;
      a message will only be sent in advance if both prefetch windows
      (and those at the channel and connection level) allow it.
      The prefetch-count is ignored if the no-ack option is set.
    </doc>
    <doc name = "rule" test = "amq_basic_18">
      The server MAY send less data in advance than allowed by the
      client's specified prefetch windows but it MUST NOT send more.
    </doc>
  </field>

  <field name = "global" type = "bit">
    apply to entire connection
    <doc>
      By default the QoS settings apply to the current channel only.  If
      this field is set, they are applied to the entire connection.
    </doc>
  </field>
</method>

<method name = "qos-ok" synchronous = "1" index = "11">
  confirm the requested qos
  <doc>
    This method tells the client that the requested QoS levels could
    be handled by the server.  The requested QoS applies to all active
    consumers until a new QoS is defined.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "consume" synchronous = "1" index = "20">
  start a queue consumer
  <doc>
    This method asks the server to start a "consumer", which is a
    transient request for messages from a specific queue. Consumers
    last as long as the channel they were created on, or until the
    client cancels them.
  </doc>
  <doc name = "rule" test = "amq_basic_01">
    The server SHOULD support at least 16 consumers per queue, unless
    the queue was declared as private, and ideally, impose no limit
    except as defined by available resources.
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
      Specifies the name of the queue to consume from.  If the queue name
      is null, refers to the current queue for the channel, which is the
      last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue name
      in this method is empty, the server MUST raise a connection exception
      with reply code 530 (not allowed).
    </doc>
  </field>

  <field name = "consumer tag" domain = "consumer tag">
    <doc>
      Specifies the identifier for the consumer. The consumer tag is
      local to a connection, so two clients can use the same consumer
      tags. If this field is empty the server will generate a unique
      tag.
    </doc>
    <doc name = "rule" test = "todo">
      The tag MUST NOT refer to an existing consumer. If the client
      attempts to create two consumers with the same non-empty tag
      the server MUST raise a connection exception with reply code
      530 (not allowed).
    </doc>
  </field>

  <field name = "no local" domain = "no local" />

  <field name = "no ack" domain = "no ack" />

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc>
      Request exclusive consumer access, meaning only this consumer can
      access the queue.
    </doc>
    <doc name = "rule" test = "amq_basic_02">
      If the server cannot grant exclusive access to the queue when asked,
      - because there are other consumers active - it MUST raise a channel
      exception with return code 403 (access refused).
    </doc>
  </field>

  <field name = "nowait" type = "bit">
    do not send a reply method
    <doc>
      If set, the server will not respond to the method. The client should
      not wait for a reply method.  If the server could not complete the
      method it will raise a channel or connection exception.
    </doc>
  </field>

  <field name = "arguments" type = "table">
    arguments for consume
    <doc>
      A set of arguments for the consume.  The syntax and semantics
      depends on the server implementation.
    </doc>
  </field>
</method>

<method name = "consume-ok" synchronous = "1" index = "21">
  confirm a new consumer
  <doc>
    The server provides the client with a consumer tag, which is used
    by the client for methods called on the consumer at a later stage.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag">
    <doc>
      Holds the consumer tag specified by the client or provided by
      the server.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel" synchronous = "1" index = "30">
  end a queue consumer
  <doc test = "amq_basic_04">
    This method cancels a consumer. This does not affect already
    delivered messages, but it does mean the server will not send any
    more messages for that consumer.  The client may receive an
    abitrary number of messages in between sending the cancel method
    and receiving the cancel-ok reply.
  </doc>
  <doc name = "rule" test = "todo">
    If the queue no longer exists when the client sends a cancel command,
    or the consumer has been cancelled for other reasons, this command
    has no effect.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "cancel-ok" />

  <field name = "consumer tag" domain = "consumer tag" />

  <field name = "nowait" type = "bit">
    do not send a reply method
    <doc>
    If set, the server will not respond to the method. The client should
    not wait for a reply method.  If the server could not complete the
    method it will raise a channel or connection exception.
    </doc>
  </field>
</method>

<method name = "cancel-ok" synchronous = "1" index = "31">
  confirm a cancelled consumer
  <doc>
    This method confirms that the cancellation was completed.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "publish" content = "1" index = "40">
  publish a message
  <doc>
    This method publishes a message to a specific exchange. The message
    will be routed to queues as defined by the exchange configuration
    and distributed to any active consumers when the transaction, if any,
    is committed.
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
    <doc name = "rule" test = "amq_basic_06">
      The server MUST accept a blank exchange name to mean the default
      exchange.
    </doc>
    <doc name = "rule" test = "amq_basic_14">
      If the exchange was declared as an internal exchange, the server
      MUST raise a channel exception with a reply code 403 (access
      refused).
    </doc>
    <doc name = "rule" test = "amq_basic_15">
      The exchange MAY refuse basic content in which case it MUST raise
      a channel exception with reply code 540 (not implemented).
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
      routed to a queue.  If this flag is set, the server will return an
      unroutable message with a Return method.  If this flag is zero, the
      server silently drops the message.
    </doc>
    <doc name = "rule" test = "amq_basic_07">
      The server SHOULD implement the mandatory flag.
    </doc>
  </field>

  <field name = "immediate" type = "bit">
    request immediate delivery
    <doc>
      This flag tells the server how to react if the message cannot be
      routed to a queue consumer immediately.  If this flag is set, the
      server will return an undeliverable message with a Return method.
      If this flag is zero, the server will queue the message, but with
      no guarantee that it will ever be consumed.
    </doc>
    <doc name = "rule" test = "amq_basic_16">
      The server SHOULD implement the immediate flag.
    </doc>
  </field>
</method>

<method name = "return" content = "1" index = "50">
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

<method name = "deliver" content = "1" index = "60">
  notify the client of a consumer message
  <doc>
    This method delivers a message to the client, via a consumer.  In
    the asynchronous message delivery model, the client starts a
    consumer using the Consume method, then the server responds with
    Deliver methods as and when messages arrive for that consumer.
  </doc>
  <doc name = "rule" test = "amq_basic_19">
    The server SHOULD track the number of times a message has been
    delivered to clients and when a message is redelivered a certain
    number of times - e.g. 5 times - without being acknowledged, the
    server SHOULD consider the message to be unprocessable (possibly
    causing client applications to abort), and move the message to a
    dead letter queue.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />

  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "redelivered" domain = "redelivered" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was
      originally published to.
    </doc>
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

<method name = "get" synchronous = "1" index = "70">
  direct access to a queue
  <doc>
    This method provides a direct access to the messages in a queue
    using a synchronous dialogue that is designed for specific types of
    application where synchronous functionality is more important than
    performance.
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
      Specifies the name of the queue to consume from.  If the queue name
      is null, refers to the current queue for the channel, which is the
      last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue name
      in this method is empty, the server MUST raise a connection exception
      with reply code 530 (not allowed).
    </doc>
  </field>

  <field name = "no ack" domain = "no ack" />
</method>

<method name = "get-ok" synchronous = "1" content = "1" index = "71">
  provide client with a message
  <doc>
    This method delivers a message to the client following a get
    method.  A message delivered by 'get-ok' must be acknowledged
    unless the no-ack option was set in the get method.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "redelivered"  domain = "redelivered"  />

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

<method name = "get-empty" synchronous = "1" index = "72">
  indicate no messages available
  <doc>
    This method tells the client that the queue has no messages
    available for the client.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "sender id" type = "shortstr">
     Sender id
    <doc>
      For use by cluster applications, should not be used by
      client applications.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "ack" index = "80">
  acknowledge one or more messages
  <doc>
    This method acknowledges one or more messages delivered via the
    Deliver or Get-Ok methods.  The client can ask to confirm a
    single message or a set of messages up to and including a specific
    message.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "multiple" type = "bit">
    acknowledge multiple messages
    <doc>
      If set to 1, the delivery tag is treated as "up to and including",
      so that the client can acknowledge multiple messages with a single
      method.  If set to zero, the delivery tag refers to a single
      message.  If the multiple field is 1, and the delivery tag is zero,
      tells the server to acknowledge all outstanding mesages.
    </doc>
    <doc name = "rule" test = "amq_basic_20">
      The server MUST validate that a non-zero delivery-tag refers to an
      delivered message, and raise a channel exception if this is not the
      case.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "reject" index = "90">
  reject an incoming message
  <doc>
    This method allows a client to reject a message.  It can be used to
    interrupt and cancel large incoming messages, or return untreatable
    messages to their original queue.
  </doc>
  <doc name = "rule" test = "amq_basic_21">
    The server SHOULD be capable of accepting and process the Reject
    method while sending message content with a Deliver or Get-Ok
    method.  I.e. the server should read and process incoming methods
    while sending output frames.  To cancel a partially-send content,
    the server sends a content body frame of size 1 (i.e. with no data
    except the frame-end octet).
  </doc>
  <doc name = "rule" test = "amq_basic_22">
    The server SHOULD interpret this method as meaning that the client
    is unable to process the message at this time.
  </doc>
  <doc name = "rule">
    A client MUST NOT use this method as a means of selecting messages
    to process.  A rejected message MAY be discarded or dead-lettered,
    not necessarily passed to another client.
  </doc>      
  <chassis name = "server" implement = "MUST" />
    
  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "requeue" type = "bit">
    requeue the message
    <doc>
      If this field is zero, the message will be discarded.  If this bit
      is 1, the server will attempt to requeue the message.
    </doc>
    <doc name = "rule" test = "amq_basic_23">
      The server MUST NOT deliver the message to the same client within
      the context of the current channel.  The recommended strategy is
      to attempt to deliver the message to an alternative consumer, and
      if that is not possible, to move the message to a dead-letter
      queue.  The server MAY use more sophisticated tracking to hold
      the message on the queue and redeliver it to the same client at
      a later stage.
    </doc>
  </field>
</method>

</class>
