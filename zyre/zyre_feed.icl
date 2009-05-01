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
    ipr_tree_open (tree, "feed");
    ipr_tree_leaf (tree, "type", self->type);
    ipr_tree_leaf (tree, "name", portal->name);
    if (*self->title)
        ipr_tree_leaf (tree, "title", self->title);
    if (*self->license)
        ipr_tree_leaf (tree, "license", self->license);
    zyre_resource_to_document (portal, context, &tree);
</method>

<method name = "put">
    <local>
    char
        *value;
    </local>
    //
    if (!self->dynamic)
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "may not modify this feed");
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
            "may not delete this feed");
</method>

<method name = "post">
    if (streq (context->request->content_type, "application/restms+xml")) {
        //  If we got a RestMS document, it must contain message elements
        if (http_driver_context_xml_parse (context, RESTMS_ROOT, "message") == 0) {
            //  Send a message for each XML item
            ipr_xml_t
                *message = ipr_xml_link (context->xml_item);
            while (message) {
                s_send_message (message, context, table, portal->name, backend);
                message = ipr_xml_next_sibling (&message);
            }
        }
    }
    else {
        //  Create a content and attach to feed
        zyre_resource_t
            *content;

        //  Create a new content resource and attach to the feed resource
        content = zyre_content__zyre_resource_new (NULL, portal, table, "content", "");
        zyre_restms__zyre_resource_bind ((zyre_restms_t *) (portal->client_object), content);
        //  Allow the content resource to configure itself from the HTTP context
        zyre_resource_request_configure (content, context, table, backend);
        //  Set Location: header to location of content
        http_response_set_header (context->response, "location",
            "%s%s%s", context->response->root_uri, RESTMS_ROOT, content->path);
        //  Drop our link to it, it's now a child of the feed
        zyre_resource_unlink (&content);
        //  Reply with 201 Created, and no content body
        http_driver_context_reply_success (context, HTTP_REPLY_CREATED);
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
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "unknown feed type '%s' specified", type);
</method>

<private name = "header">
static void
s_send_message (ipr_xml_t *message, http_driver_context_t *context,
    ipr_hash_table_t *table, char *feed_name, zyre_backend_t *backend);
</private>

<private name = "footer">
//  XML item is message
//  Table lets us find the content resource by href
//  Backend is where we send the message to
static void
s_send_message (
    ipr_xml_t *message,
    http_driver_context_t *context,
    ipr_hash_table_t *table,
    char *feed_name,
    zyre_backend_t *backend)
{
    amq_content_basic_t
        *content;                       //  Content to post
    ipr_xml_t
        *element;                       //  content element of message
    char
        *address;                       //  Address to use for routing
    asl_field_list_t
        *headers_list;                  //  Message headers as list
    icl_longstr_t
        *headers_table;                 //  Message headers as table

    //  Grab a new content to work with
    content = amq_content_basic_new ();

    //  Parse content element if any
    element = ipr_xml_find_item (message, "content");
    if (element) {
        char
            *uri = ipr_xml_attr_get (element, "href", NULL);
        if (uri) {
            //  Set content from staged content resource
            zyre_resource_t
                *resource;              //  Resource portal for content
            resource = zyre_resource_parse_uri (context, table, uri);
            if (resource) {
                //  Reach through the portal to grab the content properties
                //  Kind of ugly but it's the simplest way to get these
                amq_content_basic_record_body (content,
                    ((zyre_content_t *) (resource->server_object))->bucket);
                amq_content_basic_set_content_type (content,
                    ((zyre_content_t *) (resource->server_object))->type);

                //  Destroy the content resource now
                resource = zyre_resource_link (resource);
                zyre_resource_detach_from_parent (resource);
                zyre_resource_destroy (&resource);
            }
            else
                http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
                    "content resource does not exist");
        }
        else {
            //  Set content from embedded element value
            char
                *value = ipr_xml_text (element),
                *type = ipr_xml_attr_get (element, "type", NULL),
                *encoding = ipr_xml_attr_get (element, "encoding", NULL);
            size_t
                value_len = strlen (value);

            if (encoding) {
                if (streq (encoding, "base64")) {
                    char
                        *encoded,
                        *decoded;

                    //  Trim white space before and after encoded
                    encoded = ipr_str_crop (ipr_str_skip (value));
                    //  Decode the base64 value and switch to binary version
                    value_len = apr_base64_decode_len (encoded);
                    decoded = icl_mem_alloc (value_len);
                    apr_base64_decode (decoded, encoded);
                    icl_mem_free (value);
                    value = decoded;
                    value_len--;        //  APR counts size + 1
                }
                else
                    amq_content_basic_set_content_encoding (content, encoding);
            }
            amq_content_basic_set_body (content, value, value_len, icl_mem_free);
            if (type)
                amq_content_basic_set_content_type (content, type);
        }
        ipr_xml_unlink (&element);
    }
    //  Set AMQP properties on the message
    amq_content_basic_set_delivery_mode  (content, atoi (
        ipr_xml_attr_get (message, "delivery_mode", "0")));
    amq_content_basic_set_priority       (content, atoi (
        ipr_xml_attr_get (message, "priority", "")));
    amq_content_basic_set_correlation_id (content,
        ipr_xml_attr_get (message, "correlation_id", ""));
    amq_content_basic_set_reply_to       (content,
        ipr_xml_attr_get (message, "reply_to", ""));
    amq_content_basic_set_expiration     (content,
        ipr_xml_attr_get (message, "expiration", ""));
    amq_content_basic_set_message_id     (content,
        ipr_xml_attr_get (message, "message_id", ""));
    amq_content_basic_set_timestamp      (content, (int64_t) ipr_time_mime_decode (
        ipr_xml_attr_get (message, "timestamp", "")));
    amq_content_basic_set_type           (content,
        ipr_xml_attr_get (message, "type", ""));
    amq_content_basic_set_user_id        (content,
        ipr_xml_attr_get (message, "user_id", ""));
    amq_content_basic_set_app_id         (content,
        ipr_xml_attr_get (message, "app_id", ""));
    amq_content_basic_set_sender_id      (content,
        ipr_xml_attr_get (message, "sender_id", ""));

    //  Process message headers
    headers_list = asl_field_list_new (NULL);
    element = ipr_xml_find_item (message, "header");
    while (element) {
        char *name = ipr_xml_attr_get (element, "name", NULL);
        char *value = ipr_xml_attr_get (element, "value", NULL);
        if (name && value)
            asl_field_new_string (headers_list, name, value);
        element = ipr_xml_next_item (&element);
    }
    //  Save headers field table into content
    headers_table = asl_field_list_flatten (headers_list);
    amq_content_basic_set_headers (content, headers_table);
    asl_field_list_unlink (&headers_list);
    icl_longstr_destroy (&headers_table);

    address = ipr_xml_attr_get (message, "address", "");
    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: - publish message to feed '%s@%s' (%d bytes)",
            address, feed_name, (int) content->body_size);

    //  Send it off to the backend
    zyre_backend_request_address_post (
        backend,
        address,
        //  Map the default feed to AMQP's "" exchange
        streq (feed_name, "default")? "": feed_name,
        content);

    //  And drop our reference, we're done here
    amq_content_basic_unlink (&content);
}
</private>

<method name = "selftest" />

</class>
