<?xml?>
<class
    name      = "amq_monitor"
    comment   = "Server monitor class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Provides monitoring and measurement functionality.
</doc>

<inherit class = "icl_base" />

<public name = "include">
#include "amq_server_classes.h"
</public>

<public>
extern qbyte
    amq_monitor_messages;               //  Current server throughput
</public>

<private>
qbyte
    amq_monitor_messages = 0;
</private>

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
    cur_sample = (amq_monitor_messages - last_sample) / interval;

    last_sample = amq_monitor_messages;
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

<method name = "selftest">
    amq_monitor_report (10);
    amq_monitor_messages += 100;
    amq_monitor_report (10);
    amq_monitor_messages += 200;
    amq_monitor_report (10);
    amq_monitor_messages += 150;
    amq_monitor_report (10);
</method>
    
</class>
