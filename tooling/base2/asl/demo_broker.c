/*---------------------------------------------------------------------------
    demo_broker.c - demo_broker component

    Generated from demo_broker.icl by smt_object_gen using GSL/4.
    
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
#include "demo_broker.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_broker_t

//  Shorthands for class methods

#define self_new            demo_broker_new
#define self_annihilate     demo_broker_annihilate
#define self_free           demo_broker_free
#define self_start          demo_broker_start
#define self_shutdown       demo_broker_shutdown
#define self_selftest       demo_broker_selftest
#define self_destroy        demo_broker_destroy
#define self_unlink         demo_broker_unlink
#define self_show_animation  demo_broker_show_animation
#define self_initialise     demo_broker_initialise
#define self_terminate      demo_broker_terminate
#define self_show           demo_broker_show
#define self_alloc          demo_broker_alloc
#define self_link           demo_broker_link
#define self_cache_initialise  demo_broker_cache_initialise
#define self_cache_purge    demo_broker_cache_purge
#define self_cache_terminate  demo_broker_cache_terminate
#define self_new_in_scope   demo_broker_new_in_scope

#define demo_broker_annihilate(self)    demo_broker_annihilate_ (self, __FILE__, __LINE__)
static int
    demo_broker_annihilate_ (
demo_broker_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    demo_broker_initialise (
void);

#define demo_broker_alloc()             demo_broker_alloc_ (__FILE__, __LINE__)
static demo_broker_t *
    demo_broker_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_broker_cache_initialise (
void);

static void
    demo_broker_cache_purge (
void);

static void
    demo_broker_cache_terminate (
void);

Bool
    demo_broker_animating = TRUE;       //  Animation enabled by default
static Bool
    s_demo_broker_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_broker_mutex      = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static void
    s_format_logfile (char *logfile, char *configured);
demo_broker_t
    *demo_broker = NULL;
/*  -------------------------------------------------------------------------
    demo_broker_new

    Type: Component method
    Creates and initialises a new demo_broker_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

demo_broker_t *
    demo_broker_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
char
    *log_path,                      //  Log directory
    *archive_cmd;                   //  Archive command
icl_shortstr_t
    log_name,                       //  Full logfile name
    stats_file;                     //  Statistics file name
smt_thread_t
    *thread_p;                      //  Thread from demo_server_server_agent
    demo_broker_t *
        self = NULL;                    //  Object reference

if (!s_demo_broker_active)
    self_initialise ();
    self = demo_broker_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_BROKER_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_broker_show_);
#endif

self->thread = demo_broker_agent_class_thread_new (self);
self->thread->animate = TRUE;

//
//  Set self so that we're immediately addressable in other threads
demo_broker = self;

self->started         = smt_time_now ();
self->connections     = demo_server_connection_table_new ();
self->connection_list = demo_server_connection_list_new ();
self->monitor_timer   = demo_server_config_monitor (demo_server_config);
self->imeter          = ipr_meter_new ();
self->ometer          = ipr_meter_new ();
ipr_str_random (self->identifier, "AAAAAAAA-AAAAAAAA-AAAAAAAA");

//  Set up log files and log forwarding:
log_path     = demo_server_config_log_path     (demo_server_config);
archive_cmd  = demo_server_config_archive_cmd  (demo_server_config);

self->debug_log = smt_log_new ();   //  Debug log for low-level trace
self->daily_log = smt_log_new ();   //  Daily log for normal activity
self->alert_log = smt_log_new ();   //  Alert log for exceptions

if (demo_server_config_syslog (demo_server_config)) {
    if (ipr_log_sysopen ("demo_server"))
        icl_console_print (
            "W: syslog not available, using direct-to-file logging");
    smt_log_set_syslog (self->debug_log, IPR_LOG_SYSDEBUG);
    smt_log_set_syslog (self->daily_log, IPR_LOG_SYSINFO);
    smt_log_set_syslog (self->alert_log, IPR_LOG_SYSWARNING);
}
else
if (demo_server_config_keep_logs (demo_server_config)) {
    s_format_logfile (log_name, demo_server_config_debug_log (demo_server_config));
    smt_log_open (self->debug_log, log_name, log_path, archive_cmd);
    s_format_logfile (log_name, demo_server_config_daily_log (demo_server_config));
    smt_log_open (self->daily_log, log_name, log_path, archive_cmd);
    s_format_logfile (log_name, demo_server_config_alert_log (demo_server_config));
    smt_log_open (self->alert_log, log_name, log_path, archive_cmd);
}
//  Dump environment and configuration to debug log, silently
smt_log_dump_env (self->debug_log);
demo_server_config_dump_log (demo_server_config, self->debug_log);

//  Set up log file associations and configuration
smt_log_set_stderr (self->debug_log, TRUE);
smt_log_set_copyto (self->daily_log, self->debug_log);
smt_log_set_stderr (self->alert_log, TRUE);
smt_log_set_copyto (self->alert_log, self->daily_log);

if (demo_server_config_record_stats (demo_server_config)) {
    icl_shortstr_fmt (stats_file, "demo_server_stats_%s.del",
        demo_server_config_port (demo_server_config));
    self->stats = fopen (stats_file, "w");
    //  We use a tab-delimited form that pastes easily into spreadsheets
    fprintf (self->stats, "Clients\tMsgMemK\tCurIn\tCurOut\tAvgIn\tAvgOut\n");
}
//  Start server agent and own async thread
if (demo_server_agent_init (&thread_p) == SMT_OK) {
    self->agent_thread = smt_thread_link (thread_p);
    self_start (self);
}
}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_broker_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    demo_broker_annihilate_ (
    demo_broker_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    int
        history_last;
#endif

    demo_broker_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_BROKER_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_BROKER_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_BROKER_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_BROKER_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (demo_broker_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to demo_broker agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_broker_free

    Type: Component method
    Freess a demo_broker_t object.
    -------------------------------------------------------------------------
 */

