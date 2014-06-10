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
    name     = "ipr_demo_portal_front"
    comment  = "Portal front-end - sync client wrapper"
    script   = "icl_gen"
    abstract = "1"
    >

<!-- Force use of links in any case -->
<inherit class = "icl_object">
    <option name = "links" value = "1" />
</inherit>

<import class = "ipr_demo_portal" />
<import class = "ipr_looseref" />
<import class = "ipr_bucket" />

<context>
    ipr_looseref_list_t
        *ipr_demo_portal_list,          //  List of all own portals
        *ipr_demo_portal_factory_list;  //  List of all own portal factories
    Bool
        ipr_demo_portal_stopped;        //  Is this portal stopped?
</context>

<method name = "new">
    self->ipr_demo_portal_list = ipr_looseref_list_new ();
    self->ipr_demo_portal_factory_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    ipr_demo_portal_t
        *ipr_demo_portal;               //  Portal object
    </local>
    //
    if (icl_atomic_cas32 ((volatile qbyte *) &self->ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
        //  Destroy own portals
        while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_list))) {
            ipr_demo_portal->client_looseref = NULL;
            ipr_demo_portal_destroy (&ipr_demo_portal);
        }
        ipr_looseref_list_destroy (&self->ipr_demo_portal_list);
        //  Destroy own portal factories
        while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_factory_list))) {
            ipr_demo_portal->client_looseref = NULL;
            ipr_demo_portal_destroy (&ipr_demo_portal);
        }
        ipr_looseref_list_destroy (&self->ipr_demo_portal_factory_list);
    }
</method>

<method name = "_ipr_demo_portal bind" template = "function">
    <doc>
    Binds a ipr_demo_portal portal, ready for use. This method is for use
    outside the ipr_demo_portal class.
    </doc>
    <argument name = "ipr_demo_portal" type = "ipr_demo_portal_t *">Portal reference</argument>
    //
    ipr_demo_portal->client_object = self;
    if (ipr_demo_portal->server_object)
        ipr_demo_portal->client_looseref = ipr_looseref_queue (
            self->ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));
    else                                //  server_object is null for factories
        ipr_demo_portal->client_looseref = ipr_looseref_queue (
            self->ipr_demo_portal_factory_list, ipr_demo_portal_link (ipr_demo_portal));

    //  Build virtual function table for client-side methods
    ipr_demo_portal->client_unbind = self__ipr_demo_portal_unbind;
    ipr_demo_portal->response_print = self_print;
    ipr_demo_portal->response_mogrify = self_mogrify;

    //  Signal that portal is ready for use
    ipr_demo_portal_ready (ipr_demo_portal);
</method>

<method name = "_ipr_demo_portal unbind" private = "1">
    <doc>
    Client-to-portal unbind.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal reference</argument>
    //
    //  Drop link to portal and remove portal from our list of active portals
    if (portal->client_looseref) {
        ipr_looseref_destroy (&portal->client_looseref);
        ipr_demo_portal_unlink (&portal);
    }
</method>

<method name = "print" private = "1" return = "rc">
    <doc>
    This is the print incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal back to server</argument>
    <argument name = "string" type = "char *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self;
    </local>
    <header>
    self = ($(selftype) *) self_v;
    </header>
</method>

<method name = "mogrify" private = "1" return = "rc">
    <doc>
    This is the mogrify incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal back to server</argument>
    <argument name = "string" type = "char *" />
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <argument name = "option" type = "Bool" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self;
    </local>
    <header>
    self = ($(selftype) *) self_v;
    </header>
</method>


<method name = "selftest" />

</class>
