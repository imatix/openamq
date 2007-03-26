<?xml version="1.0"?>
<!--
    Copyright (c) 2007 iMatix Corporation

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

    This file contains excerpts from the AMQP specification.  The copyright
    and license for these excerpts is detailed below:

    Copyright Notice
    ================
    (c) Copyright JPMorgan Chase Bank & Co., Cisco Systems, Inc., 
    Envoy Technologies Inc., iMatix Corporation, IONA Technologies, 
    Red Hat, Inc., TWIST Process Innovations, and 29West Inc. 2006. 
    All rights reserved.
 
    License
    =======
    JPMorgan Chase Bank & Co., Cisco Systems, Inc., Envoy Technologies Inc., 
    iMatix Corporation, IONA Technologies, Red Hat, Inc., 
    TWIST Process Innovations, and 29West Inc. (collectively, the
    "Authors") each hereby grants to you a worldwide, perpetual,
    royalty-free, nontransferable, nonexclusive license to (i) copy,
    display, distribute and implement the Advanced Messaging Queue Protocol
    ("AMQP") Specification and (ii) the Licensed Claims that are held by
    the Authors, all for the purpose of implementing the Advanced Messaging
    Queue Protocol Specification. Your license and any rights under this
    Agreement will terminate immediately without notice from any Author if
    you bring any claim, suit, demand, or action related to the Advanced
    Messaging Queue Protocol Specification against any Author.  Upon
    termination, you shall destroy all copies of the Advanced Messaging
    Queue Protocol Specification in your possession or control.

    As used hereunder, "Licensed Claims" means those claims of a patent or
    patent application, throughout the world, excluding design patents and
    design registrations, owned or controlled, or that can be sublicensed
    without fee and in compliance with the requirements of this Agreement,
    by an Author or its affiliates now or at any future time and which
    would necessarily be infringed by implementation of the Advanced
    Messaging Queue Protocol Specification. A claim is necessarily
    infringed hereunder only when it is not possible to avoid infringing it
    because there is no plausible non-infringing alternative for
    implementing the required portions of the Advanced Messaging Queue
    Protocol Specification. Notwithstanding the foregoing, Licensed Claims
    shall not include any claims other than as set forth above even if
    contained in the same patent as Licensed Claims; or that read solely on
    any implementations of any portion of the Advanced Messaging Queue
    Protocol Specification that are not required by the Advanced Messaging
    Queue Protocol Specification, or that, if licensed, would require a
    payment of royalties by the licensor to unaffiliated third parties.
    Moreover, Licensed Claims shall not include (i) any enabling
    technologies that may be necessary to make or use any Licensed Product
    but are not themselves expressly set forth in the Advanced Messaging
    Queue Protocol Specification (e.g., semiconductor manufacturing
    technology, compiler technology, object oriented technology, networking
    technology, operating system technology, and the like); or (ii) the
    implementation of other published standards developed elsewhere and
    merely referred to in the body of the Advanced Messaging Queue Protocol
    Specification, or (iii) any Licensed Product and any combinations
    thereof the purpose or function of which is not required for compliance
    with the Advanced Messaging Queue Protocol Specification. For purposes
    of this definition, the Advanced Messaging Queue Protocol Specification
    shall be deemed to include both architectural and interconnection
    requirements essential for interoperability and may also include
    supporting source code artifacts where such architectural,
    interconnection requirements and source code artifacts are expressly
    identified as being required or documentation to achieve compliance
    with the Advanced Messaging Queue Protocol Specification.
 
    As used hereunder, "Licensed Products" means only those specific
    portions of products (hardware, software or combinations thereof) that
    implement and are compliant with all relevant portions of the Advanced
    Messaging Queue Protocol Specification.
 
    The following disclaimers, which you hereby also acknowledge as to any
    use you may make of the Advanced Messaging Queue Protocol
    Specification:
 
    THE ADVANCED MESSAGING QUEUE PROTOCOL SPECIFICATION IS PROVIDED "AS
    IS," AND THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR
    IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, OR TITLE; THAT THE
    CONTENTS OF THE ADVANCED MESSAGING QUEUE PROTOCOL SPECIFICATION ARE
    SUITABLE FOR ANY PURPOSE; NOR THAT THE IMPLEMENTATION OF THE ADVANCED
    MESSAGING QUEUE PROTOCOL SPECIFICATION WILL NOT INFRINGE ANY THIRD
    PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
 
    THE AUTHORS WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL,
    INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR RELATING TO ANY
    USE, IMPLEMENTATION OR DISTRIBUTION OF THE ADVANCED MESSAGING QUEUE
    PROTOCOL SPECIFICATION.
 
    The name and trademarks of the Authors may NOT be used in any manner,
    including advertising or publicity pertaining to the Advanced Messaging
    Queue Protocol Specification or its contents without specific, written
    prior permission. Title to copyright in the Advanced Messaging Queue
    Protocol Specification will at all times remain with the Authors.
 
    No other rights are granted by implication, estoppel or otherwise.
 
    Upon termination of your license or rights under this Agreement, you
    shall destroy all copies of the Advanced Messaging Queue Protocol
    Specification in your possession or control.
 
    Trademarks
    ==========
    "JPMorgan", "JPMorgan Chase", "Chase", the JPMorgan Chase logo and the
    Octagon Symbol are trademarks of JPMorgan Chase & Co.
 
    IMATIX and the iMatix logo are trademarks of iMatix Corporation sprl.
 
    IONA, IONA Technologies, and the IONA logos are trademarks of 
    IONA Technologies PLC and/or its subsidiaries.
 
    LINUX is a trademark of Linus Torvalds. RED HAT and JBOSS are
    registered trademarks of Red Hat, Inc. in the US and other countries.
 
    Java, all Java-based trademarks and OpenOffice.org are trademarks of
    Sun Microsystems, Inc. in the United States, other countries, or both.
 
    Other company, product, or service names may be trademarks or service
    marks of others.
 
    Links to full AMQP specification:
    =================================
    http://www.amqp.org/
