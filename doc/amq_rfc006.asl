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
      of [A-Za-z0-9] plus zero or more of [.-_/+!=:].
      </doc>
      <assert check = "length" value = "127" />
    </domain>

    <domain name = "subscription" type = "shortstr">
    subscription name
      <doc>
      A subscription name consists of any combination of at least one
      of [A-Za-z0-9] plus zero or more of [.-_/+!=:].
      </doc>
      <assert check = "length" value = "127" />
    </domain>

    <domain name = "service type" type = "octet">
    destination service type
      <doc>
      Destinations are either queues or topics - this field specifies
      the type of the destination.
      </doc>
      <assert check = "enum">
        <value name = "queue" />
        <value name = "topic" />
      </assert>
    </domain>

    <domain name = "topic selector" type = "shortstr">
    topic name or pattern
      <doc>
      Specifies the topics that will be routed to this subscription.
      This field is either a complete existing topic name or a topic
      pattern using the syntax and wildcards defined by the selector
      type field.
      </doc>
      <see name = "topic selector"/>
      <assert check = "notnull" />
    </domain>

    <domain name = "selector type" type = "octet">
    type of topic selector
      <doc>
      Specifies the syntax for the topic selector.
      </doc>
      <assert check = "enum">
        <value name = "simple" />
        <value name = "tpath"  />
        <value name = "regexp" />
      </assert>
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
