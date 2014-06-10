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
    name      = "icl_tracker"
    comment   = "iCL object tracker class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >

<doc>
    Abstract class to count the number of active object instances.
</doc>

<private name = "header">
static qbyte
    s_$(selfname)_count = 0;
</private>

<method name = "new">
    icl_atomic_inc32 ((volatile qbyte *) &s_$(selfname)_count);
</method>

<method name = "destroy">
    icl_atomic_dec32 ((volatile qbyte *) &s_$(selfname)_count);
</method>

<method name = "count" return = "count">
    <declare name = "count" type = "qbyte">Number of instances</declare>
    count = (int) s_$(selfname)_count;
</method>

</class>

