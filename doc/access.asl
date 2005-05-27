<?xml version="1.0"?>

<class
    name = "access"
    handler = "connection"
  >
  work with access tickets

<doc>
  AMQP/Fast controls access to server resources using access tickets.
  A client must explicitly request access tickets before doing work.
  An access ticket grants a client the right to use a specific set of
  resources - called a "realm" - in specific ways.
</doc>

<doc name = "grammar">
    access              = C:GRANT S:GRANT_OK
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
    /admin realm.  The mapping of destinations to realms is not
    defined in the protocol - this is a server-side configuration
    issue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "grant ok" />

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

  <field name = "query" type = "bit">
    request query rights
    <doc>
      Request query rights to the specified access realm. Query rights
      allow a client to get information about destinations in the domain
      but not make any changes to them.
    </doc>
  </field>

  <field name = "publish" type = "bit">
    request publish rights
    <doc>
      Request publisher rights to the specified access realm. A publisher
      can send messages to all destinations in the realm.
    </doc>
  </field>

  <field name = "consume" type = "bit">
    request consume rights
    <doc>
      Request consumer rights to the specified access realm. A consumer
      can receive and process messages from all destinations in the realm.
    </doc>
  </field>

  <field name = "subscribe" type = "bit">
    request subscriber rights
    <doc>
      Request subscriber rights to the specified access realm. A
      subscriber can receive messages from all topics in the realm.
    </doc>
  </field>

  <field name = "dynamic" type = "bit">
    request dynamic destination rights
    <doc>
      Request the right to define and cancel dynamic destinations in
      the specified access realm.
    </doc>
  </field>

  <field name = "purge" type = "bit">
    request purge rights
    <doc>
      Request the right to purge destinations and subscriptions in the
      specified access realm.
    </doc>
  </field>
</method>

<method name = "grant ok" synchronous = "1">
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

