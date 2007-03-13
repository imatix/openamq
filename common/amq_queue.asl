<?xml version="1.0"?>
<!--
    Copyright (c) 2007 iMatix Corporation

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

    This file contains excerpts from the AMQP specification.  The copyright
    and license for these excerpts is detailed below:

    Copyright Notice
    ================
    (c) Copyright JPMorgan Chase Bank & Co., Cisco Systems, Inc., 
    Envoy Technologies Inc., iMatix Corporation, IONA Technologies, 
    Red Hat, Inc., TWIST Process Innovations, and 29West Inc. 2006. 
    All rights reserved.
 
    License
    =======
    JPMorgan Chase Bank & Co., Cisco Systems, Inc., Envoy Technologies Inc., 
    iMatix Corporation, IONA Technologies, Red Hat, Inc., 
    TWIST Process Innovations, and 29West Inc. (collectively, the
    "Authors") each hereby grants to you a worldwide, perpetual,
    royalty-free, nontransferable, nonexclusive license to (i) copy,
    display, distribute and implement the Advanced Messaging Queue Protocol
    ("AMQP") Specification and (ii) the Licensed Claims that are held by
    the Authors, all for the purpose of implementing the Advanced Messaging
    Queue Protocol Specification. Your license and any rights under this
    Agreement will terminate immediately without notice from any Author if
    you bring any claim, suit, demand, or action related to the Advanced
    Messaging Queue Protocol Specification against any Author.  Upon
    termination, you shall destroy all copies of the Advanced Messaging
    Queue Protocol Specification in your possession or control.

    As used hereunder, "Licensed Claims" means those claims of a patent or
    patent application, throughout the world, excluding design patents and
    design registrations, owned or controlled, or that can be sublicensed
    without fee and in compliance with the requirements of this Agreement,
    by an Author or its affiliates now or at any future time and which
    would necessarily be infringed by implementation of the Advanced
    Messaging Queue Protocol Specification. A claim is necessarily
    infringed hereunder only when it is not possible to avoid infringing it
    because there is no plausible non-infringing alternative for
    implementing the required portions of the Advanced Messaging Queue
    Protocol Specification. Notwithstanding the foregoing, Licensed Claims
    shall not include any claims other than as set forth above even if
    contained in the same patent as Licensed Claims; or that read solely on
    any implementations of any portion of the Advanced Messaging Queue
    Protocol Specification that are not required by the Advanced Messaging
    Queue Protocol Specification, or that, if licensed, would require a
    payment of royalties by the licensor to unaffiliated third parties.
    Moreover, Licensed Claims shall not include (i) any enabling
    technologies that may be necessary to make or use any Licensed Product
    but are not themselves expressly set forth in the Advanced Messaging
    Queue Protocol Specification (e.g., semiconductor manufacturing
    technology, compiler technology, object oriented technology, networking
    technology, operating system technology, and the like); or (ii) the
    implementation of other published standards developed elsewhere and
    merely referred to in the body of the Advanced Messaging Queue Protocol
    Specification, or (iii) any Licensed Product and any combinations
    thereof the purpose or function of which is not required for compliance
    with the Advanced Messaging Queue Protocol Specification. For purposes
    of this definition, the Advanced Messaging Queue Protocol Specification
    shall be deemed to include both architectural and interconnection
    requirements essential for interoperability and may also include
    supporting source code artifacts where such architectural,
    interconnection requirements and source code artifacts are expressly
    identified as being required or documentation to achieve compliance
    with the Advanced Messaging Queue Protocol Specification.
 
    As used hereunder, "Licensed Products" means only those specific
    portions of products (hardware, software or combinations thereof) that
    implement and are compliant with all relevant portions of the Advanced
    Messaging Queue Protocol Specification.
 
    The following disclaimers, which you hereby also acknowledge as to any
    use you may make of the Advanced Messaging Queue Protocol
    Specification:
 
    THE ADVANCED MESSAGING QUEUE PROTOCOL SPECIFICATION IS PROVIDED "AS
    IS," AND THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR
    IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, OR TITLE; THAT THE
    CONTENTS OF THE ADVANCED MESSAGING QUEUE PROTOCOL SPECIFICATION ARE
    SUITABLE FOR ANY PURPOSE; NOR THAT THE IMPLEMENTATION OF THE ADVANCED
    MESSAGING QUEUE PROTOCOL SPECIFICATION WILL NOT INFRINGE ANY THIRD
    PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
 
    THE AUTHORS WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL,
    INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR RELATING TO ANY
    USE, IMPLEMENTATION OR DISTRIBUTION OF THE ADVANCED MESSAGING QUEUE
    PROTOCOL SPECIFICATION.
 
    The name and trademarks of the Authors may NOT be used in any manner,
    including advertising or publicity pertaining to the Advanced Messaging
    Queue Protocol Specification or its contents without specific, written
    prior permission. Title to copyright in the Advanced Messaging Queue
    Protocol Specification will at all times remain with the Authors.
 
    No other rights are granted by implication, estoppel or otherwise.
 
    Upon termination of your license or rights under this Agreement, you
    shall destroy all copies of the Advanced Messaging Queue Protocol
    Specification in your possession or control.
 
    Trademarks
    ==========
    "JPMorgan", "JPMorgan Chase", "Chase", the JPMorgan Chase logo and the
    Octagon Symbol are trademarks of JPMorgan Chase & Co.
 
    IMATIX and the iMatix logo are trademarks of iMatix Corporation sprl.
 
    IONA, IONA Technologies, and the IONA logos are trademarks of 
    IONA Technologies PLC and/or its subsidiaries.
 
    LINUX is a trademark of Linus Torvalds. RED HAT and JBOSS are
    registered trademarks of Red Hat, Inc. in the US and other countries.
 
    Java, all Java-based trademarks and OpenOffice.org are trademarks of
    Sun Microsystems, Inc. in the United States, other countries, or both.
 
    Other company, product, or service names may be trademarks or service
    marks of others.
 
    Links to full AMQP specification:
    =================================
    http://www.amqp.org/
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
                        / C:PURGE    S:PURGE-OK
                        / C:DELETE   S:DELETE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<doc name = "rule" test = "amq_queue_33">
  A server MUST allow any content class to be sent to any queue, in any
  mix, and queue and delivery these content classes independently. Note
  that all methods that fetch content off queues are specific to a given
  content class.
