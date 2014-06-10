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
    name     = "http_access_module_back"
    comment  = "Portal back-end - async server wrapper"
    script   = "smt_object_gen"
    target   = "smt"
    abstract = "1"
    >

<inherit class = "smt_object" />
<inherit class = "icl_init" />

<import class = "http_access_module" />
<import class = "http" />

<private name = "header">
static ipr_looseref_list_t
    *s_http_access_module_list;         //  List of portal factories
static Bool
    s_http_access_module_stopped;       //  Is this portal factory stopped?
</private>

<context>
    ipr_looseref_list_t
        *http_access_module_list;       //  List of portals
    Bool
        http_access_module_stopped;     //  Is this portal stopped?
</context>

<method name = "new">
    self->http_access_module_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
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
    </action>
</method>

<method name = "initialise">
    s_http_access_module_list = ipr_looseref_list_new ();
</method>

<method name = "terminate">
    <local>
    http_access_module_t
        *http_access_module;            //  Portal factory object
    </local>
    //
    if (icl_atomic_cas32 ((volatile qbyte *) &s_http_access_module_stopped, TRUE, FALSE) == FALSE) {
        while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (s_http_access_module_list))) {
            http_access_module->server_looseref = NULL;
            http_access_module_destroy (&http_access_module);
        }
    }
    ipr_looseref_list_destroy (&s_http_access_module_list);
</method>

<method name = "_http_access_module new" return = "http_access_module">
    <doc>
    Creates and initialises a new portal instance.  If the server argument is
    null, creates a hidden server object that can only be referenced through
    the portal.  This is recommended for most purposes.
    </doc>
    <argument name = "self_v" type = "void *">Untyped server reference</argument>
    <declare name = "http_access_module" type = "http_access_module_t *" />
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

    http_access_module = http_access_module_new (self, self__http_access_module_link, self__http_access_module_unlink);
    http_access_module->server_looseref = ipr_looseref_queue (
        self->http_access_module_list, http_access_module_link (http_access_module));

    //  Build virtual methods for server-side methods
    http_access_module->server_unbind = self__http_access_module_unbind_shim;
    http_access_module->request_announce = self_announce_shim;
    http_access_module->request_open = self_open_shim;
    http_access_module->request_before = self_before_shim;
    http_access_module->request_after = self_after_shim;
    http_access_module->request_close = self_close_shim;

    //  Drop link to server object
    self_unlink (&self);
</method>

<method name = "_http_access_module factory" return = "http_access_module">
    <doc>
    Creates and initialises a new portal factory instance.
    </doc>
    <declare name = "http_access_module" type = "http_access_module_t *" />
    //
    if (!s_$(selfname)_active)
        self_initialise ();             //  Since we don't call $(selfname)_new
    http_access_module = http_access_module_new (NULL, self__http_access_module_link, self__http_access_module_unlink);
    http_access_module->server_looseref = ipr_looseref_queue (
        s_http_access_module_list, http_access_module_link (http_access_module));

    //  Build virtual methods for server-side methods
    http_access_module->server_new = self__http_access_module_new;
</method>

<method name = "_http_access_module unbind shim" private = "1">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal reference</argument>
    self__http_access_module_unbind (($(selftype) *) self_v, portal);
</method>
<method name = "_http_access_module unbind" template = "async function">
    <doc>
    Server-to-portal unbind.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal reference</argument>
    //
    <action>
    if (portal->server_looseref) {
        ipr_looseref_destroy (&portal->server_looseref);
        http_access_module_unlink (&portal);
    }
    </action>
</method>

<method name = "announce shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "log" type = "smt_log_t *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_announce (($(selftype) *) self_v, portal, log);
</method>
<method name = "announce" async = "1" template = "function">
    <doc>
    This is the announce incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to client</argument>
    <argument name = "log" type = "smt_log_t *" />
    <action></action>
</method>

<method name = "open shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_open (($(selftype) *) self_v, portal, context);
</method>
<method name = "open" async = "1" template = "function">
    <doc>
    This is the open incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to client</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <action></action>
</method>

<method name = "before shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_before (($(selftype) *) self_v, portal, context);
</method>
<method name = "before" async = "1" template = "function">
    <doc>
    This is the before incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to client</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <action></action>
</method>

<method name = "after shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_after (($(selftype) *) self_v, portal, context);
</method>
<method name = "after" async = "1" template = "function">
    <doc>
    This is the after incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to client</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <action></action>
</method>

<method name = "close shim" private = "1" return = "rc">
    <argument name = "self_v" type = "void *">Self as void pointer</argument>
    <argument name = "portal" type = "http_access_module_t *">Portal back to server</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    rc = self_close (($(selftype) *) self_v, portal, context);
</method>
<method name = "close" async = "1" template = "function">
    <doc>
    This is the close incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    </doc>
    <argument name = "portal" type = "http_access_module_t *">Portal back to client</argument>
    <argument name = "context" type = "http_access_context_t *" />
    <action></action>
</method>

<!-- These two methods provide untyped link/unlink methods that can be
     called from the portal to work on the server object.
  -->
<method name = "_http_access_module link" return = "object" private = "1">
    <argument name = "object" type = "void *">Reference cast as a void *</argument>
    //
    object = (void *) $(selfname)_link ((($(selftype) *) object));
</method>

<method name = "_http_access_module unlink" private = "1">
    <argument name = "object_p" type = "void *">Reference pointer cast as a void *</argument>
    //
    $(selfname)_unlink ((($(selftype) **) object_p));
</method>

<method name = "selftest" />

</class>
