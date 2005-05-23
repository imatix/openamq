<?xml version="1.0"?>

<class
    name = "subscription"
    handler = "channel"
    >
work with subscriptions

<doc>
Subscriptions are queue objects, capable of storing all content domains.
This class allows clients to create and manage, and share subscriptions.
</doc>

<doc name = "grammar">
    subscription        = C:DEFINE S:DEFINED
                        / C:PURGE  S:PURGED
                        / C:CANCEL S:CANCELLED
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MAY"  />

<method name = "define" synchronous = "1" expect = "defined">
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
      Specifies the name of the subscription.
      </doc>
      <doc name = "rule">
      The subscription name MAY be empty, in which case the server
      MUST create a new unique name automatically and return this
      to the client in the Subscription.Defined method.
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
      The server MUST ignore the topic_selector field if the
      subscription already exists.
      </doc>
      <assert check = "notnull" />
    </field>

    <field name = "selector type" domain = "selector type">
    type of topic selector
      <doc>
      The simple syntax uses a hierarchical naming scheme for topics
      where each level is delimited by a dot. The wild cards * and #
      replace one and zero or more hierarchical levels respectively.
      The tpath syntax is the same as the simple syntax but the topic
      hierarchy is delimited by slash ("/") characters.  The regexp
      syntax uses Perl 5-compatible regular expressions and allows any
      topic naming scheme.
      </doc>
      <doc name = "rule">
      The server MUST support the simple syntax and MAY support the
      tpath and regexp syntaxes.
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

<method name = "defined" synchronous = "1">
confirms a subscription definition
    <doc>
    This method confirms a Subscription.Define method and provides the
    client with the name of the subscription in case of automatically
    named subscriptions.  The server also reports the topic selector
    and number of messages for existing subscriptions.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "subscription" domain = "subscription">
    name of subscription
      <doc>
      Confirms the name of the subscription. If the server generated
      a subscription name, this field contains that name.
      </doc>
      <assert check = "notnull" />
    </field>

    <field name = "topic selector" domain = "topic selector" />

    <field name = "selector type" domain = "selector type" />

    <field name = "message count" type = "long">
    number of messages in subscription
      <doc>
      For existing subscriptions, provides the number of messages
      waiting to be consumed.  For new subscriptions this is zero.
      </doc>
    </field>
</method>

<method name = "purge" synchronous = "1" expect = "purged">
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
    name of subscription
      <doc>
      The name of the subscription to purge.  This can be a configured
      subscription or a subscription.
      </doc>
      <assert check = "notnull" />
    </field>
</method>

<method name = "purged" synchronous = "1">
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

<method name = "cancel" synchronous = "1" expect = "cancelled">
    cancel a subscription
    <doc>
    This method cancels a subscription. Note that cancellation implies
    a purge: any pending messages in the subscription are discarded.
    All consumers for the subscription are also cancelled.
    </doc>
    <chassis name = "server" implement = "MUST" />
    <doc name = "rule">
    The client MUST use an access ticket that the server granted with
    "dynamic" access rights before sending this method.
    </doc>

    <field name = "ticket" domain = "access ticket">
      <doc name = "rule">
      The client MUST provide a valid access ticket giving "purge"
      access rights to the access realm used when defining the
      subscription.
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

<method name = "cancelled" synchronous = "1">
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

