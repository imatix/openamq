<?xml version="1.0"?>
<class
    name = "jms"
    handler = "channel"
  >
  work with JMS content

<doc>
  The JMS class provides methods that support the standard JMS API.
  JMS messages have a specific set of properties that are required for
  interoperability with JMS providers and consumers. JMS messages and
  acknowledgements are subject to channel transactions.
</doc>

<doc name = "grammar">
    jms                 = C:CONSUME S:CONSUME_OK
                        / C:CANCEL
                        / C:PUBLISH content
                        / S:DELIVER content
                        / C:BROWSE ( S:BROWSE_OK content / S:BROWSE_EMPTY )
                        / C:ACK
                        / C:REJECT
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "consume" synchronous = "1">
  start a destination consumer
  <doc>
    This method asks the server to start a "consumer", which is a
    temporary request for messages from a specific queue or topic
    subscription.  Consumers last as long as the channel they were
    created on, or until they are cancelled.
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
  <response name = "consume ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "consume"
      access rights to the access realm for the destination or
      subscription.
    </doc>
  </field>

  <field name = "service type" domain = "service type" />
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the queue or topic to consume from.
    </doc>
    <doc name = "rule">
      For queue consumers, the destination field MUST be provided. For
      topic consumers, the destination field MAY be provided but MUST
      be empty if a subscription is specified.  Note that the explicit
      use of a subscription already defines the topic destination to
      use.
    </doc>
  </field>

  <field name = "subscription" domain = "subscription">
    <doc>
      Specifies the name of the subscription, for topic consumers.
    </doc>
    <doc name = "rule">
      The subscription field MUST be empty for queue consumers and
      MAY be empty for topic consumers, in which case the server
      MUST create an unnamed, private, and temporary subscription
      automatically.  This automatic subscription is cancelled when
      the client cancels the consumer or closes the channel.
    </doc>
    <doc name = "rule">
      The client MUST define the subscription before starting a
      consumer on it.
    </doc>
  </field>
    
  <field name = "prefetch size" type = "short">
    prefetch window in octets
    <doc>
      The client can request that messages be sent in advance so that
      when the client finishes processing a message, the following
      message is already held locally, rather than needing to be sent
      down the channel.  Prefetching gives a performance improvement.
      This field specifies the prefetch window size in octets. May be
      set to zero, meaning "no specific limit".  Note that other
      prefetch limits may still apply.
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
    <doc name = "rule">
      The server MUST grant clients exclusive access to a destination
      or subscription if they ask for it.
    </doc>
  </field>

  <field name = "message selector" domain = "message selector">
    <doc>
      This field is only used when consuming from queues.
    </doc>
    <doc name = "rule">
      The server MUST ignore the message-selector field when a client
      consumes from a subscription.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message seltype" domain = "message seltype">
    <doc name = "rule">
      The server MUST support the simple syntax and SHOULD support the
      JMS syntax.
    </doc>
  </field>

  <field name = "auto cancel" type = "long">
    cancel consumer after N messages
    <doc>
      This field causes the consumer to self-cancel after a specific
      number of messages have been delivered and acknowledged.  If zero,
      the consumer expires when the client sends the Jms.Cancel method,
      or closes the channel.
    </doc>
  </field>
</method>

<method name = "consume ok" synchronous = "1">
  confirm a new consumer
  <doc>
    This method provides the client with a consumer tag which it may
    use in methods that work with the consumer.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel">
  end a destination consumer
  <doc>
    This method cancels a consumer. This does not affect already
    delivered messages, but it does mean the server will not send
    any more messages for that consumer.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "publish" content = "1">
  publish a message to a destination
  <doc>
    This method publishes a message to a specific destination.  The
    message will be saved to the destination and distributed to any
    consumers when the transaction is committed.
  </doc>
  <chassis name = "server" implement = "MUST" />
           
  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "publish"
      access rights to the access realm for the destination or
      subscription.
    </doc>
  </field>

  <field name = "service type" domain = "service type" />
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination to publish to.  If the
      destination does not exist the server will raise a channel
      exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "immediate" type = "bit">
    assert immediate delivery
    <doc>
      Asserts that the destination has one or more consumers (for
      queues) or one or more subscriptions (for topics), and causes a
      channel exception if this is not the case.  Note that a consumer
      can query the state of a destination before publishing to it.
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

  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination that the message came from.
      Note that a single channel can start many consumers on different
      destinations, and a single subscription can collect messages from
      several topics.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "browse" synchronous = "1">
  direct access to a destination
  <doc>
    This method provides a direct access to the messages in a queue or
    subscription using a synchronous dialogue that is designed for
    specific types of application where functionality is more important
    than performance.
  </doc>
  <response name = "browse ok" />
  <response name = "browse empty" />
  <chassis name = "server" implement = "MUST" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "consume"
      access rights to the access realm for the destination or
      subscription.
    </doc>
  </field>

  <field name = "service type" domain = "service type" />
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination to browse from. This
      field is used when the service type is "queue".
    </doc>
  </field>

  <field name = "subscription" domain = "subscription">
    <doc>
      Specifies the name of the subscription to browse from. This
      field is used when the service type is "topic".  It is not
      possible to browse an automatic subscription.
    </doc>
  </field>

  <field name = "no local" domain = "no local" />
  <field name = "auto ack" domain = "auto ack" />
</method>

<method name = "browse ok" synchronous = "1" content = "1">
  provide client with a browsed message
  <doc>
    This method delivers a message to the client following a browse
    method.  A browsed message will need to be acknowkedged, unless
    the auto-ack option was set to one.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "delivery tag" domain = "delivery tag" />
  <field name = "redelivered"  domain = "redelivered"  />

  <field name = "destination" domain = "destination">
    <doc>
      In the case of browsing a subscription, this holds the original
      topic to which the message was sent.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message count" type = "long" >
    number of messages pending
    <doc>
      This field reports the number of messages pending on the queue
      or subscription, excluding the message being delivered.  Note
      that this figure is indicative, not reliable, and can change
      arbitrarily as messages are added to the queue/subscription and
      removed by other clients.
    </doc>      
  </field>
</method>

<method name = "browse empty" synchronous = "1">
  indicate no messages available
  <doc>
    This method tells the client that the queue or subscription has no
    messages available for the client.
  </doc>
  <chassis name = "client" implement = "MUST" />
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
    cancel large incoming messages, or return unprocessable messages to
    their original queue or subscription.
  </doc>
  <doc name = "rule">
    The server SHOULD be capable of accepting and process the Reject
    method while sending message content with a Deliver or Browse-Ok
    method.  I.e. the server should read and process incoming methods
    while sending output frames.
  </doc>
  <doc name = "rule">
    The server SHOULD interpret this method as meaning that the client
    is unable to process the message at this time.
  </doc>
  <doc name = "rule">
    A client MUST NOT use this method as a means of selecting message
    to process.  A rejected message MAY be discarded or dead-lettered.
  </doc>      
  <chassis name = "server" implement = "MUST" />
    
  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "requeue" type = "bit">
    requeue the message
    <doc>
      If this field is zero, the message will be discarded.  If this
      bit is 1, the message will be requeued.
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

