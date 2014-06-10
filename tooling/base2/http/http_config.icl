<?xml?>
<!--
    *** GENERATED FROM http_config.opf BY opf_gen USING GSL/4 ***

    
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
    name      = "http_config"
    comment   = "Server configuration properties"
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
    *http_config;                   //  Server properties object
</public>

<private>
$(selftype)
    *http_config = NULL;            //  Server properties
</private>

<context>
    char *
        server_name;                    //  Announced server name
    char *
        port;                           //  Server port for HTTP requests
    char *
        public_port;                    //  Public port for HTTP requests
    Bool
        trace;                          //  Trace network traffic?
    Bool
        animate;                        //  Animate HTTP agent?
    Bool
        verbose;                        //  Verbose output?
    char *
        listen;                         //  Address (local network interface) to listen on
    char *
        webroot;                        //  Path to web root directory
    char *
        defaults;                       //  Default file names
    char *
        default_types;                  //  Default document types
    int
        monitor;                        //  Monitor interval, seconds
    Bool
        record_stats;                   //  Record performance statistics
    char *
        log_path;                       //  Active log file directory
    Bool
        keep_logs;                      //  Keep log files
    char *
        archive_cmd;                    //  Archive log file command
    char *
        alert_log;                      //  Error log file name
    char *
        access_log;                     //  Access log file name
    char *
        access_log_format;              //  Access log file format
    char *
        debug_log;                      //  Debug log file name
    int
        syslog;                         //  Use syslog logging protocol
    char *
        user;                           //  User to run server as
    char *
        group;                          //  Group to run server as
    char *
        blacklist;                      //  Blacklist filename
    char *
        basic_auth;                     //  Basic authentication filename
    char *
        digest_auth;                    //  Digest authentication filename
    int
        password_ttl;                   //  Password time-to-live, in seconds
    int
        hit_quota;                      //  Password hit quota in pages
    int
        miss_quota;                     //  Password miss quota in requests
    int
        password_guard;                 //  Password guard timer, in seconds.
    Bool
        policy_trace;                   //  Debug policy execution?
    int
        nervosity;                      //  Security nervosity, seconds
    int
        nonce_ttl;                      //  Nonce time-to-live, seconds
    int
        nonce_quota;                    //  Nonce quota in requests
    char *
        refresh_by;                     //  Shell command for password refresh
    char *
        refresh_log;                    //  Stdout for password refresh
    int
        address_quota;                  //  Maximum open connections per address
    int
        abort_delay;                    //  Pause time for hostile connections
    int
        coremark_cycles;                //  Duration of CoreMark test
    int
        header_max;                     //  Max size of HTTP header
    int
        bucket_max;                     //  Max size of content bucket
    int
        read_timeout;                   //  Network read timeout, seconds
    int
        write_timeout;                  //  Network write timeout, seconds
    int
        accept_retry_timeout;           //  Timeout before retrying accept calls, seconds
    Bool
        tcp_nodelay;                    //  TCP/IP NODELAY option
    int
        tcp_rcvbuf;                     //  TCP/IP receive buffer, in bytes
    int
        tcp_sndbuf;                     //  TCP/IP send buffer, in bytes
    int
        polling_threads;                //  Number of polling OS threads
    int
        working_threads;                //  Number of working OS threads
    char *
        etag;                           //  ETag header for files
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
    printf ("  --server_name    Announced server name\\n");
    printf ("  --port           Server port for HTTP requests\\n");
    printf ("  --public_port    Public port for HTTP requests\\n");
    printf ("  --trace          Trace network traffic?\\n");
    printf ("  --animate        Animate HTTP agent?\\n");
    printf ("  --verbose        Verbose output?\\n");
    printf ("  --listen         Address (local network interface) to listen on\\n");
    printf ("  --webroot        Path to web root directory\\n");
    printf ("  --defaults       Default file names\\n");
    printf ("  --default_types  Default document types\\n");
    printf ("  --monitor        Monitor interval, seconds\\n");
    printf ("  --record_stats   Record performance statistics\\n");
    printf ("  --log_path       Active log file directory\\n");
    printf ("  --keep_logs      Keep log files\\n");
    printf ("  --archive_cmd    Archive log file command\\n");
    printf ("  --alert_log      Error log file name\\n");
    printf ("  --access_log     Access log file name\\n");
    printf ("  --access_log_format  Access log file format\\n");
    printf ("  --debug_log      Debug log file name\\n");
    printf ("  --syslog         Use syslog logging protocol\\n");
    printf ("  --user           User to run server as\\n");
    printf ("  --group          Group to run server as\\n");
    printf ("  --blacklist      Blacklist filename\\n");
    printf ("  --basic_auth     Basic authentication filename\\n");
    printf ("  --digest_auth    Digest authentication filename\\n");
    printf ("  --password_ttl   Password time-to-live, in seconds\\n");
    printf ("  --hit_quota      Password hit quota in pages\\n");
    printf ("  --miss_quota     Password miss quota in requests\\n");
    printf ("  --password_guard  Password guard timer, in seconds.\\n");
    printf ("  --policy_trace   Debug policy execution?\\n");
    printf ("  --nervosity      Security nervosity, seconds\\n");
    printf ("  --nonce_ttl      Nonce time-to-live, seconds\\n");
    printf ("  --nonce_quota    Nonce quota in requests\\n");
    printf ("  --refresh_by     Shell command for password refresh\\n");
    printf ("  --refresh_log    Stdout for password refresh\\n");
    printf ("  --address_quota  Maximum open connections per address\\n");
    printf ("  --abort_delay    Pause time for hostile connections\\n");
    printf ("  --coremark_cycles  Duration of CoreMark test\\n");
    printf ("  --header_max     Max size of HTTP header\\n");
    printf ("  --bucket_max     Max size of content bucket\\n");
    printf ("  --read_timeout   Network read timeout, seconds\\n");
    printf ("  --write_timeout  Network write timeout, seconds\\n");
    printf ("  --accept_retry_timeout  Timeout before retrying accept calls, seconds\\n");
    printf ("  --tcp_nodelay    TCP/IP NODELAY option\\n");
    printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\\n");
    printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\\n");
    printf ("  --polling_threads  Number of polling OS threads\\n");
    printf ("  --working_threads  Number of working OS threads\\n");
    printf ("  --etag           ETag header for files\\n");
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
                if (streq (arg_name, "--server_name"))
                    self_set_server_name (self, arg_value);
                else
                if (streq (arg_name, "--port"))
                    self_set_port (self, arg_value);
                else
                if (streq (arg_name, "--public_port"))
                    self_set_public_port (self, arg_value);
                else
                if (streq (arg_name, "--trace"))
                    self_set_trace (self, atoi (arg_value));
                else
                if (streq (arg_name, "--animate"))
                    self_set_animate (self, atoi (arg_value));
                else
                if (streq (arg_name, "--verbose"))
                    self_set_verbose (self, atoi (arg_value));
                else
                if (streq (arg_name, "--listen"))
                    self_set_listen (self, arg_value);
                else
                if (streq (arg_name, "--webroot"))
                    self_set_webroot (self, arg_value);
                else
                if (streq (arg_name, "--defaults"))
                    self_set_defaults (self, arg_value);
                else
                if (streq (arg_name, "--default_types"))
                    self_set_default_types (self, arg_value);
                else
                if (streq (arg_name, "--monitor"))
                    self_set_monitor (self, atoi (arg_value));
                else
                if (streq (arg_name, "--record_stats"))
                    self_set_record_stats (self, atoi (arg_value));
                else
                if (streq (arg_name, "--log_path"))
                    self_set_log_path (self, arg_value);
                else
                if (streq (arg_name, "--keep_logs"))
                    self_set_keep_logs (self, atoi (arg_value));
                else
                if (streq (arg_name, "--archive_cmd"))
                    self_set_archive_cmd (self, arg_value);
                else
                if (streq (arg_name, "--alert_log"))
                    self_set_alert_log (self, arg_value);
                else
                if (streq (arg_name, "--access_log"))
                    self_set_access_log (self, arg_value);
                else
                if (streq (arg_name, "--access_log_format"))
                    self_set_access_log_format (self, arg_value);
                else
                if (streq (arg_name, "--debug_log"))
                    self_set_debug_log (self, arg_value);
                else
                if (streq (arg_name, "--syslog"))
                    self_set_syslog (self, atoi (arg_value));
                else
                if (streq (arg_name, "--user"))
                    self_set_user (self, arg_value);
                else
                if (streq (arg_name, "--group"))
                    self_set_group (self, arg_value);
                else
                if (streq (arg_name, "--blacklist"))
                    self_set_blacklist (self, arg_value);
                else
                if (streq (arg_name, "--basic_auth"))
                    self_set_basic_auth (self, arg_value);
                else
                if (streq (arg_name, "--digest_auth"))
                    self_set_digest_auth (self, arg_value);
                else
                if (streq (arg_name, "--password_ttl"))
                    self_set_password_ttl (self, atoi (arg_value));
                else
                if (streq (arg_name, "--hit_quota"))
                    self_set_hit_quota (self, atoi (arg_value));
                else
                if (streq (arg_name, "--miss_quota"))
                    self_set_miss_quota (self, atoi (arg_value));
                else
                if (streq (arg_name, "--password_guard"))
                    self_set_password_guard (self, atoi (arg_value));
                else
                if (streq (arg_name, "--policy_trace"))
                    self_set_policy_trace (self, atoi (arg_value));
                else
                if (streq (arg_name, "--nervosity"))
                    self_set_nervosity (self, atoi (arg_value));
                else
                if (streq (arg_name, "--nonce_ttl"))
                    self_set_nonce_ttl (self, atoi (arg_value));
                else
                if (streq (arg_name, "--nonce_quota"))
                    self_set_nonce_quota (self, atoi (arg_value));
                else
                if (streq (arg_name, "--refresh_by"))
                    self_set_refresh_by (self, arg_value);
                else
                if (streq (arg_name, "--refresh_log"))
                    self_set_refresh_log (self, arg_value);
                else
                if (streq (arg_name, "--address_quota"))
                    self_set_address_quota (self, atoi (arg_value));
                else
                if (streq (arg_name, "--abort_delay"))
                    self_set_abort_delay (self, atoi (arg_value));
                else
                if (streq (arg_name, "--coremark_cycles"))
                    self_set_coremark_cycles (self, atoi (arg_value));
                else
                if (streq (arg_name, "--header_max"))
                    self_set_header_max (self, atoi (arg_value));
                else
                if (streq (arg_name, "--bucket_max"))
                    self_set_bucket_max (self, atoi (arg_value));
                else
                if (streq (arg_name, "--read_timeout"))
                    self_set_read_timeout (self, atoi (arg_value));
                else
                if (streq (arg_name, "--write_timeout"))
                    self_set_write_timeout (self, atoi (arg_value));
                else
                if (streq (arg_name, "--accept_retry_timeout"))
                    self_set_accept_retry_timeout (self, atoi (arg_value));
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
                if (streq (arg_name, "--polling_threads"))
                    self_set_polling_threads (self, atoi (arg_value));
                else
                if (streq (arg_name, "--working_threads"))
                    self_set_working_threads (self, atoi (arg_value));
                else
                if (streq (arg_name, "--etag"))
                    self_set_etag (self, arg_value);
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

    printf ("/config/server/name - Announced server name\\n");
    printf ("    From command-line: --server_name newvalue\\n");
    printf ("    Specifies the Server: header field name for HTTP responses.\\n");
    printf ("    Current value is '%s'. Default value is 'X5'\\n", self->server_name);
    printf ("\\n");

    printf ("/config/server/port - Server port for HTTP requests\\n");
    printf ("    From command-line: --port newvalue\\n");
    printf ("    Specifies the port that the server opens for HTTP requests. The standard\\n");
    printf ("    port for HTTP is 80. If you want to run the server in parallel with     \\n");
    printf ("    other HTTP servers on the same server, choose a server port of 4096 or  \\n");
    printf ("    higher. If you are serving Internet clients, your network may map a     \\n");
    printf ("    public port to the server port number. In that case, you may need to    \\n");
    printf ("    specify the public port number separately.                              \\n");
    printf ("    Current value is '%s'. Default value is '80'\\n", self->port);
    printf ("\\n");

    printf ("/config/server/public_port - Public port for HTTP requests\\n");
    printf ("    From command-line: --public_port newvalue\\n");
    printf ("    Specifies the public port for client requests, if not the standard port\\n");
    printf ("    number for HTTP (80). The server needs to know this value in order to  \\n");
    printf ("    format accurate redirection responses.                                 \\n");
    printf ("    Current value is '%s'.\\n", self->public_port);
    printf ("\\n");

    printf ("/config/server/trace - Trace network traffic?\\n");
    printf ("    From command-line: --trace newvalue\\n");
    printf ("    If set, the HTTP agent displays all socket i/o, which can be useful if  \\n");
    printf ("    you need to verify what exactly web clients are sending and getting back\\n");
    printf ("    from the server. Do not enable this with high loads because you will get\\n");
    printf ("    enormous amounts of output.                                             \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->trace);
    printf ("\\n");

    printf ("/config/server/animate - Animate HTTP agent?\\n");
    printf ("    From command-line: --animate newvalue\\n");
    printf ("    If set, the HTTP agent displays its activity, which can be useful if you\\n");
    printf ("    need to verify how HTTP requests are being processed. Do not enable this\\n");
    printf ("    with high loads because you will get enormous amounts of output.        \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->animate);
    printf ("\\n");

    printf ("/config/server/verbose - Verbose output?\\n");
    printf ("    From command-line: --verbose newvalue\\n");
    printf ("    If set, the HTTP agent displays all input and output headers, which can\\n");
    printf ("    be useful for debugging configuration issues.                          \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->verbose);
    printf ("\\n");

    printf ("/config/server/listen - Address (local network interface) to listen on\\n");
    printf ("    From command-line: --listen newvalue\\n");
    printf ("    Specifies the network interface on which the server should listen for\\n");
    printf ("    connections. By default this is *, meaning all interfaces. You would \\n");
    printf ("    choose to set an address when you use this application on a box with \\n");
    printf ("    multiple network interfaces, e.g. routing between two networks.      \\n");
    printf ("    Current value is '%s'. Default value is '*'\\n", self->listen);
    printf ("\\n");

    printf ("/config/host/webroot - Path to web root directory\\n");
    printf ("    From command-line: --webroot newvalue\\n");
    printf ("    Specifies the web root directory. Normally this is used to hold static\\n");
    printf ("    resources - HTML, images, etc. The web root maps to the / URI.        \\n");
    printf ("    Current value is '%s'. Default value is 'webpages'\\n", self->webroot);
    printf ("\\n");

    printf ("/config/host/defaults - Default file names\\n");
    printf ("    From command-line: --defaults newvalue\\n");
    printf ("    Specifies a space-delimited list of file names to use as defaults on an \\n");
    printf ("    empty URI path (the URI refers to a directory name). If none of the     \\n");
    printf ("    defaults are present, Xitami will look for a file called .httpdir and if\\n");
    printf ("    that is present, it will format a directory listing.                    \\n");
    printf ("    Current value is '%s'. Default value is 'index.html default.html'\\n", self->defaults);
    printf ("\\n");

    printf ("/config/host/default_types - Default document types\\n");
    printf ("    From command-line: --default_types newvalue\\n");
    printf ("    Specifies a list of default document types. The server implements a    \\n");
    printf ("    feature called 'default typing' in which you can specify URIs without  \\n");
    printf ("    file extensions. The default types are specified in order. You can use \\n");
    printf ("    this to publish a URI that specifies an HTML document, for example, and\\n");
    printf ("    later replace it with a PDF without breaking the published URI.        \\n");
    printf ("    Current value is '%s'. Default value is '.pdf .html'\\n", self->default_types);
    printf ("\\n");

    printf ("/config/resources/monitor - Monitor interval, seconds\\n");
    printf ("    From command-line: --monitor newvalue\\n");
    printf ("    Specifies the interval in seconds at which the server will report its   \\n");
    printf ("    request/response rates. If zero, no monitoring is shown. The recommended\\n");
    printf ("    value is 1, if monitoring is desired.                                   \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->monitor);
    printf ("\\n");

    printf ("/config/resources/record_stats - Record performance statistics\\n");
    printf ("    From command-line: --record_stats newvalue\\n");
    printf ("    Specifies whether or not the server records performance statistics in a\\n");
    printf ("    format suitable for importing into a spreadsheet. If set, the server   \\n");
    printf ("    creates a file called 'http_stats_(portnumber).del' which contains     \\n");
    printf ("    performance statistics in a comma-delimited format. For this setting to\\n");
    printf ("    take effect, the monitor option must also be set.                      \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->record_stats);
    printf ("\\n");

    printf ("/config/logging/log_path - Active log file directory\\n");
    printf ("    From command-line: --log_path newvalue\\n");
    printf ("    Specifies the directory into which active log files are placed. To get\\n");
    printf ("    the old behaviour, set this option to 'logs'.                         \\n");
    printf ("    Current value is '%s'. Default value is '/var/log/xitami'\\n", self->log_path);
    printf ("\\n");

    printf ("/config/logging/keep_logs - Keep log files\\n");
    printf ("    From command-line: --keep_logs newvalue\\n");
    printf ("    Specifies whether or not the server should keep logs. If zero, the   \\n");
    printf ("    server will not keep logs. If 1, it will keep an alert log, an access\\n");
    printf ("    log, and a debug log as specified.                                   \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->keep_logs);
    printf ("\\n");

    printf ("/config/logging/archive_cmd - Archive log file command\\n");
    printf ("    From command-line: --archive_cmd newvalue\\n");
    printf ("    If specified, this command is run on all archived log files. You can use\\n");
    printf ("    this option to compress archived log files, or clean up old and unwanted\\n");
    printf ("    files. When the command is run, the name of the archived log file is    \\n");
    printf ("    provided to it as a single argument.                                    \\n");
    printf ("    Current value is '%s'.\\n", self->archive_cmd);
    printf ("\\n");

    printf ("/config/logging/alert_log - Error log file name\\n");
    printf ("    From command-line: --alert_log newvalue\\n");
    printf ("    The name of the alert log file, which records all errors and warnings \\n");
    printf ("    issued by the server. Note that the server port number is always added\\n");
    printf ("    to the name of the log file.                                          \\n");
    printf ("    Current value is '%s'. Default value is 'alert.log'\\n", self->alert_log);
    printf ("\\n");

    printf ("/config/logging/access_log - Access log file name\\n");
    printf ("    From command-line: --access_log newvalue\\n");
    printf ("    The name of the access log file, which records all HTTP requests in the\\n");
    printf ("    specified access log format. Note that the server port number is always\\n");
    printf ("    added to the name of the log file.                                     \\n");
    printf ("    Current value is '%s'. Default value is 'access.log'\\n", self->access_log);
    printf ("\\n");

    printf ("/config/logging/access_log_format - Access log file format\\n");
    printf ("    From command-line: --access_log_format newvalue\\n");
    printf ("    The format of the access log file, which can be one of: CLF - NCSA/httpd\\n");
    printf ("    common logging format (basic); CLFX - Apache format; or other - custom  \\n");
    printf ("    log file format.                                                        \\n");
    printf ("    Current value is '%s'. Default value is 'CLFX'\\n", self->access_log_format);
    printf ("\\n");

    printf ("/config/logging/debug_log - Debug log file name\\n");
    printf ("    From command-line: --debug_log newvalue\\n");
    printf ("    The name of the debug log file, which records all normal activity,      \\n");
    printf ("    errors, and debug trace information. Note that the server port number is\\n");
    printf ("    always added to the name of the log file.                               \\n");
    printf ("    Current value is '%s'. Default value is 'debug.log'\\n", self->debug_log);
    printf ("\\n");

    printf ("/config/logging/syslog - Use syslog logging protocol\\n");
    printf ("    From command-line: --syslog newvalue\\n");
    printf ("    Specifies whether or not to use the syslog protocol for logging. By     \\n");
    printf ("    default log files are written directly to disk. If this option is set to\\n");
    printf ("    1, uses the syslog protocol for all log files. Uses a priority depending\\n");
    printf ("    on the type of log message (LOG_ERROR, LOG_INFO, LOG_DEBUG). Uses the   \\n");
    printf ("    LOG_DAEMON facility. Has no effect on Windows. Overrides the keep_logs  \\n");
    printf ("    option.                                                                 \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->syslog);
    printf ("\\n");

    printf ("/config/security/user - User to run server as\\n");
    printf ("    From command-line: --user newvalue\\n");
    printf ("    If you specify a user and group, the server will switch to that user and\\n");
    printf ("    group after opening its port, thus dropping any privileges it previously\\n");
    printf ("    had.                                                                    \\n");
    printf ("    Current value is '%s'.\\n", self->user);
    printf ("\\n");

    printf ("/config/security/group - Group to run server as\\n");
    printf ("    From command-line: --group newvalue\\n");
    printf ("    If you specify a user and group, the server will switch to that user and\\n");
    printf ("    group after opening its port, thus dropping any privileges it previously\\n");
    printf ("    had.                                                                    \\n");
    printf ("    Current value is '%s'.\\n", self->group);
    printf ("\\n");

    printf ("/config/security/blacklist - Blacklist filename\\n");
    printf ("    From command-line: --blacklist newvalue\\n");
    printf ("    Specifies a text file to use for the blacklist. The blacklist is an easy\\n");
    printf ("    way to blacklist IP addresses of spammers without modifying the main    \\n");
    printf ("    server configuration files. This file contains one IP address per line. \\n");
    printf ("    To disable blacklisting, set the option to a non-existent file or "".   \\n");
    printf ("    Current value is '%s'. Default value is 'blacklist.txt'\\n", self->blacklist);
    printf ("\\n");

    printf ("/config/security/basic_auth - Basic authentication filename\\n");
    printf ("    From command-line: --basic_auth newvalue\\n");
    printf ("    Specifies the file to use for Basic authentication. The file uses the   \\n");
    printf ("    Apache htpasswd format, see                                             \\n");
    printf ("    http://httpd.apache.org/docs/2.2/misc/password_encryptions.html. The    \\n");
    printf ("    plain text format of each line in this file, if you don't have the      \\n");
    printf ("    htpasswd tool, is '{username}:{password}'. You can mix digest and passwd\\n");
    printf ("    password entries in a single file.                                      \\n");
    printf ("    Current value is '%s'. Default value is 'passwd'\\n", self->basic_auth);
    printf ("\\n");

    printf ("/config/security/digest_auth - Digest authentication filename\\n");
    printf ("    From command-line: --digest_auth newvalue\\n");
    printf ("    Specifies the file to use for Digest authentication. The file uses the\\n");
    printf ("    Apache htdigest format, see                                           \\n");
    printf ("    http://httpd.apache.org/docs/2.2/misc/password_encryptions.html.      \\n");
    printf ("    Current value is '%s'. Default value is 'digest'\\n", self->digest_auth);
    printf ("\\n");

    printf ("/config/security/password_ttl - Password time-to-live, in seconds\\n");
    printf ("    From command-line: --password_ttl newvalue\\n");
    printf ("    If set to a non-zero value, passwords will expire this many seconds  \\n");
    printf ("    after they are first loaded into Xitami. You can reset an expired    \\n");
    printf ("    password by editing the password file or setting a new password using\\n");
    printf ("    the Apache htpasswd / htdigest tools.                                \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->password_ttl);
    printf ("\\n");

    printf ("/config/security/hit_quota - Password hit quota in pages\\n");
    printf ("    From command-line: --hit_quota newvalue\\n");
    printf ("    If greater than zero, specifies the number of requests that a browser  \\n");
    printf ("    can make using the same user id (+ realm, for Digest authentication).  \\n");
    printf ("    When this quota is exhausted, the id password expires. You can reset an\\n");
    printf ("    expired password by editing the password file or setting a new password\\n");
    printf ("    using the Apache htpasswd / htdigest tools.                            \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->hit_quota);
    printf ("\\n");

    printf ("/config/security/miss_quota - Password miss quota in requests\\n");
    printf ("    From command-line: --miss_quota newvalue\\n");
    printf ("    If greater than zero, specifies the number of failed requests that a   \\n");
    printf ("    browser can make using the same user id (+ realm, for Digest           \\n");
    printf ("    authentication). When this quota is exhausted, the id password expires.\\n");
    printf ("    You can reset an expired password by editing the password file or      \\n");
    printf ("    setting a new password using the Apache htpasswd / htdigest tools.     \\n");
    printf ("    Current value is %i. Default value is 5.\\n", self->miss_quota);
    printf ("\\n");

    printf ("/config/security/password_guard - Password guard timer, in seconds.\\n");
    printf ("    From command-line: --password_guard newvalue\\n");
    printf ("    If greater than zero, specifies the frequency with which Xitami saves  \\n");
    printf ("    password guard data. The password guard ensures that the hit and miss  \\n");
    printf ("    quotas are properly written to disk so that if Xitami is restarted this\\n");
    printf ("    data is not lost. If set to zero, Xitami will never save password guard\\n");
    printf ("    data.                                                                  \\n");
    printf ("    Current value is %i. Default value is 10.\\n", self->password_guard);
    printf ("\\n");

    printf ("/config/security/policy_trace - Debug policy execution?\\n");
    printf ("    From command-line: --policy_trace newvalue\\n");
    printf ("    Specifies whether policy execution is logged or not. Set this option to\\n");
    printf ("    get a visual trace of policy execution, especially when you write your \\n");
    printf ("    own policies. Do not use on production servers, it will generate vast  \\n");
    printf ("    amounts of output and slow things down.                                \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->policy_trace);
    printf ("\\n");

    printf ("/config/security/nervosity - Security nervosity, seconds\\n");
    printf ("    From command-line: --nervosity newvalue\\n");
    printf ("    Specifies the interval in seconds at which the server will reload       \\n");
    printf ("    externally modified security files (blacklist, password, and digest).   \\n");
    printf ("    You can reduce this to one second but this will cause latency spikes due\\n");
    printf ("    to CPU burn and memory churn if you have very large blacklist files.    \\n");
    printf ("    Current value is %i. Default value is 5.\\n", self->nervosity);
    printf ("\\n");

    printf ("/config/security/nonce_ttl - Nonce time-to-live, seconds\\n");
    printf ("    From command-line: --nonce_ttl newvalue\\n");
    printf ("    This specifies the validity for Digest authentication nonces. Reducing \\n");
    printf ("    this will increase chatter, as browsers need to request new nonces more\\n");
    printf ("    often, but reduces the opportunity for replay attacks.                 \\n");
    printf ("    Current value is %i. Default value is 60.\\n", self->nonce_ttl);
    printf ("\\n");

    printf ("/config/security/nonce_quota - Nonce quota in requests\\n");
    printf ("    From command-line: --nonce_quota newvalue\\n");
    printf ("    This specifies the number of requests that a browser can make using the\\n");
    printf ("    same Digest authentication nonce. When this quota is exhausted, the    \\n");
    printf ("    nonce expires. If set to 1, browsers must ask for a new nonce at each  \\n");
    printf ("    request.                                                               \\n");
    printf ("    Current value is %i. Default value is 25.\\n", self->nonce_quota);
    printf ("\\n");

    printf ("/config/security/refresh_by - Shell command for password refresh\\n");
    printf ("    From command-line: --refresh_by newvalue\\n");
    printf ("    If specified, the http_access_refresh module runs this command to   \\n");
    printf ("    refresh expired or unknown passwords. You may use this to tie Xitami\\n");
    printf ("    authentication to external or system authentication systems. See    \\n");
    printf ("    documentation for details.                                          \\n");
    printf ("    Current value is '%s'.\\n", self->refresh_by);
    printf ("\\n");

    printf ("/config/security/refresh_log - Stdout for password refresh\\n");
    printf ("    From command-line: --refresh_log newvalue\\n");
    printf ("    Specifies the log file for the refresh command. All messages produced by\\n");
    printf ("    the refresh command (stdout and stderr) will be appended to this file.  \\n");
    printf ("    Current value is '%s'. Default value is 'password_refresh.log'\\n", self->refresh_log);
    printf ("\\n");

    printf ("/config/security/address_quota - Maximum open connections per address\\n");
    printf ("    From command-line: --address_quota newvalue\\n");
    printf ("    If greater than zero, specifies the number of open connections a client\\n");
    printf ("    can have. Setting this value to zero allows unlimited connections per  \\n");
    printf ("    address. This setting is used by the DoS module to detect and handle   \\n");
    printf ("    simple attempts to crash the server by overloading it with lots of     \\n");
    printf ("    connections.                                                           \\n");
    printf ("    Current value is %i. Default value is 5.\\n", self->address_quota);
    printf ("\\n");

    printf ("/config/security/abort_delay - Pause time for hostile connections\\n");
    printf ("    From command-line: --abort_delay newvalue\\n");
    printf ("    This specifies the number of seconds that the server will wait, when    \\n");
    printf ("    shutting a connection due to detected hostile activity. The delay wastes\\n");
    printf ("    browser time, making attacks less efficient.                            \\n");
    printf ("    Current value is %i. Default value is 5.\\n", self->abort_delay);
    printf ("\\n");

    printf ("/config/tuning/coremark_cycles - Duration of CoreMark test\\n");
    printf ("    From command-line: --coremark_cycles newvalue\\n");
    printf ("    Defines the duration of the automatic CoreMark performance test, in  \\n");
    printf ("    event cycles. If 0, no test will be done. Setting this too high will \\n");
    printf ("    cause the server to burn CPU at startup. A good value is between 100k\\n");
    printf ("    and 1M.                                                              \\n");
    printf ("    Current value is %i. Default value is 250000.\\n", self->coremark_cycles);
    printf ("\\n");

    printf ("/config/tuning/header_max - Max size of HTTP header\\n");
    printf ("    From command-line: --header_max newvalue\\n");
    printf ("    Defines the limit for HTTP headers; the server will accept up to this   \\n");
    printf ("    much data. You can increase the limit if you have applications that pass\\n");
    printf ("    excessive amounts of data as URI arguments, but usually it is           \\n");
    printf ("    recommended to pass large web forms as POST body data.                  \\n");
    printf ("    Current value is %i. Default value is 32000.\\n", self->header_max);
    printf ("\\n");

    printf ("/config/tuning/bucket_max - Max size of content bucket\\n");
    printf ("    From command-line: --bucket_max newvalue\\n");
    printf ("    Defines the limit for content managed as a memory bucket. The HTTP   \\n");
    printf ("    server can handle contents of any size, but if they exceed the bucket\\n");
    printf ("    max they will be spooled to disk, which can be slow.                 \\n");
    printf ("    Current value is %i. Default value is 128000.\\n", self->bucket_max);
    printf ("\\n");

    printf ("/config/tuning/read_timeout - Network read timeout, seconds\\n");
    printf ("    From command-line: --read_timeout newvalue\\n");
    printf ("    Defines the timeout for network reads. If a client does not send data \\n");
    printf ("    within this time, it is considered dead. If you get unexpected timeout\\n");
    printf ("    errors in the logs, you may want to increase this value.              \\n");
    printf ("    Current value is %i. Default value is 60.\\n", self->read_timeout);
    printf ("\\n");

    printf ("/config/tuning/write_timeout - Network write timeout, seconds\\n");
    printf ("    From command-line: --write_timeout newvalue\\n");
    printf ("    Defines the timeout for network writes. If a client does not accept data\\n");
    printf ("    within this time, it is considered dead. If you get unexpected timeout  \\n");
    printf ("    errors in the logs, you may want to increase this value.                \\n");
    printf ("    Current value is %i. Default value is 60.\\n", self->write_timeout);
    printf ("\\n");

    printf ("/config/tuning/accept_retry_timeout - Timeout before retrying accept calls, seconds\\n");
    printf ("    From command-line: --accept_retry_timeout newvalue\\n");
    printf ("    Defines the amount of time the server will wait before retrying an    \\n");
    printf ("    accept call if the call failed due to running out of file descriptors.\\n");
    printf ("    Current value is %i. Default value is 60.\\n", self->accept_retry_timeout);
    printf ("\\n");

    printf ("/config/tuning/tcp_nodelay - TCP/IP NODELAY option\\n");
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
    printf ("    Current value is %i. Default value is 0.\\n", self->tcp_rcvbuf);
    printf ("\\n");

    printf ("/config/tuning/tcp_sndbuf - TCP/IP send buffer, in bytes\\n");
    printf ("    From command-line: --tcp_sndbuf newvalue\\n");
    printf ("    If this value is greater than zero, all client connections will use the\\n");
    printf ("    specified value. Note: setting this value is delicate, do not use this \\n");
    printf ("    option unless you know what you are doing.                             \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->tcp_sndbuf);
    printf ("\\n");

    printf ("/config/tuning/polling_threads - Number of polling OS threads\\n");
    printf ("    From command-line: --polling_threads newvalue\\n");
    printf ("    On multithreaded builds, defines the number of OS threads dedicated to\\n");
    printf ("    socket polling.                                                       \\n");
    printf ("    Current value is %i. Default value is 4.\\n", self->polling_threads);
    printf ("\\n");

    printf ("/config/tuning/working_threads - Number of working OS threads\\n");
    printf ("    From command-line: --working_threads newvalue\\n");
    printf ("    On multithreaded builds, defines the number of OS threads dedicated to\\n");
    printf ("    processing, that is, tasks other than socket polling.                 \\n");
    printf ("    Current value is %i. Default value is 4.\\n", self->working_threads);
    printf ("\\n");

    printf ("/config/cache/etag - ETag header for files\\n");
    printf ("    From command-line: --etag newvalue\\n");
    printf ("    Defines the ETag header value to use for files. If 'fixed', the ETag  \\n");
    printf ("    consists of the file's modified time, size, and inode. If the etag    \\n");
    printf ("    option is 'float', the ETag consists of the file's modified time and  \\n");
    printf ("    size only. This is useful for large deployments where files are served\\n");
    printf ("    from multiple file systems. If the ETag option is set to any other    \\n");
    printf ("    value, no ETag headers will be produced.                              \\n");
    printf ("    Current value is '%s'. Default value is 'fixed'\\n", self->etag);
    printf ("\\n");
</method>

<method name = "set server_name" template = "function">
    <argument name = "server_name" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/server", "name", server_name);
    server_name = ipr_config_getp (self->table, "/config/server", "name", NULL);
    assert (server_name);
    self->server_name = server_name;
</method>

<method name = "server_name" return = "server_name">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "server_name" type = "char *" />
    //
    if (self)
        server_name = self->server_name;
    else
        server_name = "X5";
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
        port = "80";
</method>

<method name = "set public_port" template = "function">
    <argument name = "public_port" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/server", "public_port", public_port);
    public_port = ipr_config_getp (self->table, "/config/server", "public_port", NULL);
    assert (public_port);
    self->public_port = public_port;
</method>

<method name = "public_port" return = "public_port">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "public_port" type = "char *" />
    //
    if (self)
        public_port = self->public_port;
    else
        public_port = "";
</method>

<method name = "set trace" template = "function">
    <argument name = "trace" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", trace);
    ipr_config_putp (self->table, "/config/server", "trace", strvalue);
    self->trace = trace;
</method>

<method name = "trace" return = "trace">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "trace" type = "Bool" />
    //
    if (self)
        trace = self->trace;
    else
        trace = 0;
</method>

<method name = "set animate" template = "function">
    <argument name = "animate" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", animate);
    ipr_config_putp (self->table, "/config/server", "animate", strvalue);
    self->animate = animate;
</method>

<method name = "animate" return = "animate">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "animate" type = "Bool" />
    //
    if (self)
        animate = self->animate;
    else
        animate = 0;
</method>

<method name = "set verbose" template = "function">
    <argument name = "verbose" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", verbose);
    ipr_config_putp (self->table, "/config/server", "verbose", strvalue);
    self->verbose = verbose;
</method>

<method name = "verbose" return = "verbose">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "verbose" type = "Bool" />
    //
    if (self)
        verbose = self->verbose;
    else
        verbose = 0;
</method>

<method name = "set listen" template = "function">
    <argument name = "listen" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/server", "listen", listen);
    listen = ipr_config_getp (self->table, "/config/server", "listen", NULL);
    assert (listen);
    self->listen = listen;
</method>

<method name = "listen" return = "listen">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "listen" type = "char *" />
    //
    if (self)
        listen = self->listen;
    else
        listen = "*";
</method>

<method name = "set webroot" template = "function">
    <argument name = "webroot" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/host", "webroot", webroot);
    webroot = ipr_config_getp (self->table, "/config/host", "webroot", NULL);
    assert (webroot);
    self->webroot = webroot;
</method>

<method name = "webroot" return = "webroot">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "webroot" type = "char *" />
    //
    if (self)
        webroot = self->webroot;
    else
        webroot = "webpages";
</method>

<method name = "set defaults" template = "function">
    <argument name = "defaults" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/host", "defaults", defaults);
    defaults = ipr_config_getp (self->table, "/config/host", "defaults", NULL);
    assert (defaults);
    self->defaults = defaults;
</method>

<method name = "defaults" return = "defaults">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "defaults" type = "char *" />
    //
    if (self)
        defaults = self->defaults;
    else
        defaults = "index.html default.html";
</method>

<method name = "set default_types" template = "function">
    <argument name = "default_types" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/host", "default_types", default_types);
    default_types = ipr_config_getp (self->table, "/config/host", "default_types", NULL);
    assert (default_types);
    self->default_types = default_types;
</method>

<method name = "default_types" return = "default_types">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "default_types" type = "char *" />
    //
    if (self)
        default_types = self->default_types;
    else
        default_types = ".pdf .html";
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
        monitor = 0;
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
        log_path = "/var/log/xitami";
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

<method name = "set access_log" template = "function">
    <argument name = "access_log" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "access_log", access_log);
    access_log = ipr_config_getp (self->table, "/config/logging", "access_log", NULL);
    assert (access_log);
    self->access_log = access_log;
</method>

<method name = "access_log" return = "access_log">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "access_log" type = "char *" />
    //
    if (self)
        access_log = self->access_log;
    else
        access_log = "access.log";
</method>

<method name = "set access_log_format" template = "function">
    <argument name = "access_log_format" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/logging", "access_log_format", access_log_format);
    access_log_format = ipr_config_getp (self->table, "/config/logging", "access_log_format", NULL);
    assert (access_log_format);
    self->access_log_format = access_log_format;
</method>

<method name = "access_log_format" return = "access_log_format">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "access_log_format" type = "char *" />
    //
    if (self)
        access_log_format = self->access_log_format;
    else
        access_log_format = "CLFX";
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

<method name = "set syslog" template = "function">
    <argument name = "syslog" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", syslog);
    ipr_config_putp (self->table, "/config/logging", "syslog", strvalue);
    self->syslog = syslog;
</method>

<method name = "syslog" return = "syslog">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "syslog" type = "int" />
    //
    if (self)
        syslog = self->syslog;
    else
        syslog = 0;
</method>

<method name = "set user" template = "function">
    <argument name = "user" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "user", user);
    user = ipr_config_getp (self->table, "/config/security", "user", NULL);
    assert (user);
    self->user = user;
</method>

<method name = "user" return = "user">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "user" type = "char *" />
    //
    if (self)
        user = self->user;
    else
        user = "";
</method>

<method name = "set group" template = "function">
    <argument name = "group" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "group", group);
    group = ipr_config_getp (self->table, "/config/security", "group", NULL);
    assert (group);
    self->group = group;
</method>

<method name = "group" return = "group">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "group" type = "char *" />
    //
    if (self)
        group = self->group;
    else
        group = "";
</method>

<method name = "set blacklist" template = "function">
    <argument name = "blacklist" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "blacklist", blacklist);
    blacklist = ipr_config_getp (self->table, "/config/security", "blacklist", NULL);
    assert (blacklist);
    self->blacklist = blacklist;
</method>

<method name = "blacklist" return = "blacklist">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "blacklist" type = "char *" />
    //
    if (self)
        blacklist = self->blacklist;
    else
        blacklist = "blacklist.txt";
</method>

<method name = "set basic_auth" template = "function">
    <argument name = "basic_auth" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "basic_auth", basic_auth);
    basic_auth = ipr_config_getp (self->table, "/config/security", "basic_auth", NULL);
    assert (basic_auth);
    self->basic_auth = basic_auth;
</method>

<method name = "basic_auth" return = "basic_auth">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "basic_auth" type = "char *" />
    //
    if (self)
        basic_auth = self->basic_auth;
    else
        basic_auth = "passwd";
</method>

<method name = "set digest_auth" template = "function">
    <argument name = "digest_auth" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "digest_auth", digest_auth);
    digest_auth = ipr_config_getp (self->table, "/config/security", "digest_auth", NULL);
    assert (digest_auth);
    self->digest_auth = digest_auth;
</method>

<method name = "digest_auth" return = "digest_auth">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "digest_auth" type = "char *" />
    //
    if (self)
        digest_auth = self->digest_auth;
    else
        digest_auth = "digest";
</method>

<method name = "set password_ttl" template = "function">
    <argument name = "password_ttl" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", password_ttl);
    ipr_config_putp (self->table, "/config/security", "password_ttl", strvalue);
    self->password_ttl = password_ttl;
</method>

<method name = "password_ttl" return = "password_ttl">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "password_ttl" type = "int" />
    //
    if (self)
        password_ttl = self->password_ttl;
    else
        password_ttl = 0;
</method>

<method name = "set hit_quota" template = "function">
    <argument name = "hit_quota" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", hit_quota);
    ipr_config_putp (self->table, "/config/security", "hit_quota", strvalue);
    self->hit_quota = hit_quota;
</method>

<method name = "hit_quota" return = "hit_quota">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "hit_quota" type = "int" />
    //
    if (self)
        hit_quota = self->hit_quota;
    else
        hit_quota = 0;
</method>

<method name = "set miss_quota" template = "function">
    <argument name = "miss_quota" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", miss_quota);
    ipr_config_putp (self->table, "/config/security", "miss_quota", strvalue);
    self->miss_quota = miss_quota;
</method>

<method name = "miss_quota" return = "miss_quota">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "miss_quota" type = "int" />
    //
    if (self)
        miss_quota = self->miss_quota;
    else
        miss_quota = 5;
</method>

<method name = "set password_guard" template = "function">
    <argument name = "password_guard" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", password_guard);
    ipr_config_putp (self->table, "/config/security", "password_guard", strvalue);
    self->password_guard = password_guard;
</method>

<method name = "password_guard" return = "password_guard">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "password_guard" type = "int" />
    //
    if (self)
        password_guard = self->password_guard;
    else
        password_guard = 10;
</method>

<method name = "set policy_trace" template = "function">
    <argument name = "policy_trace" type = "Bool" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", policy_trace);
    ipr_config_putp (self->table, "/config/security", "policy_trace", strvalue);
    self->policy_trace = policy_trace;
</method>

<method name = "policy_trace" return = "policy_trace">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "policy_trace" type = "Bool" />
    //
    if (self)
        policy_trace = self->policy_trace;
    else
        policy_trace = 0;
</method>

<method name = "set nervosity" template = "function">
    <argument name = "nervosity" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (nervosity < 1) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for nervosity, using %d",
            nervosity, 1);
        nervosity = 1;
    }
    icl_shortstr_fmt (strvalue, "%i", nervosity);
    ipr_config_putp (self->table, "/config/security", "nervosity", strvalue);
    self->nervosity = nervosity;
</method>

<method name = "nervosity" return = "nervosity">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "nervosity" type = "int" />
    //
    if (self)
        nervosity = self->nervosity;
    else
        nervosity = 5;
</method>

<method name = "set nonce_ttl" template = "function">
    <argument name = "nonce_ttl" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (nonce_ttl < 1) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for nonce_ttl, using %d",
            nonce_ttl, 1);
        nonce_ttl = 1;
    }
    icl_shortstr_fmt (strvalue, "%i", nonce_ttl);
    ipr_config_putp (self->table, "/config/security", "nonce_ttl", strvalue);
    self->nonce_ttl = nonce_ttl;
</method>

<method name = "nonce_ttl" return = "nonce_ttl">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "nonce_ttl" type = "int" />
    //
    if (self)
        nonce_ttl = self->nonce_ttl;
    else
        nonce_ttl = 60;
</method>

<method name = "set nonce_quota" template = "function">
    <argument name = "nonce_quota" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (nonce_quota < 1) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for nonce_quota, using %d",
            nonce_quota, 1);
        nonce_quota = 1;
    }
    icl_shortstr_fmt (strvalue, "%i", nonce_quota);
    ipr_config_putp (self->table, "/config/security", "nonce_quota", strvalue);
    self->nonce_quota = nonce_quota;
