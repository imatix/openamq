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
    name      = "restapi"
    comment   = "OpenAMQ RESTful API"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestAPI layer for AMQP-over-HTTP access to OpenAMQ
networks.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>

<import class = "wireapi" />
<import class = "restapi_pipe" />
<import class = "restms_config" />

<public name = "header">
//  TODO: configurable properties
#define RESTAPI_TIMEOUT         5000    //  Timeout for WireAPI connections
</public>

<context>
    amq_client_connection_t
        *connection;
    amq_client_session_t
        *session;
    icl_shortstr_t
        hostname;                       //  AMQP server hostname
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    icl_shortstr_t
        queue;                          //  Our private queue
    icl_shortstr_t
        strerror,                       //  Last error string
        out_content_type,               //  Outgoing message properties
        out_content_encoding;

    //  Resolved information
    icl_shortstr_t
        sink_name,
        sink_type,
        selector;
    Bool
        is_queue;                       //  The sink is a queue?
    asl_field_list_t
        *properties;                    //  Sink properties
    icl_shortstr_t
        content_type,                   //  Last message properties
        content_encoding,
        reply_to;
</context>

<method name = "new">
    <doc>
    Creates a new session to the specified hostname:port.  Authenticates using
    the supplied user name and password.  If the connection failed, prints the
    error to stderr and returns a null object.
    </doc>
    <argument name = "hostname" type = "char *" />
    <argument name = "username" type = "char *" />
    <argument name = "password" type = "char *" />
    //
    self->auth_data = amq_client_connection_auth_plain (username, password);
    icl_shortstr_cpy (self->hostname, hostname);
    if (restapi_assert_alive (self))
        icl_console_print ("E: could not connect to %s", hostname);
</method>

<method name = "destroy">
    icl_longstr_destroy (&self->auth_data);
    asl_field_list_destroy (&self->properties);
    if (self->session) {
        amq_client_session_queue_delete (
            self->session, 0, self->queue, FALSE, FALSE);
        amq_client_session_destroy (&self->session);
        amq_client_connection_destroy (&self->connection);
    }
</method>

<method name = "assert alive" template = "function">
    <doc>
    If the WireAPI session broke (server died), re-opens it using the original
    credentials.  If the session was never opened, opens it equally.  WireAPI
    is responsible for all server-side state recreation.
    </doc>
    //
    if (self->connection && !self->connection->alive) {
        amq_client_session_destroy (&self->session);
        amq_client_connection_destroy (&self->connection);
    }
    if (!self->connection) {
        self->connection = amq_client_connection_new (
            self->hostname, "/", self->auth_data, "RestAPI", 0, RESTAPI_TIMEOUT);
        if (self->connection) {
            self->connection->direct = FALSE;
            self->session = amq_client_session_new (self->connection);
        }
        if (self->session) {
            //  Create private queue for shared queue access
            amq_client_session_queue_declare (
                self->session, 0, NULL, FALSE, FALSE, TRUE, TRUE, NULL);
            icl_shortstr_cpy (self->queue, self->session->queue);
            /* CONSUMER SHOULD BE PER PIPE...
                consume on queue for each new pipe, cancel when pipe destroyed
                need to cache and replay consumers in wireapi for failover
             */
            amq_client_session_basic_consume (
                self->session, 0,
                self->queue,            //  Queue to consume from
                self->queue,            //  Consumer tag, if any
                FALSE,                  //  No-local option
                TRUE,                   //  No-ack option
                TRUE,                   //  Exclusive option
                NULL);                  //  Arguments
        }
        else
            rc = -1;
    }
</method>

