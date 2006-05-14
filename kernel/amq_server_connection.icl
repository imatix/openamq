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
//  These are the connection groups we allow
#define AMQ_CONNECTION_GROUP_NORMAL    1
#define AMQ_CONNECTION_GROUP_SUPER     2
#define AMQ_CONNECTION_GROUP_CLUSTER   3
#define CONNECTION_IS_USER(c)    ((c) == 1 || (c) == 2)
#define CONNECTION_IS_CONTROL(c) ((c) == 3 || (c) == 4)
</public>

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    amq_connection_t
        *mgt_object;                    //  Management object
    amq_queue_list_t
        *own_queue_list;                //  List of exclusive queues
    amq_consumer_table_t
        *consumer_table;                //  Consumers for connection
    icl_shortstr_t
        cluster_id;                     //  Cluster id for connection
    qbyte
        consumer_tag;                   //  Last consumer tag
    qbyte
        per_client;                     //  Messages owned by this client
    icl_shortstr_t
        user_name;                      //  User login name
    int
        group;                          //  User connection group
</context>

<method name = "new">
    self->own_queue_list = amq_queue_list_new ();
    self->consumer_table = amq_consumer_table_new ();
    self->mgt_object     = amq_connection_new (amq_broker, self);
    icl_shortstr_fmt (self->cluster_id, "%s/%s", amq_broker->name, self->id);
</method>

<method name = "destroy">
    <local>
    amq_queue_list_iterator_t
       iterator;
    </local>
    //
    //  Firstly, send notification of connection termination to all
    //  exclusive queues created by the connection, so that they can be
    //  destroyed even if there is no consumer on the queue.
    for (iterator = amq_queue_list_begin (self->own_queue_list);
          iterator != NULL;
          iterator = amq_queue_list_next (iterator))
        amq_queue_unbind_connection (*iterator);

    amq_connection_destroy (&self->mgt_object);
    amq_vhost_unlink       (&self->vhost);
    amq_queue_list_destroy (&self->own_queue_list);
</method>

<method name = "free">
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
    iterator = amq_queue_list_find (
        amq_queue_list_begin (self->own_queue_list), NULL, queue);
    if (iterator)
        amq_queue_list_erase (self->own_queue_list, iterator);
</method>

<method name = "ready" template = "function">
    //  If cluster is booting, let through only cluster or console connections
    if (amq_cluster->enabled && !amq_cluster->ready)
        rc = CONNECTION_IS_CONTROL (self->group);
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
    //
    if (self->group == AMQ_CONNECTION_GROUP_CLUSTER) {
        icl_console_print ("E: cluster connection error (%d) %s", reply_code, reply_text);
        exit (1);
    }
    else
    if (self)
        amq_server_connection_exception (self, reply_code, reply_text);
    else
        asl_log_print (amq_broker->alert_log,
            "E: connection exception: (%d) %s", reply_code, reply_text);
</method>

<method name = "start ok" template = "function">
    //
    //  Server only supports plain authentication for now
    //
    asl_log_print (amq_broker->daily_log,
        "I: start login from=%s product=%s version=%s",
        self->client_address, self->client_product, self->client_version);

    switch (s_auth_plain (self, method)) {
        case AMQ_CONNECTION_GROUP_NORMAL:
        case AMQ_CONNECTION_GROUP_SUPER:
            self->authorised = TRUE;
            break;
        case AMQ_CONNECTION_GROUP_CLUSTER:
            self->authorised = TRUE;
            self->nowarning  = TRUE;    //  No disconnect warnings for cluster proxy
            break;
        default:
            self_exception (self, ASL_ACCESS_REFUSED, "Invalid authentication data");
    }
</method>

<method name = "open">
    //  For now, link to single global vhost object
    self->vhost = amq_vhost_link (amq_broker->vhost);

    if (!self->vhost)
        self_exception (self, ASL_ACCESS_REFUSED, "Server is not ready");
    else
    //  If locked, allow only cluster & console access
    if (amq_broker->locked && CONNECTION_IS_USER (self->group))
        self_exception (self, ASL_ACCESS_REFUSED, "Connections not allowed at present");
    else
    if (amq_cluster->enabled) {
        if (streq (method->virtual_host, amq_server_config_cluster_vhost (amq_server_config))) {
            //  Don't redirect insisting or cluster/console clients
            if (method->insist)
                amq_server_agent_connection_open_ok (self->thread, amq_cluster->known_hosts);
            else
            if (CONNECTION_IS_CONTROL (self->group))
                amq_server_agent_connection_open_ok (self->thread, NULL);
            else
                amq_cluster_balance_client (amq_cluster, self);
        }
        else {
            asl_log_print (amq_broker->alert_log,
                "E: client at %s tried to connect to invalid vhost '%s'",
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
static uint
    s_collect_plain_token (byte *data, char *token, uint token_end);
</private>

<private name = "body">
//  Sets and returns connection group, zero indicating an error

static int s_auth_plain (
    $(selftype) *self,
    amq_server_connection_start_ok_t *method)
{
    icl_shortstr_t
        login,
        password;
    uint
        token_null;
    char
        *group;                         //  Group name from config
    ipr_config_t
        *config;                        //  Current server config file

    //  method->response holds PLAIN data in format "[NULL]login[NULL]password"
    token_null = s_collect_plain_token (
        method->response->data, password, method->response->cur_size);
    if (token_null) 
        s_collect_plain_token (method->response->data, login, token_null);

    if (strnull (login) || strnull (password)) {
        self_exception (self, ASL_SYNTAX_ERROR, "Missing authentication data");
        return (0);
    }
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (!config->located) {
        asl_log_print (amq_broker->alert_log,
            "E: no 'plain' security defined in server config");
        self_exception (self, ASL_INTERNAL_ERROR, "Bad server configuration");
        return (0);
    }
    //  Now check user login and password and set group if found
    ipr_config_locate (config, "user", NULL);
    while (config->located) {
        if (streq (login,    ipr_config_get (config, "name", ""))
        &&  streq (password, ipr_config_get (config, "password", ""))) {
            icl_shortstr_cpy (self->user_name, login);
            group = ipr_config_get (config, "group", "");
            if (streq (group, "normal"))
                self->group = AMQ_CONNECTION_GROUP_NORMAL;
            else
            if (streq (group, "super"))
                self->group = AMQ_CONNECTION_GROUP_SUPER;
            else
            if (streq (group, "cluster"))
                self->group = AMQ_CONNECTION_GROUP_CLUSTER;
            else {
                asl_log_print (amq_broker->alert_log,
                    "E: invalid user group '%s' in config", group);
                self_exception (self, ASL_INTERNAL_ERROR, "Bad server configuration");
            }
            asl_log_print (amq_broker->daily_log,
                "I: valid login from=%s user=%s group=%s", self->client_address, login, group);
            break;
        }
        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
    return (self->group);
}

//  Collects a token, returns offset of null octet at start
static uint
s_collect_plain_token (byte *data, char *token, uint token_end)
{
    uint
        token_null,
        token_size;

    //  Find start of token, scanning back from known end
    token_null = --token_end;
    while (token_null && data [token_null])
        token_null--;
        
    strclr (token);
    //  Token start must point to a null octet before the string
    token_size = token_end - token_null;
    if (token_size > ICL_SHORTSTR_MAX)
        asl_log_print (amq_broker->alert_log,
            "W: client used over-long authentication value - rejected");
    else {
        memcpy (token, data + token_null + 1, token_size);
        token [token_size] = 0;
    }
    return (token_null);
}
</private>

</class>