</method>

<method name = "nonce_quota" return = "nonce_quota">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "nonce_quota" type = "int" />
    //
    if (self)
        nonce_quota = self->nonce_quota;
    else
        nonce_quota = 25;
</method>

<method name = "set refresh_by" template = "function">
    <argument name = "refresh_by" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "refresh_by", refresh_by);
    refresh_by = ipr_config_getp (self->table, "/config/security", "refresh_by", NULL);
    assert (refresh_by);
    self->refresh_by = refresh_by;
</method>

<method name = "refresh_by" return = "refresh_by">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "refresh_by" type = "char *" />
    //
    if (self)
        refresh_by = self->refresh_by;
    else
        refresh_by = "";
</method>

<method name = "set refresh_log" template = "function">
    <argument name = "refresh_log" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/security", "refresh_log", refresh_log);
    refresh_log = ipr_config_getp (self->table, "/config/security", "refresh_log", NULL);
    assert (refresh_log);
    self->refresh_log = refresh_log;
</method>

<method name = "refresh_log" return = "refresh_log">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "refresh_log" type = "char *" />
    //
    if (self)
        refresh_log = self->refresh_log;
    else
        refresh_log = "password_refresh.log";
</method>

<method name = "set address_quota" template = "function">
    <argument name = "address_quota" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", address_quota);
    ipr_config_putp (self->table, "/config/security", "address_quota", strvalue);
    self->address_quota = address_quota;
