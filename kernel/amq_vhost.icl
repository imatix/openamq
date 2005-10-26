<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
Defines a virtual host. This is a lock-free asynchronous class.
</doc>

<inherit class = "smt_object" />

<public name = "include">
#include "amq_server_classes.h"
</public>

<public>
extern $(selftype)
    *amq_vhost;                         //  Single vhost, for now
</public>

<private>
$(selftype)
    *amq_vhost = NULL;
</private>

<context>
    icl_shortstr_t
        name;                           //  Virtual host name
    amq_vhost_config_t
        *config;                        //  Virtual host configuration
    amq_exchange_table_t
        *exchange_table;                //  Exchanges for vhost, hash table
    amq_queue_table_t
        *queue_table;                   //  Queues for vhost, hash table
    amq_queue_list_t
        *queue_list;                    //  Queues for dispatching
</context>

<method name = "new">
    <argument name = "name" type = "char *">Virtual host name</argument>

    //TODO: load config from directory
    //TODO: amq_server object, holding vhost hash table
    icl_shortstr_cpy (self->name, name);
    icl_console_print ("I: starting virtual host '%s'", self->name);

    self->config         = amq_vhost_config_new (
        NULL, NULL, amq_server_config_trace (amq_server_config));
    self->exchange_table = amq_exchange_table_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_list_new ();
</method>

<method name = "destroy">
    <action>
    icl_console_print ("I: stopping virtual host '%s'...", self->name);
    amq_vhost_config_destroy   (&self->config);
    amq_exchange_table_destroy (&self->exchange_table);
    amq_queue_table_destroy    (&self->queue_table);
    amq_queue_list_destroy     (&self->queue_list);
    </action>
</method>

<method name = "dispatch" template = "async function" async = "1">
    <doc>
    Dispatches all queues that have received new messages, i.e. have the
    'dirty' property set. All dispatchable queues are at the head of the
    vhost queue list.
    </doc>
    <action>
    amq_queue_t
        *queue;

    //  Dispatch all dirty message queues, which come at start of list
    queue = amq_queue_list_first (self->queue_list);
    while (queue) {
        if (queue->dirty) {
            amq_queue_dispatch (queue);
            queue = amq_queue_list_next (&queue);
        }
        else {
            amq_queue_unlink (&queue);
            break;
        }
    }
    </action>
</method>

<method name = "selftest">
    <local>
    amq_vhost_t
        *vhost;
    </local>
    //
    smt_os_thread_initialise ();
    vhost = amq_vhost_new ("/");
    amq_vhost_destroy (&vhost);
</method>

</class>
