<?xml version="1.0"?>
<class
    name = "stream"
    handler = "channel"
  >
  work with streaming content

<doc>
  The stream class provides methods that support multimedia streaming.
  The stream class uses the following semantics: one message is one
  packet of data; delivery is unacknowleged and unreliable; the consumer
  can specify quality of service parameters that the server can try to
  adhere to; lower-priority messages may be discarded in favour of high
  priority messages.
</doc>

<doc name = "grammar">
    stream              = C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:PUBLISH content
                        / S:DELIVER content
</doc>

<chassis name = "server" implement = "SHOULD" />
<chassis name = "client" implement = "MAY"    />

<todo owner = "ph@imatix.com">
  It may be useful to allow per-class tuning, e.g. for max. number of
  consumers per channel.
</todo>


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
  <doc name = "rule">
    Streaming applications SHOULD use different channels to select
    different streaming resolutions. AMQP/Fast makes no provision for
    filtering and/or transforming streams except on the basis of
    priority-based selective delivery of individual messages.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "consume-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read" access
      rights to the realm for the queue.
    </doc>
  </field>

  <field name = "namespace" domain = "queue namespace" />
    
  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to consume from.
    </doc>
    <assert check = "notnull" />
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
      field may be used in combination with the prefetch-size field;
      a message will only be sent in advance if both prefetch windows
      (and those at the channel and connection level) allow it.
    </doc>
  </field>

  <field name = "consume rate" type = "long">
    transfer rate in octets/second
    <doc>
      Specifies a desired transfer rate in octets per second. This is
      usually determined by the application that uses the streaming
      data.  A value of zero means "no limit", i.e. as rapidly as
      possible.
    </doc>
    <doc name = "rule">
      The server MAY ignore the prefetch values and consume rates,
      depending on the type of stream and the ability of the server
      to queue and/or reply it.  The server MAY drop low-priority
      messages in favour of high-priority messages.
    </doc>
  </field>

  <field name = "no local" domain = "no local" />

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc>
      Request exclusive consumer access.  If the server cannot grant
      this - because there are other consumers active - it raises a
      channel exception.
    </doc>
    <doc name = "rule">
      The server MUST grant clients exclusive access to a queue
      or subscription if they ask for it.
    </doc>
  </field>
</method>


<method name = "consume-ok" synchronous = "1">
  confirm a new consumer
  <doc>
    This method provides the client with a consumer tag which it may
    use in methods that work with the consumer.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel" synchronous = "1">
  end a queue consumer
  <doc>
    This method cancels a consumer.  Since message delivery is
    asynchronous the client may continue to receive messages for
    a short while after cancelling a consumer.  It may process or
    discard these as appropriate.
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
    This method publishes a message to a specific proxy. The message
    will be forwarded to all registered queues and distributed to any
    active consumers when the transaction is committed.
  </doc>
  <chassis name = "server" implement = "MUST" />
           
  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "write"
      access rights to the access realm for the proxy.
    </doc>
  </field>

  <field name = "proxy" domain = "proxy name">
    <doc>
      Specifies the name of the proxy to publish to.  If the proxy
      does not exist the server will raise a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "immediate" type = "bit">
    assert immediate delivery
    <doc>
      Asserts that the message is delivered to one or more consumers
      immediately and causes a channel exception if this is not the
      case.
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

  <field name = "proxy" domain = "proxy name">
    <doc>
      Specifies the name of the proxy that the message was originally
      published to.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "namespace" domain = "queue namespace" />
    
  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue that the message came from. Note
      that a single channel can start many consumers on different
      queues.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

</class>

