<?xml?>
<class
    name      = "connection"
    comment   = "Simple wrapper for AMQ connection"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object" />

<import class = "session" />

<import class = "amq_client_connection" type = "private" />
<import class = "icl_system" type = "private" />

<context>
    amq_client_connection_t
        *connection;
</context>

<method name = "new">
    <argument name = "host" type = "char *" const = "1">Host to connect to</argument>
    <argument name = "virtual host" type = "char *" const = "1">Virtual host</argument>
    <argument name = "login" type = "char*" const = "1">Login</argument>
    <argument name = "password" type = "char*" const = "1">Password</argument>
    <argument name = "instance" type = "char*" const = "1">Client instance name</argument>
    <argument name = "trace" type = "int" default = "0">Trace level, 0 - 3</argument>
    <argument name = "timeout" type = "int" default = "30000">Connection timeout, msecs</argument>
    <local>
        icl_longstr_t
            *auth;
    </local>
    auth = amq_client_connection_auth_plain (login, password);
    assert (auth);
    self->connection = amq_client_connection_new (host, virtual_host, auth,
        instance, trace, timeout);
    icl_longstr_destroy (&auth);
</method>

<method name = "create session" template = "function" return = "rc">
    <declare name = "rc" type = "session_t*" />
    rc = session_new (amq_client_session_new (self->connection));
</method>

<method name = "destroy">
    amq_client_connection_destroy (&self->connection);
</method>

<method name = "selftest">
</method>

</class>
