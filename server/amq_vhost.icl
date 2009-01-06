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

<import class = "amq_server_classes" />

<context>
    amq_broker_t
        *broker;                        //  Parent broker
    icl_shortstr_t
        name;                           //  Virtual host name
    amq_vhost_config_t
        *config;                        //  Virtual host configuration
    amq_exchange_table_t
        *exchange_table;                //  Exchanges for vhost, hash table
    amq_exchange_by_vhost_t
        *exchange_list;                 //  List of exchanges
    amq_queue_table_t
        *queue_table;                   //  Queues for vhost, hash table
    amq_queue_by_vhost_t
        *queue_list;                    //  Queues for dispatching
    amq_exchange_t
        *default_exchange;              //  Default exchange
    ipr_symbol_table_t
        *shared_queues;                 //  Cluster shared queues
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "name"   type = "char *">Virtual host name</argument>
    icl_shortstr_cpy (self->name, name);
    smt_log_print (amq_broker->daily_log, "I: starting virtual host '%s'", self->name);

    self->broker = broker;
    self->config = amq_vhost_config_new ();

    self->exchange_table = amq_exchange_table_new ();
    self->exchange_list  = amq_exchange_by_vhost_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_by_vhost_new ();
    self->shared_queues  = ipr_symbol_table_new ();

    //  Automatic wiring schemes
    s_exchange_declare (self, "$default$",   AMQ_EXCHANGE_DIRECT,  TRUE,  FALSE);
    s_exchange_declare (self, "amq.fanout",  AMQ_EXCHANGE_FANOUT,  FALSE, FALSE);
    s_exchange_declare (self, "amq.direct",  AMQ_EXCHANGE_DIRECT,  FALSE, FALSE);
    s_exchange_declare (self, "amq.topic",   AMQ_EXCHANGE_TOPIC,   FALSE, FALSE);
    s_exchange_declare (self, "amq.headers", AMQ_EXCHANGE_HEADERS, FALSE, FALSE);
    s_exchange_declare (self, "amq.system",  AMQ_EXCHANGE_SYSTEM,  FALSE, FALSE);
    s_exchange_declare (self, "amq.status",  AMQ_EXCHANGE_DIRECT,  FALSE, FALSE);

    //  These exchanges are deprecated
    s_exchange_declare (self, "amq.notify",  AMQ_EXCHANGE_TOPIC,   FALSE, FALSE);
    
    //  These exchanges are automatically federated if --attach is specified
    s_exchange_declare (self, "amq.service", AMQ_EXCHANGE_DIRECT,  FALSE, TRUE);
    s_exchange_declare (self, "amq.data",    AMQ_EXCHANGE_TOPIC,   FALSE, TRUE);
    s_exchange_declare (self, "amq.dataex",  AMQ_EXCHANGE_HEADERS, FALSE, TRUE);
</method>

<method name = "destroy">
    <action>
    amq_exchange_t
        *exchange;

    amq_exchange_unlink            (&self->default_exchange);
    amq_vhost_config_destroy       (&self->config);
    amq_exchange_table_destroy     (&self->exchange_table);
    exchange = amq_exchange_by_vhost_pop (self->exchange_list);
    while (exchange) {
        amq_exchange_destroy (&exchange);
        exchange = amq_exchange_by_vhost_pop (self->exchange_list);
    }
    amq_exchange_by_vhost_destroy  (&self->exchange_list);
    amq_queue_table_destroy        (&self->queue_table);
    amq_queue_by_vhost_destroy     (&self->queue_list);
    ipr_symbol_table_destroy       (&self->shared_queues);
    </action>
</method>

<method name = "unbind queue" template = "async function" async = "1">
    <doc>
    Unbind a queue from the vhost.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">The queue to unbind</argument>
    //
    <possess>
    queue = amq_queue_link (queue);
    </possess>
    <release>
    amq_queue_unlink (&queue);
    </release>
    //
    <action>
    amq_exchange_t
        *exchange;

    //  Go through all exchanges & bindings, remove link to queue
    exchange = amq_exchange_by_vhost_first (self->exchange_list);
    while (exchange) {
        amq_exchange_unbind_queue (exchange, queue);
        exchange = amq_exchange_by_vhost_next (&exchange);
    }
    //  Remove the queue from queue list and queue table
    amq_queue_by_vhost_remove (queue);
    amq_queue_table_remove (queue);
    </action>
</method>

<private name = "header">
//  Prototypes for local functions
static void
    s_exchange_declare (
        amq_vhost_t *self, char *name, int type, 
        Bool default_exchange, Bool auto_federate);
</private>

<private name = "footer">
static void
s_exchange_declare (
    amq_vhost_t *self, 
    char *name, 
    int   type, 
    Bool  default_exchange, 
    Bool  auto_federate)
{
    amq_exchange_t
        *exchange;                      //  Predeclared exchange
    exchange = amq_exchange_new (
        self->exchange_table,
        self,                           //  Parent vhost
        type,                           //  Exchange type
        name,                           //  Exchange name
        default_exchange,               //  Internal?
        auto_federate);                 //  Federate automatically?
    assert (exchange);
    
    //  If this is the default exchange grab hold of it in our vhost
    if (default_exchange)
        self->default_exchange = exchange;
    else
        amq_exchange_unlink (&exchange);
}
</private>

<method name = "selftest" />

</class>
