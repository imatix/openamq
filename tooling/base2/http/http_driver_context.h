/*---------------------------------------------------------------------------
    http_driver_context.h - http_driver_context component

    This class holds properties for driver control.  These properties are
    provided to and modified by the various driver modules.  The object is
    for local use by driver modules.
    Generated from http_driver_context.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_DRIVER_CONTEXT_SAFE
#   define INCLUDE_HTTP_DRIVER_CONTEXT_SAFE
#   define INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_DRIVER_CONTEXT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_driver_context_t http_driver_context_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
#    define HTTP_DRIVER_CONTEXT_HISTORY_LENGTH 32
#endif

#define HTTP_DRIVER_CONTEXT_ALIVE       0xFABB
#define HTTP_DRIVER_CONTEXT_DEAD        0xDEAD

#define HTTP_DRIVER_CONTEXT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_driver_context\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_DRIVER_CONTEXT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_driver_context at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_DRIVER_CONTEXT_ALIVE, self->object_tag);\
        http_driver_context_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_DRIVER_CONTEXT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_DRIVER_CONTEXT_UNSAFE
#   define INCLUDE_HTTP_DRIVER_CONTEXT_UNSAFE
#   define INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    http_driver_context_animating;

//  Structure of the http_driver_context object

struct _http_driver_context_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [HTTP_DRIVER_CONTEXT_HISTORY_LENGTH];
    int
        history_line  [HTTP_DRIVER_CONTEXT_HISTORY_LENGTH];
    char
        *history_type [HTTP_DRIVER_CONTEXT_HISTORY_LENGTH];
    int
        history_links [HTTP_DRIVER_CONTEXT_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;                        //  Calling http_agent thread
http_request_t
    *request;                       //  HTTP request object
http_response_t
    *response;                      //  HTTP response object
smt_log_t
    *access_log,                    //  Access log file
    *alert_log,                     //  Alert log file
    *debug_log;                     //  Debug log file
Bool
    failed,                         //  Did request result in error?
    replied,                        //  Don't sent multiple replies
    pending;                        //  Did void reply
ipr_xml_t
    *xml_tree,                      //  Parsed XML content, if any
    *xml_root,                      //  Document root (real XML root)
    *xml_item;                      //  First element if any
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_driver_context_new(thread,server,request,response)  http_driver_context_new_ (__FILE__, __LINE__, thread, server, request, response)
http_driver_context_t *
    http_driver_context_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_thread_t * thread,                  //  Not documented
http_server_t * server,                 //  Not documented
http_request_t * request,               //  Not documented
http_response_t * response              //  Not documented
);

int
    http_driver_context_init (
http_driver_context_t * self,           //  Reference to object
http_request_t * request,               //  Not documented
http_response_t * response              //  Not documented
);

int
    http_driver_context_reply_error (
http_driver_context_t * self,           //  Reference to object
uint reply_code,                        //  Reply code
char * reply_text,                      //  Reply text
...                                     //  Variable arguments
);

int
    http_driver_context_reply_error_v (
http_driver_context_t * self,           //  Reference to object
uint reply_code,                        //  Reply code
char * reply_text,                      //  Reply text
va_list args                            //  Variable arguments
);

int
    http_driver_context_reply_success (
http_driver_context_t * self,           //  Reference to object
uint reply_code                         //  Reply code
);

int
    http_driver_context_reply_void (
http_driver_context_t * self            //  Reference to object
);

int
    http_driver_context_reply (
http_driver_context_t * self            //  Reference to object
);

int
    http_driver_context_abort (
http_driver_context_t * self            //  Reference to object
);

int
    http_driver_context_xml_parse (
http_driver_context_t * self,           //  Reference to object
char * root,                            //  Default root, or null
char * item                             //  Default item, or null
);

void
    http_driver_context_selftest (
void);

void
    http_driver_context_terminate (
void);

#define http_driver_context_show(item,opcode,trace_file)  http_driver_context_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_driver_context_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_driver_context_destroy(self)  http_driver_context_destroy_ (self, __FILE__, __LINE__)
void
    http_driver_context_destroy_ (
http_driver_context_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define http_driver_context_link(self)  http_driver_context_link_ (self, __FILE__, __LINE__)
http_driver_context_t *
    http_driver_context_link_ (
http_driver_context_t * self,           //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_driver_context_unlink(self)  http_driver_context_unlink_ (self, __FILE__, __LINE__)
void
    http_driver_context_unlink_ (
http_driver_context_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_driver_context_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_driver_context_new_in_scope(self_p,_scope,thread,server,request,response)  http_driver_context_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, thread, server, request, response)
void
    http_driver_context_new_in_scope_ (
http_driver_context_t * * self_p,       //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_thread_t * thread,                  //  Not documented
http_server_t * server,                 //  Not documented
http_request_t * request,               //  Not documented
http_response_t * response              //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_DRIVER_CONTEXT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_DRIVER_CONTEXT_INLINE)
#   define INCLUDE_HTTP_DRIVER_CONTEXT_INLINE
#   define INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_DRIVER_CONTEXT_ACTIVE
#   if defined (ICL_IMPORT_HTTP_DRIVER_CONTEXT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_DRIVER_CONTEXT_ROOT
#   endif
# endif
#endif
