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
    comment = "ASL standard server actions"
    abstract = "1"
    >

<class name = "connection">
  <action name = "start-ok">
    $(basename)_connection_start_ok (connection, method);
  </action>

  <action name = "tune-ok">
    $(basename)_connection_tune_ok (connection, method);
  </action>

  <action name = "open">
    $(basename)_connection_open (connection, method);
  </action>
</class>

</protocol>

