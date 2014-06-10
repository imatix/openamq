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
    name      = "smt_demo_object_action"
    comment   = "Demonstration of SMT objects with additional actions"
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

<method name = "request expire" template = "async function">
    <action>
    smt_timer_request_delay (self->thread, 100000, expire_event);
    </action>
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

<event name = "expire">
    <action>
    icl_mem_strfree (&self->string);
    self->string = icl_mem_strdup ("EXPIRED");
    </action>
</event>

<method name = "destroy">
    <action>
    icl_mem_strfree (&self->string);
    </action>
</method>

<method name = "selftest">
    <local>
    smt_demo_object_action_t
        *self;
    char
        prefix [] = "I say";
    </local>
    smt_initialise ();

    self = smt_demo_object_action_new ("Hello World");
    smt_demo_object_action_request_expire (self);
    smt_demo_object_action_print          (self, prefix);

    //  Wipe the prefix now to show that possess did its work.
    memset (prefix, 0, sizeof (prefix));

    apr_sleep (200000);

    smt_demo_object_action_print          (self, "After 200ms");

    smt_demo_object_action_destroy (&self);
</method>

</class>
