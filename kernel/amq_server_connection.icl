<?xml?>
<class
    name      = "amq_server_connection"
    comment   = "AMQ server connection class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements the connection class for the AMQ server.
</doc>

<inherit class = "asl_server_connection" />
<option name = "basename" value = "amq_server" />

<method name = "start ok" template = "function">
    <local>
    asl_field_list_t
        *fields;                        //  Decoded responses
    asl_field_t
        *login,                         //  Login from client
        *password;                      //  Password from client
    </local>
    //
    //  Check name and password
    fields = asl_field_list_new (method->response);
    if (fields) {
        login    = asl_field_list_search (fields, "login");
        password = asl_field_list_search (fields, "password");
        if (login && password) {
            if (streq (asl_field_string (login),    "guest")
            &&  streq (asl_field_string (password), "guest"))
                self->authorised = TRUE;
            else
                self_exception (self,
                    ASL_ACCESS_REFUSED, "Invalid user and/or password");
        }
        else
            self_exception (self,
                ASL_SYNTAX_ERROR, "Incomplete response");
        asl_field_unlink (&login);
        asl_field_unlink (&password);
        asl_field_list_destroy (&fields);
    }
    else
        self_exception (self,
            ASL_SYNTAX_ERROR, "Invalid response field table");
</method>

</class>
