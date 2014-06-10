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
    name      = "ipr_looseref"
    comment   = "Loose reference class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Allows classes to maintain lists of references to other classes without
    creating explicitly-typed classes for each case.  Uses a void * to refer
    to the target class.
</doc>

<option name = "links" value = "0"/>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "count" value = "1" />
</inherit>

<option name = "rwlock" value = "0" />

<context>
    void
        *object,                        //  Target of reference
        *argument;                      //  Application-specific argument
</context>

<method name = "new" />
<method name = "destroy" />

<method name = "queue" return = "self">
    <doc>
    Adds an object to the tail of the list.
    </doc>
    <argument name = "list"   type = "$(selfname)_list_t *">List to attach to</argument>
    <argument name = "object" type = "void *">Target object reference</argument>
    <declare  name = "self"   type = "$(selftype) *">The item</declare>
    self = ipr_looseref_new ();
    if (self) {
        self->object = object;
        $(selfname)_list_queue (list, self);
    }
</method>

<method name = "push" return = "self">
    <doc>
    Adds an object to the head of the list - a pop will remove this object.
    </doc>
    <argument name = "list"   type = "$(selfname)_list_t *">List to attach to</argument>
    <argument name = "object" type = "void *">Target object reference</argument>
    <declare  name = "self"   type = "$(selftype) *">The item</declare>
    self = ipr_looseref_new ();
    if (self) {
        self->object = object;
        $(selfname)_list_push (list, self);
    }
</method>

<method name = "pop" return = "object">
    <doc>
    Removes an object from the head of the list.
    </doc>
    <argument name = "list"   type = "$(selfname)_list_t *">List to attach to</argument>
    <declare  name = "object" type = "void *" default = "NULL">Target object reference</declare>
    <local>
    $(selftype) *self;
    </local>
    self = $(selfname)_list_pop (list);
    if (self) {
        object = self->object;
        $(selfname)_destroy (&self);
    }
</method>

<method name = "selftest">
    <local>
    ipr_looseref_list_t
        *list;
    </local>

    list = ipr_looseref_list_new ();
    ipr_looseref_queue (list, (void *) list);
    ipr_looseref_queue (list, (void *) list);
    ipr_looseref_queue (list, (void *) list);
    assert (ipr_looseref_pop (list) == (void *) list);
    ipr_looseref_list_destroy (&list);
</method>

</class>
