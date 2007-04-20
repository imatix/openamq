<?xml?>
<!--
    Copyright (c) 1996-2007 iMatix Corporation

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
    name      = "fmq_session"
    comment   = "FileMQ session framework"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Provides a simple framework for accessing the AMQP WireAPI, based on an
object class called fmq_session.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<public>
//  Events from a wait
#define FMQ_SESSION_ARRIVED   1         //  A message arrived
#define FMQ_SESSION_RETURNED  2         //  A message was undeliverable
#define FMQ_SESSION_INTERRUPT 3         //  Application was interrupted
#define FMQ_SESSION_NO_BROKER 4         //  Broker disappeared
#define FMQ_SESSION_EXCEPTION 5         //  Some fatal error happened
</public>

<context>
    amq_client_connection_t
        *connection;                    //  WireAPI connection
    amq_client_session_t
        *session;                       //  WireAPI session
    amq_content_basic_t
        *content;                       //  Current content
    icl_longstr_t
        *auth_data;                     //  Login authorisation data
    dbyte
        access_ticket;                  //  Access ticket
    Bool
        robust;                         //  Retry if broker not available?
    icl_shortstr_t
        vhost;                          //  Virtual host name
    icl_shortstr_t
        queue_name,                     //  Name of service queue
        reply_to;                       //  Reply-to from current content
    int
        trace;                          //  WireAPI trace level 0-3
</context>

<import class = "amq_client_connection" />
<import class = "amq_client_session" />
<import class = "amq_content_basic" />

<method name = "new">
    self_set_login  (self, "guest", "guest");
    self_set_vhost  (self, "/");
    self_set_robust (self, TRUE);
    self_set_trace  (self, 0);
</method>

<method name = "destroy">
    self_close (self);
    icl_longstr_destroy (&self->auth_data);
    amq_content_basic_destroy (&self->content);
</method>

<method name = "set login" template = "function">
    <argument name = "username" type = "char *" />
    <argument name = "password" type = "char *" />
    icl_longstr_destroy (&self->auth_data);
    self->auth_data = amq_client_connection_auth_plain (username, password);
</method>

<method name = "set vhost" template = "function">
    <argument name = "vhost" type = "char *" />
    icl_shortstr_cpy (self->vhost, vhost);
</method>

<method name = "set robust" template = "function">
    <argument name = "robust" type = "Bool" />
    self->robust = robust;
</method>

<method name = "set trace" template = "function">
    <argument name = "trace" type = "int" />
    self->trace = trace;
</method>

<method name = "open" template = "function">
    <argument name = "hostname"  type = "char *" />
    <argument name = "client id" type = "char *" />
    <local>
    ipr_token_list_t
        *host_list;                     //  List of known hosts
    ipr_token_t
        *token;                         //  Next host to try
    </local>
    //
    if (self->session)
        amq_client_session_destroy (&self->session);
    if (self->connection)
        amq_client_connection_destroy (&self->connection);

    host_list = ipr_token_split (hostname);
    token = ipr_token_list_first (host_list);
    while (token) {
        self->connection = amq_client_connection_new (
            token->value, self->vhost, self->auth_data, client_id, self->trace, 30000);
        if (self->connection) {
            ipr_token_unlink (&token);
            break;
        }
        token = ipr_token_list_next (&token);
    }
    ipr_token_list_destroy (&host_list);

    if (self->connection)
        self->session = amq_client_session_new (self->connection);
    else
        rc = 1;                         //  Can't connect
</method>

<method name = "listen" template = "function">
    <doc>
    Creates an exclusive auto-delete queue with the specified name, which
    can be empty to create an auto-named queue. The queue name is stored
    in self->queue_name.
    </doc>
    <argument name = "queue name" type = "char *" />
    //
    rc = amq_client_session_queue_declare (
        self->session,              //  Session reference
        self->access_ticket,        //  Access ticket
        queue_name,                 //  Queue name
        FALSE,                      //  Passive only
        FALSE,                      //  Durable queue
        TRUE,                       //  Exclusive queue
        TRUE,                       //  Auto-delete queue when unused
        NULL                        //  Additional arguments
    );
    if (!rc) {
        icl_shortstr_cpy (self->queue_name, self->session->queue);
        rc = amq_client_session_basic_consume (
            self->session,          //  Session reference
            self->access_ticket,    //  Access ticket
            self->queue_name,       //  Queue name
            NULL,                   //  Consumer tag
            FALSE,                  //  Do not deliver own messages
            TRUE,                   //  No acknowledgement needed
            TRUE,                   //  Request exclusive access
            NULL                    //  Additional arguments
        );
    }
</method>

