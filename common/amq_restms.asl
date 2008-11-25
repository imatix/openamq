<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2007 iMatix Corporation

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
    name    = "restms"
    handler = "connection"
    index   = "61501"
  >
  RestMS resource discovery and management class.

<doc>
    Provides methods to discover and work with server-side feeds and selectors
    as defined by the RestMS specification.
</doc>

<doc name = "grammar">
    restms              = C:FEED-CREATE S:FEED-CREATE-OK
                        / C:FEED-QUERY  S:FEED-QUERY-OK
                        / C:FEED-DELETE S:FEED-DELETE-OK
                        / C:FEED-CLASS-QUERY S:FEED-CLASS-QUERY-OK
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "feed-create" synchronous = "1" index = "10">
  create a feed
  <doc>
  Creates a feed of the specified class.  The feed may already exist,
  if it has the same class.  Feed names are unique across all classes.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "feed-create-ok" />
  <field name = "class name" type = "shortstr" >
    Feed class
    <doc>
    Specifies the class of the feed to create.  Valid values are: fanout,
    direct, topic, headers, rotator, and service.
    </doc>
  </field>
  <field name = "feed name" type = "shortstr" >
    Name of feed
    <doc>
    Specifies the name of the feed to create.  Feed names may not contain
    slashes.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "feed-create-ok" synchronous = "1" index = "11">
  confirm feed exists
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded.  If 1, the request failed, and the
    reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "feed-query" synchronous = "1" index = "20">
  queries a feed
  <doc>
  Gets information about a specified feed.  If the feed exists, returns
  a set of properties that provide information about the feed.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "feed-query-ok" />
  <field name = "feed name" type = "shortstr" >
    Name of feed
    <doc>
    Specifies the name of the feed to query.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "feed-query-ok" synchronous = "1" index = "21">
  return feed properties
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded.  If 1, the request failed, and the
    reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
  <field name = "properties" type = "table">
    Feed properties
    <doc>
    A set of named properties for the feed, which can be used for
    monitoring or reporting purposes (i.e. formatted and shown to the
    user).
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "feed-delete" synchronous = "1" index = "30">
  delete a feed
  <doc>
  Deletes a specified feed, if it exists.  Safe to invoke on non-existent
  or already-deleted feeds.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "feed-delete-ok" />
  <field name = "feed name" type = "shortstr" >
    feed name
    <doc>
    Specifies the name of the feed to delete.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "feed-delete-ok" synchronous = "1" index = "31">
  confirm feed deletion
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded.  If 1, the request failed, and the
    reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "feed-class-query" synchronous = "1" index = "40">
  queries a feed class
  <doc>
    Queries a feed class and returns a list of all feeds in that class.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "feed-class-query-ok" />
  <field name = "class name" type = "shortstr" >
    Name of feed class
    <doc>
    Specifies the name of the class to query.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "feed-class-query-ok" synchronous = "1" index = "41">
  return class information
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded.  If 1, the request failed, and the
    reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
  <field name = "feed list" type = "longstr">
    List of feeds
    <doc>
    Contains a list of feed names, delimited by spaces, contained in the
    class.
    </doc>
  </field>
</method>

</class>
