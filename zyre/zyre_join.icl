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
    name      = "zyre_join"
    comment   = "Zyre join entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS join object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_list_item" />

<import class = "zyre_classes" />

<context>
    zyre_pipe_t
        *pipe;                          //  Parent pipe
    icl_shortstr_t
        address,                        //  Join address
        feed_name,                      //  Feed name
        feed_class,                     //  Feed class
        uri;                            //  Join URI if any
</context>

<method name = "new">
    <argument name = "pipe" type = "zyre_pipe_t *">Parent pipe</argument>
    <argument name = "address" type = "char *">Join address</argument>
    <argument name = "feed name" type = "char *">Feed name</argument>
    <argument name = "feed class" type = "char *">Feed class</argument>
    //
    assert (pipe);
    self->pipe = pipe;
    icl_shortstr_cpy (self->address, address);
    icl_shortstr_cpy (self->feed_name, feed_name);
    icl_shortstr_cpy (self->feed_class, feed_class);
    zyre_join_list_queue (pipe->join_list, self);
</method>

<method name = "lookup" return = "join">
    <doc>
    Searches for the join specified by address, for a given pipe and feed.
    Returns the join if found, else NULL.
    </doc>
    <argument name = "pipe" type = "zyre_pipe_t *">Parent pipe</argument>
    <argument name = "address" type = "char *">Join address</argument>
    <argument name = "feed name" type = "char *">Feed name</argument>
    <declare name = "join" type = "zyre_join_t *">Join, if exists</declare>
    //
    assert (pipe);
    join = zyre_join_list_first (pipe->join_list);
    while (join) {
        if (streq (join->feed_name, feed_name)
        &&  streq (join->address, address))
            break;
        join = zyre_join_list_next (&join);
    }
    return (join);
</method>

<method name = "selftest">
    <local>
    zyre_pipe_t
        *pipe;
    zyre_pipe_table_t
        *pipe_table;                    //  Hashed table of pipes
    zyre_join_t
        *join;
    </local>
    //
    pipe_table = zyre_pipe_table_new ();
    pipe = zyre_pipe_new (pipe_table, "pipe", NULL);
    assert (pipe);

    assert (zyre_join_lookup (pipe, "*.imatix", "my.feed") == NULL);
    join = zyre_join_new (pipe, "*.imatix", "my.feed", "direct");
    zyre_join_unlink (&join);

    join = zyre_join_lookup (pipe, "*.imatix", "my.feed");
    assert (join);
    zyre_join_unlink (&join);

    zyre_pipe_destroy (&pipe);
    zyre_pipe_table_destroy (&pipe_table);
</method>

</class>
