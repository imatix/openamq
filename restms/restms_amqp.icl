<?xml?>
<!--
    Copyright (c) 1996-2008 iMatix Corporation

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
    name    = "restms_amqp"
    comment = "Rest/AMQP processing plugin"
    version = "1.0"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a web server plugin to handle Rest/AMQP requests.
</doc>

<inherit class = "http_portal_back" />

<method name = "new">
    icl_console_print ("I: initializing AMQP/Rest plugin...");
</method>

<method name = "process">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "selftest" />

</class>

