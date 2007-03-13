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
    name      = "amq_connection"
    comment   = "Connection management class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class acts as a management layer for the amq_server_connection
class.  Ideally, that would be an async class but for now, it implies
too large changes to the architecture.  At some future stage we will
merge these two classes into one.
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_list_item">
    <option name = "prefix" value = "by_broker" />
    <option name = "links" value = "1"/>
</inherit>
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "connection" parent = "broker" label = "Client Connection">
        <field name = "name" label = "Connection name">
          <local>
            amq_server_connection_t
                *connection;
          </local>
          <header>
            connection = amq_server_connection_link (self->connection);
            if (connection) {
          </header>
          <get>icl_shortstr_cpy (field_value, connection->id);</get>
          <footer>
                amq_server_connection_unlink (&connection);
            }
          </footer>
        </field>
        <field name = "pending" label = "Messages pending" type = "int">
          <rule name = "show on summary" />
          <local name = "get">
            size_t
                pending = 0;
            amq_consumer_t
                *consumer;              //  Consumer object reference
            amq_server_channel_t
                *channel;
            amq_queue_t
                *queue;
          </local>
          <get>
            //  Count total pending messages in all private (exclusive) queues
            channel = amq_server_channel_link (self->channel);
            if (channel) {
                consumer = amq_consumer_by_channel_first (channel->consumer_list);
                while (consumer) {
                    queue = amq_queue_link (consumer->queue);
                    if (queue) {
                        if (queue->exclusive)
                            pending += amq_queue_message_count (queue);
                        amq_queue_unlink (&queue);
                    }
                    consumer = amq_consumer_by_channel_next (&consumer);
                }
                amq_server_channel_unlink (&channel);
            }
            icl_shortstr_fmt (field_value, "%d", pending);
          </get>
        </field>
        <field name = "address" label = "Client IP address:port">
          <rule name = "show on summary" />
          <rule name = "ip address" />
          <get>icl_shortstr_cpy (field_value, connection->client_address);</get>
        </field>
        <field name = "user_name" label = "User login name">
          <get>icl_shortstr_cpy (field_value, connection->user_name);</get>
        </field>
        <field name = "instance" label = "Client instance name">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, connection->client_instance);</get>
        </field>
        <field name = "started" label = "Date, time connection started">
          <get>ipr_time_iso8601 (connection->started,
            ipr_date_format_minute, 0, FALSE, field_value);</get>
        </field>
        <field name = "messages_in" type = "int" label = "Messages published">
          <get>icl_shortstr_fmt (field_value, "%d", connection->contents_in);</get>
        </field>
        <field name = "messages_out" type = "int" label = "Messages consumed">
          <get>icl_shortstr_fmt (field_value, "%d", connection->contents_out);</get>
        </field>
        <field name = "megabytes_in" type = "int" label = "Megabytes published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (connection->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "megabytes_out" type = "int" label = "Megabytes consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (connection->traffic_out / (1024 * 1024)));</get>
        </field>
        <field name = "product" label = "Reported client product name">
          <get>icl_shortstr_cpy (field_value, connection->client_product);</get>
        </field>
        <field name = "version" label = "Reported client version">
          <get>icl_shortstr_cpy (field_value, connection->client_version);</get>
        </field>
        <field name = "platform" label = "Reported client platform">
          <get>icl_shortstr_cpy (field_value, connection->client_platform);</get>
        </field>
        <field name = "information" label = "Other client information">
          <get>icl_shortstr_cpy (field_value, connection->client_information);</get>
        </field>
        <field name = "trace" label = "Trace level, 0-3" type = "int">
          <get>
            icl_shortstr_fmt (field_value, "%d", connection->trace);
          </get>
          <put>
            amq_server_connection_set_trace (connection, atoi (field_value));
          </put>
        </field>

        <class name = "connection_queue" label = "Connection queues" repeat = "1">
          <get>
            channel = amq_server_channel_link (self->channel);
            if (channel) {
                consumer = amq_consumer_by_channel_first (channel->consumer_list);
                if (consumer)
                    icl_shortstr_fmt (field_value, "%d", consumer->mgt_connection_queue->object_id);
                amq_server_channel_unlink (&channel);
            }
          </get>
          <next>
            consumer = amq_consumer_by_channel_next (&consumer);
            if (consumer)
                icl_shortstr_fmt (field_value, "%d", consumer->mgt_connection_queue->object_id);
          </next>
        </class>

        <method name = "kill" label = "Kill connection">
          <doc>
          Disconnects the client application.
          </doc>
          <local>
            amq_server_connection_t
                *connection;
          </local>
          <exec>
            connection = amq_server_connection_link (self->connection);
            if (connection) {
                smt_log_print (amq_broker->alert_log,
                    "W: operator killed connection to %s", connection->client_address);
                amq_server_connection_kill (connection);
                amq_server_connection_unlink (&connection);
            }
          </exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_server_connection_t
        *connection;                    //  Parent connection
    amq_server_channel_t
        *channel;                       //  Parent channel
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Parent server channel</argument>
    //
    amq_connection_by_broker_push (broker->mgt_connection_list, self);
    self->channel = amq_server_channel_link (channel);
    if (self->channel)
        self->connection = amq_server_connection_link (self->channel->connection);
    if (!self->connection)
        self_destroy (&self);           //  Channel and/or connection lost
</method>

<method name = "destroy">
    <action>
    amq_server_channel_unlink (&self->channel);
    amq_server_connection_unlink (&self->connection);
    </action>
</method>

<method name = "selftest" />

</class>
