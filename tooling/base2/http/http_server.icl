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
    name      = "http_server"
    comment   = "HTTP server command class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    This implements the HTTP server object, which is the top-level object
    holding server-wide values and owning server-wide structures.
</doc>

<inherit class = "http_driver_module_front" />
<inherit class = "http_access_module_front" />

<public>
extern http_server_t
    *http_server;                       //  Single server, self
</public>

<private>
http_server_t
    *http_server = NULL;
</private>

<!-- Import these, get them into http.h, see project.pdl -->
<import class = "icl" />
<import class = "ipr" />
<import class = "smt" />
<import class = "http" />

<context>
    apr_time_t
        started;                        //  Time started
    ipr_meter_t
        *imeter,                        //  Incoming messages meter
        *ometer;                        //  Outgoing messages meter
    int
        monitor_timer,                  //  Monitor timer
        security_timer,                 //  Refresh security data
        password_guard,                 //  Save password data
        auto_crash_timer,               //  Automatic failure
        auto_block_timer;               //  Automatic blockage
    FILE
        *stats;                         //  Statistics output
    smt_log_t
        *access_log,                    //  Access log file
        *alert_log,                     //  Alert log file
        *debug_log;                     //  Debug log file
    Bool
        cycled;                         //  Log files just cycled?
    ipr_dict_table_t
        *content_types;                 //  Configured MIME types
    ipr_dict_table_t
        *blacklist;                     //  Banned IP addresses
    ipr_password_table_t
        *basic,                         //  Basic authentication data
        *digest;                        //  Digest authentication data

    //  Statistics
    qbyte
        requests,                       //  Total requests
        responses,                      //  Total responses
        clients;                        //  Open connections
</context>

<method name = "new">
    //
    self->monitor_timer  = http_config_monitor (http_config);
    self->security_timer = http_config_nervosity (http_config);
    self->password_guard = http_config_password_guard (http_config);
    self->imeter         = ipr_meter_new ();
    self->ometer         = ipr_meter_new ();
    self->content_types  = ipr_dict_table_new ();
    self->access_log     = smt_log_new ();
    self->debug_log      = smt_log_new ();
    self->alert_log      = smt_log_new ();

    s_setup_log_files (self);
    s_load_content_types (self);
    http_server_sync_hashes (self);

    //  Start protocol server agent
    http_agent_init (self);
    self_start (self);
    self->started = apr_time_now ();

    smt_coremark_calculate (self->alert_log, http_config_coremark_cycles (http_config));
</method>

<method name = "destroy">
    <action>
    //  Stop child objects
    ipr_meter_destroy (&self->imeter);
    ipr_meter_destroy (&self->ometer);
    smt_log_destroy (&self->access_log);
    smt_log_destroy (&self->alert_log);
    smt_log_destroy (&self->debug_log);
    ipr_dict_table_destroy (&self->content_types);
    ipr_dict_table_destroy (&self->blacklist);
    ipr_password_table_destroy (&self->basic);
    ipr_password_table_destroy (&self->digest);
    if (self->stats)
        fclose (self->stats);
    http_agent_term ();
    </action>
</method>

<method name = "start" template = "async function">
    <action>
    //  We will run the broker monitor once per second, unconditionally
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</method>

<method name = "ban address" template = "async function">
    <doc>
    Adds one IP address to the blacklist.  We do the work here to avoid more
    than one thread trying to write to the blacklist at once.  This assumes
    we only have one http_server instance, which is the current design.
    </doc>
    <argument name = "address" type = "char *" />
    <possess>
    address = icl_mem_strdup (address);
    </possess>
    <release>
    icl_mem_free (address);
    </release>
    <action>
    //  Insert into loaded blacklist table, append to file if new address
    if (ipr_dict_assume (self->blacklist, address, NULL)) {
        //  Append to blacklist file
        FILE
            *file_stream = fopen (http_config_blacklist (http_config), "a");
        if (file_stream) {
            fprintf (file_stream, "%s\\n", address);
            fclose (file_stream);
        }
    }
    </action>
</method>

