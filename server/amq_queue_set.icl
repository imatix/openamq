<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "amq_queue_set"
    comment   = "Private set of queues per arbitrary parent object"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_iter_list_head" >
    <option name = "prefix"     value = "list" />
    <option name = "childname"  value = "amq_queue" />
    <option name = "childtype"  value = "amq_queue_t" />
    <option name = "count"      value = "1" />
</inherit>
<option name = "alloc" value = "cache" />

<import class = "amq_queue" />

<method name = "find" template = "function">
    <argument name = "value" type = "$(childtype) *" />
    <declare name = "rc" type = "$(selfname)_iter_t *" />
    for (rc = self_first (self); rc; rc = self_next (&rc))
        if (rc->item == value)
            break;
</method>

<method name = "selftest">
</method>

</class>
