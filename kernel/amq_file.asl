<?xml version="1.0"?>
<class
    name    = "file"
    handler = "channel"
    index   = "7"
  >
  work with file content

<doc>
  The file class provides methods that support reliable file transfer.
  File messages have a specific set of properties that are required for
  interoperability with file transfer applications. File messages and
  acknowledgements are subject to channel transactions.  Note that the
  file class does not provide message browsing methods; these are not
  compatible with the staging model.  Applications that need browsable
  file transfer should use JMS content and the JMS class.
</doc>

<doc name = "grammar">
    file                = C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:OPEN S:OPEN-OK C:STAGE content
                        / S:OPEN C:OPEN-OK S:STAGE content
                        / C:PUBLISH
                        / S:DELIVER
                        / S:RETURN
                        / C:ACK
                        / C:REJECT
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<doc name = "rule">
  The server MUST make a best-effort to hold file messages on a
  reliable storage mechanism.
</doc>
<doc name = "rule">
  The server MUST NOT discard a file message in case of a queue
  overflow. The server MUST use the Channel.Flow method to slow or stop
  a file message publisher when necessary.
</doc>
<doc name = "rule">
  The server MUST implement at least 2 priority levels for file
  messages, where priorities 0-4 and 5-9 are treated as two distinct
  levels. The server MAY implement up to 10 priority levels.
</doc>
<doc name = "rule">
  The server MUST support both automatic and explicit acknowledgements
  on file content.
</doc>

<!--  These are the properties for a File content  -->

<field name = "content type" type = "shortstr">
    MIME content type
</field>
<field name = "content encoding" type = "shortstr">
    MIME content encoding
</field>
<field name = "headers" type = "table">
    Message header field table
</field>
<field name = "priority" type = "octet">
    The message priority, 0 to 9
</field>
<field name = "reply to" type = "shortstr">
    The destination to reply to
</field>
<field name = "message id" type = "shortstr">
    The application message identifier
</field>
<field name = "filename" type = "shortstr">
    The message filename
</field>
<field name = "timestamp" type = "timestamp">
    The message timestamp
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
      This field specifies the prefetch window size in octets. May be
      set to zero, meaning "no specific limit".  Note that other
      prefetch limits may still apply. The prefetch-size is ignored
      if the auto-ack option is set.
    </doc>
  </field>

  <field name = "prefetch count" type = "short">
    prefetch window in messages
    <doc>
      Specifies a prefetch window in terms of whole messages.  This
      is compatible with some file API implementations.  This field
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
      Request exclusive consumer access, meaning only this consumer can
      access the queue.
    </doc>
    <doc name = "rule" test = "amq_file_00">
      If the server cannot grant exclusive access to the queue when asked,
      - because there are other consumers active - it MUST raise a channel
      exception with return code 405 (resource locked).
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
    more messages for that consumer.
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

<method name = "open" synchronous = "1">
  request to start staging
  <doc>
    This method requests permission to start staging a message.  Staging
    means sending the message into a temporary area at the recipient end
    and then delivering the message by referring to this temporary area.
    Staging is how the protocol handles partial file transfers - if a
    message is partially staged and the connection breaks, the next time
    the sender starts to stage it, it can restart from where it left off.
  </doc>
  <response name = "open-ok" />
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />
  
  <field name = "identifier" type = "shortstr">
    staging identifier
    <doc>
      This is the staging identifier. This is an arbitrary string chosen
      by the sender.  For staging to work correctly the sender must use
      the same staging identifier when staging the same message a second
      time after recovery from a failure.  A good choice for the staging
      identifier would be the SHA1 hash of the message properties data
      (including the original filename, revised time, etc.).
    </doc>
  </field>

  <field name = "content size" type = "longlong">
    message content size
    <doc>
      The size of the content in octets.  The recipient may use this
      information to allocate or check available space in advance, to
      avoid "disk full" errors during staging of very large messages.
    </doc>
    <doc name = "rule">
      The sender MUST accurately fill the content-size field.
      Zero-length content is permitted.
    </doc>
  </field>
</method>

<method name = "open-ok" synchronous = "1">
  confirm staging ready
  <doc>
    This method confirms that the recipient is ready to accept staged
    data.  If the message was already partially-staged at a previous
    time the recipient will report the number of octets already staged.
  </doc>
  <response name = "stage" />
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />
  
  <field name = "staged size" type = "longlong">
    already staged amount
    <doc>
      The amount of previously-staged content in octets.  For a new
      message this will be zero.
    </doc>
    <doc name = "rule">
      The sender MUST start sending data from this octet offset in the
      message, counting from zero.
    </doc>
    <doc name = "rule">
      The recipient MAY decide how long to hold partially-staged content
      and MAY implement staging by always discarding partially-staged
      content.  However if it uses the file content type it MUST support
      the staging methods.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "stage" content = "1">
  stage message content
  <doc>
    This method stages the message, sending the message content to the
    recipient from the octet offset specified in the Open-Ok method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "publish">
  publish a message
  <doc>
    This method publishes a staged file message to a specific exchange.
    The file message will be routed to queues as defined by the exchange
    configuration and distributed to any active consumers when the
    transaction, if any, is committed.
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
      The exchange MAY refuse file content in which case it MUST respond
      with a reply code 540 (not implemented) and raise a channel
      exception.
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
    <doc name = "rule" test = "amq_file_00">
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
    <doc name = "rule" test = "amq_file_00">
      The server SHOULD implement the immediate flag.
    </doc>
  </field>

  <field name = "identifier" type = "shortstr">
    staging identifier
    <doc>
      This is the staging identifier of the message to publish.  The
      message must have been staged.  Note that a client can send the
      Publish method asynchronously without waiting for staging to
      finish.
    </doc>
  </field>
</method>

<method name = "return" content = "1">
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

<method name = "deliver">
  notify the client of a consumer message
  <doc>
    This method delivers a staged file message to the client, via a
    consumer.  In the asynchronous message delivery model, the client
    starts a consumer using the Consume method, then the server
    responds with Deliver methods as and when messages arrive for
    that consumer.
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

  <field name = "consumer tag" domain = "consumer tag" />

  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "redelivered" domain = "redelivered" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was originally
      published to.
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

  <field name = "identifier" type = "shortstr">
    staging identifier
    <doc>
      This is the staging identifier of the message to deliver.  The
      message must have been staged.  Note that a server can send the
      Deliver method asynchronously without waiting for staging to
      finish.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "ack">
  acknowledge one or more messages
  <doc>
    This method acknowledges one or more messages delivered via the
    Deliver method.  The client can ask to confirm a single message or
    a set of messages up to and including a specific message.
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
    return untreatable messages to their original queue.  Note that file
    content is staged before delivery, so the client will not use this
    method to interrupt delivery of a large message.
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