<method name = "request stage" template = "function">
    <doc>
    P:STAGE
    C:STAGE-OK | EXCEPTION

    C:STAGE
    S:STAGE-OK | EXCEPTION
    
    Stages a fragment of data to a file.  To request a restart point,
    stage an empty fragment.  Data in a file must always be staged in
    order.  Staged data is ignored if the file is already fully staged.
    </doc>
    <argument name = "send to"  type = "char *"        >Destination address</argument>
    <argument name = "bucket"   type = "ipr_bucket_t *">Bucket of data</argument>
    <argument name = "filename" type = "char *"        >File name</argument>
    <argument name = "filesize" type = "int64_t"       >File size</argument>
    <argument name = "filetime" type = "time_t"        >File revision time/date</argument>
    <argument name = "offset"   type = "int64_t"       >Offset of fragment</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILENAME", filename);
    amq_content_basic_set_headers_field (content, "FILESIZE", "%"PRId64, filesize);
    amq_content_basic_set_headers_field (content, "FILETIME", "%ld", filetime);
    amq_content_basic_set_headers_field (content, "OFFSET",   "%"PRId64, offset);
    if (bucket)
        amq_content_basic_record_body (content, bucket);
    rc = s_send_message (self, &content, "STAGE", send_to);
</method>

<method name = "reply stage ok" template = "function">
    <doc>
    Tells the sender how much of the file has already been staged, where
    files are assumed to be identical if they have the same name, size, and
    time.
    </doc>
    <argument name = "filename" type = "char *" >File name</argument>
    <argument name = "file id"  type = "char *" >File identifier</argument>
    <argument name = "staged"   type = "int64_t">Amount staged so far</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILENAME", filename);
    amq_content_basic_set_headers_field (content, "FILE-ID",  file_id);
    amq_content_basic_set_headers_field (content, "STAGED",   "%"PRId64, staged);
    rc = s_send_message (self, &content, "STAGE-OK", self->reply_to);
</method>

<method name = "reply exception" template = "function">
    <doc>
    Signals a fatal error; 
    </doc>
    <argument name = "text" type = "char *">Reason for error</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "TEXT", text);
    rc = s_send_message (self, &content, "EXCEPTION", self->reply_to);
</method>

<method name = "request publish" template = "function">
    <doc>
    P:PUBLISH
    S:PUBLISH-OK | EXCEPTION
    </doc>
    <argument name = "send to"  type = "char *">Destination address</argument>
    <argument name = "file id"  type = "char *">File identifier</argument>
    <argument name = "channel"  type = "char *">Channel to publish to</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID",  file_id);
    amq_content_basic_set_headers_field (content, "CHANNEL", channel);
    rc = s_send_message (self, &content, "PUBLISH", send_to);
</method>        

<method name = "reply publish ok" template = "function">
    <argument name = "file id"     type = "char *">File identifier</argument>
    <argument name = "subscribers" type = "size_t">Number of subscribers</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID", file_id);
    amq_content_basic_set_headers_field (content, "SUBSCRIBERS", "%ld", subscribers);
    rc = s_send_message (self, &content, "PUBLISH-OK", self->reply_to);
</method>

<method name = "request subscribe" template = "function">
    <doc>
    C:SUBSCRIBE
    S:SUBSCRIBE-OK | EXCEPTION
    </doc>
    <argument name = "send to"  type = "char *">Destination address</argument>
    <argument name = "channel"  type = "char *">Channel pattern to subscribe to</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "CHANNEL", channel);
    rc = s_send_message (self, &content, "SUBSCRIBE", send_to);
</method>

<method name = "reply subscribe ok" template = "function">
    <argument name = "channels" type = "size_t">Number of channels subscribed to</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "CHANNELS", "%ld", channels);
    rc = s_send_message (self, &content, "SUBSCRIBE-OK", self->reply_to);
</method>

<method name = "request deliver" template = "function">
    <doc>
    S:DELIVER
    C:DELIVER-OK | EXCEPTION
    </doc>
    <argument name = "send to"  type = "char *">Destination address</argument>
    <argument name = "file id"  type = "char *">File identifier</argument>
    <argument name = "channel"  type = "char *">Channel published on</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID", file_id);
    amq_content_basic_set_headers_field (content, "CHANNEL", channel);
    rc = s_send_message (self, &content, "DELIVER", send_to);
</method>

<method name = "reply deliver ok" template = "function">
    <argument name = "file id" type = "char *">File identifier</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID", file_id);
    rc = s_send_message (self, &content, "DELIVER-OK", self->reply_to);
</method>

