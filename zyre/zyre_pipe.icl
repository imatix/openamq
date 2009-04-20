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
    ipr_looseref_list_t
        *joins;                         //  Joins for pipe
    zyre_resource_t
        *asynclet;                      //  Asynclet message
</context>

<method name = "new">
    <!-- New method is used by portal, does not accept any arguments -->
    self->joins = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    ipr_looseref_t
        *looseref;
    </local>
    //
    //  Free any references to joins that we still hold (no links)
    while ((looseref = ipr_looseref_list_pop (self->joins))) {
        icl_mem_free (looseref->argument);
        ipr_looseref_destroy (&looseref);
    }
    ipr_looseref_list_destroy (&self->joins);
    zyre_backend_unlink (&self->backend);
</method>

<method name = "configure">
    <local>
    zyre_resource_t
        *join,
        *message;
    </local>
    //
    //  If the context is null, only configure an asynclet
    if (context) {
        icl_shortstr_cpy (self->type, ipr_xml_attr_get (context->xml_item, "type", "topic"));
        icl_shortstr_cpy (self->title, ipr_xml_attr_get (context->xml_item, "title", ""));
        self->backend = zyre_backend_link (backend);
        zyre_backend_request_pipe_create (self->backend, self->type, portal->name);

        //  Create join from pipe to default feed
        join = zyre_join__zyre_resource_new (NULL, portal, table, "join", "");
        zyre_restms__zyre_resource_bind ((zyre_restms_t *) (portal->client_object), join);
        zyre_resource_request_configure (join, NULL, table, self->backend);
        zyre_resource_unlink (&join);
    }
    //  Create message asynclet
    message = zyre_message__zyre_resource_new (NULL, portal, table, "message", "");
    zyre_restms__zyre_resource_bind ((zyre_restms_t *) (portal->client_object), message);
    zyre_resource_request_configure (message, context, table, self->backend);
    self->asynclet = message;
    zyre_resource_unlink (&message);
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_ROOT);
    ipr_tree_open (tree, "pipe");
    ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "name", portal->name);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    zyre_resource_to_document (portal, context, &tree);
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
    }
</method>

<method name = "delete">
    <local>
    zyre_resource_t
        *resource;
    ipr_looseref_t
        *looseref;
    </local>
    //
    while ((looseref = ipr_looseref_list_first (self->joins))) {
        //  Since we don't have a link to the resource, grab one
        resource = zyre_resource_link ((zyre_resource_t *) looseref->object);
        zyre_resource_request_delete (resource, NULL);
        zyre_resource_destroy (&resource);
    }
    zyre_backend_request_pipe_delete (self->backend, portal->name);
</method>

<method name = "post">
    if (zyre_restms_parse_document (context, NULL) == 0) {
        if (streq (ipr_xml_name (context->xml_item), "join")) {
            //  We validate the join here so that we can return the existing
            //  join, or complain about an error, or go ahead and create the
            //  join properly.  It's not sensible to do the validation after
            //  we created the join resource (at configure time).
            char
                *feed_uri = ipr_xml_attr_get (context->xml_item, "feed", ""),
                *address = ipr_xml_attr_get (context->xml_item, "address", "*");
            zyre_resource_t
                *resource;
            icl_shortstr_t
                join_key;

            resource = zyre_resource_parse_uri (context, table, feed_uri);
            if (resource && resource->type == RESTMS_RESOURCE_FEED) {
                //  Check if we already have same join, key is address@feed
                ipr_looseref_t
                    *looseref;

                icl_shortstr_fmt (join_key, "%s@%s", address, resource->path);
                looseref = ipr_looseref_list_first (self->joins);
                while (looseref) {
                    if (streq ((char *) looseref->argument, join_key))
                        break;
                    looseref = ipr_looseref_list_next (&looseref);
                }
                if (looseref) {
                    //  We have an existing join that matches so report that
                    resource = (zyre_resource_t *) looseref->object;
                    http_response_set_header (context->response, "location",
                        "%s%s%s", context->response->root_uri, RESTMS_ROOT, resource->path);
                    zyre_resource_request_get (resource, context);
                }
                else
                if (streq (resource->name, "default"))
                    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
                        "may not create joins on default feed");
                else
                    //  Ok, join is valid and new, so create it
                    zyre_resource_response_child_add (portal, context);
            }
            else
                http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
                    "feed '%s' does not exist", feed_uri);
        }
        else
            http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
                "may only create new join resources here");
    }
</method>

<method name = "report">
    ipr_tree_open (tree, "pipe");
    ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "name", portal->name);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    ipr_tree_leaf (tree, "href", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, portal->path);
    ipr_tree_shut (tree);
</method>

<method name = "attach">
    <local>
    ipr_looseref_t
        *looseref;
    </local>
    //
    looseref = ipr_looseref_queue (self->joins, resource);
    looseref->argument = icl_mem_strdup ((char *) argument);
</method>

<method name = "detach">
    <local>
    ipr_looseref_t
        *looseref;
    </local>
    //
    looseref = ipr_looseref_list_first (self->joins);
    while (looseref) {
        if (streq ((char *) looseref->argument, argument)) {
            icl_mem_free (looseref->argument);
            ipr_looseref_destroy (&looseref);
            break;
        }
        looseref = ipr_looseref_list_next (&looseref);
    }
</method>

<method name = "spec valid" return = "rc">
    <doc>
    Returns TRUE if the specified pipe specification is valid, else
    sends an error reply and returns FALSE.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <declare name = "rc" type = "int" default = "FALSE" />
    <local>
    char
        *type;
    </local>
    //
    type = ipr_xml_attr_get (context->xml_item, "type", "");
    if (strnull (type) || streq (type, "fifo"))
        rc = TRUE;
    else
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "unknown pipe type '%s' specified", type);
</method>

<method name = "selftest" />

</class>
