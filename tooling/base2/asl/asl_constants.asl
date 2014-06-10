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
<constant name = "frame method"         value = "1" />
<constant name = "frame header"         value = "2" />
<constant name = "frame body"           value = "3" />
<constant name = "frame oob method"     value = "4" />
<constant name = "frame oob header"     value = "5" />
<constant name = "frame oob body"       value = "6" />
<constant name = "frame trace"          value = "7" />
<constant name = "frame heartbeat"      value = "8" />

<constant name = "frame min size"       value = "4096" />
<constant name = "frame end"            value = "206" />

<!-- These are the standard reply codes -->
<constant name = "reply success" value = "200">
  Indicates that the method completed successfully. This reply code is
  reserved for future use - the current protocol design does not use
  positive confirmation and reply codes are sent only in case of an
  error.
</constant>

<constant name = "not delivered" value = "310" class = "soft error">
  The client asked for a specific message that is no longer available.
  The message was delivered to another client, or was purged from the
  queue for some other reason.
</constant>

<constant name = "content too large" value = "311" class = "soft error">
  The client attempted to transfer content larger than the server
  could accept at the present time.  The client may retry at a later
  time.
</constant>

<constant name = "connection forced" value = "320" class = "hard error">
  An operator intervened to close the connection for some reason.
  The client may retry at some later date.
</constant>

<constant name = "invalid path" value = "402" class = "hard error">
  The client tried to work with an unknown virtual host or cluster.
</constant>

<constant name = "access refused" value = "403" class = "soft error">
  The client attempted to work with a server entity to which it has
  no access due to security settings.
</constant>

<constant name = "not found" value = "404" class = "soft error">
  The client attempted to work with a server entity that does not exist.
</constant>

<constant name = "resource locked" value = "405" class = "soft error">
  The client attempted to work with a server entity to which it has
  no access because another client is working with it.
</constant>

<constant name = "frame error" value = "501" class = "hard error">
  The client sent a malformed frame that the server could not decode.
  This strongly implies a programming error in the client.
</constant>

<constant name = "syntax error" value = "502" class = "hard error">
  The client sent a frame that contained illegal values for one or more
  fields.  This strongly implies a programming error in the client.
</constant>

<constant name = "command invalid" value = "503" class = "hard error">
  The client sent an invalid sequence of frames, attempting to perform
  an operation that was considered invalid by the server. This usually
  implies a programming error in the client.
</constant>

<constant name = "channel error" value = "504" class = "hard error">
  The client attempted to work with a channel that had not been
  correctly opened.  This most likely indicates a fault in the client
  layer.
</constant>

<constant name = "resource error" value = "506" class = "hard error">
  The server could not complete the method because it lacked sufficient
  resources. This may be due to the client creating too many of some
  type of entity.
</constant>

<constant name = "not allowed" value = "530" class = "hard error">
  The client tried to work with some entity in a manner that is
  prohibited by the server, due to security settings or by some other
  criteria.
</constant>

<constant name = "not implemented" value = "540" class = "hard error">
  The client tried to use functionality that is not implemented in the
  server.
</constant>

<constant name = "internal error" value = "541" class = "hard error">
  The server could not complete the method because of an internal error.
  The server may require intervention by an operator in order to resume
  normal operations.
</constant>
