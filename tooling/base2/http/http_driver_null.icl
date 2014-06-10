<?xml?>
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
    name    = "http_driver_null"
    comment = "HTTP ignore driver module"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a driver module to ignore all methods.  This
    is used for testing stability in the server and clients.
</doc>

<inherit class = "http_driver_module_back" />

<method name = "announce">
    smt_log_print (log, "I: X5 null driver loaded at '%s'", portal->path);
</method>

</class>
