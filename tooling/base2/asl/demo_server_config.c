/*---------------------------------------------------------------------------
    demo_server_config.c - demo_server_config component

    Generated from demo_server_config.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "demo_server_config.h"         //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_server_config_t

//  Structure of the demo_server_config object

struct _demo_server_config_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
char *
    port;                           //  Server port for client connections
char *
    listen;                         //  Address (local network interface) to listen on
char *
    user;                           //  User to run server as
char *
    group;                          //  Group to run server as
int
    monitor;                        //  Monitor interval, seconds
Bool
    record_stats;                   //  Record performance statistics
int
    heartbeat;                      //  Heartbeat from client to server, seconds
int
    setup_timeout;                  //  Connection setup timeout, seconds
int
    close_timeout;                  //  Connection close timeout, seconds
int
    accept_retry_timeout;           //  Timeout before retrying accept calls, seconds
Bool
    tcp_nodelay;                    //  TCP/IP NODELAY option
int
    tcp_rcvbuf;                     //  TCP/IP receive buffer, in bytes
int
    tcp_sndbuf;                     //  TCP/IP send buffer, in bytes
int
    frame_max;                      //  Maximum size of AMQP content frames
int
    direct;                         //  Enables Direct Mode for client connections
char *
    log_path;                       //  Active log file directory
Bool
    keep_logs;                      //  Keep log files
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
int
    syslog;                         //  Use syslog logging protocol
int
    batching;                       //  Maximum size of Direct Mode batches
    char * filename;                    //  
    ipr_config_t * table;               //  
};

//  Shorthands for class methods

#define self_new            demo_server_config_new
#define self_annihilate     demo_server_config_annihilate
#define self_load_xmlfile   demo_server_config_load_xmlfile
#define self_load_bucket    demo_server_config_load_bucket
#define self_shadow         demo_server_config_shadow
#define self_commit         demo_server_config_commit
#define self_rollback       demo_server_config_rollback
#define self_cmdline_help   demo_server_config_cmdline_help
#define self_cmdline_parse  demo_server_config_cmdline_parse
#define self_options_help   demo_server_config_options_help
#define self_set_port       demo_server_config_set_port
#define self_port           demo_server_config_port
#define self_set_listen     demo_server_config_set_listen
#define self_listen         demo_server_config_listen
#define self_set_user       demo_server_config_set_user
#define self_user           demo_server_config_user
#define self_set_group      demo_server_config_set_group
#define self_group          demo_server_config_group
#define self_set_monitor    demo_server_config_set_monitor
#define self_monitor        demo_server_config_monitor
#define self_set_record_stats  demo_server_config_set_record_stats
#define self_record_stats   demo_server_config_record_stats
#define self_set_heartbeat  demo_server_config_set_heartbeat
#define self_heartbeat      demo_server_config_heartbeat
#define self_set_setup_timeout  demo_server_config_set_setup_timeout
#define self_setup_timeout  demo_server_config_setup_timeout
#define self_set_close_timeout  demo_server_config_set_close_timeout
#define self_close_timeout  demo_server_config_close_timeout
#define self_set_accept_retry_timeout  demo_server_config_set_accept_retry_timeout
#define self_accept_retry_timeout  demo_server_config_accept_retry_timeout
#define self_set_tcp_nodelay  demo_server_config_set_tcp_nodelay
#define self_tcp_nodelay    demo_server_config_tcp_nodelay
#define self_set_tcp_rcvbuf  demo_server_config_set_tcp_rcvbuf
#define self_tcp_rcvbuf     demo_server_config_tcp_rcvbuf
#define self_set_tcp_sndbuf  demo_server_config_set_tcp_sndbuf
#define self_tcp_sndbuf     demo_server_config_tcp_sndbuf
#define self_set_frame_max  demo_server_config_set_frame_max
#define self_frame_max      demo_server_config_frame_max
#define self_set_direct     demo_server_config_set_direct
#define self_direct         demo_server_config_direct
#define self_set_log_path   demo_server_config_set_log_path
#define self_log_path       demo_server_config_log_path
#define self_set_keep_logs  demo_server_config_set_keep_logs
#define self_keep_logs      demo_server_config_keep_logs
#define self_set_archive_cmd  demo_server_config_set_archive_cmd
#define self_archive_cmd    demo_server_config_archive_cmd
#define self_set_alert_log  demo_server_config_set_alert_log
#define self_alert_log      demo_server_config_alert_log
#define self_set_daily_log  demo_server_config_set_daily_log
#define self_daily_log      demo_server_config_daily_log
#define self_set_debug_log  demo_server_config_set_debug_log
#define self_debug_log      demo_server_config_debug_log
#define self_set_trace      demo_server_config_set_trace
#define self_trace          demo_server_config_trace
#define self_set_syslog     demo_server_config_set_syslog
#define self_syslog         demo_server_config_syslog
#define self_set_batching   demo_server_config_set_batching
#define self_batching       demo_server_config_batching
#define self_dump_log       demo_server_config_dump_log
#define self_selftest       demo_server_config_selftest
#define self_filename       demo_server_config_filename
#define self_set_filename   demo_server_config_set_filename
#define self_table          demo_server_config_table
#define self_terminate      demo_server_config_terminate
#define self_show           demo_server_config_show
#define self_destroy        demo_server_config_destroy
#define self_alloc          demo_server_config_alloc
#define self_free           demo_server_config_free
#define self_read_lock      demo_server_config_read_lock
#define self_write_lock     demo_server_config_write_lock
#define self_unlock         demo_server_config_unlock
#define self_cache_initialise  demo_server_config_cache_initialise
#define self_cache_purge    demo_server_config_cache_purge
#define self_cache_terminate  demo_server_config_cache_terminate
#define self_show_animation  demo_server_config_show_animation
#define self_new_in_scope   demo_server_config_new_in_scope

static void
    demo_server_config_annihilate (
demo_server_config_t * ( * self_p )     //  Reference to object reference
);

#define demo_server_config_alloc()      demo_server_config_alloc_ (__FILE__, __LINE__)
static demo_server_config_t *
    demo_server_config_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_server_config_free (
demo_server_config_t * self             //  Object reference
);

static void
    demo_server_config_cache_initialise (
void);

static void
    demo_server_config_cache_purge (
void);

static void
    demo_server_config_cache_terminate (
void);

Bool
    demo_server_config_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void s_validate_items (demo_server_config_t *self);
static void s_get_properties (demo_server_config_t *self);
demo_server_config_t
    *demo_server_config = NULL;         //  Global properties
static void s_validate_items (demo_server_config_t *self)
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
        && strneq (item_name, "logging")
        && strneq (item_name, "direct"))
            icl_console_print ("W: %s: unknown item '%s', ignored",
                self->filename, item_name);
        ipr_config_next (self->table);
    }
}

static void s_get_properties (demo_server_config_t *self)
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

    //  Get listen property from config file or built-in defaults
    value = ipr_config_get (self->table, "listen", NULL);
    if (value)
        self->listen = value;
    else
        self->listen = "*";

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

    //  All properties on resources path
    ipr_config_locate (self->table, "/config/resources", NULL);

    //  Get monitor property from config file or built-in defaults
    value = ipr_config_get (self->table, "monitor", NULL);
    if (value)
        self->monitor = (int) atol (value);
    else
        self->monitor = 1;

    //  Get record_stats property from config file or built-in defaults
    value = ipr_config_get (self->table, "record_stats", NULL);
    if (value)
        self->record_stats = (Bool) atol (value);
    else
        self->record_stats = 0;

    //  All properties on tuning path
    ipr_config_locate (self->table, "/config/tuning", NULL);

    //  Get heartbeat property from config file or built-in defaults
    value = ipr_config_get (self->table, "heartbeat", NULL);
    if (value)
        self->heartbeat = (int) atol (value);
    else
        self->heartbeat = 2;

    //  Get setup_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "setup_timeout", NULL);
    if (value)
        self->setup_timeout = (int) atol (value);
    else
        self->setup_timeout = 5;

    //  Get close_timeout property from config file or built-in defaults
    value = ipr_config_get (self->table, "close_timeout", NULL);
    if (value)
        self->close_timeout = (int) atol (value);
    else
        self->close_timeout = 5;

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
    if (self->frame_max > 2097152) {
        icl_console_print ("W: %s: illegal value %d for frame_max, using %d",
            self->filename, self->frame_max, 2097152);
        self->frame_max = 2097152;
    }

    //  Get direct property from config file or built-in defaults
    value = ipr_config_get (self->table, "direct", NULL);
    if (value)
        self->direct = (int) atol (value);
    else
        self->direct = 0;

    //  All properties on logging path
    ipr_config_locate (self->table, "/config/logging", NULL);

    //  Get log_path property from config file or built-in defaults
    value = ipr_config_get (self->table, "log_path", NULL);
    if (value)
        self->log_path = value;
    else
        self->log_path = "/var/log/demo";

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

    //  Get syslog property from config file or built-in defaults
    value = ipr_config_get (self->table, "syslog", NULL);
    if (value)
        self->syslog = (int) atol (value);
    else
        self->syslog = 0;

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
}
/*  -------------------------------------------------------------------------
    demo_server_config_new

    Type: Component method
    Creates and initialises a new demo_server_config_t object, returns a
    reference to the created object.
    Creates an empty configuration set.  To fill this, use the load and
    load_bucket methods.
    -------------------------------------------------------------------------
 */

