<?xml?>
<!--
    *** GENERATED FROM demo_client_config.opf BY opf_gen USING GSL/4 ***

    
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
    name      = "demo_client_config"
    comment   = "Client configuration properties"
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
    *demo_client_config;                //  Global properties object
</public>

<private>
$(selftype)
    *demo_client_config = NULL;         //  Global properties
</private>

<context>
    Bool
        tcp_nodelay;                    //  TCP/IP NODELAY option
    int
        tcp_rcvbuf;                     //  TCP/IP receive buffer, in bytes
    int
        tcp_sndbuf;                     //  TCP/IP send buffer, in bytes
    int
        high_water;                     //  High water mark for message flow control
    int
        low_water;                      //  Low water mark for message flow control
    int
        direct;                         //  Enables Direct Mode for connections
    int
        batching;                       //  Maximum size of Direct Mode batches
    char *
        on_overflow;                    //  Sets action for high-water overflow
    int
        chrono_enabled;                 //  Enable chrono pingbacks?
    int
        chrono_floor;                   //  Floor for chrono pingbacks
    int
        chrono_density;                 //  Sample density as permille
    int
        chrono_batch;                   //  Batch size for chrono pingbacks
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
    printf ("  --tcp_nodelay    TCP/IP NODELAY option\\n");
    printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\\n");
    printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\\n");
    printf ("  --high_water     High water mark for message flow control\\n");
    printf ("  --low_water      Low water mark for message flow control\\n");
    printf ("  --direct         Enables Direct Mode for connections\\n");
    printf ("  --batching       Maximum size of Direct Mode batches\\n");
    printf ("  --on_overflow    Sets action for high-water overflow\\n");
    printf ("  --chrono_enabled  Enable chrono pingbacks?\\n");
    printf ("  --chrono_floor   Floor for chrono pingbacks\\n");
    printf ("  --chrono_density  Sample density as permille\\n");
    printf ("  --chrono_batch   Batch size for chrono pingbacks\\n");
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
                if (streq (arg_name, "--tcp_nodelay"))
                    self_set_tcp_nodelay (self, atoi (arg_value));
                else
                if (streq (arg_name, "--tcp_rcvbuf"))
                    self_set_tcp_rcvbuf (self, atoi (arg_value));
                else
                if (streq (arg_name, "--tcp_sndbuf"))
                    self_set_tcp_sndbuf (self, atoi (arg_value));
                else
                if (streq (arg_name, "--high_water"))
                    self_set_high_water (self, atoi (arg_value));
                else
                if (streq (arg_name, "--low_water"))
                    self_set_low_water (self, atoi (arg_value));
                else
                if (streq (arg_name, "--direct"))
                    self_set_direct (self, atoi (arg_value));
                else
                if (streq (arg_name, "--batching"))
                    self_set_batching (self, atoi (arg_value));
                else
                if (streq (arg_name, "--on_overflow"))
                    self_set_on_overflow (self, arg_value);
                else
                if (streq (arg_name, "--chrono_enabled"))
                    self_set_chrono_enabled (self, atoi (arg_value));
                else
                if (streq (arg_name, "--chrono_floor"))
                    self_set_chrono_floor (self, atoi (arg_value));
                else
                if (streq (arg_name, "--chrono_density"))
                    self_set_chrono_density (self, atoi (arg_value));
                else
                if (streq (arg_name, "--chrono_batch"))
                    self_set_chrono_batch (self, atoi (arg_value));
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

    printf ("/config/tuning/tcp_nodelay - TCP/IP NODELAY option\\n");
    printf ("    From command-line: --tcp_nodelay newvalue\\n");
    printf ("    If this value is 1, socket data is written immediately, which is usually\\n");
    printf ("    good for latency. If this value is 0, data is buffered until there is a \\n");
    printf ("    full packet, which is usually good for throughput.                      \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->tcp_nodelay);
    printf ("\\n");

    printf ("/config/tuning/tcp_rcvbuf - TCP/IP receive buffer, in bytes\\n");
    printf ("    From command-line: --tcp_rcvbuf newvalue\\n");
    printf ("    If this value is greater than zero, the connection to the server will\\n");
    printf ("    use the specified value. Note: setting this value is delicate, do not\\n");
    printf ("    use this option unless you know what you are doing.                  \\n");
    printf ("    Current value is %i.\\n", self->tcp_rcvbuf);
    printf ("\\n");

    printf ("/config/tuning/tcp_sndbuf - TCP/IP send buffer, in bytes\\n");
    printf ("    From command-line: --tcp_sndbuf newvalue\\n");
    printf ("    If this value is greater than zero, the connection to the server will\\n");
    printf ("    use the specified value. Note: setting this value is delicate, do not\\n");
    printf ("    use this option unless you know what you are doing.                  \\n");
    printf ("    Current value is %i.\\n", self->tcp_sndbuf);
    printf ("\\n");

    printf ("/config/tuning/high_water - High water mark for message flow control\\n");
    printf ("    From command-line: --high_water newvalue\\n");
    printf ("    Number of messages in arrived queue when message flow from server is\\n");
    printf ("    stopped. If this property is 0, message flow is never switched off. \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->high_water);
    printf ("\\n");

    printf ("/config/tuning/low_water - Low water mark for message flow control\\n");
    printf ("    From command-line: --low_water newvalue\\n");
    printf ("    Number of messages in arrived queue when message flow from server is\\n");
    printf ("    started again after it had been switched off on high water mark.    \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->low_water);
    printf ("\\n");

    printf ("/config/tuning/direct - Enables Direct Mode for connections\\n");
    printf ("    From command-line: --direct newvalue\\n");
    printf ("    If set to 1, new connections will by default use Direct Mode.   \\n");
    printf ("    Applications can override this by setting the connection->direct\\n");
    printf ("    property on new connections.                                    \\n");
    printf ("    Current value is %i. Default value is 0.\\n", self->direct);
    printf ("\\n");

    printf ("/config/direct/batching - Maximum size of Direct Mode batches\\n");
    printf ("    From command-line: --batching newvalue\\n");
    printf ("    Defines the maximum batch size for Direct Mode opportunistic batching on\\n");
    printf ("    message sends. Setting this higher will improve throughput, and usually \\n");
    printf ("    with lower latency, but will cause higher memory consumption. Setting   \\n");
    printf ("    this to zero will switch off batching. Any value less than 2048 is      \\n");
    printf ("    treated as zero.                                                        \\n");
    printf ("    Current value is %i. Default value is 32768.\\n", self->batching);
    printf ("\\n");

    printf ("/config/direct/on_overflow - Sets action for high-water overflow\\n");
    printf ("    From command-line: --on_overflow newvalue\\n");
    printf ("    Specifies how the WireAPI stack should handle an overflow condition, as\\n");
    printf ("    defined by the arrived message queue reaching the high-water mark. The \\n");
    printf ("    allowed actions are: 'warn' - issue a message to the console, 'trim' - \\n");
    printf ("    discard old messages to make space for new ones, 'drop' - drop new     \\n");
    printf ("    incoming messages, and 'kill' - assert an error and kill the           \\n");
    printf ("    application.                                                           \\n");
    printf ("    Current value is '%s'. Default value is 'trim'\\n", self->on_overflow);
    printf ("\\n");

    printf ("/config/chrono/enabled - Enable chrono pingbacks?\\n");
    printf ("    From command-line: --chrono_enabled newvalue\\n");
    printf ("    Specifies whether chrono pingbacks are enabled or not. If not, then any\\n");
    printf ("    any chrono information in contents will be ignored. By default chrono  \\n");
    printf ("    pingbacks are enabled.                                                 \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->chrono_enabled);
    printf ("\\n");

    printf ("/config/chrono/floor - Floor for chrono pingbacks\\n");
    printf ("    From command-line: --chrono_floor newvalue\\n");
    printf ("    Specifies the floor for pingbacks. Only pingbacks that show an       \\n");
    printf ("    end-to-end latency greater or equal to the floor will be reported. If\\n");
    printf ("    you set this to zero, all pingbacks will be reported.                \\n");
    printf ("    Current value is %i. Default value is 100.\\n", self->chrono_floor);
    printf ("\\n");

    printf ("/config/chrono/density - Sample density as permille\\n");
    printf ("    From command-line: --chrono_density newvalue\\n");
    printf ("    Specifies the sampling density, as a permille. Valid values are 1 to  \\n");
    printf ("    1000. By default 1/1000th of messages will be sampled. The sampling is\\n");
    printf ("    randomized if the density is less than 1000 permille.                 \\n");
    printf ("    Current value is %i. Default value is 1.\\n", self->chrono_density);
    printf ("\\n");

    printf ("/config/chrono/batch - Batch size for chrono pingbacks\\n");
    printf ("    From command-line: --chrono_batch newvalue\\n");
    printf ("    Specifies the batching size for chrono pingback messages. These track  \\n");
    printf ("    the latency of contents passing through the architecture. A chrono     \\n");
    printf ("    pingback is about 15 characters long, so the default batch and promille\\n");
    printf ("    settings will result in one pingback message every 10,000 received     \\n");
    printf ("    messages. If you set the batch size too low, you may get excessive back\\n");
    printf ("    chatter on the network and if you make it too high, you may get latency\\n");
    printf ("    spikes.                                                                \\n");
    printf ("    Current value is %i. Default value is 150.\\n", self->chrono_batch);
    printf ("\\n");
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

<method name = "set high_water" template = "function">
    <argument name = "high_water" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", high_water);
    ipr_config_putp (self->table, "/config/tuning", "high_water", strvalue);
    self->high_water = high_water;
</method>

<method name = "high_water" return = "high_water">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "high_water" type = "int" />
    //
    if (self)
        high_water = self->high_water;
    else
        high_water = 0;
</method>

<method name = "set low_water" template = "function">
    <argument name = "low_water" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", low_water);
    ipr_config_putp (self->table, "/config/tuning", "low_water", strvalue);
    self->low_water = low_water;
</method>

<method name = "low_water" return = "low_water">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "low_water" type = "int" />
    //
    if (self)
        low_water = self->low_water;
    else
        low_water = 0;
</method>

<method name = "set direct" template = "function">
    <argument name = "direct" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", direct);
    ipr_config_putp (self->table, "/config/tuning", "direct", strvalue);
    self->direct = direct;
</method>

<method name = "direct" return = "direct">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "direct" type = "int" />
    //
    if (self)
        direct = self->direct;
    else
        direct = 0;
</method>

<method name = "set batching" template = "function">
    <argument name = "batching" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    if (batching > 2097152) {
        icl_console_print ("W: &lt;command line&gt;: illegal value %d for batching, using %d",
            batching, 2097152);
        batching = 2097152;
    }
    icl_shortstr_fmt (strvalue, "%i", batching);
    ipr_config_putp (self->table, "/config/direct", "batching", strvalue);
    self->batching = batching;
</method>

<method name = "batching" return = "batching">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "batching" type = "int" />
    //
    if (self)
        batching = self->batching;
    else
        batching = 32768;
</method>

<method name = "set on_overflow" template = "function">
    <argument name = "on_overflow" type = "char *" />
    <inherit name = "wrlock" />
    ipr_config_putp (self->table, "/config/direct", "on_overflow", on_overflow);
    on_overflow = ipr_config_getp (self->table, "/config/direct", "on_overflow", NULL);
    assert (on_overflow);
    self->on_overflow = on_overflow;
</method>

<method name = "on_overflow" return = "on_overflow">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "on_overflow" type = "char *" />
    //
    if (self)
        on_overflow = self->on_overflow;
    else
        on_overflow = "trim";
</method>

<method name = "set chrono_enabled" template = "function">
    <argument name = "chrono_enabled" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", chrono_enabled);
    ipr_config_putp (self->table, "/config/chrono", "enabled", strvalue);
    self->chrono_enabled = chrono_enabled;
</method>

<method name = "chrono_enabled" return = "chrono_enabled">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "chrono_enabled" type = "int" />
    //
    if (self)
        chrono_enabled = self->chrono_enabled;
    else
        chrono_enabled = 1;
</method>

<method name = "set chrono_floor" template = "function">
    <argument name = "chrono_floor" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", chrono_floor);
    ipr_config_putp (self->table, "/config/chrono", "floor", strvalue);
    self->chrono_floor = chrono_floor;
</method>

<method name = "chrono_floor" return = "chrono_floor">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "chrono_floor" type = "int" />
    //
    if (self)
        chrono_floor = self->chrono_floor;
    else
        chrono_floor = 100;
</method>

<method name = "set chrono_density" template = "function">
    <argument name = "chrono_density" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", chrono_density);
    ipr_config_putp (self->table, "/config/chrono", "density", strvalue);
    self->chrono_density = chrono_density;
</method>

<method name = "chrono_density" return = "chrono_density">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "chrono_density" type = "int" />
    //
    if (self)
        chrono_density = self->chrono_density;
    else
        chrono_density = 1;
</method>

<method name = "set chrono_batch" template = "function">
    <argument name = "chrono_batch" type = "int" />
    <inherit name = "wrlock" />
    <local>
    icl_shortstr_t
        strvalue;
    </local>
    //
    icl_shortstr_fmt (strvalue, "%i", chrono_batch);
    ipr_config_putp (self->table, "/config/chrono", "batch", strvalue);
    self->chrono_batch = chrono_batch;
</method>

<method name = "chrono_batch" return = "chrono_batch">
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <declare name = "chrono_batch" type = "int" />
    //
    if (self)
        chrono_batch = self->chrono_batch;
    else
        chrono_batch = 150;
</method>

<method name = "dump log" template = "function">
    <doc>
    Dump current option values to an smt_log file.
    </doc>
    <argument name = "logfile" type = "smt_log_t *">Log file to dump to</argument>
    smt_log_print (logfile, "*******************  Configuration Settings  ********************");
    smt_log_print (logfile, "tcp_nodelay=%i", self->tcp_nodelay);
    smt_log_print (logfile, "tcp_rcvbuf=%i", self->tcp_rcvbuf);
    smt_log_print (logfile, "tcp_sndbuf=%i", self->tcp_sndbuf);
    smt_log_print (logfile, "high_water=%i", self->high_water);
    smt_log_print (logfile, "low_water=%i", self->low_water);
    smt_log_print (logfile, "direct=%i", self->direct);
    smt_log_print (logfile, "batching=%i", self->batching);
    smt_log_print (logfile, "on_overflow=%s", self->on_overflow);
    smt_log_print (logfile, "chrono_enabled=%i", self->chrono_enabled);
    smt_log_print (logfile, "chrono_floor=%i", self->chrono_floor);
    smt_log_print (logfile, "chrono_density=%i", self->chrono_density);
    smt_log_print (logfile, "chrono_batch=%i", self->chrono_batch);
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
        if (strneq (item_name, "tuning")
        && strneq (item_name, "direct")
        && strneq (item_name, "chrono"))
            icl_console_print ("W: %s: unknown item '%s', ignored",
                self->filename, item_name);
        ipr_config_next (self->table);
    }
}

static void s_get_properties ($(selftype) *self)
{
    char
        *value;                         //  Configured value

    //  All properties on tuning path
    ipr_config_locate (self->table, "/config/tuning", NULL);

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

    //  Get high_water property from config file or built-in defaults
    value = ipr_config_get (self->table, "high_water", NULL);
    if (value)
        self->high_water = (int) atol (value);
    else
        self->high_water = 0;

    //  Get low_water property from config file or built-in defaults
    value = ipr_config_get (self->table, "low_water", NULL);
    if (value)
        self->low_water = (int) atol (value);
    else
        self->low_water = 0;

    //  Get direct property from config file or built-in defaults
    value = ipr_config_get (self->table, "direct", NULL);
    if (value)
        self->direct = (int) atol (value);
    else
        self->direct = 0;

    //  All properties on direct path
    ipr_config_locate (self->table, "/config/direct", NULL);

    //  Get batching property from config file or built-in defaults
    value = ipr_config_get (self->table, "batching", NULL);
    if (value)
        self->batching = (int) atol (value);
    else
        self->batching = 32768;
    if (self->batching > 2097152) {
        icl_console_print ("W: %s: illegal value %d for batching, using %d",
            self->filename, self->batching, 2097152);
        self->batching = 2097152;
    }

    //  Get on_overflow property from config file or built-in defaults
    value = ipr_config_get (self->table, "on_overflow", NULL);
    if (value)
        self->on_overflow = value;
    else
        self->on_overflow = "trim";

    //  All properties on chrono path
    ipr_config_locate (self->table, "/config/chrono", NULL);

    //  Get chrono_enabled property from config file or built-in defaults
    value = ipr_config_get (self->table, "enabled", NULL);
    if (value)
        self->chrono_enabled = (int) atol (value);
    else
        self->chrono_enabled = 1;

    //  Get chrono_floor property from config file or built-in defaults
    value = ipr_config_get (self->table, "floor", NULL);
    if (value)
        self->chrono_floor = (int) atol (value);
    else
        self->chrono_floor = 100;

    //  Get chrono_density property from config file or built-in defaults
    value = ipr_config_get (self->table, "density", NULL);
    if (value)
        self->chrono_density = (int) atol (value);
    else
        self->chrono_density = 1;

    //  Get chrono_batch property from config file or built-in defaults
    value = ipr_config_get (self->table, "batch", NULL);
    if (value)
        self->chrono_batch = (int) atol (value);
    else
        self->chrono_batch = 150;
}
</private>

<method name = "selftest">
</method>

</class>

