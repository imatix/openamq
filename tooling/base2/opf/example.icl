<?xml?>
<!--
    *** GENERATED FROM example.opf BY opf_gen USING GSL/4 ***

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
    name      = "example"
    comment   = "Example properties"
    script    = "icl_gen"
    license   = "gpl"
    opaque    = "1"
    >

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>

<import class = "smt_log" />

<public name = "include">
#include "ipr.h"
</public>

<public>
extern $(selftype)
    *example_config;                    //  Global properties object
</public>

<private>
$(selftype)
    *example_config = NULL;             //  Global properties
</private>

<context>
    char *
        port;                           //  Server port for client connections
    int
        monitor;                        //  Monitor interval, seconds
    size_t
        max_memory_mb;                  //  Maximum allowed memory, MBytes
    Bool
        record_stats;                   //  Record performance statistics
    int
        packio;                         //  Pack network data before writing
    int
        heartbeat;                      //  Heartbeat from client to server, seconds
    int
        read_timeout;                   //  Timeout on network reads, seconds
    int
        write_timeout;                  //  Timeout on network writes, seconds
    Bool
        tcp_nodelay;                    //  TCP/IP NDELAY option
    int
        tcp_rcvbuf;                     //  TCP/IP receive buffer, in bytes
    int
        tcp_sndbuf;                     //  TCP/IP send buffer, in bytes
    int
        frame_max;                      //  Maximum size of AMQP content frames
    char *
        log_path;                       //  Active log file directory
    Bool
        keep_logs;                      //  Keep log files
    char *
        archive_path;                   //  Archive log file directory
    char *
        archive_cmd;                    //  Archive command
    char *
        alert_log;                      //  Alert log file name
    char *
        daily_log;                      //  Daily log file name
    char *
        debug_log;                      //  Debug log file name
    int
        trace;                          //  Protocol trace level
    <property name = "filename" type = "char *" />
    <property name = "table" type = "ipr_config_t *" readonly = "1" />
</context>

<method name = "new">
    <doc>
    Creates an empty configuration set.  To fill this, use the load and
    load_bucket methods.
    </doc>
    //
    self->table = ipr_config_new (NULL, FALSE);
</method>

<method name = "destroy">
    ipr_config_destroy (&self->table);
    icl_mem_free (self->filename);
</method>

<method name = "load xmlfile" template = "function">
    <doc>
    Loads a config file into the property set, merging the new loaded data
    with any previous data, as defined in ipr_config.icl.
    </doc>
    <argument name = "filename" type = "char *">Config file to load</argument>
    <argument name = "required" type = "Bool">Config file is required?</argument>
    //
    assert (filename);
    if (!ipr_config_load (self->table, filename, required)) {
        self_set_filename (self, filename);
        s_validate_items (self);
        s_get_properties (self);
    }
</method>

<method name = "load bucket" template = "function">
    <doc>
    Loads a config resource (as a bucket) into the property set, merging the
    new loaded data with any previous data, as defined in ipr_config.icl.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Config data to load</argument>
    //
    assert (bucket);
    ipr_config_load_bucket (self->table, bucket);
    s_validate_items (self);
    s_get_properties (self);
</method>

<method name = "shadow" template = "function">
    <doc>
    Loads the data from a different property set, and updates all the properties.
    Use this when you want to provide your code with multiple property classes
    that actually contain an identical configuration set.  To use:
    my_config = my_config_new (NULL, NULL, FALSE);
    my_config_shadow (self, your_config->config);
    </doc>
    <argument name = "source" type = "ipr_config_t *">Config data to shadow</argument>
    //
    //  Get rid of our own config data
    ipr_config_destroy (&self->table);
    //  Grab a handle onto the source data
    self->table = ipr_config_dup (source);
    //  Update our properties, without validation
    s_get_properties (self);
</method>

<method name = "commit" template = "function">
    <doc>
    Saves the current configuration values to the specified file.  If the
    provided filename is null, uses the original config filename. If both
    filenames are null, does nothing.
    </doc>
    <argument name = "filename" type = "char *">Config file to save</argument>
    <inherit name = "wrlock" />
    //
    if (filename)
        ipr_config_save (self->table, filename);
    else
    if (strused (self->filename))
        ipr_config_save (self->table, self->filename);
</method>

<method name = "rollback" template = "function">
    <doc>
    Reloads the saved configuration from the saved file, if any.
    Any changes made to the current configuration are lost.
    </doc>
    <inherit name = "wrlock" />
    //
    ipr_config_destroy (&self->table);
    self->table = ipr_config_new (self->filename, TRUE);
    s_get_properties (self);
