<?xml?>
<!--
    *** GENERATED FROM demo_client_proto.asl BY ASL_GEN USING GSL/4 ***

    
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
    name      = "demo_client_connection"
    comment   = "demo_client connection class"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
A demo_client_connection object establishes and manages a single
connection to a demo server. The connection can hold
many sessions - each is a demo_client_session object. To use this
API, first create a connection to the server, then create one or
more sessions and do work on the sessions. Each session represents
a serial stream of work. In simple applications, a single session
is usually sufficient. In multithreaded applications, each thread
will want its own session.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_init" />

<import class = "asl" />
<import class = "demo_constants" />
<import class = "demo_client_session" />
<import class = "demo_content_basic"/>

<context>
    smt_thread_t
        *thread;                        //  Thread for this connection
    smt_method_queue_t
        *method_queue;                  //  Replies from protocol agent
    dbyte
        channel_nbr;                    //  Top channel opened so far
    Bool
        silent;                         //  Don't print errors
    Bool
        alive;                          //  Connection is alive?
    Bool
        interrupt;                      //  Application was interrupted?
    Bool
        shutdown;                       //  We were shutdown
    int
        trace,                          //  Trace level 0-3
        timeout;                        //  Connection timeout, msecs
    char
        *error_text;                    //  Last error cause
    Bool
        direct;                         //  Use Direct Protocol
    icl_shortstr_t
        host;                           //  Host we connected to
    byte
        version_major;                  //  protocol major version
    byte
        version_minor;                  //  protocol major version
    icl_longstr_t *
        server_properties;              //  server properties
    icl_longstr_t *
        mechanisms;                     //  available security mechanisms
    icl_longstr_t *
        locales;                        //  available message locales
    icl_longstr_t *
        challenge;                      //  security challenge data
    dbyte
        channel_max;                    //  proposed maximum channels
    qbyte
        frame_max;                      //  proposed maximum frame size
    dbyte
        heartbeat;                      //  desired heartbeat delay
    icl_shortstr_t
        known_hosts;                    //  list of known hosts
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
    icl_shortstr_t
        server_host;                    //  Reported by server
    icl_shortstr_t
        server_port;                    //  Just the port number
    icl_shortstr_t
        server_product;
    icl_shortstr_t
        server_version;
    icl_shortstr_t
        server_platform;
    icl_shortstr_t
        server_copyright;
    icl_shortstr_t
        server_information;
    icl_shortstr_t
        server_instance;
    icl_shortstr_t
        server_identifier;
    icl_shortstr_t
        id;                             //  Server-assigned ID
    Bool
        server_direct;                  //  Server proposes Direct Mode?
</context>

<method name = "new">
    <doc>
    Create new connection to server.

    The host argument specifies a server name or IP address, optionally
    ending in ':' plus a port number.

    The virtual_host name specifies the virtual host to which the connection
    will connect.  The default virtual host is "/".

    The auth_data provides an authentication block, used to login to the
    server.  To create a plain-text authentication block, use the auth_plain
    method.

    The trace argument sets the trace level for the client API.

    The timeout argument governs all synchronous exchanges with the server
    - if the server does not respond within this time, the connection treats
    it as a fatal error.  A timeout of zero means "infinite".  A good value
    for fast networks is five to ten seconds; for a slower network, a value
    of 30 seconds or more is reasonable.
    </doc>
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *">Virtual host</argument>
    <argument name = "auth data" type = "icl_longstr_t *">Authentication data</argument>
    <argument name = "instance" type = "char*">Client instance name</argument>
    <argument name = "trace" type = "int">Trace level, 0 - 3</argument>
    <argument name = "timeout" type = "int">Connection timeout, msecs</argument>
    <local>
    char
        *env_value;                 //  Get environment setting
    </local>
    //
    self->method_queue = smt_method_queue_sync_new ();
    self->timeout = timeout;
    self->trace = trace;
    self->error_text = "no error";
    icl_shortstr_cpy (self->host, host);

    env_value = getenv ("WIREAPI_SILENT");
    if (env_value && streq (env_value, "1"))
        self->silent = TRUE;

    self->thread = demo_client_agent_connection_thread_new (
        self->method_queue,
        &self->alive,
        &self->interrupt,
        &self->reply_code,
        self->reply_text,
        host,
        virtual_host,
        auth_data,
        instance,
        trace,
        timeout);

    //  Let the SMT engine take over until we get a reply on our queue
    if (self->thread) {
        if (s_wait_for_methods (self, self->timeout, TRUE) == 0) {
            self->alive = TRUE;         //  Connection is ready for use
            //  Decide whether to use Direct Mode or not
            if (demo_client_config_direct (demo_client_config) || self->server_direct)
                self->direct = TRUE;
        }
        else
            self_destroy (&self);
    }
