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
    name    = "http_driver_cgi"
    comment = "HTTP CGI driver module"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a driver module to handle CGI POSTs.
    NB: this is a stub implementation, not complete and not usable.
</doc>

<inherit class = "http_driver_module_back" />

<context>
</context>

<method name = "announce">
    smt_log_print (log, "I: X5 CGI driver loaded at '%s'", portal->path);
</method>

<method name = "destroy">
    <action>
    </action>
</method>

<method name = "get">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "GET method is not allowed on this resource");
    </action>
</method>

<method name = "head">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "HEAD method is not allowed on this resource");
    </action>
</method>

<method name = "post">
    <action>
    icl_console_print ("POSTed content size=%d type=%s location=%s",
        context->request->content_length,
        context->request->content_type,
        context->request->content_file? context->request->content_file: "memory");
    http_driver_context_reply (context);
    </action>
</method>

<method name = "put">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "PUT method is not allowed on this resource");
    </action>
</method>

<method name = "delete">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "DELETE method is not allowed on this resource");
    </action>
</method>

<method name = "move">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "MOVE method is not allowed on this resource");
    </action>
</method>

<method name = "copy">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "COPY method is not allowed on this resource");
    </action>
</method>

</class>

