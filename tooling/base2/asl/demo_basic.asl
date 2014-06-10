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
    name    = "basic"
    handler = "channel"
    index   = "60"
  >
  work with basic content

<doc>
  The Basic class provides methods that support the standard basic content
  API. Basic messages have a minimal set of properties.  Trivial messages
  are not subject to transactions.
</doc>

<doc name = "grammar">
    basic               = C:PUBLISH content
                        / C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / S:DELIVER content
                        / C:GET ( S:GET-OK content / S:GET-EMPTY )
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

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
<field name = "reply to" type = "shortstr">
    The destination to reply to
</field>
<field name = "message id" type = "shortstr">
    The application message identifier
</field>
<field name = "correlation id" type = "shortstr">
    The application correlation identifier
</field>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "consume" synchronous = "1" index = "10">
  start a queue consumer
  <doc>
    This method asks the server to start a "consumer", which is a
    transient request for messages from a specific queue. Consumers
    last as long as the channel they were created on, or until the
    client cancels them.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "consume-ok" />
  <field name = "queue" domain = "queue name" />
</method>

<method name = "consume-ok" synchronous = "1" index = "11">
  confirm a new consumer
  <doc>
    The server provides the client with a consumer tag, which is used
    by the client for methods called on the consumer at a later stage.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "consumer tag" domain = "consumer tag">
    <doc>
      Holds the consumer tag provided by the server.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel" synchronous = "1" index = "20">
  end a queue consumer
  <chassis name = "server" implement = "MUST" />
  <response name = "cancel-ok" />
  <field name = "consumer tag" domain = "consumer tag" />
</method>

<method name = "cancel-ok" synchronous = "1" index = "21">
  confirm a cancelled consumer
  <doc>
    This method confirms that the cancellation was completed.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "consumer tag" domain = "consumer tag" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "publish" content = "1" index = "30">
  publish a message
  <doc>
    This method publishes a message to a specific exchange. The message
    will be routed to queues as defined by the exchange configuration
    and distributed to any active consumers when the transaction, if any,
    is committed.
  </doc>
  <chassis name = "server" implement = "MUST" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange to publish to.  If the exchange
      does not exist the server will raise a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "routing key" type = "shortstr">
     Message routing key
    <doc>
      Specifies the routing key for the message.  The routing key is
      a value that is used for routing messages depending on the exchange
      configuration.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "deliver" content = "1" index = "40">
  notify the client of a consumer message
  <doc>
    This method delivers a message to the client, via a consumer.  In
    the asynchronous message delivery model, the client starts a
    consumer using the Consume method, then the server responds with
    Deliver methods as and when messages arrive for that consumer.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "exchange" domain = "exchange name" />
  <field name = "routing key" type = "shortstr">
     Message routing key
  </field>
  <field name = "delivery tag" domain = "delivery tag" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "get" synchronous = "1" index = "50">
  direct access to a queue
  <doc>
    This method provides a direct access to the messages in a queue
    using a synchronous dialogue that is designed for specific types of
    application where functionality is more important than performance.
  </doc>
  <response name = "get-ok" />
  <response name = "get-empty" />
  <chassis name = "server" implement = "MUST" />

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to get a message from.
    </doc>
  </field>
</method>

<method name = "get-ok" synchronous = "1" content = "1" index = "51">
  provide client with a message
  <doc>
    This method delivers a message to the client following a get
    method.
  </doc>
  <chassis name = "client" implement = "MAY" />

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


<method name = "get-empty" synchronous = "1" index = "52">
  indicate no messages available
  <doc>
    This method tells the client that the queue has no messages
    available for the client.
  </doc>
  <chassis name = "client" implement = "MAY" />
</method>


</class>


