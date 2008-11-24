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
    name      = "restapi_pipe"
    comment   = "OpenAMQ RESTful API - pipes"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestAPI pipe object.  A pipe is a container for
a stream of messages arriving from the server.  Pipes do not support
reference counting.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "restapi" />

<context>
    restapi_t
        *restapi;                       //  Parent RestAPI instance
    icl_shortstr_t
        name;                           //  Our pipe name
    amq_content_basic_list_t
        *contents;                      //  Messages for this pipe
    apr_time_t
        time_created,
        time_accessed,
        session_timestamp;              //  Private queue timestamp
    size_t
        contents_pending,               //  Messages on queue
        contents_served,                //  Messages delivered
        octets_pending,                 //  Total bytes on queue
        octets_served;                  //  Total bytes delivered
    int
        top_message_id,                 //  Newest message on queue
        cur_message_id;                 //  Last message delivered
</context>

<!--
        rc = restapi_purge_pipes (restapi)
            - purges old pipes
            - timeout is configurable
            - max pipes is configurable
        rc = restapi_ distribute messages
            - take all arrived messages
            - send to pipe queues according to consumer tag
-->

<method name = "new">
    <doc>
    Creates a new pipe, within an existing RestAPI session.  If the pipe
    could not be created, returns NULL.
    </doc>
    <argument name = "restapi" type = "restapi_t *">Parent RestAPI session</argument>
    <argument name = "name" type = "char *">Pipe name</argument>
    <dismiss argument = "key" value = "self->name">Key is lease name</dismiss>
    <local>
    size_t
        index = 0;
    </local>
    //
    self->restapi = restapi_link (restapi);
    if (name)
        icl_shortstr_cpy (self->name, name);
    else {
        ipr_str_random (self->name, "Ax16");
        icl_shortstr_fmt (self->name + strlen (self->name), "%d", ++index);
    }
    //  Create private queue for this pipe
    amq_client_session_queue_declare (
        self->restapi->session, 0, self->name, FALSE, FALSE, TRUE, TRUE, NULL);
    amq_client_session_basic_consume (
        self->restapi->session, 0,
        self->name,             //  Queue to consume from
        self->name,             //  Consumer tag, if any
        FALSE,                  //  No-local option
        TRUE,                   //  No-ack option
        TRUE,                   //  Exclusive option
        NULL);                  //  Arguments
</method>

<method name = "destroy">
    amq_client_session_queue_delete (
        self->restapi->session, 0,
        self->name,             //  Queue to delete
        FALSE,                  //  If-empty option
        FALSE);                 //  If-unused option
    amq_content_basic_list_destroy (&self->contents);
    restapi_unlink (&self->restapi);
</method>

<method name = "selftest">
    <local>
    restapi_t
        *restapi;
    restapi_pipe_t
        *pipe;
    restapi_pipe_table_t
        *pipe_table;                    //  Hashed table of pipes
    </local>
    //
    icl_console_print ("Starting amq_server...");
    restapi_openamq_start ("amq_server --port 9000");
    restapi = restapi_new ("localhost:9000", "guest", "guest");
    assert (restapi);
    pipe_table = restapi_pipe_table_new ();
    pipe = restapi_pipe_new (pipe_table, restapi, NULL);
    assert (pipe);
    restapi_pipe_destroy (&pipe);
    restapi_pipe_table_destroy (&pipe_table);
    restapi_destroy (&restapi);
    icl_console_print ("Stopping amq_server...");
    restapi_openamq_stop ();
</method>

</class>
