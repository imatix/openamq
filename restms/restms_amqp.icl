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

<method name = "announce">
    icl_console_print ("I: initializing AMQP/Rest plugin on '%s'", path);
</method>

<method name = "get">
    <action>
    icl_console_print ("AMQP GET: %s::%s", request->path, request->request_arguments);
    </action>
</method>

<method name = "head">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "post">
    <action>
    icl_console_print ("AMQP POST: %s::%s", request->path, request->request_arguments);
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "put">
    <action>
    icl_console_print ("AMQP PUT: %s::%s", request->path, request->request_arguments);
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "delete">
    <action>
    icl_console_print ("AMQP DELETE: %s::%s", request->path, request->request_arguments);
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "move">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "copy">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "selftest" />

</class>

