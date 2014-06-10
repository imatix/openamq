<?xml?>
<!---------------------------------------------------------------------------
    smt_demo_portal_client_agent.smt - smt_demo_portal_client component

    Generated from smt_demo_portal_client.icl by smt_object_gen using GSL/4.

    
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
    name = "smt_demo_portal_client_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "smt_demo_portal_client" />

<method name = "mogrify">
    <argument name = "portal" type = "smt_demo_portal_t *" />
    <argument name = "string" type = "char *" />
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <argument name = "option" type = "Bool" />
    <possess>

    
        string = icl_mem_strdup (string);
        bucket = ipr_bucket_link (bucket);
        
    
    </possess>
    <release>

    
        icl_mem_free (string);
        ipr_bucket_unlink (&bucket);
        
    
    </release>
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "_smt_demo_portal unbind">
    <argument name = "portal" type = "smt_demo_portal_t *" />
</method>

<method name = "print">
    <argument name = "portal" type = "smt_demo_portal_t *" />
    <argument name = "string" type = "char *" />
    <possess>

    
        string = icl_mem_strdup (string);
        
    
    </possess>
    <release>

    
        icl_mem_free (string);
        
    
    </release>
</method>

<thread name = "class">
    <context>
        smt_demo_portal_client_t
            *smt_demo_portal_client;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "smt_demo_portal_client_t *">The iCL object</argument>
        tcb->smt_demo_portal_client = self;
    </handler>
        
    <state name = "main">

        <method name = "mogrify"  >
            <action name = "mogrify">

#define self tcb->smt_demo_portal_client
smt_demo_portal_t *
    portal;
char *
    string;
ipr_bucket_t *
    bucket;
Bool
    option;

    portal = mogrify_m->portal;
    string = mogrify_m->string;
    bucket = mogrify_m->bucket;
    option = mogrify_m->option;
        {
icl_console_print ("CLIENT MOGRIFY: %s", string);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->smt_demo_portal_client
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
smt_demo_portal_t
    *smt_demo_portal;               //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->smt_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (self->smt_demo_portal_list))) {
        smt_demo_portal->client_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
    ipr_looseref_list_destroy (&self->smt_demo_portal_list);
    //  Destroy own portal factories
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (self->smt_demo_portal_factory_list))) {
        smt_demo_portal->client_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
    ipr_looseref_list_destroy (&self->smt_demo_portal_factory_list);
}
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_demo_portal_client
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_demo_portal_client object");
    smt_demo_portal_client_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_demo_portal_client_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "_smt_demo_portal unbind"  >
            <action name = "_smt_demo_portal unbind">

#define self tcb->smt_demo_portal_client
smt_demo_portal_t *
    portal;

    portal = _smt_demo_portal_unbind_m->portal;
        {
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    smt_demo_portal_unlink (&portal);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "print"  >
            <action name = "print">

#define self tcb->smt_demo_portal_client
smt_demo_portal_t *
    portal;
char *
    string;

    portal = print_m->portal;
    string = print_m->string;
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

#define self tcb->smt_demo_portal_client
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
smt_demo_portal_t
    *smt_demo_portal;               //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->smt_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (self->smt_demo_portal_list))) {
        smt_demo_portal->client_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
    ipr_looseref_list_destroy (&self->smt_demo_portal_list);
    //  Destroy own portal factories
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (self->smt_demo_portal_factory_list))) {
        smt_demo_portal->client_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
    ipr_looseref_list_destroy (&self->smt_demo_portal_factory_list);
}
}
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->smt_demo_portal_client
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_demo_portal_client object");
    smt_demo_portal_client_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_demo_portal_client_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "mogrify" />
        <method name = "_smt_demo_portal unbind" />
        <method name = "print" />
    </state>
</thread>

</agent>
