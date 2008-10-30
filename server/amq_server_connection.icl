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
</public>

<context>
    amq_vhost_t
        *vhost;                         //  Parent virtual host
    amq_queue_list_t
        *own_queue_list;                //  List of exclusive queues
    amq_consumer_table_t
        *consumer_table;                //  Consumers for connection
    qbyte
        consumer_tag;                   //  Last consumer tag
    icl_shortstr_t
        user_name;                      //  User login name
    int
        group;                          //  User connection group
</context>

<method name = "new">
    self->own_queue_list = amq_queue_list_new ();
    self->consumer_table = amq_consumer_table_new ();
</method>

<method name = "destroy">
    <local>
    amq_queue_list_iter_t *
       iterator;
    </local>
    //
    //  Firstly, send notification of connection termination to all
    //  exclusive queues created by the connection, so that they can be
    //  destroyed even if there is no consumer on the queue.
    for (iterator = amq_queue_list_first (self->own_queue_list);
          iterator != NULL;
          iterator = amq_queue_list_next (&iterator))
        amq_queue_unbind_connection (iterator->item);

    amq_vhost_unlink       (&self->vhost);
    amq_queue_list_destroy (&self->own_queue_list);
</method>

<method name = "free">
    amq_consumer_table_destroy (&self->consumer_table);
</method>

<method name = "own queue" template = "function">
    <argument name = "queue" type = "amq_queue_t *">Queue reference</argument>
    <inherit name = "wrlock" />
    assert (queue->exclusive);
    amq_queue_list_queue (self->own_queue_list, queue);
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Unbind a queue from the connection.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">The queue to unbind</argument>
    <inherit name = "wrlock" />
    <local>
    amq_queue_list_iter_t *
        iterator;
    </local>
    //
    //  Remove the queue from the list of exclusive connections
    iterator = amq_queue_list_find (self->own_queue_list, queue);
    if (iterator)
        amq_queue_list_iter_destroy (&iterator);
</method>

<method name = "ready" template = "function">
    rc = TRUE;
</method>

<method name = "start ok" template = "function">
    //
    //  Server only supports plain authentication for now
    //
    smt_log_print (amq_broker->daily_log,
        "I: start login from=%s product=%s version=%s",
        self->client_address, self->client_product, self->client_version);

    if (s_auth_plain (self, method))
        self->authorised = TRUE;
    else
        self_raise_exception (self, ASL_ACCESS_REFUSED, 
            "Invalid authentication data",
            AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_START_OK);
</method>

<method name = "open">
    //  For now, link to single global vhost object
    self->vhost = amq_vhost_link (amq_broker->vhost);

    if (!self->vhost)
        self_raise_exception (self, ASL_ACCESS_REFUSED, 
            "Server is not ready",
            AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_OPEN);
    else
    //  If locked, allow only super user access
    if (amq_broker->locked && self->group == AMQ_CONNECTION_GROUP_NORMAL)
        self_raise_exception (self, ASL_ACCESS_REFUSED, 
            "Connections not allowed at present",
            AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_OPEN);
    else
    if (strneq (method->virtual_host, amq_server_config_vhost (amq_server_config))) {
        smt_log_print (amq_broker->alert_log,
            "E: client at %s tried to connect to invalid vhost '%s'",
            self->client_address, method->virtual_host);
        self_raise_exception (self, ASL_INVALID_PATH, "vhost is incorrect",
            AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_OPEN);
    }
    else
    if (amq_broker->failover->enabled) {
        //  Application connections send an event to the failover FSM.
        //  Failover state machine determines whether connection should 
        //  be accepted or rejected
        if (self->group == AMQ_CONNECTION_GROUP_NORMAL) {
            if (!amq_failover_execute (amq_broker->failover, amq_ha_event_new_connection))
                self_raise_exception (self, ASL_ACCESS_REFUSED,
                    "Connections not allowed at present",
                    AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_OPEN);
        }
    }
    else
        rc = TRUE;                      //  Proceed
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
        self_raise_exception (self, ASL_SYNTAX_ERROR, 
            "Missing authentication data",
            AMQ_SERVER_CONNECTION, AMQ_SERVER_CONNECTION_START_OK);
        return (0);
    }
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (!config->located) {
        smt_log_print (amq_broker->alert_log,
            "E: no 'plain' security defined in server config");
        self_raise_exception (self, ASL_INTERNAL_ERROR, 
            "Bad server configuration", 0, 0);
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
            else {
                smt_log_print (amq_broker->alert_log,
                    "E: invalid user group '%s' in config", group);
                self_raise_exception (self, ASL_INTERNAL_ERROR, 
                    "Bad server configuration", 0, 0);
            }
            smt_log_print (amq_broker->daily_log,
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
        smt_log_print (amq_broker->alert_log,
            "W: client used over-long authentication value - rejected");
    else {
        memcpy (token, data + token_null + 1, token_size);
        token [token_size] = 0;
    }
    return (token_null);
}
</private>

</class>
