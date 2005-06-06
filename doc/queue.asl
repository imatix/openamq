<?xml version="1.0"?>

<class
    name = "queue"
    handler = "channel"
  >
  work with queues

<doc>
  Queues store and forward messages.  Queues can be configured in the server
  or created at runtime.  Queues must be attached to at least one proxy in
  order to receive messages from publishers.
</doc>

<doc name = "grammar">
    queue               = C:ASSERT   S:ASSERT-OK
                        / C:REGISTER S:REGISTER-OK
                        / C:CANCEL   S:CANCEL-OK
                        / C:PURGE    S:PURGE-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "assert" synchronous = "1">
  create queue if needed
  <doc>
    This method creates or checks a queue.  When creating a new queue
    the client can specify various properties that control the durability
    of the queue and its contents, and the level of sharing for the queue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "assert-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new queue, this belongs to the access realm
      of the ticket used.  All further work done with that queue must be
      done with an access ticket for the same realm.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active" access
      to the realm in which the queue exists or will be created, or
      "passive" access if the if-exists flag is set.
    </doc>
  </field>
    
  <field name = "namespace" domain = "queue namespace" />
    
  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue name MAY be empty, in which case the server MUST create
      a new queue with a unique generated name and return this to the
      client in the Assert-Ok method.
    </doc>
    <doc name = "rule">
      Clients asking the server to provide a queue name SHOULD use a
      specific namespace for such queues to avoid any possible conflict
      with names that might be used elsewhere by clients.
    </doc>
  </field>
    
  <field name = "template" type = "shortstr">
    queue template
    <doc>
      Specifies the name of a queue template, which is a server
      configured object that provides queue configuration options.
      Template semantics and names are not standardised.
    </doc>
    <doc name = "rule">
      If the template is empty the server SHOULD use a suitable default.
    </doc>
    <doc name = "rule">
      The server MUST ignore the template field if the queue already
      exists.
    </doc>
  </field>

  <field name = "if exists" type = "bit">
    do not create queue
    <doc>
      If set, the server will report the status of a queue if it
      exists and raise a channel assertion if not.  This flag lets
      clients discover the status of non-existent queues without
      creating them.
    </doc>
  </field>

  <field name = "durable" type = "bit">
    request a durable queue
    <doc>
      If set when creating a new queue, the queue will be marked as
      durable.  Durable queues remain active when a server restarts.
      Non-durable queues (transient queues) are purged if/when a
      server restarts.  Note that durable queues do not necessarily
      hold persistent messages, although it does not make sense to
      send persistent messages to a transient queue.
    </doc>
    <doc name = "rule">
      The server MUST support both durable and transient queues.
    </doc>
    <doc name = "rule">
      The server MUST ignore the durable field if the queue already
      exists.
    </doc>
  </field>

  <field name = "private" type = "bit">
    request a private queue
    <doc>
      If set when creating a new queue, the queue will be private and
      owned by the current client. This will fail if the queue already
      exists and is owned by another client. Private queues cannot be
      consumed from by clients except the owner.
    </doc>
    <doc name = "rule">
      The server MUST support both private and shared queues.
    </doc>
    <doc name = "rule">
      The server MUST use the client identifier supplied at connection
      open time to identify the owner of a private queue.  The client
      identifier is persistent even if the client disconnects and
      reconnects.
    </doc>
    <doc name = "rule">
      The server MUST ignore the private field if the queue already
      exists.
    </doc>
  </field>

  <field name = "auto delete" type = "bit">
    auto-delete queue when unused  
    <doc>
      If set, the queue is deleted when all clients have finished
      using it.
    </doc>
    <doc name = "rule">
      The server MUST implement the auto-delete function in this manner:
      it counts the number of queue consumers and when the last consumer
      is cancelled, it MUST delete the queue and dead-letter any messages
      it holds.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-delete field if the queue already
      exists.
    </doc>
  </field>
</method>


