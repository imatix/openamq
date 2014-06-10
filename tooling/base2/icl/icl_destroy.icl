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
    name      = "icl_destroy"
    comment   = "Object to hold objects along with their destructor functions"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    A destroy object holds an object of another class, along with its
    destroy function.  Destroying the destroy object then destroys that
    object.

    The destroy object has a list container to allow such a list to be
    used as a destructor scope.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "links"  value = "1"     />
</inherit>

<inherit name = "icl_list_item" />

<context>
    void
        **item;                         //  The pointer to the item of any class
    icl_destructor_fn
        *destroy;                       //  The item's destructor
</context>

<method name = "new">
    <argument name = "item"    type = "void * *">The pointer to the item to register</argument>
    <argument name = "destroy" type = "icl_destructor_fn *">The item's destructor</argument>
    self->item    = item;
    self->destroy = destroy;
</method>

<method name = "destroy">
    (*self->destroy) (self->item, file, line);
</method>

<method name = "selftest" export = "none">
</method>

</class>
