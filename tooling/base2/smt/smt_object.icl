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
    name      = "smt_object"
    comment   = "iCL add-ons for SMT objects"
    version   = "0.0"
    before    = "object"
    target    = "smt"
    abstract  = "1"
    >

<doc>
    Defines additional iCL fields and methods for SMT objects.
    Use as base class for SMT objects.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>
<option name = "links" value = "1" />
<inherit class = "icl_init" />

<public name = "include">
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "$(selfname)_agent.h"
</public>

<context>
    smt_thread_t
        *thread;
</context>

<method name = "new">
    self->thread = $(selfname)_agent_class_thread_new (self);
    self->thread->animate = TRUE;
</method>

<method name = "destroy public" cname = "destroy" return = "rc" inherit = "none">
    <argument name = "self" type = "$(selftype) *" ref = "1">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <animate  name = "links" value = "self?self->links:0" format = "%i" condition = "links">The number of links to the object</animate>
    <animate  name = "zombie" value = "self?self->zombie:0" format = "%i" condition = "links">Is the referenced object a zombie</animate>
    if (self) {
.
.   if icl.container ? 0
$(selfname)_remove_from_all_containers (self);
.   endif
.
.if links ? 0
        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            rc = $(base)annihilate_ (self_p, file, line);
        else
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
            $(selfname)_free (self);
.else
        rc = $(base)annihilate_ (self_p, file, line);
.endif
        *self_p = NULL;
    }
</method>

<method name = "destroy" cname = "annihilate" return = "rc" private = "1" async = "1" on_shutdown = "1" nextstate = "" inherit = "none">
    <argument name = "self" type = "$(selftype) *" ref = "1">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
.endif
    </local>
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
    self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
    self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links;
#endif
.endif

    <action name = "free">
        if (self->links == 0) {
            icl_console_print ("E: missing link on $(selfname) object");
            $(base)show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            $(selfname)_free (self);
            self = NULL;
        }
    </action>
</method>

<method name = "unlink" condition = "links" inherit = "none">
    <argument name = "self" type = "$(selftype) *" ref = "1" animate = "0">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__" animate = "0">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__" animate = "0">Line number for call</argument>
    <animate  name = "file" value = "file" format = "%s" />
    <animate  name = "line" value = "line" format = "%i" />
    <animate  name = "links" value = "self?self->links:0" format = "%i">The number of links to the object</animate>
    <animate  name = "zombie" value = "self?self->zombie:0" format = "%i">Is the referenced object a zombie</animate>
    <local>
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
    </local>
    <doc>
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    </doc>
    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on $(selfname) object");
            $(base)show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
        self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
        self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                $(selfname)_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                $(selfname)_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
</method>

<method name = "show animation">
    $(selfname)_agent_animate (enabled);
</method>

<method name = "free" private = "0">
    smt_thread_unlink (&self->thread);
</method>

<method name = "async function" template = "function" async = "1" abstract = "1">
    <doc>
    Standard function template for asynchronous functions.
    </doc>
</method>

<method name = "initialise">
    $(selfname)_agent_init ();
</method>

<method name = "terminate" private = "0">
</method>

</class>