</doc>


<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "declare" synchronous = "1" index = "10">
  declare queue, create if needed
  <doc>
    This method creates or checks a queue.  When creating a new queue
    the client can specify various properties that control the durability
    of the queue and its contents, and the level of sharing for the queue.
  </doc>
  <doc name = "rule" test = "amq_queue_34">
    The server MUST create a default binding for a newly-created queue
    to the default exchange, which is an exchange of type 'direct'.
  </doc>
  <doc name = "rule" test = "amq_queue_35">
    The server SHOULD support a minimum of 256 queues per virtual host
    and ideally, impose no limit except as defined by available resources.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "declare-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      When a client defines a new queue, this belongs to the access realm
      of the ticket used.  All further work done with that queue must be
      done with an access ticket for the same realm.
    </doc>
    <doc>
      The client provides a valid access ticket giving "active" access
      to the realm in which the queue exists or will be created, or
      "passive" access if the if-exists flag is set.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc name = "rule" test = "amq_queue_10">
      The queue name MAY be empty, in which case the server MUST create
      a new queue with a unique generated name and return this to the
      client in the Declare-Ok method.
    </doc>
    <doc name = "rule" test = "amq_queue_32">
      Queue names starting with "amq." are reserved for predeclared and
      standardised server queues.  If the queue name starts with "amq."
      and the passive option is zero, the server MUST raise a connection
      exception with reply code 403 (access refused).
    </doc>
    <assert check = "regexp" value = "^[a-zA-Z0-9-_.:/]*$" />
  </field>

  <field name = "passive" type = "bit">
    do not create queue
    <doc>
    If set, the server will not create the queue.  The client can use
    this to check whether a queue exists without modifying the server
    state.
    </doc>
    <doc name = "rule" test = "amq_queue_05" >
      If set, and the queue does not already exist, the server MUST
      respond with a reply code 404 (not found) and raise a channel
      exception.
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
    <doc name = "rule" test = "amq_queue_03">
      The server MUST recreate the durable queue after a restart.
    </doc>
    <doc name = "rule" test = "amq_queue_36">
      The server MUST support both durable and transient queues.
    </doc>
    <doc name = "rule" test = "amq_queue_37">
      The server MUST ignore the durable field if the queue already
      exists.
    </doc>
  </field>

  <field name = "exclusive" type = "bit">
    request an exclusive queue
    <doc>
      Exclusive queues may only be consumed from by the current connection.
      Setting the 'exclusive' flag always implies 'auto-delete'.
    </doc>
    <doc name = "rule" test = "amq_queue_38">
      The server MUST support both exclusive (private) and non-exclusive
      (shared) queues.
    </doc>
    <doc name = "rule" test = "amq_queue_04">
      The server MUST raise a channel exception if 'exclusive' is specified
      and the queue already exists and is owned by a different connection.
    </doc>
  </field>

  <field name = "auto delete" type = "bit">
    auto-delete queue when unused
    <doc>
      If set, the queue is deleted when all consumers have finished
      using it. Last consumer can be cancelled either explicitly or because
      its channel is closed. If there was no consumer ever on the queue, it
      won't be deleted.
    </doc>
    <doc name = "rule" test = "amq_queue_02">
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
    <doc name = "rule" test = "amq_queue_31">
      The server MUST ignore the auto-delete field if the queue already
      exists.
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

  <field name = "message count" type = "long">
    number of messages in queue
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

