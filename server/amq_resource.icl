<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "amq_resource"
    comment   = "AMQ/Rest module"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This module provides functions to do AMQP/Rest discovery and management 
    in the server.
</doc>
<inherit class = "icl_base" />
<import class = "amq_server_classes" />

<method name = "resolve" return = "rc">
    <doc>
    Resolves a path into a sink name, sink type, and selector.  Returns zero if
    the path could be resolved, else -1 and an error message.  On error, if the
    sink name is empty, the resource does not exist.  If the sink name is filled,
    the resource exists but may not be accessed by applications.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "path" type = "char *">Path to resolve</argument>
    <argument name = "sink name" type = "char *">Resolved sink name</argument>
    <argument name = "sink type" type = "char *">Resolved sink type</argument>
    <argument name = "selector" type = "char *">Resolved selector</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_exchange_t
        *exchange = NULL;
    amq_queue_t
        *queue = NULL;
    int
        start = 0;                      //  Index to start looking for slash
    char
        *slash;                         //  Slash found in path
    </local>
    strclr (sink_name);
    strclr (sink_type);
    strclr (selector);
    strclr (reply_text);

    //  Resolve path from left to right
    while (!exchange && !queue) {
        icl_shortstr_cpy (sink_name, path);
        slash = strchr (sink_name + start, '/');
        if (slash) {
            *slash = 0;
            icl_shortstr_cpy (selector, slash + 1);
            start = slash - sink_name + 1;
        }
        else
            strclr (selector);
        exchange = amq_exchange_table_search (vhost->exchange_table, sink_name);
        queue = amq_queue_table_search (vhost->queue_table, sink_name);
        if (strnull (selector))
            break;
    }
    if (exchange) {
        if (exchange->internal) {
            rc = 1;
            icl_shortstr_cpy (reply_text, "Cannot query resource: internal exchange");
        }
        else
            icl_shortstr_cpy (sink_type, amq_exchange_type_name (exchange->type));
    }
    else
    if (queue) {
        if (queue->exclusive) {
            rc = 1;
            icl_shortstr_cpy (reply_text, "Cannot query resource: exclusive queue");
        }
        else
        if (queue->auto_delete)
            icl_shortstr_cpy (sink_type, "service");
        else
            icl_shortstr_cpy (sink_type, "rotator");
    }
    else {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Cannot query resource: not found");
        strclr (sink_name);
    }
    amq_queue_unlink (&queue);
    amq_exchange_unlink (&exchange);
</method>

<method name = "put" return = "rc">
    <doc>
    Creates the resource, or asserts it exists as specified.  Returns zero if
    the specified path and type were accurate and the resource could be created
    if necessary, -1 if the path was illegal or the resource could not be created.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "path" type = "char *">Path to resource</argument>
    <argument name = "new type" type = "char *">Type of sink to create</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_exchange_t
        *exchange = NULL;
    amq_queue_t
        *queue = NULL;
    icl_shortstr_t
        sink_name,                      
        sink_type,  
        selector;
    int
        exchange_type;
    </local>
    strclr (reply_text);
    rc = amq_resource_resolve (vhost, path, sink_name, sink_type, selector, reply_text);
    if (rc) {
        //  If resource does not exist, create it
        if (strnull (sink_name)) {
            rc = 0;
            exchange_type = amq_exchange_type_lookup (new_type);
            if (ipr_str_prefixed (path, "amq.")) {
                rc = 1;
                icl_shortstr_cpy (reply_text, "Cannot create resource: illegal name");
            }
            else
            if (streq (new_type, "service") 
            ||  streq (new_type, "rotator")) {
                queue = amq_queue_new (vhost, NULL, path, FALSE, FALSE, streq (new_type, "service"), NULL);
                if (!queue)
                    queue = amq_queue_table_search (vhost->queue_table, path);
                if (queue)
                    amq_exchange_bind_queue (vhost->default_exchange, NULL, queue, queue->name, NULL);
                else {
                    rc = 1;
                    icl_shortstr_cpy (reply_text, "Cannot create resource: internal error (queues)");
                }
            }
            else
            if (exchange_type >= 0) {
                exchange = amq_exchange_new (vhost, exchange_type, path, FALSE, FALSE);
                if (!exchange)
                    exchange = amq_exchange_table_search (vhost->exchange_table, path);
                if (!exchange) {
                    rc = 1;
                    icl_shortstr_cpy (reply_text, "Cannot create resource: internal error (exchanges)");
                }
            } 
            else {
                rc = 1;
                icl_shortstr_cpy (reply_text, "Cannot create resource: invalid resource type");
            }
        }
    }
    else
    if (strused (selector)) {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Cannot create resource: existing sink name overlap");
    }
    else
    if (strneq (sink_type, new_type)) {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Cannot create resource: already exists with different type");
    }
    amq_queue_unlink (&queue);
    amq_exchange_unlink (&exchange);
</method>

<method name = "get" return = "rc">
    <doc>
    Queries the sink specified by the path.  Returns zero if the sink exists, 
    -1 if the path was illegal or the resource could not be created.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "path" type = "char *">Path to sink</argument>
    <argument name = "field list" type = "asl_field_list_t *">Properties list</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_exchange_t
        *exchange = NULL;
    amq_queue_t
        *queue = NULL;
    </local>
    //
    strclr (reply_text);
    queue = amq_queue_table_search (vhost->queue_table, path);
    exchange = amq_exchange_table_search (vhost->exchange_table, path);
    if (queue) {
        asl_field_new_string (field_list, "type", queue->auto_delete? "service": "rotator");
    }
    else
    if (exchange) {
        asl_field_new_string (field_list, "type", amq_exchange_type_name (exchange->type));
    }
    else {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Cannot query sink: not found");
    }
    amq_queue_unlink (&queue);
    amq_exchange_unlink (&exchange);
</method>

<method name = "delete" return = "rc">
    <doc>
    Deletes the resource, or asserts it no longer exists.  Returns zero if the 
    specified path was accurate and the resource could be deleted if necessary, 
    -1 if the path was illegal or the resource could not be deleted.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "path" type = "char *">Path to resource</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_exchange_t
        *exchange = NULL;
    amq_queue_t
        *queue = NULL;
    icl_shortstr_t
        sink_name,                      
        sink_type,  
        selector;
    </local>
    strclr (reply_text);
    rc = amq_resource_resolve (vhost, path, sink_name, sink_type, selector, reply_text);
    if (rc) {
        if (strnull (sink_name))
            rc = 0;                     //  It's not there, great
        //  Return any other error (internal exchange, exclusive queue,...)
    }
    else {
        //  Sink exists, delete it
        queue = amq_queue_table_search (vhost->queue_table, sink_name);
        exchange = amq_exchange_table_search (vhost->exchange_table, sink_name);
        if (ipr_str_prefixed (path, "amq.")) {
            rc = 1;
            icl_shortstr_cpy (reply_text, "Cannot delete resource: system resource");
        }
        else
        if (queue) {
            amq_vhost_delete_queue (vhost, queue);
            amq_queue_destroy (&queue);
        }
        else
        if (exchange)
            amq_exchange_destroy (&exchange);

        amq_queue_unlink (&queue);
        amq_exchange_unlink (&exchange);
    }
</method>

<method name = "selftest" />

</class>

