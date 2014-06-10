/*---------------------------------------------------------------------------
    example.c - example component

    Generated from example.icl by icl_gen using GSL/4.
    
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
#include "example.h"                    //  Definitions for our class

//  Shorthand for class type

#define self_t              example_t

//  Structure of the example object

struct _example_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
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
    char * filename;                    //  
    ipr_config_t * table;               //  
};

//  Shorthands for class methods

#define self_new            example_new
#define self_annihilate     example_annihilate
#define self_load_xmlfile   example_load_xmlfile
#define self_load_bucket    example_load_bucket
#define self_shadow         example_shadow
#define self_commit         example_commit
#define self_rollback       example_rollback
#define self_cmdline_help   example_cmdline_help
#define self_cmdline_parse  example_cmdline_parse
#define self_options_help   example_options_help
#define self_set_port       example_set_port
#define self_port           example_port
#define self_set_monitor    example_set_monitor
#define self_monitor        example_monitor
#define self_set_max_memory_mb  example_set_max_memory_mb
#define self_max_memory_mb  example_max_memory_mb
#define self_set_record_stats  example_set_record_stats
#define self_record_stats   example_record_stats
#define self_set_packio     example_set_packio
#define self_packio         example_packio
#define self_set_heartbeat  example_set_heartbeat
#define self_heartbeat      example_heartbeat
#define self_set_read_timeout  example_set_read_timeout
#define self_read_timeout   example_read_timeout
#define self_set_write_timeout  example_set_write_timeout
#define self_write_timeout  example_write_timeout
#define self_set_tcp_nodelay  example_set_tcp_nodelay
#define self_tcp_nodelay    example_tcp_nodelay
#define self_set_tcp_rcvbuf  example_set_tcp_rcvbuf
#define self_tcp_rcvbuf     example_tcp_rcvbuf
#define self_set_tcp_sndbuf  example_set_tcp_sndbuf
#define self_tcp_sndbuf     example_tcp_sndbuf
#define self_set_frame_max  example_set_frame_max
#define self_frame_max      example_frame_max
#define self_set_log_path   example_set_log_path
#define self_log_path       example_log_path
#define self_set_keep_logs  example_set_keep_logs
#define self_keep_logs      example_keep_logs
#define self_set_archive_path  example_set_archive_path
#define self_archive_path   example_archive_path
#define self_set_archive_cmd  example_set_archive_cmd
#define self_archive_cmd    example_archive_cmd
#define self_set_alert_log  example_set_alert_log
#define self_alert_log      example_alert_log
#define self_set_daily_log  example_set_daily_log
#define self_daily_log      example_daily_log
#define self_set_debug_log  example_set_debug_log
#define self_debug_log      example_debug_log
#define self_set_trace      example_set_trace
#define self_trace          example_trace
#define self_dump_log       example_dump_log
#define self_selftest       example_selftest
#define self_filename       example_filename
#define self_set_filename   example_set_filename
#define self_table          example_table
#define self_terminate      example_terminate
#define self_show           example_show
#define self_destroy        example_destroy
#define self_alloc          example_alloc
#define self_free           example_free
#define self_read_lock      example_read_lock
#define self_write_lock     example_write_lock
#define self_unlock         example_unlock
#define self_cache_initialise  example_cache_initialise
#define self_cache_purge    example_cache_purge
#define self_cache_terminate  example_cache_terminate
#define self_show_animation  example_show_animation
#define self_new_in_scope   example_new_in_scope

static void
    example_annihilate (
example_t * ( * self_p )                //  Reference to object reference
);

#define example_alloc()                 example_alloc_ (__FILE__, __LINE__)
static example_t *
    example_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    example_free (
example_t * self                        //  Object reference
);

static void
    example_cache_initialise (
void);

static void
    example_cache_purge (
void);

static void
    example_cache_terminate (
void);

Bool
    example_animating = TRUE;           //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void s_validate_items (example_t *self);
static void s_get_properties (example_t *self);
example_t
    *example_config = NULL;             //  Global properties
static void s_validate_items (example_t *self)
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

static void s_get_properties (example_t *self)
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
/*  -------------------------------------------------------------------------
    example_new

    Type: Component method
    Creates and initialises a new example_t object, returns a
    reference to the created object.
    Creates an empty configuration set.  To fill this, use the load and
    load_bucket methods.
    -------------------------------------------------------------------------
 */

