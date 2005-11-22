<?xml version="1.0"?>
<protocol
    name    = "amq_rfc006"
    comment = "AMQP protocol specifications"
    version = "0.9"
    script  = "asl_gen"
    >
  <inherit filename = "asl_base.asl" />
  <!-- Classes -->
  <include filename = "access.asl"   />
  <include filename = "exchange.asl" />
  <include filename = "queue.asl"    />
  <include filename = "trivial.asl"  />
  <include filename = "jms.asl"      />
  <include filename = "file.asl"     />
  <include filename = "stream.asl"   />
  <include filename = "tx.asl"       />
  <include filename = "dtx.asl"      />
  <include filename = "test.asl"     />

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

  <domain name = "exchange name" type = "shortstr">
    exchange name
    <doc>
      This name identifies the exchange within the virtual host.  A
      exchange name consists of any combination of at least one of
      [A-Za-z0-9] plus zero or more of [.-_/+!=:].  Exchange names are
      case-sensitive.
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "queue domain" type = "shortstr">
    queue name domain
    <doc>
      The queue domain allows different functional domains to get
      their own namespaces.  For instance, subscriptions can be kept
      separate from temporary reply queues.  The domain is an arbitrary
      string value selected by the application.  The domain may be
      empty, or may consist of any combination of at least one of
      [A-Za-z0-9] plus zero or more of [.-_/+!=:].  Queue domains are
      case-sensitive.
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "30" />
  </domain>

  <domain name = "queue name" type = "shortstr">
    queue name
    <doc>
      The queue name identifies the queue within the domain and the
      virtual host. A queue name consists of any combination of at least
      one of [A-Za-z0-9] plus zero or more of [.-_/+!=:]. Queue names
      starting with _ are reserved for server use.  Queue names are
      case-sensitive.
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "127" />
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
    message is being redelivered
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
</protocol>
