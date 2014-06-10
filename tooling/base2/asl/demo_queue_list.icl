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
    name      = "demo_queue_list"
    comment   = "list of demo_queues"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_iter_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "demo_queue" />
    <option name = "childtype"  value = "demo_queue_t" />
</inherit>
<option name = "alloc"     value = "cache" />

<import class = "demo_queue" />

<method name = "selftest">
</method>

</class>
