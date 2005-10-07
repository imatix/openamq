<?xml version="1.0"?>

<class
    name    = "exchange"
    handler = "channel"
    index   = "4"
  >
  work with exchanges

<doc>
  Exchanges match and distribute messages across queues.  Exchanges can be
  configured in the server or created at runtime.
</doc>

<doc name = "grammar">
    exchange            = C:DECLARE  S:DECLARE-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "declare" synchronous = "1">
  declare exchange, create if needed
  <doc>
    This method creates an exchange if it does not already exist, and if the
    exchange exists, verifies that it is of the correct and expected class.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "declare-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new exchange, this belongs to the access realm
      of the ticket used.  All further work done with that exchange must be
      done with an access ticket for the same realm.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active" access
      to the realm in which the exchange exists or will be created, or
      "passive" access if the if-exists flag is set.
    </doc>
  </field>
    
  <field name = "exchange" domain = "exchange name">
    <assert check = "regexp" value = "^[a-zA-Z0-9-_.]+$" />
  </field>

  <field name = "class" type = "shortstr">
    exchange class
    <doc>
      Each exchange belongs to one of a set of exchange classes
      implemented by the server.  The exchange classes define the
      functionality of the exchange - i.e. how messages are routed
      through it.  It is not valid to attempt to change the class
      of an existing exchange.
    </doc>
    <doc name = "rule">
      If the exchange already exists with a different class, the server
      MUST raise a channel exception with reply code TBD.
    </doc>
    <assert check = "regexp" value = "^[a-zA-Z0-9-_.]+$" />
  </field>

  <field name = "passive" type = "bit">
    do not create exchange
    <doc>
      If set, the server will report the status of an exchange if it
      exists and raise a channel assertion if not.  This flag lets
      clients discover the status of non-existent exchanges without
      creating them.
    </doc>
  </field>

  <field name = "durable" type = "bit">
    request a durable exchange
    <doc>
      If set when creating a new exchange, the exchange will be marked as
      durable.  Durable exchanges remain active when a server restarts.
      Non-durable exchanges (transient exchanges) are purged if/when a
      server restarts.
    </doc>
    <doc name = "rule">
      The server MUST support both durable and transient exchanges.
    </doc>
    <doc name = "rule">
      The server MUST ignore the durable field if the exchange already
      exists.
    </doc>
  </field>

  <field name = "auto delete" type = "bit">
    auto-delete when unused
    <doc>
      If set, the exchange is deleted when all queues have finished
      using it.
    </doc>
    <doc name = "rule">
      The server SHOULD allow for a reasonable delay between the point
      when it determines that an exchange is not being used (or no longer
      used), and the point when it deletes the exchange.  At the least it
      must allow a client to create an exchange and then bind a queue to
      it, with a small but non-zero delay between these two actions.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-delete field if the exchange already
      exists.
    </doc>
  </field>

  <field name = "internal" type = "bit">
    create internal exchange
    <doc>
      If set, the exchange may not be used directly by publishers, but
      only when bound to other exchanges. Internal exchanges are used to
      construct wiring that is not visible to applications.
    </doc>
  </field>
</method>


<method name = "declare-ok" synchronous = "1">
  confirms an exchange declaration
  <doc>
    This method confirms a Declare method and confirms the name of the
    exchange, essential for automatically-named exchanges.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1">
  delete an exchange
  <doc>
    This method deletes an exchange.  When an exchange is deleted all queue
    bindings on the exchange are cancelled.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active"
      access rights to the exchange's access realm.
    </doc>
  </field>

  <field name = "exchange" domain = "exchange name">
    <doc name = "rule">
      The exchange must exist. Attempting to delete a non-existing exchange
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "if unused" type = "bit">
    delete only if unused
    <doc>
      If set, the server will only delete the exchange if it has no queue
      bindings. If the exchange has queue bindings the server does not
      delete it but raises a channel exception instead.
    </doc>
  </field>
</method>


<method name = "delete-ok" synchronous = "1">
  confirm deletion of an exchange
  <doc>
    This method confirms the deletion of an exchange.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

</class>

