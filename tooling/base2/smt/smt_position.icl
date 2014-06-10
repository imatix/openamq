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
    name      = "smt_position"
    comment   = "SMT position class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_position class is used to build a stack of 'positions' in an smt
    dialog.  A position consists of a 'place' (identifies thread/event/action)
    and a 'module' (action number within a given 'place').
</doc>

<option name = "links"  value = "1"/>
<option name = "rwlock" value = "0"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
</inherit>

<inherit class = "icl_list_item" />

<import class = "smt_thread"/>

<context>
    smt_place_t   place;
    smt_module_t  module;
</context>

<method name = "selftest" inherit = "none"/>

</class>
