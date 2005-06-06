<?xml version="1.0"?>

<class
    name = "destination"
    handler = "channel"
  >
  work with dynamic destinations

<doc>
  Destinations are queues or topics, capable of storing and routing all
  content classes, that are created at runtime rather than as server
  configured objects. A dynamic destination is normally peristent - i.e.
  it continues to exist and accept messages even when the creating client
  has disconnected, or the server has restarted.  This class lets clients
  create and manage dynamic destinations.
</doc>

<doc name = "grammar">
    destination         = C:DEFINE S:DEFINE-OK
                        / C:QUERY  S:QUERY-OK
                        / C:PURGE  S:PURGE-OK
                        / C:CANCEL S:CANCEL-OK
</doc>

<chassis name = "server" implement = "SHOULD" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "define" synchronous = "1">
  create or verify a dynamic destination
  <doc>
    This method creates or reconfigures a dynamic destination. Various
    fields in this method let the client control the persistence of the
    destination. For example, one can implement a JMS-style temporary
    destination by setting both the private and auto-cancel fields. A
    client can work with an existing dynamic destination without using
    this method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "define-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new dynamic destination, this belongs to
      the access realm of the ticket used.  All further work done with
      that destination - including publishing and consuming messages -
      must be done with an access ticket for the same realm.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "dynamic"
      access rights.  If the destination already exists, the ticket
      must also be for the destination's access realm.
    </doc>
  </field>
    
  <field name = "service type" domain = "service type">
    <doc name = "rule">
      The client MUST NOT use the fuzzy type when defining a new
      destination.
    </doc>
  </field>
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination.  For publishers and
      consumers, dynamic destinations do not have any visible naming
      difference from configured destinations.
    </doc>
    <doc name = "rule">
      A peer MUST NOT use naming conventions to distinguish dynamic
      destinations from configured destinations.
    </doc>
    <doc name = "rule">
      The destination name MAY be empty, in which case the server
      MUST create a new unique name automatically and return this
      to the client in the Define-Ok method.
    </doc>
  </field>
    
  <field name = "template" type = "shortstr">
    destination template
    <doc>
      Specifies the name of a destination template, which is a server
      configured object that provides configuration options for
      dynamic destinations.  Templates are not standardised; these
      are names defined on an implementation basis.
    </doc>
    <doc name = "rule">
      If the template is empty the server SHOULD use a suitable default.
    </doc>
    <doc name = "rule">
      The server MUST ignore the template field if the destination
      already exists.
    </doc>
  </field>

  <field name = "private" type = "bit">
    request private destination
    <doc>
      If set, the destination is private and owned by the current
      client. This will fail if the destination already exists and is
      owned by another client. Private destinations cannot be consumed
      from by clients except the owner.
    </doc>
    <doc name = "rule">
      The server MUST support both private and shared dynamic
      destinations.
    </doc>
    <doc name = "rule">
      The server MUST use the client identifier supplied at connection
      open time to identify the owner of a private destination.  The
      client identifier is persistent even if the client disconnects and
      reconnects.
    </doc>
    <doc name = "rule">
      The server MUST ignore the private field if the destination
      already exists.
    </doc>
  </field>

  <field name = "auto cancel" type = "bit">
    auto-cancel when finished 
    <doc>
      If set, the destination is cancelled when all clients have
      finished using it.
    </doc>
    <doc name = "rule">
      The server MUST implement the auto-cancel function in this
      manner: it counts the number of queue consumers or topic
      subscribers (depending on the destination service type), and
      when the last consumer or subscription is cancelled, it MUST
      delete the destination and dead-letter any messages it holds.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-cancel field in this method if
      the destination already exists.  A client cannot modify the
      auto-cancel after the destination has been defined by another
      client.
    </doc>
  </field>
</method>

