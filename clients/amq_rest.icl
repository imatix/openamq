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
    name      = "amq_rest"
    comment   = "amq REST-like API"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class provides the RestAPI, a layer that sits on top of WireAPI and provides a
REST-like interface to OpenAMQ wiring and messages.  In theory will work with any 
interoperable AMQP server that implements the Rest extension class.

Documentation is on http://wiki.amqp.org/N:rest.

This API implements highly simplified AMQP semantics designed to provide HTTP/HTTPS
access to AMQP exchanges and queues.

* Private queues, bindings, and consumers are invisible and managed automatically 
  by this API.
* Exchanges and shared queues are both considered server "resources", and are
  containers for messages.
* Applications can publish both to exchanges, and directly to shared queues.
* Resource names are paths (should not start or end with slash).

Not supported:

* Does not allow binding arguments (headers) yet.
* Binding shared queues using arbitrary routing keys.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_client_classes" />
<import class = "amq_content_basic_list" />

<context>
    amq_client_connection_t
        *connection;
    amq_client_session_t
        *session;
    icl_shortstr_t
        queue,                          //  Our private queue
        routing_key;                    //  Last binding made
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
    <local>
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    </local>
    //
    auth_data = amq_client_connection_auth_plain (username, password);
    self->connection = amq_client_connection_new (
        hostname, "/", auth_data, "RestAPI", 0, 5000);
    if (self->connection) {
        self->connection->direct = TRUE;
        self->session = amq_client_session_new (self->connection);
    }
    if (!self->session) {
        icl_console_print ("E: could not connect to %s", hostname);
        self_destroy (&self);
    }
    icl_longstr_destroy (&auth_data);
</method>

<method name = "destroy">
    amq_client_session_destroy (&self->session);
    amq_client_connection_destroy (&self->connection);
</method>

<method name = "put" template = "function">
    <doc>
    Declares a resource on the server.  Only needed when the application 
    wants to explicitly create wiring (exchanges and shared queues).  Returns
    0 if successful, -1 if failed.
    </doc>
    <argument name = "path" type = "char *">Resource name</argument>
    <argument name = "type" type = "char *">Exchange type, or "queue"</argument>
    //
    amq_client_session_rest_put (self->session, path, type);
    rc = self->session->reply_code;
    if (rc)
        icl_console_print ("E: %s", self->session->reply_text);
</method>

<method name = "get" template = "function">
    <doc>
    Returns information about the resource.  Returns 0 if the resource exists, 
    else -1.  If the resource exists, puts the type and path_info into the 
    provided string, which must be icl_shortstr_t or null.
    </doc>
    <argument name = "path" type = "char *">Resource name</argument>
    <argument name = "type p" type = "char **">Pointer to type string</argument>
    <argument name = "path info p" type = "char **">Pointer to path info string</argument>
    //
    amq_client_session_rest_get (self->session, path);
    rc = self->session->reply_code;
    if (rc)
        icl_console_print ("E: %s", self->session->reply_text);
    else {
        if (type_p)
            icl_shortstr_cpy (*type_p, self->session->type);
        if (path_info_p)
            icl_shortstr_cpy (*path_info_p, self->session->path_info);
    }
</method>

<method name = "delete" template = "function">
    <doc>
    Deletes the resource, if possible.  Returns 0 if successful, else -1.
    </doc>
    <argument name = "path" type = "char *">Resource name</argument>
    //
    amq_client_session_rest_delete (self->session, path);
    rc = self->session->reply_code;
    if (rc)
        icl_console_print ("E: %s", self->session->reply_text);
</method>

<method name = "post" template = "function">
    <doc>
    Publishes a message content to the specified resource path.  The path
    has two formats: exchange-name/routing-key, or queue-name/reply-to.
    You should unlink the message bucket after doing this call. Returns 0 
    on success, -1 if the call failed.
    </doc>
    <argument name = "path" type = "char *">Resource path</argument>
    <argument name = "bucket" type = "ipr_bucket_t *">Message to send</argument>
    <local>
    amq_content_basic_t
        *content = NULL;
    </local>
    //
    amq_client_session_rest_get (self->session, path);
    rc = self->session->reply_code;
    if (rc == 0) {
        content = amq_content_basic_new ();
        amq_content_basic_record_body (content, bucket);

        if (streq (self->session->type, "queue")) {
            //  Publish to default exchange, routing-key = queue
            //  Path-info is reply-to
            //  Standard pattern for request-response
            amq_content_basic_set_reply_to (content, self->session->path_info);
            rc = amq_client_session_basic_publish (
                self->session, 
                content, 
                0,                      //  Access ticket
                NULL,                   //  Exchange = default exchange
                self->session->path,    //  Routing key = resource name
                TRUE,                   //  Mandatory routing to queue
                FALSE);                 //  Immediate to consumer
        }
        else {
            //  Publish to specified exchange/routing-key
            //  Standard pattern for publish-subscribe
            rc = amq_client_session_basic_publish (
                self->session, 
                content, 
                0,                      //  Access ticket
                self->session->path,    //  Exchange = resource name
                self->session->path_info,   //  Routing key = path info
                FALSE,                  //  Mandatory routing to queue
                FALSE);                 //  Immediate to consumer
        }
        if (rc)
            icl_console_print ("E: [%s] could not send message to server - %s",
                self->session->queue, self->session->error_text);
        amq_content_basic_unlink (&content);
    }
    else
        icl_console_print ("E: %s", self->session->reply_text);
