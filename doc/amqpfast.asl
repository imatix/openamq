<protocol
    name    = "AMQP/Fast"
    version = "0.9"
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
<include filename = "xa.asl"            />
<include filename = "test.asl"          />

<!-- All methods start with this header  -->
<method name = "default" abstract = "1">
    <argument name = "method class"     type = "octet"  >Method class</argument>
    <argument name = "method id"        type = "octet"  >Method ID</argument>
    <argument name = "method revision"  type = "octet"  >Method revision</argument>
    <argument name = "method flags"     type = "octet"  >Method flags</argument>
    <argument name = "method synchtag"  type = "long"   >Method synchtag</argument>
</method>
