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

<public name = "header">
//  These are the three connection types we allow
#define AMQ_CONNECTION_TYPE_NORMAL    1
#define AMQ_CONNECTION_TYPE_CONSOLE   2
#define AMQ_CONNECTION_TYPE_CLUSTER   3
</public>

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    ipr_looseref_list_t
        *own_queue_list;                //  List of exclusive queues
    amq_consumer_table_t
        *consumer_table;                //  Consumers for connection
    icl_shortstr_t
        cluster_id;                     //  Cluster id for connection
    qbyte
        consumer_tag;                   //  Last consumer tag
    int
        type;                           //  User connection type
</context>

<method name = "new">
    self->own_queue_list = ipr_looseref_list_new ();
    self->consumer_table = amq_consumer_table_new ();
    icl_shortstr_fmt (self->cluster_id, "%s/%s", amq_broker->name, self->id);
</method>

<method name = "destroy">
    <local>
    amq_queue_t
        *queue;                         //  Content object reference
    </local>
    ///
    //  Delete connection's exclusive queues
    while ((queue = (amq_queue_t *) ipr_looseref_pop (self->own_queue_list)))
        amq_queue_unlink (&queue);

    amq_vhost_unlink (&self->vhost);
    ipr_looseref_list_destroy (&self->own_queue_list);
    amq_consumer_table_destroy (&self->consumer_table);
</method>

<method name = "own queue" template = "function">
    <argument name = "queue" type = "amq_queue_t *">Queue reference</argument>
    assert (queue->exclusive);
    ipr_looseref_queue (self->own_queue_list, amq_queue_link (queue));
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
