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
    name    = "rest"
    handler = "connection"
    index   = "61501"
  >
  AMQP/Rest resource discovery and management class.

<doc>
    Provides methods to discover and work with server-side sinks and selectors
    as defined by the AMQP/Rest specification.  
</doc>

<doc name = "grammar">
    rest                = C:RESOLVE S:RESOLVE-OK
                        / C:PUT S:PUT-OK
                        / C:GET S:GET-OK
                        / C:DELETE S:DELETE-OK
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "resolve" synchronous = "1" index = "10">
  resolve a path
  <doc>
    If the provided path can be resolved, returns the sink name, the sink 
    type, and the selector.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "resolve-ok" />
  <field name = "path" type = "shortstr" >
    path to resolve
    <doc>
    Specifies the resource path to resolve.  As specifed by AMQP/Rest, a 
    path consists of a sink name, optionally followed by a slash and a 
    selector string.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "resolve-ok" synchronous = "1" index = "11">
  return resolved data
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the path was resolved and a sink name and type, and selector
    are provided.  If 1, the path could not be resolved.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
  <field name = "sink name" type = "shortstr" >
    Sink name
    <doc>
    Provides the resolved sink name, if the response indicator is zero.  
    Will be either an exchange or a shared queue that exists on the server.
    </doc>
  </field>
  <field name = "sink type" type = "shortstr" >
    Sink type
    <doc>
    Provides the resolved sink type, if the response indicator is zero.  
    Will be either an exchange type, or "rotator", or "service" for queues.
    </doc>
  </field>
  <field name = "selector" type = "shortstr" >
    Selector string
    <doc>
    Provides the resolved selector, if the response indicator is zero.  This
    will be all the path following the sink name and the trailing slash.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "put" synchronous = "1" index = "20">
  create a sink
  <doc>
    Creates a sink (an exchange or a queue) of the specified type.  The sink
    may already exist, this is not considered to be an error so long as the
    specified type is identical to that of the existing sink.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "put-ok" />
  <field name = "sink name" type = "shortstr" >
    Name of sink
    <doc>
    Specifies the name of the sink to create.  This is formatted as a path
    string, and may contain embedded slashes (but not at the start nor end).
    </doc>
    <assert check = "notnull" />
  </field>
  <field name = "sink type" type = "shortstr" >
    Sink type to create
    <doc>
    Specifies the type of the sink to create.  Valid values are: fanout, 
    direct, topic, headers, rotator, and service.  If the exchange already 
    exists with a different type, this method returns an error.
    </doc>
  </field>
</method>

<method name = "put-ok" synchronous = "1" index = "21">
  confirm sink exists
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded and the sink now exists.  If 1, the request
    failed, and the reply-text indicates the nature of the error.
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

<method name = "get" synchronous = "1" index = "30">
  queries a sink
  <doc>
    Queries a sink (an exchange or a queue) of the specified type.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "get-ok" />
  <field name = "sink name" type = "shortstr" >
    Name of sink
    <doc>
    Specifies the name of the sink to query.  This is formatted as a path
    string, and may contain embedded slashes (but not at the start nor end).
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "get-ok" synchronous = "1" index = "31">
  return sink information
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded and the sink exists.  If 1, the request
    failed, and the reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
  <field name = "properties" type = "table">
    Sink properties
    <doc>
      A set of named properties for the sink, which can be used for 
      monitoring or reporting purposes (i.e. formatted and shown to the
      user).
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1" index = "40">
  delete a sink
  <doc>
    Deletes a sink (an exchange or a queue) specified by name.  The sink does 
    not need to exist: deleting a non-existent sink is not considered to be an
    error.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />
  <field name = "sink name" type = "shortstr" >
    sink name
    <doc>
    Specifies the name of the sink to delete.  Must match exactly the name
    of the exchange or queue to delete.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "delete-ok" synchronous = "1" index = "41">
  confirm sink deletion
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded and the sink is now deleted.  If 1, 
    the request failed, and the reply-text indicates the nature of the error.
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
</method>

</class>

