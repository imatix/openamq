<?xml version="1.0"?>
<class
    name    = "trivial"
    handler = "channel"
    index   = "6"
  >
  work with trivial content

<doc>
  The Trivial class provides methods that support the standard Trivial API.
  Trivial messages have a specific set of properties that are required for
  interoperability with trivial providers and consumers. Trivial messages and
  acknowledgements are subject to channel transactions.
</doc>

<doc name = "grammar">
    trivial             = C:PUBLISH content
                        / C:BROWSE ( S:BROWSE-OK content / S:BROWSE_EMPTY )
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />


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

  <field name = "namespace" domain = "queue namespace" />

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to browse from.
    </doc>
  </field>

  <field name = "no local" domain = "no local" />
</method>


<method name = "browse-ok" synchronous = "1" content = "1">
  provide client with a browsed message
  <doc>
    This method delivers a message to the client following a browse
    method.  A browsed message will need to be acknowkedged, unless
    the auto-ack option was set to one.
  </doc>
  <chassis name = "client" implement = "MAY" />

  <field name = "redelivered"  domain = "redelivered"  />

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

</class>

