<?xml?>
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

//  HA peer states
#define AMQ_HAC_STATE_UNKNOWN  0
#define AMQ_HAC_STATE_PENDING  1
#define AMQ_HAC_STATE_ACTIVE   2
#define AMQ_HAC_STATE_PASSIVE  3
#define AMQ_HAC_STATE_DEAD     4
</public>

<context>
    amq_broker_t
       *broker;                         //  Parent broker
    amq_peering_t
        *peering;                       //  The peering to the other HA peer
    Bool
        enabled,                        //  If FALSE, broker is run in standalone mode
        primary;                        //  TRUE = primary, FALSE = backup
    int
        state;                          //  See AMQ_HAC_STATE constants
    apr_time_t
        last_peer_time;                 //  When peer state arrived last time
    int
        peer_state;                     //  See AMQ_HAC_STATE constants
    amq_exchange_t
        *state_exchange;                //  amq.state exchange
    Bool
        waiting_for_connection;         //  If true, backup realised that primary
                                        //  failed and is waiting for first
                                        //  connection to become active.
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
            smt_log_print (amq_broker->alert_log, "I: server is HA primary, peering with %s", ha_peer);
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
            smt_log_print (amq_broker->alert_log, "I: server is HA backup, peering with %s", ha_peer);
        }
        else
            icl_console_print ("E: please specify --primary_peer setting");
    }
    if (self->enabled) {
        //  Start peering
        self->peering = amq_peering_new (
            ha_peer,
            amq_server_config_cluster_vhost (amq_server_config),
            amq_server_config_trace (amq_server_config), 0);
        amq_peering_set_login (self->peering, "peering");
        amq_peering_set_content_handler (self->peering, s_content_handler, self);
        amq_peering_start (self->peering);

        //  Set own and peer state machines
        self->state = AMQ_HAC_STATE_PENDING;
        self->peer_state = AMQ_HAC_STATE_UNKNOWN;
        self->last_peer_time = apr_time_now ();

        //  Subscribe for HA peer's state notifications
        amq_peering_bind (self->peering, "amq.status", "direct", TRUE, FALSE,
            self->primary? "b": "p", NULL);
        amq_cluster_hac_start_monitoring (self);

        self->state_exchange = amq_exchange_table_search (broker->vhost->exchange_table, "amq.status");
        assert (self->state_exchange);

        self->broker->clustered = TRUE;
    }
    else
        smt_log_print (amq_broker->alert_log, "I: server starting in stand-alone mode");
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

<method name = "send_state" template = "function">
    <local>
    icl_shortstr_t
        status_data;
    amq_client_method_t
        *client_method;
    </local>
    //
    icl_shortstr_fmt (status_data, "%d|%d|%ld", self->state, self->primary, amq_server_connection_count ());
    client_method = amq_client_method_new_basic_publish (
        0, "amq.status", self->primary ? "p" : "b", FALSE, FALSE);
    client_method->content = amq_content_basic_new ();
    amq_content_basic_set_body (client_method->content, status_data, strlen (status_data), NULL);
    amq_exchange_publish (self->state_exchange, NULL, (amq_server_method_t *) client_method);
    amq_client_method_unlink (&client_method);
</method>

