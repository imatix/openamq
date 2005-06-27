<?xml version="1.0"?>

<class
    name    = "access"
    handler = "connection"
    index   = "3"
  >
  work with access tickets

<doc>
  AMQP/Fast controls access to server resources using access tickets.
  A client must explicitly request access tickets before doing work.
  An access ticket grants a client the right to use a specific set of
  resources - called a "realm" - in specific ways.
</doc>

<doc name = "grammar">
    access              = C:GRANT S:GRANT-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "grant" synchronous = "1">
  request an access ticket
  <doc>
    This method requests an access ticket for an access realm.
    The server responds by granting the access ticket.  If the
    client does not have access rights to the requested realm
    this causes a connection exception.  Access tickets may be
    shared across channels within a connection and expire with
    the connection.
  </doc>
  <doc name = "rule">
    The realm name MUST start with either "/data" (for application
    resources) or "/admin" (for server administration resources).
  </doc>
  <doc name = "rule">
    The server MUST implement the /data realm and MAY implement the
    /admin realm.  The mapping of resources to realms is not
    defined in the protocol - this is a server-side configuration
    issue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "grant-ok" />

  <field name = "realm" domain = "path">
    realm to work with
  </field>

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc>
      Request exclusive access to the realm. If the server cannot grant
      this - because there are other active tickets for the realm - it
      raises a channel exception.
    </doc>
    <doc name = "rule">
      The server MUST grant clients exclusive access to a realm if they
      ask for it.
    </doc>
  </field>

  <field name = "passive" type = "bit">
    request passive access
    <doc>
      Request message passive access to the specified access realm.
      Passive access lets a client get information about resources in
      the realm but not to make any changes to them.
    </doc>
  </field>

  <field name = "active" type = "bit">
    request active access
    <doc>
      Request message active access to the specified access realm.
      Acvtive access lets a client get create and delete resources in
      the realm.
    </doc>
  </field>

  <field name = "write" type = "bit">
    request write access
    <doc>
      Request write access to the specified access realm.  Write access
      lets a client publish messages to all exchanges in the realm.
    </doc>
  </field>

  <field name = "read" type = "bit">
    request read access
    <doc>
      Request read access to the specified access realm.  Read access
      lets a client consume messages from queues in the realm.
    </doc>
  </field>
</method>

<method name = "grant-ok" synchronous = "1">
  grant access to server resources
  <doc>
    This method provides the client with an access ticket. The access
    ticket is valid within the current channel and for the lifespan of
    the channel.
  </doc>
  <doc name = "rule">
    The client MUST NOT use access tickets except within the same
    channel as originally granted.
  </doc>
  <doc name = "rule">
    The server MUST isolate access tickets per channel and treat an
    attempt by a client to mix these as a connection exception.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "ticket" domain = "access ticket" />
</method>

</class>

