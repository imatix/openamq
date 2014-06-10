<?xml?>
<!---------------------------------------------------------------------------
    smt_log_agent.smt - smt_log component

    Defines a log. This is a lock-free asynchronous class.
    Generated from smt_log.icl by smt_object_gen using GSL/4.

    
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
    name = "smt_log_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "smt_log" />

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "open">
    <argument name = "filename" type = "char *" />
    <argument name = "log_path" type = "char *" />
    <argument name = "cycle_cmd" type = "char *" />
    <possess>

    filename = icl_mem_strdup (filename);
    log_path = icl_mem_strdup (log_path);
    cycle_cmd = icl_mem_strdup (cycle_cmd);
    
    </possess>
    <release>

    icl_mem_free (filename);
    icl_mem_free (log_path);
    icl_mem_free (cycle_cmd);
    
    </release>
</method>

<method name = "close">
</method>

<method name = "set syslog">
    <argument name = "priority" type = "int" />
</method>

<method name = "write bucket">
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <possess>

    bucket = ipr_bucket_link (bucket);
    
    </possess>
    <release>

    ipr_bucket_unlink (&bucket);
    
    </release>
</method>

<method name = "cycle">
</method>

<thread name = "class">
    <context>
        smt_log_t
            *smt_log;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "smt_log_t *">The iCL object</argument>
        tcb->smt_log = self;
    </handler>
        
    <state name = "main">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_log object");
    smt_log_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_log_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "open"  >
            <action name = "open">

#define self tcb->smt_log
char *
    filename;
char *
    log_path;
char *
    cycle_cmd;

    filename = open_m->filename;
    log_path = open_m->log_path;
    cycle_cmd = open_m->cycle_cmd;
        {
icl_shortstr_cpy (self->filename,  filename);
icl_shortstr_cpy (self->log_path,  log_path);
icl_shortstr_cpy (self->cycle_cmd, cycle_cmd);

//  Ensure that logging directories exist
if (*self->log_path && !ipr_file_is_directory (self->log_path))
    ipr_dir_create (self->log_path);

if (ipr_file_is_writeable (self->log_path)) {
    ipr_file_set_path (self->filename, self->log_path);

    //  Archive the previous log file if exists
    ipr_log_cycle (self->filename, NULL, self->cycle_cmd);
    self->output = fopen (self->filename, "w");
    if (!self->output)
        icl_console_print ("E: can't open %s: %s", self->filename, strerror (errno));
}
else
    icl_console_print ("E: cannot write to '%s/%s' - discarding data", self->log_path, filename);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "close"  >
            <action name = "close">

#define self tcb->smt_log

        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
    self->output = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "set syslog"  >
            <action name = "set syslog">

#define self tcb->smt_log
int
    priority;

    priority = set_syslog_m->priority;
        {
assert (!self->output);
self->tosyslog = TRUE;
self->priority = priority;
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "write bucket"  >
            <action name = "write bucket">

#define self tcb->smt_log
ipr_bucket_t *
    bucket;

    bucket = write_bucket_m->bucket;
        {
if (self->tosyslog)
    ipr_log_sysprint (self->priority, (char *) bucket->data);
else
if (self->output) {
    //  Always printf into %s to guard against %'s in data
    fprintf (self->output, "%s", (char *) (bucket->data));
    fprintf (self->output, "\n");
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "cycle"  >
            <action name = "cycle">

#define self tcb->smt_log

        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
}
//  Archive the previous log file if exists
ipr_log_cycle (self->filename, NULL, self->cycle_cmd);
self->output = fopen (self->filename, "w");
if (!self->output)
    icl_console_print ("E: can't open %s: %s", self->filename, strerror (errno));
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
            <action name = "destroy">

#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_log object");
    smt_log_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_log_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "open" />
        <method name = "close" />
        <method name = "set syslog" />
        <method name = "write bucket" />
        <method name = "cycle" />
    </state>
</thread>

</agent>
