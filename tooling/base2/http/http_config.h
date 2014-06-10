/*---------------------------------------------------------------------------
    http_config.h - http_config component

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

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_HTTP_CONFIG_SAFE
#   define INCLUDE_HTTP_CONFIG_SAFE
#   define INCLUDE_HTTP_CONFIG_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_CONFIG_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_config_t http_config_t;

#define HTTP_CONFIG_ALIVE               0xFABB
#define HTTP_CONFIG_DEAD                0xDEAD

#define HTTP_CONFIG_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_config\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_CONFIG_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_config at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_CONFIG_ALIVE, self->object_tag);\
        http_config_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "smt_log.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include "ipr.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_HTTP_CONFIG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_CONFIG_UNSAFE
#   define INCLUDE_HTTP_CONFIG_UNSAFE
#   define INCLUDE_HTTP_CONFIG_ACTIVE

#   include "smt_log.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "ipr.h"
//  Global variables

extern Bool
    http_config_animating;
extern http_config_t
    *http_config;                   //  Server properties object
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_config_new()               http_config_new_ (__FILE__, __LINE__)
http_config_t *
    http_config_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    http_config_load_xmlfile (
http_config_t * self,                   //  Reference to object
char * filename,                        //  Config file to load
Bool required                           //  Config file is required?
);

int
    http_config_load_bucket (
http_config_t * self,                   //  Reference to object
ipr_bucket_t * bucket                   //  Config data to load
);

int
    http_config_shadow (
http_config_t * self,                   //  Reference to object
ipr_config_t * source                   //  Config data to shadow
);

int
    http_config_commit (
http_config_t * self,                   //  Reference to object
char * filename                         //  Config file to save
);

int
    http_config_rollback (
http_config_t * self                    //  Reference to object
);

void
    http_config_cmdline_help (
void);

int
    http_config_cmdline_parse (
http_config_t * self,                   //  Reference to object
char * name,                            //  Name of application
int argc,                               //  Argument count
char ** argv                            //  Argument list
);

int
    http_config_options_help (
http_config_t * self,                   //  Reference to object
char * name                             //  Name of application
);

int
    http_config_set_server_name (
http_config_t * self,                   //  Reference to object
char * server_name                      //  Not documented
);

char *
    http_config_server_name (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_port (
http_config_t * self,                   //  Reference to object
char * port                             //  Not documented
);

char *
    http_config_port (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_public_port (
http_config_t * self,                   //  Reference to object
char * public_port                      //  Not documented
);

char *
    http_config_public_port (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_trace (
http_config_t * self,                   //  Reference to object
Bool trace                              //  Not documented
);

Bool
    http_config_trace (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_animate (
http_config_t * self,                   //  Reference to object
Bool animate                            //  Not documented
);

Bool
    http_config_animate (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_verbose (
http_config_t * self,                   //  Reference to object
Bool verbose                            //  Not documented
);

Bool
    http_config_verbose (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_listen (
http_config_t * self,                   //  Reference to object
char * listen                           //  Not documented
);

char *
    http_config_listen (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_webroot (
http_config_t * self,                   //  Reference to object
char * webroot                          //  Not documented
);

char *
    http_config_webroot (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_defaults (
http_config_t * self,                   //  Reference to object
char * defaults                         //  Not documented
);

char *
    http_config_defaults (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_default_types (
http_config_t * self,                   //  Reference to object
char * default_types                    //  Not documented
);

char *
    http_config_default_types (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_monitor (
http_config_t * self,                   //  Reference to object
int monitor                             //  Not documented
);

int
    http_config_monitor (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_record_stats (
http_config_t * self,                   //  Reference to object
Bool record_stats                       //  Not documented
);

Bool
    http_config_record_stats (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_log_path (
http_config_t * self,                   //  Reference to object
char * log_path                         //  Not documented
);

char *
    http_config_log_path (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_keep_logs (
http_config_t * self,                   //  Reference to object
Bool keep_logs                          //  Not documented
);

Bool
    http_config_keep_logs (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_archive_cmd (
http_config_t * self,                   //  Reference to object
char * archive_cmd                      //  Not documented
);

char *
    http_config_archive_cmd (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_alert_log (
http_config_t * self,                   //  Reference to object
char * alert_log                        //  Not documented
);

char *
    http_config_alert_log (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_access_log (
http_config_t * self,                   //  Reference to object
char * access_log                       //  Not documented
);

char *
    http_config_access_log (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_access_log_format (
http_config_t * self,                   //  Reference to object
char * access_log_format                //  Not documented
);

char *
    http_config_access_log_format (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_debug_log (
http_config_t * self,                   //  Reference to object
char * debug_log                        //  Not documented
);

char *
    http_config_debug_log (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_syslog (
http_config_t * self,                   //  Reference to object
int syslog                              //  Not documented
);

int
    http_config_syslog (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_user (
http_config_t * self,                   //  Reference to object
char * user                             //  Not documented
);

char *
    http_config_user (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_group (
http_config_t * self,                   //  Reference to object
char * group                            //  Not documented
);

char *
    http_config_group (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_blacklist (
http_config_t * self,                   //  Reference to object
char * blacklist                        //  Not documented
);

char *
    http_config_blacklist (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_basic_auth (
http_config_t * self,                   //  Reference to object
char * basic_auth                       //  Not documented
);

char *
    http_config_basic_auth (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_digest_auth (
http_config_t * self,                   //  Reference to object
char * digest_auth                      //  Not documented
);

char *
    http_config_digest_auth (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_password_ttl (
http_config_t * self,                   //  Reference to object
int password_ttl                        //  Not documented
);

int
    http_config_password_ttl (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_hit_quota (
http_config_t * self,                   //  Reference to object
int hit_quota                           //  Not documented
);

int
    http_config_hit_quota (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_miss_quota (
http_config_t * self,                   //  Reference to object
int miss_quota                          //  Not documented
);

int
    http_config_miss_quota (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_password_guard (
http_config_t * self,                   //  Reference to object
int password_guard                      //  Not documented
);

int
    http_config_password_guard (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_policy_trace (
http_config_t * self,                   //  Reference to object
Bool policy_trace                       //  Not documented
);

Bool
    http_config_policy_trace (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_nervosity (
http_config_t * self,                   //  Reference to object
int nervosity                           //  Not documented
);

int
    http_config_nervosity (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_nonce_ttl (
http_config_t * self,                   //  Reference to object
int nonce_ttl                           //  Not documented
);

int
    http_config_nonce_ttl (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_nonce_quota (
http_config_t * self,                   //  Reference to object
int nonce_quota                         //  Not documented
);

int
    http_config_nonce_quota (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_refresh_by (
http_config_t * self,                   //  Reference to object
char * refresh_by                       //  Not documented
);

char *
    http_config_refresh_by (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_refresh_log (
http_config_t * self,                   //  Reference to object
char * refresh_log                      //  Not documented
);

char *
    http_config_refresh_log (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_address_quota (
http_config_t * self,                   //  Reference to object
int address_quota                       //  Not documented
);

int
    http_config_address_quota (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_abort_delay (
http_config_t * self,                   //  Reference to object
int abort_delay                         //  Not documented
);

int
    http_config_abort_delay (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_coremark_cycles (
http_config_t * self,                   //  Reference to object
int coremark_cycles                     //  Not documented
);

int
    http_config_coremark_cycles (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_header_max (
http_config_t * self,                   //  Reference to object
int header_max                          //  Not documented
);

int
    http_config_header_max (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_bucket_max (
http_config_t * self,                   //  Reference to object
int bucket_max                          //  Not documented
);

int
    http_config_bucket_max (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_read_timeout (
http_config_t * self,                   //  Reference to object
int read_timeout                        //  Not documented
);

int
    http_config_read_timeout (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_write_timeout (
http_config_t * self,                   //  Reference to object
int write_timeout                       //  Not documented
);

int
    http_config_write_timeout (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_accept_retry_timeout (
http_config_t * self,                   //  Reference to object
int accept_retry_timeout                //  Not documented
);

int
    http_config_accept_retry_timeout (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_tcp_nodelay (
http_config_t * self,                   //  Reference to object
Bool tcp_nodelay                        //  Not documented
);

Bool
    http_config_tcp_nodelay (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_tcp_rcvbuf (
http_config_t * self,                   //  Reference to object
int tcp_rcvbuf                          //  Not documented
);

int
    http_config_tcp_rcvbuf (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_tcp_sndbuf (
http_config_t * self,                   //  Reference to object
int tcp_sndbuf                          //  Not documented
);

int
    http_config_tcp_sndbuf (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_polling_threads (
http_config_t * self,                   //  Reference to object
int polling_threads                     //  Not documented
);

int
    http_config_polling_threads (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_working_threads (
http_config_t * self,                   //  Reference to object
int working_threads                     //  Not documented
);

int
    http_config_working_threads (
http_config_t * self                    //  Reference to self
);

int
    http_config_set_etag (
http_config_t * self,                   //  Reference to object
char * etag                             //  Not documented
);

char *
    http_config_etag (
http_config_t * self                    //  Reference to self
);

int
    http_config_dump_log (
http_config_t * self,                   //  Reference to object
smt_log_t * logfile                     //  Log file to dump to
);

void
    http_config_selftest (
void);

char *
    http_config_filename (
http_config_t * self                    //  Not documented
);

void
    http_config_set_filename (
http_config_t * self,                   //  Not documented
char * filename                         //  Not documented
);

ipr_config_t *
    http_config_table (
http_config_t * self                    //  Not documented
);

void
    http_config_terminate (
void);

#define http_config_show(item,opcode,trace_file)  http_config_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_config_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_config_destroy(self)       http_config_destroy_ (self, __FILE__, __LINE__)
void
    http_config_destroy_ (
http_config_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    http_config_read_lock (
http_config_t * self                    //  Reference to object
);

int
    http_config_write_lock (
http_config_t * self                    //  Reference to object
);

int
    http_config_unlock (
http_config_t * self                    //  Reference to object
);

void
    http_config_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_config_new_in_scope(self_p,_scope)  http_config_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    http_config_new_in_scope_ (
http_config_t * * self_p,               //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_HTTP_CONFIG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_CONFIG_INLINE)
#   define INCLUDE_HTTP_CONFIG_INLINE
#   define INCLUDE_HTTP_CONFIG_ACTIVE

#   include "smt_log.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#include "ipr.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_HTTP_CONFIG_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_CONFIG_ROOT
#   endif
# endif
#endif
