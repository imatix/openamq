<?xml version="1.0"?>
<!--
    Please see AMQP specification for copyrights.
 
    Links to full AMQP specification:
    =================================
    http://www.amqp.org/
-->

<class
    name    = "exchange"
    handler = "channel"
    index   = "40"
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

<doc name = "rule" test = "amq_exchange_19" >
  The server MUST implement the direct and fanout exchange types, and
  predeclare the corresponding exchanges named amq.direct and amq.fanout
  in each virtual host. The server MUST also predeclare a direct
  exchange to act as the default exchange for content Publish methods
  and for default queue bindings.
</doc>

<doc name = "rule" test = "amq_exchange_20">
  The server SHOULD implement the topic exchange type, and predeclare
  the corresponding exchange named amq.topic in each virtual host.
</doc>

<doc name = "rule" test = "amq_exchange_21">
  The server MAY implement the system exchange type, and predeclare the
  corresponding exchanges named amq.system in each virtual host. If the
  client attempts to bind a queue to the system exchange, the server
  MUST raise a connection exception with reply code 507 (not allowed).
</doc>

<doc name = "rule" test = "amq_exchange_22"> 
  The default exchange MUST be defined as internal, and be inaccessible
  to the client except by specifying an empty exchange name in a content
  Publish method. That is, the server MUST NOT let clients make explicit
  bindings to this exchange.
</doc>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "declare" synchronous = "1" index = "10">
  declare exchange, create if needed
  <doc>
    This method creates an exchange if it does not already exist, and if the
    exchange exists, verifies that it is of the correct and expected class.
  </doc>
  <doc name = "rule" test = "amq_exchange_23">
    The server SHOULD support a minimum of 16 exchanges per virtual host
    and ideally, impose no limit except as defined by available resources.
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
    <doc name = "rule" test = "amq_exchange_15">
      Exchange names starting with "amq." are reserved for predeclared
      and standardised exchanges.  If the client attempts to create an
      exchange starting with "amq.", the server MUST raise a channel
      exception with reply code 403 (access refused).
    </doc>
    <assert check = "regexp" value = "^[a-zA-Z0-9-_.:/]+$" />
  </field>

  <field name = "type" type = "shortstr">
    exchange type
    <doc>
      Each exchange belongs to one of a set of exchange types implemented
      by the server.  The exchange types define the functionality of the
      exchange - i.e. how messages are routed through it.  It is not valid
      or meaningful to attempt to change the type of an existing exchange.
    </doc>
    <doc name = "rule" test = "amq_exchange_16">
      If the exchange already exists with a different type, the server
      MUST raise a connection exception with a reply code 507 (not allowed).
    </doc>
    <doc name = "rule" test = "amq_exchange_18">
      If the server does not support the requested exchange type it MUST
      raise a connection exception with a reply code 503 (command invalid).
    </doc>
    <assert check = "regexp" value = "^[a-zA-Z0-9-_.:/]+$" />
  </field>

  <field name = "passive" type = "bit">
    do not create exchange
    <doc>
    If set, the server will not create the exchange.  The client can use
    this to check whether an exchange exists without modifying the server
    state.
    </doc>
    <doc name = "rule" test = "amq_exchange_05" >
      If set, and the exchange does not already exist, the server MUST
      raise a channel exception with reply code 404 (not found).
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
    <doc name = "rule" test = "amq_exchange_24">
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
    <doc name = "rule" test = "amq_exchange_02">
      The server SHOULD allow for a reasonable delay between the point
      when it determines that an exchange is not being used (or no longer
      used), and the point when it deletes the exchange.  At the least it
      must allow a client to create an exchange and then bind a queue to
      it, with a small but non-zero delay between these two actions.
    </doc>
    <doc name = "rule" test = "amq_exchange_25">
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

  <field name = "nowait" type = "bit">
    do not send a reply method
    <doc>
    If set, the server will not respond to the method. The client should
    not wait for a reply method.  If the server could not complete the
    method it will raise a channel or connection exception.
    </doc>
  </field>

  <field name = "arguments" type = "table">
    arguments for declaration
    <doc>
      A set of arguments for the declaration. The syntax and semantics
      of these arguments depends on the server implementation.  This
      field is ignored if passive is 1.
    </doc>
  </field>
</method>


<method name = "declare-ok" synchronous = "1" index = "11">
  confirms an exchange declaration
  <doc>
    This method confirms a Declare method and confirms the name of the
    exchange, essential for automatically-named exchanges.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1" index = "20">
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
    <doc name = "rule" test = "amq_exchange_11">
      The exchange MUST exist. Attempting to delete a non-existing exchange
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
    <doc name = "rule" test = "amq_exchange_12">
      If set, the server SHOULD delete the exchange but only if it has
      no queue bindings.
    </doc>
    <doc name = "rule" test = "amq_exchange_13">
      If set, the server SHOULD raise a channel exception if the exchange is in
      use.
    </doc>
  </field>

  <field name = "nowait" type = "bit">
    do not send a reply method
    <doc>
    If set, the server will not respond to the method. The client should
    not wait for a reply method.  If the server could not complete the
    method it will raise a channel or connection exception.
    </doc>
  </field>
</method>

<method name = "delete-ok" synchronous = "1" index = "21">
  confirm deletion of an exchange
  <doc>
    This method confirms the deletion of an exchange.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

</class>
