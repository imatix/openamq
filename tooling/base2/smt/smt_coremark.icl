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
    name      = "smt_coremark"
    comment   = "CoreMark calculator"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    CoreMark is the Base2 performance unit for asynchronous multithreaded
    servers.  It measures the event-processing capacity of a single CPU core
    which can be multiplied by the number of available cores to give an
    overall server capacity.

    To use, send a start, then N cycles (10k-1M) and then a finish.
</doc>

<inherit class = "smt_object" />

<import class = "smt" />
<import class = "smt_log" />

<context>
    apr_time_t
        started;                        //  Time started
    size_t
        cycles;                         //  Number of cycles
</context>

<method name = "start" template = "async function">
    <action>
    self->started = apr_time_now ();
    </action>
</method>

<method name = "cycle" template = "async function">
    <action>
    self->cycles++;
    </action>
</method>

<method name = "finish" template = "async function">
    <argument name = "log" type = "smt_log_t *" />
    <action>
    double
        capacity = (double) (self->cycles / (double) (apr_time_now () - self->started));

    smt_log_print (log, "I: CoreMark performance ~%d000 events/second", (int) (capacity * 1000));
    </action>
</method>

<method name = "calculate">
    <doc>
    Runs the CoreMark test, if cycles not zero.
    </doc>
    <argument name = "log" type = "smt_log_t *" />
    <argument name = "cycles" type = "int" />
    <local>
    smt_coremark_t
        *coremark;
    </local>
    //
    if (cycles) {
        coremark = smt_coremark_new ();
        smt_coremark_start (coremark);
        while (cycles--)
            smt_coremark_cycle (coremark);
        smt_coremark_finish (coremark, log);
        smt_coremark_unlink (&coremark);
    }
</method>

<method name = "selftest" />

</class>
