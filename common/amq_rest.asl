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
  RESTful resource access class.

<doc>
    Provides methods to access server resources in a RESTful manner.  In the 
    RESTful model, exchanges and queues share a single namespace - this is 
    more limited than the normal AMQP model.  Private queues are invisible to
    the RESTful interface, and cannot be created, queried, or deleted.
</doc>

<doc name = "grammar">
    rest                = C:PUT S:PUT-OK
                        / C:GET S:GET-OK
                        / C:DELETE S:DELETE-OK
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "put" synchronous = "1" index = "10">
  create a resource
  <doc>
    Creates an exchange or a shared queue specified as a resource path.  The 
    resource may already exist, this is not considered to be an error (behaviour 
    matches normal Declare method).
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "put-ok" />
  <field name = "path" type = "shortstr" >
    Resource path
    <doc>
    Specifies the path of the resource to create.  Must match exactly the name of
    the exchange or queue to create.
    </doc>
    <assert check = "notnull" />
  </field>
  <field name = "type" type = "shortstr" >
    Resource type
    <doc>
    Specifies the type of the resource.  Must be either one of the standard exchange
    types, or "queue".  If the exchange already exists with a different type, the 
    put method will return an error.
    </doc>
  </field>
</method>

<method name = "put-ok" synchronous = "1" index = "11">
  confirm resource creation
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded and the resource can now be used for work.  If 1, 
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

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "get" synchronous = "1" index = "20">
  query a resource
  <doc>
    Queries a resource path to determine whether it maps to an exchange or
    query, if any.  Returns the matching resource path and path-info (the
    part of the path, if any, following the resource path).
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "get-ok" />
  <field name = "path" type = "shortstr" >
    Resource path
    <doc>
    Specifies the path of the resource to query.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "get-ok" synchronous = "1" index = "21">
  confirm resource query
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the resource exists and the request succeeded.  If 1, the request 
    failed, and the reply-text indicates the nature of the error (usually, the
    resource does not exist).
    </doc>
  </field>
  <field name = "reply text" type = "shortstr">
    Failure message
    <doc>
    Indicates the cause of failure, if the response indicator is 1.
    </doc>
  </field>
  <field name = "path" type = "shortstr" >
    Resource path stripped of path info
    <doc>
    Provides the leading part of the path, matching the queue or exchange
    name if any was found.
    </doc>
  </field>
  <field name = "path info" type = "shortstr" >
    Resource path info
    <doc>
    Specifies the path info, if any and if the resource was found.
    </doc>
  </field>
  <field name = "type" type = "shortstr" >
    Resource type
    <doc>
    Provides the type of the resource, if found.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "delete" synchronous = "1" index = "30">
  delete a resource
  <doc>
    Deletes an exchange or a shared queue.  The resource need not exist.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "delete-ok" />
  <field name = "path" type = "shortstr" >
    Resource path
    <doc>
    Specifies the path of the resource to delete.  Must match exactly the name
    of the exchange or queue to delete.
    </doc>
    <assert check = "notnull" />
  </field>
</method>

<method name = "delete-ok" synchronous = "1" index = "31">
  confirm resource deletion
  <doc>
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "response" type = "bit">
    Failure/success indicator
    <doc>
    If zero, the request succeeded and the resource is now deleted.  If 1, 
    the request failed, and the reply-text indicates the nature of the error.
    Note that attempting to delete a non-existent resource is NOT an error.
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

