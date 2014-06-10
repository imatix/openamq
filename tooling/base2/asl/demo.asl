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
    name     = "demo"
    comment  = "ASL demo protocol"
    script   = "asl_gen"
    target   = "stdc"
    >
  <inherit name = "asl_base" />
  <include filename = "demo_exchange.asl" />
  <include filename = "demo_queue.asl" />
  <include filename = "demo_basic.asl" />
  <include filename = "asl_direct.asl" />

  <option name = "protocol_name"     value = "DEMO" />
  <option name = "protocol_port"     value = "7654" />
  <option name = "protocol_version"  value = "\\x00\\x00\\x09\\x01" />
  <option name = "protocol_accept"   value = "\\x01\\x01\\x00\\x09" />
  <option name = "protocol_major"    value = "0" />
  <option name = "protocol_minor"    value = "9" />

  <!-- Standard field domains -->

  <domain name = "exchange name" type = "shortstr">
    exchange name
    <doc>
    This name identifies the exchange within the virtual host.
    Exchange names are case-sensitive.
    </doc>
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "queue name" type = "shortstr">
    queue name
    <doc>
    The que	ue name identifies the queue within the scope and the
    virtual host. Queue names are case-sensitive.
    </doc>
    <assert check = "length" value = "127" />
  </domain>

  <domain name = "consumer tag" type = "shortstr">
    consumer tag
    <doc>
      Identifier for the consumer, valid within the current connection.
    </doc>
    <doc name = "rule">
      The consumer tag is valid only within the channel from which the
      consumer was created. I.e. a client may not create a consumer in
      one channel and then use it in another.
    </doc>
  </domain>

  <domain name = "delivery tag" type = "longlong">
    server-assigned delivery tag
    <doc>
      The server-assigned and channel-specific delivery tag
    </doc>
    <doc name = "rule">
      The delivery tag is valid only within the channel from which the
      message was received.  I.e. a client may not receive a message on
      one channel and then acknowledge it on another.
    </doc>
    <doc name = "rule">
      The server MUST NOT use a zero value for delivery tags.  Zero is
      reserved for client use, meaning "all messages so far received".
    </doc>
  </domain>

</protocol>
