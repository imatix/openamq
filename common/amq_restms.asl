<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This code is licensed under both the GPLv3 and the IETF licence, in accordance
    with the terms of the http://www.restms.org Intellectual Property Policy.
 -->
<class
    name    = "restms"
    handler = "connection"
    index   = "61501"
  >
  RestMS resource management class.
<doc>
    Provides methods to work with server-side resources as defined by
    the RestMS specification.  All methods are request-only, without
    response.  Errors are logged at the server side and not reported
    to the client.  This model is designed to allow a RestMS server to
    push state to the AMQP server, rapidly and without handshaking.
    In future versions we may allow for asynchronous error reporting
    back to the RestMS server, probably via an AMQP exchange.
</doc>

<doc name = "grammar">
    restms              = C:PIPE-CREATE
                        / C:PIPE-DESTROY
                        / C:FEED-CREATE
                        / C:FEED-DESTROY
                        / C:JOIN-CREATE
                        / C:JOIN-DESTROY
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "pipe-create" index = "10">
  create a pipe
  <doc>
  Creates a pipe of the specified type.  The pipe may already exist,
  if it has the same type.  Pipe names are unique across all types.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "pipe type" type = "shortstr" >
    pipe type
    <doc>
    Specifies the type of the pipe to create.  Valid values are: pipe.
    </doc>
  </field>
  <field name = "pipe name" type = "shortstr" >
    Name of pipe
    <doc>
    Specifies the name of the pipe to create.  Pipe names may not contain
    slashes, spaces, or at signs.
    </doc>
  </field>
</method>

<method name = "pipe-delete" index = "20">
  delete a pipe
  <doc>
  Deletes a specified pipe, if it exists.  Safe to invoke on non-existent
  or already-deleted pipes.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "pipe name" type = "shortstr" >
    pipe name
    <doc>
    Specifies the name of the pipe to delete.
    </doc>
  </field>
</method>

<method name = "feed-create" index = "30">
  create a feed
  <doc>
  Creates a feed of the specified type.  The feed may already exist,
  if it has the same type.  Feed names are unique across all types.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "feed type" type = "shortstr" >
    Feed type
    <doc>
    Specifies the type of the feed to create.  Valid values are: fanout,
    direct, topic, headers, system, rotator, and service.
    </doc>
  </field>
  <field name = "feed name" type = "shortstr" >
    Name of feed
    <doc>
    Specifies the name of the feed to create.  Feed names may not contain
    slashes, spaces, or at signs.
    </doc>
  </field>
</method>

<method name = "feed-delete" index = "40">
  delete a feed
  <doc>
  Deletes a specified feed, if it exists.  Safe to invoke on non-existent
  or already-deleted feeds.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "feed name" type = "shortstr" >
    feed name
    <doc>
    Specifies the name of the feed to delete.
    </doc>
  </field>
</method>

<method name = "join-create" index = "50">
  create a join
  <doc>
  Creates a join on the specified pipe and feed.  The join may already
  exist, if it has the same properties.  A join will causes messages to
  be delivered on the connection.  The consumer-tag property allows
  messages to be routed into end-application pipes.  Joins on exchange
  feeds use the consumer tag "x:{pipe-name}" and joins on queue feeds
  use the consumer tag "q:{pipe-name}".  AMQP does not allow the same
  tag to be used on multiple queues.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "pipe name" type = "shortstr" >
    Name of pipe
    <doc>
    Specifies the name of the pipe, which must exist.
    </doc>
  </field>
  <field name = "feed name" type = "shortstr" >
    Name of feed
    <doc>
    Specifies the name of the feed, which must exist.
    </doc>
  </field>
  <field name = "address" type = "shortstr" >
    Join address
    <doc>
    Specifies the address to join.  This is an address literal or
    pattern who's semantics depend on the feed type.  The address
    may not contain slashes, spaces, or at signs.
    </doc>
  </field>
</method>

<method name = "join-delete" index = "60">
  delete a join
  <doc>
  Deletes a specified join, if it exists.  Safe to invoke on non-existent
  or already-deleted joins, and referring to non-existent pipes and/or
  feeds.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "pipe name" type = "shortstr" >
    Name of pipe
    <doc>
    Specifies the name of the pipe, which does not need to exist.
    </doc>
  </field>
  <field name = "feed name" type = "shortstr" >
    Name of feed
    <doc>
    Specifies the name of the feed, which does not need to exist.
    </doc>
  </field>
  <field name = "address" type = "shortstr" >
    Join address
    <doc>
    Specifies the join address.
    </doc>
  </field>
</method>

</class>