</method>

<method name = "destroy">
    <doc>
    Close the connection and all sessions on it.  Call this when your
    application ends, or if you want to break the connection to the
    server.  This performs a clean shut-down of the connection, so any
    server-side resources are freed.
    </doc>
    //
    if (self->alive) {
        demo_client_agent_connection_close (self->thread);
        while (self->alive) {
            //  Wait until we get back an CLOSED method, or timeout
            s_wait_for_methods (self, self->timeout, FALSE);
        }
    }
    icl_longstr_destroy (&self->server_properties);
    icl_longstr_destroy (&self->mechanisms);
    icl_longstr_destroy (&self->locales);
    icl_longstr_destroy (&self->challenge);
    smt_method_queue_destroy (&self->method_queue);
    smt_thread_unlink (&self->thread);
</method>

<method name = "auth plain" return = "auth data">
    <doc>
    Formats a new authentication data block for a plain login.
    </doc>
    <argument name = "login"    type = "char *" pass = "in">User login name</argument>
    <argument name = "password" type = "char *" pass = "in">User password</argument>
    <declare name = "auth data" type = "icl_longstr_t *">Authentication data</declare>
    //
    auth_data = ipr_sasl_plain_encode (login, password);
</method>

<method name = "wait" template = "function">
    <doc>
    Waits for any activity on the connection. Returns -1 if the connection
    ended abnormally. If the timeout is zero, waits forever.
    </doc>
    <argument name = "timeout" type = "int" pass = "in">Timeout, in milliseconds</argument>
    //
    if (self->alive) {
        if (s_wait_for_methods (self, timeout, FALSE))
            rc = -1;
    }
    else
        rc = -1;
</method>

<method name = "set silent" template = "function">
    <argument name = "silent" type = "Bool" />
    //
    self->silent = silent;
</method>

<method name = "set timeout" template = "function">
    <argument name = "timeout" type = "Bool" />
    //
    self->timeout = timeout;
</method>

<method name = "get silent" return = "silent">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "silent" type = "Bool" />
    //
    silent = self->silent;
</method>

<method name = "get alive" return = "alive">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "alive" type = "Bool" />
    //
    alive = self->alive;
</method>

<method name = "get error text" return = "error text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "error text" type = "char *" />
    //
    error_text = self->error_text;
</method>

<method name = "get version_major" return = "version_major">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "version_major" type = "int" />
    //
    version_major = self->version_major;
</method>

<method name = "get version_minor" return = "version_minor">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "version_minor" type = "int" />
    //
    version_minor = self->version_minor;
</method>

<method name = "get server_properties" return = "server_properties">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "server_properties" type = "icl_longstr_t *" />
    //
    server_properties = self->server_properties;
</method>

<method name = "get mechanisms" return = "mechanisms">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "mechanisms" type = "icl_longstr_t *" />
    //
    mechanisms = self->mechanisms;
</method>

<method name = "get locales" return = "locales">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "locales" type = "icl_longstr_t *" />
    //
    locales = self->locales;
</method>

<method name = "get challenge" return = "challenge">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "challenge" type = "icl_longstr_t *" />
    //
    challenge = self->challenge;
</method>

<method name = "get channel_max" return = "channel_max">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "channel_max" type = "int" />
    //
    channel_max = self->channel_max;
</method>

