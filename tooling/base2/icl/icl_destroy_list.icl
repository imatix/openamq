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
    name      = "icl_destroy_list"
    comment   = "Implements the list container for icl_destroy"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for icl_destroy

    As a destroy list is, from the application point of view, a scope, we
    define macros icl_scope_xxx to allow such an object to be referred to 
    as a scope.
</doc>

<inherit class = "icl_list_head" >
    <option name = "prefix" value = "list"/>
    <option name = "childname" value = "icl_destroy" />
    <option name = "childtype" value = "icl_destroy_t" />
    <option name = "count"     value = "1" />
    <option name = "alloc"     value = "cache" />
</inherit>
<option name = "links"     value = "1" />
<option name = "rwlock"    value = "0" />

<import class = "icl_destroy" />

<public name = "header">
//  Types                                                                      

typedef void (icl_destructor_fn) (void **, char *, size_t);

#define icl_scope_t       icl_destroy_list_t
#define icl_scope_new     icl_destroy_list_new
#define icl_scope_destroy icl_destroy_list_destroy
#define icl_scope_insert  icl_destroy_list_queue
</public>

<context>
    <!-- add properties for the container -->
</context>

<method name = "new">
    <!-- any initialisation code you need -->
</method>

<method name = "destroy">
    <!-- any destruction code you need, note that the list is
         automatically destroyed, you don't need to do this.
      -->
</method>

<method name = "selftest">
    <!-- Add your selftest code -->
    <local>
    </local>
</method>
</class>
