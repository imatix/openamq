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

<import class = "amq_queue_list" />

<inherit class = "asl_server_connection" />
<option name = "basename" value = "amq_server" />

<public name = "header">
//  These are the three connection types we allow
#define AMQ_CONNECTION_TYPE_NORMAL    1
#define AMQ_CONNECTION_TYPE_CONSOLE   2
#define AMQ_CONNECTION_TYPE_CLUSTER   3
</public>

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    amq_connection_t
        *mgt_object;                    //  Management object
    amq_queue_list_t
        *own_queue_list;                //  List of exclusive queues
    amq_queue_list_t
        *wait_queue_list;               //  Queues wanting to write to us
    amq_consumer_table_t
        *consumer_table;                //  Consumers for connection
    icl_shortstr_t
        cluster_id;                     //  Cluster id for connection
    qbyte
        consumer_tag;                   //  Last consumer tag
    icl_shortstr_t
        user_name;                      //  User login name
    int
        type;                           //  User connection type
</context>

<method name = "new">
    self->own_queue_list  = amq_queue_list_new ();
    self->wait_queue_list = amq_queue_list_new ();
    self->consumer_table  = amq_consumer_table_new ();
    self->mgt_object      = amq_connection_new (amq_broker, self);
    icl_shortstr_fmt (self->cluster_id, "%s/%s", amq_broker->name, self->id);
</method>

<method name = "destroy">
    amq_connection_destroy (&self->mgt_object);
    amq_vhost_unlink       (&self->vhost);
    amq_queue_list_destroy (&self->own_queue_list);
    amq_queue_list_destroy (&self->wait_queue_list);
    amq_consumer_table_destroy (&self->consumer_table);
</method>

<method name = "own queue" template = "function">
    <argument name = "queue" type = "amq_queue_t *">Queue reference</argument>
    assert (queue->exclusive);
    amq_queue_list_push_back (self->own_queue_list, queue);
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Unbind a queue from the connection.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">The queue to unbind</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    </local>
    //
    //  Remove the queue from the list of exclusive connections
    iterator = amq_queue_list_find (amq_queue_list_begin (self->own_queue_list),
        NULL, queue);
    if (iterator)
        amq_queue_list_erase (self->own_queue_list, iterator);
</method>

<method name = "ready" template = "function">
    //  If cluster is booting, let through only cluster or console connections
    if (amq_cluster->enabled && !amq_cluster->ready)
        rc = self->type != AMQ_CONNECTION_TYPE_NORMAL;
    else
        rc = TRUE;
</method>

<method name = "error">
    <doc>
    If the connection is alive, raises an exception with the specified
    reply code/text, otherwise prints it to the console.
    </doc>
    <argument name = "self" type = "amq_server_connection_t *" />
    <argument name = "reply code" type = "dbyte" >Error code</argument>
    <argument name = "reply text" type = "char *">Error text</argument>
    if (self)
        amq_server_connection_exception (self, reply_code, reply_text);
    else
        icl_console_print ("E: connection exception: (%d) %s", reply_code, reply_text);
</method>

<method name = "start ok" template = "function">
    //
    //  Server only supports plain authentication for now
    //
    if (amq_server_config_trace_login (amq_server_config))
        icl_console_print ("L: start-ok from=%s product=%s version=%s",
            self->client_address,
            self->client_product,
            self->client_version);

    switch (s_auth_plain (self, method)) {
        case AMQ_CONNECTION_TYPE_NORMAL:
            self->authorised = TRUE;
            break;
        case AMQ_CONNECTION_TYPE_CONSOLE:
            self->authorised = TRUE;
            break;
        case AMQ_CONNECTION_TYPE_CLUSTER:
            self->authorised = TRUE;
            self->nowarning  = TRUE;    //  No disconnect warnings for cluster proxy
            break;
        default:
            self_exception (self, ASL_ACCESS_REFUSED, "Invalid authentication data");
    }
</method>

<method name = "open">
    //  For now, link to single global vhost object
    self->vhost = amq_vhost_link (amq_vhost);
    assert (self->vhost);

    //  If locked, allow only cluster & console access
    if (amq_broker->locked
    &&  self->type == AMQ_CONNECTION_TYPE_NORMAL)
        self_exception (self, ASL_ACCESS_REFUSED, "Connections not allowed at present");
    else
    if (amq_cluster->enabled) {
        if (streq (method->virtual_host, amq_server_config_cluster_vhost (amq_server_config))) {
            //  Don't redirect insisting or cluster/console clients
            if (method->insist)
                amq_server_agent_connection_open_ok (self->thread, amq_cluster->known_hosts);
            else
            if (self->type != AMQ_CONNECTION_TYPE_NORMAL)
                amq_server_agent_connection_open_ok (self->thread, NULL);
            else
                amq_cluster_balance_client (amq_cluster, self);
        }
        else {
            icl_console_print ("E: client at %s tried to connect to invalid vhost '%s'",
                self->client_address, method->virtual_host);
            self_exception (self, ASL_INVALID_PATH, "Cluster vhost is not correct");
        }
    }
    else
    //  TODO: document or remove this debugging feature
    if (streq (method->virtual_host, "/redirect"))
        amq_server_agent_connection_redirect (self->thread, amq_broker->host, NULL);
    else
        amq_server_agent_connection_open_ok (self->thread, NULL);