void
    demo_broker_free_ (
    demo_broker_t * self,               //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_BROKER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_BROKER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_BROKER_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_BROKER_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);

smt_log_destroy (&self->alert_log);
smt_log_destroy (&self->daily_log);
smt_log_destroy (&self->debug_log);
        memset (&self->object_tag, 0, sizeof (demo_broker_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_broker_t));
        self->object_tag = DEMO_BROKER_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_broker_start

    Type: Component method
    Accepts a demo_broker_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    -------------------------------------------------------------------------
 */

int
    demo_broker_start (
    demo_broker_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_BROKER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_broker_agent_start (self->thread)) {
        //icl_console_print ("Error sending 'start' method to demo_broker agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_broker_shutdown

    Type: Component method
    Accepts a demo_broker_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    -------------------------------------------------------------------------
 */

int
    demo_broker_shutdown (
    demo_broker_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_BROKER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_broker_agent_shutdown (self->thread)) {
        //icl_console_print ("Error sending 'shutdown' method to demo_broker agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_broker_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_broker_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_broker_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_broker_destroy_ (
    demo_broker_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_broker_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        rc = demo_broker_annihilate_ (self_p, file, line);
    else
    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        demo_broker_free (self);
    *self_p = NULL;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_broker_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_broker_unlink_ (
    demo_broker_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    int
        history_last;
#endif
    demo_broker_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on demo_broker object");
            demo_broker_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_BROKER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_BROKER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_BROKER_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_BROKER_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                demo_broker_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                demo_broker_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    demo_broker_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_broker_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_broker_animating = enabled;

demo_broker_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    demo_broker_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_broker_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_broker_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_broker_mutex)
            s_demo_broker_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_broker_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_broker_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

demo_broker_agent_init ();
            s_demo_broker_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_broker_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_broker_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_broker_terminate (
void)
{

if (s_demo_broker_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_broker_mutex);
#endif
        s_demo_broker_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_broker_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_broker_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_broker_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_broker zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    if (self->history_last > DEMO_BROKER_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_BROKER_HISTORY_LENGTH;
        self->history_last %= DEMO_BROKER_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_BROKER_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_broker>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_broker_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_broker_t *
    demo_broker_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_broker_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_broker_cache_initialise ();

self = (demo_broker_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_broker_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_broker_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_broker_t *
    demo_broker_link_ (
    demo_broker_t * self,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    int
        history_last;
#endif

    if (self) {
        DEMO_BROKER_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_BROKER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_BROKER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_BROKER_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_BROKER_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_broker_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_broker_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_broker_t));
icl_system_register (demo_broker_cache_purge, demo_broker_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_broker_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_broker_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_broker_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_broker_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_broker_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_broker_new_in_scope_ (
    demo_broker_t * * self_p,           //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_broker_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_broker_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
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
        icl_shortstr_cat  (log_name, demo_server_config_port (demo_server_config));
        icl_shortstr_cat  (log_name, point);
    }
    else {
        //  Add port number at end
        icl_shortstr_cpy  (log_name, configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, demo_server_config_port (demo_server_config));
        icl_shortstr_cat  (log_name, ".log");
    }
}

//  Embed the version information in the resulting binary

char *EMBED__demo_broker_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_broker_component        = "demo_broker ";
char *EMBED__demo_broker_version          = "1.0 ";
char *EMBED__demo_broker_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_broker_filename         = "demo_broker.icl ";
char *EMBED__demo_broker_builddate        = "2011/03/01 ";
char *EMBED__demo_broker_version_end      = "VeRsIoNeNd:ipc";

