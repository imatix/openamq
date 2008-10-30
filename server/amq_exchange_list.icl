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
    name      = "amq_exchange_list"
    comment   = "list of amq_exchanges"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_iter_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "amq_exchange" />
    <option name = "childtype"  value = "amq_exchange_t" />
</inherit>
<option name = "alloc"     value = "cache" />

<import class = "amq_exchange" />

<method name = "selftest">
</method>

</class>
