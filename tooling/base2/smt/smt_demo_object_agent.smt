<?xml?>
<!---------------------------------------------------------------------------
    smt_demo_object_agent.smt - smt_demo_object component

This object is a demonstration of SMT objects.
    Generated from smt_demo_object.icl by smt_object_gen using GSL/4.

    
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
    name = "smt_demo_object_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "smt_demo_object" />

<method name = "print">
    <argument name = "prefix" type = "char *" />
    <possess>

    prefix = icl_mem_strdup (prefix);
    
    </possess>
    <release>

    icl_mem_free (prefix);
    
    </release>
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<thread name = "class">
    <context>
        smt_demo_object_t
            *smt_demo_object;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "smt_demo_object_t *">The iCL object</argument>
        tcb->smt_demo_object = self;
    </handler>
        
    <state name = "main">

        <method name = "print"  >
            <action name = "print">

#define self tcb->smt_demo_object
char *
    prefix;

    prefix = print_m->prefix;
        {
//    icl_console_print ("%s: %s", prefix, self->string);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->smt_demo_object
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
icl_mem_free (self->string);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_demo_object
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_demo_object object");
    smt_demo_object_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_demo_object_free (self);
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
            <action name = "destroy">

#define self tcb->smt_demo_object
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
icl_mem_free (self->string);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_demo_object
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_demo_object object");
    smt_demo_object_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_demo_object_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "print" />
    </state>
</thread>

</agent>
