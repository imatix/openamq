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
    name      = "amq_broker"
    comment   = "AMQ server broker class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<inherit class = "asl_broker" />
<inherit class = "amq_console_object" />
<option name = "basename" value = "amq_server" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "broker" label = "Broker">
        <field name = "name" label = "Broker name">
          <get>icl_shortstr_fmt (field_value, "OpenAMQ %s", VERSION);</get>
        </field>
        <field name = "started" label = "Date, time broker started">
          <get>ipr_time_iso8601 (self->started,
            ipr_date_format_minute, 0, FALSE, field_value);</get>
        </field>
        <field name = "locked" type = "bool" label = "Broker is locked?">
          <get>icl_shortstr_fmt (field_value, "%d", self->locked);</get>
        </field>
        <field name = "datamem" label = "Memory used for all data">
          <get>icl_shortstr_fmt (field_value, "%dK", (int) (icl_mem_used () / 1024));</get>
        </field>
        <field name = "bucketmem" label = "Memory used for messages">
          <get>icl_shortstr_fmt (field_value, "%dK", (int) (ipr_bucket_used () / 1024));</get>
        </field>
        <field name = "messages" type = "int" label = "Number of queued messages">
          <get>icl_shortstr_fmt (field_value, "%d", amq_content_basic_count ());</get>
        </field>
        <field name = "consumers" type = "int" label = "Number of queue consumers">
          <get>icl_shortstr_fmt (field_value, "%d", amq_consumer_count ());</get>
        </field>
        <field name = "bindings" type = "int" label = "Number of queue bindings">
          <get>icl_shortstr_fmt (field_value, "%d", amq_binding_count ());</get>
        </field>

        <class name = "exchange" label = "Message exchanges" repeat = "1">
          <local>
            amq_exchange_t
                *exchange;
          </local>
          <get>
            exchange = amq_exchange_list_first (self->exchange_list);
            icl_shortstr_fmt (field_value, "%d", exchange->object_id);
          </get>
          <next>
            exchange = amq_exchange_list_next (&exchange);
            if (exchange)
                icl_shortstr_fmt (field_value, "%d", exchange->object_id);
          </next>
        </class>

        <class name = "queue" label = "Shared queues" repeat = "1">
          <local>
            amq_queue_t
                *queue;
          </local>
          <get>
            queue = amq_queue_list_first (self->queue_list);
            while (queue && !self->verbose && queue->exclusive)
                queue = amq_queue_list_next (&queue);
            if (queue)
                icl_shortstr_fmt (field_value, "%d", queue->object_id);
          </get>
          <next>
            queue = amq_queue_list_next (&queue);
            while (queue && !self->verbose && queue->exclusive)
                queue = amq_queue_list_next (&queue);
            if (queue)
                icl_shortstr_fmt (field_value, "%d", queue->object_id);
          </next>
        </class>

        <class name = "connection" label = "Connections" repeat = "1">
          <local>
            amq_connection_t
                *connection;
          </local>
          <get>
            connection = amq_connection_by_broker_first (self->mgt_connection_list);
            if (connection)
                icl_shortstr_fmt (field_value, "%d", connection->object_id);
          </get>
          <next>
            connection = amq_connection_by_broker_next (&connection);
            if (connection)
                icl_shortstr_fmt (field_value, "%d", connection->object_id);
          </next>
        </class>

        <class name = "config" label = "Configuration" source = "amq_console_config">
          <get>
            icl_shortstr_fmt (field_value, "%d", amq_console_config->object_id);
          </get>
        </class>

        <method name = "shutdown" label = "Shutdown broker">
          <doc>
          Shuts-down the broker. All client connections are broken, and the
          broker process will exit.
          </doc>
          <exec>
            smt_log_print (amq_broker->alert_log,
                "W: operator requested shutdown - closing all connections");
            amq_broker_shutdown (self);
          </exec>
        </method>

        <method name = "lock" label = "Prevent new connections">
          <doc>
          Locks or unlocks the broker. When the broker is locked it will refuse
          new client connections, though you can still connect using the shell.
          </doc>
          <field name = "setting" type = "bool" label = "1|0"/>
          <exec>self->locked = setting;</exec>
        </method>

        <method name = "verbose" label = "Show all queues">
          <doc>
          Enables or disables listing of exclusive queues.  By default the broker
          only lists shared queues.
          </doc>
          <field name = "setting" type = "bool" label = "1|0"/>
          <exec>self->verbose = setting;</exec>
        </method>
    </class>
