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
    name      = "zyre_pipe"
    comment   = "Zyre pipe entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS pipe object.
</doc>

<inherit class = "zyre_resource_back" />

<context>
    icl_shortstr_t
        type,                           //  Pipe type
        title;                          //  Title if any
    zyre_backend_t
        *backend;                       //  Our protocol backend
</context>

<method name = "new">
  <!-- New method is used by portal, does not accept any arguments -->
</method>

<method name = "destroy">
    zyre_backend_unlink (&self->backend);
</method>

<method name = "configure">
    char
        *type;

    type = ipr_xml_attr_get (context->xml_item, "type", "topic");
    if (zyre_pipe_type_valid (type)) {
        icl_shortstr_cpy (self->type,  type);
        icl_shortstr_cpy (self->title, ipr_xml_attr_get (context->xml_item, "title", ""));
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
            "Invalid pipe type '%s' specified", type);

    self->backend = zyre_backend_link (backend);
    zyre_backend_request_pipe_create (self->backend, type, portal->name);
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_ROOT);
    ipr_tree_leaf (tree, "xmlns", "http://www.imatix.com/schema/restms");
    ipr_tree_open (tree, "pipe");
    ipr_tree_leaf (tree, "name", portal->name);
    ipr_tree_leaf (tree, "type", self->type);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    ipr_tree_shut (tree);
    zyre_resource_report (portal, context, tree);
    ipr_tree_destroy (&tree);
</method>

<method name = "put">
    <local>
    char
        *value;
    </local>
    //
    if (context->request->content_length == 0)
        http_driver_context_reply_success (context, HTTP_REPLY_NOCONTENT);
    else
    if (zyre_restms_parse_document (context, "pipe") == 0) {
        value = ipr_xml_attr_get (context->xml_item, "title", NULL);
        if (value)
            icl_shortstr_cpy (self->title, value);
        http_driver_context_reply_success (context, HTTP_REPLY_OK);
    }
</method>

<method name = "delete">
    zyre_backend_request_pipe_delete (self->backend, portal->name);
</method>

<method name = "post">
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The POST method is not allowed on this resource");
</method>

<method name = "report">
    ipr_tree_open (tree, "pipe");
    ipr_tree_leaf (tree, "name", portal->name);
    ipr_tree_leaf (tree, "type", self->type);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    ipr_tree_leaf (tree, "href", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, portal->path);
    ipr_tree_shut (tree);
</method>

<method name = "type valid" return = "rc">
    <doc>
    Returns TRUE if the specified pipe type is valid, else returns FALSE.
    </doc>
    <argument name = "type" type = "char *" />
    <declare name = "rc" type = "int" />
    if (streq (type, "fifo"))
        rc = TRUE;
    else
        rc = FALSE;
</method>

<method name = "selftest" />

</class>
