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
    name      = "amq_connection_queue"
    comment   = "Connection-to-queue management class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class shows the relationship from connection to queue, via the
consumer object.
</doc>

<inherit class = "smt_object" />
<!-- any containers must come here -->
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "connection_queue" parent = "connection" label = "Connection queue">
        <field name = "name">
          <local>
            amq_queue_t
                *queue;
          </local>
          <header>
            queue = amq_queue_link (self->queue);
            if (queue) {
          </header>
          <get>icl_shortstr_cpy (field_value, queue->name);</get>
          <footer>
                amq_queue_unlink (&queue);
            }
          </footer>
        </field>
        <field name = "enabled" label = "Queue accepts new messages?">
          <get>icl_shortstr_fmt (field_value, "%d", queue->enabled);</get>
          <put>queue->enabled = atoi (field_value);</put>
        </field>
        <field name = "exclusive" label = "Exclusive to one client?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", queue->exclusive);</get>
        </field>
        <field name = "exchange_name" label = "Exchange name">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, queue->last_exchange_name);</get>
        </field>
        <field name = "exchange_type" label = "Exchange type">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, queue->last_exchange_type);</get>
        </field>
        <field name = "routing_key" label = "Routing key">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, queue->last_routing_key);</get>
        </field>
        <field name = "binding_args" label = "Binding arguments">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, queue->last_binding_args);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", queue->auto_delete);</get>
        </field>
        <field name = "consumers" label = "Number of consumers" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_basic_consumer_count (queue->queue_basic));</get>
        </field>
        <field name = "pending" label = "Messages pending" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_message_count (queue));</get>
        </field>
        <field name = "messages_in" type = "int" label = "Messages published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", queue->contents_in);</get>
        </field>
        <field name = "messages_out" type = "int" label = "Messages consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", queue->contents_out);</get>
        </field>
        <field name = "megabytes_in" type = "int" label = "Megabytes published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (queue->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "megabytes_out" type = "int" label = "Megabytes consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (queue->traffic_out / (1024 * 1024)));</get>
        </field>
        <field name = "dropped" type = "int" label = "Messages dropped">
          <get>icl_shortstr_fmt (field_value, "%d", queue->dropped);</get>
        </field>

        <method name = "purge" label = "Purge all queue messages">
          <local>
            amq_queue_t
                *queue;
          </local>
          <exec>
            queue = amq_queue_link (self->queue);
            if (queue) {
                amq_queue_basic_purge (queue->queue_basic);
                amq_queue_unlink (&queue);
            }
          </exec>
        </method>
        <method name = "push" label = "Dispatch queue messages">
          <exec>
            queue = amq_queue_link (self->queue);
            if (queue) {
                amq_queue_dispatch (queue);
                amq_queue_unlink (&queue);
            }
          </exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_consumer_t
        *consumer;                          //  Consumer for this relationship
    amq_queue_t
        *queue;                             //  Link to queue
</context>

<!-- The management object has as first argument the management object that
     is its parent.
     Second, whatever objects it has to refer to.
     When we create a new mgt object we also grab links to all objects that
     provide necessary data.  If any of these links fail, we destroy ourselves.
     -->
<method name = "new">
    <argument name = "connection" type = "amq_connection_t *">Parent connection</argument>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    //
    self->consumer = amq_consumer_link (consumer);
    if (self->consumer)
        self->queue = amq_queue_link (consumer->queue);
    if (!self->queue)
        self_destroy (&self);
</method>

<method name = "destroy">
    <action>
    amq_consumer_unlink (&self->consumer);
    amq_queue_unlink (&self->queue);
    </action>
</method>

<method name = "selftest" />

</class>
