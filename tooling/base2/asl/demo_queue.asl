<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name    = "queue"
    handler = "channel"
    index   = "50"
  >
  work with queues

<doc>
  Queues store and forward messages.  Queues can be configured in the server
  or created at runtime.  Queues must be attached to at least one exchange
  in order to receive messages from publishers.
</doc>

<doc name = "grammar">
    queue               = C:DECLARE  S:DECLARE-OK
                        / C:BIND     S:BIND-OK
                        / C:CANCEL   S:CANCEL-OK
                        / C:PURGE    S:PURGE-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "declare" synchronous = "1" index = "10">
  declare queue, create if needed
  <doc>
    This method creates or checks a queue.  When creating a new queue
    the client can specify various properties that control the durability
    of the queue and its contents, and the level of sharing for the queue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "declare-ok" />

  <field name = "queue" domain = "queue name">
    <assert check = "regexp" value = "^[a-zA-Z0-9-_]+$" />
  </field>

  <field name = "passive" type = "bit">
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

  <field name = "exclusive" type = "bit">
    request an exclusive queue
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
      The server SHOULD allow for a reasonable delay between the point
      when it determines that a queue is not being used (or no longer
      used), and the point when it deletes the queue.  At the least it
      must allow a client to create a queue and then create a consumer
      to read from it, with a small but non-zero delay between these
      two actions.  The server should equally allow for clients that may
      be disconnected prematurely, and wish to re-consume from the same
      queue without losing messages.  We would recommend a configurable
      timeout, with a suitable default value being one minute.
    </doc>
    <doc name = "rule">
      The server MUST ignore the auto-delete field if the queue already
      exists.
    </doc>
  </field>
</method>


<method name = "declare-ok" synchronous = "1" index = "11">
  confirms a queue definition
  <doc>
    This method confirms a Declare method and confirms the name of the
    queue, essential for automatically-named queues.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "queue" domain = "queue name">
    <doc>
      Reports the name of the queue. If the server generated a queue
      name, this field contains that name.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "properties" type = "table">
    queue properties
    <doc>
      Returns extended properties for the queue in a field table.
    </doc>
  </field>
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "bind" synchronous = "1" index = "20">
  bind queue to an exchange
  <doc>
    This method binds a queue to an exchange.  Until a queue is
    bound it will not receive any messages.  In a classic messaging
    model, store-and-forward queues are bound to a dest-name exchange
    and subscription queues are bound to a dest-wild exchange.
  </doc>
  <doc name = "rule">
    A server MUST allow ignore duplicate bindings - that is, two or
    more bind methods for a specific queue, with identical arguments
    - without treating these as an error.
  </doc>
  <doc name = "rule">
    If a bind fails, the server MUST raise a connection exception.
  </doc>
  <doc name = "rule">
    The server MUST NOT allow a durable queue to bind to a transient
    exchange. If the client attempts this the server MUST raise a
    channel exception.
  </doc>
  <doc name = "rule">
    Bindings for durable queues are automatically durable and the
    server SHOULD restore such bindings after a server restart.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "bind-ok" />

  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue must exist. Attempting to query a non-existing queue
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>

  <field name = "exchange" domain = "exchange name">
    <doc>
      The name of the exchange to bind to. If the exchange does not
      exist the server will raise a channel exception.
    </doc>
  </field>

  <field name = "arguments" type = "table">
    arguments for binding
    <doc>
      A set of arguments for the binding.  The syntax and semantics of
      these arguments depends on the exchange class.
    </doc>
  </field>
</method>

<method name = "bind-ok" synchronous = "1" index = "21">
  confirm bind successful
  <doc>
    This method confirms that the bind was successful.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "purge" synchronous = "1" index = "30">
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

  <field name = "queue" domain = "queue name">
    <doc name = "rule">
      The queue must exist. Attempting to purge a non-existing queue
      causes a channel exception.
    </doc>
    <assert check = "notnull" />
  </field>
</method>


<method name = "purge-ok" synchronous = "1" index = "31">
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

<method name = "delete" synchronous = "1" index = "40">
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


<method name = "delete-ok" synchronous = "1" index = "41">
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

