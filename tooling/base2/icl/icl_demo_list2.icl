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
    name      = "icl_demo_list2"
    comment   = "Demo of a 2d doubly-linked list"
    version   = "1.0"
    script    = "icl_gen"
    >

<option name = "links"  value = "1" />
<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "1" />
    <option name = "links"  value = "1" />
</inherit>
<inherit class = "icl_list_item" />
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "list2"/>
</inherit>

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
    $(selfname)_list_t *
        list_head;
    $(selfname)_list2_t *
        list2_head;
    $(selftype) *
        item;
    </local>

    list_head = $(selfname)_list_new ();

    item = $(selfname)_new ("Santorini", "0x012201");
    $(selfname)_list_push (list_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("Imafufou", "0x006552A");
    $(selfname)_list_push (list_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("DosLagos", "0x8272210");
    $(selfname)_list_push (list_head, item);
    $(selfname)_unlink (&item);
    
    while (!$(selfname)_list_isempty (list_head)) {
        item = $(selfname)_list_pop (list_head);
        $(selfname)_destroy (&item);
    }
    list2_head = $(selfname)_list2_new ();

    item = $(selfname)_new ("Santorini2", "0x012201");
    $(selfname)_list2_push (list2_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("Imafufou2", "0x006552A");
    $(selfname)_list2_push (list2_head, item);
    $(selfname)_unlink (&item);
    item = $(selfname)_new ("DosLagos2", "0x8272210");
    $(selfname)_list2_push (list2_head, item);
    $(selfname)_unlink (&item);

    while (!$(selfname)_list2_isempty (list2_head)) {
        item = $(selfname)_list2_pop (list2_head);
        $(selfname)_destroy (&item);
    }
    $(selfname)_list_destroy  (&list_head);
    $(selfname)_list2_destroy (&list2_head);
</method>

</class>
