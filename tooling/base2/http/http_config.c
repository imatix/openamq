/*---------------------------------------------------------------------------
    http_config.c - http_config component

    Generated from http_config.icl by icl_gen using GSL/4.
    
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
#include "http_config.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              http_config_t

//  Structure of the http_config object

struct _http_config_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
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
    char * filename;                    //  
    ipr_config_t * table;               //  
};

//  Shorthands for class methods

#define self_new            http_config_new
#define self_annihilate     http_config_annihilate
#define self_load_xmlfile   http_config_load_xmlfile
#define self_load_bucket    http_config_load_bucket
#define self_shadow         http_config_shadow
#define self_commit         http_config_commit
#define self_rollback       http_config_rollback
#define self_cmdline_help   http_config_cmdline_help
#define self_cmdline_parse  http_config_cmdline_parse
#define self_options_help   http_config_options_help
#define self_set_server_name  http_config_set_server_name
#define self_server_name    http_config_server_name
#define self_set_port       http_config_set_port
#define self_port           http_config_port
#define self_set_public_port  http_config_set_public_port
#define self_public_port    http_config_public_port
#define self_set_trace      http_config_set_trace
#define self_trace          http_config_trace
#define self_set_animate    http_config_set_animate
#define self_animate        http_config_animate
#define self_set_verbose    http_config_set_verbose
#define self_verbose        http_config_verbose
#define self_set_listen     http_config_set_listen
#define self_listen         http_config_listen
#define self_set_webroot    http_config_set_webroot
#define self_webroot        http_config_webroot
#define self_set_defaults   http_config_set_defaults
#define self_defaults       http_config_defaults
#define self_set_default_types  http_config_set_default_types
#define self_default_types  http_config_default_types
#define self_set_monitor    http_config_set_monitor
#define self_monitor        http_config_monitor
#define self_set_record_stats  http_config_set_record_stats
#define self_record_stats   http_config_record_stats
#define self_set_log_path   http_config_set_log_path
#define self_log_path       http_config_log_path
#define self_set_keep_logs  http_config_set_keep_logs
#define self_keep_logs      http_config_keep_logs
#define self_set_archive_cmd  http_config_set_archive_cmd
#define self_archive_cmd    http_config_archive_cmd
#define self_set_alert_log  http_config_set_alert_log
#define self_alert_log      http_config_alert_log
#define self_set_access_log  http_config_set_access_log
#define self_access_log     http_config_access_log
#define self_set_access_log_format  http_config_set_access_log_format
#define self_access_log_format  http_config_access_log_format
#define self_set_debug_log  http_config_set_debug_log
#define self_debug_log      http_config_debug_log
#define self_set_syslog     http_config_set_syslog
#define self_syslog         http_config_syslog
#define self_set_user       http_config_set_user
#define self_user           http_config_user
#define self_set_group      http_config_set_group
#define self_group          http_config_group
#define self_set_blacklist  http_config_set_blacklist
#define self_blacklist      http_config_blacklist
#define self_set_basic_auth  http_config_set_basic_auth
#define self_basic_auth     http_config_basic_auth
#define self_set_digest_auth  http_config_set_digest_auth
#define self_digest_auth    http_config_digest_auth
#define self_set_password_ttl  http_config_set_password_ttl
#define self_password_ttl   http_config_password_ttl
#define self_set_hit_quota  http_config_set_hit_quota
#define self_hit_quota      http_config_hit_quota
#define self_set_miss_quota  http_config_set_miss_quota
#define self_miss_quota     http_config_miss_quota
#define self_set_password_guard  http_config_set_password_guard
#define self_password_guard  http_config_password_guard
#define self_set_policy_trace  http_config_set_policy_trace
#define self_policy_trace   http_config_policy_trace
#define self_set_nervosity  http_config_set_nervosity
#define self_nervosity      http_config_nervosity
#define self_set_nonce_ttl  http_config_set_nonce_ttl
#define self_nonce_ttl      http_config_nonce_ttl
#define self_set_nonce_quota  http_config_set_nonce_quota
#define self_nonce_quota    http_config_nonce_quota
#define self_set_refresh_by  http_config_set_refresh_by
#define self_refresh_by     http_config_refresh_by
#define self_set_refresh_log  http_config_set_refresh_log
#define self_refresh_log    http_config_refresh_log
#define self_set_address_quota  http_config_set_address_quota
#define self_address_quota  http_config_address_quota
#define self_set_abort_delay  http_config_set_abort_delay
#define self_abort_delay    http_config_abort_delay
#define self_set_coremark_cycles  http_config_set_coremark_cycles
#define self_coremark_cycles  http_config_coremark_cycles
#define self_set_header_max  http_config_set_header_max
#define self_header_max     http_config_header_max
#define self_set_bucket_max  http_config_set_bucket_max
#define self_bucket_max     http_config_bucket_max
#define self_set_read_timeout  http_config_set_read_timeout
#define self_read_timeout   http_config_read_timeout
#define self_set_write_timeout  http_config_set_write_timeout
#define self_write_timeout  http_config_write_timeout
#define self_set_accept_retry_timeout  http_config_set_accept_retry_timeout
#define self_accept_retry_timeout  http_config_accept_retry_timeout
#define self_set_tcp_nodelay  http_config_set_tcp_nodelay
#define self_tcp_nodelay    http_config_tcp_nodelay
#define self_set_tcp_rcvbuf  http_config_set_tcp_rcvbuf
#define self_tcp_rcvbuf     http_config_tcp_rcvbuf
#define self_set_tcp_sndbuf  http_config_set_tcp_sndbuf
#define self_tcp_sndbuf     http_config_tcp_sndbuf
#define self_set_polling_threads  http_config_set_polling_threads
#define self_polling_threads  http_config_polling_threads
#define self_set_working_threads  http_config_set_working_threads
#define self_working_threads  http_config_working_threads
#define self_set_etag       http_config_set_etag
#define self_etag           http_config_etag
#define self_dump_log       http_config_dump_log
#define self_selftest       http_config_selftest
#define self_filename       http_config_filename
#define self_set_filename   http_config_set_filename
#define self_table          http_config_table
#define self_terminate      http_config_terminate
#define self_show           http_config_show
#define self_destroy        http_config_destroy
#define self_alloc          http_config_alloc
#define self_free           http_config_free
#define self_read_lock      http_config_read_lock
#define self_write_lock     http_config_write_lock
#define self_unlock         http_config_unlock
#define self_cache_initialise  http_config_cache_initialise
#define self_cache_purge    http_config_cache_purge
#define self_cache_terminate  http_config_cache_terminate
#define self_show_animation  http_config_show_animation
#define self_new_in_scope   http_config_new_in_scope

static void
    http_config_annihilate (
http_config_t * ( * self_p )            //  Reference to object reference
);

#define http_config_alloc()             http_config_alloc_ (__FILE__, __LINE__)
static http_config_t *
    http_config_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_config_free (
http_config_t * self                    //  Object reference
);

static void
    http_config_cache_initialise (
void);

static void
    http_config_cache_purge (
void);

static void
    http_config_cache_terminate (
void);

Bool
    http_config_animating = TRUE;       //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void s_validate_items (http_config_t *self);
static void s_get_properties (http_config_t *self);
http_config_t
    *http_config = NULL;            //  Server properties
static void s_validate_items (http_config_t *self)
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

static void s_get_properties (http_config_t *self)
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
/*  -------------------------------------------------------------------------
    http_config_new

    Type: Component method
    Creates and initialises a new http_config_t object, returns a
    reference to the created object.
    Creates an empty configuration set.  To fill this, use the load and
    load_bucket methods.
    -------------------------------------------------------------------------
 */

