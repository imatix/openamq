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
    name      = "smt_demo_object"
    comment   = "Demonstration of SMT objects"
    version   = "1.0"
    script    = "smt_object_gen"
    >
<doc>
This object is a demonstration of SMT objects.
</doc>

<inherit class = "smt_object" />

<context>
    char
        *string;
</context>

<method name = "new">
    <argument name = "string" type = "char *" />
    self->string = icl_mem_strdup (string);
</method>

<method name = "print" template = "async function">
    <argument name = "prefix" type = "char *" />
    <possess>
    prefix = icl_mem_strdup (prefix);
    </possess>
    <release>
    icl_mem_free (prefix);
    </release>
    <action>
//    icl_console_print ("%s: %s", prefix, self->string);
    </action>
</method>

<method name = "destroy">
    <action>
    icl_mem_free (self->string);
    </action>
</method>

<method name = "selftest">
    <local>
    smt_demo_object_t
        *self,
        *self_link;
    char
        prefix [] = "I say, ";
    </local>
    smt_initialise ();
    smt_demo_object_agent_animate (TRUE);

    self = smt_demo_object_new ("Hello World");
    self_link = smt_demo_object_link (self);
    smt_demo_object_print (self, prefix);

    //  Wipe the prefix now to show that possess did its work.
    memset (prefix, 0, sizeof (prefix));

    //  Make sure that multiple destroys work
    smt_demo_object_destroy (&self);
    smt_demo_object_destroy (&self_link);
</method>

</class>
