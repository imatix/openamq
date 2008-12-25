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
    name      = "zyre_nozzle"
    comment   = "Zyre nozzle entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS nozzle object.
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
        name;                           //  Nozzle name
    amq_content_basic_list_t
        *contents;                      //  Messages for pipe
    http_response_t
        *response;                      //  Response waiting for message
    Bool
        auto_delete,                    //  Delete messages when delivered?
        waiting;                        //  Waiting for a message from pipe?
</context>

<method name = "new">
    <argument name = "pipe" type = "zyre_pipe_t *">Parent pipe</argument>
    <argument name = "name" type = "char *">Nozzle name</argument>
    //
    assert (pipe);
    self->pipe = pipe;
    icl_shortstr_cpy (self->name, name);
    zyre_nozzle_list_queue (pipe->nozzle_list, self);
    self->contents = amq_content_basic_list_new ();
    //  Unnamed nozzles delete messages automatically when sent, and die when
    //  pedantic success or failure is signalled by the HTTP thread
    self->auto_delete = (strnull (self->name));
</method>

<method name = "destroy">
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    //  Move any contents back onto the pipe
    assert (self->pipe->contents);
    content = amq_content_basic_list_pop (self->contents);
    while (content) {
        amq_content_basic_list_queue (self->pipe->contents, content);
        amq_content_basic_unlink (&content);
        content = amq_content_basic_list_pop (self->contents);
    }
    http_response_unlink (&self->response);
    amq_content_basic_list_destroy (&self->contents);
</method>

<method name = "lookup" return = "nozzle">
    <doc>
    Searches for the nozzle specified by address, for a given pipe and feed.
    Returns the nozzle if found, else NULL.
    </doc>
    <argument name = "pipe" type = "zyre_pipe_t *">Parent pipe</argument>
    <argument name = "name" type = "char *">Nozzle name</argument>
    <declare name = "nozzle" type = "zyre_nozzle_t *">nozzle, if exists</declare>
    //
    assert (pipe);
    nozzle = zyre_nozzle_list_first (pipe->nozzle_list);
    while (nozzle) {
        if (streq (nozzle->name, name))
            break;
        nozzle = zyre_nozzle_list_next (&nozzle);
    }
    return (nozzle);
</method>

<method name = "accept" template = "function">
    <doc>
    Accept a message content onto the nozzle and dispatch to the client
    application, if any.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">Content</argument>
    //
    amq_content_basic_list_queue (self->contents, content);
    if (self->waiting)
        self_dispatch (self, content);
</method>

<method name = "message get" template = "function">
    <doc>
    Gets the message, specified by index, from the nozzle or pipe if
    possible, else waits for a message from the pipe.
    </doc>
    <argument name = "response" type = "http_response_t *">Client response</argument>
    <argument name = "index" type = "size_t">Message index</argument>
    <local>
    amq_content_basic_t
        *content;
    amq_content_basic_list_iter_t
        *iterator;                      //  For walking iter-style lists
    </local>
    //
    //  We allow one response per nozzle; if multiple apps use the same
    //  nozzle the results are undefined and unpredictable.
    http_response_unlink (&self->response);
    self->response = http_response_link (response);

    //  Look for message according to index
    //  Optimise for common case where index is greater than list size
    self->waiting = TRUE;
    if (index >= amq_content_basic_list_count (self->contents)) {
        //  Index is outside nozzle range, so look on pipe
        content = amq_content_basic_list_pop (self->pipe->contents);
        if (content) {
            //  Send content back to client application
            self_dispatch (self, content);
            //  Move content from pipe to nozzle
            amq_content_basic_list_queue (self->contents, content);
            amq_content_basic_unlink (&content);
        }
    }
    else {
        //  Look by index for content already in nozzle
        iterator = amq_content_basic_list_first (self->contents);
        while (index--)
            iterator = amq_content_basic_list_next (&iterator);
        if (iterator)
            self_dispatch (self, iterator->item);
    }
</method>

<method name = "delivery success" template = "function">
    <doc>
    Invoked by the HTTP thread when a message was successfully delivered, and
    the nozzle had asked for pedantic responses.  When the nozzle gets this,
    it does an auto-delete (destroying the content it holds).
    </doc>
    //
    assert (self->auto_delete);
    assert (self_purge (self) == 1);

    //  To self-destruct we need first to grab a link
    self = zyre_nozzle_link (self);
    self_destroy (&self);
</method>

<method name = "delivery failure" template = "function">
    <doc>
    Invoked by the HTTP thread when a message was successfully delivered, and
    the nozzle had asked for pedantic responses.  When the nozzle gets this,
    it does an auto-delete (destroying the content it holds).
    </doc>
    <local>
    zyre_pipe_t
        *pipe;
    amq_content_basic_t
        *content;
    </local>
    //
    //  Get content off the nozzle
    content = amq_content_basic_list_pop (self->contents);
    assert (content);
    pipe = self->pipe;

    //  To self-destruct we need first to grab a link
    self = zyre_nozzle_link (self);
    self_destroy (&self);

    //  Now pass content back to pipe to hold or pass to other nozzles
    zyre_pipe_accept (pipe, content);
    amq_content_basic_unlink (&content);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Sends a specified message to the current response.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">Content</argument>
    <local>
    asl_reader_t
        reader;
    ipr_bucket_t
        *bucket;
    </local>
    //
    assert (self->response);

    amq_content_basic_set_reader (content, &reader, IPR_BUCKET_MAX_SIZE);
    bucket = amq_content_basic_replay_body (content, &reader);
    assert (bucket);
    http_response_set_dynamic (self->response, bucket, "application/octet-stream");
    ipr_bucket_unlink (&bucket);

    //  If auto-delete, we need to get a success/failure back
    if (self->auto_delete)
        http_response_set_pedantic (self->response, self);

    //  Send our response back to the HTTP thread via the portal
    http_portal_response_reply (self->response->portal, self->response);
    self->waiting = FALSE;
</method>

<method name = "purge" template = "function">
    <doc>
    Deletes any and all messages held on the nozzle, returns number of
    contents destroyed.
    </doc>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_list_pop (self->contents);
    while (content) {
        rc++;
        amq_content_basic_unlink (&content);
        content = amq_content_basic_list_pop (self->contents);
    }
</method>

<method name = "selftest">
    <local>
    zyre_pipe_t
        *pipe;
    zyre_pipe_table_t
        *pipe_table;                    //  Hashed table of pipes
    zyre_nozzle_t
        *nozzle;
    </local>
    //
    pipe_table = zyre_pipe_table_new ();
    pipe = zyre_pipe_new (pipe_table, "pipe", NULL);
    assert (pipe);

    assert (zyre_nozzle_lookup (pipe, "my.nozzle") == NULL);
    nozzle = zyre_nozzle_new (pipe, "my.nozzle");
    zyre_nozzle_unlink (&nozzle);

    nozzle = zyre_nozzle_lookup (pipe, "my.nozzle");
    assert (nozzle);
    zyre_nozzle_unlink (&nozzle);

    zyre_pipe_destroy (&pipe);
    zyre_pipe_table_destroy (&pipe_table);
</method>

</class>
