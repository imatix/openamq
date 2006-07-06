<?xml?>
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
<!-- any containers must come here -->
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "connection" parent = "broker" label = "Client Connection">
        <field name = "name" label = "Connection name">
          <get>icl_shortstr_cpy (field_value, self->parent->id);</get>
        </field>
        <field name = "address" label = "Client IP address:port">
          <rule name = "show on summary" />
          <rule name = "ip address" />
          <get>icl_shortstr_cpy (field_value, self->parent->client_address);</get>
        </field>
        <field name = "user_name" label = "User login name">
          <get>icl_shortstr_cpy (field_value, self->parent->user_name);</get>
        </field>
        <field name = "instance" label = "Client instance name">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->parent->client_instance);</get>
        </field>
        <field name = "queues" label = "Number of private queues" type = "int">
          <get>
          icl_shortstr_fmt (field_value, "%d",
            self->parent->own_queue_list? amq_queue_list_size (self->parent->own_queue_list): 0);
          </get>
        </field>
        <field name = "consumers" label = "Number of consumers" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d",
            self->parent->consumer_table? self->parent->consumer_table->nbr_items: 0);</get>
        </field>
        <field name = "channels" label = "Number of open channels" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d",
            self->parent->channels? self->parent->channels->nbr_items: 0);</get>
        </field>
        <field name = "started" label = "Date, time connection started">
          <get>ipr_time_iso8601 (self->parent->started,
            ipr_date_format_minute, 0, ipr_time_zone (), field_value);</get>
        </field>
        <field name = "traffic_in" type = "int" label = "Inbound traffic, MB">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->parent->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "traffic_out" type = "int" label = "Outbound traffic, MB">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->parent->traffic_out / (1024 * 1024)));</get>
        </field>
        <field name = "contents_in" type = "int" label = "Total messages received">
          <get>icl_shortstr_fmt (field_value, "%d", self->parent->contents_in);</get>
        </field>
        <field name = "contents_out" type = "int" label = "Total messages sent">
          <get>icl_shortstr_fmt (field_value, "%d", self->parent->contents_out);</get>
        </field>
        <field name = "channel_max" label = "Channel limit" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d", self->parent->channel_max);</get>
        </field>
        <field name = "frame_max" label = "Frame size limit" type = "int">
          <get>icl_shortstr_fmt (field_value, "%d", self->parent->frame_max);</get>
        </field>
        <field name = "product" label = "Reported client product name">
          <get>icl_shortstr_cpy (field_value, self->parent->client_product);</get>
        </field>
        <field name = "version" label = "Reported client version">
          <get>icl_shortstr_cpy (field_value, self->parent->client_version);</get>
        </field>
        <field name = "platform" label = "Reported client platform">
          <get>icl_shortstr_cpy (field_value, self->parent->client_platform);</get>
        </field>
        <field name = "information" label = "Other client information">
          <get>icl_shortstr_cpy (field_value, self->parent->client_information);</get>
        </field>
        <field name = "trace" label = "Trace level, 0-3" type = "int">
          <get>
            icl_shortstr_fmt (field_value, "%d", self->parent->trace);
          </get>
          <put>
            amq_server_connection_set_trace (self->parent, atoi (field_value));
          </put>
        </field>

        <class name = "queue" label = "Private queues" repeat = "1">
          <local>
            amq_queue_list_iterator_t
                queue_p = NULL;
          </local>
          <get>
            if (self->parent->own_queue_list
            && !amq_queue_list_empty (self->parent->own_queue_list)) {
                queue_p = amq_queue_list_begin (self->parent->own_queue_list);
                icl_shortstr_fmt (field_value, "%d", (*queue_p)->object_id);
            }
          </get>
          <next>
            if (queue_p) {
                queue_p = amq_queue_list_next (queue_p);
                if (queue_p)
                    icl_shortstr_fmt (field_value, "%d", (*queue_p)->object_id);
            }
          </next>
        </class>

        <method name = "kill" label = "Kill connection">
          <doc>
          Disconnects the client application.
          </doc>
          <exec>
            smt_log_print (amq_broker->alert_log,
                "W: operator killed connection to %s", self->parent->client_address);
            amq_server_connection_kill (self->parent);
          </exec>
        </method>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_server_connection_t
        *parent;                        //  Parent connection
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "parent" type = "amq_server_connection_t *">Parent server connection</argument>
    self->parent = amq_server_connection_link (parent);
</method>

<method name = "destroy">
    <action>
    amq_server_connection_unlink (&self->parent);
    </action>
</method>

<method name = "selftest" />

</class>
