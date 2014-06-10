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
    name      = "smt_object_tracker"
    comment   = "SMT/iCL asynch object tracker class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >

<doc>
    Abstract class to count the number of active object instances.
</doc>

<public name = "header">
extern qbyte
    gbl_$(selfname)_count;
</public>

<private name = "header">
qbyte
    gbl_$(selfname)_count = 0;
</private>

<method name = "new">
    icl_atomic_inc32 ((volatile qbyte *) &gbl_$(selfname)_count);
</method>

<method name = "destroy">
    <action>
    icl_atomic_dec32 ((volatile qbyte *) &gbl_$(selfname)_count);
    </action>
</method>

<method name = "count" return = "count">
    <declare name = "count" type = "qbyte">Number of instances</declare>
    count = (int) gbl_$(selfname)_count;
</method>

</class>

