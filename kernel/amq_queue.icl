<?xml?>
<class
    name      = "amq_queue"
    comment   = "Polymorphic queue class"
    version   = "1.0"
    script    = "smt_object_gen"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
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

<public name = "include">
#include "amq_server_classes.h"
</public>

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
        durable,                        //  Is queue durable?
        private,                        //  Is queue private?
        auto_delete,                    //  Auto-delete unused queue?
        exclusive,                      //  Queue for exclusive consumer
        dirty;                          //  Queue has to be dispatched?
    int
        consumers;                      //  Number of consumers
    amq_queue_jms_t
        *queue_jms;                     //  JMS content queue
    amq_queue_basic_t
        *queue_basic;                   //  Basic content queue
</context>

<method name = "new">
    <argument name = "owner id" type = "qbyte">Owner context id</argument>
    <argument name = "vhost"    type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "scope"    type = "char *">Queue scope</argument>
    <argument name = "name"     type = "char *">Queue name</argument>
    <argument name = "durable"  type = "Bool">Is queue durable?</argument>
    <argument name = "private"  type = "Bool">Is queue private?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused queue?</argument>
    <dismiss argument = "table" value = "vhost->queue_table" />
    <dismiss argument = "key"   value = "fullname">
        Hash key is fullname formatted from queue scope plus name
    </dismiss>
    <local>
    icl_shortstr_t
        fullname;
    </local>
    //
    self->owner_id    = owner_id;
    self->vhost       = vhost;
    self->durable     = durable;
    self->private     = private;
    self->auto_delete = auto_delete;
    self->queue_jms   = amq_queue_jms_new   (self);
    self->queue_basic = amq_queue_basic_new (self);

    icl_shortstr_cpy (self->scope, scope);
    icl_shortstr_cpy (self->name,  name);

    //  Format fully-scoped queue name
    self_fullname (scope, name, fullname);

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
    if (class_id == AMQ_SERVER_JMS)
        amq_queue_jms_publish (self->queue_jms, channel, content, immediate);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_queue_basic_publish (self->queue_basic, channel, content, immediate);
    </action>
</method>

<method name = "browse" template = "async function" async = "1">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel"  type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id" type = "int" >The content class</argument>
    //
    <action>
    if (class_id == AMQ_SERVER_JMS)
        amq_queue_jms_browse (self->queue_jms, channel);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_queue_basic_browse (self->queue_basic, channel);
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
    if (self->private && self->owner_id != consumer->channel->connection->context_id)
        error = "Queue is private for another connection";
    else
    if (consumer->exclusive) {
        if (self->consumers == 0)
            self->exclusive = TRUE;     //  Grant exclusive access
        else
            error = "Exclusive access to queue not possible";
    }
    else 
    if (self->exclusive)
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
    self->exclusive = FALSE;
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

<method name = "pre dispatch" template = "function">
    <doc>
    Flags the queue as "dirty" and moves it to the front of the dispatch
    list so that the virtual host will dispatch it next.
    </doc>
    //
    self->dirty = TRUE;
    amq_queue_list_push (self->vhost->queue_list, self);
</method>

<method name = "selftest">
    <local>
    amq_queue_table_t
        *queue_table;
    amq_queue_t
        *queue;
    amq_vhost_t
        *vhost;
    int
        count;
    </local>
    //
    //  Queues are async objects, so we must fire-up SMT before using them
    smt_os_thread_initialise ();        //  Initialise SMT engine

    //  We need a virtual host in which to store our test queues
    vhost = amq_vhost_new ();
    queue_table = amq_queue_table_new ();

    //  Now let's create and destroy a lot of queues
    for (count = 0; count < 10000; count++) {
        queue = amq_queue_new (0, vhost, "global", "test", 0, 0, 0);
        amq_queue_destroy (&queue);
        smt_os_thread_wait (0);
    }
    //  Release resources
    amq_queue_table_destroy (&queue_table);
    amq_vhost_destroy (&vhost);
</method>

</class>