</method>

<method name = "cmdline help">
    <doc>
    Prints command-line help for setting options from the command
    line.
    </doc>
    printf ("Options that override config values; these take an argument:\\n");
    printf ("  --port           Server port for client connections\\n");
    printf ("  --monitor        Monitor interval, seconds\\n");
    printf ("  --max_memory_mb  Maximum allowed memory, MBytes\\n");
    printf ("  --record_stats   Record performance statistics\\n");
    printf ("  --packio         Pack network data before writing\\n");
    printf ("  --heartbeat      Heartbeat from client to server, seconds\\n");
    printf ("  --read_timeout   Timeout on network reads, seconds\\n");
    printf ("  --write_timeout  Timeout on network writes, seconds\\n");
    printf ("  --tcp_nodelay    TCP/IP NDELAY option\\n");
    printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\\n");
    printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\\n");
    printf ("  --frame_max      Maximum size of AMQP content frames\\n");
    printf ("  --log_path       Active log file directory\\n");
    printf ("  --keep_logs      Keep log files\\n");
    printf ("  --archive_path   Archive log file directory\\n");
    printf ("  --archive_cmd    Archive command\\n");
    printf ("  --alert_log      Alert log file name\\n");
    printf ("  --daily_log      Daily log file name\\n");
    printf ("  --debug_log      Debug log file name\\n");
    printf ("  --trace          Protocol trace level\\n");
</method>

<method name = "cmdline parse" template = "function">
    <doc>
    Parses recognized command-line options starting with --.  Prints an error
    message and exits if the syntax is invalid.
    </doc>
    <argument name = "name" type = "char *">Name of application</argument>
    <argument name = "argc" type = "int">Argument count</argument>
    <argument name = "argv" type = "char **">Argument list</argument>
    <local>
    int
        argn;                           //  Argument number
    char
        *arg_name,
        *arg_value;
    </local>
    //
    argn = 1;
    while (argn < argc) {
        arg_name = argv [argn];
        if (streq (arg_name, "--help")) {
            self_options_help (self, name);
            exit (EXIT_SUCCESS);
        }
        else
        if (ipr_str_prefixed (argv [argn], "--")) {
            argn++;
            if (argn < argc && *argv [argn] != '-') {
                arg_value = argv [argn];
                argn++;

                //  Set property
                if (streq (arg_name, "--port"))
                    self_set_port (self, arg_value);
                else
                if (streq (arg_name, "--monitor"))
                    self_set_monitor (self, atoi (arg_value));
                else
                if (streq (arg_name, "--max_memory_mb"))
                    self_set_max_memory_mb (self, atoi (arg_value));
                else
                if (streq (arg_name, "--record_stats"))
                    self_set_record_stats (self, atoi (arg_value));
                else
                if (streq (arg_name, "--packio"))
                    self_set_packio (self, atoi (arg_value));
                else
                if (streq (arg_name, "--heartbeat"))
                    self_set_heartbeat (self, atoi (arg_value));
                else
                if (streq (arg_name, "--read_timeout"))
                    self_set_read_timeout (self, atoi (arg_value));
                else
                if (streq (arg_name, "--write_timeout"))
                    self_set_write_timeout (self, atoi (arg_value));
                else
                if (streq (arg_name, "--tcp_nodelay"))
                    self_set_tcp_nodelay (self, atoi (arg_value));
                else
                if (streq (arg_name, "--tcp_rcvbuf"))
                    self_set_tcp_rcvbuf (self, atoi (arg_value));
                else
                if (streq (arg_name, "--tcp_sndbuf"))
                    self_set_tcp_sndbuf (self, atoi (arg_value));
                else
                if (streq (arg_name, "--frame_max"))
                    self_set_frame_max (self, atoi (arg_value));
                else
                if (streq (arg_name, "--log_path"))
                    self_set_log_path (self, arg_value);
                else
                if (streq (arg_name, "--keep_logs"))
                    self_set_keep_logs (self, atoi (arg_value));
                else
                if (streq (arg_name, "--archive_path"))
                    self_set_archive_path (self, arg_value);
                else
                if (streq (arg_name, "--archive_cmd"))
                    self_set_archive_cmd (self, arg_value);
                else
                if (streq (arg_name, "--alert_log"))
                    self_set_alert_log (self, arg_value);
                else
                if (streq (arg_name, "--daily_log"))
                    self_set_daily_log (self, arg_value);
                else
                if (streq (arg_name, "--debug_log"))
                    self_set_debug_log (self, arg_value);
                else
                if (streq (arg_name, "--trace"))
                    self_set_trace (self, atoi (arg_value));
                else {
                    icl_console_print ("E: unrecognised option '%s'", arg_name);
                    exit (EXIT_FAILURE);
                }
            }
            else {
                icl_console_print ("E: %s option needs a parameter", arg_name);
                exit (EXIT_FAILURE);
            }
        }
        else
            argn++;
    }
    rc = argc;                          //  Return final arg count
