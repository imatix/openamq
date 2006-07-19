<?xml?>
<class
    name      = "amq_queue_connection"
    comment   = "Queue-to-connection management class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class shows the relationship from queue to connection, via the
consumer object.
</doc>

<inherit class = "smt_object" />
<!-- any containers must come here -->
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "queue_connection" parent = "queue" label = "Queue connection">
        <field name = "name" label = "Connection name">
          <get>icl_shortstr_cpy (field_value, self->connection->id);</get>
        </field>
        <field name = "address" label = "Client IP address:port">
          <rule name = "show on summary" />
          <rule name = "ip address" />
          <get>icl_shortstr_cpy (field_value, self->connection->client_address);</get>
        </field>
        <field name = "user_name" label = "User login name">
          <get>icl_shortstr_cpy (field_value, self->connection->user_name);</get>
        </field>
        <field name = "instance" label = "Client instance name">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->connection->client_instance);</get>
        </field>
        <field name = "started" label = "Date, time connection started">
          <get>ipr_time_iso8601 (self->connection->started,
            ipr_date_format_minute, 0, ipr_time_zone (), field_value);</get>
        </field>
        <field name = "messages_in" type = "int" label = "Messages published">
          <get>icl_shortstr_fmt (field_value, "%d", self->connection->contents_in);</get>
        </field>
        <field name = "messages_out" type = "int" label = "Messages consumed">
          <get>icl_shortstr_fmt (field_value, "%d", self->connection->contents_out);</get>
        </field>
        <field name = "megabytes_in" type = "int" label = "Megabytes published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->connection->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "megabytes_out" type = "int" label = "Megabytes consumed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->connection->traffic_out / (1024 * 1024)));</get>
        </field>
        <field name = "product" label = "Reported client product name">
          <get>icl_shortstr_cpy (field_value, self->connection->client_product);</get>
        </field>
        <field name = "version" label = "Reported client version">
          <get>icl_shortstr_cpy (field_value, self->connection->client_version);</get>
        </field>
        <field name = "platform" label = "Reported client platform">
          <get>icl_shortstr_cpy (field_value, self->connection->client_platform);</get>
        </field>
        <field name = "information" label = "Other client information">
          <get>icl_shortstr_cpy (field_value, self->connection->client_information);</get>
        </field>
        <field name = "trace" label = "Trace level, 0-3" type = "int">
          <get>
            icl_shortstr_fmt (field_value, "%d", self->connection->trace);
          </get>
          <put>
            amq_server_connection_set_trace (self->connection, atoi (field_value));
          </put>
        </field>

        <method name = "kill" label = "Kill connection">
          <doc>
          Disconnects the client application.
          </doc>
          <exec>
            asl_log_print (amq_broker->alert_log,
                "W: operator killed connection to %s", self->connection->client_address);
            amq_server_connection_kill (self->connection);
          </exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_consumer_t
        *consumer;                      //  Consumer for this relationship
    amq_server_connection_t
        *connection;                    //  Link to connection
    amq_server_channel_t
        *channel;                       //  Link to channel
</context>

<method name = "new">
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    //
    self->consumer = amq_consumer_link (consumer);
    self->channel  = amq_server_channel_link (consumer->channel);
    if (self->channel)
        self->connection = amq_server_connection_link (self->channel->connection);
    if (!self->connection)
        self_destroy (&self);           //  Channel and/or connection lost
</method>

<method name = "destroy">
    <action>
    amq_consumer_unlink (&self->consumer);
    amq_server_channel_unlink (&self->channel);
    amq_server_connection_unlink (&self->connection);
    </action>
</method>

<method name = "selftest" />

</class>