<method name = "get frame_max" return = "frame_max">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "frame_max" type = "qbyte" />
    //
    frame_max = self->frame_max;
</method>

<method name = "get heartbeat" return = "heartbeat">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "heartbeat" type = "int" />
    //
    heartbeat = self->heartbeat;
</method>

<method name = "get known_hosts" return = "known_hosts">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "known_hosts" type = "char *" />
    //
    known_hosts = self->known_hosts;
</method>

<method name = "get reply_code" return = "reply_code">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_code" type = "int" />
    //
    reply_code = self->reply_code;
</method>

<method name = "get reply_text" return = "reply_text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_text" type = "char *" />
    //
    reply_text = self->reply_text;
</method>

<method name = "get class_id" return = "class_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "class_id" type = "int" />
    //
    class_id = self->class_id;
</method>

<method name = "get method_id" return = "method_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "method_id" type = "int" />
    //
    method_id = self->method_id;
</method>

<private name = "header">
static int
    s_wait_for_methods ($(selftype) *self, int timeout, Bool blocking);
</private>

<private name = "footer">
/*  Waits for methods from the protocol agent.  If the blocking argument
    is true, returns only when receiving a synchronous reply method.  If not,
    returns when any incoming method is received.  Returns 0 if ok, -1 if
    the connection is closed or a blocking wait timed-out.
 */
static int
s_wait_for_methods (
    $(selftype) *self,
    int timeout,
    Bool blocking)
{
    smt_method_t
        *smt_method = NULL;
    demo_client_method_t
        *proto_method;
    int
        rc = 0;                         //  OK by default

    FOREVER {
        smt_method_destroy (&smt_method);
        smt_method_queue_wait (self->method_queue, timeout);
        smt_method = smt_method_queue_pop (self->method_queue);
        //  If we don't have a method, we timed-out
        if (!smt_method) {
            if (blocking) {
                self->error_text = "No reply received - timeout";
                rc = -1;
            }
            break;
        }
        else
        //  ALMOST means we should wait for the next method...
        if (smt_method->result == DEMO_CLIENT_REPLY_ALMOST) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method)
                demo_client_method_execute (proto_method, self, NULL);
        }
        else
        //  INTERN is an internal synchronisation from the agent
        if (smt_method->result == DEMO_CLIENT_REPLY_INTERN)
            break;                      //  We can return to caller
        else
        //  METHOD means the agent is sending us either:
        //    - a sync method, which is a reply to a sync request
        //    - an async method, which we want to process and then continue
        //
        //  We always stop when we get a sync method. If we have an async
        //  method, we stop only if the queue is empty and we're not blocking.
        //  Blocking means we want a sync method within the timeout.
        //
        if (smt_method->result == DEMO_CLIENT_REPLY_METHOD) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method) {
                demo_client_method_execute (proto_method, self, NULL);
                if (proto_method->sync)
                    break;              //  Got synchronous method, end
                else
                if (smt_method_queue_isempty (self->method_queue) && !blocking)
                    break;              //  Got final async method but not blocking
            }
            else
                break;                  //  Synchronisation from agent
        }
        else
        if (smt_method->result == DEMO_CLIENT_REPLY_CLOSED) {
            self->alive = FALSE;        //  Connection is closed
            rc = -1;                    //  Signal connection is closed
            break;
        }
        else {
            self->error_text = "Internal error - invalid reply method from protocol agent";
            icl_console_print ("E: invalid reply result %d", smt_method->result);
        }
    }
    smt_method_destroy (&smt_method);
    return (rc);
}
</private>

<method name = "initialise">
    <doc internal = "1">
    Initialise the connection class.  If the application has not already
    loaded config data, loads this from wireapi.cfg.
    </doc>
    if (!demo_client_config) {
        demo_client_config = demo_client_config_new ();
        demo_client_config_load_xmlfile (demo_client_config, "wireapi.cfg", FALSE);
    }
    smt_initialise ();
    demo_client_agent_init ();
</method>

<method name = "terminate">
    demo_client_config_destroy (&demo_client_config);
</method>

<method name = "selftest" />

</class>