-->

<class
    name    = "stream"
    handler = "channel"
    index   = "80"
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
    stream              = C:QOS S:QOS-OK
                        / C:CONSUME S:CONSUME-OK
                        / C:CANCEL S:CANCEL-OK
                        / C:PUBLISH content
                        / S:RETURN
                        / S:DELIVER content
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<doc name = "rule">
  The server SHOULD discard stream messages on a priority basis if
  the queue size exceeds some configured limit.
</doc>
<doc name = "rule">
  The server MUST implement at least 2 priority levels for stream
  messages, where priorities 0-4 and 5-9 are treated as two distinct
  levels. The server MAY implement up to 10 priority levels.
</doc>
<doc name = "rule">
  The server MUST implement automatic acknowledgements on stream
  content.  That is, as soon as a message is delivered to a client
  via a Deliver method, the server must remove it from the queue.
</doc>


<!--  These are the properties for a Stream content  -->

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
<field name = "timestamp" type = "timestamp">
    The message timestamp
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
  <doc name = "rule">
    The server SHOULD support at least 16 consumers per queue, unless
    the queue was declared as private, and ideally, impose no limit
    except as defined by available resources.
  </doc>
  <doc name = "rule">
    Streaming applications SHOULD use different channels to select
    different streaming resolutions. AMQP makes no provision for
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
    This method provides the client with a consumer tag which it may
    use in methods that work with the consumer.
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
  <doc>
    This method cancels a consumer.  Since message delivery is
    asynchronous the client may continue to receive messages for
    a short while after canceling a consumer.  It may process or
    discard these as appropriate.
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
      The exchange MAY refuse stream content in which case it MUST
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
      routed to a queue.  If this flag is set, the server will return an
      unroutable message with a Return method.  If this flag is zero, the
      server silently drops the message.
    </doc>
    <doc name = "rule" test = "amq_stream_00">
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
    <doc name = "rule" test = "amq_stream_00">
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
  <chassis name = "client" implement = "MUST" />

  <field name = "consumer tag" domain = "consumer tag" />

  <field name = "delivery tag" domain = "delivery tag" />

  <field name = "exchange" domain = "exchange name">
    <doc>
      Specifies the name of the exchange that the message was originally
      published to.
    </doc>
  </field>

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

