<?xml version="1.0"?>

<class
    name = "subscription"
    handler = "channel"
  >
  work with subscriptions

<doc>
  Subscriptions are requests to receive messages from one or more topics.
  Subscriptions can be temporary or persistent, and private or shared.
  In the AMQ queuing and routing model, subscriptions are orthogonal with
  queues and provide very similar functionality except that a client can
  not send messages directly to a subscription, only via a topic.
  Subscriptions are capable of storing all content domains. This class
  provides methods for clients to create, manage, and share subscriptions.
</doc>

<doc name = "grammar">
    subscription        = C:DEFINE S:DEFINE_OK
                        / C:QUERY  S:QUERY_OK
                        / C:PURGE  S:PURGE_OK
                        / C:CANCEL S:CANCEL_OK
</doc>

<doc name = "rule">
  AMQP assumes strongly that queues and subscriptions share the same
  mechanisms for holding persistent and non-persistent messages, for
  handling acknowledgements, for transactions, for browsing, and so on.
  An AMQP server SHOULD implement subscriptions using the same internal
  mechanisms as queues.
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "define" synchronous = "1">
  create or configure a persistent subscription
  <doc>
    This method creates or reconfigures a subscription. A subscription
    is normally peristent - i.e. it continues to exist and accept
    messages even when the creating client has disconnected, or the
    server has restarted. Various fields in this method let the client
    control the persistence of the subscription. A client can work with
    an existing subscription without using this method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "define ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new subscription, this belongs to the
      access realm of the ticket used.  Any further use of the
      subscription requires an access ticket for that realm.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "subscribe"
      access rights.  If the subscription already exists, the ticket
      must also be for the correct access realm.
    </doc>
  </field>
    
  <field name = "subscription" domain = "subscription">
    <doc>
      Specifies the name of the subscription to define.
    </doc>
    <doc name = "rule">
      The subscription name MAY be empty, in which case the server
      MUST create a new unique name automatically and return this
      to the client in the Subscription.Define_ok method.
    </doc>
  </field>
    
  <field name = "template" type = "shortstr">
    subscription template
    <doc>
      Specifies the name of a subscription template, which is a server
      configured object that provides configuration options. Templates
      are not standardised; these are names defined on an implementation
      basis.
    </doc>
    <doc name = "rule">
      If the template is empty the server SHOULD use a suitable default.
    </doc>
    <doc name = "rule">
      The server MUST ignore the template field if the subscription
      already exists.
    </doc>
  </field>

  <field name = "topic selector" domain = "topic selector">
    <doc>
      This field is only used when defining a new subscription.
    </doc>
    <doc name = "rule">
      The server MUST ignore the topic-selector field if the
      subscription already exists.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "topic seltype" domain = "topic seltype">
    <doc name = "rule">
      The server MUST support the simple syntax and MAY support the
      tpath and regexp syntaxes.
    </doc>
  </field>

  <field name = "message selector" domain = "message selector">
    <doc>
      This field is only used when defining a new subscription.
    </doc>
    <doc name = "rule">
      The server MUST ignore the message-selector field if the
      subscription already exists.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message seltype" domain = "message seltype">
    <doc name = "rule">
      The server MUST support the simple syntax and SHOULD support the
      JMS syntax.
    </doc>
  </field>

  <field name = "private" type = "bit">
    request private subscription
    <doc>
      If set, the subscription is private and owned by the current
      client.  This will fail if the subscription already exists and
      is owned by another client.  Private subscriptions cannot be
      consumed from by clients except the owner.
    </doc>
    <doc name = "rule">
      The server MUST support both private and shared subscriptions.
    </doc>
    <doc name = "rule">
      The server MUST use the client identifier supplied at connection
      open time to identify the owner of a private subscription. The
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
      If set, the subscription is cancelled when all clients have
      finished using it.
    </doc>
    <doc name = "rule">
      The server MUST implement the auto-cancel function in this
      manner: it counts the number of consumers using the subscription,
      and when the last consumer for the subscription is cancelled, it
      MUST delete the subscription and any messages it holds.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-cancel field if the destination
      already exists.
    </doc>
  </field>
</method>

<method name = "define ok" synchronous = "1">
  confirms a subscription definition
  <doc>
    This method confirms a Subscription.Define method and provides the
    client with the name of the subscription in case of automatically
    named subscriptions.  The server also reports the topic selector
    and number of messages for existing subscriptions.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "subscription" domain = "subscription">
    <doc>
      Confirms the name of the subscription. If the server generated
      a subscription name, this field contains that name.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "topic selector" domain = "topic selector" />

  <field name = "topic seltype" domain = "topic seltype" />

  <field name = "message count" type = "long">
    number of messages in subscription
    <doc>
      For existing subscriptions, provides the number of messages
      waiting to be consumed.  For new subscriptions this is zero.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "query" synchronous = "1">
  query status of subscription
  <doc>
    This method queries the status of a subscription.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "query ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "query"
      access rights for the subscription's access realm..
    </doc>
  </field>
    
  <field name = "subscription" domain = "subscription">
    subscription name
    <doc>
      Specifies the name of the subscription to query.  The
      subscription must exist.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "query ok" synchronous = "1">
  provide status of subscription
  <doc>
    This method returns the status of the subscription.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "subscription" domain = "subscription">
    <doc>
      Confirms the name of the subscription.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "message count" type = "long">
    number of messages in subscription
    <doc>
      Provides the number of pending messages in the subscription.
    </doc>
  </field>

  <field name = "consumer count" type = "long">
    number of consumers
    <doc>
      Provides the number of consumers for the subscription..
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "purge" synchronous = "1">
  purge a subscription 
  <doc>
    This method removes all messages from a subscription. It does not
    cancel any consumers on the subscription.
  </doc>
  <doc name = "rule">
    On transacted channels the server MUST not purge messages that have
    already been sent to a client but not yet acknowledged.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "purge ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      The access ticket must be for the access realm that holds the
      subscription.
    </doc>
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights for the subscription's access realm.
    </doc>
  </field>

  <field name = "subscription" domain = "subscription">
    <doc>
      The name of the subscription to purge.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "purge ok" synchronous = "1">
  confirms a subscription purge
  <doc>
    This method confirms the purge of a subscription.
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
  cancel a subscription
  <doc>
    This method cancels a subscription. Note that cancellation implies
    a purge: any pending messages in the subscription are discarded.
    All consumers for the subscription are also cancelled.
  </doc>
  <doc name = "rule">
    The client MUST use an access ticket that the server granted with
    "dynamic" access rights.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "cancel ok" />

  <field name = "ticket" domain = "access ticket">
    <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights to the subscrition's access realm.
    </doc>
  </field>

  <field name = "subscription" domain = "subscription">
    <doc>
      Specifies the name of the subscription. This subscription
      must exist. Attempting to cancel a non-existing subscription
      causes a connection exception.
    </doc>
  </field>
    
  <field name = "ifempty" type = "bit">
    cancel only if empty
    <doc>
      If set, the server will only cancel the subscription if it has no
      messages (for a queue).  If the subscription is not empty the
      server raises a channel exception.
    </doc>
  </field>
</method>

<method name = "cancel ok" synchronous = "1">
  confirm cancellation of a subscription
  <doc>
    This method confirms the cancellation of a subscription.
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