</method>

<method name = "address_quota" return = "address_quota">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "address_quota" type = "int" />
    //
    if (self)
        address_quota = self->address_quota;
    else
        address_quota = 5;
</method>

<method name = "set abort_delay" template = "function">
    <argument name = "abort_delay" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", abort_delay);
    ipr_config_putp (self->table, "/config/security", "abort_delay", strvalue);
    self->abort_delay = abort_delay;
</method>

<method name = "abort_delay" return = "abort_delay">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "abort_delay" type = "int" />
    //
    if (self)
        abort_delay = self->abort_delay;
    else
        abort_delay = 5;
</method>

<method name = "set coremark_cycles" template = "function">
    <argument name = "coremark_cycles" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", coremark_cycles);
    ipr_config_putp (self->table, "/config/tuning", "coremark_cycles", strvalue);
    self->coremark_cycles = coremark_cycles;
</method>

<method name = "coremark_cycles" return = "coremark_cycles">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "coremark_cycles" type = "int" />
    //
    if (self)
        coremark_cycles = self->coremark_cycles;
    else
        coremark_cycles = 250000;
</method>

<method name = "set header_max" template = "function">
    <argument name = "header_max" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (header_max < 2048) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for header_max, using %d",
            header_max, 2048);
        header_max = 2048;
    }
    if (header_max > 2097152) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for header_max, using %d",
            header_max, 2097152);
        header_max = 2097152;
    }
    icl_shortstr_fmt (strvalue, "%i", header_max);
    ipr_config_putp (self->table, "/config/tuning", "header_max", strvalue);
    self->header_max = header_max;
