<?xml?>
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
    <option name = "hash_size" value = "65535" />
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
        <field name = "private" label = "Private queue?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->exclusive);</get>
        </field>
        <field name = "client" label = "Client host name">
          <rule name = "show on summary" />
          <rule name = "ip address" />
          <get>icl_shortstr_cpy (field_value, self->connection? self->connection->client_address: "");</get>
        </field>
        <field name = "exchange_type" label = "Exchange type">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, amq_exchange_type_name (self->last_exchange_type));</get>
        </field>
        <field name = "routing_key" label = "Routing key">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->last_routing_key);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->auto_delete);</get>
        </field>
        <field name = "consumers" label = "Number of consumers" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->consumers);</get>
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
        <field name = "dropped" type = "int" label = "Messages dropped">
          <get>icl_shortstr_fmt (field_value, "%d", self->dropped);</get>
        </field>

        <class name = "queue_connection" label = "Queue connections" repeat = "1">
          <local>
            amq_consumer_t
                *consumer;              //  Consumer object reference
          </local>
          <get>
            consumer = amq_consumer_by_queue_first (self->queue_basic->active_consumers);
            while (consumer) {
                icl_shortstr_fmt (field_value, "%d", consumer->mgt_queue_connection->object_id);
                consumer = amq_consumer_by_queue_next (&consumer);
            }
          </get>
        </class>

        <method name = "purge" label = "Purge all queue messages">
          <exec>amq_queue_basic_purge (self->queue_basic);</exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<public>
//  Queue limit actions
#define AMQ_QUEUE_LIMIT_WARN   1        //  Send warning to alert log
#define AMQ_QUEUE_LIMIT_DROP   2        //  Drop the message
#define AMQ_QUEUE_LIMIT_TRIM   3        //  Trim the queue first
#define AMQ_QUEUE_LIMIT_KILL   4        //  Kill the connection

#define AMQ_QUEUE_LIMIT_MAX    10       //  Allow up to 10 limits
</public>

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
        locked,                         //  Queue for exclusive consumer?
        dirty;                          //  Queue has to be dispatched?
    int
        consumers;                      //  Number of consumers
    amq_queue_basic_t
        *queue_basic;                   //  Basic content queue
    int
        last_exchange_type;             //  Last exchange type bound to
    icl_shortstr_t
        last_routing_key;               //  Last routing key
    qbyte
        limits,                         //  Number of limits
        limit_min,                      //  Lowest limit
        limit_value  [AMQ_QUEUE_LIMIT_MAX],
        limit_action [AMQ_QUEUE_LIMIT_MAX];

    //  Statistics
    int64_t
        traffic_in,                     //  Traffic in, in octets
        traffic_out,                    //  Traffic out, in octets
        contents_in,                    //  Contents in, in octets
        contents_out,                   //  Contents out, in octets
        dropped;                        //  Dropped messages
</context>

<method name = "new">
    <argument name = "vhost"       type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "connection"  type = "amq_server_connection_t *">Owner connection</argument>
    <argument name = "name"        type = "char *">Queue name</argument>
    <argument name = "durable"     type = "Bool">Is queue durable?</argument>
    <argument name = "exclusive"   type = "Bool">Is queue exclusive?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused queue?</argument>

    <dismiss argument = "table" value = "vhost->queue_table" />
    <dismiss argument = "key" value = "name" />
    <local>
    amq_broker_t
        *broker = amq_broker;
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
        asl_log_print (amq_broker->debug_log, 
            "Q: create   queue=%s auto_delete=%d", self->name, self->auto_delete);

    s_set_queue_limits (self);
</method>

<method name = "destroy">
    <action>
    amq_server_connection_unlink (&self->connection);
    if (amq_server_config_debug_queue (amq_server_config))
        asl_log_print (amq_broker->debug_log, "Q: destroy  queue=%s", self->name);

    amq_queue_basic_destroy (&self->queue_basic);
    </action>
</method>