<method name = "resolve" template = "function">
    <doc>
    Resolves a resource path and sets the following properties:
    self->sink_name         Name of the sink
    self->sink_type         Type of sink as a string
    self->selector          Selector string if not empty
    self->is_queue          TRUE if the sink is a queue
    If path could not be resolved, returns -1 and sets self->strerror to
    the error cause.  An unresolved path refers to a non-existent sink.
    </doc>
    <argument name = "path" type = "char *">Resource path</argument>
    <local>
    char
        *path_delim;
    </local>
    //
    assert (path && strused (path));
    self->is_queue = FALSE;
    path_delim = path + strlen (self->sink_name);
    //  If path starts with cached sink name, cleanly ending in
    //    slash or null, then use cache
    if (strused (self->sink_name)
    &&  ipr_str_prefixed (path, self->sink_name)
    &&  (*path_delim == 0 || *path_delim == '/')) {
        if (*path_delim == 0)
            strclr (self->selector);
        else
        if (*path_delim == '/')
            icl_shortstr_cpy (self->selector, path_delim + 1);
        if (streq (self->sink_type, "rotator")
        ||  streq (self->sink_type, "service"))
            self->is_queue = TRUE;
    }
    else {
        amq_client_session_restms_resolve (self->session, path);
        rc = s_check_restms_reply (self);
        if (rc == 0) {
            icl_shortstr_cpy (self->sink_name, self->session->sink_name);
            icl_shortstr_cpy (self->sink_type, self->session->sink_type);
            icl_shortstr_cpy (self->selector,  self->session->selector);
        }
        if (streq (self->sink_type, "rotator")
        ||  streq (self->sink_type, "service"))
            self->is_queue = TRUE;
    }
</method>

<method name = "sink create" template = "function">
    <doc>
    Creates the sink as specified, or checks that the sink exists.
    Returns 0 if OK, -1 if the method failed.
    </doc>
    <argument name = "sink name" type = "char *">Full sink name</argument>
    <argument name = "sink type" type = "char *">A valid sink type</argument>
    //
    assert (sink_name && strused (sink_name));
    assert (sink_type && strused (sink_type));
    amq_client_session_restms_put (self->session, sink_name, sink_type);
    rc = s_check_restms_reply (self);
</method>

<method name = "sink query" template = "function">
    <doc>
    Queries the sink as specified.  Returns 0 if the sink exists, or -1
    if the sink does not exist.  If the sink exists, returns a set of sink
    properties in self->properties.
    </doc>
    <argument name = "sink name" type = "char *">Full sink name</argument>
    //
    assert (sink_name && strused (sink_name));
    amq_client_session_restms_get (self->session, sink_name);
    rc = s_check_restms_reply (self);
    if (rc == 0) {
        asl_field_list_destroy (&self->properties);
        self->properties = asl_field_list_new (self->session->properties);
    }
</method>

<method name = "sink delete" template = "function">
    <doc>
    Deletes the sink as specified.  Returns 0 if the sink was deleted or
    was already absent.  Returns -1 if the delete request was not allowed.
    </doc>
    <argument name = "sink name" type = "char *">Full sink name</argument>
    //
    assert (sink_name && strused (sink_name));
    amq_client_session_restms_delete (self->session, sink_name);
    rc = s_check_restms_reply (self);
    //  Empty sink cache if successful
    if (!rc)
        strclr (self->sink_name);
</method>

<method name = "selector create" template = "function">
    <doc>
    Creates the selector as specified, and starts to receive messages from
    the specified sink. Returns 0 if OK, -1 if the method failed.
    </doc>
    <argument name = "path" type = "char *">Sink/selector path</argument>
    <argument name = "pipe name" type = "char *">Pipe name</argument>
    <local>
    icl_shortstr_t
        consume_tag;
    </local>
    //
    assert (path && strused (path));
    assert (pipe_name && strused (pipe_name));
    rc = restapi_resolve (self, path);
    if (rc == 0) {
        if (self->is_queue) {
            //  Consume from queue:
            //   - we ignore the selector value
            //   - consumer tag is { pipe name [space] queue name }
            //   - requires assertive patch for Basic.Consume
            icl_shortstr_fmt (consume_tag, "%s %s", pipe_name, self->sink_name);
            amq_client_session_basic_consume (self->session, 0,
                self->sink_name,
                consume_tag,
                FALSE, FALSE, FALSE, NULL);
        }
        else
            //  Bind to exchange using specified routing key
            amq_client_session_queue_bind (
                self->session, 0,
                self->queue,
                self->sink_name,
                self->selector,
                NULL);
    }
</method>

