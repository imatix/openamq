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
    name      = "zyre_join"
    comment   = "Zyre join entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS join object.  Joins are children of both
pipes and feeds which makes management complex.  We can delete joins in 3
ways: directly, by deleting parent pipe, or by deleting parent feed.  We
use a symmetric model so that the logic works the same whether we delete
via the pipe or the feed.  Pipes and feeds hold a list of joins, which is
updated via attach/detach requests from new/deleting joins.  When a pipe or
feed is deleted, it deletes all its joins.  When a join is deleted it
detaches from its pipe and feed.
</doc>

<inherit class = "zyre_resource_back" />

<context>
    Bool
        dynamic;                        //  Dynamic resource?
    icl_shortstr_t
        address;                        //  Address pattern
    zyre_resource_t
        *pipe,                          //  Pipe resource portal
        *feed;                          //  Feed resource portal
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
    <local>
    icl_shortstr_t
        join_key;
    </local>
    //
    assert (backend);
    portal->private = FALSE;            //  Discoverable via pipe
    self->pipe = portal->parent;
    //  If the context is null, configure a default join
    if (context) {
        self->backend = zyre_backend_link (backend);
        self->dynamic = TRUE;
        self->feed = zyre_resource_parse_uri (context, table,
            ipr_xml_attr_get (context->xml_item, "feed", NULL));
        icl_shortstr_cpy (self->address,
            ipr_xml_attr_get (context->xml_item, "address", "*"));
        zyre_backend_request_join_create (
            self->backend, self->pipe->name, self->feed->name, self->address);
    }
    else {
        //  Default feed is not visible to backend
        self->dynamic = FALSE;
        self->feed = ipr_hash_lookup (table, "/feed/default");
        icl_shortstr_cpy (self->address, self->pipe->name);
    }
    assert (self->feed);

    //  Attach to pipe, key is address@feed
    icl_shortstr_fmt (join_key, "%s@%s", self->address, self->feed->path);
    zyre_resource_request_attach (self->pipe, portal, join_key);

    //  Attach to feed, key is address@pipe
    icl_shortstr_fmt (join_key, "%s@%s", self->address, self->pipe->path);
    zyre_resource_request_attach (self->feed, portal, join_key);
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_ROOT);
    ipr_tree_open (tree, "join");
    ipr_tree_leaf (tree, "address", self->address);
    ipr_tree_leaf (tree, "feed", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, self->feed->path);
    zyre_resource_to_document (portal, context, &tree);
</method>

<method name = "put">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "PUT method is not allowed on this resource");
</method>

<method name = "delete">
    //  We can delete configured resources internally, context will be null
    if (self->dynamic || context == NULL) {
        icl_shortstr_t
            join_key;

        //  Detach from pipe, key is address@feed
        icl_shortstr_fmt (join_key, "%s@%s", self->address, self->feed->path);
        zyre_resource_request_detach (self->pipe, portal, join_key);

        //  Detach from feed, key is address@pipe
        icl_shortstr_fmt (join_key, "%s@%s", self->address, self->pipe->path);
        zyre_resource_request_detach (self->feed, portal, join_key);

        if (self->backend)
            zyre_backend_request_join_delete (
                self->backend, self->pipe->name, self->feed->name, self->address);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "may not delete this join");
</method>

<method name = "post">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "POST method is not allowed on this resource");
</method>

<method name = "report">
    ipr_tree_open (tree, "join");
    ipr_tree_leaf (tree, "address", self->address);
    ipr_tree_leaf (tree, "feed", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, self->feed->path);
    ipr_tree_leaf (tree, "href", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, portal->path);
    ipr_tree_shut (tree);
</method>

<method name = "selftest" />

</class>
