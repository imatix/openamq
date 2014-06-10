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
    name      = "icl_stack_head"
    comment   = "Abstract base class for list head container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for atomic stack container
</doc>

<inherit name = "icl_object"/>

<context>
    $(childtype)
        $(childname);                   //  Self starts with child object
.if count ? 0
    volatile qbyte
        count;
.endif
</context>

<method name = "new">
    <header export = "after">
    self->$(childname).$(prefix)_next = NULL;
    self->$(childname).$(prefix)_head = NULL;
.if count ? 0
    self->count = 0;
.endif
    </header>
</method>

<method name = "destroy">
    <doc>
    Destroys the stack and unlinks all its items.
    </doc>
    <local>
    $(childtype)
        *item;
    </local>
    <header>
    <!-- We do the popping manually because zombie is set by now -->
    do item = ($(childtype) *) self->$(childname).$(prefix)_next;
    while ((item ? icl_atomic_casptr ((void *) &self->$(childname).$(prefix)_next, ($(childtype) *) item->$(prefix)_next, item) : NULL) != item);
    while (item) {
.if link_to_child ? 1
        $(childname)_unlink (&item);
.else
        $(childname)_destroy (&item);
.endif
        do item = ($(childtype) *) self->$(childname).$(prefix)_next;
        while ((item ? icl_atomic_casptr ((void *) &self->$(childname).$(prefix)_next, ($(childtype) *) item->$(prefix)_next, item) : NULL) != item);
    }
    </header>
</method>

<method name = "isempty" template = "function">
    <doc>
    Returns true if the list is empty, else returns false.
    </doc>
        rc = (self->$(childname).$(prefix)_next == NULL);
</method>

<method name = "push" template = "function">
    <doc>
    Pushes the item onto the stack.
    </doc>
    <argument name = "item" type = "$(childtype) *" />
    <header>
.if link_to_child ? 1
            $(childname)_link_ (item, NULL, 0);
.endif
        do item->$(prefix)_next = self->$(childname).$(prefix)_next;
            while (icl_atomic_casptr ((void *) &self->$(childname).$(prefix)_next, item, ($(childtype) *) item->$(prefix)_next) != item->$(prefix)_next);
.if count ? 0
        icl_atomic_inc32 (&self->count);
.endif
    </header>
</method>

<method name = "pop" return = "item">
    <doc>
    Pops and returns an item from the stack.
    </doc>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <declare name = "item" type = "$(childtype) *" default = "NULL" />
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
        do item = ($(childtype) *) self->$(childname).$(prefix)_next;
        while ((item ? icl_atomic_casptr ((void *) &self->$(childname).$(prefix)_next, ($(childtype) *) item->$(prefix)_next, item) : NULL) != item);
.if count ? 0
        icl_atomic_dec32 (&self->count);
.endif
    </header>
</method>

</class>
