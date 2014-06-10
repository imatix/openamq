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
    name    = "http_access_module"
    comment = "HTTP access module portal"
    script  = "icl_gen"
    >
<doc>
    This class enables the creation of access modules that implement access
    control on an HTTP request.  The server executes all registered access
    modules in order, before and after executing a request.  Following the
    before method, if the response reply code is not HTTP_REPLY_OK the
    request will not be passed to a plugin to handle.  Note that while the
    back-end is an async object, the before & after methods are synch.  If
    you want to do async work in these methods, use a further internal
    async method.
</doc>

<inherit class = "ipr_portal">
    <option name = "front_end" value = "async" />
    <option name = "back_end" value = "async" />
</inherit>

<import class = "http" />

<public name = "header">
//  These are the values an access module can return by setting 'rc'
//  There is no option to stop executing access modules and send a response
//  back to the browser immediately, this would bypass logging and other
//  essential access modules.
//
//  Continue executing the remaining access modules, default return code.
//  Allowed on: open, before, after, close
#define HTTP_ACCESS_CONTINUE        0

//  Abort the current connection, do not send a response.  This could be
//  used to respond to hostile attacks on the server.  The connection aborts
//  after a delay configured via the --abort_delay option.
//  Allowed on: open, before, after
#define HTTP_ACCESS_ABORT           1

//  Wait a short time and then retry the access modules.  This could be
//  used when the access module is expecting new authentication data.
//  Allowed on: before
#define HTTP_ACCESS_RETRY           2
</public>

<data>
    <request name = "announce">
        <field name = "log" type = "smt_log_t *" />
    </request>
    <request name = "open">
        <doc>
        Open a new incoming connection.
        </doc>
        <field name = "context" type = "http_access_context_t *" />
    </request>
    <request name = "before">
        <doc>
        Check before executing a request.
        </doc>
        <field name = "context" type = "http_access_context_t *" />
    </request>
    <request name = "after">
        <doc>
        Check after executing a request.
        </doc>
        <field name = "context" type = "http_access_context_t *" />
    </request>
    <request name = "close">
        <doc>
        Close a connection.
        </doc>
        <field name = "context" type = "http_access_context_t *" />
    </request>

    <response name = "new password">
        <doc>
        Updates one password entry.  This method can be used by objects that get
        updated password hashes from some source, e.g. an access refresh operation.
        Note that the http_access_refresh module updates password files externally
        (an external process modifies the files, and this thread reloads them) and
        this method updates them internally (updates its tables and then saves to
        a file); the two mechanisms should not be mixed or password data could get
        lost.
        </doc>
        <field name = "key"  type = "char *">Password key</field>
        <field name = "hash" type = "char *">Password hash</field>
        <field name = "digest" type = "Bool">Digest authentication?</field>
        <possess>
        key = icl_mem_strdup (key);
        hash = icl_mem_strdup (hash);
        </possess>
        <release>
        icl_mem_strfree (&key);
        icl_mem_strfree (&hash);
        </release>
    </response>
</data>

</class>
