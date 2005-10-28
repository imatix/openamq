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
<!-- any containers must come here -->
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "vhost" parent = "broker">
        <field name = "name" label = "Virtual host path" />
        <class name = "exchange" />
        <class name = "queue"    />
    </class>
</data>

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
    amq_broker_t
        *broker;                        //  Parent broker
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
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "name"   type = "char *">Virtual host name</argument>

    //TODO: load config from directory
    //TODO: amq_server object, holding vhost hash table
    icl_shortstr_cpy (self->name, name);
    icl_console_print ("I: starting virtual host '%s'", self->name);

    self->broker = broker;
    self->config = amq_vhost_config_new (
        NULL, NULL, amq_server_config_trace (amq_server_config));

    self->exchange_table = amq_exchange_table_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_list_new ();

    //  Automatic wiring schemes
    s_exchange_declare (self, "amq.direct", AMQ_EXCHANGE_DIRECT);
    s_exchange_declare (self, "amq.reply",  AMQ_EXCHANGE_DIRECT);
    s_exchange_declare (self, "amq.topic",  AMQ_EXCHANGE_TOPIC);
    s_exchange_declare (self, "amq.match",  AMQ_EXCHANGE_HEADERS);
    s_exchange_declare (self, "amq.system", AMQ_EXCHANGE_SYSTEM);
    s_exchange_declare (self, "amq.notify", AMQ_EXCHANGE_TOPIC);
</method>

<method name = "destroy">
    <action>
    icl_console_print ("I: stopping virtual host '%s'", self->name);
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

<private name = "header">
//  Prototypes for local functions
static void
    s_exchange_declare (amq_vhost_t *self, char *name, int type);
</private>

<private name = "footer">
static void
s_exchange_declare (amq_vhost_t *self, char *name, int type)
{
    amq_exchange_t
        *exchange;                      //  Predeclared exchanges

    exchange = amq_exchange_new (
        self->exchange_table,
        self,                           //  Parent vhost
        type,                           //  Exchange type
        name,                           //  Exchange name
        TRUE,                           //  Durable, allows durable queues
        FALSE,                          //  Do not auto-delete
        FALSE);                         //  Not internal
    assert (exchange);
    amq_exchange_unlink (&exchange);
}
</private>

<method name = "selftest" />

</class>
