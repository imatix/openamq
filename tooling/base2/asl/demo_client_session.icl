<?xml?>
<!--
    *** GENERATED FROM demo_client_proto.asl BY ASL_GEN USING GSL/4 ***

    
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
    name      = "demo_client_session"
    comment   = "demo_client connection session class"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class provides the session serialisation API.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_init" />

<import class = "demo_client_classes" />
<import class = "demo_constants" />
<import class = "demo_client_connection" />
<import class = "demo_content_basic_list" />

<context>
    demo_client_connection_t
        *connection;                    //  Parent connection object
    smt_method_queue_t
        *method_queue;                  //  Replies from protocol agent
    dbyte
        channel_nbr;                    //  Session channel number
    Bool
        alive;                          //  Is the session alive?
    apr_time_t
        timestamp;                      //  Time session was opened
    volatile qbyte
        flow_stopped;                   //  Is flow of messages from server stopped?
    char
        *error_text;                    //  Last error cause
    char
        *chrono_block;                  //  Accumulated chrono deltas
    ipr_hash_table_t
        *dp_sinks,                      //  DP sink threads
        *dp_feeds;                      //  DP feed threads
    icl_longstr_t *
        channel_id;                     //  unique id of the channel
    Bool
        active;                         //  start/stop content frames
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
    icl_shortstr_t
        lease;                          //  Lease for connection
    icl_shortstr_t
        consumer_tag;                   //  consumer tag
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        routing_key;                    //  Message routing key
    int64_t
        delivery_tag;                   //  server-assigned delivery tag
    qbyte
        message_count;                  //  number of messages pending
    icl_shortstr_t
        queue;                          //  queue name
    icl_longstr_t *
        properties;                     //  queue properties
    //  Handling for basic messages
    demo_content_basic_list_t
        *arrived_basic_list;            //  Basic messages
    demo_content_basic_list_t
        *returned_basic_list;           //  Returned basic messages
    demo_content_basic_list_t
        *chrono_list;                   //  Pending chrono messages
</context>

<method name = "new">
    <doc>
    Create new session on an established connection.

    The connection argument refers to a previously-created connection
    object.
    </doc>
    <argument name = "connection" type = "demo_client_connection_t *" />
    <local>
    int
        rc;
    </local>
    //
    assert (connection);

    self->connection   = connection;
    self->channel_nbr  = ++connection->channel_nbr;
    self->method_queue = smt_method_queue_sync_new ();
    self->error_text   = "no error";
    self->arrived_basic_list = demo_content_basic_list_new ();
    self->returned_basic_list = demo_content_basic_list_new ();
    self->chrono_list = demo_content_basic_list_new ();
    self->dp_sinks = ipr_hash_table_new ();
    self->dp_feeds = ipr_hash_table_new ();

    //  Chrono block is the batch size plus enough for one delta
    //  Thus we dispatch the chrono block when it's GE the batch size
    self->chrono_block = icl_mem_alloc (
        demo_client_config_chrono_batch (demo_client_config)
        + ICL_SHORTSTR_MAX);
    strclr (self->chrono_block);

    if (connection->alive) {
        rc = demo_client_agent_channel_open (
            self->connection->thread,
            self->method_queue,
            &self->alive,
            &self->reply_code,
            self->reply_text,
            self->channel_nbr);

        //  If no immediate error, wait for confirmation that session was OK
        if (!rc) {
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE) == 0) {
                self->alive = TRUE;     //  Session is ready for use
                self->timestamp = apr_time_now ();
            }
        }
    }
    else {
        self->error_text = "connection is closed";
        rc = -1;
    }
    if (!self->alive)
        self_destroy (&self);
</method>

