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
    amq_console_cancel (self->console, self->object_id);
    amq_console_unlink (&self->console);
</method>

<method name = "inspect" async = "1" return = "rc">
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "request" type = "amq_content_basic_t *">The original request</argument>
    <argument name = "detail"  type = "Bool"  >Return child object list?</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    assert (self);
    </header>
    <possess>
    amq_content_basic_possess (request);
    </possess>
    <release>
    amq_content_basic_destroy (&request);
    </release>
    <action>
    asl_field_list_t
        *fields;
    icl_shortstr_t
        field_value;
        
    fields = asl_field_list_new (NULL);
.for global.top->data->class.field
.   for get
    $(string.trim (get.))
    asl_field_new_string (fields, "$(field.name)", field_value);
.   endfor
.endfor
    amq_console_inspect_ok (amq_console, request, self->object_id, 123, fields);
    asl_field_list_destroy (&fields);
    </action>
</method>

<method name = "initialise">
    s_class = amq_console_class_new ();
    s_class->name    = "$(console_class)";
    s_class->inspect = $(selfname)_inspect;
</method>

<method name = "terminate">
    amq_console_class_destroy (&s_class);
</method>

</class>
