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
    name      = "amq_queue"
    comment   = "Polymorphic queue class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class implements the server queue class, an asynchronous object
that acts as a envelope for the separate queue managers for each
class.  This is a lock-free asynchronous class.
</doc>

<option name = "links" value = "1"/>

<inherit class = "smt_object">
    <option name = "possess" value = "1" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "by_vhost" />
</inherit>
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "queue" parent = "broker" label = "Message Queue" >
        <field name = "name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>
        <field name = "pending" label = "Messages pending" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_message_count (self));</get>
        </field>
        <field name = "durable" label = "Durable queue?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->durable);</get>
        </field>
        <field name = "exclusive" label = "Exclusive queue?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->exclusive);</get>
        </field>
        <field name = "client" label = "Client host name">
          <rule name = "show on summary" />
          <rule name = "ip address" />
          <local name = "get">
            amq_server_connection_t
                *connection;
          </local>
          <get>
            connection = amq_server_connection_link (self->connection);
            if (connection) {
                icl_shortstr_cpy (field_value, connection->client_address);
                amq_server_connection_unlink (&connection);
            }
            else
                icl_shortstr_cpy (field_value, "");
          </get>
        </field>
        <field name = "exchange_type" label = "Exchange type">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, amq_exchange_type_name (self->last_exchange_type));</get>
        </field>
        <field name = "routing_key" label = "Routing key">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->last_routing_key);</get>
        </field>
        <field name = "binding_args" label = "Binding arguments">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->last_binding_args);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->auto_delete);</get>
        </field>
        <field name = "consumers" label = "Number of consumers" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_basic_consumer_count (self->queue_basic));</get>
        </field>
        <field name = "messages_in" type = "int" label = "Messages published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->contents_in);</get>
        </field>
        <field name = "messages_out" type = "int" label = "Messages consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->contents_out);</get>
        </field>
        <field name = "megabytes_in" type = "int" label = "Megabytes published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "megabytes_out" type = "int" label = "Megabytes consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->traffic_out / (1024 * 1024)));</get>
        </field>
        <field name = "drop_count" type = "int" label = "Messages dropped">
          <get>icl_shortstr_fmt (field_value, "%d", self->drop_count);</get>
        </field>

        <class name = "queue_connection" label = "Queue connections" repeat = "1">
          <local>
            amq_consumer_t
                *consumer;              //  Consumer object reference
          </local>
          <get>
            consumer = amq_consumer_by_queue_first (self->queue_basic->consumer_list);
            if (consumer)
                icl_shortstr_fmt (field_value, "%d", consumer->mgt_queue_connection->object_id);
          </get>
          <next>
            consumer = amq_consumer_by_queue_next (&consumer);
            if (consumer)
                icl_shortstr_fmt (field_value, "%d", consumer->mgt_queue_connection->object_id);
          </next>
        </class>

        <method name = "purge" label = "Purge all queue messages">
          <exec>amq_queue_basic_purge (self->queue_basic);</exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_broker_t
        *broker;                        //  Parent broker
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    amq_server_connection_t
        *connection;                    //  Parent connection, if any
    icl_shortstr_t
        name;                           //  Queue name
    Bool
        enabled,                        //  Queue is enabled?
        durable,                        //  Is queue durable?
        exclusive,                      //  Is queue exclusive?
        auto_delete,                    //  Auto-delete unused queue?
        locked;                         //  Queue for exclusive consumer?
    amq_queue_basic_t
        *queue_basic;                   //  Basic content queue
    int
        last_exchange_type;             //  Last exchange type bound to
    icl_shortstr_t
        last_routing_key;               //  Last routing key
    icl_shortstr_t
        last_binding_args;              //  Last binding arguments
    qbyte
        warn_limit,                     //  Warn user that we're in trouble
        drop_limit,                     //  Drop new incoming messages
        trim_limit,                     //  Trim old stored messages
        kill_limit;                     //  Kill the connection, it's dead Jim
    amq_lease_t
        *lease;                         //  Feed lease, if any
    Bool
        feed_on,                        //  Feed lease enabled/disabled
        feed_no_local;                  //  No-local applied to feed

    //  Statistics
    int64_t
        traffic_in,                     //  Traffic in, in octets
        traffic_out,                    //  Traffic out, in octets
        contents_in,                    //  Contents in, in octets
        contents_out,                   //  Contents out, in octets
        drop_count;                     //  Dropped messages
    Bool
        warned,                         //  Have we already warned?
        dropped,                        //  Are we already dropping?
        trimmed;                        //  Are we already trimming?
</context>

