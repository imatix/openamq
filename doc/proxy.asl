<?xml version="1.0"?>

<class
    name = "proxy"
    handler = "channel"
  >
  work with proxies

<doc>
  Proxies match and distribute messages across queues.  Proxies can be
  configured in the server or created at runtime.
</doc>

<doc name = "grammar">
    proxy               = C:ASSERT   S:ASSERT-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "assert" synchronous = "1">
  create proxy if needed
  <doc>
    This method creates or checks a proxy.  When creating a new proxy
    the client can specify various properties that control the durability
    of the proxy and the level of sharing for the proxy.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "assert-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new proxy, this belongs to the access realm
      of the ticket used.  All further work done with that proxy must be
      done with an access ticket for the same realm.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active" access
      to the realm in which the proxy exists or will be created, or
      "passive" access if the if-exists flag is set.
    </doc>
  </field>
    
  <field name = "proxy" domain = "proxy name">
    <assert check = "notnull" />
  </field>
    
  <field name = "class" type = "shortstr">
    proxy class
    <doc>
      Specifies the name of a proxy class, which is a server defined
      class that defines the proxy functionality.  Proxy classes are
      standardised but can be extended on a per-implementation basis.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "if exists" type = "bit">
    do not create proxy
    <doc>
      If set, the server will report the status of a proxy if it
      exists and raise a channel assertion if not.  This flag lets
      clients discover the status of non-existent proxies without
      creating them.
    </doc>
  </field>

  <field name = "durable" type = "bit">
    request a durable proxy
    <doc>
      If set when creating a new proxy, the proxy will be marked as
      durable.  Durable proxies remain active when a server restarts.
      Non-durable proxies (transient proxies) are purged if/when a
      server restarts.
    </doc>
    <doc name = "rule">
      The server MUST support both durable and transient proxies.
    </doc>
    <doc name = "rule">
      The server MUST ignore the durable field if the proxy already
      exists.
    </doc>
  </field>

  <field name = "auto delete" type = "bit">
    auto-delete proxy when unused  
    <doc>
      If set, the proxy is deleted when all queues have finished
      using it.
    </doc>
    <doc name = "rule">
      The server MUST implement the auto-delete function in this manner:
      it counts the number of queue registrations for the proxy and when
      the last queue registration is cancelled, it MUST delete the proxy.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-delete field if the proxy already
      exists.
    </doc>
  </field>
</method>


<method name = "assert-ok" synchronous = "1">
  confirms a proxy definition
  <doc>
    This method confirms a Assert method and confirms the name of the
    proxy, essential for automatically-named proxies.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1">
  delete a proxy
  <doc>
    This method deletes a proxy.  When a proxy is deleted all queue
    registrations on the proxy are cancelled.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active"
      access rights to the proxy's access realm.
    </doc>
  </field>

  <field name = "proxy" domain = "proxy name">
    <doc name = "rule">
      The proxy must exist. Attempting to delete a non-existing proxy
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "if unused" type = "bit">
    delete only if unused
    <doc>
      If set, the server will only delete the proy if it has no queue
      registrations. If the proxy has queue registrations the server
      does does not delete it but raises a channel exception instead.
    </doc>
  </field>
</method>


<method name = "delete-ok" synchronous = "1">
  confirm deletion of a proxy
  <doc>
    This method confirms the deletion of a proxy.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

</class>