</method>

<method name = "options help" template = "function">
    <doc>
    Prints full help for all configuration options.
    </doc>
    <argument name = "name" type = "char *">Name of application</argument>
    printf ("These are the configured options for %s\\n\\n", name);

    printf ("/config/server/port - Server port for client connections\\n");
    printf ("    From command-line: --port newvalue\\n");
    printf ("    Specifies the port on which the server should open its connections.\\n");
    printf ("    Current value is '%s'. Default value is '7654'\\n", self->port);
    printf ("\\n");

    printf ("/config/resources/monitor - Monitor interval, seconds\\n");
    printf ("    From command-line: --monitor newvalue\\n");
    printf ("    Specifies the interval in seconds at which the server will report its   \\n");
    printf ("    message rates. If zero, no monitoring is shown. The recommended value is\\n");
    printf ("    1, if monitoring is desired.                                            \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->monitor);
    printf ("\\n");

    printf ("/config/resources/max_memory_mb - Maximum allowed memory, MBytes\\n");
    printf ("    From command-line: --max_memory_mb newvalue\\n");
    printf ("    Specifies the maximum memory allowed for the server, in MB. When the   \\n");
    printf ("    server reaches this limit, it will attempt to free resources and refuse\\n");
    printf ("    new connections until memory has been made available. If zero, no limit\\n");
    printf ("    is used.                                                               \\n");
    printf ("    Current value is %zu.\\n", self->max_memory_mb);
    printf ("\\n");

    printf ("/config/resources/record_stats - Record performance statistics\\n");
    printf ("    From command-line: --record_stats newvalue\\n");
    printf ("    Specifies whether or not the server records performance statistics in a \\n");
    printf ("    format suitable for importing into a spreadsheet. If set, the server    \\n");
    printf ("    creates a file called 'amq_server_stats_(portnumber).del' which contains\\n");
    printf ("    performance statistics in a comma-delimited format. For this setting to \\n");
    printf ("    take effect, the monitor option must also be set.                       \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->record_stats);
    printf ("\\n");

    printf ("/config/tuning/packio - Pack network data before writing\\n");
    printf ("    From command-line: --packio newvalue\\n");
    printf ("    If set, data is packed into larger buffers before writing. This can     \\n");
    printf ("    improve performance significantly on some systems, especially Windows   \\n");
    printf ("    and Linux with 2.4 kernels. If you disable this setting, you should test\\n");
    printf ("    the performance using a bulk message transfer test.                     \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->packio);
    printf ("\\n");

    printf ("/config/tuning/heartbeat - Heartbeat from client to server, seconds\\n");
    printf ("    From command-line: --heartbeat newvalue\\n");
    printf ("    Defines the timeout for connection heartbeating.\\n");
    printf ("    Current value is %i. Default value is 2.\\n", self->heartbeat);
    printf ("\\n");

    printf ("/config/tuning/read_timeout - Timeout on network reads, seconds\\n");
    printf ("    From command-line: --read_timeout newvalue\\n");
    printf ("    Defines the timeout for network reads. If a peer does not respond within\\n");
    printf ("    this time, it is considered dead. If you get unexpected timeout errors  \\n");
    printf ("    in the logs, you may want to increase this value.                       \\n");
    printf ("    Current value is %i. Default value is 30.\\n", self->read_timeout);
    printf ("\\n");

    printf ("/config/tuning/write_timeout - Timeout on network writes, seconds\\n");
    printf ("    From command-line: --write_timeout newvalue\\n");
    printf ("    Defines the timeout for network writes. If a peer does not accept data\\n");
    printf ("    within this time, it is considered dead. If you get unexpected timeout\\n");
    printf ("    errors in the logs, you may want to increase this value.              \\n");
    printf ("    Current value is %i. Default value is 30.\\n", self->write_timeout);
    printf ("\\n");

    printf ("/config/tuning/tcp_nodelay - TCP/IP NDELAY option\\n");
    printf ("    From command-line: --tcp_nodelay newvalue\\n");
    printf ("    If this value is 1, socket data is written immediately, which is usually\\n");
    printf ("    good for latency. If this value is 0, data is buffered until there is a \\n");
    printf ("    full packet, which is usually good for throughput.                      \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->tcp_nodelay);
    printf ("\\n");

    printf ("/config/tuning/tcp_rcvbuf - TCP/IP receive buffer, in bytes\\n");
    printf ("    From command-line: --tcp_rcvbuf newvalue\\n");
    printf ("    If this value is greater than zero, all client connections will use the\\n");
    printf ("    specified value. Note: setting this value is delicate, do not use this \\n");
    printf ("    option unless you know what you are doing.                             \\n");
    printf ("    Current value is %i.\\n", self->tcp_rcvbuf);
    printf ("\\n");

    printf ("/config/tuning/tcp_sndbuf - TCP/IP send buffer, in bytes\\n");
    printf ("    From command-line: --tcp_sndbuf newvalue\\n");
    printf ("    If this value is greater than zero, all client connections will use the\\n");
    printf ("    specified value. Note: setting this value is delicate, do not use this \\n");
    printf ("    option unless you know what you are doing.                             \\n");
    printf ("    Current value is %i.\\n", self->tcp_sndbuf);
    printf ("\\n");

    printf ("/config/tuning/frame_max - Maximum size of AMQP content frames\\n");
    printf ("    From command-line: --frame_max newvalue\\n");
    printf ("    Defines the maximum frame size the server will accept. Larger frame     \\n");
    printf ("    sizes are useful for applications that create very large message headers\\n");
    printf ("    but will cause congestion on multi-channel connections.                 \\n");
    printf ("    Current value is %i. Default value is 32768.\\n", self->frame_max);
    printf ("\\n");

    printf ("/config/logging/log_path - Active log file directory\\n");
    printf ("    From command-line: --log_path newvalue\\n");
    printf ("    Specifies the directory path into which active log files are placed.\\n");
    printf ("    Current value is '%s'. Default value is 'logs'\\n", self->log_path);
    printf ("\\n");

    printf ("/config/logging/keep_logs - Keep log files\\n");
    printf ("    From command-line: --keep_logs newvalue\\n");
    printf ("    Specifies whether or not the server should keep logs. If zero, the      \\n");
    printf ("    server will not keep logs. If 1, it will keep an alert log, a daily log,\\n");
    printf ("    and a debug log as specified.                                           \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->keep_logs);
    printf ("\\n");

    printf ("/config/logging/archive_path - Archive log file directory\\n");
    printf ("    From command-line: --archive_path newvalue\\n");
    printf ("    Specifies the directory path into which archived log files are placed. \\n");
    printf ("    This can be the same as the active path; archived log files are renamed\\n");
    printf ("    to avoid conflict with active log files.                               \\n");
    printf ("    Current value is '%s'. Default value is 'archive'\\n", self->archive_path);
    printf ("\\n");

    printf ("/config/logging/archive_cmd - Archive command\\n");
    printf ("    From command-line: --archive_cmd newvalue\\n");
    printf ("    If specified, this command is run on all archived log files. You can use\\n");
    printf ("    this option to compress archived log files, or clean up old and unwanted\\n");
    printf ("    files. When the command is run, the name of the archived log file is    \\n");
    printf ("    provided as an argument to it.                                          \\n");
    printf ("    Current value is '%s'.\\n", self->archive_cmd);
    printf ("\\n");

    printf ("/config/logging/alert_log - Alert log file name\\n");
    printf ("    From command-line: --alert_log newvalue\\n");
    printf ("    The name of the alert log file, which records all alerts issued by the \\n");
    printf ("    server. Note that the server port number is always added to the name of\\n");
    printf ("    the log file.                                                          \\n");
    printf ("    Current value is '%s'. Default value is 'alert.log'\\n", self->alert_log);
    printf ("\\n");

    printf ("/config/logging/daily_log - Daily log file name\\n");
    printf ("    From command-line: --daily_log newvalue\\n");
    printf ("    The name of the daily log file, which records all normal activity as   \\n");
    printf ("    well as errors. Note that the server port number is always added to the\\n");
    printf ("    name of the log file.                                                  \\n");
    printf ("    Current value is '%s'. Default value is 'daily.log'\\n", self->daily_log);
    printf ("\\n");

    printf ("/config/logging/debug_log - Debug log file name\\n");
    printf ("    From command-line: --debug_log newvalue\\n");
    printf ("    The name of the debug log file, which records all normal activity,      \\n");
    printf ("    errors, and debug trace information. Note that the server port number is\\n");
    printf ("    always added to the name of the log file.                               \\n");
    printf ("    Current value is '%s'. Default value is 'debug.log'\\n", self->debug_log);
    printf ("\\n");

    printf ("/config/logging/trace - Protocol trace level\\n");
    printf ("    From command-line: --trace newvalue\\n");
    printf ("    Specifies the protocol debug level, which can be 0 (no tracing), 1 (show\\n");
    printf ("    information and protocol methods), 2 (show protocol animation), 3 (show \\n");
    printf ("    wire-level data). You should not use any tracing on production servers  \\n");
    printf ("    except in low-volume scenarios.                                         \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->trace);
    printf ("\\n");
</method>

<method name = "set port" template = "function">
    <argument name = "port" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/server", "port", port);
    port = ipr_config_getp (self->table, "/config/server", "port", NULL);
    assert (port);
    self->port = port;
</method>

<method name = "port" return = "port">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "port" type = "char *" />
    //
    if (self)
        port = self->port;
    else
        port = "7654";
</method>

<method name = "set monitor" template = "function">
    <argument name = "monitor" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", monitor);
    ipr_config_putp (self->table, "/config/resources", "monitor", strvalue);
    self->monitor = monitor;
</method>

<method name = "monitor" return = "monitor">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "monitor" type = "int" />
    //
    if (self)
        monitor = self->monitor;
    else
        monitor = 1;
</method>

<method name = "set max_memory_mb" template = "function">
    <argument name = "max_memory_mb" type = "size_t" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%zu", max_memory_mb);
    ipr_config_putp (self->table, "/config/resources", "max_memory_mb", strvalue);
    self->max_memory_mb = max_memory_mb;
</method>

<method name = "max_memory_mb" return = "max_memory_mb">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "max_memory_mb" type = "size_t" />
    //
    if (self)
        max_memory_mb = self->max_memory_mb;
    else
        max_memory_mb = 0;
</method>

<method name = "set record_stats" template = "function">
    <argument name = "record_stats" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", record_stats);
    ipr_config_putp (self->table, "/config/resources", "record_stats", strvalue);
    self->record_stats = record_stats;
</method>

<method name = "record_stats" return = "record_stats">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "record_stats" type = "Bool" />
    //
    if (self)
        record_stats = self->record_stats;
    else
        record_stats = 0;
</method>

<method name = "set packio" template = "function">
    <argument name = "packio" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", packio);
    ipr_config_putp (self->table, "/config/tuning", "packio", strvalue);
    self->packio = packio;
</method>

<method name = "packio" return = "packio">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "packio" type = "int" />
    //
    if (self)
        packio = self->packio;
    else
        packio = 1;
</method>

<method name = "set heartbeat" template = "function">
    <argument name = "heartbeat" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", heartbeat);
    ipr_config_putp (self->table, "/config/tuning", "heartbeat", strvalue);
    self->heartbeat = heartbeat;
</method>

<method name = "heartbeat" return = "heartbeat">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "heartbeat" type = "int" />
    //
    if (self)
        heartbeat = self->heartbeat;
    else
        heartbeat = 2;
</method>

<method name = "set read_timeout" template = "function">
    <argument name = "read_timeout" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", read_timeout);
    ipr_config_putp (self->table, "/config/tuning", "read_timeout", strvalue);
    self->read_timeout = read_timeout;
</method>

<method name = "read_timeout" return = "read_timeout">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "read_timeout" type = "int" />
    //
    if (self)
        read_timeout = self->read_timeout;
    else
        read_timeout = 30;
</method>

<method name = "set write_timeout" template = "function">
    <argument name = "write_timeout" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", write_timeout);
    ipr_config_putp (self->table, "/config/tuning", "write_timeout", strvalue);
    self->write_timeout = write_timeout;
</method>

<method name = "write_timeout" return = "write_timeout">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "write_timeout" type = "int" />
    //
    if (self)
        write_timeout = self->write_timeout;
    else
        write_timeout = 30;
</method>

<method name = "set tcp_nodelay" template = "function">
    <argument name = "tcp_nodelay" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", tcp_nodelay);
    ipr_config_putp (self->table, "/config/tuning", "tcp_nodelay", strvalue);
    self->tcp_nodelay = tcp_nodelay;
</method>

<method name = "tcp_nodelay" return = "tcp_nodelay">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "tcp_nodelay" type = "Bool" />
    //
    if (self)
        tcp_nodelay = self->tcp_nodelay;
    else
        tcp_nodelay = 1;
</method>

<method name = "set tcp_rcvbuf" template = "function">
    <argument name = "tcp_rcvbuf" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", tcp_rcvbuf);
    ipr_config_putp (self->table, "/config/tuning", "tcp_rcvbuf", strvalue);
    self->tcp_rcvbuf = tcp_rcvbuf;
</method>

<method name = "tcp_rcvbuf" return = "tcp_rcvbuf">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "tcp_rcvbuf" type = "int" />
    //
    if (self)
        tcp_rcvbuf = self->tcp_rcvbuf;
    else
        tcp_rcvbuf = 0;
</method>

<method name = "set tcp_sndbuf" template = "function">
    <argument name = "tcp_sndbuf" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", tcp_sndbuf);
    ipr_config_putp (self->table, "/config/tuning", "tcp_sndbuf", strvalue);
    self->tcp_sndbuf = tcp_sndbuf;
</method>

<method name = "tcp_sndbuf" return = "tcp_sndbuf">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "tcp_sndbuf" type = "int" />
    //
    if (self)
        tcp_sndbuf = self->tcp_sndbuf;
    else
        tcp_sndbuf = 0;
</method>

<method name = "set frame_max" template = "function">
    <argument name = "frame_max" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (frame_max < 2048) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for frame_max, using %d",
            frame_max, 2048);
        frame_max = 2048;
    }
    if (frame_max > 4194304) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for frame_max, using %d",
            frame_max, 4194304);
        frame_max = 4194304;
    }
    icl_shortstr_fmt (strvalue, "%i", frame_max);
    ipr_config_putp (self->table, "/config/tuning", "frame_max", strvalue);
    self->frame_max = frame_max;
</method>

<method name = "frame_max" return = "frame_max">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "frame_max" type = "int" />
    //
    if (self)
        frame_max = self->frame_max;
    else
        frame_max = 32768;
</method>

<method name = "set log_path" template = "function">
    <argument name = "log_path" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "log_path", log_path);
    log_path = ipr_config_getp (self->table, "/config/logging", "log_path", NULL);
    assert (log_path);
    self->log_path = log_path;
</method>

<method name = "log_path" return = "log_path">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "log_path" type = "char *" />
    //
    if (self)
        log_path = self->log_path;
    else
        log_path = "logs";
</method>

<method name = "set keep_logs" template = "function">
    <argument name = "keep_logs" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", keep_logs);
    ipr_config_putp (self->table, "/config/logging", "keep_logs", strvalue);
    self->keep_logs = keep_logs;
</method>

<method name = "keep_logs" return = "keep_logs">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "keep_logs" type = "Bool" />
    //
    if (self)
        keep_logs = self->keep_logs;
    else
        keep_logs = 1;
</method>

<method name = "set archive_path" template = "function">
    <argument name = "archive_path" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "archive_path", archive_path);
    archive_path = ipr_config_getp (self->table, "/config/logging", "archive_path", NULL);
    assert (archive_path);
    self->archive_path = archive_path;
