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
    name      = "zyre_amqp_pipe"
    comment   = "Cached pipe"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
  Implements the bare pipe object for use in the AMQP backend cache.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        type,                           //  Pipe type
        name;                           //  Pipe name
    ipr_looseref_list_t
        *joins;                         //  Joins for pipe
</context>

<method name = "new">
    <argument name = "type" type = "char *">Pipe type</argument>
    <argument name = "name" type = "char *">Pipe name</argument>
    //
    assert (name);
    icl_shortstr_cpy (self->name, name);
    icl_shortstr_cpy (self->type, type);
    self->joins = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    //  Any joins on the pipe must have been destroyed already
    ipr_looseref_list_destroy (&self->joins);
</method>

<method name = "join lookup" return = "join">
    <doc>
    Searches for the join specified by address, for a the pipe and feed.
    Returns the join if found, else NULL.
    </doc>
    <argument name = "self" type = "zyre_amqp_pipe_t *">Parent pipe</argument>
    <argument name = "feed name" type = "char *">Feed name</argument>
    <argument name = "address" type = "char *">Join address</argument>
    <declare name = "join" type = "zyre_amqp_join_t *" default="NULL">Join, if exists</declare>
    <local>
    ipr_looseref_t
        *looseref;
    </local>
    //
    assert (self);
    looseref = ipr_looseref_list_first (self->joins);
    while (looseref) {
        zyre_amqp_join_t
            *this_join;
        this_join = ((zyre_amqp_join_t *) looseref->object);
        if (streq (this_join->feed_name, feed_name)
        &&  streq (this_join->address, address)) {
            join = this_join;
            break;
        }
        looseref = ipr_looseref_list_next (&looseref);
    }
</method>

<method name = "selftest" />

</class>
