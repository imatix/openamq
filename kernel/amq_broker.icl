<?xml?>
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
    <class name = "broker" label = "Brokers" >
        <field name = "name" label = "Broker name">
          <get>icl_shortstr_fmt (field_value, "OpenAMQ:%d", amq_server_config_port (amq_server_config));</get>
        </field>
        <field name = "started" label = "Date, time broker started">
          <get>ipr_time_iso8601 (self->started, ipr_date_format_minute, 0, 0, field_value);</get>
        </field>
        <field name = "start_time" type = "time" label = "Date, time broker started, time_t">
          <get>icl_shortstr_fmt (field_value, "%ld", (time_t) self->started);</get>
        </field>
        <field name = "locked" type = "bool" label = "Broker locked?">
          <get>icl_shortstr_fmt (field_value, "%d", self->locked);</get>
        </field>
        <field name = "spid" label = "Broker server process id (spid)">
          <get>icl_shortstr_cpy (field_value, self->spid);</get>
          <put>icl_shortstr_cpy (self->spid, field_value);</get>
        </field>
        <field name = "vhost" type = "objref" repeat = "1">
          <get>
            icl_shortstr_fmt (field_value, "%ld", amq_vhost->object_id);
          </get>
        </field>
        <!-- TODO
        <field name = "cluster" type = "objref">
          <get>
            icl_shortstr_fmt (field_value, "%ld", amq_cluster->object_id);
          </get>
        </field>
        -->
        <method name = "lock" label = "Prevent new connections">
          <exec>self->locked = TRUE;</exec>
        </method>
        <method name = "unlock" label = "Allow new connections">
          <exec>self->locked = FALSE;</exec>
        </method>
    </class>
</data>

<public>
extern $(selftype)
    *amq_broker;                        //  Single broker
</public>

<context>
    Bool
        locked;                         //  Is broker locked?
    ipr_meter_t
        *xmeter,                        //  External switch meter
        *imeter;                        //  Internal switch meter
</context>

<method name = "new">
    <header>
    //  Set callback from config file, otherwise it'll be set to
    //  our first IP address and port. We have to do this before
    //  the broker starts the protocol agent, which may also set
    //  the callback.
    icl_shortstr_cpy (self->callback,
        amq_server_config_cluster_callback (amq_server_config));
    </header>

    //  We use a single global vhost for now
    //  TODO: load list of vhosts from config file
    amq_vhost = amq_vhost_new (self, "/");
    self->xmeter = ipr_meter_new ();
    self->imeter = ipr_meter_new ();
</method>

<method name = "destroy">
    <action>
    amq_vhost_destroy (&amq_vhost);
    ipr_meter_destroy (&self->xmeter);
    ipr_meter_destroy (&self->imeter);
    </action>
</method>

<method name = "report">
    <action>
    if (ipr_meter_mark (self->xmeter, interval))
        icl_console_print ("I: external message rate=%d average=%d peak=%d",
            self->xmeter->current,
            self->xmeter->average,
            self->xmeter->maximum);

    if (ipr_meter_mark (self->imeter, interval))
        icl_console_print ("I: internal message rate=%d average=%d peak=%d",
            self->imeter->current,
            self->imeter->average,
            self->imeter->maximum);
    </action>
</method>

<method name = "selftest" />

</class>
