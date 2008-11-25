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
    comment   = "RestMS interface class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This module provides functions to work with RestMS resources.
</doc>
<inherit class = "icl_base" />
<import class = "amq_server_classes" />

<method name = "feed create" return = "rc">
    <doc>
    Creates the feed, or asserts it exists as specified.  Returns zero if
    the specified feed name and class were accurate and the feed could be
    created if necessary, -1 if the feed name was illegal or the feed could
    not be created.
    </doc>
    <argument name = "feed name" type = "char *">Feed name</argument>
    <argument name = "class name" type = "char *">Feed class</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_queue_t
        *queue = NULL;
    amq_exchange_t
        *exchange = NULL;
    Bool
        auto_delete;
    int
        exchange_type;
    </local>
    strclr (reply_text);
    queue = amq_queue_table_search (amq_broker->queue_table, feed_name);
    auto_delete = streq (class_name, "service");
    exchange = amq_exchange_table_search (amq_broker->exchange_table, feed_name);
    exchange_type = amq_exchange_type_lookup (class_name);

    if (streq (class_name, "service")
    ||  streq (class_name, "rotator")) {
        if (exchange
        || (queue && queue->auto_delete != auto_delete)) {
            rc = 1;
            icl_shortstr_cpy (reply_text, "Resource class inconsistency");
        }
        else
        if (queue == NULL) {
            if (ipr_str_prefixed (feed_name, "amq.")) {
                rc = 1;
                icl_shortstr_cpy (reply_text, "Illegal feed name");
            }
            else {
                //  Create queue as specified, bind to default exchange
                queue = amq_queue_new (NULL, feed_name, FALSE, FALSE, auto_delete, NULL);
                if (queue)
                    amq_exchange_bind_queue (amq_broker->default_exchange,
                        NULL, queue, queue->name, NULL);
                else {
                    rc = 1;
                    icl_shortstr_cpy (reply_text, "Internal error (queues)");
                }
            }
        }
    }
    else
    if (exchange_type >= 0) {
        if (queue
        || (exchange && exchange->type != exchange_type)) {
            rc = 1;
            icl_shortstr_cpy (reply_text, "Resource class inconsistency");
        }
        else
        if (exchange == NULL) {
            if (ipr_str_prefixed (feed_name, "amq.")) {
                rc = 1;
                icl_shortstr_cpy (reply_text, "Illegal feed name");
            }
            else {
                //  Create exchange as specified
                exchange = amq_exchange_new (exchange_type, feed_name, FALSE, FALSE);
                if (!exchange) {
                    rc = 1;
                    icl_shortstr_cpy (reply_text, "Internal error (exchanges)");
                }
            }
        }
    }
    else {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Invalid feed class");
    }
    amq_queue_unlink (&queue);
    amq_exchange_unlink (&exchange);
</method>

<method name = "feed query" return = "rc">
    <doc>
    Queries a specified feed.  Returns a list of properties if the feed
    exists, and sets the return code to 0, otherwise returns -1.
    </doc>
    <argument name = "feed name" type = "char *">Feed name</argument>
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
    queue = amq_queue_table_search (amq_broker->queue_table, feed_name);
    exchange = amq_exchange_table_search (amq_broker->exchange_table, feed_name);
    if (queue) {
        asl_field_new_string (field_list, "class", queue->auto_delete? "service": "rotator");
    }
    else
    if (exchange) {
        asl_field_new_string (field_list, "class", amq_exchange_type_name (exchange->type));
    }
    else {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Cannot query feed: not found");
    }
    amq_queue_unlink (&queue);
    amq_exchange_unlink (&exchange);
</method>

<method name = "feed query class" return = "rc">
    <doc>
    Queries a specified feed class.  Returns a list of feeds if the feed
    class is valid, and sets the return code to 0, otherwise returns -1.
    </doc>
    <argument name = "class name" type = "char *">Class name</argument>
    <argument name = "feed list" type = "asl_field_list_t *">Feed list</argument>
    <argument name = "reply text" type = "char *">Error message if any</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    amq_exchange_t
        *exchange = NULL;
    amq_queue_t
        *queue = NULL;
    Bool
        auto_delete;
    int
        exchange_type;
    </local>
    strclr (reply_text);
    auto_delete = streq (class_name, "service");
    exchange_type = amq_exchange_type_lookup (class_name);

    if (streq (class_name, "service")
    ||  streq (class_name, "rotator")) {
        queue = amq_queue_global_first (amq_broker->queue_list);
        while (queue) {
            if (queue->auto_delete == auto_delete)
                icl_longstr_cat (feed_list, "%s ", queue->name);
            queue = amq_queue_global_next (&queue);
        }
    }
    else
    if (exchange_type >= 0) {
        exchange = amq_exchange_global_first (amq_broker->exchange_list);
        while (exchange) {
            if (exchange->type = exchange_type)
                icl_longstr_cat (feed_list, "%s ", exchange->name);
            exchange = amq_exchange_global_next (&exchange);
        }
    }
    else {
        rc = 1;
        icl_shortstr_cpy (reply_text, "Invalid feed class");
    }
</method>

<method name = "selftest" />

</class>