<method name = "unbind connection" template = "async function" async = "1">
    <action>
    amq_queue_t
        *queue_ref;                     //  Need a reference to call destroy

    assert (self->connection);
    assert (self->auto_delete);
    if (self->consumers == 0) {
        if (amq_server_config_debug_queue (amq_server_config))
            asl_log_print (amq_broker->debug_log, "Q: auto-del queue=%s", self->name);

        queue_ref = amq_queue_link (self);
        amq_vhost_unbind_queue (self->vhost, queue_ref);
        //  Ask broker to ask connections to drop link to queue
        if (self->exclusive)
            amq_broker_unbind_queue (amq_broker, queue_ref);

        amq_queue_unlink (&queue_ref);
    }
    </action>
</method>

<method name = "publish" template = "async function" async = "1">
    <doc>
    Publish message content onto queue.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method"  type = "amq_server_method_t *">Publish method</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    method = amq_server_method_link (method);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_server_method_unlink (&method);
    </release>
    //
    <action>
    if (self->enabled) {
        if (method->class_id == AMQ_SERVER_BASIC)
            amq_queue_basic_publish (
                self->queue_basic,
                channel,
                method->content,
                method->payload.basic_publish.immediate);
    }
    else
        amq_server_channel_error (channel, ASL_ACCESS_REFUSED, "Queue is disabled");
    </action>
</method>

<method name = "get" template = "async function" async = "1">
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
    if (class_id == AMQ_SERVER_BASIC)
        amq_queue_basic_get (self->queue_basic, channel);
    else
        asl_log_print (amq_broker->alert_log, "E: illegal content class (%d)", class_id);
    </action>
</method>

<method name = "consume" template = "async function" async = "1">
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
        if (self->consumers == 0)
            self->locked = TRUE;        //  Grant exclusive access
        else
            error = "Exclusive access to queue not possible";
    }
    else
    if (self->locked)
        error = "Queue is being used exclusively by another consumer";

    if (error) {
        if (channel) {
            amq_server_channel_error (channel, ASL_ACCESS_REFUSED, error);
            amq_server_channel_cancel (channel, consumer->tag, FALSE, TRUE);
        }
    }
    else {
        if (consumer->class_id == AMQ_SERVER_BASIC) {
            amq_queue_basic_consume (self->queue_basic, consumer, active);
            if (connection && !nowait)
                amq_server_agent_basic_consume_ok (
                    connection->thread, channel->number, consumer->tag);
        }
        self->consumers++;
        amq_queue_dispatch (self);
    }
    amq_server_connection_unlink (&connection);
    amq_server_channel_unlink (&channel);
    </action>
</method>

<method name = "cancel" template = "async function" async = "1" on_shutdown = "1">
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
        
    if (consumer->class_id == AMQ_SERVER_BASIC) {
        if (notify) {
            channel = amq_server_channel_link (consumer->channel);
            if (channel) {
                connection = amq_server_connection_link (channel->connection);
                if (connection && !nowait) {
                    amq_server_agent_basic_cancel_ok (
                        connection->thread, channel->number, consumer->tag);
                    amq_server_connection_unlink (&connection);
                }
                amq_server_channel_unlink (&channel);
            }
        }
        amq_queue_basic_cancel (self->queue_basic, consumer);
    }
    self->locked = FALSE;
    self->consumers--;
    if (self->auto_delete && self->consumers == 0) {
        int
            timeout = amq_server_config_queue_timeout (amq_server_config)
                    * 1000 * 1000;
        if (timeout == 0)
            timeout = 1;                //  Send the event very rapidly
        smt_timer_request_delay (self->thread, timeout, auto_delete_event);
    }
    </action>
</method>

<event name = "auto_delete">
    <action>
    //  If we're still at zero consumers, self-destruct
    if (self->consumers == 0) {
        if (amq_server_config_debug_queue (amq_server_config))
            asl_log_print (amq_broker->debug_log, "Q: auto-del queue=%s", self->name);

        amq_queue_self_destruct (self);
    }
    </action>
</event>

