<?xml?>
<class
    name      = "amq_consumer"
    comment   = "Consumer class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
The consumer class defines a single consumer for a queue or subscription.
It manages the window (i.e. how many unacknowledged messages can be pending),
and other properties of the consumer.  Consumers are attached to a handle
and to a queue.
</doc>

<inherit class = "ipr_list_item" >
    <option name = "prefix" value = "by_handle"/>
</inherit>
<inherit class = "ipr_list_item" >
    <option name = "prefix" value = "by_queue"/>
</inherit>

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private name = "header">
#include "amq_server_agent.h"           /*  Server agent methods             */
</private>

<context>
    /*  References to parent objects                                         */
    amq_handle_t
        *handle;                        /*  Parent handle                    */
    amq_dest_t
        *dest;                          /*  Parent destination               */
    amq_queue_t
        *queue;                         /*  Parent queue                     */
    amq_subscr_t
        *subscr;                        /*  Subscription, if any             */

    /*  Object properties                                                    */
    int
        prefetch;                       /*  Max prefetch size                */
    int
        window;                         /*  Remaining prefetch window        */
    Bool
        no_local;                       /*  Don't deliver own msgs to self   */
    Bool
        no_ack;                         /*  No ACKs are required             */
    Bool
        dynamic;                        /*  Dynamic queue consumer           */
    Bool
        exclusive;                      /*  Exclusive consumer               */
    int
        index;                          /*  Unique index per vhost           */
</context>

<method name = "new">
    <argument name = "handle"  type = "amq_handle_t *"        >Current handle</argument>
    <argument name = "command" type = "amq_handle_consume_t *">Passed command</argument>
    self->handle    = handle;
    self->prefetch  = command->prefetch? command->prefetch: 1;
    self->window    = self->prefetch;
    self->no_local  = command->no_local;
    self->no_ack    = command->no_ack;
    self->dynamic   = command->dynamic;
    self->exclusive = command->exclusive;

    if (handle->service_type == AMQP_SERVICE_QUEUE)
        s_init_queue_consumer (self, command);
    else {
        assert (handle->service_type == AMQP_SERVICE_TOPIC);
        s_init_topic_consumer (self, command);
    }
    if (self->dest) {
        amq_consumer_by_handle_queue (handle->consumers, self);
        if (handle->paused)
            self_deactivate (self);
        else
            self_activate (self);
    }
    else
        self_destroy (&self);
</method>

<method name = "destroy">
    if (self->queue) {
        amq_consumer_by_queue_unlink (self);
        amq_queue_detach (self->queue, self);
    }
    amq_subscr_destroy (&self->subscr);
</method>

<method name = "window close" template = "function">
    <doc>
    Updates the consumer and destination windowing for a dispatched
    message.
    </doc>
    /*  Dispatched message decrements message windows                        */
    if (self->window > 0) {
        self->queue->window--;
        self->window--;
        if (self->window == 0)
            self_deactivate (self);
    }
</method>

<method name = "window open" template = "function">
    <doc>
    Updates the consumer and destination windowing for an acknowledged
    message.
    </doc>
    /*  Message queue and consumer can accept a new message              */
    if (self->window < self->prefetch) {
        self->window++;
        self->queue->window++;
        if (self->window == 1)
            self_activate (self);       /*  Just activated               */
    }
</method>

<method name = "activate" template = "function">
    <doc>
    Moves the consumer to its queue's active list.
    </doc>
    amq_consumer_by_queue_unlink (self);
    amq_consumer_by_queue_queue  (self->queue->active_consumers, self);
</method>

<method name = "deactivate" template = "function">
    <doc>
    Moves the consumer to its queue's inactive list.
    </doc>
    amq_consumer_by_queue_unlink (self);
    amq_consumer_by_queue_queue  (self->queue->inactive_consumers, self);
</method>

<method name = "cancel" template = "function">
    self_destroy (&self);
</method>

<private name = "header">
static void
    s_init_queue_consumer ($(selftype) *self, amq_handle_consume_t *command);
static void
    s_init_topic_consumer ($(selftype) *self, amq_handle_consume_t *command);
</private>

<private>
/*  For a queue service the consumer references a specific destination.
    We look for this destination in the queue_hash, and if we find it
    we initialise the consumer and then dispatch the destination queue.
 */

static void
s_init_queue_consumer ($(selftype) *self, amq_handle_consume_t *command)
{
    static qbyte
        dyn_count = 0;                  /*  Dynamic destination number       */

    /*  Provide dynamic queue name if necessary                              */
    if (self->dynamic && strnull (command->dest_name))
        ipr_shortstr_fmt (command->dest_name, "dyn-%09ld", ++dyn_count);

    /*  Look for queue destination                                           */
    self->dest = amq_dest_search (self->handle->vhost->queue_hash, command->dest_name);

    /*  Create queue for dynamic consumers if needed                         */
    if (self->dest == NULL && self->dynamic) {
        coprintf ("I: creating dynamic queue '%s'", command->dest_name);
        self->dest = amq_dest_new (
            self->handle->vhost->queue_hash,
            self->handle->vhost,
            AMQP_SERVICE_QUEUE,
            TRUE,
            command->dest_name);
        amq_server_agent_handle_created (
            self->handle->thread,
            (dbyte) self->handle->key,
            self->dest->key);
    }
    if (self->dest) {
        if (self->dest->opt_max_consumers == 0
        ||  self->dest->opt_max_consumers > self->dest->queue->nbr_consumers)
            amq_queue_attach (self->dest->queue, self);
        else
            amq_global_set_error (AMQP_NOT_ALLOWED, "Destination consumer limit reached");
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
}

/*  For a topic service the consumer has its own subscription destination.
    (It does not reference a specific topic destination because the consumer
    can take messages from multiple topics.)
 */

static void
s_init_topic_consumer ($(selftype) *self, amq_handle_consume_t *command)
{
    ipr_shortstr_t
        dest_name;
    static int
        subscr_nbr = 0;                 /*  Generate unique subscriptions    */

    /*  Subscription is a destination held in the vhost subscr_hash table    */
    ipr_shortstr_fmt (dest_name, "sub-%d", ++subscr_nbr);
    self->dest = amq_dest_new (
        self->handle->vhost->subscr_hash,
        self->handle->vhost,
        AMQP_SERVICE_SUBSCR,
        TRUE,                           /*  Dynamic                          */
        dest_name);

    /*  Attach consumer to subscription queue - will be only consumer        */
    amq_queue_attach (self->dest->queue, self);

    /*  Create new subscription                                              */
    self->subscr = amq_subscr_new (self, command);
}
</private>

<method name = "selftest">
</method>

</class>