</data>

<private name = "async header">
#include "version.h"
</private>

<context>
    amq_exchange_table_t
        *exchange_table;                //  Table of exchanges
    amq_exchange_list_t
        *exchange_list;                 //  List of exchanges
    amq_queue_table_t
        *queue_table;                   //  Table of queues
    amq_queue_list_t
        *queue_list;                    //  List of queues
    amq_exchange_t
        *default_exchange;              //  Default exchange
    ipr_symbol_table_t
        *shared_queues;                 //  Cluster shared queues
    Bool
        locked,                         //  Is broker locked?
        restart,                        //  Restart broker after exit?
        verbose;                        //  Show exclusive queues?
    int
        dump_state_timer,               //  Dump state timer
        auto_crash_timer,               //  Automatic failure
        auto_block_timer;               //  Automatic blockage
    amq_connection_by_broker_t
        *mgt_connection_list;           //  Connection mgt objects list
    amq_failover_t
        *failover;                      //  Failover controller
    qbyte
        direct_sunk,                    //  Direct messages sunk
        direct_fed;                     //  Direct messages fed
    qbyte
        direct_high;                    //  High water mark for direct feeds
</context>

<method name = "new">
    self->dump_state_timer = amq_server_config_dump_state (amq_server_config);
    self->auto_crash_timer = amq_server_config_auto_crash (amq_server_config);
    self->auto_block_timer = amq_server_config_auto_block (amq_server_config);
    self->mgt_connection_list = amq_connection_by_broker_new ();
    amq_console_config = amq_console_config_new (self);

    self->exchange_table = amq_exchange_table_new ();
    self->exchange_list  = amq_exchange_list_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_list_new ();
    self->shared_queues  = ipr_symbol_table_new ();

    //  Automatic wiring schemes
    //  If you change these, please maintain zyre/zyre_restms.icl
    s_exchange_declare (self, "$default$",   AMQ_EXCHANGE_DIRECT,  TRUE,  FALSE);
    s_exchange_declare (self, "amq.fanout",  AMQ_EXCHANGE_FANOUT,  FALSE, FALSE);
    s_exchange_declare (self, "amq.direct",  AMQ_EXCHANGE_DIRECT,  FALSE, FALSE);
    s_exchange_declare (self, "amq.topic",   AMQ_EXCHANGE_TOPIC,   FALSE, FALSE);
    s_exchange_declare (self, "amq.headers", AMQ_EXCHANGE_HEADERS, FALSE, FALSE);
    s_exchange_declare (self, "amq.regexp",  AMQ_EXCHANGE_REGEXP,  FALSE, FALSE);
    s_exchange_declare (self, "amq.system",  AMQ_EXCHANGE_SYSTEM,  FALSE, FALSE);
    s_exchange_declare (self, "amq.status",  AMQ_EXCHANGE_DIRECT,  FALSE, FALSE);
    s_exchange_declare (self, "amq.notify",  AMQ_EXCHANGE_TOPIC,   FALSE, FALSE);

    //  These exchanges are automatically federated if --attach is specified
    s_exchange_declare (self, "amq.service", AMQ_EXCHANGE_DIRECT,  FALSE, TRUE);
    s_exchange_declare (self, "amq.data",    AMQ_EXCHANGE_TOPIC,   FALSE, TRUE);
    s_exchange_declare (self, "amq.dataex",  AMQ_EXCHANGE_HEADERS, FALSE, TRUE);

    randomize ();
    if (self->auto_crash_timer)
        self->auto_crash_timer = randomof (self->auto_crash_timer) + 1;
    if (self->auto_block_timer)
        self->auto_block_timer = randomof (self->auto_block_timer) + 1;

    //  Initialise failover agent
    self->failover = amq_failover_new ();

    //  Calculate server performance
    smt_coremark_calculate (self->daily_log, amq_server_config_coremark_cycles (amq_server_config));
</method>

