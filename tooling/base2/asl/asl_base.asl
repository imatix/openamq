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
<protocol
    name    = "asl_base"
    comment = "ASL base specifications"
    version = "1.0"
    script  = "asl_gen"
    >
  <!-- Classes -->
  <include filename = "asl_constants.asl"  />
  <include filename = "asl_connection.asl" />
  <include filename = "asl_channel.asl"    />

  <!-- All methods start with this header  -->
  <class name = "template" abstract = "1" inherit = "none">
    <method name = "template" abstract = "1" inherit = "none">
      <field name = "std class id"  domain = "class id"  global = "1" >class ID</field>
      <field name = "std method id" domain = "method id" global = "1" >method ID</field>
    </method>
  </class>

  <!-- Standard field domains -->
  <domain name = "class id"  type = "short" />
  <domain name = "method id" type = "short" />

  <domain name = "path" type = "shortstr">
    <doc>
      Must start with a slash "/" and continue with path names
      separated by slashes. A path name consists of any combination
      of at least one of [A-Za-z0-9] plus zero or more of [.-_+!=:].
    </doc>
    <assert check = "notnull" />
    <assert check = "syntax" rule = "path" />
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "peer properties" type = "table">
    <doc>
    This string provides a set of peer properties, used for
    identification, debugging, and general information.
    </doc>
    <doc name = "rule">
    The properties SHOULD contain these fields:
    "product", giving the name of the peer product, "version", giving
    the name of the peer version, "platform", giving the name of the
    operating system, "copyright", if appropriate, and "information",
    giving other general information.
    </doc>
  </domain>

  <domain name = "known hosts" type = "shortstr">
    list of known hosts
    <doc>
    Specifies the list of equivalent or alternative hosts that the server
    knows about, which will normally include the current server itself.
    Clients can cache this information and use it when reconnecting to a
    server after a failure.
    </doc>
    <doc name = "rule">
    The server MAY leave this field empty if it knows of no other
    hosts than itself.
    </doc>
  </domain>

  <domain name = "reply code" type = "short">
    reply code from server
    <doc>
      The reply code. The AMQ reply codes are defined in AMQ RFC 011.
    </doc>
    <assert check = "notnull" />
  </domain>

  <domain name = "reply text" type = "shortstr">
    localised reply text
    <doc>
      The localised reply text.  This text can be logged as an aid to
      resolving issues.
    </doc>
    <assert check = "notnull" />
  </domain>
</protocol>