</method>

<method name = "fetch" return = "bucket">
    <doc>
    Fetches a message from the specified resource.  Returns a bucket containing
    the message, or NULL if there was no message to fetch.  Will wait for up to 
    one second for a message to arrive.
    </doc>
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <argument name = "path" type = "char *">Resource name</argument>
    <declare name = "bucket" type = "ipr_bucket_t *" default = "NULL">Returned message</declare>
    <local>
    amq_content_basic_t
        *content;
    asl_reader_t
        reader;                         //  Body reader
    int
        rc,                             //  Return code from calls
        polls;
    </local>
    //
    amq_client_session_rest_get (self->session, path);
    rc = self->session->reply_code;
    if (rc == 0) {
        if (streq (self->session->type, "queue")) {
            //  Get message using synchronous Basic.Get with explicit acknowledge
            amq_client_session_basic_get (self->session, 0, self->session->path, FALSE);
            content = amq_client_session_basic_arrived (self->session);
            polls = 5;
            while (!content && polls) {
                amq_client_session_wait (self->session, 200);
                amq_client_session_basic_get (self->session, 0, self->session->path, FALSE);
                content = amq_client_session_basic_arrived (self->session);
                polls--;
            }
        }
        else {
            //  Create private queue if this is the first fetch
            if (!self->queue) {
                amq_client_session_queue_declare (
                    self->session, 0, NULL, FALSE, FALSE, TRUE, TRUE, NULL);
                icl_shortstr_cpy (self->queue, self->session->queue);
                amq_client_session_basic_consume (
                    self->session, 0, self->queue, NULL, FALSE, TRUE, FALSE, NULL);
            }
            //  Unbind old routing key if needed and bind new one
            if (strneq (self->routing_key, self->session->path_info)) {
                if (strused (self->routing_key))
                    amq_client_session_queue_unbind (
                        self->session, 0, self->queue, self->session->path, self->routing_key, NULL);
                icl_shortstr_cpy (self->routing_key, self->session->path_info);
                amq_client_session_queue_bind (
                    self->session, 0, self->queue, self->session->path, self->routing_key, NULL);
            }
            //  Wait up to one second for a message
            amq_client_session_wait (self->session, 1000);
            content = amq_client_session_basic_arrived (self->session);
        }
        //  Process content, if any
        if (content) {
            amq_content_basic_set_reader (content, &reader, IPR_BUCKET_MAX_SIZE);
            bucket = amq_content_basic_replay_body (content, &reader);
            amq_content_basic_unlink (&content);
        }
    }
</method>

<method name = "selftest">
    <local>
    ipr_process_t
        *process;
    amq_rest_t
        *rest;
    ipr_bucket_t
        *bucket;
    int
        messages;
    </local>
    /*
    //  Start an instance of amq_server
    process = ipr_process_new ("amq_server --port 9000 --debug_queue 1 --debug_route 1 --trace 1", NULL, NULL, NULL);
    ipr_process_start (process, ".");
    apr_sleep (1000000);                //  Give server time to settle
    assert (ipr_process_wait (process, FALSE));
    */
    //  Open new RestAPI session
    rest = amq_rest_new ("localhost", "guest", "guest");
    assert (rest);

    assert (amq_rest_put (rest, "my/test/fanout", "fanout") == 0);
    assert (amq_rest_put (rest, "my/test/direct", "direct") == 0);
    assert (amq_rest_put (rest, "my/test/topic", "topic") == 0);
    assert (amq_rest_put (rest, "my/test/header", "header") == 0);
    assert (amq_rest_put (rest, "my/test/queue", "queue") == 0);

    assert (amq_rest_get (rest, "my/test/fanout", NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/direct", NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/topic", NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/header", NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/queue", NULL, NULL) == 0);

    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (amq_rest_post (rest, "my/test/queue", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/queue/reply", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/queue/reply/to/me", bucket) == 0);
    ipr_bucket_unlink (&bucket);

    //  We have three test messages, check we get them all and no more
    for (messages = 0; messages &lt; 3; messages++) {
        bucket = amq_rest_fetch (rest, "my/test/queue");
        assert (bucket);
        ipr_bucket_unlink (&bucket);
    }
    bucket = amq_rest_fetch (rest, "my/test/queue");
    assert (!bucket);

    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (amq_rest_post (rest, "my/test/topic", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/topic/routing", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/topic/routing/key/value", bucket) == 0);
    ipr_bucket_unlink (&bucket);

    //  We have three test messages, check we get them all and no more
    for (messages = 0; messages &lt; 3; messages++) {
        bucket = amq_rest_fetch (rest, "my/test/queue");
        assert (bucket);
        ipr_bucket_unlink (&bucket);
    }
    bucket = amq_rest_fetch (rest, "my/test/queue");
    assert (!bucket);

    assert (amq_rest_delete (rest, "my/test/fanout") == 0);
    assert (amq_rest_delete (rest, "my/test/direct") == 0);
    assert (amq_rest_delete (rest, "my/test/topic") == 0);
    assert (amq_rest_delete (rest, "my/test/header") == 0);
    assert (amq_rest_delete (rest, "my/test/queue") == 0);

    //  Close RestAPI session and clean up
    amq_rest_destroy (&rest);
    /*
    ipr_process_destroy (&process);
    ipr_dir_remove ("./archive");
    ipr_dir_remove ("./logs");
    */
</method>

</class>
