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
.for global.top.data
.   for class
.       global.top.console_class  = name
.       if defined (parent)
.           global.top.console_parent = "$(parent)->object_id"
.       else
.           global.top.console_parent = "0"
.       endif
.   else
.       abort "E: invalid CML definition for $(class.name)"
.   endfor
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
static amq_classdesc_t
    s_classdesc;                        //  Class descriptor
</private>

<method name = "new">
    <header>
    self->console   = amq_console_link (amq_console);
    self->object_id = icl_atomic_inc32 ((volatile qbyte *) &amq_object_id);
    amq_console_register (self->console, &s_classdesc, $(console_parent), self->object_id);
    </header>
</method>

<method name = "destroy">
    amq_console_cancel (self->console, self->object_id);
    amq_console_unlink (&self->console);
</method>

<method name = "initialise">
    s_classdesc.name = "$(console_class)";
</method>

</class>