</method>

<method name = "archive_path" return = "archive_path">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "archive_path" type = "char *" />
    //
    if (self)
        archive_path = self->archive_path;
    else
        archive_path = "archive";
</method>

<method name = "set archive_cmd" template = "function">
    <argument name = "archive_cmd" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "archive_cmd", archive_cmd);
    archive_cmd = ipr_config_getp (self->table, "/config/logging", "archive_cmd", NULL);
    assert (archive_cmd);
    self->archive_cmd = archive_cmd;
</method>

<method name = "archive_cmd" return = "archive_cmd">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "archive_cmd" type = "char *" />
    //
    if (self)
        archive_cmd = self->archive_cmd;
    else
        archive_cmd = "";
</method>

<method name = "set alert_log" template = "function">
    <argument name = "alert_log" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "alert_log", alert_log);
    alert_log = ipr_config_getp (self->table, "/config/logging", "alert_log", NULL);
    assert (alert_log);
    self->alert_log = alert_log;
</method>

<method name = "alert_log" return = "alert_log">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "alert_log" type = "char *" />
    //
    if (self)
        alert_log = self->alert_log;
    else
        alert_log = "alert.log";
</method>

<method name = "set daily_log" template = "function">
    <argument name = "daily_log" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "daily_log", daily_log);
    daily_log = ipr_config_getp (self->table, "/config/logging", "daily_log", NULL);
    assert (daily_log);
    self->daily_log = daily_log;
