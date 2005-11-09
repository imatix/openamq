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

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "65535" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "list" />
</inherit>
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "queue" parent = "vhost">
        <field name = "scope">
          <get>icl_shortstr_cpy (field_value, self->scope);</get>
        </field>
        <field name = "name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>
        <field name = "enabled"     label = "Queue accepts new messages?">
          <get>icl_shortstr_fmt (field_value, "%d", self->enabled);</get>
          <put>self->enabled = atoi (field_value);</put>
        </field>
        <field name = "durable"     label = "Durable queue?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->durable);</get>
        </field>
        <field name = "exclusive"   label = "Exclusive to one connection?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->exclusive);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->auto_delete);</get>
        </field>
        <field name = "consumers"   label = "Number of consumers" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d", self->consumers);</get>
        </field>
        <field name = "messages"    label = "Number of messages" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_message_count (self));</get>
        </field>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    icl_shortstr_t
        scope;                          //  Queue scope
    icl_shortstr_t
        name;                           //  Queue name
    qbyte
        owner_id;                       //  Owner connection
    Bool
        enabled,                        //  Queue is enabled?
        durable,                        //  Is queue durable?
        exclusive,                      //  Is queue exclusive?
        auto_delete,                    //  Auto-delete unused queue?
        locked,                         //  Queue for exclusive consumer
        dirty;                          //  Queue has to be dispatched?
    int
        consumers;                      //  Number of consumers
    amq_queue_jms_t
        *queue_jms;                     //  JMS content queue
    amq_queue_basic_t
        *queue_basic;                   //  Basic content queue
</context>

<method name = "new">
    <argument name = "vhost" type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "owner id" type = "qbyte">Owner context id</argument>
    <argument name = "scope" type = "char *">Queue scope</argument>
    <argument name = "name" type = "char *">Queue name</argument>
    <argument name = "durable" type = "Bool">Is queue durable?</argument>
    <argument name = "exclusive" type = "Bool">Is queue exclusive?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused queue?</argument>

    <dismiss argument = "table" value = "vhost->queue_table" />
    <dismiss argument = "key" value = "fullname">
        Hash key is fullname formatted from queue scope plus name
    </dismiss>

    <local>
    icl_shortstr_t
        fullname;
    </local>
    //
    self->vhost       = vhost;
    self->owner_id    = owner_id;
    self->enabled     = TRUE;
    self->durable     = durable;
    self->exclusive   = exclusive;
    self->auto_delete = auto_delete;
    self->queue_jms   = amq_queue_jms_new (self);
    self->queue_basic = amq_queue_basic_new (self);

    icl_shortstr_cpy (self->scope, scope);
    icl_shortstr_cpy (self->name,  name);

    //  Format fully-scoped queue name
    self_fullname (scope, name, fullname);

    amq_queue_list_queue (self->vhost->queue_list, self);
    if (amq_server_config_trace_queue (amq_server_config))
        icl_console_print ("Q: create   queue=%s|%s", self->scope, self->name);
</method>

<method name = "destroy">
    <action>
    if (amq_server_config_trace_queue (amq_server_config))
        icl_console_print ("Q: destroy  queue=%s|%s", self->scope, self->name);

    amq_queue_jms_destroy   (&self->queue_jms);
    amq_queue_basic_destroy (&self->queue_basic);
    </action>
</method>

<method name = "fullname" return = "fullname">
    <doc>
    Formats a full internal queue name based on the supplied scope
    and queue name.
    </doc>
    <argument name = "scope"    type = "char *">Queue scope</argument>
    <argument name = "name"     type = "char *">Queue name</argument>
    <argument name = "fullname" type = "char *">Result to format</argument>
    //
    icl_shortstr_fmt (fullname, "%s|%s", scope? scope: "", name);
</method>

<method name = "search" return = "self">
    <argument name = "table"  type = "$(selfname)_table_t *">Queue table</argument>
    <argument name = "scope"  type = "char *"               >Queue scope</argument>
    <argument name = "name"   type = "char *"               >Queue name</argument>
    <declare  name = "self" type = "$(selftype) *">The found object</declare>
    <local>
    icl_shortstr_t
        fullname;
    </local>
    //
    self_fullname (scope, name, fullname);
    self = $(selfname)_table_search (table, fullname);
</method>