<method name = "bind" synchronous = "1" index = "20">
  bind queue to an exchange
  <doc>
    This method binds a queue to an exchange.  Until a queue is
    bound it will not receive any messages.  In a classic messaging
    model, store-and-forward queues are bound to a dest exchange
    and subscription queues are bound to a dest_wild exchange.
  </doc>
  <doc name = "rule" test = "amq_queue_25">
    A server MUST allow ignore duplicate bindings - that is, two or
    more bind methods for a specific queue, with identical arguments
    - without treating these as an error.
  </doc>
  <doc name = "rule" test = "amq_queue_39">
    If a bind fails, the server MUST raise a connection exception.
  </doc>
  <doc name = "rule" test = "amq_queue_12">
    The server MUST NOT allow a durable queue to bind to a transient
    exchange. If the client attempts this the server MUST raise a
    channel exception.
  </doc>
  <doc name = "rule" test = "amq_queue_13">
    Bindings for durable queues are automatically durable and the
    server SHOULD restore such bindings after a server restart.
  </doc>
  <doc name = "rule" test = "amq_queue_17">
    If the client attempts to an exchange that was declared as internal,
    the server MUST raise a connection exception with reply code 530
    (not allowed).
  </doc>
  <doc name = "rule" test = "amq_queue_40">
    The server SHOULD support at least 4 bindings per queue, and
    ideally, impose no limit except as defined by available resources.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "bind-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      The client provides a valid access ticket giving "active"
      access rights to the queue's access realm.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to bind.  If the queue name is
      empty, refers to the current queue for the channel, which is
      the last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue
      name in this method is empty, the server MUST raise a connection
      exception with reply code 530 (not allowed).
    </doc>
    <doc name = "rule" test = "amq_queue_26">
      If the queue does not exist the server MUST raise a channel exception
      with reply code 404 (not found).
    </doc>
  </field>

  <field name = "exchange" domain = "exchange name">
    <doc name = "rule">
      The name of the exchange to bind to.
    </doc>
    <doc name = "rule" test = "amq_queue_14">
      If the exchange does not exist the server MUST raise a channel
      exception with reply code 404 (not found).
    </doc>
  </field>

  <field name = "routing key" type = "shortstr">
     message routing key
    <doc>
      Specifies the routing key for the binding.  The routing key is
      used for routing messages depending on the exchange configuration.
      Not all exchanges use a routing key - refer to the specific
      exchange documentation. If the routing key is empty and the queue
      name is empty, the routing key will be the current queue for the
      channel, which is the last declared queue.
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

