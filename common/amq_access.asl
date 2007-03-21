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
    name    = "access"
    handler = "connection"
    index   = "30"
  >
  work with access tickets

<doc>
  The protocol control access to server resources using access tickets.
  A client must explicitly request access tickets before doing work.
  An access ticket grants a client the right to use a specific set of
  resources - called a "realm" - in specific ways.
</doc>

<doc name = "grammar">
    access              = C:REQUEST S:REQUEST-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "request" synchronous = "1" index = "10">
  request an access ticket
  <doc>
    This method requests an access ticket for an access realm.
    The server responds by granting the access ticket.  If the
    client does not have access rights to the requested realm
    this causes a connection exception.  Access tickets are a
    per-channel resource.
  </doc>
  <doc name = "rule">
    The realm name MUST start with either "/data" (for application
    resources) or "/admin" (for server administration resources).
    If the realm starts with any other path, the server MUST raise
    a connection exception with reply code 403 (access refused).
  </doc>
  <doc name = "rule">
    The server MUST implement the /data realm and MAY implement the
    /admin realm.  The mapping of resources to realms is not
    defined in the protocol - this is a server-side configuration
    issue.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "request-ok" />

  <field name = "realm" domain = "path" >
    name of requested realm
    <doc name = "rule">
      If the specified realm is not known to the server, the server
      must raise a channel exception with reply code 402 (invalid
      path).
    </doc>
  </field>

  <field name = "exclusive" type = "bit">
    request exclusive access
    <doc>
      Request exclusive access to the realm. If the server cannot grant
      this - because there are other active tickets for the realm - it
      raises a channel exception.
    </doc>
  </field>

  <field name = "passive" type = "bit">
    request passive access
    <doc>
      Request message passive access to the specified access realm.
      Passive access lets a client get information about resources in
      the realm but not to make any changes to them.
    </doc>
  </field>

  <field name = "active" type = "bit">
    request active access
    <doc>
      Request message active access to the specified access realm.
      Acvtive access lets a client get create and delete resources in
      the realm.
    </doc>
  </field>

  <field name = "write" type = "bit">
    request write access
    <doc>
      Request write access to the specified access realm.  Write access
      lets a client publish messages to all exchanges in the realm.
    </doc>
  </field>

  <field name = "read" type = "bit">
    request read access
    <doc>
      Request read access to the specified access realm.  Read access
      lets a client consume messages from queues in the realm.
    </doc>
  </field>
</method>

<method name = "request-ok" synchronous = "1" index = "11">
  grant access to server resources
  <doc>
    This method provides the client with an access ticket. The access
    ticket is valid within the current channel and for the lifespan of
    the channel.
  </doc>
  <doc name = "rule">
    The client MUST NOT use access tickets except within the same
    channel as originally granted.
  </doc>
  <doc name = "rule">
    The server MUST isolate access tickets per channel and treat an
    attempt by a client to mix these as a connection exception.
  </doc>
  <chassis name = "client" implement = "MUST" />

  <field name = "ticket" domain = "access ticket" />
</method>

</class>