<method name = "selector query" template = "function">
    <doc>
    Queries the selector as specified. Current implementation just checks
    that the path resolves and returns the type of the sink.  Returns 0
    if OK, -1 if the method failed.
    </doc>
    <argument name = "path" type = "char *">Sink/selector path</argument>
    <argument name = "pipe name" type = "char *">Pipe name</argument>
    //
    assert (path && strused (path));
    assert (pipe_name && strused (pipe_name));
    rc = restapi_resolve (self, path);
    if (rc == 0)
        restapi_sink_query (self, self->sink_name);
</method>

<method name = "selector delete" template = "function">
    <doc>
    Deletes the selector as specified, and stops receiving messages from
    the specified sink/selector. Returns 0 if OK, -1 if the method failed.
    </doc>
    <argument name = "path" type = "char *">Sink/selector path</argument>
    <argument name = "pipe name" type = "char *">Pipe name</argument>
    <local>
    icl_shortstr_t
        consume_tag;
    </local>
    //
    assert (path && strused (path));
    assert (pipe_name && strused (pipe_name));
    rc = restapi_resolve (self, path);
    if (rc == 0) {
        if (self->is_queue) {
            //  Consume-tag is { pipe-name [space] sink-name }
            icl_shortstr_fmt (consume_tag, "%s %s", pipe_name, self->sink_name);
            amq_client_session_basic_cancel (self->session, consume_tag);
        }
        else
            amq_client_session_queue_unbind (
                self->session, 0,
                self->queue,
                self->sink_name,
                self->selector,
                NULL);
    }
</method>

<!--
    **************************************************************************
                    The following methods are to be reviewed
    **************************************************************************
-->

<method name = "message set type" template = "function">
    <doc>
    Sets the content encoding and type for outgoing messages.
    </doc>
    <argument name = "content type" type = "char *">Message MIME type</argument>
    <argument name = "content encoding" type = "char *">Message MIME encoding</argument>
    //
    icl_shortstr_cpy (self->out_content_type, content_type);
    icl_shortstr_cpy (self->out_content_encoding, content_encoding);
</method>

<method name = "message post" template = "function">
    <doc>
    Sends a content body to the specified sink.  If the sink is an exchange,
    the selector is used as the routing key and the message is published to
    the exchange. If the sink is a queue, the selector is used as the message
    id, self->queue as the reply-to, and the message is published to the default
    exchange. In both cases the selector may be empty.  You should unlink the
    message bucket after doing this call. Returns 0 on success, or -1 if the
    call failed.
    </doc>
    <argument name = "path" type = "char *">Resource path</argument>
    <argument name = "bucket" type = "ipr_bucket_t *">Message to send</argument>
    <local>
    amq_content_basic_t
        *content = NULL;
    </local>
    //
    assert (path && strused (path));
    assert (bucket);
    rc = restapi_resolve (self, path);
    if (rc == 0) {
        content = amq_content_basic_new ();
        amq_content_basic_record_body (content, bucket);
        amq_content_basic_set_content_type (content, self->out_content_type);
        amq_content_basic_set_content_encoding (content, self->out_content_encoding);
        if (self->is_queue) {
            //  Set reply-to to own queue name
            amq_content_basic_set_reply_to (content, self->queue);
            //  Set message-id to selector string
            amq_content_basic_set_message_id (content, self->selector);
            //  Publish to default exchange using sink name as routing key
            amq_client_session_basic_publish (
                self->session, content, 0,
                NULL,                   //  Exchange = default exchange
                self->sink_name,        //  Routing key
                TRUE,                   //  Mandatory routing to queue
                FALSE);                 //  Immediate to consumer
        }
        else {
            amq_client_session_basic_publish (
                self->session, content, 0,
                self->sink_name,        //  Exchange name
                self->selector,         //  Routing key = selector
                FALSE,                  //  Mandatory routing to queue
                FALSE);                 //  Immediate to consumer
        }
        amq_content_basic_unlink (&content);
    }
</method>

