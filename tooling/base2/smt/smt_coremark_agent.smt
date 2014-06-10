<?xml?>
<!---------------------------------------------------------------------------
    smt_coremark_agent.smt - smt_coremark component

    CoreMark is the Base2 performance unit for asynchronous multithreaded
    servers.  It measures the event-processing capacity of a single CPU core
    which can be multiplied by the number of available cores to give an
    overall server capacity.

    To use, send a start, then N cycles (10k-1M) and then a finish.
    Generated from smt_coremark.icl by smt_object_gen using GSL/4.

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
 ----------------------------------------------------------------------------->

<agent
    name = "smt_coremark_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "smt_coremark" />

<method name = "start">
</method>

<method name = "cycle">
</method>

<method name = "finish">
    <argument name = "log" type = "smt_log_t *" />
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<thread name = "class">
    <context>
        smt_coremark_t
            *smt_coremark;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "smt_coremark_t *">The iCL object</argument>
        tcb->smt_coremark = self;
    </handler>
        
    <state name = "main">

        <method name = "start"  >
            <action name = "start">

#define self tcb->smt_coremark

        {
self->started = apr_time_now ();
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "cycle"  >
            <action name = "cycle">

#define self tcb->smt_coremark

        {
self->cycles++;
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "finish"  >
            <action name = "finish">

#define self tcb->smt_coremark
smt_log_t *
    log;

    log = finish_m->log;
        {
double
    capacity = (double) (self->cycles / (double) (apr_time_now () - self->started));

smt_log_print (log, "I: CoreMark performance ~%d000 events/second", (int) (capacity * 1000));
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "free">

#define self tcb->smt_coremark
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_coremark object");
    smt_coremark_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_coremark_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <event name = "shutdown" nextstate = "shutting down" />
    </state>
    
    <state name = "shutting down">

        <method name = "destroy"  nextstate = ""  >
            <action name = "free">

#define self tcb->smt_coremark
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_coremark object");
    smt_coremark_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_coremark_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "start" />
        <method name = "cycle" />
        <method name = "finish" />
    </state>
</thread>

</agent>
