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
    name      = "demo_queue"
    comment   = "Demo server queue class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    This class implements the server queue class, an asynchronous object
    that acts as a envelope for the separate queue managers for each
    class.
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "demo_server_classes" />
<import class = "demo_content_basic_list" />

<context>
    char
        *name;                          //  Queue name
    Bool
        durable;                        //  Is queue durable?
    Bool
        exclusive;                      //  Is queue exclusive?
    demo_server_channel_t
        *channel;                       //  Owning channel if exclusive
    Bool
        auto_delete;                    //  Auto-delete unused queue?
    demo_content_basic_list_t
        *content_list;                  //  List of message contents
    demo_lease_t
        *lease;                         //  Feed lease, if any
    Bool
        feed_on;                        //  Feed lease enabled/disabled
</context>

<private name = "header">
static $(selfname)_table_t
    *s_$(selfname)_table;               //  The table of existing items
</private>

<method name = "new">
    <argument name = "name" type = "char *">Queue name</argument>
    <argument name = "durable" type = "Bool">Is queue durable?</argument>
    <argument name = "exclusive" type = "Bool">Is queue exclusive?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused queue?</argument>
    <argument name = "channel" type = "demo_server_channel_t *">Server channel</argument>
    <dismiss argument = "key" value = "name">Key is queue name</dismiss>
    <dismiss argument = "table" value = "s_$(selfname)_table">Use global table</dismiss>
    self->name         = icl_mem_strdup (name);
    self->durable      = durable;
    self->exclusive    = exclusive;
    self->auto_delete  = auto_delete;
    self->content_list = demo_content_basic_list_new ();
    if (exclusive)
        self->channel = demo_server_channel_link (channel);
</method>

<method name = "destroy">
    <action>
    demo_server_channel_unlink (&self->channel);
    demo_content_basic_list_destroy (&self->content_list);
    </action>
</method>

<method name = "free">
    //  Free structures that may still be used while object is zombied
    icl_mem_strfree (&self->name);
</method>

<method name = "search" return = "self">
    <argument name = "name" type = "char *">Exchange name</argument>
    <declare  name = "self" type = "$(selftype) *">The found object</declare>
    if (!s_$(selfname)_active)
        $(selfname)_initialise ();
    self = $(selfname)_table_search (s_$(selfname)_table, name);
</method>

<method name = "publish" template = "async function">
    <doc>
    Publish message content onto queue.
    </doc>
    <argument name = "channel"  type = "demo_server_channel_t *" />
    <argument name = "content"  type = "demo_content_basic_t *" />
    <possess>
    content = demo_content_basic_link (content);
    demo_content_basic_chrono_add (content);
    </possess>
    <release>
    demo_content_basic_unlink (&content);
    </release>
    <action>
    demo_content_basic_list_queue (self->content_list, content);
    </action>
</method>

<method name = "get" template = "async function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel" type = "demo_server_channel_t *" />
    <action>
    demo_content_basic_t
        *content;                       //  Content object reference

    //  Get next message off list, if any
    if (!demo_content_basic_list_isempty (self->content_list)) {
        content = demo_content_basic_list_pop (self->content_list);
        demo_server_agent_basic_get_ok (
            channel->connection->thread,
            channel->number,
            content,
            content->exchange,
            content->routing_key,
            demo_content_basic_list_count (self->content_list),
            NULL);
        demo_content_basic_unlink (&content);
    }
    else
        demo_server_agent_basic_get_empty (
            channel->connection->thread,
            channel->number);
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