<method name = "define-ok" synchronous = "1">
  confirms a destination definition
  <doc>
    This method confirms a Define method and confirms the name of the
    destination, essential for automatically-named destinations.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "destination" domain = "destination">
    name of destination
    <doc>
      Confirms the name of the destination. If the server generated a
      destination name, this field contains that name.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message count" type = "long">
    number of messages in queue destination
    <doc>
      For queue destinations, provides the number of messages in the
      queue. For topics this is always zero.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "query" synchronous = "1">
  query status of destination
  <doc>
    This method queries the status of a destination.  This can be a
    dynamic destination or a configured destination, and it can be
    a queue or a topic as defined by the service-type field.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "query-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "query"
      access rights to the destination's access realm.
    </doc>
  </field>
    
  <field name = "service type" domain = "service type" />
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination, which must exist.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "query-ok" synchronous = "1">
  provide status of destination
  <doc>
    This method returns the status of the destination.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "service type" domain = "service type" />

  <field name = "destination" domain = "destination">
    name of destination
    <doc>
      Confirms the name of the destination.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message count" type = "long">
    number of messages in queue destination
    <doc>
      For queue destinations, provides the number of messages in the 
      queue. For topics this is always zero.
    </doc>
  </field>

  <field name = "consumer count" type = "long">
    number of consumers
    <doc>
      Provides the number of consumers for the queue or topic.  For
      topics the number may be inaccurate if topic selectors are used
      to steer multiple topics into a single subscriber.
    </doc>
  </field>

  <field name = "subscriber count" type = "long">
    number of subscribers
    <doc>
      For topic destinations, provides the number of subscribers that 
      are using the topic.  The number may be inaccurate if topic 
      selectors are used to steer multiple topics into a single 
      subscriber.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "purge" synchronous = "1">
  purge a destination 
  <doc>
    This method removes all messages from a queue destination or all
    messages from the subscriptions for a topic.  It does not cancel
    consumers or subscriptions.  Purged messages are deleted without
    any formal "undo" mechanism.
  </doc>
  <doc name = "rule">
    On transacted channels the server MUST not purge messages that have
    already been sent to a client but not yet acknowledged.
  </doc>
  <doc name = "rule">
    When purging a topic, the server SHOULD purge subcriptions that are
    unambiguously made on this topic. The server MAY ignore
    subscriptions that use topic wildcarding to subscribe to more than
    one topic.
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
      destination.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights to the destination's access realm.
    </doc>
  </field>

  <field name = "service type" domain = "service type" />

  <field name = "destination" domain = "destination">
    name of destination
    <doc>
      The name of the destination to purge.  This can be a configured
      destination or a dynamic destination.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "purge-ok" synchronous = "1">
  confirms a destination purge
  <doc>
    This method confirms the purge of a destination.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "message count" type = "long">
    number of messages purged
    <doc>
      Provides the number of messages purged.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "cancel" synchronous = "1">
  cancel a dynamic destination
  <doc>
    This method cancels (deletes) a dynamic destination.  When a queue
    is cancelled, any pending messages are sent to a dead-letter queue
    if this is defined in the server configuration, and all consumers
    on the queue are cancelled.  When a topic is cancelled, all
    subscriptions on that topic are also cancelled.
  </doc>
  <doc name = "rule">
    The client MUST use an access ticket that the server granted with
    "dynamic" access rights to the destination's access realm.
  </doc>
  <doc name = "rule">
    The server SHOULD use a dead-letter queue or topic to hold messages
    that were pending on a cancelled queue, and MAY provide facilities
    for a system administrator to move these messages back to an active
    queue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "cancel-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights to the destination's access realm.
    </doc>
  </field>

  <field name = "service type" domain = "service type" />
    
  <field name = "destination" domain = "destination">
    <doc>
      Specifies the name of the destination. This destination
      must exist.  Attempting to cancel a non-existing destination
      causes a connection exception.
    </doc>
    <assert check = "notnull" />
  </field>
    
  <field name = "ifempty" type = "bit">
    cancel only if empty
    <doc>
      If set, the server will only cancel the destination if it has no
      messages (for a queue) or no subscriptions (for a topic).  If the
      destination is not empty the server raises a channel exception.
    </doc>
  </field>
</method>

<method name = "cancel-ok" synchronous = "1">
  confirm cancellation of a dynamic destination
  <doc>
    This method confirms the cancellation of a destination.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "message count" type = "long">
    number of messages purged
    <doc>
      Provides the number of messages purged.
    </doc>
  </field>
</method>

</class>