<method name = "unbind" synchronous = "1" index = "22">
  unbind queue from an exchange
  <doc>
    This method unbinds a queue from an exchange.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <doc name = "rule">
  TODO: What happens if specified binding does not exist?
  </doc>
  <response name = "unbind-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      The client provides a valid access ticket giving "active"
      access rights to the queue's access realm.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to unbind.  If the queue name is
      empty, refers to the current queue for the channel, which is
      the last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue
      name in this method is empty, the server MUST raise a connection
      exception with reply code 530 (not allowed).
    </doc>
    <doc name = "rule" test = "amq_queue_26">
      If the queue does not exist the server MUST raise a channel exception
      with reply code 404 (not found).
    </doc>
  </field>

  <field name = "exchange" domain = "exchange name">
    <doc name = "rule">
      The name of the exchange to unbind from.
    </doc>
    <doc name = "rule" test = "amq_queue_14">
      If the exchange does not exist the server MUST raise a channel
      exception with reply code 404 (not found).
    </doc>
  </field>

  <field name = "routing key" type = "shortstr">
     message routing key
    <doc>
      Specifies the routing key of the binding to unbind.
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
    arguments for binding
    <doc>
      A set of arguments of the binding to unbind.
    </doc>
  </field>
</method>

<method name = "unbind-ok" synchronous = "1" index = "23">
  confirm unbind successful
  <doc>
    This method confirms that the unbind was successful.
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
  <doc name = "rule" test = "amq_queue_15">
    A call to purge MUST result in an empty queue.
  </doc>
  <doc name = "rule" test = "amq_queue_41">
    On transacted channels the server MUST not purge messages that have
    already been sent to a client but not yet acknowledged.
  </doc>
  <doc name = "rule" test = "amq_queue_42">
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

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to purge.  If the queue name is
      empty, refers to the current queue for the channel, which is
      the last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue
      name in this method is empty, the server MUST raise a connection
      exception with reply code 530 (not allowed).
    </doc>
    <doc name = "rule" test = "amq_queue_16">
      The queue must exist. Attempting to purge a non-existing queue
      causes a channel exception.
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
    server configuration, and all consumers on the queue are cancelled.
  </doc>
  <doc name = "rule" test = "amq_queue_43">
    The server SHOULD use a dead-letter queue to hold messages that
    were pending on a deleted queue, and MAY provide facilities for
    a system administrator to move these messages back to an active
    queue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />

  <field name = "ticket" domain = "access ticket">
    <doc>
      The client provides a valid access ticket giving "active"
      access rights to the queue's access realm.
    </doc>
  </field>

  <field name = "queue" domain = "queue name">
    <doc>
      Specifies the name of the queue to delete. If the queue name is
      empty, refers to the current queue for the channel, which is the
      last declared queue.
    </doc>
    <doc name = "rule">
      If the client did not previously declare a queue, and the queue
      name in this method is empty, the server MUST raise a connection
      exception with reply code 530 (not allowed).
    </doc>
    <doc name = "rule" test = "amq_queue_21">
      The queue must exist. Attempting to delete a non-existing queue
      causes a channel exception.
    </doc>
  </field>

  <field name = "if unused" type = "bit" test = "amq_queue_29">
    delete only if unused
    <doc>
      If set, the server will only delete the queue if it has no
      consumers. If the queue has consumers the server does does not
      delete it but raises a channel exception instead.
    </doc>
    <doc name = "rule" test = "amq_queue_30">
      The server MUST respect the if-unused flag when deleting a queue.
    </doc>
  </field>

  <field name = "if empty" type = "bit" test = "amq_queue_27">
    delete only if empty
    <doc>
      If set, the server will only delete the queue if it has no
      messages. If the queue is not empty the server raises a channel
      exception.
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
