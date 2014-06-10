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
    name    = "smt_demo_portal"
    comment = "This is a demo portal"
    script  = "icl_gen"
    >

<inherit class = "ipr_portal">
    <!-- Generate an async-to-async portal -->
    <option name = "front_end" value = "async" />
    <option name = "back_end" value = "async" />
</inherit>

<import class = "ipr_bucket" />

<context>
    icl_shortstr_t
        name;
</context>

<method name = "new">
    <argument name = "name" type = "char *" />
    icl_shortstr_cpy (self->name, name);
</method>

<data>
    <!-- Requests from client to server -->
    <request name = "hello" />
    <request name = "print">
        <field name = "string" type = "char *" />
        <possess>
        string = icl_mem_strdup (string);
        </possess>
        <release>
        icl_mem_free (string);
        </release>
    </request>

    <!-- Responses from server to client -->
    <response name = "print">
        <field name = "string" type = "char *" />
        <possess>
        string = icl_mem_strdup (string);
        </possess>
        <release>
        icl_mem_free (string);
        </release>
    </response>
    <response name = "mogrify">
        <field name = "string" type = "char *" />
        <field name = "bucket" type = "ipr_bucket_t *" />
        <field name = "option" type = "Bool" />
        <possess>
        string = icl_mem_strdup (string);
        bucket = ipr_bucket_link (bucket);
        </possess>
        <release>
        icl_mem_free (string);
        ipr_bucket_unlink (&bucket);
        </release>
    </response>
</data>

</class>
