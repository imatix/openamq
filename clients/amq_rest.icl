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
    icl_shortstr_t
        cached_path,                    //  Cache Rest.Get calls
        cached_info,
        cached_type;
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
        self->connection->direct = FALSE;
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

    PUT (path, [type])
        type -> create sink
        else try to create binding
    DELETE (path)
    POST (path)
    [message] = GET (path)

<method name = "put" template = "function">
    <doc>
    Declares a resource on the server.  A resource is either a sink (a queue or
    exchange), or a binding.  Sinks are containers for bindings.  This method
    lets an application explicitly create sinks or bindings, or assert that they
    exist.  If the type argument is specified, the path is the sink name.  If
    the type argument is not specified, the path is an existing sink name
    followed by a routing key pattern. Returns 0 if successful, -1 if failed.
    The valid sink types are the AMQP exchange types plus "queue".
    </doc>
    <argument name = "path" type = "char *">Resource path</argument>
    <argument name = "type" type = "char *">Sink type</argument>
    //
    assert (path && strused (path));
    //  Maps straight through to Rest.Put
    amq_client_session_rest_put (self->session, path, type);
    rc = self->session->reply_code;
    if (rc)
        icl_console_print ("E: %s", self->session->reply_text);
</method>

<method name = "get" template = "function">
    <doc>
    Queries a resource on the server.  A resource is either a sink (a queue or
    exchange), or a binding.  Sinks are containers for bindings.  This method
    lets an application 



-- merge this with fetch
    Returns information about the resource.  Returns 0 if the resource exists, 
    else -1.  If the resource exists, puts the type and info into the 
    provided string, which must be icl_shortstr_t or null.
    </doc>
    <argument name = "path" type = "char *">Resource name</argument>
    <argument name = "out path" type = "char *">Pointer to path string</argument>
    <argument name = "out type" type = "char *">Pointer to type string</argument>
    <argument name = "out info" type = "char *">Pointer to path info string</argument>
    //
    assert (path && strused (path));
    if (strused (self->cached_path) 
    &&  ipr_str_prefixed (path, self->cached_path))
        icl_shortstr_cpy (self->cached_info, path + strlen (self->cached_path) + 1);
    else {
        amq_client_session_rest_get (self->session, path);
        rc = self->session->reply_code;
        if (rc)
            icl_console_print ("E: %s", self->session->reply_text);
        else {
            icl_shortstr_cpy (self->cached_path, self->session->path);
            icl_shortstr_cpy (self->cached_type, self->session->type);
            icl_shortstr_cpy (self->cached_info, self->session->info);
        }
    }
    if (!rc) {
        if (out_path)
            icl_shortstr_cpy (out_path, self->cached_path);
        if (out_type)
            icl_shortstr_cpy (out_type, self->cached_type);
        if (out_info)
            icl_shortstr_cpy (out_info, self->cached_info);
    }
</method>