</method>

<method name = "daily_log" return = "daily_log">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "daily_log" type = "char *" />
    //
    if (self)
        daily_log = self->daily_log;
    else
        daily_log = "daily.log";
</method>

<method name = "set debug_log" template = "function">
    <argument name = "debug_log" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "debug_log", debug_log);
    debug_log = ipr_config_getp (self->table, "/config/logging", "debug_log", NULL);
    assert (debug_log);
    self->debug_log = debug_log;
</method>

<method name = "debug_log" return = "debug_log">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "debug_log" type = "char *" />
    //
    if (self)
        debug_log = self->debug_log;
    else
        debug_log = "debug.log";
</method>

<method name = "set trace" template = "function">
    <argument name = "trace" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", trace);
    ipr_config_putp (self->table, "/config/logging", "trace", strvalue);
    self->trace = trace;
</method>

<method name = "trace" return = "trace">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "trace" type = "int" />
    //
    if (self)
        trace = self->trace;
    else
        trace = 0;
</method>

<method name = "dump log" template = "function">
    <doc>
    Dump current option values to an smt_log file.
    </doc>
    <argument name = "logfile" type = "smt_log_t *">Log file to dump to</argument>
    smt_log_print (logfile, "*******************  Configuration Settings  ********************");
    smt_log_print (logfile, "port=%s", self->port);
    smt_log_print (logfile, "monitor=%i", self->monitor);
    smt_log_print (logfile, "max_memory_mb=%zu", self->max_memory_mb);
    smt_log_print (logfile, "record_stats=%i", self->record_stats);
    smt_log_print (logfile, "packio=%i", self->packio);
    smt_log_print (logfile, "heartbeat=%i", self->heartbeat);
    smt_log_print (logfile, "read_timeout=%i", self->read_timeout);
    smt_log_print (logfile, "write_timeout=%i", self->write_timeout);
    smt_log_print (logfile, "tcp_nodelay=%i", self->tcp_nodelay);
    smt_log_print (logfile, "tcp_rcvbuf=%i", self->tcp_rcvbuf);
    smt_log_print (logfile, "tcp_sndbuf=%i", self->tcp_sndbuf);
    smt_log_print (logfile, "frame_max=%i", self->frame_max);
    smt_log_print (logfile, "log_path=%s", self->log_path);
    smt_log_print (logfile, "keep_logs=%i", self->keep_logs);
    smt_log_print (logfile, "archive_path=%s", self->archive_path);
    smt_log_print (logfile, "archive_cmd=%s", self->archive_cmd);
    smt_log_print (logfile, "alert_log=%s", self->alert_log);
    smt_log_print (logfile, "daily_log=%s", self->daily_log);
    smt_log_print (logfile, "debug_log=%s", self->debug_log);
    smt_log_print (logfile, "trace=%i", self->trace);
    smt_log_print (logfile, "");
