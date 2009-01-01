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
    name      = "amq_lease"
    comment   = "Server direct lease class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Implements the Direct Protocol lease for the OpenAMQ server.
    Creates and resolves leases for sinks (exchanges) and feeds
    (queues).
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>
<inherit class = "icl_init" />

<import class = "amq_server_classes" />

<context>
    amq_server_connection_t
        *connection;                    //  Parent connection
    amq_server_channel_t
        *channel;                       //  Parent channel
    smt_thread_t
        *thread;                        //  Connection thread, if lease used
    icl_shortstr_t
        name;                           //  Field name
    byte
        type;                           //  Field type
    icl_shortstr_t
        connection_id;                  //  ID of main connection
    amq_exchange_t
        *sink;                          //  Sink we are using
    amq_queue_t
        *feed;                          //  Feed for the lease
    volatile qbyte
        pending;                        //  Number of pending outgoing messages
</context>

<private name = "header">
static $(selfname)_table_t
    *s_$(selfname)_table;               //  The table of existing items
</private>

<method name = "new">
    <argument name = "vhost" type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "name" type = "char *">Sink or feed name</argument>
    <argument name = "type" type = "int">DP_SINK or DP_FEED</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Parent channel</argument>
    <dismiss argument = "table" value = "s_$(selfname)_table">Use global table</dismiss>
    <dismiss argument = "key" value = "self->name">Key is lease name</dismiss>
    <local>
    apr_time_t
        time_now;
    </local>
    //
    assert (type == DP_SINK || type == DP_FEED);
    self->type = type;
    self->channel = amq_server_channel_link (channel);
    self->connection = amq_server_connection_link (channel->connection);
    icl_shortstr_cpy (self->connection_id, self->connection->id);

    time_now = apr_time_now ();
    if (type == DP_SINK) {
        if (*name)
            self->sink = amq_exchange_table_search (vhost->exchange_table, name);
        else
            //  Get default exchange for virtual host
            self->sink = amq_exchange_link (vhost->default_exchange);

        if (self->sink)
            icl_shortstr_fmt (self->name, "S-%08X%08X-%s",
                (qbyte) (time_now >> 32), (qbyte) time_now & 0xFFFFFFFF, self->sink->name);
        else
            self_destroy (&self);
    }
    else
    if (type == DP_FEED) {
        //  Only allow one lease per queue
        self->feed = amq_queue_table_search (vhost->queue_table, name);
        if (self->feed && !self->feed->lease) {
            //  Cannot link to lease since that forms circular reference
            self->feed->lease = self;
            icl_shortstr_fmt (self->name, "F-%08X%08X-%s",
                (qbyte) (time_now >> 32), (qbyte) time_now & 0xFFFFFFFF, self->feed->name);
        }
        else
            self_destroy (&self);
    }
</method>

<method name = "destroy">
    if (self->feed) {
        self->feed->lease = NULL;
        amq_queue_unlink (&self->feed);
    }
    amq_server_channel_unlink (&self->channel);
    amq_server_connection_unlink (&self->connection);
    amq_exchange_unlink (&self->sink);
    smt_thread_unlink (&self->thread);
</method>

<method name = "search" return = "self">
    <argument name = "name" type = "char *">Exchange name</argument>
    <declare name = "self" type = "$(selftype) *">The found object</declare>
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
    <argument name = "content" type = "amq_content_basic_t *" />
    <argument name = "options" type = "byte">Publish options octet</argument>
    //
    icl_shortstr_cpy (content->producer_id, self->connection_id);
    //  Options octet is [0][0][0][0][0][0][mandatory][immediate]
    amq_exchange_publish (
        self->sink,
        self->channel,
        content,
        (options >> 1) & 1,             //  Mandatory bit
        options & 1,                    //  Immediate bit
        self->connection->group);
    icl_atomic_inc32 ((volatile qbyte *) &(amq_broker->direct_sunk));
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