demo_server_config_t *
    demo_server_config_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    demo_server_config_t *
        self = NULL;                    //  Object reference

    self = demo_server_config_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_SERVER_CONFIG_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_server_config_show_);
#endif

//
self->table = ipr_config_new (NULL, FALSE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_config_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_config_annihilate (
    demo_server_config_t * ( * self_p )  //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    demo_server_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    DEMO_SERVER_CONFIG_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

ipr_config_destroy (&self->table);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    demo_server_config_load_xmlfile

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config file into the property set, merging the new loaded data
    with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_load_xmlfile (
    demo_server_config_t * self,        //  Reference to object
    char * filename,                    //  Config file to load
    Bool required                       //  Config file is required?
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
assert (filename);
if (!ipr_config_load (self->table, filename, required)) {
    self_set_filename (self, filename);
    s_validate_items (self);
    s_get_properties (self);
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_load_bucket

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config resource (as a bucket) into the property set, merging the
    new loaded data with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_load_bucket (
    demo_server_config_t * self,        //  Reference to object
    ipr_bucket_t * bucket               //  Config data to load
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
assert (bucket);
ipr_config_load_bucket (self->table, bucket);
s_validate_items (self);
s_get_properties (self);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_shadow

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads the data from a different property set, and updates all the properties.
    Use this when you want to provide your code with multiple property classes
    that actually contain an identical configuration set.  To use:
    my_config = my_config_new (NULL, NULL, FALSE);
    my_config_shadow (self, your_config->config);
    -------------------------------------------------------------------------
 */

int
    demo_server_config_shadow (
    demo_server_config_t * self,        //  Reference to object
    ipr_config_t * source               //  Config data to shadow
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
//  Get rid of our own config data
ipr_config_destroy (&self->table);
//  Grab a handle onto the source data
self->table = ipr_config_dup (source);
//  Update our properties, without validation
s_get_properties (self);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_commit

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the current configuration values to the specified file.  If the
    provided filename is null, uses the original config filename. If both
    filenames are null, does nothing.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_commit (
    demo_server_config_t * self,        //  Reference to object
    char * filename                     //  Config file to save
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
if (filename)
    ipr_config_save (self->table, filename);
else
if (strused (self->filename))
    ipr_config_save (self->table, self->filename);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_rollback

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Reloads the saved configuration from the saved file, if any.
    Any changes made to the current configuration are lost.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_rollback (
    demo_server_config_t * self         //  Reference to object
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
ipr_config_destroy (&self->table);
self->table = ipr_config_new (self->filename, TRUE);
s_get_properties (self);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_cmdline_help

    Type: Component method
    Prints command-line help for setting options from the command
    line.
    -------------------------------------------------------------------------
 */

void
    demo_server_config_cmdline_help (
void)
{

printf ("Options that override config values; these take an argument:\n");
printf ("  --port           Server port for client connections\n");
printf ("  --listen         Address (local network interface) to listen on\n");
printf ("  --user           User to run server as\n");
printf ("  --group          Group to run server as\n");
printf ("  --monitor        Monitor interval, seconds\n");
printf ("  --record_stats   Record performance statistics\n");
printf ("  --heartbeat      Heartbeat from client to server, seconds\n");
printf ("  --setup_timeout  Connection setup timeout, seconds\n");
printf ("  --close_timeout  Connection close timeout, seconds\n");
printf ("  --accept_retry_timeout  Timeout before retrying accept calls, seconds\n");
printf ("  --tcp_nodelay    TCP/IP NODELAY option\n");
printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\n");
printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\n");
printf ("  --frame_max      Maximum size of AMQP content frames\n");
printf ("  --direct         Enables Direct Mode for client connections\n");
printf ("  --log_path       Active log file directory\n");
printf ("  --keep_logs      Keep log files\n");
printf ("  --archive_cmd    Archive command\n");
printf ("  --alert_log      Alert log file name\n");
printf ("  --daily_log      Daily log file name\n");
printf ("  --debug_log      Debug log file name\n");
printf ("  --trace          Protocol trace level\n");
printf ("  --syslog         Use syslog logging protocol\n");
printf ("  --batching       Maximum size of Direct Mode batches\n");
}
/*  -------------------------------------------------------------------------
    demo_server_config_cmdline_parse

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Parses recognized command-line options starting with --.  Prints an error
    message and exits if the syntax is invalid.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_cmdline_parse (
    demo_server_config_t * self,        //  Reference to object
    char * name,                        //  Name of application
    int argc,                           //  Argument count
    char ** argv                        //  Argument list
)
{
int
    argn;                           //  Argument number
char
    *arg_name,
    *arg_value;
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

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
            if (streq (arg_name, "--listen"))
                self_set_listen (self, arg_value);
            else
            if (streq (arg_name, "--user"))
                self_set_user (self, arg_value);
            else
            if (streq (arg_name, "--group"))
                self_set_group (self, arg_value);
            else
            if (streq (arg_name, "--monitor"))
                self_set_monitor (self, atoi (arg_value));
            else
            if (streq (arg_name, "--record_stats"))
                self_set_record_stats (self, atoi (arg_value));
            else
            if (streq (arg_name, "--heartbeat"))
                self_set_heartbeat (self, atoi (arg_value));
            else
            if (streq (arg_name, "--setup_timeout"))
                self_set_setup_timeout (self, atoi (arg_value));
            else
            if (streq (arg_name, "--close_timeout"))
                self_set_close_timeout (self, atoi (arg_value));
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
            if (streq (arg_name, "--frame_max"))
                self_set_frame_max (self, atoi (arg_value));
            else
            if (streq (arg_name, "--direct"))
                self_set_direct (self, atoi (arg_value));
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
            if (streq (arg_name, "--daily_log"))
                self_set_daily_log (self, arg_value);
            else
            if (streq (arg_name, "--debug_log"))
                self_set_debug_log (self, arg_value);
            else
            if (streq (arg_name, "--trace"))
                self_set_trace (self, atoi (arg_value));
            else
            if (streq (arg_name, "--syslog"))
                self_set_syslog (self, atoi (arg_value));
            else
            if (streq (arg_name, "--batching"))
                self_set_batching (self, atoi (arg_value));
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


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_options_help

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Prints full help for all configuration options.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_options_help (
    demo_server_config_t * self,        //  Reference to object
    char * name                         //  Name of application
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

printf ("These are the configured options for %s\n\n", name);

printf ("/config/server/port - Server port for client connections\n");
printf ("    From command-line: --port newvalue\n");
printf ("    Specifies the port on which the server should open its connections.\n");
printf ("    Current value is '%s'. Default value is '7654'\n", self->port);
printf ("\n");

printf ("/config/server/listen - Address (local network interface) to listen on\n");
printf ("    From command-line: --listen newvalue\n");
printf ("    Specifies the network interface on which the server should listen for  \n");
printf ("    connections. By default this is *, meaning all interfaces. You would   \n");
printf ("    choose to set an address when you use OpenAMQ on a server with multiple\n");
printf ("    network interfaces, e.g. routing between two networks.                 \n");
printf ("    Current value is '%s'. Default value is '*'\n", self->listen);
printf ("\n");

printf ("/config/server/user - User to run server as\n");
printf ("    From command-line: --user newvalue\n");
printf ("    If you specify a user and group, the server will switch to that user and\n");
printf ("    group after opening its port, thus dropping any privileges it previously\n");
printf ("    had.                                                                    \n");
printf ("    Current value is '%s'.\n", self->user);
printf ("\n");

printf ("/config/server/group - Group to run server as\n");
printf ("    From command-line: --group newvalue\n");
printf ("    If you specify a user and group, the server will switch to that user and\n");
printf ("    group after opening its port, thus dropping any privileges it previously\n");
printf ("    had.                                                                    \n");
printf ("    Current value is '%s'.\n", self->group);
printf ("\n");

printf ("/config/resources/monitor - Monitor interval, seconds\n");
printf ("    From command-line: --monitor newvalue\n");
printf ("    Specifies the interval in seconds at which the server will report its   \n");
printf ("    message rates. If zero, no monitoring is shown. The recommended value is\n");
printf ("    1, if monitoring is desired.                                            \n");
printf ("    Current value is %i. Default value is 1.\n", self->monitor);
printf ("\n");

printf ("/config/resources/record_stats - Record performance statistics\n");
printf ("    From command-line: --record_stats newvalue\n");
printf ("    Specifies whether or not the server records performance statistics in a \n");
printf ("    format suitable for importing into a spreadsheet. If set, the server    \n");
printf ("    creates a file called 'amq_server_stats_(portnumber).del' which contains\n");
printf ("    performance statistics in a comma-delimited format. For this setting to \n");
printf ("    take effect, the monitor option must also be set.                       \n");
printf ("    Current value is %i. Default value is 0.\n", self->record_stats);
printf ("\n");

printf ("/config/tuning/heartbeat - Heartbeat from client to server, seconds\n");
printf ("    From command-line: --heartbeat newvalue\n");
printf ("    Defines the timeout for connection heartbeating.\n");
printf ("    Current value is %i. Default value is 2.\n", self->heartbeat);
printf ("\n");

printf ("/config/tuning/setup_timeout - Connection setup timeout, seconds\n");
printf ("    From command-line: --setup_timeout newvalue\n");
printf ("    Defines the maximum amount of time connection negotiation may take,\n");
printf ("    before the connection is forcibly closed by the server.            \n");
printf ("    Current value is %i. Default value is 5.\n", self->setup_timeout);
printf ("\n");

printf ("/config/tuning/close_timeout - Connection close timeout, seconds\n");
printf ("    From command-line: --close_timeout newvalue\n");
printf ("    Defines the maximum amount of time the server will wait for a client to\n");
printf ("    send a connection.close-ok when closing the connection due to an       \n");
printf ("    exception.                                                             \n");
printf ("    Current value is %i. Default value is 5.\n", self->close_timeout);
printf ("\n");

printf ("/config/tuning/accept_retry_timeout - Timeout before retrying accept calls, seconds\n");
printf ("    From command-line: --accept_retry_timeout newvalue\n");
printf ("    Defines the amount of time the server will wait before retrying an    \n");
printf ("    accept call if the call failed due to running out of file descriptors.\n");
printf ("    Current value is %i. Default value is 60.\n", self->accept_retry_timeout);
printf ("\n");

printf ("/config/tuning/tcp_nodelay - TCP/IP NODELAY option\n");
printf ("    From command-line: --tcp_nodelay newvalue\n");
printf ("    If this value is 1, socket data is written immediately, which is usually\n");
printf ("    good for latency. If this value is 0, data is buffered until there is a \n");
printf ("    full packet, which is usually good for throughput.                      \n");
printf ("    Current value is %i. Default value is 1.\n", self->tcp_nodelay);
printf ("\n");

printf ("/config/tuning/tcp_rcvbuf - TCP/IP receive buffer, in bytes\n");
printf ("    From command-line: --tcp_rcvbuf newvalue\n");
printf ("    If this value is greater than zero, all client connections will use the\n");
printf ("    specified value. Note: setting this value is delicate, do not use this \n");
printf ("    option unless you know what you are doing.                             \n");
printf ("    Current value is %i.\n", self->tcp_rcvbuf);
printf ("\n");

printf ("/config/tuning/tcp_sndbuf - TCP/IP send buffer, in bytes\n");
printf ("    From command-line: --tcp_sndbuf newvalue\n");
printf ("    If this value is greater than zero, all client connections will use the\n");
printf ("    specified value. Note: setting this value is delicate, do not use this \n");
printf ("    option unless you know what you are doing.                             \n");
printf ("    Current value is %i.\n", self->tcp_sndbuf);
printf ("\n");

printf ("/config/tuning/frame_max - Maximum size of AMQP content frames\n");
printf ("    From command-line: --frame_max newvalue\n");
printf ("    Defines the maximum frame size the server will accept. Larger frame     \n");
printf ("    sizes are useful for applications that create very large message headers\n");
printf ("    but will cause congestion on multi-channel connections.                 \n");
printf ("    Current value is %i. Default value is 32768.\n", self->frame_max);
printf ("\n");

printf ("/config/tuning/direct - Enables Direct Mode for client connections\n");
printf ("    From command-line: --direct newvalue\n");
printf ("    If set to 1, new connections will by default use Direct Mode.   \n");
printf ("    Applications can override this by setting the connection->direct\n");
printf ("    property on new connections.                                    \n");
printf ("    Current value is %i. Default value is 0.\n", self->direct);
printf ("\n");

printf ("/config/logging/log_path - Active log file directory\n");
printf ("    From command-line: --log_path newvalue\n");
printf ("    Specifies the directory into which active log files are placed. To get\n");
printf ("    the old behaviour, set this option to 'logs'.                         \n");
printf ("    Current value is '%s'. Default value is '/var/log/demo'\n", self->log_path);
printf ("\n");

printf ("/config/logging/keep_logs - Keep log files\n");
printf ("    From command-line: --keep_logs newvalue\n");
printf ("    Specifies whether or not the server should keep logs. If zero, the      \n");
printf ("    server will not keep logs. If 1, it will keep an alert log, a daily log,\n");
printf ("    and a debug log as specified.                                           \n");
printf ("    Current value is %i. Default value is 1.\n", self->keep_logs);
printf ("\n");

printf ("/config/logging/archive_cmd - Archive command\n");
printf ("    From command-line: --archive_cmd newvalue\n");
printf ("    If specified, this command is run on all archived log files. You can use\n");
printf ("    this option to compress archived log files, or clean up old and unwanted\n");
printf ("    files. When the command is run, the name of the archived log file is    \n");
printf ("    provided as an argument to it.                                          \n");
printf ("    Current value is '%s'.\n", self->archive_cmd);
printf ("\n");

printf ("/config/logging/alert_log - Alert log file name\n");
printf ("    From command-line: --alert_log newvalue\n");
printf ("    The name of the alert log file, which records all alerts issued by the \n");
printf ("    server. Note that the server port number is always added to the name of\n");
printf ("    the log file.                                                          \n");
printf ("    Current value is '%s'. Default value is 'alert.log'\n", self->alert_log);
printf ("\n");

printf ("/config/logging/daily_log - Daily log file name\n");
printf ("    From command-line: --daily_log newvalue\n");
printf ("    The name of the daily log file, which records all normal activity as   \n");
printf ("    well as errors. Note that the server port number is always added to the\n");
printf ("    name of the log file.                                                  \n");
printf ("    Current value is '%s'. Default value is 'daily.log'\n", self->daily_log);
printf ("\n");

printf ("/config/logging/debug_log - Debug log file name\n");
printf ("    From command-line: --debug_log newvalue\n");
printf ("    The name of the debug log file, which records all normal activity,      \n");
printf ("    errors, and debug trace information. Note that the server port number is\n");
printf ("    always added to the name of the log file.                               \n");
printf ("    Current value is '%s'. Default value is 'debug.log'\n", self->debug_log);
printf ("\n");

printf ("/config/logging/trace - Protocol trace level\n");
printf ("    From command-line: --trace newvalue\n");
printf ("    Specifies the protocol debug level, which can be 0 (no tracing), 1 (show\n");
printf ("    information and protocol methods), 2 (show protocol animation), 3 (show \n");
printf ("    wire-level data). You should not use any tracing on production servers  \n");
printf ("    except in low-volume scenarios.                                         \n");
printf ("    Current value is %i. Default value is 0.\n", self->trace);
printf ("\n");

printf ("/config/logging/syslog - Use syslog logging protocol\n");
printf ("    From command-line: --syslog newvalue\n");
printf ("    Specifies whether or not to use the syslog protocol for logging. By     \n");
printf ("    default log files are written directly to disk. If this option is set to\n");
printf ("    1, uses the syslog protocol for all log files. Uses a priority depending\n");
printf ("    on the type of log message (LOG_ERROR, LOG_INFO, LOG_DEBUG). Uses the   \n");
printf ("    LOG_DAEMON facility. Has no effect on Windows. Overrides the keep_logs  \n");
printf ("    option.                                                                 \n");
printf ("    Current value is %i. Default value is 0.\n", self->syslog);
printf ("\n");

printf ("/config/direct/batching - Maximum size of Direct Mode batches\n");
printf ("    From command-line: --batching newvalue\n");
printf ("    Defines the maximum batch size for Direct Mode opportunistic batching on\n");
printf ("    message sends. Setting this higher will improve throughput, and usually \n");
printf ("    with lower latency, but will cause higher memory consumption. Setting   \n");
printf ("    this to zero will switch off batching. Any value less than 2048 is      \n");
printf ("    treated as zero.                                                        \n");
printf ("    Current value is %i. Default value is 32768.\n", self->batching);
printf ("\n");


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_port

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_port (
    demo_server_config_t * self,        //  Reference to object
    char * port                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "port", port);
port = ipr_config_getp (self->table, "/config/server", "port", NULL);
assert (port);
self->port = port;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_port

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_port (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        port;                           //  Not documented

//
if (self)
    port = self->port;
else
    port = "7654";

    return (port);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_listen

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_listen (
    demo_server_config_t * self,        //  Reference to object
    char * listen                       //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "listen", listen);
listen = ipr_config_getp (self->table, "/config/server", "listen", NULL);
assert (listen);
self->listen = listen;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_listen

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_listen (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        listen;                         //  Not documented

//
if (self)
    listen = self->listen;
else
    listen = "*";

    return (listen);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_user

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_user (
    demo_server_config_t * self,        //  Reference to object
    char * user                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "user", user);
user = ipr_config_getp (self->table, "/config/server", "user", NULL);
assert (user);
self->user = user;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_user

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_user (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        user;                           //  Not documented

//
if (self)
    user = self->user;
else
    user = "";

    return (user);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_group

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_group (
    demo_server_config_t * self,        //  Reference to object
    char * group                        //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "group", group);
group = ipr_config_getp (self->table, "/config/server", "group", NULL);
assert (group);
self->group = group;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_group

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_group (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        group;                          //  Not documented

//
if (self)
    group = self->group;
else
    group = "";

    return (group);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_monitor

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_monitor (
    demo_server_config_t * self,        //  Reference to object
    int monitor                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", monitor);
ipr_config_putp (self->table, "/config/resources", "monitor", strvalue);
self->monitor = monitor;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_monitor

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_monitor (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        monitor;                        //  Not documented

//
if (self)
    monitor = self->monitor;
else
    monitor = 1;

    return (monitor);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_record_stats

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_record_stats (
    demo_server_config_t * self,        //  Reference to object
    Bool record_stats                   //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", record_stats);
ipr_config_putp (self->table, "/config/resources", "record_stats", strvalue);
self->record_stats = record_stats;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_record_stats

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_server_config_record_stats (
    demo_server_config_t * self         //  Reference to self
)
{
    Bool
        record_stats;                   //  Not documented

//
if (self)
    record_stats = self->record_stats;
else
    record_stats = 0;

    return (record_stats);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_heartbeat

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_heartbeat (
    demo_server_config_t * self,        //  Reference to object
    int heartbeat                       //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", heartbeat);
ipr_config_putp (self->table, "/config/tuning", "heartbeat", strvalue);
self->heartbeat = heartbeat;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_heartbeat

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_heartbeat (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        heartbeat;                      //  Not documented

//
if (self)
    heartbeat = self->heartbeat;
else
    heartbeat = 2;

    return (heartbeat);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_setup_timeout

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_setup_timeout (
    demo_server_config_t * self,        //  Reference to object
    int setup_timeout                   //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", setup_timeout);
ipr_config_putp (self->table, "/config/tuning", "setup_timeout", strvalue);
self->setup_timeout = setup_timeout;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_setup_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_setup_timeout (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        setup_timeout;                  //  Not documented

//
if (self)
    setup_timeout = self->setup_timeout;
else
    setup_timeout = 5;

    return (setup_timeout);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_close_timeout

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_close_timeout (
    demo_server_config_t * self,        //  Reference to object
    int close_timeout                   //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", close_timeout);
ipr_config_putp (self->table, "/config/tuning", "close_timeout", strvalue);
self->close_timeout = close_timeout;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_close_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_close_timeout (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        close_timeout;                  //  Not documented

//
if (self)
    close_timeout = self->close_timeout;
else
    close_timeout = 5;

    return (close_timeout);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_accept_retry_timeout

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_accept_retry_timeout (
    demo_server_config_t * self,        //  Reference to object
    int accept_retry_timeout            //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
if (accept_retry_timeout < 1) {
    icl_console_print ("W: <command line>: illegal value %d for accept_retry_timeout, using %d",
        accept_retry_timeout, 1);
    accept_retry_timeout = 1;
}
icl_shortstr_fmt (strvalue, "%i", accept_retry_timeout);
ipr_config_putp (self->table, "/config/tuning", "accept_retry_timeout", strvalue);
self->accept_retry_timeout = accept_retry_timeout;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_accept_retry_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_accept_retry_timeout (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        accept_retry_timeout;           //  Not documented

//
if (self)
    accept_retry_timeout = self->accept_retry_timeout;
else
    accept_retry_timeout = 60;

    return (accept_retry_timeout);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_tcp_nodelay

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_tcp_nodelay (
    demo_server_config_t * self,        //  Reference to object
    Bool tcp_nodelay                    //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", tcp_nodelay);
ipr_config_putp (self->table, "/config/tuning", "tcp_nodelay", strvalue);
self->tcp_nodelay = tcp_nodelay;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_tcp_nodelay

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_server_config_tcp_nodelay (
    demo_server_config_t * self         //  Reference to self
)
{
    Bool
        tcp_nodelay;                    //  Not documented

//
if (self)
    tcp_nodelay = self->tcp_nodelay;
else
    tcp_nodelay = 1;

    return (tcp_nodelay);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_tcp_rcvbuf

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_tcp_rcvbuf (
    demo_server_config_t * self,        //  Reference to object
    int tcp_rcvbuf                      //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", tcp_rcvbuf);
ipr_config_putp (self->table, "/config/tuning", "tcp_rcvbuf", strvalue);
self->tcp_rcvbuf = tcp_rcvbuf;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_tcp_rcvbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_tcp_rcvbuf (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        tcp_rcvbuf;                     //  Not documented

//
if (self)
    tcp_rcvbuf = self->tcp_rcvbuf;
else
    tcp_rcvbuf = 0;

    return (tcp_rcvbuf);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_tcp_sndbuf

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_tcp_sndbuf (
    demo_server_config_t * self,        //  Reference to object
    int tcp_sndbuf                      //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", tcp_sndbuf);
ipr_config_putp (self->table, "/config/tuning", "tcp_sndbuf", strvalue);
self->tcp_sndbuf = tcp_sndbuf;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_tcp_sndbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_tcp_sndbuf (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        tcp_sndbuf;                     //  Not documented

//
if (self)
    tcp_sndbuf = self->tcp_sndbuf;
else
    tcp_sndbuf = 0;

    return (tcp_sndbuf);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_frame_max

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_frame_max (
    demo_server_config_t * self,        //  Reference to object
    int frame_max                       //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
if (frame_max < 2048) {
    icl_console_print ("W: <command line>: illegal value %d for frame_max, using %d",
        frame_max, 2048);
    frame_max = 2048;
}
if (frame_max > 2097152) {
    icl_console_print ("W: <command line>: illegal value %d for frame_max, using %d",
        frame_max, 2097152);
    frame_max = 2097152;
}
icl_shortstr_fmt (strvalue, "%i", frame_max);
ipr_config_putp (self->table, "/config/tuning", "frame_max", strvalue);
self->frame_max = frame_max;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_frame_max

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_frame_max (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        frame_max;                      //  Not documented

//
if (self)
    frame_max = self->frame_max;
else
    frame_max = 32768;

    return (frame_max);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_direct

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_direct (
    demo_server_config_t * self,        //  Reference to object
    int direct                          //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", direct);
ipr_config_putp (self->table, "/config/tuning", "direct", strvalue);
self->direct = direct;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_direct

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_direct (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        direct;                         //  Not documented

//
if (self)
    direct = self->direct;
else
    direct = 0;

    return (direct);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_log_path

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_log_path (
    demo_server_config_t * self,        //  Reference to object
    char * log_path                     //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "log_path", log_path);
log_path = ipr_config_getp (self->table, "/config/logging", "log_path", NULL);
assert (log_path);
self->log_path = log_path;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_log_path

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_log_path (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        log_path;                       //  Not documented

//
if (self)
    log_path = self->log_path;
else
    log_path = "/var/log/demo";

    return (log_path);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_keep_logs

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_keep_logs (
    demo_server_config_t * self,        //  Reference to object
    Bool keep_logs                      //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", keep_logs);
ipr_config_putp (self->table, "/config/logging", "keep_logs", strvalue);
self->keep_logs = keep_logs;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_keep_logs

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_server_config_keep_logs (
    demo_server_config_t * self         //  Reference to self
)
{
    Bool
        keep_logs;                      //  Not documented

//
if (self)
    keep_logs = self->keep_logs;
else
    keep_logs = 1;

    return (keep_logs);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_archive_cmd

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_archive_cmd (
    demo_server_config_t * self,        //  Reference to object
    char * archive_cmd                  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "archive_cmd", archive_cmd);
archive_cmd = ipr_config_getp (self->table, "/config/logging", "archive_cmd", NULL);
assert (archive_cmd);
self->archive_cmd = archive_cmd;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_archive_cmd

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_archive_cmd (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        archive_cmd;                    //  Not documented

//
if (self)
    archive_cmd = self->archive_cmd;
else
    archive_cmd = "";

    return (archive_cmd);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_alert_log

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_alert_log (
    demo_server_config_t * self,        //  Reference to object
    char * alert_log                    //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "alert_log", alert_log);
alert_log = ipr_config_getp (self->table, "/config/logging", "alert_log", NULL);
assert (alert_log);
self->alert_log = alert_log;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_alert_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_alert_log (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        alert_log;                      //  Not documented

//
if (self)
    alert_log = self->alert_log;
else
    alert_log = "alert.log";

    return (alert_log);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_daily_log

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_daily_log (
    demo_server_config_t * self,        //  Reference to object
    char * daily_log                    //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "daily_log", daily_log);
daily_log = ipr_config_getp (self->table, "/config/logging", "daily_log", NULL);
assert (daily_log);
self->daily_log = daily_log;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_daily_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_daily_log (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        daily_log;                      //  Not documented

//
if (self)
    daily_log = self->daily_log;
else
    daily_log = "daily.log";

    return (daily_log);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_debug_log

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_debug_log (
    demo_server_config_t * self,        //  Reference to object
    char * debug_log                    //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "debug_log", debug_log);
debug_log = ipr_config_getp (self->table, "/config/logging", "debug_log", NULL);
assert (debug_log);
self->debug_log = debug_log;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_debug_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_debug_log (
    demo_server_config_t * self         //  Reference to self
)
{
    char *
        debug_log;                      //  Not documented

//
if (self)
    debug_log = self->debug_log;
else
    debug_log = "debug.log";

    return (debug_log);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_trace

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_trace (
    demo_server_config_t * self,        //  Reference to object
    int trace                           //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", trace);
ipr_config_putp (self->table, "/config/logging", "trace", strvalue);
self->trace = trace;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_trace

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_trace (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        trace;                          //  Not documented

//
if (self)
    trace = self->trace;
else
    trace = 0;

    return (trace);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_syslog

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_syslog (
    demo_server_config_t * self,        //  Reference to object
    int syslog                          //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", syslog);
ipr_config_putp (self->table, "/config/logging", "syslog", strvalue);
self->syslog = syslog;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_syslog

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_syslog (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        syslog;                         //  Not documented

//
if (self)
    syslog = self->syslog;
else
    syslog = 0;

    return (syslog);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_batching

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_set_batching (
    demo_server_config_t * self,        //  Reference to object
    int batching                        //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

icl_shortstr_t
    strvalue;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

//
if (batching > 2097152) {
    icl_console_print ("W: <command line>: illegal value %d for batching, using %d",
        batching, 2097152);
    batching = 2097152;
}
icl_shortstr_fmt (strvalue, "%i", batching);
ipr_config_putp (self->table, "/config/direct", "batching", strvalue);
self->batching = batching;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_batching

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_config_batching (
    demo_server_config_t * self         //  Reference to self
)
{
    int
        batching;                       //  Not documented

//
if (self)
    batching = self->batching;
else
    batching = 32768;

    return (batching);
}
/*  -------------------------------------------------------------------------
    demo_server_config_dump_log

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    Dump current option values to an smt_log file.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_dump_log (
    demo_server_config_t * self,        //  Reference to object
    smt_log_t * logfile                 //  Log file to dump to
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

smt_log_print (logfile, "*******************  Configuration Settings  ********************");
smt_log_print (logfile, "port=%s", self->port);
smt_log_print (logfile, "listen=%s", self->listen);
smt_log_print (logfile, "user=%s", self->user);
smt_log_print (logfile, "group=%s", self->group);
smt_log_print (logfile, "monitor=%i", self->monitor);
smt_log_print (logfile, "record_stats=%i", self->record_stats);
smt_log_print (logfile, "heartbeat=%i", self->heartbeat);
smt_log_print (logfile, "setup_timeout=%i", self->setup_timeout);
smt_log_print (logfile, "close_timeout=%i", self->close_timeout);
smt_log_print (logfile, "accept_retry_timeout=%i", self->accept_retry_timeout);
smt_log_print (logfile, "tcp_nodelay=%i", self->tcp_nodelay);
smt_log_print (logfile, "tcp_rcvbuf=%i", self->tcp_rcvbuf);
smt_log_print (logfile, "tcp_sndbuf=%i", self->tcp_sndbuf);
smt_log_print (logfile, "frame_max=%i", self->frame_max);
smt_log_print (logfile, "direct=%i", self->direct);
smt_log_print (logfile, "log_path=%s", self->log_path);
smt_log_print (logfile, "keep_logs=%i", self->keep_logs);
smt_log_print (logfile, "archive_cmd=%s", self->archive_cmd);
smt_log_print (logfile, "alert_log=%s", self->alert_log);
smt_log_print (logfile, "daily_log=%s", self->daily_log);
smt_log_print (logfile, "debug_log=%s", self->debug_log);
smt_log_print (logfile, "trace=%i", self->trace);
smt_log_print (logfile, "syslog=%i", self->syslog);
smt_log_print (logfile, "batching=%i", self->batching);
smt_log_print (logfile, "");


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_config_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_server_config_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_server_config_filename (
    demo_server_config_t * self         //  Not documented
)
{
    char *
        filename;                       //  Not documented

filename = self->filename;

    return (filename);
}
/*  -------------------------------------------------------------------------
    demo_server_config_set_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_config_set_filename (
    demo_server_config_t * self,        //  Not documented
    char * filename                     //  Not documented
)
{

icl_mem_free (self->filename);
self->filename = icl_mem_strdup (filename);
}
/*  -------------------------------------------------------------------------
    demo_server_config_table

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_config_t *
    demo_server_config_table (
    demo_server_config_t * self         //  Not documented
)
{
    ipr_config_t *
        table;                          //  Not documented

table = self->table;

    return (table);
}
/*  -------------------------------------------------------------------------
    demo_server_config_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_config_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    demo_server_config_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_config_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
demo_server_config_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <demo_server_config file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    demo_server_config_destroy

    Type: Component method
    Destroys a demo_server_config_t object. Takes the address of a
    demo_server_config_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    -------------------------------------------------------------------------
 */

void
    demo_server_config_destroy_ (
    demo_server_config_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_server_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    demo_server_config_annihilate (self_p);
    demo_server_config_free ((demo_server_config_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_server_config_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_server_config_t *
    demo_server_config_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_server_config_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_server_config_cache_initialise ();

self = (demo_server_config_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_server_config_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_config_free

    Type: Component method
    Freess a demo_server_config_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_server_config_free (
    demo_server_config_t * self         //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (demo_server_config_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_server_config_t));
        self->object_tag = DEMO_SERVER_CONFIG_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_server_config_read_lock

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_read_lock (
    demo_server_config_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_write_lock

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_write_lock (
    demo_server_config_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_unlock

    Type: Component method
    Accepts a demo_server_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_config_unlock (
    demo_server_config_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_config_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_server_config_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_server_config_t));
icl_system_register (demo_server_config_cache_purge, demo_server_config_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_server_config_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_config_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_server_config_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_config_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_server_config_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_server_config_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_server_config_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_server_config_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_config_new_in_scope_ (
    demo_server_config_t * * self_p,    //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_server_config_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_server_config_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_server_config_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_server_config_component  = "demo_server_config ";
char *EMBED__demo_server_config_version   = "1.1 ";
char *EMBED__demo_server_config_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_server_config_filename  = "demo_server_config.icl ";
char *EMBED__demo_server_config_builddate  = "2011/03/01 ";
char *EMBED__demo_server_config_version_end  = "VeRsIoNeNd:ipc";

