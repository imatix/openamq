<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "amq_mgt_console"
    comment   = "Management console class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Implements the management console object, used by applications to
manage OpenAMQ servers.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "asl" />
<import class = "amq_cml" />
<import class = "amq_client_connection" />
<import class = "amq_client_session" />
<import class = "amq_mgt_broker" />

<public>
extern int
    g_opt_trace;                        //  Trace option
extern Bool
    g_opt_addr;                         //  Don't do RDNS lookups
</public>

<private>
int
    g_opt_trace = 0;                    //  Trace option
Bool
    g_opt_addr = FALSE;                 //  Don't do RDNS lookups
</private>

<context>
    amq_client_connection_t
        *connection;                    //  Current connection
    amq_client_session_t
        *session;                       //  Current session
    amq_mgt_broker_t
        *broker;                        //  Top-level object
</context>

<method name = "new">
    <argument name = "host" type = "char *">Server host name</argument>
    <argument name = "vhost" type = "char *">Virtual host name</argument>
    <argument name = "user" type = "char *">Login user name</argument>
    <argument name = "password" type = "char *">Login password</argument>
    <local>
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    int
        rc;
    </local>
    //
    icl_system_initialise (0, NULL);

    auth_data = amq_client_connection_auth_plain (user, password);
    self->connection = amq_client_connection_new (host, vhost, auth_data, "amq_shell", g_opt_trace, 5000);
    icl_longstr_destroy (&auth_data);

    if (self->connection) {
        self->session = amq_client_session_new (self->connection);
        assert (self->session);

        //  Create a private queue for replies from console
        rc = amq_client_session_queue_declare (self->session,
            0, NULL, FALSE, FALSE, TRUE, TRUE, NULL);

        if (!rc)
            rc = amq_client_session_basic_consume (self->session,
                0, self->session->queue, NULL, FALSE, TRUE, TRUE, NULL);

        if (!rc)
            rc = amq_client_session_queue_bind (self->session,
                0, self->session->queue, "amq.direct", self->session->queue, NULL);

        if (!rc)
            self->broker = amq_mgt_broker_new (self->session, 0);
        else {
            icl_console_print ("E: could not initialise console reply queue");
            self_destroy (&self);
        }
    }
    else {
        icl_console_print ("E: could not connect to '%s'", host);
        self_destroy (&self);
    }
</method>

<method name = "destroy">
    amq_mgt_broker_destroy (&self->broker);
    amq_client_session_destroy (&self->session);
    amq_client_connection_destroy (&self->connection);
</method>

<method name = "selftest" />

</class>
