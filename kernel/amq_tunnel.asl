<?xml version="1.0"?>
<class
    name    = "tunnel"
    handler = "tunnel"
    index   = "11"
  >
  methods for protocol tunneling.

<doc>
  The tunnel methods are used to send blocks of binary data - which
  can be serialised AMQP methods or other protocol frames - between
  AMQP peers.
</doc>

<doc name = "grammar">
    tunnel              = C:REQUEST
                        / S:REQUEST
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<field name = "headers" type = "table">
    Message header field table
</field>
<field name = "proxy id" type = "shortstr">
    The identity of the tunnelling proxy
</field>
<field name = "data name" type = "shortstr">
    The name or type of the message being tunnelled
</field>
<field name = "durable" type = "octet">
    The message durability indicator
</field>
<field name = "broadcast" type = "octet">
    The message broadcast mode
</field>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "request" content = "1">
  sends a tunnelled method
  <doc>
    This method tunnels a block of binary data, which can be an
    encoded AMQP method or other data.  The binary data is sent
    as the content for the Tunnel.Request method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "meta data" type = "table">
    meta data for the tunnelled block
    <doc>
    This field table holds arbitrary meta-data that the sender needs
    to pass to the recipient.
    </doc>
  </field>
</method>

</class>

