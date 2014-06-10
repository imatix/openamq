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
    name    = "http_driver_module"
    comment = "HTTP driver module portal"
    script  = "icl_gen"
    >
<doc>
    This class enables the creation of plug-in modules that handle specific
    URI paths for the server.  The plug-in accepts HTTP methods and an
    http_request object and returns a http_response object.
</doc>

<inherit class = "ipr_portal">
    <option name = "front_end" value = "async" />
    <option name = "back_end" value = "async" />
</inherit>

<import class = "http" />

<context>
    icl_shortstr_t
        path;                           //  Plugin path specification

    //  add file name pattern
    //  add mime type pattern
</context>

<data>
    <request name = "announce">
        <field name = "log" type = "smt_log_t *" />
    </request>

    <request name = "driver request" abstract = "1">
        <field name = "context" type = "http_driver_context_t *" />
        <possess>
        assert (context);
        context = http_driver_context_link (context);
        </possess>
        <release>
        http_driver_context_unlink (&context);
        </release>
    </request>

    <request name = "get"    template = "driver request" />
    <request name = "head"   template = "driver request" />
    <request name = "post"   template = "driver request" />
    <request name = "put"    template = "driver request" />
    <request name = "delete" template = "driver request" />
    <request name = "move"   template = "driver request" />
    <request name = "copy"   template = "driver request" />
</data>

<method name = "new">
    <argument name = "path" type = "char *" />
    icl_shortstr_cpy (self->path, path);
</method>

</class>
