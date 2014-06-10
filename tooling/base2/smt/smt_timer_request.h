/*---------------------------------------------------------------------------
    smt_timer_request.h - smt_timer_request component

    The smt_timer_request class handles timing requests made either directly
    by application code or indirectly via socket requests with timeouts.
    Generated from smt_timer_request.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_TIMER_REQUEST_SAFE
#   define INCLUDE_SMT_TIMER_REQUEST_SAFE
#   define INCLUDE_SMT_TIMER_REQUEST_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_TIMER_REQUEST_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_timer_request_t smt_timer_request_t;

#define SMT_TIMER_REQUEST_ALIVE         0xFABB
#define SMT_TIMER_REQUEST_DEAD          0xDEAD

#define SMT_TIMER_REQUEST_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_timer_request\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_TIMER_REQUEST_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_timer_request at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_TIMER_REQUEST_ALIVE, self->object_tag);\
        smt_timer_request_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#include "base_apr.h"
#   ifdef __cplusplus
}
#   endif

#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "smt_timer_request_list.h"
#   include "smt_timer_request_by_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_TIMER_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_TIMER_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_TIMER_REQUEST_UNSAFE
#   define INCLUDE_SMT_TIMER_REQUEST_UNSAFE
#   define INCLUDE_SMT_TIMER_REQUEST_ACTIVE

#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_timer_request_animating;

//  Structure of the smt_timer_request object

struct _smt_timer_request_t {
smt_timer_request_t
    *local_next;
dbyte
    object_tag;                     //  Object validity marker
volatile smt_timer_request_list_t *list_head;
volatile smt_timer_request_t *list_prev;
volatile smt_timer_request_t *list_next;
volatile smt_timer_request_by_thread_t *by_thread_head;
volatile smt_timer_request_t *by_thread_prev;
volatile smt_timer_request_t *by_thread_next;
    smt_thread_t         *thread;       //  Thread to wake up                 
    smt_os_thread_t      *os_thread;    //  OS thread holding pollset          
    apr_time_t           time;          //  Expiry time                        
    smt_event_t          event;         //  Event to deliver                   
    smt_socket_request_t *socket_request;   //  Associated socket request      
#if !defined (BASE_THREADSAFE)
    Bool                 expired;       //  For timed SMT execution
#endif
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_timer_request_list.h"
#   include "smt_timer_request_by_thread.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    smt_timer_request_initialise (
void);

#define smt_timer_request_new(os_thread,time,event)  smt_timer_request_new_ (__FILE__, __LINE__, os_thread, time, event)
smt_timer_request_t *
    smt_timer_request_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_os_thread_t * os_thread,            //  OS thread managing this request
apr_time_t time,                        //  Time at which to deliver reply
smt_event_t event                       //  Event to deliver on alarm; SMT_NULL_EVENT for simple thread-blocking
);

smt_timer_request_t *
    smt_timer_request_wakeup (
smt_thread_t * thread,                  //  Thread to deliver reply to
apr_time_t time,                        //  Time at which to deliver reply
smt_event_t event                       //  Event to deliver on alarm; SMT_NULL_EVENT for simple thread-blocking
);

smt_timer_request_t *
    smt_timer_request_delay (
smt_thread_t * thread,                  //  Thread to deliver reply to
int64_t usecs,                          //  Delay timeout
smt_event_t event                       //  Event to deliver on timeout; SMT_NULL_EVENT for simple thread-blocking
);

Bool
    smt_timer_request_expire_all (
smt_os_thread_t * os_thread,            //  Not documented
apr_time_t time_now                     //  Not documented
);

apr_time_t
    smt_timer_request_first (
smt_os_thread_t * os_thread             //  Not documented
);

void
    smt_timer_request_selftest (
void);

void
    smt_timer_request_remove_from_all_containers (
smt_timer_request_t * self              //  The itemThe item
);

#define smt_timer_request_show(item,opcode,trace_file)  smt_timer_request_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_timer_request_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_timer_request_destroy(self)  smt_timer_request_destroy_ (self, __FILE__, __LINE__)
void
    smt_timer_request_destroy_ (
smt_timer_request_t * ( * self_p ),     //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    smt_timer_request_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_timer_request_new_in_scope(self_p,_scope,os_thread,time,event)  smt_timer_request_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, os_thread, time, event)
void
    smt_timer_request_new_in_scope_ (
smt_timer_request_t * * self_p,         //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_os_thread_t * os_thread,            //  OS thread managing this request
apr_time_t time,                        //  Time at which to deliver reply
smt_event_t event                       //  Event to deliver on alarm; SMT_NULL_EVENT for simple thread-blocking
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_TIMER_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_TIMER_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_TIMER_REQUEST_INLINE)
#   define INCLUDE_SMT_TIMER_REQUEST_INLINE
#   define INCLUDE_SMT_TIMER_REQUEST_ACTIVE

#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "smt_timer_request_list.h"
#   include "smt_timer_request_by_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_TIMER_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_TIMER_REQUEST_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_TIMER_REQUEST_ROOT
#   endif
# endif
#endif
