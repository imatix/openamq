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
    name      = "zyre_feed"
    comment   = "Zyre feed entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS feed object.
</doc>

<inherit class = "zyre_resource_back" />

<context>
    Bool
        dynamic;                        //  Dynamic resource?
    icl_shortstr_t
        type,                           //  Feed type
        title,                          //  Title text
        license;                        //  License text
    zyre_backend_t
        *backend;                       //  Our protocol backend
    ipr_looseref_list_t
        *joins;                         //  Joins for feed
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
    //  If the context is null, configure a default feed
    assert (backend);
    if (context) {
        self->dynamic = TRUE;
        self->backend = zyre_backend_link (backend);
        icl_shortstr_cpy (self->type, ipr_xml_attr_get (context->xml_item, "type", "topic"));
        icl_shortstr_cpy (self->title, ipr_xml_attr_get (context->xml_item, "title", ""));
        icl_shortstr_cpy (self->license, ipr_xml_attr_get (context->xml_item, "license", ""));
        zyre_backend_request_feed_create (self->backend, self->type, portal->name);
    }
    else {
        //  Default feed is not visible to backend
        self->dynamic = FALSE;
        icl_shortstr_cpy (self->type, "direct");
        icl_shortstr_cpy (self->title, "Default feed");
    }
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_ROOT);
    ipr_tree_leaf (tree, "xmlns", "http://www.imatix.com/schema/restms");
    ipr_tree_open (tree, "feed");
    ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "name", portal->name);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    if (*self->license)
        ipr_tree_leaf (tree, "license", self->license);
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
    if (!self->dynamic)
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "Not allowed to modify this feed");
    else
    if (context->request->content_length == 0)
        http_driver_context_reply_success (context, HTTP_REPLY_NOCONTENT);
    else
    if (zyre_restms_parse_document (context, "feed") == 0) {
        value = ipr_xml_attr_get (context->xml_item, "title", NULL);
        if (value)
            icl_shortstr_cpy (self->title, value);
        value = ipr_xml_attr_get (context->xml_item, "license", NULL);
        if (value)
            icl_shortstr_cpy (self->license, value);
        http_driver_context_reply_success (context, HTTP_REPLY_OK);
    }
</method>

<method name = "delete">
    //  We can delete configured resources internally, context will be null
    if (self->dynamic || context == NULL) {
        zyre_resource_t
            *resource;
        ipr_looseref_t
            *looseref;

        while ((looseref = ipr_looseref_list_first (self->joins))) {
            //  Since we don't have a link to the resource, grab one
            resource = zyre_resource_link ((zyre_resource_t *) looseref->object);
            zyre_resource_request_delete (resource, context);
            zyre_resource_destroy (&resource);
        }
        if (self->backend)
            zyre_backend_request_feed_delete (self->backend, portal->name);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "Not allowed to delete this feed");
</method>

<method name = "post">
    if (streq (context->request->content_type, "application/restms+xml")) {
        if (http_driver_context_xml_parse (context, RESTMS_ROOT, "message") == 0) {
            //  s_post_message
            amq_content_basic_t
                *content;               //  Content to post

            content = amq_content_basic_new ();
            //  get contents from feed (?)
            //  amq_content_basic_record_body  (content, context->request->content);

            amq_content_basic_set_delivery_mode  (content, atoi (
                ipr_xml_attr_get (context->xml_item, "delivery_mode", "0")));
            amq_content_basic_set_priority       (content, atoi (
                ipr_xml_attr_get (context->xml_item, "priority", "")));
            amq_content_basic_set_correlation_id (content,
                ipr_xml_attr_get (context->xml_item, "correlation_id", ""));
            amq_content_basic_set_reply_to       (content,
                ipr_xml_attr_get (context->xml_item, "reply_to", ""));
            amq_content_basic_set_expiration     (content,
                ipr_xml_attr_get (context->xml_item, "expiration", ""));
            amq_content_basic_set_message_id     (content,
                ipr_xml_attr_get (context->xml_item, "message_id", ""));
            amq_content_basic_set_timestamp      (content, (int64_t) ipr_time_mime_decode (
                ipr_xml_attr_get (context->xml_item, "timestamp", "")));
            amq_content_basic_set_type           (content,
                ipr_xml_attr_get (context->xml_item, "type", ""));
            amq_content_basic_set_user_id        (content,
                ipr_xml_attr_get (context->xml_item, "user_id", ""));
            amq_content_basic_set_app_id         (content,
                ipr_xml_attr_get (context->xml_item, "app_id", ""));
            amq_content_basic_set_sender_id      (content,
                ipr_xml_attr_get (context->xml_item, "sender_id", ""));

            zyre_backend_request_address_post (
                backend,
                ipr_xml_attr_get (context->xml_item, "address", ""),
                streq (portal->name, "default")? "": portal->name,
                content);

            amq_content_basic_unlink (&content);
            http_driver_context_reply_success (context, HTTP_REPLY_OK);
        }
    }
    else {
        //
        icl_console_print ("HAVE CONTENT WILL TRAVEL");
        http_driver_context_reply_error (context, HTTP_REPLY_NOTIMPLEMENTED,
            "Try Again Later, much later");
    }
</method>

<method name = "report">
    ipr_tree_open (tree, "feed");
    ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "name", portal->name);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    if (*self->license)
        ipr_tree_leaf (tree, "license", self->license);
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
    Returns TRUE if the specified feed specification is valid, else
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
    if (strnull (type)
    ||  streq (type, "fanout")
    ||  streq (type, "direct")
    ||  streq (type, "topic")
    ||  streq (type, "headers")
    ||  streq (type, "service")
    ||  streq (type, "rotator")
    ||  streq (type, "system"))
        rc = TRUE;
    else
        http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
            "Invalid feed type '%s' specified", type);
</method>

<method name = "selftest" />

</class>