http_config_t *
    http_config_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    http_config_t *
        self = NULL;                    //  Object reference

    self = http_config_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_CONFIG_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_config_show_);
#endif

//
self->table = ipr_config_new (NULL, FALSE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_config_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_config_annihilate (
    http_config_t * ( * self_p )        //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    http_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    HTTP_CONFIG_ASSERT_SANE (self);
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
    http_config_load_xmlfile

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config file into the property set, merging the new loaded data
    with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    http_config_load_xmlfile (
    http_config_t * self,               //  Reference to object
    char * filename,                    //  Config file to load
    Bool required                       //  Config file is required?
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_load_bucket

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a config resource (as a bucket) into the property set, merging the
    new loaded data with any previous data, as defined in ipr_config.icl.
    -------------------------------------------------------------------------
 */

int
    http_config_load_bucket (
    http_config_t * self,               //  Reference to object
    ipr_bucket_t * bucket               //  Config data to load
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

//
assert (bucket);
ipr_config_load_bucket (self->table, bucket);
s_validate_items (self);
s_get_properties (self);


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_shadow

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads the data from a different property set, and updates all the properties.
    Use this when you want to provide your code with multiple property classes
    that actually contain an identical configuration set.  To use:
    my_config = my_config_new (NULL, NULL, FALSE);
    my_config_shadow (self, your_config->config);
    -------------------------------------------------------------------------
 */

int
    http_config_shadow (
    http_config_t * self,               //  Reference to object
    ipr_config_t * source               //  Config data to shadow
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_commit

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the current configuration values to the specified file.  If the
    provided filename is null, uses the original config filename. If both
    filenames are null, does nothing.
    -------------------------------------------------------------------------
 */

int
    http_config_commit (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_rollback

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Reloads the saved configuration from the saved file, if any.
    Any changes made to the current configuration are lost.
    -------------------------------------------------------------------------
 */

int
    http_config_rollback (
    http_config_t * self                //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_cmdline_help

    Type: Component method
    Prints command-line help for setting options from the command
    line.
    -------------------------------------------------------------------------
 */

void
    http_config_cmdline_help (
void)
{

printf ("Options that override config values; these take an argument:\n");
printf ("  --server_name    Announced server name\n");
printf ("  --port           Server port for HTTP requests\n");
printf ("  --public_port    Public port for HTTP requests\n");
printf ("  --trace          Trace network traffic?\n");
printf ("  --animate        Animate HTTP agent?\n");
printf ("  --verbose        Verbose output?\n");
printf ("  --listen         Address (local network interface) to listen on\n");
printf ("  --webroot        Path to web root directory\n");
printf ("  --defaults       Default file names\n");
printf ("  --default_types  Default document types\n");
printf ("  --monitor        Monitor interval, seconds\n");
printf ("  --record_stats   Record performance statistics\n");
printf ("  --log_path       Active log file directory\n");
printf ("  --keep_logs      Keep log files\n");
printf ("  --archive_cmd    Archive log file command\n");
printf ("  --alert_log      Error log file name\n");
printf ("  --access_log     Access log file name\n");
printf ("  --access_log_format  Access log file format\n");
printf ("  --debug_log      Debug log file name\n");
printf ("  --syslog         Use syslog logging protocol\n");
printf ("  --user           User to run server as\n");
printf ("  --group          Group to run server as\n");
printf ("  --blacklist      Blacklist filename\n");
printf ("  --basic_auth     Basic authentication filename\n");
printf ("  --digest_auth    Digest authentication filename\n");
printf ("  --password_ttl   Password time-to-live, in seconds\n");
printf ("  --hit_quota      Password hit quota in pages\n");
printf ("  --miss_quota     Password miss quota in requests\n");
printf ("  --password_guard  Password guard timer, in seconds.\n");
printf ("  --policy_trace   Debug policy execution?\n");
printf ("  --nervosity      Security nervosity, seconds\n");
printf ("  --nonce_ttl      Nonce time-to-live, seconds\n");
printf ("  --nonce_quota    Nonce quota in requests\n");
printf ("  --refresh_by     Shell command for password refresh\n");
printf ("  --refresh_log    Stdout for password refresh\n");
printf ("  --address_quota  Maximum open connections per address\n");
printf ("  --abort_delay    Pause time for hostile connections\n");
printf ("  --coremark_cycles  Duration of CoreMark test\n");
printf ("  --header_max     Max size of HTTP header\n");
printf ("  --bucket_max     Max size of content bucket\n");
printf ("  --read_timeout   Network read timeout, seconds\n");
printf ("  --write_timeout  Network write timeout, seconds\n");
printf ("  --accept_retry_timeout  Timeout before retrying accept calls, seconds\n");
printf ("  --tcp_nodelay    TCP/IP NODELAY option\n");
printf ("  --tcp_rcvbuf     TCP/IP receive buffer, in bytes\n");
printf ("  --tcp_sndbuf     TCP/IP send buffer, in bytes\n");
printf ("  --polling_threads  Number of polling OS threads\n");
printf ("  --working_threads  Number of working OS threads\n");
printf ("  --etag           ETag header for files\n");
}
/*  -------------------------------------------------------------------------
    http_config_cmdline_parse

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Parses recognized command-line options starting with --.  Prints an error
    message and exits if the syntax is invalid.
    -------------------------------------------------------------------------
 */

int
    http_config_cmdline_parse (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_options_help

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Prints full help for all configuration options.
    -------------------------------------------------------------------------
 */

int
    http_config_options_help (
    http_config_t * self,               //  Reference to object
    char * name                         //  Name of application
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

printf ("These are the configured options for %s\n\n", name);

printf ("/config/server/name - Announced server name\n");
printf ("    From command-line: --server_name newvalue\n");
printf ("    Specifies the Server: header field name for HTTP responses.\n");
printf ("    Current value is '%s'. Default value is 'X5'\n", self->server_name);
printf ("\n");

printf ("/config/server/port - Server port for HTTP requests\n");
printf ("    From command-line: --port newvalue\n");
printf ("    Specifies the port that the server opens for HTTP requests. The standard\n");
printf ("    port for HTTP is 80. If you want to run the server in parallel with     \n");
printf ("    other HTTP servers on the same server, choose a server port of 4096 or  \n");
printf ("    higher. If you are serving Internet clients, your network may map a     \n");
printf ("    public port to the server port number. In that case, you may need to    \n");
printf ("    specify the public port number separately.                              \n");
printf ("    Current value is '%s'. Default value is '80'\n", self->port);
printf ("\n");

printf ("/config/server/public_port - Public port for HTTP requests\n");
printf ("    From command-line: --public_port newvalue\n");
printf ("    Specifies the public port for client requests, if not the standard port\n");
printf ("    number for HTTP (80). The server needs to know this value in order to  \n");
printf ("    format accurate redirection responses.                                 \n");
printf ("    Current value is '%s'.\n", self->public_port);
printf ("\n");

printf ("/config/server/trace - Trace network traffic?\n");
printf ("    From command-line: --trace newvalue\n");
printf ("    If set, the HTTP agent displays all socket i/o, which can be useful if  \n");
printf ("    you need to verify what exactly web clients are sending and getting back\n");
printf ("    from the server. Do not enable this with high loads because you will get\n");
printf ("    enormous amounts of output.                                             \n");
printf ("    Current value is %i. Default value is 0.\n", self->trace);
printf ("\n");

printf ("/config/server/animate - Animate HTTP agent?\n");
printf ("    From command-line: --animate newvalue\n");
printf ("    If set, the HTTP agent displays its activity, which can be useful if you\n");
printf ("    need to verify how HTTP requests are being processed. Do not enable this\n");
printf ("    with high loads because you will get enormous amounts of output.        \n");
printf ("    Current value is %i. Default value is 0.\n", self->animate);
printf ("\n");

printf ("/config/server/verbose - Verbose output?\n");
printf ("    From command-line: --verbose newvalue\n");
printf ("    If set, the HTTP agent displays all input and output headers, which can\n");
printf ("    be useful for debugging configuration issues.                          \n");
printf ("    Current value is %i. Default value is 0.\n", self->verbose);
printf ("\n");

printf ("/config/server/listen - Address (local network interface) to listen on\n");
printf ("    From command-line: --listen newvalue\n");
printf ("    Specifies the network interface on which the server should listen for\n");
printf ("    connections. By default this is *, meaning all interfaces. You would \n");
printf ("    choose to set an address when you use this application on a box with \n");
printf ("    multiple network interfaces, e.g. routing between two networks.      \n");
printf ("    Current value is '%s'. Default value is '*'\n", self->listen);
printf ("\n");

printf ("/config/host/webroot - Path to web root directory\n");
printf ("    From command-line: --webroot newvalue\n");
printf ("    Specifies the web root directory. Normally this is used to hold static\n");
printf ("    resources - HTML, images, etc. The web root maps to the / URI.        \n");
printf ("    Current value is '%s'. Default value is 'webpages'\n", self->webroot);
printf ("\n");

printf ("/config/host/defaults - Default file names\n");
printf ("    From command-line: --defaults newvalue\n");
printf ("    Specifies a space-delimited list of file names to use as defaults on an \n");
printf ("    empty URI path (the URI refers to a directory name). If none of the     \n");
printf ("    defaults are present, Xitami will look for a file called .httpdir and if\n");
printf ("    that is present, it will format a directory listing.                    \n");
printf ("    Current value is '%s'. Default value is 'index.html default.html'\n", self->defaults);
printf ("\n");

printf ("/config/host/default_types - Default document types\n");
printf ("    From command-line: --default_types newvalue\n");
printf ("    Specifies a list of default document types. The server implements a    \n");
printf ("    feature called 'default typing' in which you can specify URIs without  \n");
printf ("    file extensions. The default types are specified in order. You can use \n");
printf ("    this to publish a URI that specifies an HTML document, for example, and\n");
printf ("    later replace it with a PDF without breaking the published URI.        \n");
printf ("    Current value is '%s'. Default value is '.pdf .html'\n", self->default_types);
printf ("\n");

printf ("/config/resources/monitor - Monitor interval, seconds\n");
printf ("    From command-line: --monitor newvalue\n");
printf ("    Specifies the interval in seconds at which the server will report its   \n");
printf ("    request/response rates. If zero, no monitoring is shown. The recommended\n");
printf ("    value is 1, if monitoring is desired.                                   \n");
printf ("    Current value is %i. Default value is 0.\n", self->monitor);
printf ("\n");

printf ("/config/resources/record_stats - Record performance statistics\n");
printf ("    From command-line: --record_stats newvalue\n");
printf ("    Specifies whether or not the server records performance statistics in a\n");
printf ("    format suitable for importing into a spreadsheet. If set, the server   \n");
printf ("    creates a file called 'http_stats_(portnumber).del' which contains     \n");
printf ("    performance statistics in a comma-delimited format. For this setting to\n");
printf ("    take effect, the monitor option must also be set.                      \n");
printf ("    Current value is %i. Default value is 0.\n", self->record_stats);
printf ("\n");

printf ("/config/logging/log_path - Active log file directory\n");
printf ("    From command-line: --log_path newvalue\n");
printf ("    Specifies the directory into which active log files are placed. To get\n");
printf ("    the old behaviour, set this option to 'logs'.                         \n");
printf ("    Current value is '%s'. Default value is '/var/log/xitami'\n", self->log_path);
printf ("\n");

printf ("/config/logging/keep_logs - Keep log files\n");
printf ("    From command-line: --keep_logs newvalue\n");
printf ("    Specifies whether or not the server should keep logs. If zero, the   \n");
printf ("    server will not keep logs. If 1, it will keep an alert log, an access\n");
printf ("    log, and a debug log as specified.                                   \n");
printf ("    Current value is %i. Default value is 1.\n", self->keep_logs);
printf ("\n");

printf ("/config/logging/archive_cmd - Archive log file command\n");
printf ("    From command-line: --archive_cmd newvalue\n");
printf ("    If specified, this command is run on all archived log files. You can use\n");
printf ("    this option to compress archived log files, or clean up old and unwanted\n");
printf ("    files. When the command is run, the name of the archived log file is    \n");
printf ("    provided to it as a single argument.                                    \n");
printf ("    Current value is '%s'.\n", self->archive_cmd);
printf ("\n");

printf ("/config/logging/alert_log - Error log file name\n");
printf ("    From command-line: --alert_log newvalue\n");
printf ("    The name of the alert log file, which records all errors and warnings \n");
printf ("    issued by the server. Note that the server port number is always added\n");
printf ("    to the name of the log file.                                          \n");
printf ("    Current value is '%s'. Default value is 'alert.log'\n", self->alert_log);
printf ("\n");

printf ("/config/logging/access_log - Access log file name\n");
printf ("    From command-line: --access_log newvalue\n");
printf ("    The name of the access log file, which records all HTTP requests in the\n");
printf ("    specified access log format. Note that the server port number is always\n");
printf ("    added to the name of the log file.                                     \n");
printf ("    Current value is '%s'. Default value is 'access.log'\n", self->access_log);
printf ("\n");

printf ("/config/logging/access_log_format - Access log file format\n");
printf ("    From command-line: --access_log_format newvalue\n");
printf ("    The format of the access log file, which can be one of: CLF - NCSA/httpd\n");
printf ("    common logging format (basic); CLFX - Apache format; or other - custom  \n");
printf ("    log file format.                                                        \n");
printf ("    Current value is '%s'. Default value is 'CLFX'\n", self->access_log_format);
printf ("\n");

printf ("/config/logging/debug_log - Debug log file name\n");
printf ("    From command-line: --debug_log newvalue\n");
printf ("    The name of the debug log file, which records all normal activity,      \n");
printf ("    errors, and debug trace information. Note that the server port number is\n");
printf ("    always added to the name of the log file.                               \n");
printf ("    Current value is '%s'. Default value is 'debug.log'\n", self->debug_log);
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

printf ("/config/security/user - User to run server as\n");
printf ("    From command-line: --user newvalue\n");
printf ("    If you specify a user and group, the server will switch to that user and\n");
printf ("    group after opening its port, thus dropping any privileges it previously\n");
printf ("    had.                                                                    \n");
printf ("    Current value is '%s'.\n", self->user);
printf ("\n");

printf ("/config/security/group - Group to run server as\n");
printf ("    From command-line: --group newvalue\n");
printf ("    If you specify a user and group, the server will switch to that user and\n");
printf ("    group after opening its port, thus dropping any privileges it previously\n");
printf ("    had.                                                                    \n");
printf ("    Current value is '%s'.\n", self->group);
printf ("\n");

printf ("/config/security/blacklist - Blacklist filename\n");
printf ("    From command-line: --blacklist newvalue\n");
printf ("    Specifies a text file to use for the blacklist. The blacklist is an easy\n");
printf ("    way to blacklist IP addresses of spammers without modifying the main    \n");
printf ("    server configuration files. This file contains one IP address per line. \n");
printf ("    To disable blacklisting, set the option to a non-existent file or "".   \n");
printf ("    Current value is '%s'. Default value is 'blacklist.txt'\n", self->blacklist);
printf ("\n");

printf ("/config/security/basic_auth - Basic authentication filename\n");
printf ("    From command-line: --basic_auth newvalue\n");
printf ("    Specifies the file to use for Basic authentication. The file uses the   \n");
printf ("    Apache htpasswd format, see                                             \n");
printf ("    http://httpd.apache.org/docs/2.2/misc/password_encryptions.html. The    \n");
printf ("    plain text format of each line in this file, if you don't have the      \n");
printf ("    htpasswd tool, is '{username}:{password}'. You can mix digest and passwd\n");
printf ("    password entries in a single file.                                      \n");
printf ("    Current value is '%s'. Default value is 'passwd'\n", self->basic_auth);
printf ("\n");

printf ("/config/security/digest_auth - Digest authentication filename\n");
printf ("    From command-line: --digest_auth newvalue\n");
printf ("    Specifies the file to use for Digest authentication. The file uses the\n");
printf ("    Apache htdigest format, see                                           \n");
printf ("    http://httpd.apache.org/docs/2.2/misc/password_encryptions.html.      \n");
printf ("    Current value is '%s'. Default value is 'digest'\n", self->digest_auth);
printf ("\n");

printf ("/config/security/password_ttl - Password time-to-live, in seconds\n");
printf ("    From command-line: --password_ttl newvalue\n");
printf ("    If set to a non-zero value, passwords will expire this many seconds  \n");
printf ("    after they are first loaded into Xitami. You can reset an expired    \n");
printf ("    password by editing the password file or setting a new password using\n");
printf ("    the Apache htpasswd / htdigest tools.                                \n");
printf ("    Current value is %i. Default value is 0.\n", self->password_ttl);
printf ("\n");

printf ("/config/security/hit_quota - Password hit quota in pages\n");
printf ("    From command-line: --hit_quota newvalue\n");
printf ("    If greater than zero, specifies the number of requests that a browser  \n");
printf ("    can make using the same user id (+ realm, for Digest authentication).  \n");
printf ("    When this quota is exhausted, the id password expires. You can reset an\n");
printf ("    expired password by editing the password file or setting a new password\n");
printf ("    using the Apache htpasswd / htdigest tools.                            \n");
printf ("    Current value is %i. Default value is 0.\n", self->hit_quota);
printf ("\n");

printf ("/config/security/miss_quota - Password miss quota in requests\n");
printf ("    From command-line: --miss_quota newvalue\n");
printf ("    If greater than zero, specifies the number of failed requests that a   \n");
printf ("    browser can make using the same user id (+ realm, for Digest           \n");
printf ("    authentication). When this quota is exhausted, the id password expires.\n");
printf ("    You can reset an expired password by editing the password file or      \n");
printf ("    setting a new password using the Apache htpasswd / htdigest tools.     \n");
printf ("    Current value is %i. Default value is 5.\n", self->miss_quota);
printf ("\n");

printf ("/config/security/password_guard - Password guard timer, in seconds.\n");
printf ("    From command-line: --password_guard newvalue\n");
printf ("    If greater than zero, specifies the frequency with which Xitami saves  \n");
printf ("    password guard data. The password guard ensures that the hit and miss  \n");
printf ("    quotas are properly written to disk so that if Xitami is restarted this\n");
printf ("    data is not lost. If set to zero, Xitami will never save password guard\n");
printf ("    data.                                                                  \n");
printf ("    Current value is %i. Default value is 10.\n", self->password_guard);
printf ("\n");

printf ("/config/security/policy_trace - Debug policy execution?\n");
printf ("    From command-line: --policy_trace newvalue\n");
printf ("    Specifies whether policy execution is logged or not. Set this option to\n");
printf ("    get a visual trace of policy execution, especially when you write your \n");
printf ("    own policies. Do not use on production servers, it will generate vast  \n");
printf ("    amounts of output and slow things down.                                \n");
printf ("    Current value is %i. Default value is 0.\n", self->policy_trace);
printf ("\n");

printf ("/config/security/nervosity - Security nervosity, seconds\n");
printf ("    From command-line: --nervosity newvalue\n");
printf ("    Specifies the interval in seconds at which the server will reload       \n");
printf ("    externally modified security files (blacklist, password, and digest).   \n");
printf ("    You can reduce this to one second but this will cause latency spikes due\n");
printf ("    to CPU burn and memory churn if you have very large blacklist files.    \n");
printf ("    Current value is %i. Default value is 5.\n", self->nervosity);
printf ("\n");

printf ("/config/security/nonce_ttl - Nonce time-to-live, seconds\n");
printf ("    From command-line: --nonce_ttl newvalue\n");
printf ("    This specifies the validity for Digest authentication nonces. Reducing \n");
printf ("    this will increase chatter, as browsers need to request new nonces more\n");
printf ("    often, but reduces the opportunity for replay attacks.                 \n");
printf ("    Current value is %i. Default value is 60.\n", self->nonce_ttl);
printf ("\n");

printf ("/config/security/nonce_quota - Nonce quota in requests\n");
printf ("    From command-line: --nonce_quota newvalue\n");
printf ("    This specifies the number of requests that a browser can make using the\n");
printf ("    same Digest authentication nonce. When this quota is exhausted, the    \n");
printf ("    nonce expires. If set to 1, browsers must ask for a new nonce at each  \n");
printf ("    request.                                                               \n");
printf ("    Current value is %i. Default value is 25.\n", self->nonce_quota);
printf ("\n");

printf ("/config/security/refresh_by - Shell command for password refresh\n");
printf ("    From command-line: --refresh_by newvalue\n");
printf ("    If specified, the http_access_refresh module runs this command to   \n");
printf ("    refresh expired or unknown passwords. You may use this to tie Xitami\n");
printf ("    authentication to external or system authentication systems. See    \n");
printf ("    documentation for details.                                          \n");
printf ("    Current value is '%s'.\n", self->refresh_by);
printf ("\n");

printf ("/config/security/refresh_log - Stdout for password refresh\n");
printf ("    From command-line: --refresh_log newvalue\n");
printf ("    Specifies the log file for the refresh command. All messages produced by\n");
printf ("    the refresh command (stdout and stderr) will be appended to this file.  \n");
printf ("    Current value is '%s'. Default value is 'password_refresh.log'\n", self->refresh_log);
printf ("\n");

printf ("/config/security/address_quota - Maximum open connections per address\n");
printf ("    From command-line: --address_quota newvalue\n");
printf ("    If greater than zero, specifies the number of open connections a client\n");
printf ("    can have. Setting this value to zero allows unlimited connections per  \n");
printf ("    address. This setting is used by the DoS module to detect and handle   \n");
printf ("    simple attempts to crash the server by overloading it with lots of     \n");
printf ("    connections.                                                           \n");
printf ("    Current value is %i. Default value is 5.\n", self->address_quota);
printf ("\n");

printf ("/config/security/abort_delay - Pause time for hostile connections\n");
printf ("    From command-line: --abort_delay newvalue\n");
printf ("    This specifies the number of seconds that the server will wait, when    \n");
printf ("    shutting a connection due to detected hostile activity. The delay wastes\n");
printf ("    browser time, making attacks less efficient.                            \n");
printf ("    Current value is %i. Default value is 5.\n", self->abort_delay);
printf ("\n");

printf ("/config/tuning/coremark_cycles - Duration of CoreMark test\n");
printf ("    From command-line: --coremark_cycles newvalue\n");
printf ("    Defines the duration of the automatic CoreMark performance test, in  \n");
printf ("    event cycles. If 0, no test will be done. Setting this too high will \n");
printf ("    cause the server to burn CPU at startup. A good value is between 100k\n");
printf ("    and 1M.                                                              \n");
printf ("    Current value is %i. Default value is 250000.\n", self->coremark_cycles);
printf ("\n");

printf ("/config/tuning/header_max - Max size of HTTP header\n");
printf ("    From command-line: --header_max newvalue\n");
printf ("    Defines the limit for HTTP headers; the server will accept up to this   \n");
printf ("    much data. You can increase the limit if you have applications that pass\n");
printf ("    excessive amounts of data as URI arguments, but usually it is           \n");
printf ("    recommended to pass large web forms as POST body data.                  \n");
printf ("    Current value is %i. Default value is 32000.\n", self->header_max);
printf ("\n");

printf ("/config/tuning/bucket_max - Max size of content bucket\n");
printf ("    From command-line: --bucket_max newvalue\n");
printf ("    Defines the limit for content managed as a memory bucket. The HTTP   \n");
printf ("    server can handle contents of any size, but if they exceed the bucket\n");
printf ("    max they will be spooled to disk, which can be slow.                 \n");
printf ("    Current value is %i. Default value is 128000.\n", self->bucket_max);
printf ("\n");

printf ("/config/tuning/read_timeout - Network read timeout, seconds\n");
printf ("    From command-line: --read_timeout newvalue\n");
printf ("    Defines the timeout for network reads. If a client does not send data \n");
printf ("    within this time, it is considered dead. If you get unexpected timeout\n");
printf ("    errors in the logs, you may want to increase this value.              \n");
printf ("    Current value is %i. Default value is 60.\n", self->read_timeout);
printf ("\n");

printf ("/config/tuning/write_timeout - Network write timeout, seconds\n");
printf ("    From command-line: --write_timeout newvalue\n");
printf ("    Defines the timeout for network writes. If a client does not accept data\n");
printf ("    within this time, it is considered dead. If you get unexpected timeout  \n");
printf ("    errors in the logs, you may want to increase this value.                \n");
printf ("    Current value is %i. Default value is 60.\n", self->write_timeout);
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
printf ("    Current value is %i. Default value is 0.\n", self->tcp_rcvbuf);
printf ("\n");

printf ("/config/tuning/tcp_sndbuf - TCP/IP send buffer, in bytes\n");
printf ("    From command-line: --tcp_sndbuf newvalue\n");
printf ("    If this value is greater than zero, all client connections will use the\n");
printf ("    specified value. Note: setting this value is delicate, do not use this \n");
printf ("    option unless you know what you are doing.                             \n");
printf ("    Current value is %i. Default value is 0.\n", self->tcp_sndbuf);
printf ("\n");

printf ("/config/tuning/polling_threads - Number of polling OS threads\n");
printf ("    From command-line: --polling_threads newvalue\n");
printf ("    On multithreaded builds, defines the number of OS threads dedicated to\n");
printf ("    socket polling.                                                       \n");
printf ("    Current value is %i. Default value is 4.\n", self->polling_threads);
printf ("\n");

printf ("/config/tuning/working_threads - Number of working OS threads\n");
printf ("    From command-line: --working_threads newvalue\n");
printf ("    On multithreaded builds, defines the number of OS threads dedicated to\n");
printf ("    processing, that is, tasks other than socket polling.                 \n");
printf ("    Current value is %i. Default value is 4.\n", self->working_threads);
printf ("\n");

printf ("/config/cache/etag - ETag header for files\n");
printf ("    From command-line: --etag newvalue\n");
printf ("    Defines the ETag header value to use for files. If 'fixed', the ETag  \n");
printf ("    consists of the file's modified time, size, and inode. If the etag    \n");
printf ("    option is 'float', the ETag consists of the file's modified time and  \n");
printf ("    size only. This is useful for large deployments where files are served\n");
printf ("    from multiple file systems. If the ETag option is set to any other    \n");
printf ("    value, no ETag headers will be produced.                              \n");
printf ("    Current value is '%s'. Default value is 'fixed'\n", self->etag);
printf ("\n");


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_set_server_name

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_server_name (
    http_config_t * self,               //  Reference to object
    char * server_name                  //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "name", server_name);
server_name = ipr_config_getp (self->table, "/config/server", "name", NULL);
assert (server_name);
self->server_name = server_name;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_server_name

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_server_name (
    http_config_t * self                //  Reference to self
)
{
    char *
        server_name;                    //  Not documented

//
if (self)
    server_name = self->server_name;
else
    server_name = "X5";

    return (server_name);
}
/*  -------------------------------------------------------------------------
    http_config_set_port

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_port (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_port

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_port (
    http_config_t * self                //  Reference to self
)
{
    char *
        port;                           //  Not documented

//
if (self)
    port = self->port;
else
    port = "80";

    return (port);
}
/*  -------------------------------------------------------------------------
    http_config_set_public_port

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_public_port (
    http_config_t * self,               //  Reference to object
    char * public_port                  //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/server", "public_port", public_port);
public_port = ipr_config_getp (self->table, "/config/server", "public_port", NULL);
assert (public_port);
self->public_port = public_port;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_public_port

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_public_port (
    http_config_t * self                //  Reference to self
)
{
    char *
        public_port;                    //  Not documented

//
if (self)
    public_port = self->public_port;
else
    public_port = "";

    return (public_port);
}
/*  -------------------------------------------------------------------------
    http_config_set_trace

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_trace (
    http_config_t * self,               //  Reference to object
    Bool trace                          //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", trace);
ipr_config_putp (self->table, "/config/server", "trace", strvalue);
self->trace = trace;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_trace

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_trace (
    http_config_t * self                //  Reference to self
)
{
    Bool
        trace;                          //  Not documented

//
if (self)
    trace = self->trace;
else
    trace = 0;

    return (trace);
}
/*  -------------------------------------------------------------------------
    http_config_set_animate

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_animate (
    http_config_t * self,               //  Reference to object
    Bool animate                        //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", animate);
ipr_config_putp (self->table, "/config/server", "animate", strvalue);
self->animate = animate;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_animate

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_animate (
    http_config_t * self                //  Reference to self
)
{
    Bool
        animate;                        //  Not documented

//
if (self)
    animate = self->animate;
else
    animate = 0;

    return (animate);
}
/*  -------------------------------------------------------------------------
    http_config_set_verbose

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_verbose (
    http_config_t * self,               //  Reference to object
    Bool verbose                        //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", verbose);
ipr_config_putp (self->table, "/config/server", "verbose", strvalue);
self->verbose = verbose;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_verbose

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_verbose (
    http_config_t * self                //  Reference to self
)
{
    Bool
        verbose;                        //  Not documented

//
if (self)
    verbose = self->verbose;
else
    verbose = 0;

    return (verbose);
}
/*  -------------------------------------------------------------------------
    http_config_set_listen

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_listen (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_listen

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_listen (
    http_config_t * self                //  Reference to self
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
    http_config_set_webroot

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_webroot (
    http_config_t * self,               //  Reference to object
    char * webroot                      //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/host", "webroot", webroot);
webroot = ipr_config_getp (self->table, "/config/host", "webroot", NULL);
assert (webroot);
self->webroot = webroot;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_webroot

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_webroot (
    http_config_t * self                //  Reference to self
)
{
    char *
        webroot;                        //  Not documented

//
if (self)
    webroot = self->webroot;
else
    webroot = "webpages";

    return (webroot);
}
/*  -------------------------------------------------------------------------
    http_config_set_defaults

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_defaults (
    http_config_t * self,               //  Reference to object
    char * defaults                     //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/host", "defaults", defaults);
defaults = ipr_config_getp (self->table, "/config/host", "defaults", NULL);
assert (defaults);
self->defaults = defaults;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_defaults

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_defaults (
    http_config_t * self                //  Reference to self
)
{
    char *
        defaults;                       //  Not documented

//
if (self)
    defaults = self->defaults;
else
    defaults = "index.html default.html";

    return (defaults);
}
/*  -------------------------------------------------------------------------
    http_config_set_default_types

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_default_types (
    http_config_t * self,               //  Reference to object
    char * default_types                //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/host", "default_types", default_types);
default_types = ipr_config_getp (self->table, "/config/host", "default_types", NULL);
assert (default_types);
self->default_types = default_types;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_default_types

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_default_types (
    http_config_t * self                //  Reference to self
)
{
    char *
        default_types;                  //  Not documented

//
if (self)
    default_types = self->default_types;
else
    default_types = ".pdf .html";

    return (default_types);
}
/*  -------------------------------------------------------------------------
    http_config_set_monitor

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_monitor (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_monitor

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_monitor (
    http_config_t * self                //  Reference to self
)
{
    int
        monitor;                        //  Not documented

//
if (self)
    monitor = self->monitor;
else
    monitor = 0;

    return (monitor);
}
/*  -------------------------------------------------------------------------
    http_config_set_record_stats

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_record_stats (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_record_stats

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_record_stats (
    http_config_t * self                //  Reference to self
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
    http_config_set_log_path

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_log_path (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_log_path

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_log_path (
    http_config_t * self                //  Reference to self
)
{
    char *
        log_path;                       //  Not documented

//
if (self)
    log_path = self->log_path;
else
    log_path = "/var/log/xitami";

    return (log_path);
}
/*  -------------------------------------------------------------------------
    http_config_set_keep_logs

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_keep_logs (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_keep_logs

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_keep_logs (
    http_config_t * self                //  Reference to self
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
    http_config_set_archive_cmd

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_archive_cmd (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_archive_cmd

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_archive_cmd (
    http_config_t * self                //  Reference to self
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
    http_config_set_alert_log

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_alert_log (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_alert_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_alert_log (
    http_config_t * self                //  Reference to self
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
    http_config_set_access_log

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_access_log (
    http_config_t * self,               //  Reference to object
    char * access_log                   //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "access_log", access_log);
access_log = ipr_config_getp (self->table, "/config/logging", "access_log", NULL);
assert (access_log);
self->access_log = access_log;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_access_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_access_log (
    http_config_t * self                //  Reference to self
)
{
    char *
        access_log;                     //  Not documented

//
if (self)
    access_log = self->access_log;
else
    access_log = "access.log";

    return (access_log);
}
/*  -------------------------------------------------------------------------
    http_config_set_access_log_format

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_access_log_format (
    http_config_t * self,               //  Reference to object
    char * access_log_format            //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/logging", "access_log_format", access_log_format);
access_log_format = ipr_config_getp (self->table, "/config/logging", "access_log_format", NULL);
assert (access_log_format);
self->access_log_format = access_log_format;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_access_log_format

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_access_log_format (
    http_config_t * self                //  Reference to self
)
{
    char *
        access_log_format;              //  Not documented

//
if (self)
    access_log_format = self->access_log_format;
else
    access_log_format = "CLFX";

    return (access_log_format);
}
/*  -------------------------------------------------------------------------
    http_config_set_debug_log

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_debug_log (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_debug_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_debug_log (
    http_config_t * self                //  Reference to self
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
    http_config_set_syslog

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_syslog (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_syslog

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_syslog (
    http_config_t * self                //  Reference to self
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
    http_config_set_user

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_user (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "user", user);
user = ipr_config_getp (self->table, "/config/security", "user", NULL);
assert (user);
self->user = user;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_user

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_user (
    http_config_t * self                //  Reference to self
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
    http_config_set_group

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_group (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "group", group);
group = ipr_config_getp (self->table, "/config/security", "group", NULL);
assert (group);
self->group = group;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_group

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_group (
    http_config_t * self                //  Reference to self
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
    http_config_set_blacklist

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_blacklist (
    http_config_t * self,               //  Reference to object
    char * blacklist                    //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "blacklist", blacklist);
blacklist = ipr_config_getp (self->table, "/config/security", "blacklist", NULL);
assert (blacklist);
self->blacklist = blacklist;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_blacklist

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_blacklist (
    http_config_t * self                //  Reference to self
)
{
    char *
        blacklist;                      //  Not documented

//
if (self)
    blacklist = self->blacklist;
else
    blacklist = "blacklist.txt";

    return (blacklist);
}
/*  -------------------------------------------------------------------------
    http_config_set_basic_auth

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_basic_auth (
    http_config_t * self,               //  Reference to object
    char * basic_auth                   //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "basic_auth", basic_auth);
basic_auth = ipr_config_getp (self->table, "/config/security", "basic_auth", NULL);
assert (basic_auth);
self->basic_auth = basic_auth;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_basic_auth

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_basic_auth (
    http_config_t * self                //  Reference to self
)
{
    char *
        basic_auth;                     //  Not documented

//
if (self)
    basic_auth = self->basic_auth;
else
    basic_auth = "passwd";

    return (basic_auth);
}
/*  -------------------------------------------------------------------------
    http_config_set_digest_auth

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_digest_auth (
    http_config_t * self,               //  Reference to object
    char * digest_auth                  //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "digest_auth", digest_auth);
digest_auth = ipr_config_getp (self->table, "/config/security", "digest_auth", NULL);
assert (digest_auth);
self->digest_auth = digest_auth;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_digest_auth

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_digest_auth (
    http_config_t * self                //  Reference to self
)
{
    char *
        digest_auth;                    //  Not documented

//
if (self)
    digest_auth = self->digest_auth;
else
    digest_auth = "digest";

    return (digest_auth);
}
/*  -------------------------------------------------------------------------
    http_config_set_password_ttl

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_password_ttl (
    http_config_t * self,               //  Reference to object
    int password_ttl                    //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", password_ttl);
ipr_config_putp (self->table, "/config/security", "password_ttl", strvalue);
self->password_ttl = password_ttl;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_password_ttl

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_password_ttl (
    http_config_t * self                //  Reference to self
)
{
    int
        password_ttl;                   //  Not documented

//
if (self)
    password_ttl = self->password_ttl;
else
    password_ttl = 0;

    return (password_ttl);
}
/*  -------------------------------------------------------------------------
    http_config_set_hit_quota

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_hit_quota (
    http_config_t * self,               //  Reference to object
    int hit_quota                       //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", hit_quota);
ipr_config_putp (self->table, "/config/security", "hit_quota", strvalue);
self->hit_quota = hit_quota;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_hit_quota

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_hit_quota (
    http_config_t * self                //  Reference to self
)
{
    int
        hit_quota;                      //  Not documented

//
if (self)
    hit_quota = self->hit_quota;
else
    hit_quota = 0;

    return (hit_quota);
}
/*  -------------------------------------------------------------------------
    http_config_set_miss_quota

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_miss_quota (
    http_config_t * self,               //  Reference to object
    int miss_quota                      //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", miss_quota);
ipr_config_putp (self->table, "/config/security", "miss_quota", strvalue);
self->miss_quota = miss_quota;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_miss_quota

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_miss_quota (
    http_config_t * self                //  Reference to self
)
{
    int
        miss_quota;                     //  Not documented

//
if (self)
    miss_quota = self->miss_quota;
else
    miss_quota = 5;

    return (miss_quota);
}
/*  -------------------------------------------------------------------------
    http_config_set_password_guard

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_password_guard (
    http_config_t * self,               //  Reference to object
    int password_guard                  //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", password_guard);
ipr_config_putp (self->table, "/config/security", "password_guard", strvalue);
self->password_guard = password_guard;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_password_guard

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_password_guard (
    http_config_t * self                //  Reference to self
)
{
    int
        password_guard;                 //  Not documented

//
if (self)
    password_guard = self->password_guard;
else
    password_guard = 10;

    return (password_guard);
}
/*  -------------------------------------------------------------------------
    http_config_set_policy_trace

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_policy_trace (
    http_config_t * self,               //  Reference to object
    Bool policy_trace                   //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", policy_trace);
ipr_config_putp (self->table, "/config/security", "policy_trace", strvalue);
self->policy_trace = policy_trace;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_policy_trace

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_policy_trace (
    http_config_t * self                //  Reference to self
)
{
    Bool
        policy_trace;                   //  Not documented

//
if (self)
    policy_trace = self->policy_trace;
else
    policy_trace = 0;

    return (policy_trace);
}
/*  -------------------------------------------------------------------------
    http_config_set_nervosity

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_nervosity (
    http_config_t * self,               //  Reference to object
    int nervosity                       //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
if (nervosity < 1) {
    icl_console_print ("W: <command line>: illegal value %d for nervosity, using %d",
        nervosity, 1);
    nervosity = 1;
}
icl_shortstr_fmt (strvalue, "%i", nervosity);
ipr_config_putp (self->table, "/config/security", "nervosity", strvalue);
self->nervosity = nervosity;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_nervosity

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_nervosity (
    http_config_t * self                //  Reference to self
)
{
    int
        nervosity;                      //  Not documented

//
if (self)
    nervosity = self->nervosity;
else
    nervosity = 5;

    return (nervosity);
}
/*  -------------------------------------------------------------------------
    http_config_set_nonce_ttl

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_nonce_ttl (
    http_config_t * self,               //  Reference to object
    int nonce_ttl                       //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
if (nonce_ttl < 1) {
    icl_console_print ("W: <command line>: illegal value %d for nonce_ttl, using %d",
        nonce_ttl, 1);
    nonce_ttl = 1;
}
icl_shortstr_fmt (strvalue, "%i", nonce_ttl);
ipr_config_putp (self->table, "/config/security", "nonce_ttl", strvalue);
self->nonce_ttl = nonce_ttl;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_nonce_ttl

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_nonce_ttl (
    http_config_t * self                //  Reference to self
)
{
    int
        nonce_ttl;                      //  Not documented

//
if (self)
    nonce_ttl = self->nonce_ttl;
else
    nonce_ttl = 60;

    return (nonce_ttl);
}
/*  -------------------------------------------------------------------------
    http_config_set_nonce_quota

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_nonce_quota (
    http_config_t * self,               //  Reference to object
    int nonce_quota                     //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
if (nonce_quota < 1) {
    icl_console_print ("W: <command line>: illegal value %d for nonce_quota, using %d",
        nonce_quota, 1);
    nonce_quota = 1;
}
icl_shortstr_fmt (strvalue, "%i", nonce_quota);
ipr_config_putp (self->table, "/config/security", "nonce_quota", strvalue);
self->nonce_quota = nonce_quota;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_nonce_quota

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_nonce_quota (
    http_config_t * self                //  Reference to self
)
{
    int
        nonce_quota;                    //  Not documented

//
if (self)
    nonce_quota = self->nonce_quota;
else
    nonce_quota = 25;

    return (nonce_quota);
}
/*  -------------------------------------------------------------------------
    http_config_set_refresh_by

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_refresh_by (
    http_config_t * self,               //  Reference to object
    char * refresh_by                   //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "refresh_by", refresh_by);
refresh_by = ipr_config_getp (self->table, "/config/security", "refresh_by", NULL);
assert (refresh_by);
self->refresh_by = refresh_by;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_refresh_by

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_refresh_by (
    http_config_t * self                //  Reference to self
)
{
    char *
        refresh_by;                     //  Not documented

//
if (self)
    refresh_by = self->refresh_by;
else
    refresh_by = "";

    return (refresh_by);
}
/*  -------------------------------------------------------------------------
    http_config_set_refresh_log

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_refresh_log (
    http_config_t * self,               //  Reference to object
    char * refresh_log                  //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/security", "refresh_log", refresh_log);
refresh_log = ipr_config_getp (self->table, "/config/security", "refresh_log", NULL);
assert (refresh_log);
self->refresh_log = refresh_log;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_refresh_log

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_refresh_log (
    http_config_t * self                //  Reference to self
)
{
    char *
        refresh_log;                    //  Not documented

//
if (self)
    refresh_log = self->refresh_log;
else
    refresh_log = "password_refresh.log";

    return (refresh_log);
}
/*  -------------------------------------------------------------------------
    http_config_set_address_quota

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_address_quota (
    http_config_t * self,               //  Reference to object
    int address_quota                   //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", address_quota);
ipr_config_putp (self->table, "/config/security", "address_quota", strvalue);
self->address_quota = address_quota;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_address_quota

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_address_quota (
    http_config_t * self                //  Reference to self
)
{
    int
        address_quota;                  //  Not documented

//
if (self)
    address_quota = self->address_quota;
else
    address_quota = 5;

    return (address_quota);
}
/*  -------------------------------------------------------------------------
    http_config_set_abort_delay

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_abort_delay (
    http_config_t * self,               //  Reference to object
    int abort_delay                     //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", abort_delay);
ipr_config_putp (self->table, "/config/security", "abort_delay", strvalue);
self->abort_delay = abort_delay;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_abort_delay

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_abort_delay (
    http_config_t * self                //  Reference to self
)
{
    int
        abort_delay;                    //  Not documented

//
if (self)
    abort_delay = self->abort_delay;
else
    abort_delay = 5;

    return (abort_delay);
}
/*  -------------------------------------------------------------------------
    http_config_set_coremark_cycles

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_coremark_cycles (
    http_config_t * self,               //  Reference to object
    int coremark_cycles                 //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", coremark_cycles);
ipr_config_putp (self->table, "/config/tuning", "coremark_cycles", strvalue);
self->coremark_cycles = coremark_cycles;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_coremark_cycles

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_coremark_cycles (
    http_config_t * self                //  Reference to self
)
{
    int
        coremark_cycles;                //  Not documented

//
if (self)
    coremark_cycles = self->coremark_cycles;
else
    coremark_cycles = 250000;

    return (coremark_cycles);
}
/*  -------------------------------------------------------------------------
    http_config_set_header_max

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_header_max (
    http_config_t * self,               //  Reference to object
    int header_max                      //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
if (header_max < 2048) {
    icl_console_print ("W: <command line>: illegal value %d for header_max, using %d",
        header_max, 2048);
    header_max = 2048;
}
if (header_max > 2097152) {
    icl_console_print ("W: <command line>: illegal value %d for header_max, using %d",
        header_max, 2097152);
    header_max = 2097152;
}
icl_shortstr_fmt (strvalue, "%i", header_max);
ipr_config_putp (self->table, "/config/tuning", "header_max", strvalue);
self->header_max = header_max;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_header_max

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_header_max (
    http_config_t * self                //  Reference to self
)
{
    int
        header_max;                     //  Not documented

//
if (self)
    header_max = self->header_max;
else
    header_max = 32000;

    return (header_max);
}
/*  -------------------------------------------------------------------------
    http_config_set_bucket_max

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_bucket_max (
    http_config_t * self,               //  Reference to object
    int bucket_max                      //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
if (bucket_max > 2097152) {
    icl_console_print ("W: <command line>: illegal value %d for bucket_max, using %d",
        bucket_max, 2097152);
    bucket_max = 2097152;
}
icl_shortstr_fmt (strvalue, "%i", bucket_max);
ipr_config_putp (self->table, "/config/tuning", "bucket_max", strvalue);
self->bucket_max = bucket_max;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_bucket_max

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_bucket_max (
    http_config_t * self                //  Reference to self
)
{
    int
        bucket_max;                     //  Not documented

//
if (self)
    bucket_max = self->bucket_max;
else
    bucket_max = 128000;

    return (bucket_max);
}
/*  -------------------------------------------------------------------------
    http_config_set_read_timeout

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_read_timeout (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_read_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_read_timeout (
    http_config_t * self                //  Reference to self
)
{
    int
        read_timeout;                   //  Not documented

//
if (self)
    read_timeout = self->read_timeout;
else
    read_timeout = 60;

    return (read_timeout);
}
/*  -------------------------------------------------------------------------
    http_config_set_write_timeout

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_write_timeout (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_write_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_write_timeout (
    http_config_t * self                //  Reference to self
)
{
    int
        write_timeout;                  //  Not documented

//
if (self)
    write_timeout = self->write_timeout;
else
    write_timeout = 60;

    return (write_timeout);
}
/*  -------------------------------------------------------------------------
    http_config_set_accept_retry_timeout

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_accept_retry_timeout (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_accept_retry_timeout

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_accept_retry_timeout (
    http_config_t * self                //  Reference to self
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
    http_config_set_tcp_nodelay

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_tcp_nodelay (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_tcp_nodelay

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    http_config_tcp_nodelay (
    http_config_t * self                //  Reference to self
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
    http_config_set_tcp_rcvbuf

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_tcp_rcvbuf (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_tcp_rcvbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_tcp_rcvbuf (
    http_config_t * self                //  Reference to self
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
    http_config_set_tcp_sndbuf

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_tcp_sndbuf (
    http_config_t * self,               //  Reference to object
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

HTTP_CONFIG_ASSERT_SANE (self);

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
    http_config_tcp_sndbuf

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_tcp_sndbuf (
    http_config_t * self                //  Reference to self
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
    http_config_set_polling_threads

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_polling_threads (
    http_config_t * self,               //  Reference to object
    int polling_threads                 //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", polling_threads);
ipr_config_putp (self->table, "/config/tuning", "polling_threads", strvalue);
self->polling_threads = polling_threads;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_polling_threads

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_polling_threads (
    http_config_t * self                //  Reference to self
)
{
    int
        polling_threads;                //  Not documented

//
if (self)
    polling_threads = self->polling_threads;
else
    polling_threads = 4;

    return (polling_threads);
}
/*  -------------------------------------------------------------------------
    http_config_set_working_threads

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_working_threads (
    http_config_t * self,               //  Reference to object
    int working_threads                 //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

//
icl_shortstr_fmt (strvalue, "%i", working_threads);
ipr_config_putp (self->table, "/config/tuning", "working_threads", strvalue);
self->working_threads = working_threads;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_working_threads

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_config_working_threads (
    http_config_t * self                //  Reference to self
)
{
    int
        working_threads;                //  Not documented

//
if (self)
    working_threads = self->working_threads;
else
    working_threads = 4;

    return (working_threads);
}
/*  -------------------------------------------------------------------------
    http_config_set_etag

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_set_etag (
    http_config_t * self,               //  Reference to object
    char * etag                         //  Not documented
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

HTTP_CONFIG_ASSERT_SANE (self);

ipr_config_putp (self->table, "/config/cache", "etag", etag);
etag = ipr_config_getp (self->table, "/config/cache", "etag", NULL);
assert (etag);
self->etag = etag;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_etag

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_etag (
    http_config_t * self                //  Reference to self
)
{
    char *
        etag;                           //  Not documented

//
if (self)
    etag = self->etag;
else
    etag = "fixed";

    return (etag);
}
/*  -------------------------------------------------------------------------
    http_config_dump_log

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    Dump current option values to an smt_log file.
    -------------------------------------------------------------------------
 */

int
    http_config_dump_log (
    http_config_t * self,               //  Reference to object
    smt_log_t * logfile                 //  Log file to dump to
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_config_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_config_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    http_config_filename (
    http_config_t * self                //  Not documented
)
{
    char *
        filename;                       //  Not documented

filename = self->filename;

    return (filename);
}
/*  -------------------------------------------------------------------------
    http_config_set_filename

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_config_set_filename (
    http_config_t * self,               //  Not documented
    char * filename                     //  Not documented
)
{

icl_mem_free (self->filename);
self->filename = icl_mem_strdup (filename);
}
/*  -------------------------------------------------------------------------
    http_config_table

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_config_t *
    http_config_table (
    http_config_t * self                //  Not documented
)
{
    ipr_config_t *
        table;                          //  Not documented

table = self->table;

    return (table);
}
/*  -------------------------------------------------------------------------
    http_config_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_config_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_config_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_config_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
http_config_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <http_config file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    http_config_destroy

    Type: Component method
    Destroys a http_config_t object. Takes the address of a
    http_config_t reference (a pointer to a pointer) and nullifies the
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
    http_config_destroy_ (
    http_config_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    http_config_annihilate (self_p);
    http_config_free ((http_config_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_config_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_config_t *
    http_config_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_config_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_config_cache_initialise ();

self = (http_config_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_config_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_config_free

    Type: Component method
    Freess a http_config_t object.
    -------------------------------------------------------------------------
 */

static void
    http_config_free (
    http_config_t * self                //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (http_config_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_config_t));
        self->object_tag = HTTP_CONFIG_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_config_read_lock

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_read_lock (
    http_config_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_write_lock

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_write_lock (
    http_config_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_unlock

    Type: Component method
    Accepts a http_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_config_unlock (
    http_config_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    http_config_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_config_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_config_t));
icl_system_register (http_config_cache_purge, http_config_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_config_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_config_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_config_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_config_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_config_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_config_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_config_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_config_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_config_new_in_scope_ (
    http_config_t * * self_p,           //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_config_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_config_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_config_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__http_config_component        = "http_config ";
char *EMBED__http_config_version          = "1.1 ";
char *EMBED__http_config_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_config_filename         = "http_config.icl ";
char *EMBED__http_config_builddate        = "2011/03/01 ";
char *EMBED__http_config_version_end      = "VeRsIoNeNd:ipc";

