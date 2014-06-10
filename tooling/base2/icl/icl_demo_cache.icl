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
    name      = "icl_demo_cache"
    comment   = "Demo of a class built using the cached allocator"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
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
    $(selftype)
        *item,
        *item2,
        *item3;
    int
        count;
    </local>
    //
    //  Explicit object destruction
    for (count = 0; count < 5; count++) {
        item = $(selfname)_new ("Depaul", "New Street");
        $(selfname)_destroy (&item);
    }

    //  Purging the cache
    item  = $(selfname)_new ("Jones", "Main Street");
    item2 = $(selfname)_new ("Smith", "High Street");
    item3 = $(selfname)_new ("Weston", "Dead Alley");

    $(selfname)_destroy (&item);
    $(selfname)_destroy (&item);
    $(selfname)_destroy (&item2);
    icl_system_purge ();

    $(selfname)_destroy (&item3);
</method>

</class>
