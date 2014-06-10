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
    name      = "icl_demo_array"
    comment   = "Demo of a dynamic array"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_array_item" />

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
    $(selfname)_array_t *
        array;
    $(selftype) *
        item;
    </local>

    array = $(selfname)_array_new ();

    item = $(selfname)_new (array, 10, "Santorini", "0x012201");
    $(selfname)_unlink (&item);
    item = $(selfname)_new (array, 20, "Imafufou", "0x006552A");
    $(selfname)_unlink (&item);
    item = $(selfname)_new (array, 30, "DosLagos", "0x8272210");
    $(selfname)_unlink (&item);
    assert ((long) $(selfname)_array_ubound (array) == 31);

    item = $(selfname)_array_fetch (array, 30);
    $(selfname)_destroy (&item);
    assert ((long) $(selfname)_array_ubound (array) == 21);

    item = $(selfname)_array_fetch (array, 20);
    $(selfname)_destroy (&item);
    assert ((long) $(selfname)_array_ubound (array) == 11);

    item = $(selfname)_array_fetch (array, 10);
    $(selfname)_destroy (&item);
    assert ((long) $(selfname)_array_ubound (array) == 0);

    $(selfname)_array_reset   (array);
    $(selfname)_array_reset   (array);
    $(selfname)_array_destroy (&array);
</method>

</class>
