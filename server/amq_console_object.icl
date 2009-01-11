<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name     = "amq_console_object"
    comment  = "AMQ Console object base class"
    version  = "1.0"
    script   = "smt_object_gen"
    target   = "smt"
    abstract = "1"
    >
<doc>
This class turns an icl-CML definition into the methods needed to make
an object work with the AMQ Console (amq.console system exchange).
</doc>

<invoke>
.if count (global.top.data) = 0
.   abort "E: no CML data definitions for console object"
.endif
.for global.top->data.class
.   global.top.console_class = name
.   if defined (parent)
.       global.top.parent_id = "$(parent)->object_id"
.   else
.       global.top.parent_id = "0"
.   endif
.else
.   abort "E: missing CML definition for $(class.name)"
.endfor
</invoke>

<inherit class = "icl_init" />

<import class = "asl_field_list" />

<context>
    qbyte
        object_id;                      //  System-wide object id
    amq_console_t
        *console;                       //  Reference to console
</context>

<private name = "header">
static amq_console_class_t
    *s_class;                           //  Class descriptor
</private>

<method name = "new">
    <header>
    self->console = amq_console_link (amq_console);
    self->object_id = icl_atomic_inc32 ((volatile qbyte *) &amq_object_id);
    amq_console_register (self->console, self->object_id, self_link (self), s_class, $(parent_id));
    </header>
</method>

<method name = "destroy">
    <action>
    amq_console_cancel (self->console, self->object_id);
    amq_console_unlink (&self->console);
    </action>
</method>

<method name = "inspect shim" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <declare name = "rc" type = "int" default = "0" />
    self_inspect (($(selftype) *) (self_v), request);
</method>

<method name = "inspect" template = "async function">
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <possess>
    request = amq_content_basic_link (request);
    </possess>
    <release>
    amq_content_basic_unlink (&request);
    </release>
    <action>
    asl_field_list_t
        *fields;                        //  Properties of object
    icl_shortstr_t
        field_value;
.for global.top->data->class.field
.   for local where (0.name?"get") = "get"
    $(string.trim (local.?''))
.   endfor
.endfor
.for global.top->data->class.class
.   for local where (0.name?"get") = "get"
    $(string.trim (local.?''))
.   endfor
.endfor

    fields = asl_field_list_new (NULL);
.for global.top->data->class.field
.   for header where (0.name?"get") = "get"
    $(string.trim (header.?''))
.   endfor
.endfor
.for global.top->data->class.field
.   for get
    $(string.trim (get.?''))
.   endfor
    asl_field_new_string (fields, "$(name)", field_value);
.endfor
.for global.top->data->class.class
.   for get
    strclr (field_value);
    $(string.trim (get.?''))
.   endfor
.   if repeat ?= 1
    while (*field_value) {
        asl_field_new_string (fields, "$(name)", field_value);
        strclr (field_value);
.       for next
    $(string.trim (next.?''))
.       endfor
    }
.   else
    asl_field_new_string (fields, "$(name)", field_value);
.   endif
.endfor
.for global.top->data->class.field
.   for footer where (0.name?"get") = "get"
    $(string.trim (footer.?''))
.   endfor
.endfor
    amq_console_reply_ok (amq_console, "inspect-reply", request, self->object_id, fields, NULL);
    asl_field_list_unlink (&fields);
    </action>
</method>
<method name = "modify shim" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields"  type = "asl_field_list_t *">Fields to modify</argument>
    <declare name = "rc" type = "int" default = "0" />
    self_modify (($(selftype) *) (self_v), request, fields);
</method>

<method name = "modify" template = "async function">
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields"  type = "asl_field_list_t *">Fields to modify</argument>
    <possess>
    request = amq_content_basic_link (request);
    asl_field_list_link (fields);
    </possess>
    <release>
    amq_content_basic_unlink (&request);
    asl_field_list_unlink (&fields);
    </release>
    <action>
