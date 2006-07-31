<?xml version="1.0"?>
<protocol>

<class
    name    = "cluster"
    handler = "cluster"
    index   = "61440"
  >
  methods for intracluster communications

<doc>
  The cluster methods are used by peers in a cluster.
</doc>

<doc name = "rule">
  Standard clients MUST NOT use the cluster class, which is reserved
  for AMQP servers participating in cluster communications.
</doc>

<doc name = "grammar">
    cluster             = C:HELLO
                        / C:STATUS
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "hello" index = "10">
  greet cluster peer
  <doc>
    This method tells the cluster peer our name and cluster protocol
    version.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />

  <field name = "version" type = "short">
     cluster protocol version
    <doc>
      This is the cluster protocol version.  The recipient should
      verify that this matches its own version number.
    </doc>
  </field>

  <field name = "name" type = "shortstr">
    own cluster name
    <doc>
      This is the cluster name of the sender.  The recipient should
      verify that this is the expected name.
    </doc>
  </field>

  <field name = "vhost" type = "shortstr">
    virtual host name
    <doc>
      This is the virtual host name of the sender. The recipient
      should verify that this matches its own virtual host name.
    </doc>
  </field>

  <field name = "signature" type = "long">
    configuration signature
    <doc>
      The signature (checksum) of the cluster configuration; this must
      be identical across all servers; a server must reject peers that
      do not provide a valid signature.
    </doc>
  </field>
</method>

<method name = "status" index = "20">
  provide peer status data
  <doc>
    This method provides a cluster peer with status information.  We
    use this method for cluster heartbeating and synchronisation.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />

  <field name = "active" type = "bit">
     cluster active server?
    <doc>
      Specifies whether the sender considers itself the active peer.
      This should only be possible if the sender is the primary or
      the backup server.
    </doc>
  </field>

  <field name = "connections" type = "long">
    number of client connections
    <doc>
      Specifies the number of client connections that the peer
      has.
    </doc>
  </field>

  <field name = "messages" type = "long">
    number of held messages
    <doc>
      Specifies the number of messages that the peer currently
      holds in memory.
    </doc>
  </field>

  <field name = "allocation" type = "long">
    allocated memory
    <doc>
      Specifies the current memory consumption of the peer, in
      Megabytes.
    </doc>
  </field>

  <field name = "exchanges" type = "long">
    number of exchanges
    <doc>
      Specifies the number of exchanges that exist on the peer.
    </doc>
  </field>

  <field name = "queues" type = "long">
    number of queues
    <doc>
      Specifies the number of queues that exist on the peer.
    </doc>
  </field>

  <field name = "consumers" type = "long">
    number of consumers
    <doc>
      Specifies the number of consumers that exist on the peer.
    </doc>
  </field>

  <field name = "bindings" type = "long">
    number of bindings
    <doc>
      Specifies the number of bindings that exist on the peer.
    </doc>
  </field>

  <field name = "peers" type = "long">
    number of cluster peers
    <doc>
      Specifies the total number of cluster peers that the peer
      can see, including itself.
    </doc>
  </field>
</method>

</class>
</protocol>
