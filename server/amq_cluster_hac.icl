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
    name      = "amq_cluster_hac"
    comment   = "High availability agent"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
</doc>

<option name = "links" value = "1"/>

<inherit class = "smt_object" />

<import class = "amq_server_classes" />

<public>
//  Monitoring frequency (in milliseconds)
#define AMQ_HAC_MONITOR_FREQUENCY 1000
//  Time required to consider other HA peer dead (in milliseconds)
#define AMQ_HAC_FAILOVER_TIME 10000

typedef enum
{
    amq_hac_state_pending = 1,  //  Waiting for peer to connect
    amq_hac_state_active  = 2,  //  Active - accepting connections
    amq_hac_state_passive = 3   //  Passive - not accepting connections
} amq_hac_state;

typedef enum
{
    amq_hac_event_peer_pending   = 1,  //  HAC peer became pending
    amq_hac_event_peer_active    = 2,  //  HAC peer became active
    amq_hac_event_peer_passive   = 3,  //  HAC peer became passive
    amq_hac_event_new_connection = 4   //  Attempt to create a new connection
}  amq_hac_event;

</public>

<context>
    amq_broker_t
       *broker;                 //  Parent broker
    amq_peering_t
        *peering;               //  The peering to the other HA peer
    Bool
        enabled,                //  If FALSE, broker is run in standalone mode
        primary;                //  TRUE = primary, FALSE = backup
                                //  NB that this flag is relevant only when the
                                //  whole cluster is being started
    amq_hac_state
        state;                  //  State of HAC FSM
    apr_time_t
        last_peer_time;         //  Time when peer state arrived lately
                                //  If this time is older than
                                //  AMQ_HAC_FAILOVER_TIME peer's considered dead
    amq_exchange_t
        *state_exchange;        //  amq.state exchange
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <local>
    char
        *ha_peer = NULL;
    </local>
    //
    self->broker = amq_broker_link (broker);
    if (amq_server_config_is_primary (amq_server_config)) {
        ha_peer = amq_server_config_backup_peer (amq_server_config);
        if (*ha_peer) {
            self->enabled = TRUE;
            self->primary = TRUE;
        }
        else
            icl_console_print ("E: please specify --backup_peer setting");
    }
    else
    if (amq_server_config_is_backup (amq_server_config)) {
        ha_peer = amq_server_config_primary_peer (amq_server_config);
        if (*ha_peer) {
            self->enabled = TRUE;
            self->primary = FALSE;
        }
        else
            icl_console_print ("E: please specify --primary_peer setting");
    }
    if (self->enabled) {
        smt_log_print (amq_broker->alert_log, "I: server starting in clustered mode");

        self->state = amq_hac_state_pending;
        self->last_peer_time = 0;

        //  Start peering
        self->peering = amq_peering_new (
            ha_peer,
            amq_server_config_cluster_vhost (amq_server_config),
            amq_server_config_trace (amq_server_config),
            "amq.status", "direct", TRUE, FALSE);
        amq_peering_set_login (self->peering, "peering");
        amq_peering_set_content_handler (self->peering, s_content_handler, self);
        amq_peering_start (self->peering);

        //  Subscribe for HA peer's state notifications
        amq_peering_bind (self->peering, self->primary? "b": "p", NULL);
        amq_cluster_hac_start_monitoring (self);

        self->state_exchange = amq_exchange_table_search (broker->vhost->exchange_table, "amq.status");
        assert (self->state_exchange);

        self->broker->clustered = TRUE;
    }
    else
        smt_log_print (amq_broker->alert_log, "I: server starting in non-clustered mode");
</method>

<method name = "start_monitoring" template = "async function" async = "1">
    <action>
    smt_timer_request_delay (self->thread, AMQ_HAC_MONITOR_FREQUENCY * 1000, monitor_event);
    </action>
</method>

<method name = "destroy">
    if (self->peering) {
        amq_peering_stop (self->peering);
        amq_peering_unlink (&self->peering);
    }
    amq_exchange_unlink (&self->state_exchange);
    amq_broker_unlink (&self->broker);
</method>

<method name = "send state" template = "function">
    <local>
    icl_shortstr_t
        state;
    amq_client_method_t
        *client_method;
    </local>
    //
    icl_shortstr_fmt (state, "%d", self->state);
    client_method = amq_client_method_new_basic_publish (
        0, "amq.status", self->primary ? "p" : "b", FALSE, FALSE);
    client_method->content = amq_content_basic_new ();
    
    amq_content_basic_set_body (client_method->content, icl_mem_strdup (state),
        strlen (state) + 1, icl_mem_free);
    amq_exchange_publish (self->state_exchange, NULL,
        (amq_server_method_t *) client_method);

    amq_client_method_unlink (&client_method);
