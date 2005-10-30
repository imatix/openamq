<?xml?>
<class
    name      = "amq_broker"
    comment   = "AMQ broker class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This implements the broker object, which is the top-level
object holding server-wide values.
</doc>

<inherit class = "smt_object" />
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "broker">
        <field name = "started" label = "Date, time broker started">
          <get>
            apr_rfc822_date (field_value, self->started);
          </get>
        </field>
        <class name = "vhost" />
    </class>
</data>

<public name = "include">
#include "amq_server_classes.h"
</public>

<public>
extern $(selftype)
    *amq_broker;                        //  Single broker 
extern qbyte
    amq_broker_messages;                //  Current server throughput
</public>

<private>
$(selftype)
    *amq_broker = NULL;
qbyte
    amq_broker_messages = 0;
</private>

<context>
    apr_time_t
        started;                        //  Time started
</context>

<method name = "new">
    //  We use a single global vhost for now
    //  TODO: load list of vhosts from config file
    self->started = apr_time_now ();
    amq_server_agent_init ();
    amq_vhost = amq_vhost_new (self, "/");
    amq_server_callback_monitor (amq_broker_report);
</method>

<method name = "destroy">
    <action>
    amq_vhost_destroy (&amq_vhost);
    amq_server_agent_term ();
    </action>
</method>

<method name = "report">
    <argument name = "interval" type = "int">Interval, in seconds</argument>
    <local>
#   define SAMPLE_SIZE 10
    static qbyte
        sample_table [SAMPLE_SIZE];
    static Bool
        first_time = TRUE;
    static qbyte
        last_sample = 0,
        top_sample = 0;
    static uint
        sample_index = 0;
    uint
        cur_index,
        sample_size;
    qbyte
        cur_sample,
        total_sample;
    </local>
    if (first_time) {
        memset (&sample_table, sizeof (sample_table), 0);
        first_time = FALSE;
    }
    //  Unsigned arithmetic will always give us correct difference
    cur_sample = (amq_broker_messages - last_sample) / interval;

    last_sample = amq_broker_messages;
    if (cur_sample) {
        sample_table [sample_index++] = cur_sample;
        sample_index = sample_index % SAMPLE_SIZE;

        //  Calculate rolling average
        total_sample = 0;
        sample_size  = 0;
        if (top_sample < cur_sample)
            top_sample = cur_sample;
        for (cur_index = 0; cur_index < SAMPLE_SIZE; cur_index++) {
            if (sample_table [cur_index]) {
                total_sample += sample_table [cur_index];
                sample_size++;
            }
        }
        icl_console_print ("I: messages/sec current=%d average=%d peak=%d",
            cur_sample, total_sample / sample_size, top_sample);
    }
</method>

<method name = "selftest" />

</class>