<method name = "delete" template = "function">
    <doc>
    Deletes the resource, if possible.  Returns 0 if successful, else -1.
    </doc>
    <argument name = "path" type = "char *">Resource name</argument>
    //
    assert (path && strused (path));
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
    icl_shortstr_t
        resource_name,        
        resource_type,        
        resource_info;        
    </local>
    //
    assert (path && strused (path));
    assert (bucket);

    rc = amq_rest_get (self, path, resource_name, resource_type, resource_info);
    if (rc == 0) {
        content = amq_content_basic_new ();
        amq_content_basic_record_body (content, bucket);

        if (streq (resource_type, "queue")) {
            //  Publish to default exchange, routing-key = queue
            //  Path-info is reply-to
            //  Standard pattern for request-response
            amq_content_basic_set_reply_to (content, self->session->info);
            rc = amq_client_session_basic_publish (
                self->session, 
                content, 
                0,                      //  Access ticket
                NULL,                   //  Exchange = default exchange
                resource_name,          //  Routing key = resource name
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
                resource_name,          //  Exchange = resource name
                resource_info,          //  Routing key = path info
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
    the message, or NULL if there was no message to fetch.  If timeout is zero,
    does not wait but returns immediately if there are no messages pending.
    </doc>
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <argument name = "path" type = "char *">Resource name</argument>
    <argument name = "timeout" type = "size_t">Msecs to wait for message</argument>
    <declare name = "bucket" type = "ipr_bucket_t *" default = "NULL">Returned message</declare>
    <local>
    amq_content_basic_t
        *content;
    asl_reader_t
        reader;                         //  Body reader
    int
        rc;                             //  Return code from calls
    icl_shortstr_t
        resource_name,        
        resource_type,        
        resource_info;        
    </local>
    //
    assert (path && strused (path));
    rc = amq_rest_get (self, path, resource_name, resource_type, resource_info);
    if (rc == 0) {
        if (streq (resource_type, "queue")) {
            //  Get message using synchronous Basic.Get with explicit acknowledge
            amq_client_session_basic_get (self->session, 0, resource_name, FALSE);
            content = amq_client_session_basic_arrived (self->session);
            if (timeout && !content) {
                amq_client_session_wait (self->session, timeout);
                amq_client_session_basic_get (self->session, 0, resource_name, FALSE);
                content = amq_client_session_basic_arrived (self->session);
            }
        }
        else {
            //  Create private queue if this is the first fetch
            if (strnull (self->queue)) {
                amq_client_session_queue_declare (
                    self->session, 0, NULL, FALSE, FALSE, TRUE, TRUE, NULL);
                icl_shortstr_cpy (self->queue, self->session->queue);
                amq_client_session_basic_consume (
                    self->session, 0, self->queue, NULL, FALSE, TRUE, FALSE, NULL);
            }
            //  Unbind old routing key if needed and bind new one
            if (strneq (self->routing_key, resource_info)) {
                if (strused (self->routing_key))
                    amq_client_session_queue_unbind (
                        self->session, 0, self->queue, resource_name, self->routing_key, NULL);
                icl_shortstr_cpy (self->routing_key, resource_info);
                amq_client_session_queue_bind (
                    self->session, 0, self->queue, resource_name, self->routing_key, NULL);
            }
            amq_client_session_wait (self->session, timeout? timeout: 1);
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
//    ipr_process_t
  //      *process;
    amq_rest_t
        *rest;
    ipr_bucket_t
        *bucket;
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

    //  Test creating, querying, deleting a bunch of resources
    assert (amq_rest_put (rest, "my/test/fanout", "fanout") == 0);
    assert (amq_rest_put (rest, "my/test/direct", "direct") == 0);
    assert (amq_rest_put (rest, "my/test/topic", "topic") == 0);
    assert (amq_rest_put (rest, "my/test/header", "header") == 0);
    assert (amq_rest_put (rest, "my/test/queue", "queue") == 0);

    assert (amq_rest_get (rest, "my/test/fanout", NULL, NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/direct", NULL, NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/topic", NULL, NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/header", NULL, NULL, NULL) == 0);
    assert (amq_rest_get (rest, "my/test/queue", NULL, NULL, NULL) == 0);

    assert (amq_rest_delete (rest, "my/test/fanout") == 0);
    assert (amq_rest_delete (rest, "my/test/direct") == 0);
    assert (amq_rest_delete (rest, "my/test/topic") == 0);
    assert (amq_rest_delete (rest, "my/test/header") == 0);
    assert (amq_rest_delete (rest, "my/test/queue") == 0);

    //  Test publishing and reading from a queue
    bucket = ipr_bucket_new (1000);
    ipr_bucket_fill_random (bucket, 1000);
    assert (amq_rest_put  (rest, "my/test/queue", "queue") == 0);
    assert (amq_rest_post (rest, "my/test/queue", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/queue/reply", bucket) == 0);
    assert (amq_rest_post (rest, "my/test/queue/reply/to/me", bucket) == 0);
    ipr_bucket_unlink (&bucket);

    //  We have three test messages, check we get them all and no more
    bucket = amq_rest_fetch (rest, "my/test/queue", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "my/test/queue", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "my/test/queue", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "my/test/queue", 0);
    assert (!bucket);
    assert (amq_rest_delete (rest, "my/test/queue") == 0);

    //  Test publishing and reading from a topic
    bucket = amq_rest_fetch (rest, "amq.topic/" "*/*", 0);
    assert (!bucket);

    bucket = ipr_bucket_new (1000);
    bucket->cur_size = 1;
    bucket->data [0] = '!';

    //  These messages will match
    assert (amq_rest_post (rest, "amq.topic/one/two", bucket) == 0);
    assert (amq_rest_post (rest, "amq.topic/routing/key", bucket) == 0);
    assert (amq_rest_post (rest, "amq.topic//key", bucket) == 0);

    //  These messages won't match
    assert (amq_rest_post (rest, "amq.topic/one", bucket) == 0);
    assert (amq_rest_post (rest, "amq.topic/one/two/three", bucket) == 0);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "amq.topic/" "*/*", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "amq.topic/" "*/*", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "amq.topic/" "*/*", 100);
    assert (bucket);
    ipr_bucket_unlink (&bucket);

    bucket = amq_rest_fetch (rest, "amq.topic/" "*/*", 0);
    assert (!bucket);

/*
- allow put to create binding
- also allow fetch/bind in one go
- allow delete to do unbind
- handle post in Rest class
*/
    //  Close RestAPI session and clean up
    amq_rest_destroy (&rest);
    /*
    ipr_process_destroy (&process);
    ipr_dir_remove ("./archive");
    ipr_dir_remove ("./logs");
    */
</method>

</class>