</method>

<method name = "execute" return = "rc" template = "function">
    <argument name = "event" type = "int" />
    <declare name = "rc" type = "int" default = "0" />
    switch (self->state) {
    case amq_hac_state_pending:
        switch (event) {
        case amq_hac_event_peer_pending:
            if (self->primary) {
                self->state = amq_hac_state_active;
                smt_log_print (amq_broker->alert_log,
                    "I: HAC peer accessible; becoming master");
            }
            break;
        case amq_hac_event_peer_active:
            self->state = amq_hac_state_passive;
            smt_log_print (amq_broker->alert_log,
                "I: HAC peer accessible; becoming slave");
            break;
        case amq_hac_event_peer_passive:
            //  Do nothing; wait while peer switches to active
            break;
        case amq_hac_event_new_connection:
            //  Connections are refused in pending state
            rc = 0;
            break;
        default:
            assert (0);
        }
        break;
    case amq_hac_state_active:
        switch (event) {
        case amq_hac_event_peer_pending:
            //  Do nothing; slave is starting
            break;
        case amq_hac_event_peer_active:
            //  No way to have two masters - that would mean split-brain
            assert (0);
            break;
        case amq_hac_event_peer_passive:
            //  Do nothing; everything is OK
            break;
        case amq_hac_event_new_connection:
            //  Active state, we do accept new connections
            rc = 1;
            break;
        default:
            assert (0);
        }
        break;
    case amq_hac_state_passive:
        switch (event) {
        case amq_hac_event_peer_pending:
            //  The peer is restarting; become active (peer will become passive)
            self->state = amq_hac_state_active;
            smt_log_print (amq_broker->alert_log,
                "I: HAC peer restarting; becoming master");
            break;
        case amq_hac_event_peer_active:
            //  Do nothing; everything is OK
            break;
        case amq_hac_event_peer_passive:
            //  No way to have two passives - cluster would be non-responsive
            assert (0);
            break;
        case amq_hac_event_new_connection:
            if (apr_time_now () - self->last_peer_time >
                  AMQ_HAC_FAILOVER_TIME * 1000) {
                //  If peer is dead, switch to the active state
                self->state = amq_hac_state_active;
                smt_log_print (amq_broker->alert_log,
                    "I: HAC peer dead; becoming master");
                rc = 1;
            }
            else
                //  If peer is alive, reject connections
                rc = 0;
            break;
        default:
            assert (0);
        }
        break;
    default:
        assert (0);
    }
</method>

<event name = "monitor">
    <action>
    //  Send state notification to HA peer (if peer is not dead)
    if (self->last_peer_time == 0 || apr_time_now () -
          self->last_peer_time <= AMQ_HAC_FAILOVER_TIME * 1000)
        amq_cluster_hac_send_state (self);

    //  Schedule new monitoring event
    smt_timer_request_delay (
        self->thread, AMQ_HAC_MONITOR_FREQUENCY * 1000, monitor_event);
    </action>
</event>

<private name = "header">
static int
    s_content_handler (void *vself, amq_peering_t *peering, amq_peer_method_t *peer_method);
</private>

<private name = "footer">
static int
s_content_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_cluster_hac_t
        *self = (amq_cluster_hac_t*) vself;
    asl_reader_t
        reader;
    ipr_bucket_t
        *body;
    amq_hac_state
        state;
    amq_hac_event
        event;

    assert (peer_method->class_id == AMQ_PEER_BASIC);
    assert (peer_method->method_id == AMQ_PEER_BASIC_DELIVER);

    //  Status from other HAC party received
    self->last_peer_time = apr_time_now ();

    //  Parse content
    amq_content_basic_set_reader (peer_method->content, &reader, 4096);
    body = amq_content_basic_replay_body (peer_method->content, &reader);
    assert (body);
    state = atoi ((char *) body->data);
    assert (state != 0);
    ipr_bucket_destroy (&body);

    //  Convert peer's state to FSM event
    switch (state) {
    case amq_hac_state_pending:
        event = amq_hac_event_peer_pending;
        break;
    case amq_hac_state_active:
        event = amq_hac_event_peer_active;
        break;
    case amq_hac_state_passive:
        event = amq_hac_event_peer_passive;
        break;
    default:
        assert (0);
    }

    //  Run the FSM
    self_execute (self, event);

    return 0;
}
</private>

<method name = "selftest" />

</class>
