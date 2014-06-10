<?xml?>
<!---------------------------------------------------------------------------
    http_driver_null_agent.smt - http_driver_null component

    This class implements a driver module to ignore all methods.  This
    is used for testing stability in the server and clients.
    Generated from http_driver_null.icl by smt_object_gen using GSL/4.

    
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
    name = "http_driver_null_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "http_driver_null" />

<method name = "announce">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "log" type = "smt_log_t *" />
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "_http_driver_module unbind">
    <argument name = "portal" type = "http_driver_module_t *" />
</method>

<method name = "get">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "head">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "post">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "put">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "delete">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "move">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<method name = "copy">
    <argument name = "portal" type = "http_driver_module_t *" />
    <argument name = "context" type = "http_driver_context_t *" />
    <possess>

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    </possess>
    <release>

    
        http_driver_context_unlink (&context);
        
    
    </release>
</method>

<thread name = "class">
    <context>
        http_driver_null_t
            *http_driver_null;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "http_driver_null_t *">The iCL object</argument>
        tcb->http_driver_null = self;
    </handler>
        
    <state name = "main">

        <method name = "announce"  >
            <action name = "announce">

#define self tcb->http_driver_null
http_driver_module_t *
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


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_driver_module_t
    *http_driver_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->server_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
}
ipr_looseref_list_destroy (&self->http_driver_module_list);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_driver_null object");
    http_driver_null_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_driver_null_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "_http_driver_module unbind"  >
            <action name = "_http_driver_module unbind">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;

    portal = _http_driver_module_unbind_m->portal;
        {
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    http_driver_module_unlink (&portal);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "get"  >
            <action name = "get">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = get_m->portal;
    context = get_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "head"  >
            <action name = "head">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = head_m->portal;
    context = head_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "post"  >
            <action name = "post">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = post_m->portal;
    context = post_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "put"  >
            <action name = "put">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = put_m->portal;
    context = put_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "delete"  >
            <action name = "delete">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = delete_m->portal;
    context = delete_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "move"  >
            <action name = "move">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = move_m->portal;
    context = move_m->context;
        {
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "copy"  >
            <action name = "copy">

#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = copy_m->portal;
    context = copy_m->context;
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

#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_driver_module_t
    *http_driver_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->server_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
}
ipr_looseref_list_destroy (&self->http_driver_module_list);
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_driver_null object");
    http_driver_null_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_driver_null_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "announce" />
        <method name = "_http_driver_module unbind" />
        <method name = "get" />
        <method name = "head" />
        <method name = "post" />
        <method name = "put" />
        <method name = "delete" />
        <method name = "move" />
        <method name = "copy" />
    </state>
</thread>

</agent>