<method name = "message get" return = "bucket">
    <doc>
    [[TODO: this needs to be fixed]]
    Gets the next message, if any, from the pipe.  Returns a bucket with the
    message content, or NULL.  Stores the following message properties in the
    Rest object: content_type, content_encoding, reply_to.
    </doc>
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <argument name = "timeout" type = "size_t">msecs to wait for message</argument>
    <declare name = "bucket" type = "ipr_bucket_t *" default = "NULL">Returned message</declare>
    <local>
    amq_content_basic_t
        *content;
    asl_reader_t
        reader;                         //  Body reader
    </local>
    //
    amq_client_session_wait (self->session, timeout? timeout: 1);
    content = amq_client_session_basic_arrived (self->session);
    if (content) {
        amq_content_basic_set_reader (content, &reader, IPR_BUCKET_MAX_SIZE);
        bucket = amq_content_basic_replay_body (content, &reader);
        icl_shortstr_cpy (self->content_type, content->content_type);
        icl_shortstr_cpy (self->content_encoding, content->content_encoding);
        icl_shortstr_cpy (self->reply_to, content->reply_to);
        amq_content_basic_unlink (&content);
    }
</method>

<method name = "message ack" template = "function">
    <doc>
    Acknowledges the previous pedantic message received.
    </doc>
    rc = -1;                            //  Not implemented
</method>

<method name = "message nack" template = "function">
    <doc>
    Rejects the previous pedantic message received.
    </doc>
    rc = -1;                            //  Not implemented
</method>

<method name = "openamq start">
    <doc>
    Starts an instance of the OpenAMQ server, for tests.
    </doc>
    <argument name = "command" type = "char *">Command to start</argument>
    //  Start an instance of amq_server
    s_openamq_process = ipr_process_new (
        command, NULL, "amq_server.lst", "amq_server.err");
    ipr_process_start (s_openamq_process, ".");
    apr_sleep (1000000);                //  Give server time to settle
    assert (ipr_process_wait (s_openamq_process, FALSE));
</method>

<method name = "openamq stop">
    <doc>
    Stops the instance of the OpenAMQ server started by the
    openamq_start() method.
    </doc>
    if (s_openamq_process) {
        ipr_process_destroy (&s_openamq_process);
        ipr_file_delete ("amq_server.lst");
        ipr_file_delete ("amq_server.err");
        ipr_dir_remove ("./archive");
        ipr_dir_remove ("./logs");
    }
</method>

<private name = "header">
static int s_check_restms_reply ($(selftype) *self);
static ipr_process_t
    *s_openamq_process = NULL;
</private>
<private name = "footer">
static int s_check_restms_reply ($(selftype) *self)
{
    if (self->session->response)
        icl_shortstr_fmt (self->strerror, "E: %s", self->session->reply_text);
    return (self->session->response);
}
</private>