<method name = "new">
    <argument name = "vhost"       type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "connection"  type = "amq_server_connection_t *">Owner connection</argument>
    <argument name = "name"        type = "char *">Queue name</argument>
    <argument name = "durable"     type = "Bool">Is queue durable?</argument>
    <argument name = "exclusive"   type = "Bool">Is queue exclusive?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused queue?</argument>
    <argument name = "arguments"   type = "icl_longstr_t*">Queue arguments</argument>

    <dismiss argument = "table" value = "vhost->queue_table" />
    <dismiss argument = "key" value = "name" />
    <local>
    amq_broker_t
        *broker = amq_broker;
    asl_field_list_t
        *arg_list;
    asl_field_t
        *profile_field;
    char
        *profile;
    </local>
    //
    self->broker      = broker;
    self->vhost       = vhost;
    self->connection  = amq_server_connection_link (connection);
    self->enabled     = TRUE;
    self->durable     = durable;
    self->exclusive   = exclusive;
    self->auto_delete = auto_delete;
    self->queue_basic = amq_queue_basic_new (self);
    icl_shortstr_cpy (self->name, name);
    amq_queue_by_vhost_queue (self->vhost->queue_list, self);
    if (amq_server_config_debug_queue (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "Q: create   queue=%s auto_delete=%d", self->name, self->auto_delete);
    //  Process arguments
    arg_list = asl_field_list_new (arguments);
    if (!arg_list) {
        //  Catch case where field list is malformed
        amq_server_connection_error (NULL, ASL_SYNTAX_ERROR,
            "Malformed 'arguments' field in queue.declare",
            AMQ_SERVER_QUEUE, AMQ_SERVER_QUEUE_DECLARE);
        self_destroy (&self);
    }
    else {
        //  If unspecified, queue profile defaults to 'private' for exclusive
        //  queues, 'shared' for shared queues
        profile_field = asl_field_list_search (arg_list, "profile");
        if (!profile_field)
            profile = self->exclusive? "private": "shared";
        else
            profile = asl_field_string (profile_field);
        //  Reject unknown queue profiles
        if (s_set_queue_limits (self, profile)) {
            smt_log_print (amq_broker->alert_log,
                "E: client requested unknown queue profile '%s' (%s, %s, %s, %s)",
                profile,
                self->connection->client_address,
                self->connection->client_product,
                self->connection->client_version,
                self->connection->client_instance);
            amq_server_connection_error (connection, ASL_SYNTAX_ERROR,
                "Unknown queue profile requested",
                AMQ_SERVER_QUEUE, AMQ_SERVER_QUEUE_DECLARE);
            if (profile_field)
                asl_field_destroy (&profile_field);
            asl_field_list_destroy (&arg_list);
            self_destroy (&self);
        }
        if (profile_field)
            asl_field_destroy (&profile_field);
        asl_field_list_destroy (&arg_list);
    }
</method>

<method name = "destroy">
    <action>
    amq_server_connection_unlink (&self->connection);
    if (amq_server_config_debug_queue (amq_server_config))
        smt_log_print (amq_broker->debug_log, "Q: destroy  queue=%s", self->name);

    amq_queue_basic_destroy (&self->queue_basic);
    </action>
</method>

<method name = "unbind connection" template = "async function">
    <action>
    amq_queue_t
        *queue_ref;                     //  Need a reference to call destroy

    assert (self->connection);
    assert (self->auto_delete);
    if (amq_queue_basic_consumer_count (self->queue_basic) == 0) {
        if (amq_server_config_debug_queue (amq_server_config))
            smt_log_print (amq_broker->debug_log, "Q: auto-del queue=%s", self->name);

        queue_ref = amq_queue_link (self);
        amq_vhost_unbind_queue (self->vhost, queue_ref);
        //  Ask broker to ask connections to drop link to queue
        if (self->exclusive)
            amq_broker_unbind_queue (amq_broker, queue_ref);

        //  Tell console to drop link back to queue
        amq_console_cancel (self->console, self->object_id);
        amq_queue_unlink (&queue_ref);
    }
    </action>
</method>

<method name = "publish" template = "async function">
    <doc>
    Publish message content onto queue.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "content" type = "amq_content_basic_t *">Content to publish</argument>
    <argument name = "immediate" type = "Bool">Send immediately or return?</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    content = amq_content_basic_link (content);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_content_basic_unlink (&content);
    </release>
    //
    <action>
    if (self->enabled)
        amq_queue_basic_publish (self->queue_basic, channel, content, immediate);
    else
        amq_server_channel_error (channel, ASL_ACCESS_REFUSED, "Queue is disabled",
            AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_PUBLISH);
    </action>
</method>

<method name = "get" template = "async function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel"  type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id" type = "int" >The content class</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    </release>
    <action>
    amq_queue_basic_get (self->queue_basic, channel);
    </action>
</method>

<method name = "consume" template = "async function">
    <doc>
    Attach consumer to appropriate queue consumer list.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <argument name = "active" type = "Bool">Create active consumer?</argument>
    <argument name = "nowait" type = "Bool">No reply method wanted</argument>
    //
    <possess>
    consumer = amq_consumer_link (consumer);
    </possess>
    <release>
    amq_consumer_unlink (&consumer);
    </release>
    <action>
    //
    char
        *error = NULL;                  //  If not null, consumer is invalid
    amq_server_connection_t
        *connection;
    amq_server_channel_t
        *channel;

    //  Validate consumer
    channel = amq_server_channel_link (consumer->channel);
    if (channel)
        connection = amq_server_connection_link (channel->connection);
    else
        connection = NULL;

    if (self->exclusive && self->connection != connection)
        error = "Queue is exclusive to another connection";
    else
    if (consumer->exclusive) {
        if (amq_queue_basic_consumer_count (self->queue_basic) == 0)
            self->locked = TRUE;        //  Grant exclusive access
        else
            error = "Exclusive access to queue not possible";
    }
    else
    if (self->locked)
        error = "Queue is being used exclusively by another consumer";

    if (error) {
        if (channel) {
            amq_server_channel_error (channel, ASL_ACCESS_REFUSED,
                error, AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_CONSUME);
            amq_server_channel_cancel (channel, consumer->tag, FALSE, TRUE);
        }
    }
    else {
        consumer->paused = !active;
        self->feed_on = active;
        self->feed_no_local = consumer->no_local;
        amq_queue_basic_consume (self->queue_basic, consumer);
        if (connection && !nowait)
            amq_server_agent_basic_consume_ok (
                connection->thread, channel->number, consumer->tag);
        amq_queue_dispatch (self);
    }
    amq_server_connection_unlink (&connection);
    amq_server_channel_unlink (&channel);
    </action>
</method>

<method name = "cancel" template = "async function" on_shutdown = "1">
    <doc>
    Cancel consumer, by reference, and alert client application if
    we're doing this in a synchronous exchange of methods.  If the
    cancel is being done at channel close, no notify will be sent
    back to the client.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <argument name = "notify" type = "Bool">Notify client application?</argument>
    <argument name = "nowait" type = "Bool">No reply method wanted</argument>
    //
    <possess>
    consumer = amq_consumer_link (consumer);
    </possess>
    <release>
    amq_consumer_unlink (&consumer);
    </release>
    //
    <action>
    amq_server_connection_t
        *connection;
    amq_server_channel_t
        *channel;

    if (notify && !nowait) {
        channel = amq_server_channel_link (consumer->channel);
        if (channel) {
            connection = amq_server_connection_link (channel->connection);
            if (connection) {
                amq_server_agent_basic_cancel_ok (
                    connection->thread, channel->number, consumer->tag);
                amq_server_connection_unlink (&connection);
            }
            amq_server_channel_unlink (&channel);
        }
    }
    amq_queue_basic_cancel (self->queue_basic, consumer);

    self->feed_on = FALSE;              //  Stop direct feed from queue, if any
    self->locked = FALSE;
    if (self->auto_delete && amq_queue_basic_consumer_count (self->queue_basic) == 0) {
        int
            timeout = amq_server_config_queue_timeout (amq_server_config);
        if (timeout)
            smt_timer_request_delay (self->thread, timeout * 1000 * 1000, auto_delete_event);
        else {
            if (amq_server_config_debug_queue (amq_server_config))
                smt_log_print (amq_broker->debug_log, "Q: auto-del queue=%s", self->name);
            amq_queue_self_destruct (self);
        }
    }
    </action>
</method>

<event name = "auto_delete">
    <action>
    //  If we're still at zero consumers, self-destruct
    if (amq_queue_basic_consumer_count (self->queue_basic) == 0) {
        if (amq_server_config_debug_queue (amq_server_config))
            smt_log_print (amq_broker->debug_log, "Q: auto-del queue=%s", self->name);

        amq_queue_self_destruct (self);
    }
    </action>
</event>

<method name = "self destruct" template = "async function">
    <doc>
    Self-destruct the queue.  This is somewhat delicate because it's
    an async object, and because it's linked to by exchanges/bindings
    and by connections.
    </doc>
    //
    <action>
    amq_queue_t
        *queue_ref;                     //  Need a reference to call destroy

    queue_ref = amq_queue_link (self);
    amq_vhost_unbind_queue (self->vhost, queue_ref);
    //  Ask broker to ask connections to drop link to queue
    if (self->exclusive)
        amq_broker_unbind_queue (amq_broker, queue_ref);

    //  Stop consumers because they link back to queue
    amq_queue_basic_stop (self->queue_basic);
    //  Tell console to drop link back to queue
    amq_console_cancel (self->console, self->object_id);
    amq_queue_unlink (&queue_ref);
    </action>
</method>

<method name = "consumer ack" template = "async function">
    <doc>
    Acknowledge messages on consumer, if any.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer to ack</argument>
    //
    <action>
    amq_queue_basic_consumer_ack (self->queue_basic, consumer);
    </action>
</method>

<method name = "purge" template = "async function">
    <doc>
    Purge all content on a queue.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "nowait" type = "Bool">No reply method wanted</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    </release>
    //
    <action>
    long
        messages = 0;
    amq_server_connection_t
        *connection;

    messages += amq_queue_basic_purge (self->queue_basic);
    if (!nowait) {
        connection = channel?
            amq_server_connection_link (channel->connection): NULL;
        if (connection) {
            amq_server_agent_queue_purge_ok (
                connection->thread, channel->number, messages);
            amq_server_connection_unlink (&connection);
        }
    }
    </action>
</method>

<method name = "dispatch" template = "async function">
    <doc>
    Dispatches all pending messages waiting on the specified message queue.
    </doc>
    //
    <action>
    amq_queue_basic_dispatch (self->queue_basic);
    </action>
</method>

<method name = "message count" template = "function">
    <doc>
    Return number of messages on queue.
    </doc>
    //
    rc = amq_queue_basic_message_count (self->queue_basic) +
         (self->connection ?
         smt_thread_reply_backlog (self->connection->thread) :
         0);
</method>

<method name = "consumer count" template = "function">
    <doc>
    Return number of consumers on queue.
    </doc>
    //
    rc = amq_queue_basic_consumer_count (self->queue_basic);
</method>

<method name = "set last binding" template = "async function">
    <doc>
    Sets the last binding information for the queue. We do this via an
    async method to avoid two threads squashing the queue's context at the
    same time. The last binding information is used only by the console.
    </doc>
    <argument name = "exchange type" type = "int"></argument>
    <argument name = "routing key" type = "char *"></argument>
    <argument name = "arguments" type = "icl_longstr_t*"></argument>
    //
    <possess>
    routing_key = icl_mem_strdup (routing_key);
    arguments = icl_longstr_dup (arguments);
    </possess>
    <release>
    icl_longstr_destroy (&arguments);
    icl_mem_free (routing_key);
    </release>
    <action>
    asl_field_list_t
        *field_list;

    self->last_exchange_type = exchange_type;
    icl_shortstr_cpy (self->last_routing_key, routing_key);

    // Convert binding arguments to human readable string
    field_list = asl_field_list_new (arguments);
    assert (field_list);
    asl_field_list_dump (field_list, self->last_binding_args);
    asl_field_list_destroy (&field_list);
    </action>
</method>

<private name = "header">
static int
    s_set_queue_limits ($(selftype) *self, char *profile);
</private>

<private name = "footer">
static int
s_set_queue_limits ($(selftype) *self, char *profile)
{
    ipr_config_t
        *config;                        //  Current server config file
    qbyte
        limit_value;                    //  Specified limit value
    char
        *action_text;                   //  Limit action as string

    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/queue_profile", profile);

    if (!config->located) {
        ipr_config_destroy (&config);
        return 1;
    }
    if (config->located)
        ipr_config_locate (config, "limit", NULL);

    while (config->located) {
        action_text = ipr_config_get (config, "name", "(empty)");
        limit_value = atol (ipr_config_get (config, "value",  "0"));
        if (streq (action_text, "warn"))
            self->warn_limit = limit_value;
        else
        if (streq (action_text, "drop"))
            self->drop_limit = limit_value;
        else
        if (streq (action_text, "trim"))
            self->trim_limit = limit_value;
        else
        if (streq (action_text, "kill"))
            self->kill_limit = limit_value;
        else
            smt_log_print (amq_broker->alert_log,
                "E: invalid configured limit action '%s'", action_text);

        if (amq_server_config_debug_queue (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "Q: setlimit queue=%s limit=%d action=%s",
                self->name, limit_value, action_text);

        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
    return (0);
}
</private>

<method name = "selftest" />

</class>
