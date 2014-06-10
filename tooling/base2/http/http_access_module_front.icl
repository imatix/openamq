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
    name     = "http_access_module_front"
    comment  = "Portal front-end - async client wrapper"
    script   = "smt_object_gen"
    target   = "smt"
    abstract = "1"
    >

<inherit class = "smt_object" />

<import class = "http_access_module" />
<import class = "ipr_looseref" />
<import class = "http" />

<context>
    ipr_looseref_list_t
        *http_access_module_list,       //  List of all own portals
        *http_access_module_factory_list;  //  List of all own portal factories
    Bool
        http_access_module_stopped;     //  Is this portal stopped?
</context>

<method name = "new">
    self->http_access_module_list = ipr_looseref_list_new ();
    self->http_access_module_factory_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
    {
    http_access_module_t
        *http_access_module;            //  Portal object

    if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
        //  Destroy own portals
        while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
            http_access_module->client_looseref = NULL;
            http_access_module_destroy (&http_access_module);
        }
        ipr_looseref_list_destroy (&self->http_access_module_list);
        //  Destroy own portal factories
        while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_factory_list))) {
            http_access_module->client_looseref = NULL;
            http_access_module_destroy (&http_access_module);
        }
        ipr_looseref_list_destroy (&self->http_access_module_factory_list);
    }
    }
    </action>
</method>

<method name = "_http_access_module bind" template = "function">
    <doc>
    Binds a http_access_module portal, ready for use.
    </doc>
    <argument name = "http_access_module" type = "http_access_module_t *">Portal reference</argument>
    //
    http_access_module->client_object = self;
    if (http_access_module->server_object)
        http_access_module->client_looseref = ipr_looseref_queue (
            self->http_access_module_list, http_access_module_link (http_access_module));
    else                                //  server_object is null for factories
        http_access_module->client_looseref = ipr_looseref_queue (
            self->http_access_module_factory_list, http_access_module_link (http_access_module));

    //  Build virtual function table for client-side methods
    http_access_module->client_unbind = self__http_access_module_unbind_shim;
    http_access_module->response_new_password = self_new_password_shim;

    //  Signal that portal is ready for use
    http_access_module_ready (http_access_module);
</method>

<!--
    The shims provide a way for us to cast void * into a correctly-typed
    reference that works for passing to async methods, since we cannot make
    async methods that accept a void pointer.
 -->
<method name = "_http_access_module unbind shim" private = "1">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal reference</argument>
    self__http_access_module_unbind (($(selftype) *) self_v, portal);
</method>
<method name = "_http_access_module unbind" template = "async function">
    <doc>
    Client-to-portal unbind.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal reference</argument>
    <action>
    if (portal->client_looseref) {
        ipr_looseref_destroy (&portal->client_looseref);
        http_access_module_unlink (&portal);
    }
    </action>
</method>

<method name = "new password shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "key" type = "char *" />
    <argument name = "hash" type = "char *" />
    <argument name = "digest" type = "Bool" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_new_password (($(selftype) *) self_v, portal, key, hash, digest);
</method>
<method name = "new password" async = "1" template = "function">
    <doc>
    This is the new password incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "key" type = "char *" />
    <argument name = "hash" type = "char *" />
    <argument name = "digest" type = "Bool" />
    <possess>
    
        key = icl_mem_strdup (key);
        hash = icl_mem_strdup (hash);
        
    </possess>
    <release>
    
        icl_mem_strfree (&key);
        icl_mem_strfree (&hash);
        
    </release>
    <action></action>
</method>


<method name = "selftest" />

</class>
