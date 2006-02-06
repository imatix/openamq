<?xml?>
<class
    name      = "amq_console_object"
    comment   = "AMQ Console object base class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    abstract  = "1"
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
.       global.top.console_parent = "$(parent)->object_id"
.   else
.       global.top.console_parent = "0"
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
    self->console   = amq_console_link (amq_console);
    self->object_id = icl_atomic_inc32 ((volatile qbyte *) &amq_object_id);
    amq_console_register (self->console, self->object_id, self, s_class, $(console_parent));
    </header>
</method>

<method name = "destroy">
    <action>
    amq_console_cancel (self->console, self->object_id);
    amq_console_unlink (&self->console);
    </action>
</method>

<method name = "inspect" async = "1" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    assert (self);
    </header>
    <possess>
    amq_content_basic_link (request);
    </possess>
    <release>
    amq_content_basic_unlink (&request);
    </release>
    <action>
    asl_field_list_t
        *fields;                        //  Properties of object
    icl_shortstr_t
        field_value;
.for global.top->data->class.field where repeat ?= 1
.   for local
    $(string.trim (local.?''))
.   endfor
.endfor

    fields = asl_field_list_new (NULL);
.for global.top->data->class.field
.   for get
    $(string.trim (get.?''))
.   endfor
.   if repeat ?= 1
    while (*field_value) {
        asl_field_new_string (fields, "$(field.name)", field_value);
        strclr (field_value);
.       for next
    $(string.trim (next.?''))
.       endfor
    }
.   else
    asl_field_new_string (fields, "$(field.name)", field_value);
.   endif
.endfor
    amq_console_reply_ok (amq_console, "inspect-reply", request, self->object_id, fields);
    asl_field_list_unlink (&fields);
    </action>
</method>

<method name = "modify" async = "1" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields"  type = "asl_field_list_t *">Fields to modify</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    assert (self);
    </header>
    <possess>
    amq_content_basic_link (request);
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
.   for put
    field = asl_field_list_search (fields, "$(field.name)");
    if (field) {
        icl_shortstr_cpy (field_value, asl_field_string (field));
        $(string.trim (.))
    }
.   endfor
.endfor
    amq_console_reply_ok (amq_console, "modify-reply", request, self->object_id, NULL);
    </action>
</method>

<method name = "method" async = "1" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "method name"  type = "char *">Argument fields</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "fields"  type = "asl_field_list_t *">Argument fields</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    assert (self);
    </header>
    <possess>
    method_name = icl_mem_strdup (method_name);
    amq_content_basic_link (request);
    asl_field_list_link (fields);
    </possess>
    <release>
    icl_mem_free (method_name);
    amq_content_basic_unlink (&request);
    asl_field_list_unlink (&fields);
    </release>
    <action>
    int
        rc = -1;
.for global.top->data->class.method
    if (streq (method_name, "$(method.name)")) {
.   for exec
        $(string.trim (.))
.   endfor
        rc = 0;
    }
.endfor
    if (rc == 0)
        amq_console_reply_ok (amq_console, "method-reply", request, self->object_id, NULL);
    else
        amq_console_reply_error (amq_console, "method-reply", "invalid", request, self->object_id);
    </action>
</method>

<method name = "initialise">
    s_class = amq_console_class_new ();
    s_class->name    = "$(console_class)";
    s_class->inspect = $(selfname)_inspect;
    s_class->modify  = $(selfname)_modify;
    s_class->method  = $(selfname)_method;
</method>

<method name = "terminate">
    amq_console_class_destroy (&s_class);
</method>

</class>
