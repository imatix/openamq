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
  <include filename = "exchange.asl"      />
  <include filename = "queue.asl"         />
  <include filename = "trivial.asl"       />
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
      <field name = "method synctag"  type = "long"  standard = "1" >synctag</field>
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

  <domain name = "destination" type = "shortstr">
    destination name
    <doc>
      A destination name consists of any combination of at least one of
      [A-Za-z0-9] plus zero or more of [.-_/+!=:].
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "queue namespace" type = "shortstr">
    queue namespace
    <doc>
      The queue namespace is an arbitrary string chosen by the
      application.  The combination of queue namespace and queue name
      is unique per virtual host.  A queue namespace consists of any
      combination of at least one of [A-Za-z0-9] plus zero or more of
      [.-_/+!=:].  Queue namespaces are case-sensitive.
    </doc>
    <doc name = "rule">
      The queue namespace MAY be empty - the empty namespace acts
      just like any other name space.
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "30" />
  </domain>

  <domain name = "queue name" type = "shortstr">
    queue name
    <doc>
      The queue name identifies the queue within the namespace and the
      virtual host. A queue name consists of any combination of at least
      one of [A-Za-z0-9] plus zero or more of [.-_/+!=:]. Queue names
      starting with _ are reserved for server use.  Queue names are
      case-sensitive.
    </doc>
    <assert check = "re" value = "[A-Za-z0-9][A-Za-z0-9.\-_\/+!=:]*" />
    <assert check = "length" value = "127" />
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