<method name = "self destruct" template = "async function" async = "1">
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

    amq_queue_unlink (&queue_ref);
    </action>
</method>

<method name = "purge" template = "async function" async = "1">
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

<method name = "flow" template = "async function" async = "1">
    <doc>
    Pause or restart consumer.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    <argument name = "active"   type = "Bool">Active consumer?</argument>
    //
    <possess>
    consumer = amq_consumer_link (consumer);
    </possess>
    <release>
    amq_consumer_unlink (&consumer);
    </release>
    <action>
    if (consumer->class_id == AMQ_SERVER_BASIC)
        amq_queue_basic_flow (self->queue_basic, consumer, active);
    </action>
</method>

<method name = "dispatch" template = "async function" async = "1">
    <doc>
    Dispatches all pending messages waiting on the specified message queue.
    </doc>
    //
    <action>
    amq_queue_basic_dispatch (self->queue_basic);
    self->dirty = FALSE;                //  Queue has been dispatched
    </action>
</method>

<method name = "message count" template = "function">
    <doc>
    Return number of messages on queue.
    </doc>
    //
    rc = amq_queue_basic_message_count (self->queue_basic);
</method>

<method name = "consumer count" template = "function">
    <doc>
    Return number of consumers on queue.
    </doc>
    //
    rc = self->consumers;
</method>

<method name = "set last binding" template = "async function" async = "1">
    <doc>
    Sets the last binding information for the queue. We do this via an
    async method to avoid two threads squashing the queue's context at the
    same time. The last binding information is used only by the console.
    </doc>
    <argument name = "exchange type" type = "int"></argument>
    <argument name = "routing key" type = "char *"></argument>
    //
    <possess>
    routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
    icl_mem_free (routing_key);
    </release>
    <action>
    self->last_exchange_type = exchange_type;
    icl_shortstr_cpy (self->last_routing_key, routing_key);
    </action>
</method>

<private name = "header">
static void
    s_set_queue_limits ($(selftype) *self);
</private>

<private name = "footer">
static void
s_set_queue_limits ($(selftype) *self)
{
    ipr_config_t
        *config;                        //  Current server config file
    qbyte
        limit_value,                    //  Specified limit value
        limit_action;                   //  Specified limit action
    char
        *action_text;                   //  Limit action as string

    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/queue_profile", self->exclusive? "private": "shared");
    if (config->located)
        ipr_config_locate (config, "limit", NULL);

    self->limit_min = UINT_MAX;
    while (config->located) {
        action_text = ipr_config_get (config, "name", "(empty)");
        if (streq (action_text, "warn"))
            limit_action = AMQ_QUEUE_LIMIT_WARN;
        else
        if (streq (action_text, "drop"))
            limit_action = AMQ_QUEUE_LIMIT_DROP;
        else
        if (streq (action_text, "trim"))
            limit_action = AMQ_QUEUE_LIMIT_TRIM;
        else
        if (streq (action_text, "kill"))
            limit_action = AMQ_QUEUE_LIMIT_KILL;
        else {
            limit_action = 0;
            asl_log_print (amq_broker->alert_log,
                "E: invalid configured limit action '%s'", action_text);
        }
        limit_value = atol (ipr_config_get (config, "value",  "0"));
        if (limit_value && limit_action) {
            if (self->limits < AMQ_QUEUE_LIMIT_MAX) {
                self->limit_value  [self->limits] = limit_value;
                self->limit_action [self->limits] = limit_action;
                self->limits++;
                //  We track lowest limit so that we don't need to start
                //  testing limits until the queue size has exceeded this.
                if (self->limit_min > limit_value)
                    self->limit_min = limit_value;
            }
            else {
                asl_log_print (amq_broker->alert_log,
                    "E: too many limits for queue profile (%d)", self->limits);
                break;
            }
        }
        else
            asl_log_print (amq_broker->alert_log,
                "W: configured limit value '%s' ignored", action_text);
        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
}
</private>

<method name = "selftest" />

</class>
