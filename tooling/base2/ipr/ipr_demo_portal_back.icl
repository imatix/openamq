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
    name     = "ipr_demo_portal_back"
    comment  = "Portal back-end - sync server wrapper"
    script   = "icl_gen"
    abstract = "1"
    >

<!-- Force use of links in any case -->
<inherit class = "icl_object">
    <option name = "links" value = "1" />
</inherit>
<inherit class = "icl_init" />

<import class = "ipr_demo_portal" />
<import class = "ipr_bucket" />

<private name = "header">
static ipr_looseref_list_t
    *s_ipr_demo_portal_list;            //  List of portal factories
static Bool
    s_ipr_demo_portal_stopped;          //  Is this portal factory stopped?
</private>

<context>
    ipr_looseref_list_t
        *ipr_demo_portal_list;          //  List of portals for object
    Bool
        ipr_demo_portal_stopped;        //  Is this portal stopped?
</context>

<method name = "new">
    self->ipr_demo_portal_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    ipr_demo_portal_t
        *ipr_demo_portal;               //  Portal object
    </local>
    //
    if (icl_atomic_cas32 ((volatile qbyte *) &self->ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
        while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_list))) {
            ipr_demo_portal->server_looseref = NULL;
            ipr_demo_portal_destroy (&ipr_demo_portal);
        }
    }
    ipr_looseref_list_destroy (&self->ipr_demo_portal_list);
</method>

<method name = "initialise">
    s_ipr_demo_portal_list = ipr_looseref_list_new ();
</method>

<method name = "terminate">
    <local>
    ipr_demo_portal_t
        *ipr_demo_portal;               //  Portal factory object
    </local>
    //
    if (icl_atomic_cas32 ((volatile qbyte *) &s_ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
        while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (s_ipr_demo_portal_list))) {
            ipr_demo_portal->server_looseref = NULL;
            ipr_demo_portal_destroy (&ipr_demo_portal);
        }
    }
    ipr_looseref_list_destroy (&s_ipr_demo_portal_list);
</method>

<method name = "_ipr_demo_portal new" return = "ipr_demo_portal">
    <doc>
    Creates and initialises a new portal instance.  If the _server argument is
    null, creates a hidden server object that can only be referenced through
    the portal.  This is recommended for most purposes.
    </doc>
    <argument name = "self_v" type = "void *">Untyped server reference</argument>
    <argument name = "name" type = "char *" />
    <declare name = "ipr_demo_portal" type = "ipr_demo_portal_t *" />
    <local>
    $(selftype)
        *self;
    </local>
    //
    //  Grab a link to a new or the provided server object
    if (self_v)
        self = self_link (($(selftype) *) self_v);
    else
        self = self_new ();

    ipr_demo_portal = ipr_demo_portal_new (self, self__ipr_demo_portal_link, self__ipr_demo_portal_unlink, name);
    ipr_demo_portal->server_looseref = ipr_looseref_queue (
        self->ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));

    //  Build virtual methods for server-side methods
    ipr_demo_portal->server_unbind = self__ipr_demo_portal_unbind;
    ipr_demo_portal->request_hello = self_hello;
    ipr_demo_portal->request_print = self_print;
    //  Drop link to server object
    self_unlink (&self);
</method>

<method name = "_ipr_demo_portal factory" return = "ipr_demo_portal">
    <doc>
    Creates and initialises a new portal factory instance.
    </doc>
    <argument name = "name" type = "char *" />
    <declare name = "ipr_demo_portal" type = "ipr_demo_portal_t *" />
    //
    if (!s_$(selfname)_active)
        self_initialise ();             //  Since we don't call $(selfname)_new
    ipr_demo_portal = ipr_demo_portal_new (NULL, self__ipr_demo_portal_link, self__ipr_demo_portal_unlink, name);
    ipr_demo_portal->server_looseref = ipr_looseref_queue (
        s_ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));

    //  Build virtual methods for server-side methods
    ipr_demo_portal->server_new = self__ipr_demo_portal_new;
</method>

<method name = "_ipr_demo_portal unbind" private = "1">
    <doc>
    Server-to-portal unbind.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal reference</argument>
    //
    //  Drop link to portal and remove portal from our list of active portals
    if (portal->server_looseref) {
        ipr_looseref_destroy (&portal->server_looseref);
        ipr_demo_portal_unlink (&portal);
    }
</method>

<method name = "hello" private = "1" return = "rc">
    <doc>
    This is the hello incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal back to client</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self;
    </local>
    <header>
    self = ($(selftype) *) self_v;
    </header>
</method>

<method name = "print" private = "1" return = "rc">
    <doc>
    This is the print incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "ipr_demo_portal_t *">Portal back to client</argument>
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

<!-- These two methods provide untyped link/unlink methods that can be
     called from the portal to work on the server object.
  -->
<method name = "_ipr_demo_portal link" return = "object" private = "1">
    <argument name = "object" type = "void *">Reference cast as a void *</argument>
    //
    object = (void *) $(selfname)_link ((($(selftype) *) object));
</method>

<method name = "_ipr_demo_portal unlink" private = "1">
    <argument name = "object_p" type = "void *">Reference pointer cast as a void *</argument>
    //
    $(selfname)_unlink ((($(selftype) **) object_p));
</method>

<method name = "selftest" />

</class>