</method>

<method name = "header_max" return = "header_max">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "header_max" type = "int" />
    //
    if (self)
        header_max = self->header_max;
    else
        header_max = 32000;
</method>

<method name = "set bucket_max" template = "function">
    <argument name = "bucket_max" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (bucket_max > 2097152) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for bucket_max, using %d",
            bucket_max, 2097152);
        bucket_max = 2097152;
    }
    icl_shortstr_fmt (strvalue, "%i", bucket_max);
    ipr_config_putp (self->table, "/config/tuning", "bucket_max", strvalue);
    self->bucket_max = bucket_max;
</method>

<method name = "bucket_max" return = "bucket_max">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "bucket_max" type = "int" />
    //
    if (self)
        bucket_max = self->bucket_max;
    else
        bucket_max = 128000;
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
        read_timeout = 60;
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
        write_timeout = 60;
</method>

<method name = "set accept_retry_timeout" template = "function">
    <argument name = "accept_retry_timeout" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (accept_retry_timeout < 1) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for accept_retry_timeout, using %d",
            accept_retry_timeout, 1);
        accept_retry_timeout = 1;
    }
    icl_shortstr_fmt (strvalue, "%i", accept_retry_timeout);
    ipr_config_putp (self->table, "/config/tuning", "accept_retry_timeout", strvalue);
    self->accept_retry_timeout = accept_retry_timeout;
