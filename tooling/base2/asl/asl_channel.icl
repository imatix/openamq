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
    name      = "asl_channel"
    comment   = "Channel class"
    version   = "1.0"
    abstract  = "1"
    >
<doc>
    This abstract base class implements the ASL channel class.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "int"  />
</inherit>

<import class = "asl"/>
<import class = "$(basename)_method" />

<context>
    Bool
        active;                         //  If TRUE, channel is active
</context>

<method name = "new">
    self->active = TRUE;
</method>

<method name = "selftest" />

</class>