<method name = "sync hashes" template = "async function">
    <doc>
    Loads, or reloads the server hashes.  These are the blacklist, the basic
    password, and the digest password file.
    </doc>
    <action>
    icl_shortstr_t
        time_string;                    //  Modification time of file, for display
    char
        *filename;

    //  Load/reload blacklist data
    filename = http_config_blacklist (http_config);
    if (ipr_dict_table_file_sync (&self->blacklist, filename, "(.+)(.*)") > 0)
        smt_log_print (self->alert_log,
            "I: blacklist file '%s' loaded (%s, %ld entries)", filename,
            ipr_time_mime (self->blacklist->file_time, time_string), self->blacklist->nbr_items);

    //  Load/reload Basic authentication data and set policies if necessary
    if (self->basic == NULL) {
        self->basic = ipr_password_table_new (http_config_basic_auth (http_config));
        ipr_password_table_guard_set (self->basic,
            http_config_hit_quota    (http_config),
            http_config_miss_quota   (http_config),
            http_config_password_ttl (http_config));
        if (self->basic->file_time)
            smt_log_print (self->alert_log,
                "I: basic auth file '%s' loaded (%s, %ld entries)", self->basic->file_name,
                ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);
        else
            smt_log_print (self->alert_log,
                "I: basic auth file '%s' not found", self->basic->file_name);
    }
    else
    if (ipr_password_table_sync (self->basic))
        smt_log_print (self->alert_log,
            "I: basic auth file '%s' reloaded (%s, %ld entries)", self->basic->file_name,
            ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);

    if (self->digest == NULL) {
        self->digest = ipr_password_table_new (http_config_digest_auth (http_config));
        ipr_password_table_guard_set (self->digest,
            http_config_hit_quota    (http_config),
            http_config_miss_quota   (http_config),
            http_config_password_ttl (http_config));
        if (self->digest->file_time)
            smt_log_print (self->alert_log,
                "I: digest auth file '%s' loaded (%s, %ld entries)", self->digest->file_name,
                ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
        else
            smt_log_print (self->alert_log,
                "I: digest auth file '%s' not found", self->digest->file_name);

        ipr_password_table_set_nonce_ttl   (self->digest, http_config_nonce_ttl (http_config));
        ipr_password_table_set_nonce_quota (self->digest, http_config_nonce_quota (http_config));
    }
    else
    if (ipr_password_table_sync (self->digest))
        smt_log_print (self->alert_log,
            "I: digest auth file '%s' reloaded (%s, %ld entries)", self->digest->file_name,
            ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
   </action>
</method>

<!-- Reponse methods from access modules, see http_access_module.icl -->

<method name = "new password">
    <action>
    ipr_password_table_t
        *table;

    table = digest? self->digest: self->basic;
    ipr_password_assume (table, key, hash);
    ipr_password_table_save (table);
    smt_log_print (self->alert_log,
        "I: %s auth file '%s' saved (%ld entries)",
        digest? "digest": "basic", table->file_name, table->nbr_items);
    </action>
</method>

<!--  Monitor event is unleashed once per second -->

<event name = "monitor">
    <action>
    //  Security timer minimum is 1 so we're always GT 0 here
    self->security_timer--;
    if (self->security_timer == 0) {
        self->security_timer = http_config_nervosity (http_config);
        http_server_sync_hashes (self);
        if (self->password_guard) {
            self->password_guard -= http_config_nervosity (http_config);
            if (self->password_guard &lt;= 0) {
                self->password_guard = http_config_password_guard (http_config);
                ipr_password_table_guard_sync (self->basic);
                ipr_password_table_guard_sync (self->digest);
            }
        }
    }
    //  Cycle log files at midnight
    if (ipr_time_now () > 235900) {
        if (!self->cycled) {
            self->cycled = TRUE;        //  Don't cycle again today
            smt_log_cycle (self->access_log);
            smt_log_cycle (self->debug_log);
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

            self->monitor_timer = http_config_monitor (http_config);
            if (ipr_meter_mark (self->imeter, http_config_monitor (http_config))) {
                smt_log_print (self->debug_log,
                    "I: request  rate=%d mean=%d peak=%d",
                    self->imeter->current,
                    self->imeter->average,
                    self->imeter->maximum);
                new_stats = TRUE;
            }
            if (ipr_meter_mark (self->ometer, http_config_monitor (http_config))) {
                smt_log_print (self->debug_log,
                    "I: response rate=%d mean=%d peak=%d iomean=%d",
                    self->ometer->current,
                    self->ometer->average,
                    self->ometer->maximum,
                    self->ometer->average + self->imeter->average);
                new_stats = TRUE;
            }
            smt_log_print (self->debug_log, "I: clients=%d requests=%d responses=%d",
                    self->clients,
                    self->requests,
                    self->responses);

            //  Record statistics if necessary
            if (self->stats && new_stats) {
                fprintf (self->stats, "%zu\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\t%d\\n",
                    ipr_bucket_used () / 1024,
                    self->imeter->current, self->ometer->current,
                    self->imeter->average, self->ometer->average,
                    self->clients, self->requests, self->responses);
            }
        }
    }
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);

    if (self->auto_crash_timer) {
        if (--self->auto_crash_timer == 0) {
            smt_log_print (self->alert_log,
                "W: ************************  AUTO-CRASH  ************************");
            smt_log_print (self->alert_log,
                "W: server is now emulating a system crash, and will exit brutally.");
            exit (0);
        }
    }
    if (self->auto_block_timer) {
        if (--self->auto_block_timer == 0) {
            smt_log_print (self->alert_log,
                "W: ************************  AUTO-BLOCK  ************************");
            smt_log_print (self->alert_log,
                "W: server is now emulating a blockage, and will freeze for 5 minutes.");
            sleep (300);
        }
    }
    </action>
</event>

<private name = "header">
static void
    s_setup_log_files ($(selftype) *self);
static void
    s_load_content_types ($(selftype) *self);
static void
    s_format_logfile (char *logfile, char *configured);
</private>

<private name = "footer">
static void
s_setup_log_files ($(selftype) *self)
{
    char
        *log_path,                      //  Log directory
        *archive_cmd;                   //  Archive command
    icl_shortstr_t
        log_name,                       //  Full logfile name
        stats_file;                     //  Statistics file name

    log_path    = http_config_log_path    (http_config);
    archive_cmd = http_config_archive_cmd (http_config);

    if (http_config_syslog (http_config)) {
        if (ipr_log_sysopen ("http"))
            icl_console_print (
                "W: syslog not available, using direct-to-file logging");
        smt_log_set_syslog (self->access_log, IPR_LOG_SYSINFO);
        smt_log_set_syslog (self->debug_log,  IPR_LOG_SYSDEBUG);
        smt_log_set_syslog (self->alert_log,  IPR_LOG_SYSWARNING);
    }
    else
    if (http_config_keep_logs (http_config)) {
        s_format_logfile (log_name, http_config_access_log (http_config));
        smt_log_open (self->access_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, http_config_debug_log (http_config));
        smt_log_open (self->debug_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, http_config_alert_log (http_config));
        smt_log_open (self->alert_log, log_name, log_path, archive_cmd);
    }
    //  Dump environment and configuration to debug log, silently
    smt_log_dump_env (self->debug_log);
    http_config_dump_log (http_config, self->debug_log);

    //  Set up log file associations and configuration
    smt_log_set_raw    (self->access_log, TRUE);
    smt_log_set_stderr (self->debug_log, TRUE);
    smt_log_set_stderr (self->alert_log, TRUE);

    if (http_config_record_stats (http_config)) {
        icl_shortstr_fmt (stats_file, "http_stats_%s.del", http_config_port (http_config));
        self->stats = fopen (stats_file, "w");
        //  We use a tab-delimited form that pastes easily into spreadsheets
        fprintf (self->stats, "BuffersKB\\tCurIn\\tCurOut\\tAvgIn\\tAvgOut\\tTotReq\\tTotResp\\tClients\\n");
    }
}

//  Format a configured value into a log name including the port
//
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
        icl_shortstr_cat  (log_name, http_config_port (http_config));
        icl_shortstr_cat  (log_name, point);
    }
    else {
        //  Add port number at end
        icl_shortstr_cpy  (log_name, configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, http_config_port (http_config));
        icl_shortstr_cat  (log_name, ".log");
    }
}

//  Loads content types from configuration file
static void
s_load_content_types ($(selftype) *self)
{
    ipr_config_t
        *config;                        //  Current server config file
    char
        *name,                          //  File extension
        *value;                         //  Content type

    config = ipr_config_dup (http_config_table (http_config));
    ipr_config_locate (config, "/config/content/type", NULL);
    while (config->located) {
        name  = ipr_config_get (config, "name", NULL);
        value = ipr_config_get (config, "value", NULL);
        if (name && value)
            ipr_dict_assume (self->content_types, name, value);
        else
            smt_log_print (self->alert_log, "W: config file has bad content-type definition");

        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
}
</private>

<method name = "selftest" />

</class>