</method>

<method name = "accept_retry_timeout" return = "accept_retry_timeout">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "accept_retry_timeout" type = "int" />
    //
    if (self)
        accept_retry_timeout = self->accept_retry_timeout;
    else
        accept_retry_timeout = 60;
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

<method name = "set polling_threads" template = "function">
    <argument name = "polling_threads" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", polling_threads);
    ipr_config_putp (self->table, "/config/tuning", "polling_threads", strvalue);
    self->polling_threads = polling_threads;
</method>

<method name = "polling_threads" return = "polling_threads">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "polling_threads" type = "int" />
    //
    if (self)
        polling_threads = self->polling_threads;
    else
        polling_threads = 4;
</method>

<method name = "set working_threads" template = "function">
    <argument name = "working_threads" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", working_threads);
    ipr_config_putp (self->table, "/config/tuning", "working_threads", strvalue);
    self->working_threads = working_threads;
</method>

<method name = "working_threads" return = "working_threads">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "working_threads" type = "int" />
    //
    if (self)
        working_threads = self->working_threads;
    else
        working_threads = 4;
</method>

<method name = "set etag" template = "function">
    <argument name = "etag" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/cache", "etag", etag);
    etag = ipr_config_getp (self->table, "/config/cache", "etag", NULL);
    assert (etag);
    self->etag = etag;