<method name = "assert-ok" synchronous = "1">
  confirms a queue definition
  <doc>
    This method confirms a Assert method and confirms the name of the
    queue, essential for automatically-named queues.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "queue" domain = "queue name">
    name of queue
    <doc>
      Reports the name of the queue. If the server generated a queue
      name, this field contains that name.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "proxy" domain = "proxy name">
    queue proxy, if any
    <doc>
      Reports the name of the last-registered proxy for the queue.  In
      the case of newly-created queues this will be empty.
    </doc>
    <doc name = "rule">
      The server MUST NOT register newly-created queues with proxies
      implicitly or automatically. The decision of what proxies to use
      is taken by the client application responsible for administrating
      the server.  However the server SHOULD store durable registrations
      so that on a server restart these can be recreated.
    </doc>
  </field>

  <field name = "message count" type = "long">
    number of messages in queue queue
    <doc>
      Reports the number of messages in the queue, which will be zero
      for newly-created queues.
    </doc>
  </field>

  <field name = "consumer count" type = "long">
    number of consumers
    <doc>
      Reports the number of active consumers for the queue. Note that
      consumers can suspend activity (Channel.Flow) in which case they
      do not appear in this count.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "register" synchronous = "1">
  register queue with a proxy
  <doc>
    This method registers a queue with a proxy.  Until a queue is 
    registered it will not receive any messages.  The two main types
    of queue are store-and-forward queues (registered with a dest-name
    proxy) and subscription queues (registered with a dest-wild proxy).
  </doc>
  <doc name = "rule">
    The server MUST NOT allow the registration of a durable queue with
    a transient proxy.  If the client attempts this the server MUST
    raise a channel exception.
  </doc>
  <doc name = "rule">
    Registrations for durable queues are automatically durable and the
    server SHOULD restore such registrations after a server restart.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "register-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active"
      access rights to the queue's access realm.
    </doc>
  </field>
    
  <field name = "namespace" domain = "queue namespace" />

  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue must exist. Attempting to query a non-existing queue
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "proxy" domain = "proxy name">
    proxy to register with
    <doc>
      The name of the proxy to register with. If the proxy does not
      exist the server will raise a channel exception.
    </doc>
  </field>

  <field name = "arguments" type = "table">
    arguments for registration
    <doc>
      A set of arguments for the proxy.  The syntax and semantics of
      these arguments depends on the proxy class.
    </doc>
  </field>
</method>

<method name = "register-ok" synchronous = "1">
  confirm registration successful
  <doc>
    This method confirms that a registration was successful.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "purge" synchronous = "1">
  purge a queue 
  <doc>
    This method removes all messages from a queue.  It does not cancel
    consumers.  Purged messages are deleted without any formal "undo"
    mechanism.
  </doc>
  <doc name = "rule">
    On transacted channels the server MUST not purge messages that have
    already been sent to a client but not yet acknowledged.
  </doc>
  <doc name = "rule">
    The server MAY implement a purge queue or log that allows system
    administrators to recover accidentally-purged messages.  The server
    SHOULD NOT keep purged messages in the same storage spaces as the
    live messages since the volumes of purged messages may get very
    large.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "purge-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      The access ticket must be for the access realm that holds the
      queue.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "read" access
      rights to the queue's access realm.  Note that purging a queue is
      equivalent to reading all messages and discarding them.
    </doc>
  </field>

  <field name = "namespace" domain = "queue namespace" />

  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue must exist. Attempting to purge a non-existing queue
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
</method>


<method name = "purge-ok" synchronous = "1">
  confirms a queue purge
  <doc>
    This method confirms the purge of a queue.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "message count" type = "long">
    number of messages purged
    <doc>
      Reports the number of messages purged.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1">
  delete a queue
  <doc>
    This method deletes a queue.  When a queue is deleted any pending
    messages are sent to a dead-letter queue if this is defined in the
    server configuration, and all consumers on the queue are cancelwled.
  </doc>
  <doc name = "rule">
    The server SHOULD use a dead-letter queue to hold messages that
    were pending on a deleted queue, and MAY provide facilities for
    a system administrator to move these messages back to an active
    queue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "active"
      access rights to the queue's access realm.
    </doc>
  </field>

  <field name = "namespace" domain = "queue namespace" />

  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue must exist. Attempting to delete a non-existing queue
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "if unused" type = "bit">
    delete only if unused
    <doc>
      If set, the server will only delete the queue if it has no
      consumers. If the queue has consumers the server does does not
      delete it but raises a channel exception instead.
    </doc>
  </field>

  <field name = "if empty" type = "bit">
    delete only if empty
    <doc>
      If set, the server will only delete the queue if it has no
      messages. If the queue is not empty the server raises a channel
      exception.
    </doc>
  </field>
</method>


<method name = "delete-ok" synchronous = "1">
  confirm deletion of a queue
  <doc>
    This method confirms the deletion of a queue.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "message count" type = "long">
    number of messages purged
    <doc>
      Reports the number of messages purged.
    </doc>
  </field>
</method>

</class>

