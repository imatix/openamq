/*---------------------------------------------------------------------------
    http_server.h - http_server component

    This implements the HTTP server object, which is the top-level object
    holding server-wide values and owning server-wide structures.
    Generated from http_server.icl by smt_object_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_SERVER_SAFE
#   define INCLUDE_HTTP_SERVER_SAFE
#   define INCLUDE_HTTP_SERVER_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_SERVER_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_server_t http_server_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
#    define HTTP_SERVER_HISTORY_LENGTH 32
#endif

#define HTTP_SERVER_ALIVE               0xFABB
#define HTTP_SERVER_DEAD                0xDEAD

#define HTTP_SERVER_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_server\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_SERVER_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_server at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_SERVER_ALIVE, self->object_tag);\
        http_server_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl.h"
#   include "ipr.h"
#   include "smt.h"
#   include "http.h"
#   include "http_access_module.h"
#   include "ipr_looseref.h"
#   include "http_driver_module.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_server_agent.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_SERVER_ACTIVE
#   if defined (ICL_IMPORT_HTTP_SERVER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_SERVER_UNSAFE
#   define INCLUDE_HTTP_SERVER_UNSAFE
#   define INCLUDE_HTTP_SERVER_ACTIVE

#   include "icl.h"
#   include "ipr.h"
#   include "smt.h"
#   include "http.h"
#   include "http_access_module.h"
#   include "ipr_looseref.h"
#   include "http_driver_module.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_server_agent.h"
//  Global variables

extern Bool
    http_server_animating;
extern http_server_t
    *http_server;                       //  Single server, self

//  Structure of the http_server object

struct _http_server_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [HTTP_SERVER_HISTORY_LENGTH];
    int
        history_line  [HTTP_SERVER_HISTORY_LENGTH];
    char
        *history_type [HTTP_SERVER_HISTORY_LENGTH];
    int
        history_links [HTTP_SERVER_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;
ipr_looseref_list_t
    *http_driver_module_list,       //  List of all own portals
    *http_driver_module_factory_list;  //  List of all own portal factories
Bool
    http_driver_module_stopped;     //  Is this portal stopped?
ipr_looseref_list_t
    *http_access_module_list,       //  List of all own portals
    *http_access_module_factory_list;  //  List of all own portal factories
Bool
    http_access_module_stopped;     //  Is this portal stopped?
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
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_server_new()               http_server_new_ (__FILE__, __LINE__)
http_server_t *
    http_server_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    http_server_start (
http_server_t * self                    //  Reference to object
);

int
    http_server_ban_address (
http_server_t * self,                   //  Reference to object
char * address                          //  Not documented
);

int
    http_server_sync_hashes (
http_server_t * self                    //  Reference to object
);

int
    http_server_new_password (
http_server_t * self,                   //  Reference to object
http_access_module_t * portal,          //  Portal back to server
char * key,                             //  Not documented
char * hash,                            //  Not documented
Bool digest                             //  Not documented
);

void
    http_server_selftest (
void);

int
    http_server__http_access_module_bind (
http_server_t * self,                   //  Reference to object
http_access_module_t * http_access_module  //  Portal reference
);

int
    http_server__http_access_module_unbind (
http_server_t * self,                   //  Reference to object
http_access_module_t * portal           //  Portal reference
);

int
    http_server__http_driver_module_bind (
http_server_t * self,                   //  Reference to object
http_driver_module_t * http_driver_module  //  Portal reference
);

int
    http_server__http_driver_module_unbind (
http_server_t * self,                   //  Reference to object
http_driver_module_t * portal           //  Portal reference
);

#define http_server_destroy(self)       http_server_destroy_ (self, __FILE__, __LINE__)
int
    http_server_destroy_ (
http_server_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_server_unlink(self)        http_server_unlink_ (self, __FILE__, __LINE__)
void
    http_server_unlink_ (
http_server_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_server_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_server_free(self)          http_server_free_ (self, __FILE__, __LINE__)
void
    http_server_free_ (
http_server_t * self,                   //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    http_server_terminate (
void);

#define http_server_show(item,opcode,trace_file)  http_server_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_server_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_server_link(self)          http_server_link_ (self, __FILE__, __LINE__)
http_server_t *
    http_server_link_ (
http_server_t * self,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_server_new_in_scope(self_p,_scope)  http_server_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    http_server_new_in_scope_ (
http_server_t * * self_p,               //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_SERVER_ACTIVE
#   if defined (ICL_IMPORT_HTTP_SERVER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_SERVER_INLINE)
#   define INCLUDE_HTTP_SERVER_INLINE
#   define INCLUDE_HTTP_SERVER_ACTIVE

#   include "icl.h"
#   include "ipr.h"
#   include "smt.h"
#   include "http.h"
#   include "http_access_module.h"
#   include "ipr_looseref.h"
#   include "http_driver_module.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_server_agent.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_SERVER_ACTIVE
#   if defined (ICL_IMPORT_HTTP_SERVER_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_SERVER_ROOT
#   endif
# endif
#endif
