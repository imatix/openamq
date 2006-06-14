<?xml?>
<class
    name      = "connection"
    comment   = "Simple wrapper for AMQ connection"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object" />
<import class = "amq_client_connection" interface = "0" />
<import class = "session" interface = "1" />
<import class = "icl_system" interface = "0" />

<context>
    amq_client_connection_t
        *connection;
</context>

<method name = "new">
    <argument name = "host" type = "char *" pass = "in">Host to connect to</argument>
    <argument name = "virtual host" type = "char *" pass = "in">Virtual host</argument>
    <argument name = "login" type = "char*" pass = "in">Login</argument>
    <argument name = "password" type = "char*" pass = "in">Password</argument>
    <argument name = "instance" type = "char*" pass = "in">Client instance name</argument>
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
