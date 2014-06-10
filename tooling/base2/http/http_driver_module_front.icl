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
    name     = "http_driver_module_front"
    comment  = "Portal front-end - async client wrapper"
    script   = "smt_object_gen"
    target   = "smt"
    abstract = "1"
    >

<inherit class = "smt_object" />

<import class = "http_driver_module" />
<import class = "ipr_looseref" />
<import class = "http" />

<context>
    ipr_looseref_list_t
        *http_driver_module_list,       //  List of all own portals
        *http_driver_module_factory_list;  //  List of all own portal factories
    Bool
        http_driver_module_stopped;     //  Is this portal stopped?
</context>

<method name = "new">
    self->http_driver_module_list = ipr_looseref_list_new ();
    self->http_driver_module_factory_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
    {
    http_driver_module_t
        *http_driver_module;            //  Portal object

    if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
        //  Destroy own portals
        while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
            http_driver_module->client_looseref = NULL;
            http_driver_module_destroy (&http_driver_module);
        }
        ipr_looseref_list_destroy (&self->http_driver_module_list);
        //  Destroy own portal factories
        while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_factory_list))) {
            http_driver_module->client_looseref = NULL;
            http_driver_module_destroy (&http_driver_module);
        }
        ipr_looseref_list_destroy (&self->http_driver_module_factory_list);
    }
    }
    </action>
</method>

<method name = "_http_driver_module bind" template = "function">
    <doc>
    Binds a http_driver_module portal, ready for use.
    </doc>
    <argument name = "http_driver_module" type = "http_driver_module_t *">Portal reference</argument>
    //
    http_driver_module->client_object = self;
    if (http_driver_module->server_object)
        http_driver_module->client_looseref = ipr_looseref_queue (
            self->http_driver_module_list, http_driver_module_link (http_driver_module));
    else                                //  server_object is null for factories
        http_driver_module->client_looseref = ipr_looseref_queue (
            self->http_driver_module_factory_list, http_driver_module_link (http_driver_module));

    //  Build virtual function table for client-side methods
    http_driver_module->client_unbind = self__http_driver_module_unbind_shim;

    //  Signal that portal is ready for use
    http_driver_module_ready (http_driver_module);
</method>

<!--
    The shims provide a way for us to cast void * into a correctly-typed
    reference that works for passing to async methods, since we cannot make
    async methods that accept a void pointer.
 -->
<method name = "_http_driver_module unbind shim" private = "1">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_driver_module_t *">Portal reference</argument>
    self__http_driver_module_unbind (($(selftype) *) self_v, portal);
</method>
<method name = "_http_driver_module unbind" template = "async function">
    <doc>
    Client-to-portal unbind.
    </doc>
    <argument name = "portal" type = "http_driver_module_t *">Portal reference</argument>
    <action>
    if (portal->client_looseref) {
        ipr_looseref_destroy (&portal->client_looseref);
        http_driver_module_unlink (&portal);
    }
    </action>
</method>


<method name = "selftest" />

</class>