</method>

<method name = "etag" return = "etag">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "etag" type = "char *" />
    //
    if (self)
        etag = self->etag;
    else
        etag = "fixed";
</method>

<method name = "dump log" template = "function">
    <doc>
    Dump current option values to an smt_log file.
    </doc>
    <argument name = "logfile" type = "smt_log_t *">Log file to dump to</argument>
    smt_log_print (logfile, "*******************  Configuration Settings  ********************");
    smt_log_print (logfile, "server_name=%s", self->server_name);
    smt_log_print (logfile, "port=%s", self->port);
    smt_log_print (logfile, "public_port=%s", self->public_port);
    smt_log_print (logfile, "trace=%i", self->trace);
    smt_log_print (logfile, "animate=%i", self->animate);
    smt_log_print (logfile, "verbose=%i", self->verbose);
    smt_log_print (logfile, "listen=%s", self->listen);
    smt_log_print (logfile, "webroot=%s", self->webroot);
    smt_log_print (logfile, "defaults=%s", self->defaults);
    smt_log_print (logfile, "default_types=%s", self->default_types);
    smt_log_print (logfile, "monitor=%i", self->monitor);
    smt_log_print (logfile, "record_stats=%i", self->record_stats);
    smt_log_print (logfile, "log_path=%s", self->log_path);
    smt_log_print (logfile, "keep_logs=%i", self->keep_logs);
    smt_log_print (logfile, "archive_cmd=%s", self->archive_cmd);
    smt_log_print (logfile, "alert_log=%s", self->alert_log);
    smt_log_print (logfile, "access_log=%s", self->access_log);
    smt_log_print (logfile, "access_log_format=%s", self->access_log_format);
    smt_log_print (logfile, "debug_log=%s", self->debug_log);
    smt_log_print (logfile, "syslog=%i", self->syslog);
    smt_log_print (logfile, "user=%s", self->user);
    smt_log_print (logfile, "group=%s", self->group);
    smt_log_print (logfile, "blacklist=%s", self->blacklist);
    smt_log_print (logfile, "basic_auth=%s", self->basic_auth);
    smt_log_print (logfile, "digest_auth=%s", self->digest_auth);
    smt_log_print (logfile, "password_ttl=%i", self->password_ttl);
    smt_log_print (logfile, "hit_quota=%i", self->hit_quota);
    smt_log_print (logfile, "miss_quota=%i", self->miss_quota);
    smt_log_print (logfile, "password_guard=%i", self->password_guard);
    smt_log_print (logfile, "policy_trace=%i", self->policy_trace);
    smt_log_print (logfile, "nervosity=%i", self->nervosity);
    smt_log_print (logfile, "nonce_ttl=%i", self->nonce_ttl);
    smt_log_print (logfile, "nonce_quota=%i", self->nonce_quota);
    smt_log_print (logfile, "refresh_by=%s", self->refresh_by);
    smt_log_print (logfile, "refresh_log=%s", self->refresh_log);
    smt_log_print (logfile, "address_quota=%i", self->address_quota);
    smt_log_print (logfile, "abort_delay=%i", self->abort_delay);
    smt_log_print (logfile, "coremark_cycles=%i", self->coremark_cycles);
    smt_log_print (logfile, "header_max=%i", self->header_max);
    smt_log_print (logfile, "bucket_max=%i", self->bucket_max);
    smt_log_print (logfile, "read_timeout=%i", self->read_timeout);
    smt_log_print (logfile, "write_timeout=%i", self->write_timeout);
    smt_log_print (logfile, "accept_retry_timeout=%i", self->accept_retry_timeout);
    smt_log_print (logfile, "tcp_nodelay=%i", self->tcp_nodelay);
    smt_log_print (logfile, "tcp_rcvbuf=%i", self->tcp_rcvbuf);
    smt_log_print (logfile, "tcp_sndbuf=%i", self->tcp_sndbuf);
    smt_log_print (logfile, "polling_threads=%i", self->polling_threads);
    smt_log_print (logfile, "working_threads=%i", self->working_threads);
    smt_log_print (logfile, "etag=%s", self->etag);
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
        && strneq (item_name, "host")
        && strneq (item_name, "resources")
        && strneq (item_name, "logging")
        && strneq (item_name, "security")
        && strneq (item_name, "tuning")
        && strneq (item_name, "cache")
        && strneq (item_name, "access")
        && strneq (item_name, "content"))
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

    //  Get server_name property from config file or built-in defaults
    value = ipr_config_get (self->table, "name", NULL);
    if (value)
        self->server_name = value;
    else
        self->server_name = "X5";

    //  Get port property from config file or built-in defaults
    value = ipr_config_get (self->table, "port", NULL);
    if (value)
        self->port = value;
    else
        self->port = "80";

    //  Get public_port property from config file or built-in defaults
    value = ipr_config_get (self->table, "public_port", NULL);
    if (value)
        self->public_port = value;
    else
        self->public_port = "";

    //  Get trace property from config file or built-in defaults
    value = ipr_config_get (self->table, "trace", NULL);
    if (value)
        self->trace = (Bool) atol (value);
    else
        self->trace = 0;

    //  Get animate property from config file or built-in defaults
    value = ipr_config_get (self->table, "animate", NULL);
    if (value)
        self->animate = (Bool) atol (value);
    else
        self->animate = 0;

    //  Get verbose property from config file or built-in defaults
    value = ipr_config_get (self->table, "verbose", NULL);
    if (value)
        self->verbose = (Bool) atol (value);
    else
        self->verbose = 0;

    //  Get listen property from config file or built-in defaults
    value = ipr_config_get (self->table, "listen", NULL);
    if (value)
        self->listen = value;
    else
        self->listen = "*";

    //  All properties on host path
    ipr_config_locate (self->table, "/config/host", NULL);

    //  Get webroot property from config file or built-in defaults
    value = ipr_config_get (self->table, "webroot", NULL);
    if (value)
        self->webroot = value;
    else
        self->webroot = "webpages";

    //  Get defaults property from config file or built-in defaults
    value = ipr_config_get (self->table, "defaults", NULL);
    if (value)
        self->defaults = value;
    else
        self->defaults = "index.html default.html";

    //  Get default_types property from config file or built-in defaults
    value = ipr_config_get (self->table, "default_types", NULL);
    if (value)
        self->default_types = value;
    else
        self->default_types = ".pdf .html";

    //  All properties on resources path
    ipr_config_locate (self->table, "/config/resources", NULL);

    //  Get monitor property from config file or built-in defaults
    value = ipr_config_get (self->table, "monitor", NULL);
    if (value)
        self->monitor = (int) atol (value);
    else
        self->monitor = 0;

    //  Get record_stats property from config file or built-in defaults
    value = ipr_config_get (self->table, "record_stats", NULL);
    if (value)
        self->record_stats = (Bool) atol (value);
    else
        self->record_stats = 0;

    //  All properties on logging path
    ipr_config_locate (self->table, "/config/logging", NULL);

    //  Get log_path property from config file or built-in defaults
    value = ipr_config_get (self->table, "log_path", NULL);
    if (value)
        self->log_path = value;
    else
        self->log_path = "/var/log/xitami";

    //  Get keep_logs property from config file or built-in defaults
    value = ipr_config_get (self->table, "keep_logs", NULL);
    if (value)
        self->keep_logs = (Bool) atol (value);
    else
        self->keep_logs = 1;

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

    //  Get access_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "access_log", NULL);
    if (value)
        self->access_log = value;
    else
        self->access_log = "access.log";

    //  Get access_log_format property from config file or built-in defaults
    value = ipr_config_get (self->table, "access_log_format", NULL);
    if (value)
        self->access_log_format = value;
    else
        self->access_log_format = "CLFX";

    //  Get debug_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "debug_log", NULL);
    if (value)
        self->debug_log = value;
    else
        self->debug_log = "debug.log";

    //  Get syslog property from config file or built-in defaults
    value = ipr_config_get (self->table, "syslog", NULL);
    if (value)
        self->syslog = (int) atol (value);
    else
        self->syslog = 0;

    //  All properties on security path
    ipr_config_locate (self->table, "/config/security", NULL);

    //  Get user property from config file or built-in defaults
    value = ipr_config_get (self->table, "user", NULL);
    if (value)
        self->user = value;
    else
        self->user = "";

    //  Get group property from config file or built-in defaults
    value = ipr_config_get (self->table, "group", NULL);
    if (value)
        self->group = value;
    else
        self->group = "";

    //  Get blacklist property from config file or built-in defaults
    value = ipr_config_get (self->table, "blacklist", NULL);
    if (value)
        self->blacklist = value;
    else
        self->blacklist = "blacklist.txt";

    //  Get basic_auth property from config file or built-in defaults
    value = ipr_config_get (self->table, "basic_auth", NULL);
    if (value)
        self->basic_auth = value;
    else
        self->basic_auth = "passwd";

    //  Get digest_auth property from config file or built-in defaults
    value = ipr_config_get (self->table, "digest_auth", NULL);
    if (value)
        self->digest_auth = value;
    else
        self->digest_auth = "digest";

    //  Get password_ttl property from config file or built-in defaults
    value = ipr_config_get (self->table, "password_ttl", NULL);
    if (value)
        self->password_ttl = (int) atol (value);
    else
        self->password_ttl = 0;

    //  Get hit_quota property from config file or built-in defaults
    value = ipr_config_get (self->table, "hit_quota", NULL);
    if (value)
        self->hit_quota = (int) atol (value);
    else
        self->hit_quota = 0;

    //  Get miss_quota property from config file or built-in defaults
    value = ipr_config_get (self->table, "miss_quota", NULL);
    if (value)
        self->miss_quota = (int) atol (value);
    else
        self->miss_quota = 5;

    //  Get password_guard property from config file or built-in defaults
    value = ipr_config_get (self->table, "password_guard", NULL);
    if (value)
        self->password_guard = (int) atol (value);
    else
        self->password_guard = 10;

    //  Get policy_trace property from config file or built-in defaults
    value = ipr_config_get (self->table, "policy_trace", NULL);
    if (value)
        self->policy_trace = (Bool) atol (value);
    else
        self->policy_trace = 0;

    //  Get nervosity property from config file or built-in defaults
    value = ipr_config_get (self->table, "nervosity", NULL);
    if (value)
        self->nervosity = (int) atol (value);
    else
        self->nervosity = 5;
    if (self->nervosity < 1) {
        icl_console_print ("W: %s: illegal value %d for nervosity, using %d",
            self->filename, self->nervosity, 1);
        self->nervosity = 1;
    }

    //  Get nonce_ttl property from config file or built-in defaults
    value = ipr_config_get (self->table, "nonce_ttl", NULL);
    if (value)
        self->nonce_ttl = (int) atol (value);
    else
        self->nonce_ttl = 60;
    if (self->nonce_ttl < 1) {
        icl_console_print ("W: %s: illegal value %d for nonce_ttl, using %d",
            self->filename, self->nonce_ttl, 1);
        self->nonce_ttl = 1;
    }

    //  Get nonce_quota property from config file or built-in defaults
    value = ipr_config_get (self->table, "nonce_quota", NULL);
    if (value)
        self->nonce_quota = (int) atol (value);
    else
        self->nonce_quota = 25;
    if (self->nonce_quota < 1) {
        icl_console_print ("W: %s: illegal value %d for nonce_quota, using %d",
            self->filename, self->nonce_quota, 1);
        self->nonce_quota = 1;
    }

    //  Get refresh_by property from config file or built-in defaults
    value = ipr_config_get (self->table, "refresh_by", NULL);
    if (value)
        self->refresh_by = value;
    else
        self->refresh_by = "";

    //  Get refresh_log property from config file or built-in defaults
    value = ipr_config_get (self->table, "refresh_log", NULL);
    if (value)
        self->refresh_log = value;
    else
        self->refresh_log = "password_refresh.log";

    //  Get address_quota property from config file or built-in defaults
    value = ipr_config_get (self->table, "address_quota", NULL);
    if (value)
        self->address_quota = (int) atol (value);
    else
        self->address_quota = 5;

    //  Get abort_delay property from config file or built-in defaults
    value = ipr_config_get (self->table, "abort_delay", NULL);
    if (value)
        self->abort_delay = (int) atol (value);
    else
        self->abort_delay = 5;

    //  All properties on tuning path
    ipr_config_locate (self->table, "/config/tuning", NULL);

    //  Get coremark_cycles property from config file or built-in defaults
    value = ipr_config_get (self->table, "coremark_cycles", NULL);
    if (value)
        self->coremark_cycles = (int) atol (value);
    else
        self->coremark_cycles = 250000;

    //  Get header_max property from config file or built-in defaults
    value = ipr_config_get (self->table, "header_max", NULL);
    if (value)
        self->header_max = (int) atol (value);
    else
        self->header_max = 32000;
    if (self->header_max < 2048) {
        icl_console_print ("W: %s: illegal value %d for header_max, using %d",
            self->filename, self->header_max, 2048);
        self->header_max = 2048;
    }
    if (self->header_max > 2097152) {
        icl_console_print ("W: %s: illegal value %d for header_max, using %d",
            self->filename, self->header_max, 2097152);
        self->header_max = 2097152;
    }

    //  Get bucket_max property from config file or built-in defaults
    value = ipr_config_get (self->table, "bucket_max", NULL);
    if (value)
        self->bucket_max = (int) atol (value);
    else
        self->bucket_max = 128000;
    if (self->bucket_max > 2097152) {
        icl_console_print ("W: %s: illegal value %d for bucket_max, using %d",
            self->filename, self->bucket_max, 2097152);
        self->bucket_max = 2097152;
    }

    //  Get read_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "read_timeout", NULL);
    if (value)
        self->read_timeout = (int) atol (value);
    else
        self->read_timeout = 60;

    //  Get write_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "write_timeout", NULL);
    if (value)
        self->write_timeout = (int) atol (value);
    else
        self->write_timeout = 60;

    //  Get accept_retry_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "accept_retry_timeout", NULL);
    if (value)
        self->accept_retry_timeout = (int) atol (value);
    else
        self->accept_retry_timeout = 60;
    if (self->accept_retry_timeout < 1) {
        icl_console_print ("W: %s: illegal value %d for accept_retry_timeout, using %d",
            self->filename, self->accept_retry_timeout, 1);
        self->accept_retry_timeout = 1;
    }

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

    //  Get polling_threads property from config file or built-in defaults
    value = ipr_config_get (self->table, "polling_threads", NULL);
    if (value)
        self->polling_threads = (int) atol (value);
    else
        self->polling_threads = 4;

    //  Get working_threads property from config file or built-in defaults
    value = ipr_config_get (self->table, "working_threads", NULL);
    if (value)
        self->working_threads = (int) atol (value);
    else
        self->working_threads = 4;

    //  All properties on cache path
    ipr_config_locate (self->table, "/config/cache", NULL);

    //  Get etag property from config file or built-in defaults
    value = ipr_config_get (self->table, "etag", NULL);
    if (value)
        self->etag = value;
    else
        self->etag = "fixed";

    //  All properties on access path
    ipr_config_locate (self->table, "/config/access", NULL);

    //  All properties on content path
    ipr_config_locate (self->table, "/config/content", NULL);
}
</private>

<method name = "selftest">
</method>

</class>

