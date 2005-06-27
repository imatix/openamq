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
    exchange            = C:ASSERT   S:ASSERT-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "assert" synchronous = "1">
  create exchange if needed
  <doc>
    This method creates or checks an exchange. When creating a new exchange
    the client can specify various properties that control the durability
    of the exchange and the level of sharing for the exchange.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "assert-ok" />

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
    <assert check = "notnull" />
  </field>
    
  <field name = "class" type = "shortstr">
    exchange class
    <doc>
      Specifies the name of an exchange class, which is a server defined
      class that defines the exchange functionality.  Exchange classes are
      standardised but can be extended on a per-implementation basis.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "if exists" type = "bit">
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
    auto-delete exchange when unused  
    <doc>
      If set, the exchange is deleted when all queues have finished
      using it.
    </doc>
    <doc name = "rule">
      The server MUST implement the auto-delete function in this manner:
      it counts the number of queue registrations for the exchange and when
      the last queue registration is cancelled, it MUST delete the exchange.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-delete field if the exchange already
      exists.
    </doc>
  </field>
</method>


<method name = "assert-ok" synchronous = "1">
  confirms an exchange definition
  <doc>
    This method confirms a Assert method and confirms the name of the
    exchange, essential for automatically-named exchanges.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1">
  delete an exchange
  <doc>
    This method deletes an exchange.  When an exchange is deleted all queue
    registrations on the exchange are cancelled.
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
      If set, the server will only delete the proy if it has no queue
      registrations. If the exchange has queue registrations the server
      does does not delete it but raises a channel exception instead.
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

