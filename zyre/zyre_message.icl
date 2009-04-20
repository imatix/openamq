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
        reply_to,                       //  Reply-to property
        feed,                           //  Originating feed
        correlation_id,                 //  AMQP envelope property
        expiration,                     //  AMQP envelope property
        message_id,                     //  AMQP envelope property
        timestamp,                      //  AMQP envelope property
        type,                           //  AMQP envelope property
        user_id,                        //  AMQP envelope property
        app_id,                         //  AMQP envelope property
        sender_id;                      //  AMQP envelope property
    int
        priority,                       //  AMQP envelope property
        delivery_mode;                  //  AMQP envelope property
    asl_field_list_t
        *headers;                       //  Header fields
    Bool
        pending;                        //  Waiting for content?
    http_driver_context_t
        *context;                       //  Context waiting for message
</context>

<method name = "new">
    self->pending = TRUE;               //  No content yet
</method>

<method name = "configure">
    portal->private = FALSE;            //  Discoverable via pipe
</method>

<method name = "destroy">
    asl_field_list_unlink (&self->headers);
    http_driver_context_unlink (&self->context);
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    asl_field_t
        *field;                     //  One field from headers list
    </local>
    //
    //  We either have a front-end context to talk to, or a saved context from
    //  the last GET method and we were still pending... Either will do.
    if (context == NULL) {
        context = self->context;
        if (context) {
            //  We previously did a void reply, so allow a real reply now
            context->replied = FALSE;
            //  If context has been shut (client timeout) then don't use it
            if (context->response == NULL)
                context = NULL;     //  Message will stay on pipe...
        }
    }
    if (self->pending) {
        //  Save the context for a backend arrival call
        http_driver_context_reply_void (context);
        http_driver_context_unlink (&self->context);
        self->context = http_driver_context_link (context);
        if (zyre_config_restms_debug (zyre_config))
            icl_console_print ("R: - starting long poll on asynclet message");
    }
    else
    //  Only respond if we have a valid context
    if (context) {
        tree = ipr_tree_new (RESTMS_ROOT);
        ipr_tree_open (tree, "message");
        ipr_tree_leaf (tree, "feed", self->feed);
        if (*self->address)
            ipr_tree_leaf (tree, "address", self->address);
        if (*self->reply_to)
            ipr_tree_leaf (tree, "reply_to", self->reply_to);
        if (self->delivery_mode)
            ipr_tree_leaf (tree, "delivery_mode", "%d", self->delivery_mode);
        if (self->priority)
            ipr_tree_leaf (tree, "priority", "%d", self->priority);
        if (*self->correlation_id)
            ipr_tree_leaf (tree, "correlation_id", self->correlation_id);
        if (*self->expiration)
            ipr_tree_leaf (tree, "expiration", self->expiration);
        if (*self->message_id)
            ipr_tree_leaf (tree, "message_id", self->message_id);
        if (*self->type)
            ipr_tree_leaf (tree, "type", self->type);
        if (*self->user_id)
            ipr_tree_leaf (tree, "user_id", self->user_id);
        if (*self->app_id)
            ipr_tree_leaf (tree, "app_id", self->app_id);
        if (*self->sender_id)
            ipr_tree_leaf (tree, "sender_id", self->sender_id);
        if (*self->timestamp)
            ipr_tree_leaf (tree, "timestamp", self->timestamp);

        field = asl_field_list_first (self->headers);
        while (field) {
            ipr_tree_open (tree, "header");
            ipr_tree_leaf (tree, "name", field->name);
            ipr_tree_leaf (tree, "value", asl_field_string (field));
            ipr_tree_shut (tree);
            field = asl_field_list_next (&field);
        }
        zyre_resource_to_document (portal, context, &tree);
    }
</method>

<method name = "put">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "may not modify messages");
</method>

<method name = "delete">
    //  OK, allowed to delete messages
</method>

<method name = "post">
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "POST method is not allowed on messages");
</method>

<method name = "report">
    ipr_tree_open (tree, "message");
    ipr_tree_leaf (tree, "href", "%s%s%s",
        context->response->root_uri, RESTMS_ROOT, portal->path);
    if (self->pending)
        ipr_tree_leaf (tree, "async", "1");
    if (*self->message_id)
        ipr_tree_leaf (tree, "message_id", self->message_id);
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
        *content;                       //  Content object
    </local>
    //
    method  = (zyre_peer_method_t *) argument;
    content = (amq_content_basic_t *) method->content;
    //
    //  feed = exchange, address = routing-key
    icl_shortstr_cpy (self->address,        method->payload.basic_deliver.routing_key);
    icl_shortstr_cpy (self->feed,           method->payload.basic_deliver.exchange);
    icl_shortstr_cpy (self->reply_to,       content->reply_to);
    icl_shortstr_cpy (self->correlation_id, content->correlation_id);
    icl_shortstr_cpy (self->expiration,     content->expiration);
    icl_shortstr_cpy (self->message_id,     content->message_id);
    icl_shortstr_cpy (self->type,           content->type);
    icl_shortstr_cpy (self->user_id,        content->user_id);
    icl_shortstr_cpy (self->app_id,         content->app_id);
    icl_shortstr_cpy (self->sender_id,      content->sender_id);
    if (content->timestamp)
        ipr_time_mime ((apr_time_t) content->timestamp, self->timestamp);
    self->priority = content->priority;
    self->delivery_mode = content->delivery_mode;
    self->headers = asl_field_list_new (content->headers);
    self->pending = FALSE;              //  Now we have something
</method>

<method name = "selftest" />

</class>
