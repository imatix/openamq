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
    name      = "icl_trace"
    comment   = "iCL trace class"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    Class to enable trace of program behaviour.
    To trace any program operation, call the trace method during operation.
    The trace is replayed via the dump method.
</doc>

<inherit class = "icl_object"  >
    <option name = "alloc"  value = "direct" />
</inherit>

<public>
//  Macros

#define ICL_TRACE_SIZE 256

//  Types

typedef void (icl_trace_dump_fn) (
    icl_os_thread_t thread,
    apr_time_t      time,
    qbyte           info);

//  Structure to hold an individual trace item
    
typedef struct {
    icl_trace_dump_fn
        *dump;                          //  The function to interpret trace
#if (defined (BASE_THREADSAFE))
    apr_os_thread_t
        thread;                         //  The OS thread
#endif
    apr_time_t
        time;                           //  The time of the trace
    qbyte
        info;                           //  The trace information
} icl_trace_item_t;

//  These variables are accessed only from inline functions, so are
//  extern but also named s_ to mark their private nature.

extern icl_trace_t
    *s_icl_trace;
</public>

<private name = "header">
//  Types

icl_trace_t
    *s_icl_trace = NULL;
</private>

<context>
    icl_trace_item_t
        *array;                         //  Array of trace items
    uint
        size,                           //  Total size of trace
        idx;                            //  Current index in trace
</context>

<method name = "new">
    <argument name = "size" type = "uint">Size for this trace</argument>
    <local>
    uint
        idx;
    </local>
    self->array = malloc (size * sizeof (icl_trace_item_t));
    self->size  = size;
    self->idx   = 0;
    
    for (idx = 0; idx < size; idx++) {
        self->array [idx].dump   = NULL;
#if (defined (BASE_THREADSAFE))
        self->array [idx].thread = 0;
#endif
        self->array [idx].info   = 0;
    }
</method>

<method name = "destroy">
    free (self->array);
</method>

<method name = "record" inline = "1">
    <doc>
    Call this method to register an event.
    
    On playback, the dump callback will be called with the
    thread and time of the trace call along with the info.
    </doc>
    <argument name = "self" type = "icl_trace_t *">The trace structure</argument>
    <argument name = "dump" type = "icl_trace_dump_fn *">The function to dump the trace</argument>
    <argument name = "info" type = "qbyte">Thd ID of the event, place or action</argument>
    <local>
    uint
        idx;
    </local>
    if (self == NULL) {
        if (!s_icl_trace)
            s_icl_trace = icl_trace_new (ICL_TRACE_SIZE);
            
        self = s_icl_trace;
    }

    idx = icl_atomic_inc32 ((qbyte*) &self->idx) % self->size;
    self->array[idx].dump = dump;
#if (defined (BASE_THREADSAFE))
    self->array[idx].thread      = apr_os_thread_current ();
#endif
    self->array[idx].time        = apr_time_now ();
    self->array[idx].info        = info;
</method>

<method name = "dump">
    <doc>
    This method dumps the dumped animation trace to the console.
    </doc>
    <argument name = "self" type = "icl_trace_t *">The trace structure</argument>
    <local>
    uint
        trace_idx;
    icl_trace_item_t
        *trace;
    </local>
    if (self == NULL)
        self = s_icl_trace;

    if (self) {
        icl_console_mode (ICL_CONSOLE_DATE,   FALSE);
        icl_console_mode (ICL_CONSOLE_TIME,   TRUE);
        icl_console_mode (ICL_CONSOLE_MICRO,  TRUE);
        icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
        icl_console_print ("--------------------  START OF TRACE  ---------------------");
        trace_idx = (self->idx + 1) % self->size;
        while (trace_idx != self->idx % self->size) {
            trace = &self->array [trace_idx];
            if (trace->dump)
#if (defined (BASE_THREADSAFE))
                (trace->dump) (
                    trace->thread, trace->time, trace->info);
#else
                (trace->dump) (
                    0, trace->time, trace->info);
#endif
            trace_idx = (trace_idx + 1) % self->size;
        }
        icl_console_print ("----------------------  END OF TRACE  ---------------------");
    }
</method>

<method name = "terminate">
    icl_trace_destroy (&s_icl_trace);
</method>

<method name = "selftest">
</method>

</class>

