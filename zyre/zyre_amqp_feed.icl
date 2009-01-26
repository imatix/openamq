<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This file is licensed under the GPL as follows:

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
    name      = "zyre_amqp_feed"
    comment   = "Cached feed"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
  Implements the bare feed object for use in the AMQP backend cache.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        type,                           //  Feed type
        name;                           //  Feed name
    Bool
        as_queue;                       //  Implemented as AMQP queue?
</context>

<method name = "new">
    <argument name = "type" type = "char *">Feed type</argument>
    <argument name = "name" type = "char *">Feed name</argument>
    //
    assert (name);
    icl_shortstr_cpy (self->name, name);
    icl_shortstr_cpy (self->type, type);
    self->as_queue = (streq (type, "service") || streq (type, "rotator"));
</method>

<method name = "selftest" />

</class>
