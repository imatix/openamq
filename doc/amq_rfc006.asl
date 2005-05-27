<?xml version="1.0"?>
<protocol
    name    = "amq_rfc006"
    comment = "AMQP/Fast protocol specifications"
    version = "0.9"
    script  = "asl_gen.gsl"
    target  = "doc"
    >
  <!-- Classes -->
  <include filename = "connection.asl"    />
  <include filename = "channel.asl"       />
  <include filename = "access.asl"        />
  <include filename = "destination.asl"   />
  <include filename = "subscription.asl"  />
  <include filename = "jms.asl"           />
  <include filename = "file.asl"          />
  <include filename = "stream.asl"        />
  <include filename = "tx.asl"            />
  <include filename = "dtx.asl"           />
  <include filename = "test.asl"          />

  <!-- All methods start with this header  -->
  <class name = "template" abstract = "1" inherit = "none">
    <method name = "template" abstract = "1" inherit = "none">
      <field name = "method class"    type = "octet" standard = "1" >class ID</field>
      <field name = "method id"       type = "octet" standard = "1" >method ID</field>
      <field name = "method flags"    type = "octet" standard = "1" >method flags</field>
      <field name = "method synchtag" type = "long"  standard = "1" >synchtag</field>
    </method>
  </class>

  <!-- Standard field domains -->
  <domain name = "access ticket" type = "short">
    access ticket granted by server
    <doc>
      An access ticket granted by the server for a certain set of access
      rights within a specific realm. Access tickets may be shared across
      channels within a connection and expire with the connection.
    </doc>
    <assert check = "ne" value = "0"/>
  </domain>

  <domain name = "path" type = "shortstr">
    path value
    <doc>
      Must start with a slash "/" and continue with path names
      separated by slashes. A path name consists of any combination
      of at least one of [A-Za-z0-9] plus zero or more of [.-_+!=:].
    </doc>
    <assert check = "notnull" />
    <assert check = "syntax" rule = "path" />
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "destination" type = "shortstr">
    destination name
    <doc>
      A destination name consists of any combination of at least one
      of [A-Za-z0-9] plus zero or more of [.-_/+!=:]. Destination
      names starting with _ are reserved for server use.
    </doc>
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "subscription" type = "shortstr">
    subscription name
    <doc>
      A subscription name consists of any combination of at least one
      of [A-Za-z0-9] plus zero or more of [.-_/+!=:].  Subscription
      names starting with _ are reserved for server use.
    </doc>
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "service type" type = "octet">
    destination service type
    <doc>
      Destinations are either queues or topics - this field specifies
      the type of the destination.  The "fuzzy" type searches first for
      a queue, then for a topic with the specified destination name.
    </doc>
    <assert check = "enum">
      <value name = "queue" />
      <value name = "topic" />
      <value name = "fuzzy" />
    </assert>
  </domain>

  <domain name = "topic selector" type = "shortstr">
    topic name or pattern
    <doc>
      Specifies the topics that will be routed to this subscription. 
      This field is either a complete existing topic name or a topic
      pattern using the syntax and wildcards defined by the
      topic-seltype field.
    </doc>
    <see name = "topic selector"/>
    <assert check = "notnull" />
  </domain>

  <domain name = "topic seltype" type = "octet">
    type of topic selector
    <doc>
      Specifies the syntax for the topic selector.
      The simple syntax uses a hierarchical naming scheme for topics
      where each level is delimited by a dot. The wild cards * and #
      replace one and zero or more hierarchical levels respectively.
      The tpath syntax is the same as the simple syntax but the topic
      hierarchy is delimited by slash ("/") characters.  The regexp
      syntax uses Perl 5-compatible regular expressions and allows any
      topic naming scheme.
    </doc>
    <assert check = "enum">
      <value name = "simple" />
      <value name = "tpath"  />
      <value name = "regexp" />
    </assert>
  </domain>

  <domain name = "message selector" type = "longstr">
    message selector
    <doc>
      Specifies which precise messages will be routed to this queue or
      subscription.  AMQP/Fast provides several alternative syntaxes
      for message selectors, as specified by the message-seltype field.
    </doc>
  </domain>

  <domain name = "message seltype" type = "octet">
    type of message selector
    <doc>
      Specifies the syntax for the message selector.
    </doc>
    <assert check = "enum">
      <value name = "fast" />
      <value name = "jms"  />
    </assert>
  </domain>

  <domain name = "no local" type = "bit">
    do not receive own messages
    <doc>
      If this field is set the server will not send messages to the
      client that published them.
    </doc>
  </domain>

  <domain name = "auto ack" type = "bit">
    no acknowledgement needed
    <doc>
      If this field is set the server does not expect acknowledgments
      for messages.  That is, when a message is delivered to the client
      the server automatically and silently acknowledges it on behalf
      of the client.  This functionality increases performance but at
      the cost of reliability.  Messages can get lost if a client dies
      before it can deliver them to the application.
    </doc>
  </domain>

  <domain name = "consumer tag" type = "short">
    server-assigned consumer tag
    <doc>
      The server-assigned and channel-specific consumer tag
    </doc>
    <doc name = "rule">
      This field is an arbitrary ID number for the consumer. The
      consumer tag is specific to a single channel and the client MUST
      NOT use this tag in any channel except the channel where the
      consumer was created.
    </doc>
    <assert check = "ne" value = "0" />
  </domain>

  <domain name = "delivery tag" type = "longlong">
    server-assigned delivery tag
    <doc>
      The server-assigned and channel-specific delivery tag
    </doc>
    <doc name = "rule">
      This field is an sequential ID number for the delivery. The
      delivery tag is specific to a single channel and the client MUST
      NOT use this tag in any channel except the channel where the
      message was delivered.
    </doc>
  </domain>

  <domain name = "redelivered" type = "bit">
    signal a redelivered message
    <doc>
      This indicates that the message has been previously delivered to
      this or another client.
    </doc>
    <doc name = "rule">
      The server SHOULD try to signal redelivered messages when it can.
      When redelivering a message that was not successfully acknowledged,
      the server SHOULD deliver it to the original client if possible.
    </doc>
    <doc name = "rule">
      The client MUST NOT rely on this field but MUST take it as a hint
      that the message may already have been processed.  A fully robust
      client must be able to track duplicate received messages on non
      transacted, and locally-transacted channels.
    </doc>
  </domain>

  <domain name = "reply code" type = "short">
    reply code from server
    <doc>
      The reply code. The AMQ reply codes are defined in AMQ RFC 011.
    </doc>
    <assert check = "ne" value = "0" />
  </domain>

  <domain name = "reply text" type = "shortstr">
    localised reply text
    <doc>
      The localised reply text.  This text can be logged as an aid to
      resolving issues.
    </doc>
    <assert check = "notnull" />
  </domain>
</protocol>
