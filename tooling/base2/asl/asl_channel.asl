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
    name    = "channel"
    handler = "channel"
    index   = "20"
  >
  work with channels
<doc>
  The channel class provides methods for a client to establish a virtual
  connection - a channel - to a server and for both peers to operate the
  virtual connection thereafter.
</doc>

<doc name = "grammar">
    channel             = open-channel *use-channel close-channel
    open-channel        = C:OPEN S:OPEN-OK
    use-channel         = C:FLOW S:FLOW-OK
                        / S:FLOW C:FLOW-OK
                        / S:ALERT
                        / functional-class
    close-channel       = C:CLOSE S:CLOSE-OK
                        / S:CLOSE C:CLOSE-OK
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "open" synchronous = "1" index = "10">
  open a channel for use
  <doc>
    This method opens a virtual connection (a channel).
  </doc>
  <doc name = "rule">
    This method is not allowed when the channel is already open.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "open-ok" />

  <field name = "out of band" type = "shortstr">
    out-of-band settings
    <doc>
      Configures out-of-band transfers on this channel.  The syntax and
      meaning of this field will be formally defined at a later date.
    </doc>
    <assert check = "null" />
  </field>
</method>

<method name = "open-ok" synchronous = "1" index = "11">
  signal that the channel is ready
  <doc>
    This method signals to the client that the channel is ready for use.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <field name = "channel id" type = "longstr">
    unique id of the channel
    <doc>
      Server assigns an unique id to each channel. The id can be used to
      generate unique names for private queues, exchanges and consumers.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "flow" synchronous = "1" index = "20">
  enable/disable flow from peer
  <doc>
    This method asks the peer to pause or restart the flow of content
    data. This is a simple flow-control mechanism that a peer can use
    to avoid oveflowing its queues or otherwise finding itself receiving
    more messages than it can process.  Note that this method is not
    intended for window control.  The peer that receives a request to
    stop sending content should finish sending the current content, if
    any, and then wait until it receives a Flow restart method.
  </doc>
  <doc name = "rule">
    When a new channel is opened, it is active.  Some applications
    assume that channels are inactive until started.  To emulate this
    behaviour a client should open the channel, then pause it.
  </doc>
  <doc name = "rule">
    When sending content data in multiple frames, a peer SHOULD monitor
    the channel for incoming methods and respond to a Channel.Flow as
    rapidly as possible.
  </doc>
  <doc name = "rule">
    A peer MAY use the Channel.Flow method to throttle incoming content
    data for internal reasons, for example, when exchangeing data over a
    slower connection.
  </doc>
  <doc name = "rule">
    The peer that requests a Channel.Flow method MAY disconnect and/or
    ban a peer that does not respect the request.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />
  <response name = "flow-ok" />

  <field name = "active" type = "bit">
    start/stop content frames
    <doc>
      If 1, the peer starts sending content frames.  If 0, the peer
      stops sending content frames.
    </doc>
  </field>
</method>

<method name = "flow-ok" index = "21">
  confirm a flow method
  <doc>
    Confirms to the peer that a flow command was received and processed.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <chassis name = "client" implement = "MUST" />

  <field name = "active" type = "bit">
    current flow setting
    <doc>
      Confirms the setting of the processed flow method: 1 means the
      peer will start sending or continue to send content frames; 0
      means it will not.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "close" synchronous = "1" index = "40">
  request a channel close
  <doc>
    This method indicates that the sender wants to close the channel.
    This may be due to internal conditions (e.g. a forced shut-down) or
    due to an error handling a specific method, i.e. an exception.  When
    a close is due to an exception, the sender provides the class and
    method id of the method which caused the exception.
  </doc>
  <doc name = "rule">
    After sending this method any received method except
    Channel.Close-OK MUST be discarded.
  </doc>
  <doc name = "rule">
    The peer sending this method MAY use a counter or timeout to detect
    failure of the other peer to respond correctly with Channel.Close-OK..
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "close-ok" />

  <field name = "reply code" domain = "reply code" />
  <field name = "reply text" domain = "reply text" />

  <field name = "class id" domain = "class id">
    failing method class
    <doc>
      When the close is provoked by a method exception, this is the
      class of the method.
    </doc>
  </field>

  <field name = "method id" domain = "method id">
    failing method ID
    <doc>
      When the close is provoked by a method exception, this is the
      ID of the method.
    </doc>
  </field>
</method>

<method name = "close-ok" synchronous = "1" index = "41">
  confirm a channel close
  <doc>
    This method confirms a Channel.Close method and tells the recipient
    that it is safe to release resources for the channel and close the
    socket.
  </doc>
  <doc name = "rule">
    A peer that detects a socket closure without having received a
    Channel.Close-Ok handshake method SHOULD log the error.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
</method>

</class>
