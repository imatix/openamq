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
//  Time required to consider other HAC peer dead (in milliseconds)
#define AMQ_HAC_FAILOVER_TIME 10000

//  HAC peer states
#define AMQ_HAC_STATE_UNKNOWN  0
#define AMQ_HAC_STATE_PENDING  1
#define AMQ_HAC_STATE_ACTIVE   2
#define AMQ_HAC_STATE_PASSIVE  3
#define AMQ_HAC_STATE_DEAD     4
</public>

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
    state = atoi (body->data);
    ipr_bucket_destroy (&body);

    if (self->peer_state != state) {
        amq_cluster_hac_peer_state_changed (self, self->peer_state, state);
        self->peer_state = state;
    }

    //  If notification about peer being in pending state is received
    //  start the monitor
    if (state == AMQ_HAC_STATE_PENDING)
        amq_cluster_hac_start_monitoring (self);
    
    return 0;
}
</private>

<context>
    char
       *name;
    amq_broker_t
       *broker;                         //  Reference to broker
    amq_peering_t
        *peering;                       //  The peering to the other broker
                                        //  in HAC pair
    Bool
        hac_on;                         //  If FALSE, broker is run in standalone mode
    Bool
        primary;                        //  TRUE = primary, FALSE = backup
    int
        state;                          //  See AMQ_HAC_STATE constants
    apr_time_t
        last_peer_time;                 //  Time at which last state message
                                        //  from HAC peer have arrived
    int
        peer_state;                     //  See AMQ_HAC_STATE constants
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t*" />
    <argument name = "name" type = "char*" />
    <local>
    ipr_config_t
        *config;
    char
        *enabled;
    char
        *other_peer;
    char
        *virtual_host;
    </local>

    self->broker = amq_broker_link (broker);

    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/cluster-hac", NULL);
    if (config->located) {

        enabled  = ipr_config_get (config, "enabled", "1");
        if (strcmp (enabled, "0") == 0)
            icl_console_print ("Server being run in non-HAC mode.");
        else {
            //  If server is run in HAC mode, it has to have name specified
            assert (name);

            self->name  = ipr_config_get (config, "primary-name", NULL);
            if (strcmp (self->name, name) == 0) {
                icl_console_print ("Server %s being run in 'primary' HAC mode.",
                    self->name);
                self->hac_on = TRUE;
                self->primary = TRUE;
                other_peer = ipr_config_get (config, "backup-host", NULL);
            }
            else {
                self->name  = ipr_config_get (config, "backup-name", NULL);
                if (strcmp (self->name, name) == 0) {
                    icl_console_print ("Server %s being run in 'backup' HAC mode.",
                        self->name);
                    self->hac_on = TRUE;
                    self->primary = FALSE;
                    other_peer = ipr_config_get (config, "primary-host", NULL);
                }
                else
                    icl_console_print ("Server being run in non-HAC mode.");
            }
        }
    }
    else
        icl_console_print ("Server being run in non-HAC mode.");

    self->state = AMQ_HAC_STATE_UNKNOWN;
    self->peer_state = AMQ_HAC_STATE_UNKNOWN;
    self->last_peer_time = apr_time_now ();

    if (self->hac_on) {

        //  Start peering
        assert (other_peer);
        virtual_host = ipr_config_get (config, "primary-host", NULL);
        self->peering = amq_peering_new (other_peer, virtual_host,
            amq_server_config_trace (amq_server_config));
        amq_peering_set_login (self->peering, "peering");
        amq_peering_set_content_handler (self->peering, s_content_handler, self);
        amq_peering_start (self->peering);

        //  Switch to pending state
        self->state = AMQ_HAC_STATE_PENDING;

        //  Subscribe for HAC peer's state notifications
        amq_peering_bind (self->peering, "amq.status",
            self->primary ? "p" : "b", NULL);

        //  Send state to the HAC peer
        amq_cluster_hac_send_state (self);
    }

    ipr_config_destroy (&config);
</method>

<method name = "start_monitoring" template = "async function" async = "1">
    <action>
    smt_timer_request_delay (self->thread, AMQ_HAC_MONITOR_FREQUENCY * 1000,
        monitor_event);
    </action>
</method>

<method name = "destroy">
    if (self->peering) {
        amq_peering_stop (self->peering);
        amq_peering_unlink (&self->peering);
    }
    amq_broker_unlink (&self->broker);
</method>

<method name = "send_state" template = "function">
    <local>
    char
        status_data [1024];
    amq_content_basic_t
        *content;
    </local> 

    sprintf (status_data, "%ld|%s|%ld",
        (long) self->state,
        self->name,
        (long) amq_server_connection_count ());
    content = amq_content_basic_new ();
    assert (content);
    amq_content_basic_set_body (content,
        status_data, strlen (status_data), NULL);
    amq_peering_forward (self->peering, "amq.status", self->primary ? "p" : "b",
        content, FALSE, FALSE);
    amq_content_basic_unlink (&content);
</method>

<method name = "peer_state_changed" template = "function">
    <argument name = "old_state" type = "int" />
    <argument name = "new_state" type = "int" />
    icl_console_print ("I: %s's state changed from %d to %d",
        self->primary ? "Primary" : "Backup", old_state, new_state);
</method>

<event name = "monitor">
    <action>
    //  Send state notification to HAC peer
    amq_cluster_hac_send_state (self);

    //  Test whether peer died
    //  If it did, stop monitoring
    if (apr_time_now () - self->last_peer_time > AMQ_HAC_FAILOVER_TIME * 1000) {
        icl_console_print ("W: HAC peer died !");
        amq_cluster_hac_peer_state_changed (self, self->peer_state,
            AMQ_HAC_STATE_DEAD);
        self->peer_state = AMQ_HAC_STATE_DEAD;
    }
    else {
        smt_timer_request_delay (self->thread, AMQ_HAC_MONITOR_FREQUENCY * 1000,
            monitor_event);
    }
    </action>
</event>

<method name = "selftest" />
</class>