<method name = "peer_state_changed" template = "function">
    <argument name = "old_state" type = "int" />
    <argument name = "new_state" type = "int" />
    <local>
    int
        old_self_state = self->state;
    </local>

    self->waiting_for_connection = FALSE;

    assert (self->state != AMQ_HAC_STATE_UNKNOWN);
    assert (self->state != AMQ_HAC_STATE_DEAD);

    if (self->primary) {
        //  Primary server's state machine
        if (self->state == AMQ_HAC_STATE_PENDING) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                self->state = AMQ_HAC_STATE_ACTIVE;
            else
            if (new_state == AMQ_HAC_STATE_ACTIVE)
                self->state = AMQ_HAC_STATE_PASSIVE;
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE)
                self->state = AMQ_HAC_STATE_ACTIVE;
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                ;   //  nothing
        }
        else
        if (self->state == AMQ_HAC_STATE_ACTIVE) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_ACTIVE) {
                icl_console_print ("E: HA backup became active unexpectedly - aborting");
                assert (0);
            }
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                ;   //  nothing
        }
        else
        if (self->state == AMQ_HAC_STATE_PASSIVE) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                self->state = AMQ_HAC_STATE_ACTIVE;
            else
            if (new_state == AMQ_HAC_STATE_ACTIVE)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE) {
                icl_console_print ("E: HA backup went passive unexpectedly - aborting");
                assert (0);
            }
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                ;   //  nothing
        }
    }
    else {
        //  Backup server's state machine
        if (self->state == AMQ_HAC_STATE_PENDING) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                ;   //  nothing
            if (new_state == AMQ_HAC_STATE_ACTIVE)
                self->state = AMQ_HAC_STATE_PASSIVE;
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                ;   //  nothing
        }
        else
        if (self->state == AMQ_HAC_STATE_ACTIVE) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_ACTIVE) {
                icl_console_print ("E: HA primary became active unexpectedly - aborting");
                assert (0);
            }
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                ;   //  nothing
        }
        else
        if (self->state == AMQ_HAC_STATE_PASSIVE) {
            if (new_state == AMQ_HAC_STATE_PENDING)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_ACTIVE)
                ;   //  nothing
            else
            if (new_state == AMQ_HAC_STATE_PASSIVE) {
                icl_console_print ("E: HA backup became passive unexpectedly - aborting");
                assert (0);
            }
            else
            if (new_state == AMQ_HAC_STATE_DEAD)
                self->waiting_for_connection = TRUE;
        }
    }
    if (self->state != old_self_state) {
        if (self->state == AMQ_HAC_STATE_ACTIVE)
            smt_log_print (amq_broker->alert_log, "I: *** server is now high-availability master ***");
        else
        if (self->state == AMQ_HAC_STATE_PASSIVE)
            smt_log_print (amq_broker->alert_log, "I: --- server is now high-availability slave ---");
    }
    self->peer_state = new_state;
</method>

<method name = "new_connection" template = "async function" async = "1">
    <action>
    if (self->waiting_for_connection) {
        self->state = AMQ_HAC_STATE_ACTIVE;
        self->waiting_for_connection = FALSE;
        smt_log_print (amq_broker->alert_log, "I: server is now high-availability master");
    }
    </action>
</method>

<event name = "monitor">
    <action>
    //  Test whether peer died
    //  If it did, stop monitoring
    if (self->peer_state != AMQ_HAC_STATE_DEAD
    &&  self->peer_state != AMQ_HAC_STATE_UNKNOWN
    &&  apr_time_now () - self->last_peer_time > AMQ_HAC_FAILOVER_TIME * 1000) {
        smt_log_print (amq_broker->alert_log,
            "W: high-availability peer disconnected unexpectedly");
        amq_cluster_hac_peer_state_changed (self, self->peer_state, AMQ_HAC_STATE_DEAD);
    }
    //  Send state notification to HA peer
    if (self->peer_state != AMQ_HAC_STATE_DEAD)
        amq_cluster_hac_send_state (self);

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
    int
        state;

    assert (peer_method->class_id == AMQ_PEER_BASIC);
    assert (peer_method->method_id == AMQ_PEER_BASIC_DELIVER);

    //  Status from other HAC party received
    self->last_peer_time = apr_time_now ();

    //  Parse content
    amq_content_basic_set_reader (peer_method->content, &reader, 4096);
    body = amq_content_basic_replay_body (peer_method->content, &reader);
    assert (body);
    state = atoi ((char*) body->data);
    ipr_bucket_destroy (&body);

    if (self->peer_state != state)
        amq_cluster_hac_peer_state_changed (self, self->peer_state, state);

    return (0);
}
</private>

<method name = "selftest" />

</class>