<method name = "selftest">
    <local>
    restapi_t
        *restapi;
    ipr_bucket_t
        *bucket;
    </local>
    icl_console_print ("I: starting RestAPI tests...");

    //  Open new RestAPI session
    restapi_openamq_start ("amq_server --port 9000");
    restapi = restapi_new ("localhost:9000", "guest", "guest");
    assert (restapi);

    icl_console_print ("I: Test rotator sink");
    assert (restapi_sink_create (restapi, "test/rotator", "rotator") == 0);
    assert (restapi_sink_create (restapi, "test/rotator", "rotator") == 0);
    assert (restapi_selector_create (restapi, "test/rotator/*", "pipe") == 0);
    assert (restapi_selector_create (restapi, "test/rotator/*", "pipe") == 0);
    assert (restapi_selector_query (restapi, "test/rotator/*", "pipe") == 0);
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (restapi_message_post (restapi, "test/rotator/id-001", bucket) == 0);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 100);
    assert (bucket == NULL);
    assert (restapi_sink_query (restapi, "test/rotator") == 0);
    assert (restapi_selector_delete (restapi, "test/rotator/*", "pipe") == 0);
    assert (restapi_selector_delete (restapi, "test/rotator/*", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/rotator") == 0);
    assert (restapi_sink_delete (restapi, "test/rotator") == 0);
    assert (restapi_sink_query (restapi, "test/rotator"));
    assert (restapi_sink_delete (restapi, "test/rotator") == 0);

    icl_console_print ("I: Test service sink");
    assert (restapi_sink_create (restapi, "test/service", "service") == 0);
    assert (restapi_sink_create (restapi, "test/service", "service") == 0);
    assert (restapi_selector_create (restapi, "test/service/*", "pipe") == 0);
    assert (restapi_selector_create (restapi, "test/service/*", "pipe") == 0);
    assert (restapi_selector_query (restapi, "test/service/*", "pipe") == 0);
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (restapi_message_post (restapi, "test/service/id-001", bucket) == 0);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 100);
    assert (bucket == NULL);
    assert (restapi_sink_query (restapi, "test/service") == 0);
    assert (restapi_selector_delete (restapi, "test/service/*", "pipe") == 0);
    //  Sink is gone when selector is deleted
    apr_sleep (100 * 1000);
    assert (restapi_sink_query (restapi, "test/service"));
    assert (restapi_sink_delete (restapi, "test/service") == 0);
    assert (restapi_selector_delete (restapi, "test/service/*", "pipe"));

    icl_console_print ("I: Test direct sink");
    assert (restapi_sink_create (restapi, "test/direct", "direct") == 0);
    assert (restapi_sink_create (restapi, "test/direct", "direct") == 0);
    assert (restapi_selector_create (restapi, "test/direct/good/address", "pipe") == 0);
    assert (restapi_selector_create (restapi, "test/direct/good/address", "pipe") == 0);
    assert (restapi_selector_query (restapi, "test/direct/good/address", "pipe") == 0);
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (restapi_message_post (restapi, "test/direct/good/address", bucket) == 0);
    assert (restapi_message_post (restapi, "test/direct/wrong/address", bucket) == 0);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 100);
    assert (bucket == NULL);
    assert (restapi_sink_query (restapi, "test/direct") == 0);
    assert (restapi_selector_delete (restapi, "test/direct/good/address", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/direct") == 0);
    assert (restapi_selector_delete (restapi, "test/direct/good/address", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/direct") == 0);
    assert (restapi_sink_delete (restapi, "test/direct") == 0);
    assert (restapi_sink_delete (restapi, "test/direct") == 0);
    assert (restapi_sink_query (restapi, "test/direct"));
    assert (restapi_sink_delete (restapi, "test/direct") == 0);

    icl_console_print ("I: Test topic sink");
    assert (restapi_sink_create (restapi, "test/topic", "topic") == 0);
    assert (restapi_sink_create (restapi, "test/topic", "topic") == 0);
    assert (restapi_selector_create (restapi, "test/topic/a4/*", "pipe") == 0);
    assert (restapi_selector_create (restapi, "test/topic/*/color", "pipe") == 0);
    assert (restapi_selector_query (restapi, "test/topic/*/color", "pipe") == 0);
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (restapi_message_post (restapi, "test/topic/a4/bw", bucket) == 0);
    assert (restapi_message_post (restapi, "test/topic/a5/bw", bucket) == 0);
    assert (restapi_message_post (restapi, "test/topic/a5/color", bucket) == 0);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 100);
    assert (bucket == NULL);
    assert (restapi_sink_query (restapi, "test/topic") == 0);
    assert (restapi_selector_delete (restapi, "test/topic/a4/*", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/topic") == 0);
    assert (restapi_selector_delete (restapi, "test/topic/a4/*", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/topic") == 0);
    assert (restapi_selector_delete (restapi, "test/topic/*/color", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/topic") == 0);
    assert (restapi_selector_delete (restapi, "test/topic/*/color", "pipe") == 0);
    assert (restapi_sink_query (restapi, "test/topic") == 0);
    assert (restapi_sink_delete (restapi, "test/topic") == 0);
    assert (restapi_sink_delete (restapi, "test/topic") == 0);
    assert (restapi_sink_query (restapi, "test/topic"));
    assert (restapi_sink_delete (restapi, "test/topic") == 0);

    icl_console_print ("I: Test pedantic messaging");
    assert (restapi_sink_create (restapi, "test/pedantic", "rotator") == 0);
    assert (restapi_selector_create (restapi, "test/pedantic/*", "pipe") == 0);
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (restapi_message_post (restapi, "test/pedantic/id-001", bucket) == 0);
    ipr_bucket_unlink (&bucket);
    bucket = restapi_message_get (restapi, 2000);
    assert (bucket);
    ipr_bucket_unlink (&bucket);
    assert (restapi_message_nack (restapi));
    assert (restapi_message_ack (restapi));
    assert (restapi_sink_delete (restapi, "test/pedantic") == 0);

    //  Close RestAPI restapi and clean up
    restapi_destroy (&restapi);
    restapi_openamq_stop ();
</method>

</class>