</method>

<method name = "wait queue" template = "function">
    <doc>
    Attach a queue to our wait list, which means the queue will be
    signalled when the connection is free to send messages again.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue reference</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    </local>
    //
    //  Only store the queue if it's not already on the list
    //  This is a very temporary way of limiting the number of
    //  times a queue gets onto this list... once we're happy
    //  with the mechanism we'll move it into the vhost.
    //
    iterator = amq_queue_list_begin (self->wait_queue_list);
    while (iterator) {
        if (queue == *iterator) {
            queue = NULL;
            break;
        }
        else
            iterator = amq_queue_list_next (iterator);
    }
    if (queue)
        amq_queue_list_push_back (self->wait_queue_list, queue);

    if (amq_queue_list_size (self->wait_queue_list) > 100) {
        icl_console_print ("## Queue wakeup overflow, please notify Pieter");
        exit (1);
    }
</method>

<method name = "finish output" template = "function">
    <local>
    amq_queue_t
        *queue;                         //  Queue to dispatch
    int
        list_size;                      //  Number of queues to process
    </local>
    //
    //  Tell all waiting queues to dispatch
    //
    list_size = amq_queue_list_size (self->wait_queue_list);
    while (list_size && !amq_queue_list_empty (self->wait_queue_list)) {
        queue = *amq_queue_list_begin (self->wait_queue_list);
        amq_queue_list_pop_front (self->wait_queue_list);
        amq_queue_dispatch (queue);
        list_size--;                    //  In case list is being modified
    }   
</method>

<private name = "header">
static int
    s_auth_plain ($(selftype) *self, amq_server_connection_start_ok_t *method);
</private>

<private name = "body">
//  Sets and returns connection type, zero indicating an error

static int s_auth_plain (
    $(selftype) *self,
    amq_server_connection_start_ok_t *method)
{
    asl_field_list_t
        *fields;                        //  Decoded response table
    char
        *login = NULL,                  //  Login string
        *password = NULL,               //  Password string
        *type;                          //  Type name from config
    ipr_config_t
        *config;                        //  Current server config file

    //  Check name and password
    if ((fields = asl_field_list_new (method->response)) == NULL) {
        self_exception (self, ASL_SYNTAX_ERROR, "Invalid response field table");
        return (0);
    }
    login    = asl_field_list_string (fields, "login");
    password = asl_field_list_string (fields, "password");
    if (!login || !password) {
        self_exception (self, ASL_SYNTAX_ERROR, "Missing authentication data");
        asl_field_list_unlink (&fields);
        return (0);
    }
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (!config->located) {
        icl_console_print ("E: no 'plain' security defined in server config");
        self_exception (self, ASL_INTERNAL_ERROR, "Bad server configuration");
        asl_field_list_unlink (&fields);
        return (0);
    }
    //  Now check user login and password and set type if found
    ipr_config_locate (config, "user", NULL);
    while (config->located) {
        if (streq (login,    ipr_config_get (config, "name", ""))
        &&  streq (password, ipr_config_get (config, "password", ""))) {
            icl_shortstr_cpy (self->user_name, login);
            type = ipr_config_get (config, "type", "");
            if (streq (type, "normal"))
                self->type = AMQ_CONNECTION_TYPE_NORMAL;
            else
            if (streq (type, "console"))
                self->type = AMQ_CONNECTION_TYPE_CONSOLE;
            else
            if (streq (type, "cluster"))
                self->type = AMQ_CONNECTION_TYPE_CLUSTER;
            else {
                icl_console_print ("E: invalid user type '%s' in config", type);
                self_exception (self, ASL_INTERNAL_ERROR, "Bad server configuration");
            }
            if (amq_server_config_trace_login (amq_server_config))
                icl_console_print ("L: login    from=%s user=%s type=%s",
                    self->client_address, login, type);
            break;
        }
        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
    asl_field_list_unlink (&fields);
    return (self->type);
}
</private>

</class>
