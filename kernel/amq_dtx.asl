<?xml version="1.0"?>
<class
    name    = "dtx"
    handler = "channel"
    index   = "100"
  >
  work with distributed transactions

<doc>
  Distributed transactions provide so-called "2-phase commit".  This
  is slower and more complex than standard transactions but provides
  more assurance that messages will be delivered exactly once.  The
  AMQP distributed transaction model supports the X-Open XA
  architecture and other distributed transaction implementations.
  The Dtx class assumes that the server has a private communications
  channel (not AMQP) to a distributed transaction coordinator.
</doc>

<doc name = "grammar">
    dtx                 = C:SELECT S:SELECT-OK
                          C:START S:START-OK
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "select" synchronous = "1" index = "10">
select standard transaction mode
  <doc>
    This method sets the channel to use distributed transactions.  The
    client must use this method at least once on a channel before
    using the Start method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "select-ok" />
</method>

<method name = "select-ok" synchronous = "1" index = "11">
confirm transaction mode
  <doc>
    This method confirms to the client that the channel was successfully
    set to use distributed transactions.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "start" synchronous = "1" index = "20">
  start a new distributed transaction
  <doc>
    This method starts a new distributed transaction.  This must be
    the first method on a new channel that uses the distributed
    transaction mode, before any methods that publish or consume
    messages.
  </doc>
  <chassis name = "server" implement = "MAY" />
  <response name = "start-ok" />

  <field name = "dtx identifier" type = "shortstr">
    transaction identifier
    <doc>
      The distributed transaction key. This identifies the transaction
      so that the AMQP server can coordinate with the distributed
      transaction coordinator.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "start-ok" synchronous = "1" index = "21">
  confirm the start of a new distributed transaction
  <doc>
    This method confirms to the client that the transaction started.
    Note that if a start fails, the server raises a channel exception.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

</class>

