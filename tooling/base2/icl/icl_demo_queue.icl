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
    name      = "icl_demo_queue"
    comment   = "Demo of a singly-linked queue with atomic thread-safe operations"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>
<inherit class = "icl_queue_item" />

<context>
    char *name;
    char *address;
</context>

<method name = "new">
    <argument name = "name"    type = "char *" />
    <argument name = "address" type = "char *" />
    self->name    = icl_mem_strdup (name);
    self->address = icl_mem_strdup (address);
</method>

<method name = "destroy">
    icl_mem_free (self->name);
    icl_mem_free (self->address);
</method>

<method name = "selftest">
    <local>
    $(selfname)_queue_t *
        queue_head;
    $(selftype) *
        item;
    </local>

    queue_head = $(selfname)_queue_new ();

    item = $(selfname)_new ("Santorini", "0x012201");;
    $(selfname)_queue_queue (queue_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("Imafufou", "0x006552A");
    $(selfname)_queue_queue (queue_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("DosLagos", "0x8272210");
    $(selfname)_queue_queue (queue_head, item);
    $(selfname)_unlink (&item);

    item = $(selfname)_queue_pop (queue_head);
    while (item) {
        $(selfname)_destroy (&item);
        item = $(selfname)_queue_pop (queue_head);
    }
    $(selfname)_queue_destroy (&queue_head);
</method>

</class>