<method name = "publish" template = "async function" async = "1">
    <doc>
    Publish message content onto queue.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id"  type = "int">The content class</argument>
    <argument name = "content"   type = "void *">Message content</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers?</argument>
    //
    <possess>
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_possess (content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_possess (content);
    </possess>
    <release>
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_destroy ((amq_content_jms_t **) &content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_destroy ((amq_content_basic_t **) &content);
    </release>
    <action>
    if (self->enabled) {
        if (class_id == AMQ_SERVER_JMS)
            amq_queue_jms_publish (self->queue_jms, channel, content, immediate);
        else
        if (class_id == AMQ_SERVER_BASIC)
            amq_queue_basic_publish (self->queue_basic, channel, content, immediate);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_ALLOWED, "Queue is disabled");
    </action>
</method>

<method name = "get" template = "async function" async = "1">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel"  type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id" type = "int" >The content class</argument>
    //
    <action>
    if (class_id == AMQ_SERVER_JMS)
        amq_queue_jms_get (self->queue_jms, channel);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_queue_basic_get (self->queue_basic, channel);
    else
        icl_console_print ("E: illegal content class (%d)", class_id);
    </action>
</method>

<method name = "consume" template = "async function" async = "1">
    <doc>
    Attach consumer to appropriate queue consumer list.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <argument name = "active"   type = "Bool">Create active consumer?</argument>
    //
    <possess>
    amq_consumer_link (consumer);
    </possess>
    <release>
    amq_consumer_unlink (&consumer);
    </release>
    <action>
    //
    char
        *error = NULL;                  //  If not null, consumer is invalid

    //  Validate consumer
    if (self->exclusive && self->owner_id != consumer->channel->connection->context_id)
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
        amq_server_channel_close (consumer->channel, ASL_RESOURCE_ERROR, error);
        amq_server_channel_cancel (consumer->channel, consumer->tag, FALSE);
    }
    else {
        if (consumer->class_id == AMQ_SERVER_JMS) {
            amq_queue_jms_consume (self->queue_jms, consumer, active);
            if (amq_server_channel_alive (consumer->channel))
                amq_server_agent_jms_consume_ok (
                    consumer->channel->connection->thread,
                    (dbyte) consumer->channel->key,
                    consumer->tag);
        }
        else
        if (consumer->class_id == AMQ_SERVER_BASIC) {
            amq_queue_basic_consume (self->queue_basic, consumer, active);
            if (amq_server_channel_alive (consumer->channel))
                amq_server_agent_basic_consume_ok (
                    consumer->channel->connection->thread,
                    (dbyte) consumer->channel->key,
                    consumer->tag);
        }
        self->consumers++;
    }
    </action>
</method>

<method name = "cancel" template = "async function" async = "1">
    <doc>
    Cancel consumer, by reference, and alert client application if
    we're doing this in a synchronous exchange of methods.  If the
    cancel is being done at channel close, no notify will be sent
    back to the client.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *" ref = "1">Consumer reference</argument>
    <argument name = "notify" type = "Bool">Notify client application?</argument>
    //
    <action>
    if (consumer->class_id == AMQ_SERVER_JMS) {
        if (notify && amq_server_channel_alive (consumer->channel))
            amq_server_agent_jms_cancel_ok (
                consumer->channel->connection->thread,
                (dbyte) consumer->channel->key);
        amq_queue_jms_cancel (self->queue_jms, consumer);
    }
    else
    if (consumer->class_id == AMQ_SERVER_BASIC) {
        if (notify && amq_server_channel_alive (consumer->channel))
            amq_server_agent_basic_cancel_ok (
                consumer->channel->connection->thread,
                (dbyte) consumer->channel->key);
        amq_queue_basic_cancel (self->queue_basic, consumer);
    }
    self->locked = FALSE;
    self->consumers--;
    if (self->auto_delete && self->consumers == 0) {
        int timeout
            = amq_server_config_queue_timeout (amq_server_config) * 1000 * 1000;
        if (timeout == 0)
            timeout = 1;
        smt_timer_request_delay (self->thread, timeout, auto_delete_event);
    }
    </action>
</method>

<event name = "auto_delete">
    <action>
    amq_queue_t
        *queue_ref;                     //  Need a reference to call destroy

    //  If we're still at zero consumers, self-destruct
    if (self->consumers == 0) {
        queue_ref = amq_queue_link (self);
        amq_queue_destroy (&queue_ref);
    }
    </action>
</event>

<method name = "purge" template = "async function" async = "1">
    <doc>
    Purge all content on a queue.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <action>
    long
        messages = 0;
        
    messages += amq_queue_jms_purge   (self->queue_jms);
    messages += amq_queue_basic_purge (self->queue_basic);
    amq_server_agent_queue_purge_ok (
        channel->connection->thread,
        (dbyte) channel->key,
        messages);
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
    amq_consumer_link (consumer);
    </possess>
    <release>
    amq_consumer_unlink (&consumer);
    </release>
    <action>
    if (consumer->class_id == AMQ_SERVER_JMS)
        amq_queue_jms_flow (self->queue_jms, consumer, active);
    else
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
    amq_queue_jms_dispatch   (self->queue_jms);
    amq_queue_basic_dispatch (self->queue_basic);
    self->dirty = FALSE;                //  Queue has been dispatched
    </action>
</method>

<method name = "message count" template = "function">
    <doc>
    Return number of messages on queue.
    </doc>
    //
    rc = amq_queue_jms_message_count   (self->queue_jms)
       + amq_queue_basic_message_count (self->queue_basic);
</method>

<method name = "consumer count" template = "function">
    <doc>
    Return number of consumers on queue.
    </doc>
    //
    rc = self->consumers;
</method>

<method name = "pre dispatch" template = "function">
    <doc>
    Flags the queue as "dirty" and moves it to the front of the dispatch
    list so that the virtual host will dispatch it next.
    </doc>
    //
    self->dirty = TRUE;
    amq_queue_list_push (self->vhost->queue_list, self);
</method>

<method name = "selftest" />

</class>