<method name = "destroy">
    <doc>
    Close the session.
    </doc>
    //  Close and flush any Direct Mode connections
    ipr_hash_table_apply (self->dp_sinks, s_close_dp_connection, self);
    ipr_hash_table_destroy (&self->dp_sinks);
    ipr_hash_table_apply (self->dp_feeds, s_close_dp_connection, self);
    ipr_hash_table_destroy (&self->dp_feeds);

    //  Close main connection
    if (self->alive) {
        demo_client_agent_channel_close (self->connection->thread, self->channel_nbr);
        while (self->alive) {
            //  Wait until we get back an CLOSED method, or timeout
            s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE);
        }
    }
    icl_longstr_destroy (&self->channel_id);
    icl_longstr_destroy (&self->properties);
    icl_mem_free (self->chrono_block);
    smt_method_queue_destroy (&self->method_queue);
    demo_content_basic_list_destroy (&self->arrived_basic_list);
    demo_content_basic_list_destroy (&self->returned_basic_list);
    demo_content_basic_list_destroy (&self->chrono_list);
</method>

<method name = "wait" template = "function">
    <doc>
    Waits for any activity on the session.  Returns 0 if content arrived
    or the timeout expired, -1 if the session ended abnormally.  If the timeout
    is zero, waits forever.  If the timeout is -1, does not wait at all, but
    processes pending methods and then returns.
    </doc>
    <argument name = "timeout" type = "int">Timeout, in milliseconds</argument>
    //
    if (!demo_content_basic_list_isempty (self->arrived_basic_list)
    ||  !demo_content_basic_list_isempty (self->returned_basic_list))
        return (0);                    //  Return if there is content to process
    if (self->alive) {
        if (s_wait_for_methods (self, timeout, FALSE, TRUE))
            rc = -1;
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "channel flow" template = "function">
    <doc>
    Enable/Disable Flow From Peer.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "active" type = "Bool">start/stop content frames</argument>
    if (self->alive) {
        rc = demo_client_agent_channel_flow (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            active);                     //  start/stop content frames

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "channel flow-ok" template = "function">
    <doc>
    Confirm A Flow Method.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "active" type = "Bool">current flow setting</argument>
    if (self->alive) {
        rc = demo_client_agent_channel_flow_ok (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            active);                     //  current flow setting

        if (rc == 0) {
            if (s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE))
                rc = -1;
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "exchange declare" template = "function">
    <doc>
    Declare Exchange, Create If Needed.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "exchange" type = "char *">exchange name</argument>
    <argument name = "type" type = "char *">exchange class</argument>
    <argument name = "passive" type = "Bool">do not create exchange</argument>
    <argument name = "durable" type = "Bool">request a durable exchange</argument>
    <argument name = "auto_delete" type = "Bool">auto-delete when unused</argument>
    if (self->alive) {
        rc = demo_client_agent_exchange_declare (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            exchange,                    //  exchange name
            type,                        //  exchange class
            passive,                     //  do not create exchange
            durable,                     //  request a durable exchange
            auto_delete);                //  auto-delete when unused

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "exchange delete" template = "function">
    <doc>
    Delete An Exchange.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "exchange" type = "char *">exchange name</argument>
    <argument name = "if_unused" type = "Bool">delete only if unused</argument>
    if (self->alive) {
        rc = demo_client_agent_exchange_delete (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            exchange,                    //  exchange name
            if_unused);                  //  delete only if unused

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "queue declare" template = "function">
    <doc>
    Declare Queue, Create If Needed.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    <argument name = "passive" type = "Bool">do not create queue</argument>
    <argument name = "durable" type = "Bool">request a durable queue</argument>
    <argument name = "exclusive" type = "Bool">request an exclusive queue</argument>
    <argument name = "auto_delete" type = "Bool">auto-delete queue when unused</argument>
    <local>
        smt_thread_t
        *thread;
    </local>
    if (self->alive) {
        rc = demo_client_agent_queue_declare (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue,                       //  queue name
            passive,                     //  do not create queue
            durable,                     //  request a durable queue
            exclusive,                   //  request an exclusive queue
            auto_delete);                //  auto-delete queue when unused

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
    //  Action handler after calling method
    if (rc == 0) {
    if (self->connection->direct && exclusive) {
        thread = demo_client_session_dp_lookup (self, queue, DP_FEED);
        if (!thread) {
            thread = demo_client_session_dp_new (self, queue, DP_FEED);
            assert (thread);
        }
        smt_thread_unlink (&thread);
    }
    }
</method>

<method name = "queue bind" template = "function">
    <doc>
    Bind Queue To An Exchange.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    <argument name = "exchange" type = "char *">exchange name</argument>
    <argument name = "arguments" type = "icl_longstr_t *">arguments for binding</argument>
    if (self->alive) {
        rc = demo_client_agent_queue_bind (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue,                       //  queue name
            exchange,                    //  exchange name
            arguments);                  //  arguments for binding

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "queue purge" template = "function">
    <doc>
    Purge A Queue.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    if (self->alive) {
        rc = demo_client_agent_queue_purge (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue);                      //  queue name

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "queue delete" template = "function">
    <doc>
    Delete A Queue.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    <argument name = "if_unused" type = "Bool">delete only if unused</argument>
    <argument name = "if_empty" type = "Bool">delete only if empty</argument>
    if (self->alive) {
        rc = demo_client_agent_queue_delete (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue,                       //  queue name
            if_unused,                   //  delete only if unused
            if_empty);                   //  delete only if empty

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
    //  Action handler after calling method
    if (rc == 0) {
    if (self->connection->direct)
        demo_client_session_dp_destroy (self, queue, DP_FEED);
    }
</method>

<method name = "basic consume" template = "function">
    <doc>
    Start A Queue Consumer.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    if (self->alive) {
        rc = demo_client_agent_basic_consume (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue);                      //  queue name

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "basic cancel" template = "function">
    <doc>
    End A Queue Consumer.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "consumer_tag" type = "char *">consumer tag</argument>
    if (self->alive) {
        rc = demo_client_agent_basic_cancel (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            consumer_tag);               //  consumer tag

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "basic publish" template = "function">
    <doc>
    Publish A Message.
    The caller must create a new demo_content_basic object
    for this call; the object is destroyed after being sent.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "content" type = "demo_content_basic_t *">Message content</argument>
    <argument name = "exchange" type = "char *">exchange name</argument>
    <argument name = "routing_key" type = "char *">Message routing key</argument>
    <local>
        smt_thread_t
        *thread;
    </local>
    assert (content);                   //  Content must be supplied
    //  Action handler before calling method
    if (self->connection->direct) {
        thread = demo_client_session_dp_lookup (self, exchange, DP_SINK);
        if (!thread) {
            thread = demo_client_session_dp_new (self, exchange, DP_SINK);
            assert (thread);
        }
        demo_content_basic_set_routing_key (content, exchange, routing_key, NULL);
        demo_client_agent_direct_out (thread, content, 0);
        smt_thread_unlink (&thread);
    }
    else
    if (self->alive) {
        rc = demo_client_agent_basic_publish (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            content,                     //  message content
            exchange,                    //  exchange name
            routing_key);                //  message routing key

        if (rc == 0) {
            if (s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE))
                rc = -1;
        }
        else
            self->error_text = "unable to send method to protocol agent";
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "basic get" template = "function">
    <doc>
    Direct Access To A Queue.
    Returns 0 on success, 1 on failure.
    </doc>
    <argument name = "queue" type = "char *">queue name</argument>
    if (self->alive) {
        rc = demo_client_agent_basic_get (
            self->connection->thread,    //  connection thread
            self->channel_nbr,           //  channel to send on
            queue);                      //  queue name

        if (rc == 0) {
            if (self->alive) {
                //  Wait until the agent has synchronised with us
                if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                    rc = -1;
            }
            else {
                self->error_text = "session is closed";
                rc = -1;
            }
        }
    }
    else {
        self->error_text = "session is closed";
        rc = -1;
    }
</method>

<method name = "push arrived" template = "function">
    <doc>
    Stores an arrived content, and takes appropriate action if the arrived
    queue hits the high water mark.
    </doc>
    <argument name = "content" type = "demo_content_basic_t *">Message content</argument>
    <argument name = "exchange" type = "char *">Value to stamp into content</argument>
    <argument name = "routing key" type = "char *">Value to stamp into content</argument>
    <argument name = "consumer tag" type = "char *">Value to stamp into content</argument>
    <argument name = "delivery tag" type = "int64_t">Value to stamp into content</argument>
    <local>
    size_t
        high_water,                     //  Value of high water setting
        queue_size;                     //  Queue size before we push our content
    char
        *on_overflow;                   //  on_overflow action for direct connections
    </local>
    //
    //  Stamp content with values from method, if any
    if (exchange && *exchange)
        icl_shortstr_cpy (content->exchange, exchange);
    if (routing_key && routing_key)
        icl_shortstr_cpy (content->routing_key, routing_key);
    if (consumer_tag && *consumer_tag)
        icl_shortstr_cpy (content->consumer_tag, consumer_tag);
    content->delivery_tag = delivery_tag;

    //  Send chronometer deltas, if any, to chrono@amq.status exchange
    if (demo_client_config_chrono_enabled (demo_client_config))
        s_process_chrono (self, content);

    //  Check if queue would overflow, take appropriate action
    high_water = demo_client_config_high_water (demo_client_config);
    if (high_water) {                   //  If it's zero, we don't interfere
        queue_size = self_get_basic_arrived_count (self);
        if (queue_size == high_water) {
            if (self->connection->direct) {
                on_overflow = demo_client_config_on_overflow (demo_client_config);
                if (streq (on_overflow, "warn")) {
                    //  Warn that we hit high-water, but only once per tide
                    if (icl_atomic_cas32 (&self->flow_stopped, TRUE, FALSE) == FALSE) {
                        icl_console_print ("W: high-water mark reached, application is too slow");
                        icl_console_print ("I: contact iMatix for help on tuning your application");
                    }
                }
                else
                if (streq (on_overflow, "trim")) {
                    //  Chuck away oldest content on arrived list
                    demo_content_basic_t
                        *trimmed;
                    trimmed = demo_content_basic_list_pop (self->arrived_basic_list);
                    demo_content_basic_unlink (&trimmed);
                }
                else
                if (streq (on_overflow, "drop")) {
                    //  Chuck away new incoming content
                    content = NULL;     //  Just don't store the content
                }
                else
                if (streq (on_overflow, "kill")) {
                    //  To die. In the rain. Alone.
                    icl_system_panic ("demo_client_session",
                        "E: high water reached (%d): on_overflow requested fatal stop", high_water);
                    icl_system_panic ("demo_client_session",
                        "I: contact iMatix for help on tuning your application");
                    exit (1);
                }
            }
            else
            //  If we're in normal AMQP mode, send Channel.Flow Off if needed
            if (icl_atomic_cas32 (&self->flow_stopped, TRUE, FALSE) == FALSE)
                demo_client_session_channel_flow (self, FALSE);
        }
    }
    //  Queue content if we did not discard it
    if (content)
        demo_content_basic_list_queue (self->arrived_basic_list, content);
</method>

<method name = "push returned" template = "function">
    <doc>
    Stores a returned content.  Currently not guarded by any high water marks.
    </doc>
    <argument name = "content" type = "demo_content_basic_t *">Message content</argument>
    <argument name = "exchange" type = "char *">Value to stamp into content</argument>
    <argument name = "routing key" type = "char *">Value to stamp into content</argument>
    <argument name = "consumer tag" type = "char *">Value to stamp into content</argument>
    //
    //  Stamp content with values from method, if any
    if (exchange && *exchange)
        icl_shortstr_cpy (content->exchange, exchange);
    if (routing_key && routing_key)
        icl_shortstr_cpy (content->routing_key, routing_key);
    if (consumer_tag && *consumer_tag)
        icl_shortstr_cpy (content->consumer_tag, consumer_tag);
    content->delivery_tag = 0;

    //  Set the returned flag - is this actually useful?
    content->returned = TRUE;

    //  Queue content
    demo_content_basic_list_queue (self->returned_basic_list, content);
    if (!self->connection->silent)
        icl_console_print ("W: basic message was returned: %d - %s",
            self->reply_code, self->reply_text);
</method>

<method name = "get basic arrived count" template = "function">
    rc = demo_content_basic_list_count (self->arrived_basic_list);
</method>

<method name = "basic arrived" return = "content">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <declare name = "content" type = "demo_content_basic_t *" default = "NULL">Message content</declare>
    //
    //  Get next message off list, if any
    content = demo_content_basic_list_pop (self->arrived_basic_list);

    //  Patch session to properly reflect this content's properties
    if (content) {
        icl_shortstr_cpy (self->exchange, content->exchange);
        icl_shortstr_cpy (self->routing_key, content->routing_key);
        if (*content->consumer_tag)
            icl_shortstr_cpy (self->consumer_tag, content->consumer_tag);
        self->delivery_tag = content->delivery_tag;
    }
    if (demo_client_config_high_water (demo_client_config)
    && self_get_basic_arrived_count (self) == demo_client_config_low_water (demo_client_config)
    && icl_atomic_cas32 (&self->flow_stopped, FALSE, TRUE) == TRUE)
        self_channel_flow (self, TRUE);
</method>

<method name = "get basic returned count" template = "function">
    rc = demo_content_basic_list_count (self->returned_basic_list);
</method>

<method name = "basic returned" return = "content">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <declare name = "content" type = "demo_content_basic_t *" default = "NULL">Message content</declare>
    //
    //  Get next message off list, if any
    content = demo_content_basic_list_pop (self->returned_basic_list);
</method>

<method name = "get alive" return = "alive">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "alive" type = "Bool" />
    //
    alive = self->alive;
</method>

<method name = "get error text" return = "error text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "error text" type = "char *" />
    //
    error_text = self->error_text;
</method>

<method name = "get channel_id" return = "channel_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "channel_id" type = "icl_longstr_t *" />
    //
    channel_id = self->channel_id;
</method>

<method name = "get active" return = "active">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "active" type = "Bool" />
    //
    active = self->active;
</method>

<method name = "get reply_code" return = "reply_code">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_code" type = "int" />
    //
    reply_code = self->reply_code;
</method>

<method name = "get reply_text" return = "reply_text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_text" type = "char *" />
    //
    reply_text = self->reply_text;
</method>

<method name = "get class_id" return = "class_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "class_id" type = "int" />
    //
    class_id = self->class_id;
</method>

<method name = "get method_id" return = "method_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "method_id" type = "int" />
    //
    method_id = self->method_id;
</method>

<method name = "get lease" return = "lease">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "lease" type = "char *" />
    //
    lease = self->lease;
</method>

<method name = "get consumer_tag" return = "consumer_tag">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "consumer_tag" type = "char *" />
    //
    consumer_tag = self->consumer_tag;
</method>

<method name = "get exchange" return = "exchange">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "exchange" type = "char *" />
    //
    exchange = self->exchange;
</method>

<method name = "get routing_key" return = "routing_key">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "routing_key" type = "char *" />
    //
    routing_key = self->routing_key;
</method>

<method name = "get delivery_tag" return = "delivery_tag">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "delivery_tag" type = "int64_t" />
    //
    delivery_tag = self->delivery_tag;
</method>

<method name = "get message_count" return = "message_count">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "message_count" type = "qbyte" />
    //
    message_count = self->message_count;
</method>

<method name = "get queue" return = "queue">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "queue" type = "char *" />
    //
    queue = self->queue;
</method>

<method name = "get properties" return = "properties">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "properties" type = "icl_longstr_t *" />
    //
    properties = self->properties;
</method>

<method name = "dp new" return = "thread">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <argument name = "name"  type = "char *">Resource name</argument>
    <argument name = "type"  type = "int">Resource type</argument>
    <declare name = "thread" type = "smt_thread_t *" default = "NULL" />
    <local>
    int
        rc = 0;
    ipr_hash_t
        *hash;
    </local>
    //
    //  Start a new direct connection
    //  - send Direct.Put or Direct.Get via main connection
    //  - server provides lease via Put-Ok or Get-Ok
    //  - create new direct thread and give it the lease
    //
    assert (type == DP_SINK || type == DP_FEED);
    if (name == NULL)
        name = "";                            //  Blank name is valid

    //  Request lease and wait for response
    if (type == DP_SINK)
        demo_client_agent_direct_put (self->connection->thread, self->channel_nbr, name);
    else
        demo_client_agent_direct_get (self->connection->thread, self->channel_nbr, name);
    rc = s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE);
    assert (!rc);

    //  Create direct connection thread and wait for it to signal ready
    thread = demo_client_agent_direct_thread_new (
        type,
        self->method_queue,
        self->connection->host,
        self->lease,
        self->connection->trace,
        self->connection->timeout);
    assert (thread);
    smt_thread_wake (thread);
    rc = s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE);
    assert (!rc);

    //  Store thread in hash table with extra link, so that caller can
    //  unlink thread properly.
    if (type == DP_SINK)
        hash = ipr_hash_new (self->dp_sinks, name, smt_thread_link (thread));
    else
        hash = ipr_hash_new (self->dp_feeds, name, smt_thread_link (thread));
    ipr_hash_unlink (&hash);
</method>

<method name = "dp lookup" return = "thread">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <argument name = "name"  type = "char *">Resource name</argument>
    <argument name = "type"  type = "int">Resource type</argument>
    <declare name = "thread" type = "smt_thread_t *" default = "NULL" />
    //
    <local>
    ipr_hash_t
        *hash;
    </local>
    assert (type == DP_SINK || type == DP_FEED);
    if (type == DP_SINK)
        hash = ipr_hash_table_search (self->dp_sinks, name);
    else
        hash = ipr_hash_table_search (self->dp_feeds, name);

    if (hash) {
        thread = smt_thread_link ((smt_thread_t *) hash->data);
        ipr_hash_unlink (&hash);
    }
</method>

<method name = "dp destroy" template = "function">
    <argument name = "name"  type = "char *">Resource name</argument>
    <argument name = "type"  type = "int">Resource type</argument>
    <local>
    ipr_hash_t
        *hash;
    </local>
    //
    assert (type == DP_SINK || type == DP_FEED);
    if (type == DP_SINK) {
        hash = ipr_hash_table_search (self->dp_sinks, name);
        if (hash) {
            s_close_dp_connection (hash, self);
            ipr_hash_destroy (&hash);
        }
    }
    else {
        hash = ipr_hash_table_search (self->dp_feeds, name);
        if (hash) {
            s_close_dp_connection (hash, self);
            ipr_hash_destroy (&hash);
        }
    }
</method>

<private name = "header">
static int
    s_wait_for_methods ($(selftype) *self, int timeout, Bool blocking, Bool want_content);
static void
    s_process_chrono ($(selftype) *self, demo_content_basic_t *content);
static void
    s_close_dp_connection (ipr_hash_t *hash, void *argument);
</private>

<private name = "footer">
/*  Waits for methods from the protocol agent.  If the blocking argument
    is true, returns only when receiving a synchronous reply method.  If not,
    returns when any incoming method is received.  Returns 0 if ok, -1 if
    the connection is closed or a blocking wait timed-out.
 */
static int
s_wait_for_methods (
    $(selftype) *self,
    int timeout,
    Bool blocking,
    Bool want_content)
{
    smt_method_t
        *smt_method = NULL;
    demo_client_method_t
        *proto_method;
    int
        rc = 0;                         //  OK by default
    qbyte
        pending;                        //  Size of method queue
    Bool
        got_content = FALSE;            //  Did we get 1+ contents?

    smt_method_queue_t
        *smt_queue = smt_method_queue_link (self->method_queue);
    assert (!smt_queue->zombie);
    FOREVER {
        smt_method_unlink (&smt_method);
        if (timeout >= 0)
            smt_method_queue_wait (smt_queue, timeout);
        smt_method = smt_method_queue_pop (smt_queue);
        pending = smt_queue->count;
        //  If we don't have a method, we timed-out
        if (!smt_method) {
            if (blocking) {
                if (smt_queue->zombie)
                    self->error_text = "No reply - method queue destroyed";
                else
                    self->error_text = "No reply received - timeout";
                rc = -1;
            }
            break;
        }
        //  ALMOST means we should wait for the next method...
        if (smt_method->result == DEMO_CLIENT_REPLY_ALMOST) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method)
                demo_client_method_execute (proto_method, self->connection, self);
        }
        else
        //  INTERN is an internal synchronisation from the agent
        if (smt_method->result == DEMO_CLIENT_REPLY_INTERN)
            break;                      //  We can return to caller
        else
        //  METHOD means the agent is sending us either:
        //    - a sync method, which is a reply to a sync request
        //    - an async method, which we want to process and then continue
        //
        //  We always stop when we get a sync method. If we have an async
        //  method, we stop only if the queue is empty and we're not blocking.
        //  Blocking means we want a sync method within the timeout.
        //
        if (smt_method->result == DEMO_CLIENT_REPLY_METHOD) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method) {
                demo_client_method_execute (proto_method, self->connection, self);
                if (proto_method->content)
                    got_content = TRUE;
                if (proto_method->sync
                || (got_content && want_content && pending == 0))
                    break;              //  Got synchronous method, end
            }
            else
                break;                  //  Synchronisation from agent
        }
        else
        if (smt_method->result == DEMO_CLIENT_REPLY_CLOSED) {
            self->error_text = "session is closed";
            self->alive = FALSE;        //  Session is closed
            rc = -1;                    //  Signal session is closed
            break;                      //  We can return to caller
        }
        else
        //  DIRECT_OK means the direct thread started ok
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_OK)
            break;                      //  We can return to caller
        else
        //  DIRECT_NG means the direct thread did not start
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_NG) {
            self->error_text = "direct connection failed";
            rc = -1;                    //  Signal session is closed
            break;                      //  We can return to caller
        }
        else
        //  DIRECT_MSG means we have a message, process & continue
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_MSG) {
            self_push_arrived (
                self, (demo_content_basic_t *) smt_method->data, NULL, NULL, NULL, 0);
            if (want_content && pending == 0)
                break;                  //  We were waiting for content
        }
        else {
            self->error_text = "Internal error - invalid reply method from protocol agent";
            icl_console_print ("E: invalid reply result %d", smt_method->result);
        }
    }
    smt_method_queue_unlink (&smt_queue);
    smt_method_unlink (&smt_method);
    return (rc);
}

static void
s_process_chrono (
    $(selftype) *self,
    demo_content_basic_t *content)
{
}

static void
s_close_dp_connection (ipr_hash_t *hash, void *argument)
{
    $(selftype)
        *self = argument;
    smt_thread_t
        *thread = (smt_thread_t *) hash->data;

    //  Ensure connection is really closed before returning to caller
    //  This call will also flush any data pending to be written
    demo_client_agent_direct_close (thread);
    s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE);
    smt_thread_unlink (&thread);
}
</private>

<method name = "initialise">
    demo_client_agent_init ();
</method>

<method name = "selftest" />

</class>