example_t *
    example_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    example_t *
        self = NULL;                    //  Object reference

    self = example_alloc_ (file, line);
    if (self) {
        self->object_tag   = EXAMPLE_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, example_show_);
#endif

//
self->table = ipr_config_new (NULL, FALSE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    example_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    example_annihilate (
    example_t * ( * self_p )            //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    example_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    EXAMPLE_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

ipr_config_destroy (&self->table);
icl_mem_free (self->filename);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    example_load_xmlfile

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config file into the property set, merging the new loaded data
    with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    example_load_xmlfile (
    example_t * self,                   //  Reference to object
    char * filename,                    //  Config file to load
    Bool required                       //  Config file is required?
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

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
    example_load_bucket

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config resource (as a bucket) into the property set, merging the
    new loaded data with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    example_load_bucket (
    example_t * self,                   //  Reference to object
    ipr_bucket_t * bucket               //  Config data to load
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

//
assert (bucket);
ipr_config_load_bucket (self->table, bucket);
s_validate_items (self);
s_get_properties (self);


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_shadow

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Loads the data from a different property set, and updates all the properties.
    Use this when you want to provide your code with multiple property classes
    that actually contain an identical configuration set.  To use:
    my_config = my_config_new (NULL, NULL, FALSE);
    my_config_shadow (self, your_config->config);
    -------------------------------------------------------------------------
 */

int
    example_shadow (
    example_t * self,                   //  Reference to object
    ipr_config_t * source               //  Config data to shadow
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

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
    example_commit

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the current configuration values to the specified file.  If the
    provided filename is null, uses the original config filename. If both
    filenames are null, does nothing.
    -------------------------------------------------------------------------
 */

int
    example_commit (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_rollback

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Reloads the saved configuration from the saved file, if any.
    Any changes made to the current configuration are lost.
    -------------------------------------------------------------------------
 */

int
    example_rollback (
    example_t * self                    //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_cmdline_help

    Type: Component method
    Prints command-line help for setting options from the command
    line.
    -------------------------------------------------------------------------
 */

void
    example_cmdline_help (
void)
{

printf ("Options that override config values; these take an argument:\n");
printf ("  --port           Server port for client connections\n");
printf ("  --monitor        Monitor interval, seconds\n");
printf ("  --max_memory_mb  Maximum allowed memory, MBytes\n");
printf ("  --record_stats   Record performance statistics\n");
printf ("  --packio         Pack network data before writing\n");
printf ("  --heartbeat      Heartbeat from client to server, seconds\n");
printf ("  --read_timeout   Timeout on network reads, seconds\n");
printf ("  --write_timeout  Timeout on network writes, seconds\n");
printf ("  --tcp_nodelay    TCP/IP NDELAY option\n");
printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\n");
printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\n");
printf ("  --frame_max      Maximum size of AMQP content frames\n");
printf ("  --log_path       Active log file directory\n");
printf ("  --keep_logs      Keep log files\n");
printf ("  --archive_path   Archive log file directory\n");
printf ("  --archive_cmd    Archive command\n");
printf ("  --alert_log      Alert log file name\n");
printf ("  --daily_log      Daily log file name\n");
printf ("  --debug_log      Debug log file name\n");
printf ("  --trace          Protocol trace level\n");
}
/*  -------------------------------------------------------------------------
    example_cmdline_parse

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Parses recognized command-line options starting with --.  Prints an error
    message and exits if the syntax is invalid.
    -------------------------------------------------------------------------
 */

int
    example_cmdline_parse (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_options_help

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Prints full help for all configuration options.
    -------------------------------------------------------------------------
 */

int
    example_options_help (
    example_t * self,                   //  Reference to object
    char * name                         //  Name of application
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

printf ("These are the configured options for %s\n\n", name);

printf ("/config/server/port - Server port for client connections\n");
printf ("    From command-line: --port newvalue\n");
printf ("    Specifies the port on which the server should open its connections.\n");
printf ("    Current value is '%s'. Default value is '7654'\n", self->port);
printf ("\n");

printf ("/config/resources/monitor - Monitor interval, seconds\n");
printf ("    From command-line: --monitor newvalue\n");
printf ("    Specifies the interval in seconds at which the server will report its   \n");
printf ("    message rates. If zero, no monitoring is shown. The recommended value is\n");
printf ("    1, if monitoring is desired.                                            \n");
printf ("    Current value is %i. Default value is 1.\n", self->monitor);
printf ("\n");

printf ("/config/resources/max_memory_mb - Maximum allowed memory, MBytes\n");
printf ("    From command-line: --max_memory_mb newvalue\n");
printf ("    Specifies the maximum memory allowed for the server, in MB. When the   \n");
printf ("    server reaches this limit, it will attempt to free resources and refuse\n");
printf ("    new connections until memory has been made available. If zero, no limit\n");
printf ("    is used.                                                               \n");
printf ("    Current value is %zu.\n", self->max_memory_mb);
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

printf ("/config/tuning/packio - Pack network data before writing\n");
printf ("    From command-line: --packio newvalue\n");
printf ("    If set, data is packed into larger buffers before writing. This can     \n");
printf ("    improve performance significantly on some systems, especially Windows   \n");
printf ("    and Linux with 2.4 kernels. If you disable this setting, you should test\n");
printf ("    the performance using a bulk message transfer test.                     \n");
printf ("    Current value is %i. Default value is 1.\n", self->packio);
printf ("\n");

printf ("/config/tuning/heartbeat - Heartbeat from client to server, seconds\n");
printf ("    From command-line: --heartbeat newvalue\n");
printf ("    Defines the timeout for connection heartbeating.\n");
printf ("    Current value is %i. Default value is 2.\n", self->heartbeat);
printf ("\n");

printf ("/config/tuning/read_timeout - Timeout on network reads, seconds\n");
printf ("    From command-line: --read_timeout newvalue\n");
printf ("    Defines the timeout for network reads. If a peer does not respond within\n");
printf ("    this time, it is considered dead. If you get unexpected timeout errors  \n");
printf ("    in the logs, you may want to increase this value.                       \n");
printf ("    Current value is %i. Default value is 30.\n", self->read_timeout);
printf ("\n");

printf ("/config/tuning/write_timeout - Timeout on network writes, seconds\n");
printf ("    From command-line: --write_timeout newvalue\n");
printf ("    Defines the timeout for network writes. If a peer does not accept data\n");
printf ("    within this time, it is considered dead. If you get unexpected timeout\n");
printf ("    errors in the logs, you may want to increase this value.              \n");
printf ("    Current value is %i. Default value is 30.\n", self->write_timeout);
printf ("\n");

printf ("/config/tuning/tcp_nodelay - TCP/IP NDELAY option\n");
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

printf ("/config/logging/log_path - Active log file directory\n");
printf ("    From command-line: --log_path newvalue\n");
printf ("    Specifies the directory path into which active log files are placed.\n");
printf ("    Current value is '%s'. Default value is 'logs'\n", self->log_path);
printf ("\n");

printf ("/config/logging/keep_logs - Keep log files\n");
printf ("    From command-line: --keep_logs newvalue\n");
printf ("    Specifies whether or not the server should keep logs. If zero, the      \n");
printf ("    server will not keep logs. If 1, it will keep an alert log, a daily log,\n");
printf ("    and a debug log as specified.                                           \n");
printf ("    Current value is %i. Default value is 1.\n", self->keep_logs);
printf ("\n");

printf ("/config/logging/archive_path - Archive log file directory\n");
printf ("    From command-line: --archive_path newvalue\n");
printf ("    Specifies the directory path into which archived log files are placed. \n");
printf ("    This can be the same as the active path; archived log files are renamed\n");
printf ("    to avoid conflict with active log files.                               \n");
printf ("    Current value is '%s'. Default value is 'archive'\n", self->archive_path);
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


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_set_port

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_port (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_port

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_port (
    example_t * self                    //  Reference to self
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
    example_set_monitor

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_monitor (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_monitor

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_monitor (
    example_t * self                    //  Reference to self
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
    example_set_max_memory_mb

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_max_memory_mb (
    example_t * self,                   //  Reference to object
    size_t max_memory_mb                //  Not documented
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

EXAMPLE_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%zu", max_memory_mb);
ipr_config_putp (self->table, "/config/resources", "max_memory_mb", strvalue);
self->max_memory_mb = max_memory_mb;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_max_memory_mb

    Type: Component method
    -------------------------------------------------------------------------
 */

size_t
    example_max_memory_mb (
    example_t * self                    //  Reference to self
)
{
    size_t
        max_memory_mb;                  //  Not documented

//
if (self)
    max_memory_mb = self->max_memory_mb;
else
    max_memory_mb = 0;

    return (max_memory_mb);
}
/*  -------------------------------------------------------------------------
    example_set_record_stats

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_record_stats (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_record_stats

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    example_record_stats (
    example_t * self                    //  Reference to self
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
    example_set_packio

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_packio (
    example_t * self,                   //  Reference to object
    int packio                          //  Not documented
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

EXAMPLE_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", packio);
ipr_config_putp (self->table, "/config/tuning", "packio", strvalue);
self->packio = packio;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_packio

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_packio (
    example_t * self                    //  Reference to self
)
{
    int
        packio;                         //  Not documented

//
if (self)
    packio = self->packio;
else
    packio = 1;

    return (packio);
}
/*  -------------------------------------------------------------------------
    example_set_heartbeat

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_heartbeat (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_heartbeat

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_heartbeat (
    example_t * self                    //  Reference to self
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
    example_set_read_timeout

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_read_timeout (
    example_t * self,                   //  Reference to object
    int read_timeout                    //  Not documented
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

EXAMPLE_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", read_timeout);
ipr_config_putp (self->table, "/config/tuning", "read_timeout", strvalue);
self->read_timeout = read_timeout;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_read_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_read_timeout (
    example_t * self                    //  Reference to self
)
{
    int
        read_timeout;                   //  Not documented

//
if (self)
    read_timeout = self->read_timeout;
else
    read_timeout = 30;

    return (read_timeout);
}
/*  -------------------------------------------------------------------------
    example_set_write_timeout

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_write_timeout (
    example_t * self,                   //  Reference to object
    int write_timeout                   //  Not documented
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

EXAMPLE_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", write_timeout);
ipr_config_putp (self->table, "/config/tuning", "write_timeout", strvalue);
self->write_timeout = write_timeout;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_write_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_write_timeout (
    example_t * self                    //  Reference to self
)
{
    int
        write_timeout;                  //  Not documented

//
if (self)
    write_timeout = self->write_timeout;
else
    write_timeout = 30;

    return (write_timeout);
}
/*  -------------------------------------------------------------------------
    example_set_tcp_nodelay

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_tcp_nodelay (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_tcp_nodelay

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    example_tcp_nodelay (
    example_t * self                    //  Reference to self
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
    example_set_tcp_rcvbuf

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_tcp_rcvbuf (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_tcp_rcvbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_tcp_rcvbuf (
    example_t * self                    //  Reference to self
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
    example_set_tcp_sndbuf

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_tcp_sndbuf (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_tcp_sndbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_tcp_sndbuf (
    example_t * self                    //  Reference to self
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
    example_set_frame_max

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_frame_max (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

//
if (frame_max < 2048) {
    icl_console_print ("W: <command line>: illegal value %d for frame_max, using %d",
        frame_max, 2048);
    frame_max = 2048;
}
if (frame_max > 4194304) {
    icl_console_print ("W: <command line>: illegal value %d for frame_max, using %d",
        frame_max, 4194304);
    frame_max = 4194304;
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
    example_frame_max

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_frame_max (
    example_t * self                    //  Reference to self
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
    example_set_log_path

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_log_path (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_log_path

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_log_path (
    example_t * self                    //  Reference to self
)
{
    char *
        log_path;                       //  Not documented

//
if (self)
    log_path = self->log_path;
else
    log_path = "logs";

    return (log_path);
}
/*  -------------------------------------------------------------------------
    example_set_keep_logs

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_keep_logs (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_keep_logs

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    example_keep_logs (
    example_t * self                    //  Reference to self
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
    example_set_archive_path

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_archive_path (
    example_t * self,                   //  Reference to object
    char * archive_path                 //  Not documented
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

EXAMPLE_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "archive_path", archive_path);
archive_path = ipr_config_getp (self->table, "/config/logging", "archive_path", NULL);
assert (archive_path);
self->archive_path = archive_path;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_archive_path

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_archive_path (
    example_t * self                    //  Reference to self
)
{
    char *
        archive_path;                   //  Not documented

//
if (self)
    archive_path = self->archive_path;
else
    archive_path = "archive";

    return (archive_path);
}
/*  -------------------------------------------------------------------------
    example_set_archive_cmd

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_archive_cmd (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_archive_cmd

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_archive_cmd (
    example_t * self                    //  Reference to self
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
    example_set_alert_log

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_alert_log (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_alert_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_alert_log (
    example_t * self                    //  Reference to self
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
    example_set_daily_log

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_daily_log (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_daily_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_daily_log (
    example_t * self                    //  Reference to self
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
    example_set_debug_log

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_debug_log (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_debug_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_debug_log (
    example_t * self                    //  Reference to self
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
    example_set_trace

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_set_trace (
    example_t * self,                   //  Reference to object
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

EXAMPLE_ASSERT_SANE (self);

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
    example_trace

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    example_trace (
    example_t * self                    //  Reference to self
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
    example_dump_log

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    Dump current option values to an smt_log file.
    -------------------------------------------------------------------------
 */

int
    example_dump_log (
    example_t * self,                   //  Reference to object
    smt_log_t * logfile                 //  Log file to dump to
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    example_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    example_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    example_filename (
    example_t * self                    //  Not documented
)
{
    char *
        filename;                       //  Not documented

filename = self->filename;

    return (filename);
}
/*  -------------------------------------------------------------------------
    example_set_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    example_set_filename (
    example_t * self,                   //  Not documented
    char * filename                     //  Not documented
)
{

icl_mem_free (self->filename);
self->filename = icl_mem_strdup (filename);
}
/*  -------------------------------------------------------------------------
    example_table

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_config_t *
    example_table (
    example_t * self                    //  Not documented
)
{
    ipr_config_t *
        table;                          //  Not documented

table = self->table;

    return (table);
}
/*  -------------------------------------------------------------------------
    example_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    example_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    example_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    example_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
example_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <example file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    example_destroy

    Type: Component method
    Destroys a example_t object. Takes the address of a
    example_t reference (a pointer to a pointer) and nullifies the
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
    example_destroy_ (
    example_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    example_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    example_annihilate (self_p);
    example_free ((example_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    example_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static example_t *
    example_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    example_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    example_cache_initialise ();

self = (example_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (example_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    example_free

    Type: Component method
    Freess a example_t object.
    -------------------------------------------------------------------------
 */

static void
    example_free (
    example_t * self                    //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (example_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (example_t));
        self->object_tag = EXAMPLE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    example_read_lock

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_read_lock (
    example_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_write_lock

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_write_lock (
    example_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    example_unlock

    Type: Component method
    Accepts a example_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    example_unlock (
    example_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

EXAMPLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    example_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    example_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (example_t));
icl_system_register (example_cache_purge, example_cache_terminate);
}
/*  -------------------------------------------------------------------------
    example_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    example_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    example_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    example_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    example_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    example_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

example_animating = enabled;
}
/*  -------------------------------------------------------------------------
    example_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    example_new_in_scope_ (
    example_t * * self_p,               //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = example_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) example_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__example_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__example_component            = "example ";
char *EMBED__example_version              = "1.1 ";
char *EMBED__example_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__example_filename             = "example.icl ";
char *EMBED__example_builddate            = "2011/03/01 ";
char *EMBED__example_version_end          = "VeRsIoNeNd:ipc";