<method name = "request sync" template = "function">
    <doc>
    C:SYNC
    S:SYNC-OK | EXCEPTION
    </doc>
    <argument name = "send to"  type = "char *">Destination address</argument>
    <argument name = "file id"  type = "char *">Sync to identifier</argument>
    <argument name = "channel"  type = "char *">Channel published on</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID", file_id);
    amq_content_basic_set_headers_field (content, "CHANNEL", channel);
    rc = s_send_message (self, &content, "SYNC", send_to);
</method>

<method name = "reply sync ok" template = "function">
    <argument name = "file id" type = "char *">File identifier</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_new ();
    amq_content_basic_set_headers_field (content, "FILE-ID", file_id);
    rc = s_send_message (self, &content, "SYNC-OK", self->reply_to);
</method>

<method name = "wait" template = "function">
    <doc>
    Wait for something to happen on the session.  Returns when anything
    interesting happens; see the list of session events.  If content
    arrives or is returned, sets self->content.
    </doc>
    assert (self->session);
    assert (self->connection);

    FOREVER {
        amq_client_session_wait (self->session, 1000);
        if (amq_client_session_get_basic_arrived_count (self->session)) {
            amq_content_basic_destroy (&self->content);
            self->content = amq_client_session_basic_arrived (self->session);
            icl_shortstr_cpy (self->reply_to, self->content->reply_to);
            rc = FMQ_SESSION_ARRIVED;
            break;
        }
        else
        if (amq_client_session_get_basic_returned_count (self->session)) {
            amq_content_basic_destroy (&self->content);
            self->content = amq_client_session_basic_returned (self->session);
            rc = FMQ_SESSION_RETURNED;
            break;
        }
        else
        if (self->connection->interrupt) {
            //  Our process was killed (Ctrl-C)
            rc = FMQ_SESSION_INTERRUPT;
            break;
        }
        else
        if (!self->connection->alive && self->connection->reply_code == 100) {
            //  Remote server closed on us
            rc = FMQ_SESSION_NO_BROKER;
            break;
        }
        else
        if (!self->connection->alive) {
            //  Remote server kicked us, don't retry
            rc = FMQ_SESSION_EXCEPTION;
            break;
        }
    }
</method>

<method name = "close" template = "function">
    amq_client_session_destroy (&self->session);
    amq_client_connection_destroy (&self->connection);
</method>

<private name = "header">
static int s_send_message (
    $(selftype) *self,                  //  Our FMQ session
    amq_content_basic_t **content_p,    //  Content pointer ref
    char *name,                         //  Message name
    char *send_to);                     //  Destination queue
</private>

<private name = "footer">
static int s_send_message (
    $(selftype) *self,                  //  Our FMQ session
    amq_content_basic_t **content_p,    //  Content pointer ref
    char *name,                         //  Message name
    char *send_to)                      //  Destination queue
{
    int
        rc;
        
    amq_content_basic_set_type (*content_p, name);
    amq_content_basic_set_reply_to (*content_p, self->queue_name);
    
    rc = amq_client_session_basic_publish (
        self->session,                  //  Session reference
        *content_p,                     //  Message to sent
        self->access_ticket,            //  Access ticket
        NULL,                           //  Default exchange
        send_to,                        //  Route to destination
        TRUE,                           //  Mandatory routing
        TRUE);                          //  Immediate delivery
    amq_content_basic_destroy (content_p);
    return (rc);
}
</private>

<method name = "selftest">
    <local>
    fmq_session_t
        *session;
    int
        rc;
    </local>
    //
    session = fmq_session_new ();
    fmq_session_set_trace (session, 1);
    fmq_session_set_login (session, "guest", "guest");
    rc = fmq_session_open (session, "localhost", "test client");
    if (rc) {
        icl_console_print ("E: can't connect to AMQP broker");
        exit (1);
    }
    fmq_session_listen (session, "me");

    fmq_session_request_stage (session, "me", NULL, "somefile", 100, 0, 200);
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);
    
    fmq_session_reply_stage_ok (session, "somefile", "fileid", 1234);
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_reply_exception (session, "error text");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_request_publish (session, "me", "fileid", "channel");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_reply_publish_ok (session, "fileid", 100);
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_request_subscribe (session, "me", "channel");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_reply_subscribe_ok (session, 100);
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_request_deliver (session, "me", "fileid", "channel");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_reply_deliver_ok (session, "fileid");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_request_sync (session, "me", "fileid", "channel");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);

    fmq_session_reply_sync_ok (session, "fileid");
    rc = fmq_session_wait (session);
    assert (rc == FMQ_SESSION_ARRIVED);
    
    fmq_session_close     (session);
    fmq_session_destroy  (&session);
</method>

</class>

