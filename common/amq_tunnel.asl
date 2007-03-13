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
    name    = "tunnel"
    handler = "tunnel"
    index   = "110"
  >
  methods for protocol tunneling.

<doc>
  The tunnel methods are used to send blocks of binary data - which
  can be serialised AMQP methods or other protocol frames - between
  AMQP peers.
</doc>

<doc name = "grammar">
    tunnel              = C:REQUEST
                        / S:REQUEST
</doc>

<chassis name = "server" implement = "MAY" />
<chassis name = "client" implement = "MAY" />

<field name = "headers" type = "table">
    Message header field table
</field>
<field name = "proxy name" type = "shortstr">
    The identity of the tunnelling proxy
</field>
<field name = "data name" type = "shortstr">
    The name or type of the message being tunnelled
</field>
<field name = "durable" type = "octet">
    The message durability indicator
</field>
<field name = "broadcast" type = "octet">
    The message broadcast mode
</field>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "request" content = "1" index = "10">
  sends a tunnelled method
  <doc>
    This method tunnels a block of binary data, which can be an
    encoded AMQP method or other data.  The binary data is sent
    as the content for the Tunnel.Request method.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <field name = "meta data" type = "table">
    meta data for the tunnelled block
    <doc>
    This field table holds arbitrary meta-data that the sender needs
    to pass to the recipient.
    </doc>
  </field>
</method>

</class>

