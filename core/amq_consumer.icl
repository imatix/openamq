<?xml?>
<class
    name      = "amq_consumer"
    comment   = "Consumer class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_list_item" />

<import class = "ipr_classes" />
<import class = "amq_global"  />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  References to parent objects                                         */
    smt_thread_handle_t
        *thread;                        /*  Parent thread                    */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_connection_t
        *connection;                    /*  Parent connection                */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
    amq_handle_t
        *handle;                        /*  Parent handle for consumer       */
    amq_queue_t
        *queue;                         /*  Parent queue                     */
    qbyte
        client_id;                      /*  Parent client record             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    int
        prefetch;                       /*  Max prefetch size                */
    int
        window;                         /*  Remaining prefetch window        */
    Bool
        no_local;                       /*  Don't deliver own msgs to self   */
    Bool
        unreliable;                     /*  No ACKs are required             */

    /*  For topic subscriptions                                              */
    ipr_shortstr_t
        identifier;                     /*  Name of persistent subscription  */
    qbyte
        sub_dest_id;                    /*  Durable subscription is a queue  */
</context>

<method name = "new">
    <argument name = "handle"  type = "amq_handle_t *" />
    <argument name = "command" type = "amq_handle_consume_t *" />

    /*  De-normalise from parent object, for simplicity of use               */
    self->handle      = handle;
    self->client_id   = handle->client_id;
    self->channel     = handle->channel;
    self->connection  = handle->connection;
    self->vhost       = handle->vhost;
    self->thread      = handle->thread;
    self->db          = handle->db;
    self->ddb         = handle->ddb;

    /*  Initialise other properties                                          */
    self->prefetch   = command->prefetch? command->prefetch: 1;
    self->window     = self->prefetch;
    self->no_local   = command->no_local;
    self->unreliable = command->unreliable;
    ipr_shortstr_cpy (self->identifier, command->identifier);

    /*  Attach to queue                                                      */
    self->queue = amq_queue_consume (self, command->dest_name);
    if (!self->queue)
        $(selfname)_destroy (&self);
</method>

<method name = "destroy">
    if (self->queue)
        amq_queue_cancel (self->queue, self);
</method>

<method name = "cancel" template = "function">
    ASSERT (self->sub_dest_id);
    amq_db_dest_delete_fast (self->ddb, self->sub_dest_id);
    amq_consumer_destroy (&self);
</method>

<method name = "selftest">
</method>

</class>
