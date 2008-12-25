<?xml?>
<!--
    Copyright (c) 1996-2007 iMatix Corporation

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
        name,                           //  Pipe name
        uri;                            //  Pipe URI if any
    amq_content_basic_list_t
        *contents;                      //  Messages for pipe
    zyre_join_list_t
        *join_list;                     //  Joins for pipe
    zyre_nozzle_list_t
        *nozzle_list;                   //  Nozzles for pipe
    apr_time_t
        time_created,                   //  For house-keeping
        time_accessed;
    size_t
        contents_pending,               //  Messages on pipe
        contents_served,                //  Messages delivered
        octets_pending,                 //  Total bytes on pipe
        octets_served;                  //  Total bytes delivered
    int
        top_message_id,                 //  Newest message on pipe
        cur_message_id;                 //  Last message delivered
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
    assert (zyre_uri_is_pipe_class (class));
    if (name)
        icl_shortstr_cpy (self->name, name);
    else {
        ipr_str_random (self->name, "Ax16");
        icl_shortstr_fmt (self->name + strlen (self->name), "%d", ++index);
    }
    icl_shortstr_cpy (self->class, class);
    self->contents = amq_content_basic_list_new ();
    self->join_list = zyre_join_list_new ();
    self->nozzle_list = zyre_nozzle_list_new ();
</method>

<method name = "destroy">
    zyre_nozzle_list_destroy (&self->nozzle_list);
    zyre_join_list_destroy (&self->join_list);
    amq_content_basic_list_destroy (&self->contents);
</method>

<method name = "set uri" template = "function">
    <argument name = "format" type = "char *">Format specifier</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    <local>
    icl_shortstr_t
        string;
    </local>
    //
    apr_vsnprintf (string, ICL_SHORTSTR_MAX, format, args);
    icl_shortstr_cpy (self->uri, string);
</method>

<method name = "accept" template = "function">
    <doc>
    Accept a message content onto the pipe and dispatches messages in as far
    as possible.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">Content</argument>
    <local>
    zyre_nozzle_t
        *nozzle;
    </local>
    //
    //  Pass message to first waiting nozzle, if any, else queue on pipe
    nozzle = zyre_nozzle_list_first (self->nozzle_list);
    while (nozzle) {
        if (nozzle->waiting) {
            //  Move nozzle to end of list to implement a round-robin
            zyre_nozzle_list_queue (self->nozzle_list, nozzle);
            zyre_nozzle_accept (nozzle, content);
            break;
        }
        nozzle = zyre_nozzle_list_next (&nozzle);
    }
    //  Hold on own list if we could not pass the content to a nozzle
    if (nozzle)
        zyre_nozzle_unlink (&nozzle);
    else
        amq_content_basic_list_queue (self->contents, content);
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
