<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name      = "ipr_meter"
    comment   = "Traffic rate metering"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The traffic rate meter tracks activity per time segment.
</doc>
<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<public name = "header">
//  Metering is done on small sample size

#define IPR_METER_HISTORY  10
</public>

<context>
    qbyte
        sample,                         //  Current meter value
        last_sample,                    //  Previous meter value
        top_sample,                     //  Highest meter value
        history [IPR_METER_HISTORY];    //  Last set of samples
    uint
        history_index;                  //  Current point in history
    qbyte
        average,                        //  Last average value
        maximum,                        //  Last maximum value
        current;                        //  Last current value
</context>

<method name = "count" template = "function" inline = "1">
    <doc>
    Increments the current sample value.  You should call this for each
    activity you want to meter.  Note that the sample values can wrap around,
    this is intentional and works correctly.
    </doc>
    icl_atomic_inc32 ((volatile qbyte *) &(self->sample));
</method>

<method name = "mark" template = "function">
    <doc>
    Returns the current metered rate.  This is calculated by taking the
    difference between the current sample rate and the last measured
    rate and then calculating the average over the history.
    </doc>
    <argument name = "interval" type = "int">Interval, in seconds</argument>
    <local>
    uint
        cur_index,
        sample_size;
    qbyte
        sample_total;
    </local>
    assert (interval);
    //
    //  Unsigned arithmetic will always give us correct difference
    self->current = (self->sample - self->last_sample) / interval;
    self->last_sample = self->sample;

    //  Ignore meter values of zero
    if (self->current) {
        self->history [self->history_index++] = self->current;
        self->history_index = self->history_index % IPR_METER_HISTORY;

        //  Calculate rolling average
        sample_total = 0;
        sample_size  = 0;
        self->maximum = 0;
        for (cur_index = 0; cur_index < IPR_METER_HISTORY; cur_index++) {
            if (self->history [cur_index]) {
                sample_total += self->history [cur_index];
                sample_size++;
                //  Show maximum of last recorded history
                if (self->maximum < self->history [cur_index])
                    self->maximum = self->history [cur_index];
            }
        }
        self->average = sample_total / sample_size;
    }
    rc = (int) self->current;
</method>

<method name = "selftest">
    <local>
    ipr_meter_t
        *meter;
    </local>
    meter = ipr_meter_new ();

    ipr_meter_count (meter);
    ipr_meter_count (meter);
    ipr_meter_count (meter);
    ipr_meter_count (meter);
    ipr_meter_count (meter);
    ipr_meter_count (meter);

    assert (ipr_meter_mark (meter, 1) == 6);
    ipr_meter_destroy (&meter);
</method>

</class>
