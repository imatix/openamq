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

<!--
<include filename = "channel.asl"       />
<include filename = "access.asl"        />
<include filename = "destination.asl"   />
<include filename = "subscription.asl"  />
<include filename = "jms.asl"           />
<include filename = "file.asl"          />
<include filename = "stream.asl"        />
<include filename = "xa.asl"            />
<include filename = "test.asl"          />
-->

<!-- All methods start with this header  -->
<class name = "template" abstract = "1">
    <method name = "template" abstract = "1">
        <field name = "method class"     type = "octet"  >Method class</field>
        <field name = "method id"        type = "octet"  >Method ID</field>
        <field name = "method revision"  type = "octet"  >Method revision</field>
        <field name = "method flags"     type = "octet"  >Method flags</field>
        <field name = "method synchtag"  type = "long"   >Method synchtag</field>
    </method>
</class>

</protocol>
