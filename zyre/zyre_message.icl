<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This file is licensed under the GPL as follows:

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
    name      = "zyre_message"
    comment   = "Zyre message entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS message object.
</doc>

<inherit class = "zyre_resource_back" />

<context>
    icl_shortstr_t
        address,                        //  Destination address
        reply_to;                       //  Reply-to property
</context>

<method name = "new">
</method>

<method name = "configure">
</method>

<method name = "get">
</method>

<method name = "put">
</method>

<method name = "delete">
</method>

<method name = "post">
</method>

<method name = "report">
</method>

<method name = "attach">
    <local>
    zyre_resource_t
        *content;
    </local>
    //
    //  Create new content resource as child of message and pass it the
    //  argument, which holds the AMQP content
    content = zyre_message__zyre_resource_new (NULL, message, self->resources, "content", "");
    icl_shortstr_cpy (self->type, context->request->content_type);
    self->bucket = ipr_bucket_link (context->request->content);
    self->length = context->request->content_length;
    zyre_resource_unlink (&content);

</method>

<method name = "selftest" />

</class>
