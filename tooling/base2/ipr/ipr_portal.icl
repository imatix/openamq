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
    name     = "ipr_portal"
    comment  = "Portal abstract base class"
    script   = "icl_gen"
    abstract = "1"
    >
<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "links"  value = "1" />
    <option name = "rwlock" value = "1" />
</inherit>

<import class = "ipr_looseref" />
<invoke script = "ipr_portal" />

<public name = "header">
#ifndef IPR_UNTYPED_LINK
#define IPR_UNTYPED_LINK
    typedef void * (untyped_link_fn) (void *object);
    typedef void (untyped_unlink_fn) (void *object_p);
#endif
</public>

<context>
    Bool
        bound,                          //  Has portal been bound?
        stopped;                        //  Has portal been stopped?

    //  Server-side state glue
    void
        *server_object;                 //  If null, is a factory
    ipr_looseref_t
        *server_looseref;               //  Where server holds portal reference
    //  Server virtual methods
    void (*server_unlink)
        (void *server);
    void (*server_unbind)
        (void *server, $(selftype) *portal);

    //  Client-side state glue
    void
        *client_object;                 //  Client object reference
    ipr_looseref_t
        *client_looseref;               //  Where client holds portal reference
    void (*client_unbind)
        (void *client, $(selftype) *portal);
</context>

<method name = "new">
    <argument name = "server object" type = "void *" />
    <argument name = "server link"   type = "untyped_link_fn *" />
    <argument name = "server unlink" type = "untyped_unlink_fn *" />
    //
    self->server_object = (server_link) (server_object);
    self->server_unlink = server_unlink;
</method>

<method name = "destroy">
    if (icl_atomic_cas32 ((volatile qbyte *) &self->stopped, TRUE, FALSE) == FALSE) {
        self->bound = FALSE;
        (self->client_unbind) (self->client_object, self);
        if (self->server_object) {
            (self->server_unbind) (self->server_object, self);
            (self->server_unlink) (&self->server_object);
        }
    }
</method>

<method name = "ready" template = "function">
    <doc>
    Signals to the server that the portal has been bound by the client and
    is now ready for use.
    </doc>
    //
    self->bound = TRUE;
</method>

<method name = "selftest" />

</class>
