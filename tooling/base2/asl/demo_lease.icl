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
    name      = "demo_lease"
    comment   = "Server direct lease class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Implements the Direct Protocol lease for the demo server.
    Creates and resolves leases for sinks (exchanges) and
    feeds (queues).
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>
<inherit class = "icl_init" />

<import class = "demo_server_classes" />

<context>
    demo_server_connection_t
        *connection;                    //  Parent connection
    demo_server_channel_t
        *channel;                       //  Parent channel
    smt_thread_t
        *thread;                        //  Connection thread, if lease used
    icl_shortstr_t
        name;                           //  Field name
    byte
        type;                           //  Field type
    demo_exchange_t
        *sink;                          //  Sink we are using
    demo_queue_t
        *feed;                          //  Feed for the lease
    volatile qbyte
        pending;                        //  Number of pending outgoing messages
</context>

<private name = "header">
static $(selfname)_table_t
    *s_$(selfname)_table;               //  The table of existing items
</private>

<method name = "new">
    <argument name = "name" type = "char *">Sink or feed name</argument>
    <argument name = "type" type = "int">DP_SINK or DP_FEED</argument>
    <argument name = "channel" type = "demo_server_channel_t *">Parent channel</argument>
    <dismiss argument = "key"   value = "self->name">Key is lease name</dismiss>
    <dismiss argument = "table" value = "s_$(selfname)_table">Use global table</dismiss>
    //
    <local>
    apr_time_t
        time_now;
    </local>
    assert (type == DP_SINK || type == DP_FEED);
    self->type = type;
    self->channel = demo_server_channel_link (channel);
    self->connection = demo_server_connection_link (channel->connection);
    time_now = apr_time_now ();

    if (type == DP_SINK) {
        self->sink = demo_exchange_search (name);
        if (self->sink)
            icl_shortstr_fmt (self->name, "S-%08X%08X-%s",
                (qbyte) (time_now >> 32), (qbyte) time_now & 0xFFFFFFFF, self->sink->name);
        else
            self_destroy (&self);
    }
    else
    if (type == DP_FEED) {
        //  Only allow one lease per queue
        self->feed = demo_queue_search (name);
        if (self->feed && !self->feed->lease) {
            self->feed->lease = self;
            icl_shortstr_fmt (self->name, "F-%08X%08X-%s",
                (qbyte) (time_now >> 32), (qbyte) time_now & 0xFFFFFFFF, self->feed->name);
        }
        else
            self_destroy (&self);
    }
</method>

<method name = "destroy">
    demo_server_channel_unlink (&self->channel);
    demo_server_connection_unlink (&self->connection);
    demo_exchange_unlink (&self->sink);
    demo_queue_unlink (&self->feed);
    smt_thread_unlink (&self->thread);
</method>

<method name = "search" return = "self">
    <argument name = "name" type = "char *">Exchange name</argument>
    <declare  name = "self" type = "$(selftype) *">The found object</declare>
    if (!s_$(selfname)_active)
        $(selfname)_initialise ();
    self = $(selfname)_table_search (s_$(selfname)_table, name);
</method>

<method name = "acquire" template = "function">
    <doc>
    Acquires the lease; returns -1 if the lease was already acquired by
    another thread.  The acquiring thread will receive messages when the
    lease is for a feed.
    </doc>
    <argument name = "thread" type = "smt_thread_t *" />
    <argument name = "socket" type = "smt_socket_t *" />
    if (self->thread)
        rc = -1;                        //  Already acquired
    self->thread = smt_thread_link (thread);
</method>

<method name = "sink" template = "function">
    <doc>
    Send a content to a sink.  This function does the application specific
    processing for a sink, which in this instance is an exchange.
    </doc>
    <argument name = "content" type = "demo_content_basic_t *" />
    <argument name = "options" type = "byte">Publish options octet</argument>
    //
    demo_exchange_publish (self->sink, self->channel, content);
</method>

<method name = "initialise">
    s_$(selfname)_table = $(selfname)_table_new ();
</method>

<method name = "terminate">
    $(selfname)_table_destroy (&s_$(selfname)_table);
</method>

<method name = "selftest">
</method>

</class>
