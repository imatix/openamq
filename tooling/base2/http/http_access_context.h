/*---------------------------------------------------------------------------
    http_access_context.h - http_access_context component

    This class holds properties for access control.  These properties are
    provided to and modified by the various access modules.  The object is
    for local use by access modules.
    Generated from http_access_context.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_ACCESS_CONTEXT_SAFE
#   define INCLUDE_HTTP_ACCESS_CONTEXT_SAFE
#   define INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_ACCESS_CONTEXT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_access_context_t http_access_context_t;

#define HTTP_ACCESS_CONTEXT_ALIVE       0xFABB
#define HTTP_ACCESS_CONTEXT_DEAD        0xDEAD

#define HTTP_ACCESS_CONTEXT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_access_context\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_ACCESS_CONTEXT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_access_context at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_ACCESS_CONTEXT_ALIVE, self->object_tag);\
        http_access_context_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_CONTEXT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_ACCESS_CONTEXT_UNSAFE
#   define INCLUDE_HTTP_ACCESS_CONTEXT_UNSAFE
#   define INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    http_access_context_animating;

//  Structure of the http_access_context object

struct _http_access_context_t {
dbyte
    object_tag;                     //  Object validity marker
//  This is the thread of the client connection
smt_thread_t
    *thread;                        //  Client thread in http_agent.smt
//  This is the IP address of the client (could be a proxy)
char
    *address;                       //  Dotted numeric notation
//  The request and response refer to the two main objects holding the parsed
//  HTTP request, and prepared response data.
http_request_t
    *request;                       //  HTTP request object
http_response_t
    *response;                      //  HTTP response object
//  These properties are set by the authentication code, copying from the
//  relevant password table.
char
    *fail_user,                     //  If authentication failed
    *fail_realm,                    //  If known
    *auth_user,                     //  If authentication succeeded
    *auth_group;                    //  User group, or "users"
int
    fail_code;                      //  IPR_PASSWORD_xxx value
//  This is true if we did a Digest authentication
Bool
    digest_auth;                    //  Did we try Digest auth?
//  Links to the two server password tables
ipr_password_table_t
    *basic,                         //  Basic authentication data
    *digest;                        //  Digest authentication data
//  Links to the server's log files
smt_log_t
    *access_log,                    //  Access log file
    *alert_log,                     //  Alert log file
    *debug_log;                     //  Debug log file
//  Links to the server's blacklist table
ipr_dict_table_t
    *blacklist;                     //  Banned IP addresses
//  Counts the number of retries
size_t
    retries;                        //  How many retries so far
//  If an access module needs to run an external process it can put the
//  handle here, and then recheck that the process is not still running
//  in a retry loop.
ipr_process_t
    *process;                       //  External process handle
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_access_context_new(server)  http_access_context_new_ (__FILE__, __LINE__, server)
http_access_context_t *
    http_access_context_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server                  //  Not documented
);

int
    http_access_context_set_thread (
http_access_context_t * self,           //  Reference to object
smt_thread_t * thread                   //  Not documented
);

int
    http_access_context_set_address (
http_access_context_t * self,           //  Reference to object
char * address                          //  Not documented
);

int
    http_access_context_set_request (
http_access_context_t * self,           //  Reference to object
http_request_t * request                //  Not documented
);

int
    http_access_context_set_response (
http_access_context_t * self,           //  Reference to object
http_response_t * response              //  Not documented
);

int
    http_access_context_set_auth (
http_access_context_t * self,           //  Reference to object
ipr_password_table_t * table,           //  Not documented
char * group                            //  Not documented
);

int
    http_access_context_set_fail (
http_access_context_t * self,           //  Reference to object
ipr_password_table_t * table            //  Not documented
);

char *
    http_access_context_format (
http_access_context_t * self,           //  Not documented
char * format                           //  Not documented
);

void
    http_access_context_selftest (
void);

void
    http_access_context_terminate (
void);

#define http_access_context_show(item,opcode,trace_file)  http_access_context_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_access_context_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_access_context_destroy(self)  http_access_context_destroy_ (self, __FILE__, __LINE__)
void
    http_access_context_destroy_ (
http_access_context_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    http_access_context_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_access_context_new_in_scope(self_p,_scope,server)  http_access_context_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, server)
void
    http_access_context_new_in_scope_ (
http_access_context_t * * self_p,       //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server                  //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_CONTEXT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_ACCESS_CONTEXT_INLINE)
#   define INCLUDE_HTTP_ACCESS_CONTEXT_INLINE
#   define INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_CONTEXT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_ACCESS_CONTEXT_ROOT
#   endif
# endif
#endif
