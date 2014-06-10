<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name      = "asl_broker"
    comment   = "Abstract broker class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    This implements the broker object, which is the top-level object
    holding server-wide values.
</doc>

<inherit class = "smt_object" />
<import class = "$(basename)_agent" />

<public>
extern $(selftype)
    *$(selfname);                       //  Single broker, self
</public>

<private>
$(selftype)
    *$(selfname) = NULL;
</private>

<context>
    $(basename)_connection_table_t
        *connections;                   //  Connection table
    $(basename)_connection_list_t
        *connection_list;               //  Connection list
    smt_thread_t
        *agent_thread;                  //  $(basename)_server_agent thread
    apr_time_t
        started;                        //  Time started
    icl_shortstr_t
        host;                           //  Server network address
    icl_shortstr_t
        name,                           //  Server name
        identifier;                     //  Unique identifier
    ipr_meter_t
        *imeter,                        //  Incoming messages meter
        *ometer;                        //  Outgoing messages meter
    int
        monitor_timer;                  //  Monitor timer
    FILE
        *stats;                         //  Statistics output
    smt_log_t
        *alert_log,                     //  Alert log file
        *daily_log,                     //  Daily log file
        *debug_log;                     //  Debug log file
    Bool
        cycled;                         //  Log files just cycled?
    qbyte
        direct_in,                      //  Direct messages received
        direct_out;                     //  Direct messages sent out
</context>

<method name = "new">
    <local>
    char
        *log_path,                      //  Log directory
        *archive_cmd;                   //  Archive command
    icl_shortstr_t
        log_name,                       //  Full logfile name
        stats_file;                     //  Statistics file name
    smt_thread_t
        *thread_p;                      //  Thread from $(basename)_server_agent
    </local>
    //
    //  Set self so that we're immediately addressable in other threads
    $(selfname) = self;

    self->started         = smt_time_now ();
    self->connections     = $(basename)_connection_table_new ();
    self->connection_list = $(basename)_connection_list_new ();
    self->monitor_timer   = $(basename)_config_monitor ($(basename)_config);
    self->imeter          = ipr_meter_new ();
    self->ometer          = ipr_meter_new ();
    ipr_str_random (self->identifier, "AAAAAAAA-AAAAAAAA-AAAAAAAA");

    //  Set up log files and log forwarding:
    log_path     = $(basename)_config_log_path     ($(basename)_config);
    archive_cmd  = $(basename)_config_archive_cmd  ($(basename)_config);

    self->debug_log = smt_log_new ();   //  Debug log for low-level trace
    self->daily_log = smt_log_new ();   //  Daily log for normal activity
    self->alert_log = smt_log_new ();   //  Alert log for exceptions

    if ($(basename)_config_syslog ($(basename)_config)) {
        if (ipr_log_sysopen ("$(basename)"))
            icl_console_print (
                "W: syslog not available, using direct-to-file logging");
        smt_log_set_syslog (self->debug_log, IPR_LOG_SYSDEBUG);
        smt_log_set_syslog (self->daily_log, IPR_LOG_SYSINFO);
        smt_log_set_syslog (self->alert_log, IPR_LOG_SYSWARNING);
    }
    else
    if ($(basename)_config_keep_logs ($(basename)_config)) {
        s_format_logfile (log_name, $(basename)_config_debug_log ($(basename)_config));
        smt_log_open (self->debug_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, $(basename)_config_daily_log ($(basename)_config));
        smt_log_open (self->daily_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, $(basename)_config_alert_log ($(basename)_config));
        smt_log_open (self->alert_log, log_name, log_path, archive_cmd);
    }
    //  Dump environment and configuration to debug log, silently
    smt_log_dump_env (self->debug_log);
    $(basename)_config_dump_log ($(basename)_config, self->debug_log);

    //  Set up log file associations and configuration
    smt_log_set_stderr (self->debug_log, TRUE);
    smt_log_set_copyto (self->daily_log, self->debug_log);
    smt_log_set_stderr (self->alert_log, TRUE);
    smt_log_set_copyto (self->alert_log, self->daily_log);

    if ($(basename)_config_record_stats ($(basename)_config)) {
        icl_shortstr_fmt (stats_file, "$(basename)_stats_%s.del",
            $(basename)_config_port ($(basename)_config));
        self->stats = fopen (stats_file, "w");
        //  We use a tab-delimited form that pastes easily into spreadsheets
        fprintf (self->stats, "Clients\\tMsgMemK\\tCurIn\\tCurOut\\tAvgIn\\tAvgOut\\n");
    }
    //  Start server agent and own async thread
    if ($(basename)_agent_init (&thread_p) == SMT_OK) {
        self->agent_thread = smt_thread_link (thread_p);
        self_start (self);
    }
</method>

<method name = "destroy">
    <action>
    smt_signal_deregister (SIGINT);
    smt_signal_deregister (SIGTERM);
    ipr_meter_destroy (&self->imeter);
    ipr_meter_destroy (&self->ometer);
    $(basename)_connection_table_destroy (&self->connections);
    $(basename)_connection_list_destroy (&self->connection_list);
    smt_thread_unlink (&self->agent_thread);
    $(basename)_agent_term ();
    if (self->stats)
        fclose (self->stats);
    </action>
</method>

<method name = "free">
    smt_log_destroy (&self->alert_log);
    smt_log_destroy (&self->daily_log);
    smt_log_destroy (&self->debug_log);
</method>

<method name = "start" template = "async function">
    <action>
    //  We will run the broker monitor once per second, unconditionally
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    //  Register to receive interrupt event on SIGINT, SIGTERM
    smt_signal_register (SIGINT, thread, interrupt_event);
    smt_signal_register (SIGTERM, thread, interrupt_event);
    </action>