<method name = "destroy">
    <action>
    {
    amq_exchange_t
        *exchange;

    amq_failover_destroy (&self->failover);
    amq_console_config_destroy (&amq_console_config);
    amq_connection_by_broker_destroy (&self->mgt_connection_list);

    amq_exchange_unlink (&self->default_exchange);
    amq_exchange_table_destroy (&self->exchange_table);
    exchange = amq_exchange_list_pop (self->exchange_list);
    while (exchange) {
        amq_exchange_destroy (&exchange);
        exchange = amq_exchange_list_pop (self->exchange_list);
    }
    amq_exchange_list_destroy (&self->exchange_list);
    amq_queue_table_destroy (&self->queue_table);
    amq_queue_list_destroy (&self->queue_list);
    ipr_symbol_table_destroy (&self->shared_queues);
    }
    </action>
</method>

<method name = "unbind queue" template = "async function">
    <doc>
    Unbind a queue from the broker.
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
    amq_server_connection_t
        *connection;

    //  Go through all connections
    connection = amq_server_connection_list_first (self->connection_list);
    while (connection) {
        amq_server_connection_unbind_queue (connection, queue);
        connection = amq_server_connection_list_next (&connection);
    }
    </action>
</method>

<method name = "delete queue" template = "async function">
    <doc>
    Unbind and delete a queue from the broker.
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
    exchange = amq_exchange_list_first (self->exchange_list);
    while (exchange) {
        amq_exchange_broker_unbind_queue (exchange, queue);
        exchange = amq_exchange_list_next (&exchange);
    }
    //  Remove the queue from queue list and queue table
    amq_queue_list_remove (queue);
    amq_queue_table_remove (queue);
    </action>
</method>

<event name = "monitor">
    <action>
    if (self->dump_state_timer) {
        self->dump_state_timer--;
        if (self->dump_state_timer == 0) {
            self->dump_state_timer = amq_server_config_dump_state (amq_server_config);
            smt_log_print (amq_broker->debug_log,
                "I: cnn=%d msg=%d mem=%uK/%uK exc=%d que=%d csm=%d bnd=%d adx=%d idx=%d map=%d",
                amq_server_connection_count (),
                amq_content_basic_count (),
                ipr_bucket_used () / 1024,
                icl_mem_used () / 1024,
                amq_exchange_count (),
                amq_queue_count (),
                amq_consumer_count (),
                amq_binding_count (),
                amq_index_count (),
                ipr_index_count (),
                ipr_bits_count ());

            smt_log_print (amq_broker->debug_log,
                "I: qcn=%d cnq=%d dsk=%d dfd=%d din=%d dot=%d dhi=%d",
                amq_queue_connection_count (),
                amq_connection_queue_count (),
                amq_broker->direct_sunk,
                amq_broker->direct_fed,
                amq_broker->direct_in,
                amq_broker->direct_out,
                amq_broker->direct_high);
        }
    }
    if (self->auto_crash_timer) {
        if (--self->auto_crash_timer == 0) {
            smt_log_print (amq_broker->alert_log,
                "W: ************************  AUTO-CRASH  ************************");
            smt_log_print (amq_broker->alert_log,
                "W: server is now emulating a system crash, and will exit brutally.");
            exit (EXIT_SUCCESS);
        }
    }
    if (self->auto_block_timer) {
        if (--self->auto_block_timer == 0) {
            smt_log_print (amq_broker->alert_log,
                "W: ************************  AUTO-BLOCK  ************************");
            smt_log_print (amq_broker->alert_log,
                "W: server is now emulating a blockage, and will freeze for 5 minutes.");
            sleep (300);
        }
    }
    </action>
</event>

<private name = "header">
//  Prototypes for local functions
static void
s_exchange_declare (
    amq_broker_t *self, char *name, int type, Bool default_exchange, Bool auto_federate);
</private>

<private name = "footer">
static void
s_exchange_declare (
    amq_broker_t *self,
    char *name,
    int   type,
    Bool  default_exchange,
    Bool  auto_federate)
{
    amq_exchange_t
        *exchange;                      //  Predeclared exchange
    exchange = amq_exchange_new (
        type,                           //  Exchange type
        name,                           //  Exchange name
        default_exchange,               //  Internal?
        auto_federate);                 //  Federate automatically?
    assert (exchange);

    //  If this is the default exchange grab hold of it in self
    if (default_exchange)
        self->default_exchange = exchange;
    else
        amq_exchange_unlink (&exchange);
}
</private>

<method name = "selftest" />

</class>
