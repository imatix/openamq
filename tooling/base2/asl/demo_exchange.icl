<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name      = "demo_exchange"
    comment   = "Demo server exchange class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    This class implements the ASL demo server exchange class (this is a
    fanout exchange).
</doc>

<import class = "demo_queue_list" />

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str"   />
</inherit>

<import class = "demo_server_classes" />

<context>
    char
        *name;                          //  Exchange name
    Bool
        durable;                        //  Is exchange durable?
    Bool
        auto_delete;                    //  Auto-delete unused exchange?
    demo_queue_list_t
        *queue_list;                    //  List of bound queues
</context>

<private name = "header">
static $(selfname)_table_t
    *s_$(selfname)_table;               //  The table of existing items
</private>

<method name = "new">
    <argument name = "name"        type = "char *">Exchange name</argument>
    <argument name = "durable"     type = "Bool">Is exchange durable?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused exchange?</argument>
    <dismiss argument = "key"   value = "name">Key is exchange name</dismiss>
    <dismiss argument = "table" value = "s_$(selfname)_table">Use global table</dismiss>
    self->name        = icl_mem_strdup (name);
    self->durable     = durable;
    self->auto_delete = auto_delete;
    self->queue_list  = demo_queue_list_new ();
</method>

<method name = "destroy">
    <action>
    icl_mem_strfree (&self->name);
    demo_queue_list_destroy (&self->queue_list);
    </action>
</method>

<method name = "search" return = "self">
    <argument name = "name"    type = "char *">Exchange name</argument>
    <declare  name = "self"    type = "$(selftype) *">The found object</declare>
    //
    if (!s_$(selfname)_active)
        $(selfname)_initialise ();

    self = $(selfname)_table_search (s_$(selfname)_table, name);
</method>

<method name = "bind" template = "async function">
    <argument name = "channel"   type = "demo_server_channel_t *">The channel in question</argument>
    <argument name = "queue"     type = "demo_queue_t *">The queue to bind</argument>
    <argument name = "arguments" type = "icl_longstr_t *">Bind arguments</argument>
    <action>
    demo_queue_list_iter_t *
        iterator;

    iterator = demo_queue_list_first (self->queue_list);
    while (iterator && iterator->item != queue)
        iterator = demo_queue_list_next (&iterator);
    if (!iterator)
        demo_queue_list_queue (self->queue_list, queue);
    demo_server_agent_queue_bind_ok (channel->connection->thread, channel->number);
    </action>
</method>

<method name = "publish" template = "async function">
    <doc>
    Sends the content to all registered queues.
    </doc>
    <argument name = "channel"   type = "demo_server_channel_t *">The channel in question</argument>
    <argument name = "content"   type = "demo_content_basic_t *">The message content</argument>
    <possess>
    content = demo_content_basic_link (content);
    demo_content_basic_chrono_add (content);
    </possess>
    <release>
    demo_content_basic_unlink (&content);
    </release>
    <action>
    demo_queue_list_iter_t *
        iterator;
    demo_queue_t
        *queue;

    iterator = demo_queue_list_first (self->queue_list);
    while (iterator) {
        queue = demo_queue_link (iterator->item);
        if (queue->lease && queue->feed_on && queue->lease->thread) {
            demo_server_agent_direct_out (queue->lease->thread, content, 0);
            icl_atomic_inc32 ((volatile qbyte *) &(queue->lease->pending));
        }
        else
            demo_queue_publish (iterator->item, channel, content);
        demo_queue_unlink (&queue);
        iterator = demo_queue_list_next (&iterator);
    }
    </action>
</method>

<method name = "initialise">
    s_$(selfname)_table = $(selfname)_table_new ();
</method>

<method name = "terminate">
    $(selfname)_table_destroy (&s_$(selfname)_table);
</method>

<method name = "selftest" inherit = "none"/>

</class>