</method>

<event name = "monitor">
    <action>
    //  Cycle log files at midnight
    if (ipr_time_now () > 235900) {
        if (!self->cycled) {
            self->cycled = TRUE;        //  Don't cycle again today
            smt_log_cycle (self->debug_log);
            smt_log_cycle (self->daily_log);
            smt_log_cycle (self->alert_log);
        }
    }
    else
        self->cycled = FALSE;

    if (self->monitor_timer) {
        self->monitor_timer--;
        if (self->monitor_timer == 0) {
            Bool
                new_stats = FALSE;      //  Do we have new stats to print?

            self->monitor_timer = $(basename)_config_monitor ($(basename)_config);
            if (ipr_meter_mark (self->imeter, $(basename)_config_monitor ($(basename)_config))) {
                smt_log_print (self->debug_log,
                    "I: incoming rate=%d mean=%d peak=%d",
                    self->imeter->current,
                    self->imeter->average,
                    self->imeter->maximum);
                new_stats = TRUE;
            }
            if (ipr_meter_mark (self->ometer, $(basename)_config_monitor ($(basename)_config))) {
                smt_log_print (self->debug_log,
                    "I: outgoing rate=%d mean=%d peak=%d iomean=%d",
                    self->ometer->current,
                    self->ometer->average,
                    self->ometer->maximum,
                    self->ometer->average + self->imeter->average);
                new_stats = TRUE;
            }
            //  Record statistics if necessary
            if (self->stats && new_stats) {
                fprintf (self->stats, "%d\\t%zu\\t%d\\t%d\\t%d\\t%d\\n",
                    $(basename)_connection_count (),
                    ipr_bucket_used () / 1024,
                    self->imeter->current, self->ometer->current,
                    self->imeter->average, self->ometer->average);
            }
        }
    }
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<!--  Public method that other broker agents should call to request a broker
      shutdown.                                                             -->

<method name = "shutdown" template = "async function">
    <action>
        //  Tell $(basename)_agent to close master socket
        $(basename)_agent_shutdown (self->agent_thread);
        //  TODO: Shutdown any peerings or other objects that would malfunction
        //  Start graceful shutdown sequence
        smt_thread_set_next_event (thread, waiting_for_shutdown_event);
    </action>
</method>

<event name = "waiting for shutdown">
    <action name = "shutdown active connections">
        //  Spend a maximum of 30 seconds shutting down
        s_shutdown_time_spent++;
        if (s_shutdown_time_spent > 30)
            smt_thread_set_next_event (thread, finished_with_shutdown_event);
        //  For each active connection, tell it's $(basename)_agent instance
        //  to shut it down.
        s_active_connections = 0;
        $(basename)_connection_table_apply (
            self->connections, s_shutdown_connection, NULL);
        if (s_active_connections) {
            if (s_shutdown_time_spent == 1)
                //  Only print message once
                smt_log_print (self->alert_log,
                    "W: closing %d connection%s, please wait up to 30 seconds",
                    s_active_connections,
                    s_active_connections > 1? "s": "");
            //  Loop and try again in one second
            smt_timer_request_delay (thread, 1000000,
                waiting_for_shutdown_event);
        }
        else
            //  Finished, proceed with broker shutdown
            smt_thread_set_next_event (thread, finished_with_shutdown_event);
    </action>
</event>

<event name = "finished with shutdown">
    <action name = "shutdown broker">
        smt_shut_down ();
    </action>
</event>

<!--  Event triggered on receipt of SIGINT or SIGTERM.  On first receipt of a
      termination signal we attempt a graceful shutdown.  Any further receipt
      of a termination signal will trigger an immediate shutdown.           -->

<event name = "interrupt">
    <action name = "report server interrupted">
        s_interrupted++;
        if (s_interrupted == 1) {
            smt_log_print (self->alert_log,
                "W: caught signal, shutting down");
            //  Start graceful shutdown
            $(selfname)_shutdown (self);
        }
        else
        if (s_interrupted > 1) {
            smt_log_print (self->alert_log,
                "W: caught another signal, shutting down immediately");
            smt_shut_down ();
        }
    </action>
</event>

<private name = "header">
static void
    s_format_logfile (char *logfile, char *configured);
</private>

<private name = "async header">
static void
    s_shutdown_connection ($(basename)_connection_t *connection_p, void *argument);
static int
    s_interrupted = 0,                  //  Server interrupted?
    s_shutdown_time_spent = 0;          //  Time spent in graceful shutdown
static dbyte
    s_active_connections = 0;           //  Used during server shutdown
</private>

<private name = "footer">
//  Format a logfile name including the port.
static void
s_format_logfile (char *log_name, char *configured)
{
    char
        *point;

    point = strchr (configured, '.');
    if (point) {
        //  Insert port number before extension
        icl_shortstr_ncpy (log_name, configured, point - configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, $(basename)_config_port ($(basename)_config));
        icl_shortstr_cat  (log_name, point);
    }
    else {
        //  Add port number at end
        icl_shortstr_cpy  (log_name, configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, $(basename)_config_port ($(basename)_config));
        icl_shortstr_cat  (log_name, ".log");
    }
}
</private>

<private name = "async footer">
//  Shutdown a single connection
static void
s_shutdown_connection ($(basename)_connection_t *connection_p, void *argument)
{
    $(basename)_connection_t
        *connection;

    connection = $(basename)_connection_link (connection_p);
    if (connection) {
        if ($(basename)_agent_shutdown (connection->thread) == 0)
            s_active_connections++;
        $(basename)_connection_unlink (&connection);
    }
}
</private>

<method name = "selftest" />

</class>
