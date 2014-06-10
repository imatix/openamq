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
    name      = "demo_queue_basic"
    comment   = "Basic content queue manager"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the basic content queue manager.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "demo_server_classes" />
<import class = "demo_content_basic_list" />

<context>
    demo_content_basic_list_t
        *content_list;                  //  List of message contents           
</context>

<method name = "new">
    self->content_list = demo_content_basic_list_new ();
</method>

<method name = "destroy">
    demo_content_basic_list_destroy (&self->content_list);
</method>

<method name = "publish" template = "function">
    <doc>
    Accept message content onto queue.
    </doc>
    <argument name = "content" type = "demo_content_basic_t *">Message content</argument>
    demo_content_basic_list_queue (self->content_list, content);
</method>

<method name = "get" template = "function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel" type = "demo_server_channel_t *">The channel in question</argument>
    <local>
    demo_content_basic_t
        *content;                       //  Content object reference
    </local>
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
</method>

<method name = "selftest" />

</class>
