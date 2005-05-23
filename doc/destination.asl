<?xml version="1.0"?>

<class
    name = "destination"
    handler = "channel"
    >
work with dynamic queues and topics

<doc>
Destinations are queues or topics, capable of storing and routing all
types of content.  This class provides a way for clients to create
and manage destinations through the protocol.  So-called "dynamic"
destinations are defined and cancelled at run-time.
</doc>

<doc name = "grammar">
    destination         = C:DEFINE S:DEFINED
                        / C:PURGE  S:PURGED
                        / C:CANCEL S:CANCELLED
</doc>

<chassis name = "server" implement = "SHOULD" />
<chassis name = "client" implement = "MAY" />

<method name = "define" synchronous = "1" expect = "defined">
    create or configure a dynamic destination
    <doc>
    This method creates or reconfigures a dynamic destination, which
    is a persistent destination created and managed through protocol
    methods - this class - rather than as a configured server object.
    A dynamic destination is normally peristent - i.e. it continues
    to exist and accept messages even when the creating client has
    disconnected, or the server has restarted.  Various fields in
    the Destination.Open method let the client control the persistence
    of the destination. For example, one can implement a JMS-style
    temporary queue by setting both the private and auto-cancel
    fields.  A client can work with an existing dynamic destination
    without using this method.
    </doc>
    <chassis name = "server" implement = "MUST" />

    <field name = "ticket" domain = "access ticket">
      <doc>
      When a client defines a new dynamic destination, this belongs
      to the access realm of the ticket used.  All further work done
      with that domain - including publishing and consuming messages
      - must be done with an access ticket for the same domain.
      </doc>
      <doc name = "rule">
      The client MUST provide a valid access ticket giving "dynamic"
      access rights.  If the destination already exists, the ticket
      must also be for the correct access realm.
      </doc>
    </field>
    
    <field name = "service type" domain = "service type" />
    
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
      to the client in the Destination.Opened method.
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
    </field>

    <field name = "private" type = "bit">
    request private destination
      <doc>
      If set, the destination is private and owned by the current
      client.  This will fail if the destination already exists and
      is owned by another client.  Private destinations cannot be
      opened by clients except the owner.
      </doc>
      <doc name = "rule">
      The server MUST support both private and shared dynamic
      destinations.
      </doc>
      <doc name = "rule">
      The server MUST use the client identifier supplied at 
      connection open time to identify the owner of a private
      destination.  The client identifier is persistent even
      if the client disconnects and reconnects.
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
      delete the destination and any messages it holds.
      </doc>
      <doc name = "rule">
      A dynamic destination with no consumers or subscriptions MUST
      be cancelled when its owning client disconnects.
      </doc>
    </field>
</method>

<method name = "defined" synchronous = "1">
confirms a destination definition
    <doc>
    This method confirms a Destination.Define method and provides the
    client with the name of the destination in case of automatically
    named destinations.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "destination" domain = "destination">
    name of destination
      <doc>
      Confirms the name of the destination. If the server assigned a
      destination name, this field contains the invented value.
      </doc>
      <assert check = "notnull" />
    </field>

    <field name = "message count" type = "long">
    number of messages in queue destination
      <doc>
      For dynamic queue destinations, provides the number of messages
      in the queue. For topics this is always zero.
      </doc>
    </field>
</method>

<method name = "purge" synchronous = "1" expect = "purged">
purge a destination 
    <doc>
    This method removes all messages from a queue destination or all
    messages from the subscriptions for a topic.  It does not cancel
    consumers or subscriptions.
    </doc>
    <doc name = "rule">
    On transacted channels the server MUST not purge messages that have
    already been sent to a client but not yet acknowledged.
    </doc>
    <doc name = "rule">
    When purging a topic, the server SHOULD purge subcriptions that are
    unambiguously made on this topic.  The server MAY ignore
    subscriptions that use topic wildcarding to subscribe to more than
    one topic.
    </doc>
    <chassis name = "server" implement = "MUST" />

    <field name = "ticket" domain = "access ticket">
      <doc>
      The access ticket must be for the access realm that holds the
      destination.
      </doc>
      <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights for the destination access realm.
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

<method name = "purged" synchronous = "1">
confirms a destination purge
    <doc>
    This method confirms the purge of a queue destination.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "message count" type = "long">
    number of messages purged
      <doc>
      Provides the number of messages purged. If zero, the queue was
      empty.
      </doc>
    </field>
</method>

<method name = "cancel" synchronous = "1" expect = "cancelled">
    cancel a dynamic destination
    <doc>
    This method cancels a dynamic destination, which is a queue or
    topic created using the Destination.Define method.  When a
    queue is cancelled, any pending messages are discarded. When a
    topic is cancelled, all subscriptions on that topic are also
    cancelled.  Note that cancellation also implies a purge.
    </doc>
    <chassis name = "server" implement = "MUST" />
    <doc name = "rule">
    The client MUST use an access ticket that the server granted with
    "dynamic" access rights before sending this method.
    </doc>

    <field name = "ticket" domain = "access ticket">
      <doc name = "rule">
      The client MUST provide a valid access ticket giving "dynamic"
      access rights to the access realm used when defining the
      destination.
      </doc>
    </field>

    <field name = "service type" domain = "service type" />
    
    <field name = "destination" domain = "destination">
      <doc>
      Specifies the name of the destination. This destination
      must exist.  Attempting to cancel a non-existing destination
      causes a connection exception.
      </doc>
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

<method name = "cancelled" synchronous = "1">
    confirm cancellation of a dynamic destination
    <doc>
    This method confirms the cancellation of a destination.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "message count" type = "long">
    number of messages purged
      <doc>
      Provides the number of messages purged. If zero, the queue
      destination was empty or the topic destination did not have any
      subscription messages to purge.
      </doc>
    </field>
</method>

</class>

