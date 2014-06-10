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
    name     = "smt_demo_portal_front"
    comment  = "Portal front-end - async client wrapper"
    script   = "smt_object_gen"
    target   = "smt"
    abstract = "1"
    >

<inherit class = "smt_object" />

<import class = "smt_demo_portal" />
<import class = "ipr_looseref" />
<import class = "ipr_bucket" />

<context>
    ipr_looseref_list_t
        *smt_demo_portal_list,          //  List of all own portals
        *smt_demo_portal_factory_list;  //  List of all own portal factories
    Bool
        smt_demo_portal_stopped;        //  Is this portal stopped?
</context>

<method name = "new">
    self->smt_demo_portal_list = ipr_looseref_list_new ();
    self->smt_demo_portal_factory_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
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
    </action>
</method>

<method name = "_smt_demo_portal bind" template = "function">
    <doc>
    Binds a smt_demo_portal portal, ready for use.
    </doc>
    <argument name = "smt_demo_portal" type = "smt_demo_portal_t *">Portal reference</argument>
    //
    smt_demo_portal->client_object = self;
    if (smt_demo_portal->server_object)
        smt_demo_portal->client_looseref = ipr_looseref_queue (
            self->smt_demo_portal_list, smt_demo_portal_link (smt_demo_portal));
    else                                //  server_object is null for factories
        smt_demo_portal->client_looseref = ipr_looseref_queue (
            self->smt_demo_portal_factory_list, smt_demo_portal_link (smt_demo_portal));

    //  Build virtual function table for client-side methods
    smt_demo_portal->client_unbind = self__smt_demo_portal_unbind_shim;
    smt_demo_portal->response_print = self_print_shim;
    smt_demo_portal->response_mogrify = self_mogrify_shim;

    //  Signal that portal is ready for use
    smt_demo_portal_ready (smt_demo_portal);
</method>

<!--
    The shims provide a way for us to cast void * into a correctly-typed
    reference that works for passing to async methods, since we cannot make
    async methods that accept a void pointer.
 -->
<method name = "_smt_demo_portal unbind shim" private = "1">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal reference</argument>
    self__smt_demo_portal_unbind (($(selftype) *) self_v, portal);
</method>
<method name = "_smt_demo_portal unbind" template = "async function">
    <doc>
    Client-to-portal unbind.
    </doc>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal reference</argument>
    <action>
    if (portal->client_looseref) {
        ipr_looseref_destroy (&portal->client_looseref);
        smt_demo_portal_unlink (&portal);
    }
    </action>
</method>

<method name = "print shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal back to server</argument>
    <argument name = "string" type = "char *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_print (($(selftype) *) self_v, portal, string);
</method>
<method name = "print" async = "1" template = "function">
    <doc>
    This is the print incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal back to server</argument>
    <argument name = "string" type = "char *" />
    <possess>
    
        string = icl_mem_strdup (string);
        
    </possess>
    <release>
    
        icl_mem_free (string);
        
    </release>
    <action></action>
</method>

<method name = "mogrify shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal back to server</argument>
    <argument name = "string" type = "char *" />
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <argument name = "option" type = "Bool" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_mogrify (($(selftype) *) self_v, portal, string, bucket, option);
</method>
<method name = "mogrify" async = "1" template = "function">
    <doc>
    This is the mogrify incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "smt_demo_portal_t *">Portal back to server</argument>
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
    <action></action>
</method>


<method name = "selftest" />

</class>
