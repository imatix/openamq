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
        <field name = "name" label = "Broker cluster name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
          <put>icl_shortstr_cpy (self->name, field_value);</put>
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

<context>
    Bool
        locked;                         //  Is broker locked?
    int
        monitor_timer,                  //  Monitor timer
        dump_state_timer,               //  Dump state timer
        recycler_timer,                 //  Memory recycle timer
        auto_crash_timer,               //  Automatic failure
        auto_block_timer;               //  Automatic blockage
    ipr_meter_t
        *xmeter,                        //  External switch meter
        *imeter;                        //  Internal switch meter
</context>

<method name = "new">
    <header>
    //  Set host address from config file, otherwise it'll be set to
    //  our first IP address and port. We have to do this before
    //  the broker starts the protocol agent, which may also set
    //  the host value.
    icl_shortstr_cpy (self->host, amq_server_config_cluster_host (amq_server_config));
    </header>

    //  We use a single global vhost for now
    //  TODO: load list of vhosts from config file
    amq_vhost = amq_vhost_new (self, "/");
    self->xmeter = ipr_meter_new ();
    self->imeter = ipr_meter_new ();
    self->monitor_timer    = amq_server_config_monitor    (amq_server_config);
    self->dump_state_timer = amq_server_config_dump_state (amq_server_config);
    self->recycler_timer   = amq_server_config_recycler   (amq_server_config);
    self->auto_crash_timer = amq_server_config_auto_crash (amq_server_config);
    self->auto_block_timer = amq_server_config_auto_block (amq_server_config);

    randomize ();
    if (self->auto_crash_timer)
        self->auto_crash_timer = randomof (self->auto_crash_timer) + 1;
    if (self->auto_block_timer)
        self->auto_block_timer = randomof (self->auto_block_timer) + 1;
</method>

<method name = "destroy">
    <action>
    amq_vhost_destroy (&amq_vhost);
    ipr_meter_destroy (&self->xmeter);
    ipr_meter_destroy (&self->imeter);
    </action>
</method>

<event name = "monitor">
    <action>
    if (self->monitor_timer) {
        self->monitor_timer--;
        if (self->monitor_timer == 0) {
            self->monitor_timer = amq_server_config_monitor (amq_server_config);

            if (ipr_meter_mark (self->xmeter, amq_server_config_monitor (amq_server_config)))
                icl_console_print ("I: external message rate=%d average=%d peak=%d",
                    self->xmeter->current,
                    self->xmeter->average,
                    self->xmeter->maximum);

            if (ipr_meter_mark (self->imeter, amq_server_config_monitor (amq_server_config)))
                icl_console_print ("I: internal message rate=%d average=%d peak=%d",
                    self->imeter->current,
                    self->imeter->average,
                    self->imeter->maximum);
        }
    }
    if (self->dump_state_timer) {
        self->dump_state_timer--;
        if (self->dump_state_timer == 0) {
            static qbyte
                connections = 0,
                messages = 0,
                memorymb = 0,
                exchanges = 0,
                queues = 0,
                consumers = 0,
                bindings = 0;

            self->dump_state_timer = amq_server_config_dump_state (amq_server_config);

            //  Only print the state if it's changed
            if (connections != amq_server_connection_count ()
            ||  messages    != amq_content_basic_count ()
            ||  memorymb    != icl_mem_used () / (1024 * 1024)
            ||  exchanges   != amq_exchange_count ()
            ||  queues      != amq_queue_count ()
            ||  consumers   != amq_consumer_count ()
            ||  bindings    != amq_binding_count ()) {

                connections = amq_server_connection_count ();
                messages    = amq_content_basic_count ();
                memorymb    = icl_mem_used () / (1024 * 1024);
                exchanges   = amq_exchange_count ();
                queues      = amq_queue_count ();
                consumers   = amq_consumer_count ();
                bindings    = amq_binding_count ();

                icl_console_print ("I: cnn=%ld msg=%ld mem=%ldMB exc=%ld que=%ld csm=%ld bnd=%ld",
                    connections,
                    messages,
                    memorymb,
                    exchanges,
                    queues,
                    consumers,
                    bindings);
            }
        }
    }
    if (self->recycler_timer) {
        self->recycler_timer--;
        if (self->recycler_timer == 0) {
            self->recycler_timer = amq_server_config_recycler (amq_server_config);
            icl_system_purge ();
        }
    }
    if (self->auto_crash_timer) {
        if (--self->auto_crash_timer == 0) {
            icl_console_print ("W: #########################  AUTO-CRASH  ########################");
            icl_console_print ("W: server is now emulating a system crash, and will exit brutally.");
            exit (0);
        }
    }
    if (self->auto_block_timer) {
        if (--self->auto_block_timer == 0) {
            icl_console_print ("W: #########################  AUTO-BLOCK  ########################");
            icl_console_print ("W: server is now emulating a blockage, and will freeze for 5 minutes.");
            sleep (300);
        }
    }
    </action>
</event>

<method name = "selftest" />

</class>
