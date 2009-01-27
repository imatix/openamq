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
    name      = "zyre_amqp_join"
    comment   = "Cached join"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
  Implements the bare join object for use in the AMQP backend cache.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        pipe_name,                      //  Pipe name
        feed_name,                      //  Feed name
        address;                        //  Join address
    ipr_looseref_t
        *pipe_ref;                      //  Backlink to pipe
</context>

<method name = "new">
    <argument name = "pipe name" type = "char *">Pipe name</argument>
    <argument name = "feed name" type = "char *">Feed name</argument>
    <argument name = "address" type = "char *">Join address</argument>
    //
    icl_shortstr_cpy (self->pipe_name, pipe_name);
    icl_shortstr_cpy (self->feed_name, feed_name);
    icl_shortstr_cpy (self->address, address);
</method>

<method name = "selftest" />

</class>
