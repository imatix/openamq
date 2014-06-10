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
    name      = "asl_channel_table"
    comment   = "Implements the hash table container for asl_channel"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the hash table container for asl_channel
</doc>

<inherit class = "icl_hash_head" >
    <option name = "prefix"     value = "table"/>
    <option name = "childname"  value = "asl_channel"/>
    <option name = "childtype"  value = "asl_channel_t"/>
    <option name = "hash_type"  value = "int"/>
    <option name = "initial_size" value = "15" />
</inherit>
<import class = "asl_channel" />
<option name = "rwlock"     value = "1"/>


<context>
    <!-- add properties for the container -->
</context>

<method name = "new">
    <!-- any initialisation code you need -->
</method>

<method name = "destroy">
    <!-- any destruction code you need -->
</method>

<method name = "selftest">
    <!-- Add your selftest code -->
    <local>
    </local>
</method>
</class>
