<?xml version="1.0"?>
<class
    name    = "jms"
    handler = "channel"
    index   = "7"
  >
  work with JMS content

<doc>
  The JMS class provides methods that support the standard JMS API.
  JMS messages have a specific set of properties that are required for
  interoperability with JMS providers and consumers. JMS messages and
  acknowledgements are subject to transactions.
</doc>

<doc name = "grammar">
    jms                 = C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:PUBLISH content
                        / S:BOUNCE content
                        / S:DELIVER content
                        / C:BROWSE ( S:BROWSE-OK content / S:BROWSE-EMPTY )
                        / C:ACK
                        / C:REJECT
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />


<!--  These are the properties for a JMS content  -->

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
<field name = "timestamp" type = "long">
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

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "consume" synchronous = "1">
  start a queue consumer
  <doc>
    This method asks the server to start a "consumer", which is a
    transient request for messages from a specific queue. Consumers
    last as long as the channel they were created on, or until the
    client cancels them.
  </doc>
  <doc name = "rule">
    The server MAY restrict the number of consumers per channel to an
    arbitrary value, which MUST be at least 8, and MUST be specified in
    the Connection.Tune method.
  </doc>
  <doc name = "rule">
    The client MUST be able to work with the server-defined limits with
    respect to the maximum number of consumers per channel.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "consume-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read" access
      rights to the realm for the queue.
    </doc>
  </field>

  <field name = "scope" domain = "queue scope" />

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to consume from.
    </doc>
    <assert check = "notnull" />
  </field>

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
      The prefetch-size is ignored if the auto-ack option is set.
    </doc>
    <doc name = "rule">
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
      is compatible with some JMS API implementations.  This field
      may be used in combination with the prefetch-size field; a
      message will only be sent in advance if both prefetch windows
      (and those at the channel and connection level) allow it.
      The prefetch-count is ignored if the auto-ack option is set.
    </doc>
    <doc name = "rule">
      The server MAY send less data in advance than allowed by the
      client's specified prefetch windows but it MUST NOT send more.
    </doc>
  </field>

  <field name = "no local" domain = "no local" />
  <field name = "auto ack" domain = "auto ack" />

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc>
      Request exclusive consumer access.  If the server cannot grant
      this - because there are other consumers active - it raises a
      channel exception.
    </doc>
  </field>
</method>

<method name = "consume-ok" synchronous = "1">
  confirm a new consumer
  <doc>
    This method provides the client with a consumer tag which it MUST
    use in methods that work with the consumer.
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

<method name = "publish" content = "1">
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
      Specifies the name of the exchange to publish to.  If the exchange
      does not exist the server will raise a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "destination" type = "shortstr">
     Message destination name
    <doc>
      Specifies the destination for the message.  The destination is
      a name that is used for routing messages depending on the exchange
      configuration.
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
  
  <field name = "destination" type = "shortstr">
     Message destination name
    <doc>
      Specifies the destination name specified when the message was
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
  <doc name = "rule">
    The server SHOULD track the number of times a message has been
    delivered to clients and when a message is redelivered a certain
    number of times - e.g. 5 times - without being acknowledged, the
    server SHOULD consider the message to be unprocessable (possibly
    causing client applications to abort), and move the message to a
    dead letter queue.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "delivery tag" domain = "delivery tag" />
  <field name = "redelivered" domain = "redelivered" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was
      originally published to.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "destination" type = "shortstr">
     Message destination name
    <doc>
      Specifies the destination name specified when the message was
      published.
    </doc>     
  </field>

  <field name = "scope" domain = "queue scope" />
    
  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue that the message came from. Note
      that a single channel can start many consumers on different
      queues.
    </doc>
    <assert check = "notnull" />
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "browse" synchronous = "1">
  direct access to a queue
  <doc>
    This method provides a direct access to the messages in a queue
    using a synchronous dialogue that is designed for specific types of
    application where functionality is more important than performance.
  </doc>
  <response name = "browse-ok" />
  <response name = "browse empty" />
  <chassis name = "server" implement = "MUST" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read"
      access rights to the realm for the queue.
    </doc>
  </field>

  <field name = "scope" domain = "queue scope" />
    
  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to browse from.
    </doc>
  </field>

  <field name = "auto ack" domain = "auto ack" />
</method>

<method name = "browse-ok" synchronous = "1" content = "1">
  provide client with a browsed message
  <doc>
    This method delivers a message to the client following a browse
    method.  A browsed message must be acknowledged unless the
    auto-ack option was set in the Browse method.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "delivery tag" domain = "delivery tag" />
  <field name = "redelivered"  domain = "redelivered"  />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was
      originally published to.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "destination" type = "shortstr">
     Message destination name
    <doc>
      Specifies the destination name specified when the message was
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

<method name = "browse empty" synchronous = "1">
  indicate no messages available
  <doc>
    This method tells the client that the queue has no messages
    available for the client.
  </doc>
  <chassis name = "client" implement = "MAY" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "ack">
  acknowledge one or more messages
  <doc>
    This method acknowledges one or more messages delivered via the
    Deliver or Browse-Ok methods.  The client can ask to confirm a
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
    <doc name = "rule">
      The server MUST validate that a non-zero delivery-tag refers to an
      delivered message, and raise a channel exception if this is not the
      case.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "reject">
  reject an incoming message
  <doc>
    This method allows a client to reject a message.  It can be used to
    interrupt and cancel large incoming messages, or return untreatable
    messages to their original queue.
  </doc>
  <doc name = "rule">
    The server SHOULD be capable of accepting and process the Reject
    method while sending message content with a Deliver or Browse-Ok
    method.  I.e. the server should read and process incoming methods
    while sending output frames.  To cancel a partially-send content,
    the server sends a content body frame of size 1 (i.e. with no data
    except the frame-end octet).
  </doc>
  <doc name = "rule">
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
    <doc name = "rule">
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