.for global.top->data->class.field where count (put)
    asl_field_t
        *field;
    icl_shortstr_t
        field_value;
.   last
.endfor
.for global.top->data->class.field
.   for local where (0.name?"put") = "put"
    $(string.trim (local.?''))
.   endfor
.endfor
    char
        *notice_text = NULL;            //  Notice to UI, if any

.for global.top->data->class.field
.   for header where (0.name?"put") = "put"
    $(string.trim (header.?''))
.   endfor
.endfor
.for global.top->data->class.field
.   for put
    field = asl_field_list_search (fields, "$(name)");
    if (field) {
        icl_shortstr_cpy (field_value, asl_field_string (field));
        $(string.trim (.))
        asl_field_unlink (&field);
    }
.   endfor
.endfor
.for global.top->data->class.field
.   for footer where (0.name?"put") = "put"
    $(string.trim (footer.?''))
.   endfor
.endfor
    amq_console_reply_ok (
        amq_console, "modify-reply", request, self->object_id, NULL, notice_text);
    </action>
</method>

<method name = "method shim" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "method name" type = "char *">Method name</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields"  type = "asl_field_list_t *">Fields to modify</argument>
    <declare name = "rc" type = "int" default = "0" />
    self_method (($(selftype) *) (self_v), method_name, request, fields);
</method>

<method name = "method" template = "async function">
    <argument name = "method name" type = "char *">Method name</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields" type = "asl_field_list_t *">Argument fields</argument>
    <possess>
    method_name = icl_mem_strdup (method_name);
    request = amq_content_basic_link (request);
    fields  = asl_field_list_link (fields);
    </possess>
    <release>
    icl_mem_free (method_name);
    amq_content_basic_unlink (&request);
    asl_field_list_unlink (&fields);
    </release>
    <action>
    int
        rc = 0;
    char
        *notice_text = NULL;            //  Notice to UI, if any
.for global.top->data->class.method
.   for local
    $(string.trim (local.?''))
.   endfor
.endfor
.for global.top->data->class.method
    if (streq (method_name, "$(method.name)")) {
.   for field
.       field.type ?= "string"
.       if first ()
        asl_field_t
            *field;
.       endif
.       if type = "string"
        icl_shortstr_t
            $(name);
.       elsif type = "int"
        qbyte
            $(name) = 0;
.       elsif type = "bool"
        Bool
            $(name) = FALSE;
.       elsif type = "time"
        time_t
            $(name) = 0;
.       endif
.   endfor
.   for field
        field = asl_field_list_search (fields, "$(name)");
        if (field) {
.       if type = "string"
            icl_shortstr_cpy ($(name), asl_field_string (field));
.       else
            $(name) = atoi (asl_field_string (field));
.       endif
            asl_field_unlink (&field);
        }
.       if type = "string"
        else
            strclr ($(name));
.       endif
.   endfor
.   for exec
        $(string.trim (exec.?''))
.   endfor
    }
.endfor
    if (rc == 0)
        amq_console_reply_ok (
            amq_console, "method-reply", request, self->object_id, NULL, notice_text);
    else
        amq_console_reply_error (
            amq_console, "method-reply", "invalid", request, self->object_id);
    </action>
</method>

<method name = "unlink shim">
    <argument name = "object_p" type = "void *">Reference pointer cast as a void *</argument>
    //
    $(selfname)_unlink ((($(selftype) **) object_p));
</method>

<method name = "initialise">
    s_class = amq_console_class_new ();
    s_class->name    = "$(console_class)";
    s_class->inspect = $(selfname)_inspect_shim;
    s_class->modify  = $(selfname)_modify_shim;
    s_class->method  = $(selfname)_method_shim;
    s_class->unlink  = $(selfname)_unlink_shim;
</method>

<method name = "terminate">
    amq_console_class_destroy (&s_class);
</method>

</class>