</method>

<private name = "header">
static void s_validate_items ($(selftype) *self);
static void s_get_properties ($(selftype) *self);
</private>

<private name = "body">
static void s_validate_items ($(selftype) *self)
{
    char
        *item_name;

    //  Locate first child and check all items for validity
    ipr_config_locate (self->table, "/config/+", "");
    while (self->table->located) {
        item_name = ipr_config_item (self->table);
        if (streq (item_name, "echo"))
            icl_console_print ("I: %s: %s", self->filename, ipr_config_value (self->table));
        else
        if (strneq (item_name, "server")
        && strneq (item_name, "resources")
        && strneq (item_name, "tuning")
        && strneq (item_name, "logging"))
            icl_console_print ("W: %s: unknown item '%s', ignored",
                self->filename, item_name);
        ipr_config_next (self->table);
    }
}

static void s_get_properties ($(selftype) *self)
{
    char
        *value;                         //  Configured value

    //  All properties on server path
    ipr_config_locate (self->table, "/config/server", NULL);

    //  Get port property from config file or built-in defaults
    value = ipr_config_get (self->table, "port", NULL);
    if (value)
        self->port = value;
    else
        self->port = "7654";

    //  All properties on resources path
    ipr_config_locate (self->table, "/config/resources", NULL);

    //  Get monitor property from config file or built-in defaults
    value = ipr_config_get (self->table, "monitor", NULL);
    if (value)
        self->monitor = (int) atol (value);
    else
        self->monitor = 1;

    //  Get max_memory_mb property from config file or built-in defaults
    value = ipr_config_get (self->table, "max_memory_mb", NULL);
    if (value)
        self->max_memory_mb = (size_t) atol (value);
    else
        self->max_memory_mb = 0;

    //  Get record_stats property from config file or built-in defaults
    value = ipr_config_get (self->table, "record_stats", NULL);
    if (value)
        self->record_stats = (Bool) atol (value);
    else
        self->record_stats = 0;

    //  All properties on tuning path
    ipr_config_locate (self->table, "/config/tuning", NULL);

    //  Get packio property from config file or built-in defaults
    value = ipr_config_get (self->table, "packio", NULL);
    if (value)
        self->packio = (int) atol (value);
    else
        self->packio = 1;

    //  Get heartbeat property from config file or built-in defaults
    value = ipr_config_get (self->table, "heartbeat", NULL);
    if (value)
        self->heartbeat = (int) atol (value);
    else
        self->heartbeat = 2;

    //  Get read_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "read_timeout", NULL);
    if (value)
        self->read_timeout = (int) atol (value);
    else
        self->read_timeout = 30;

    //  Get write_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "write_timeout", NULL);
    if (value)
        self->write_timeout = (int) atol (value);
    else
        self->write_timeout = 30;

    //  Get tcp_nodelay property from config file or built-in defaults
    value = ipr_config_get (self->table, "tcp_nodelay", NULL);
    if (value)
        self->tcp_nodelay = (Bool) atol (value);
    else
        self->tcp_nodelay = 1;

    //  Get tcp_rcvbuf property from config file or built-in defaults
    value = ipr_config_get (self->table, "tcp_rcvbuf", NULL);
    if (value)
        self->tcp_rcvbuf = (int) atol (value);
    else
        self->tcp_rcvbuf = 0;

    //  Get tcp_sndbuf property from config file or built-in defaults
    value = ipr_config_get (self->table, "tcp_sndbuf", NULL);
    if (value)
        self->tcp_sndbuf = (int) atol (value);
    else
        self->tcp_sndbuf = 0;

    //  Get frame_max property from config file or built-in defaults
    value = ipr_config_get (self->table, "frame_max", NULL);
    if (value)
        self->frame_max = (int) atol (value);
    else
        self->frame_max = 32768;
    if (self->frame_max < 2048) {
        icl_console_print ("W: %s: illegal value %d for frame_max, using %d",
            self->filename, self->frame_max, 2048);
        self->frame_max = 2048;
    }
    if (self->frame_max > 4194304) {
        icl_console_print ("W: %s: illegal value %d for frame_max, using %d",
            self->filename, self->frame_max, 4194304);
        self->frame_max = 4194304;
    }

    //  All properties on logging path
    ipr_config_locate (self->table, "/config/logging", NULL);

    //  Get log_path property from config file or built-in defaults
    value = ipr_config_get (self->table, "log_path", NULL);
    if (value)
        self->log_path = value;
    else
        self->log_path = "logs";

    //  Get keep_logs property from config file or built-in defaults
    value = ipr_config_get (self->table, "keep_logs", NULL);
    if (value)
        self->keep_logs = (Bool) atol (value);
    else
        self->keep_logs = 1;

    //  Get archive_path property from config file or built-in defaults
    value = ipr_config_get (self->table, "archive_path", NULL);
    if (value)
        self->archive_path = value;
    else
        self->archive_path = "archive";

    //  Get archive_cmd property from config file or built-in defaults
    value = ipr_config_get (self->table, "archive_cmd", NULL);
    if (value)
        self->archive_cmd = value;
    else
        self->archive_cmd = "";

    //  Get alert_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "alert_log", NULL);
    if (value)
        self->alert_log = value;
    else
        self->alert_log = "alert.log";

    //  Get daily_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "daily_log", NULL);
    if (value)
        self->daily_log = value;
    else
        self->daily_log = "daily.log";

    //  Get debug_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "debug_log", NULL);
    if (value)
        self->debug_log = value;
    else
        self->debug_log = "debug.log";

    //  Get trace property from config file or built-in defaults
    value = ipr_config_get (self->table, "trace", NULL);
    if (value)
        self->trace = (int) atol (value);
    else
        self->trace = 0;
}
</private>

<method name = "selftest">
</method>

</class>

