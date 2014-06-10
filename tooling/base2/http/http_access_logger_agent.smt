<?xml?>
<!---------------------------------------------------------------------------
    http_access_logger_agent.smt - http_access_logger component

    Logs the result of access control.
    Generated from http_access_logger.icl by smt_object_gen using GSL/4.

    
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
    name = "http_access_logger_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "http_access_logger" />

<method name = "announce">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "log" type = "smt_log_t *" />
</method>

<method name = "after">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "context" type = "http_access_context_t *" />
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "_http_access_module unbind">
    <argument name = "portal" type = "http_access_module_t *" />
</method>

<method name = "open">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "context" type = "http_access_context_t *" />
</method>

<method name = "before">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "context" type = "http_access_context_t *" />
</method>

<method name = "close">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "context" type = "http_access_context_t *" />
</method>

<thread name = "class">
    <context>
        http_access_logger_t
            *http_access_logger;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "http_access_logger_t *">The iCL object</argument>
        tcb->http_access_logger = self;
    </handler>
        
    <state name = "main">

        <method name = "announce"  >
            <action name = "announce">

#define self tcb->http_access_logger
http_access_module_t *
    portal;
smt_log_t *
    log;

    portal = announce_m->portal;
    log = announce_m->log;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "after"  >
            <action name = "after">

#define self tcb->http_access_logger
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = after_m->portal;
    context = after_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->http_access_logger
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->server_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
}
ipr_looseref_list_destroy (&self->http_access_module_list);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_access_logger
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_access_logger object");
    http_access_logger_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_access_logger_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "_http_access_module unbind"  >
            <action name = "_http_access_module unbind">

#define self tcb->http_access_logger
http_access_module_t *
    portal;

    portal = _http_access_module_unbind_m->portal;
        {
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    http_access_module_unlink (&portal);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "open"  >
            <action name = "open">

#define self tcb->http_access_logger
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = open_m->portal;
    context = open_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "before"  >
            <action name = "before">

#define self tcb->http_access_logger
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = before_m->portal;
    context = before_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "close"  >
            <action name = "close">

#define self tcb->http_access_logger
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = close_m->portal;
    context = close_m->context;
        {
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

#define self tcb->http_access_logger
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->server_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
}
ipr_looseref_list_destroy (&self->http_access_module_list);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_access_logger
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_access_logger object");
    http_access_logger_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_access_logger_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "announce" />
        <method name = "after" />
        <method name = "_http_access_module unbind" />
        <method name = "open" />
        <method name = "before" />
        <method name = "close" />
    </state>
</thread>

</agent>
