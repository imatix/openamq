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

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    ipr_looseref_list_t
        *own_queue_list;                //  List of exclusive queues
</context>

<method name = "new">
    self->own_queue_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    amq_queue_t
        *queue;                         //  Content object reference
    </local>

    //  Delete connection's exclusive queues
    while ((queue = (amq_queue_t *) ipr_looseref_pop (self->own_queue_list)))
        amq_queue_destroy (&queue);

    amq_vhost_unlink (&self->vhost);
    ipr_looseref_list_destroy (&self->own_queue_list);
</method>

<method name = "own queue" template = "function">
    <argument name = "queue" type = "amq_queue_t *">Queue reference</argument>
    assert (queue->exclusive);
    ipr_looseref_queue (self->own_queue_list, amq_queue_link (queue));
</method>

<method name = "start ok" template = "function">
    <local>
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

<method name = "open">
    //  For now, link to single global vhost object
    self->vhost = amq_vhost_link (amq_vhost);
    assert (self->vhost);
</method>

</class>
