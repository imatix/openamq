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
    name      = "zyre_pipe"
    comment   = "Zyre pipe entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS pipe object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        class,                          //  Pipe class
        name;                           //  Pipe name
    zyre_join_list_t
        *join_list;                     //  Joins for pipe
</context>

<method name = "new">
    <argument name = "class" type = "char *">Pipe class</argument>
    <argument name = "name" type = "char *">Pipe name</argument>
    <dismiss argument = "key" value = "self->name">Key is pipe name</dismiss>
    <local>
    size_t
        index = 0;
    </local>
    //
    if (name)
        icl_shortstr_cpy (self->name, name);
    else {
        ipr_str_random (self->name, "Ax16");
        icl_shortstr_fmt (self->name + strlen (self->name), "%d", ++index);
    }
    icl_shortstr_cpy (self->class, class);
    self->contents = amq_content_basic_list_new ();
    self->join_list = zyre_join_list_new ();
</method>

<method name = "destroy">
    zyre_join_list_destroy (&self->join_list);
</method>

<method name = "selftest">
    <local>
    zyre_pipe_t
        *pipe;
    zyre_pipe_table_t
        *pipe_table;                    //  Hashed table of pipes
    </local>
    //
    pipe_table = zyre_pipe_table_new ();
    pipe = zyre_pipe_new (pipe_table, "pipe", NULL);
    assert (pipe);
    zyre_pipe_destroy (&pipe);
    zyre_pipe_table_destroy (&pipe_table);
</method>

</class>
