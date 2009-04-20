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
    name      = "zyre_content"
    comment   = "Zyre content entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS content object.
</doc>

<inherit class = "zyre_resource_back" />

<context>
    icl_shortstr_t
        type;                           //  MIME type
    size_t
        length;                         //  Content length
    ipr_bucket_t
        *bucket;                        //  Uploaded bucket
    amq_content_basic_t
        *content;                       //  AMQP content
</context>

<method name = "new">
    <!-- New method is used by portal, does not accept any arguments -->
</method>

<method name = "destroy">
    ipr_bucket_unlink (&self->bucket);
    amq_content_basic_unlink (&self->content);
</method>

<method name = "configure">
    <doc>
    We process a content uploaded via the HTTP connection.
    </doc>
    portal->private = TRUE;             //  Not discoverable via feed
    icl_shortstr_cpy (self->type, context->request->content_type);
    self->bucket = ipr_bucket_link (context->request->content);
    self->length = context->request->content_length;
</method>

<method name = "get">
    if (self->bucket)
        http_response_set_from_bucket (context->response, self->bucket, self->type);
    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: - content length: %d", context->response->content_length);
</method>

<method name = "put">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "may not modify contents");
</method>

<method name = "delete">
    //  OK, allowed to delete contents
</method>

<method name = "post">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "POST method is not allowed on contents");
</method>

<method name = "report">
    ipr_tree_open (tree, "content");
    if (*self->type)
        ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "length", "%ld", self->length);
    ipr_tree_leaf (tree, "href", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, portal->path);
    ipr_tree_shut (tree);
</method>

<method name = "attach">
    <doc>
    We process an AMQP content coming in via the back-end.
    </doc>
    <local>
    zyre_peer_method_t
        *method;                        //  Peer method object
    amq_content_basic_t
        *content;
    asl_reader_t
        reader;
    </local>
    //
    method = (zyre_peer_method_t *) argument;
    content = (amq_content_basic_t *) method->content;
    amq_content_basic_set_reader (content, &reader, IPR_BUCKET_MAX_SIZE);
    icl_shortstr_cpy (self->type, content->content_type);
    self->content = amq_content_basic_link (content);
    self->bucket = amq_content_basic_replay_body (content, &reader);
    self->length = (size_t) content->body_size;
    portal->private = FALSE;            //  Discoverable via messages
</method>

<method name = "selftest" />

</class>
