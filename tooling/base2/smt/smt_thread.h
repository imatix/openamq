/*---------------------------------------------------------------------------
    smt_thread.h - smt_thread component

    The smt_thread class manipulates SMT threads.
    For SMT internal use only.
    Generated from smt_thread.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_THREAD_SAFE
#   define INCLUDE_SMT_THREAD_SAFE
#   define INCLUDE_SMT_THREAD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_THREAD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_thread_t smt_thread_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD)
#    define SMT_THREAD_HISTORY_LENGTH 32
#endif

#define SMT_THREAD_ALIVE                0xFABB
#define SMT_THREAD_DEAD                 0xDEAD

#define SMT_THREAD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_thread\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_THREAD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_thread at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_THREAD_ALIVE, self->object_tag);\
        smt_thread_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
//  Macros                                                                     

#define SMT_NULL_STATE       0          //  Thread not initialised

#define SMT_NULL_EVENT       0          //  No event provided
#define SMT_TERM_EVENT      -1          //  Terminate the thread               
#define SMT_TERM_PLACE      (qbyte) -1  //  Last place
#define SMT_TERM_MODULE     (qbyte) -1  //  End of module list

//  Return codes

#define SMT_OK                0
#define SMT_THREAD_ERROR     -1
#define SMT_SLEEPING         -2
#define SMT_FINISHED         -3
#define SMT_BREAK            -4
#define SMT_EMPTY            -5
#define SMT_ILLEGAL_ARGUMENT  1
#define SMT_SELECT_ERROR      2
#define SMT_SOCKET_ERROR      3
#define SMT_TIMEOUT           4
#define SMT_THREAD_INVALID    5
#define SMT_FILEIO_ERROR      6
#define SMT_SOCKET_CLOSING    7

//  Priorities, ordered highest to lowest.
//  JS: I might change the ordering of priorities to make it more intuitive,
//      but the code is simpler with the reverse ordering.

#define SMT_PRIORITY_LAST    SMT_PRIORITY_NORMAL    //  Last defined priority

typedef enum {
    SMT_PRIORITY_HIGH,
    SMT_PRIORITY_NORMAL
} smt_priority_t;

//  Types                                                                      

//  Thread status

#define SMT_THREAD_NEWBORN   0          //  Thread just created
#define SMT_THREAD_SLEEPING  1          //  Thread with nothing happening
#define SMT_THREAD_WAITING   2          //  Thread waiting for attention
#define SMT_THREAD_WAKING    3          //  Thread with monitor and response
#define SMT_THREAD_POLLING   4          //  Thread polling for response
#define SMT_THREAD_PENDING   5          //  Thread asleep pending response
#define SMT_THREAD_ACTIVE    6          //  Thread receiving attention
#define SMT_THREAD_ZOMBIE    7          //  Thread closed down

typedef qbyte smt_thread_id_t;
typedef qbyte smt_state_t;
typedef qbyte smt_place_t;
typedef qbyte smt_event_t;
typedef qbyte smt_module_t;

typedef int (smt_thread_p_fn)  (smt_thread_t **thread);
typedef int (smt_thread_fn)    (smt_thread_t  *thread);

typedef smt_thread_t * (smt_agent_thread_new_fn)  (void);
#   ifdef __cplusplus
}
#   endif

#   include "icl_trace.h"
#   include "smt_os_thread.h"
#   include "smt_position.h"
#   include "smt_method.h"
#   include "smt_timer_request.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "smt_signal.h"
#   include "smt_thread_by_status.h"
#   include "smt_thread_queue.h"
#   include "smt_thread_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_THREAD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_THREAD_UNSAFE
#   define INCLUDE_SMT_THREAD_UNSAFE
#   define INCLUDE_SMT_THREAD_ACTIVE

#   include "icl_trace.h"
#   include "smt_os_thread.h"
#   include "smt_position.h"
#   include "smt_method.h"
#   include "smt_timer_request.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "smt_signal.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_thread_animating;

//  Structure of the smt_thread object

struct _smt_thread_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [SMT_THREAD_HISTORY_LENGTH];
    int
        history_line  [SMT_THREAD_HISTORY_LENGTH];
    char
        *history_type [SMT_THREAD_HISTORY_LENGTH];
    int
        history_links [SMT_THREAD_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile smt_thread_by_status_t *by_status_head;
volatile smt_thread_t *by_status_prev;
volatile smt_thread_t *by_status_next;
volatile smt_thread_queue_t *queue_head;
volatile smt_thread_t *queue_prev;
volatile smt_thread_t *queue_next;
volatile smt_thread_list_t *list_head;
volatile smt_thread_t *list_prev;
volatile smt_thread_t *list_next;
    smt_os_thread_t      *os_thread;        //  OS thread running this thread  
    smt_thread_p_fn      *manager;          //  Agent manager function         
    smt_thread_fn        *catcher;          //  Agent error catch function     
    smt_thread_fn        *destructor;       //  Agent error catch function     
    smt_thread_id_t      id;                //  Thread ID
    smt_priority_t       priority;          //  Thread priority
    smt_poll_tag_t       poll_tag;          //  Poll tag at last thread service
    smt_place_t          place;             //  Current location in dialog     
    smt_module_t         module;            //  Index in module list
    smt_method_t         *method;           //  Last method received
    smt_event_t          _next_event;       //  Next event                     
    smt_event_t          _exception_event;  //  Exception event                
    Bool                 _exception_raised; //  Did an exception occur?        
    Bool                 animate;           //  Animation enabled?             
    Bool                 shutting_down;     //  Thread is shutting down        
    Bool                 shutdown_request;  //  Request thread to shut down
    smt_signal_by_thread_t
                         *signal_list;      //  Signal registered with thread
    volatile Bool        signal_raised;     //  Set when a signal was sent
    smt_event_t          signal_event;      //  Event to send on signal
    volatile qbyte       status;            //  Thread status 
    int                  schedule;          //  Scheduler for this thread
    void                 *context;          //  Thread-specific data           
    int                  result;            //  Result of last operation       
    int                  error;             //  System error number on error   
    smt_socket_request_t *monitor;          //  Monitor socket request         
    qbyte                pending_count;     //  No. io/timer op'ns pending     
    smt_method_queue_t   *reply_queue;      //  Queued replies for thread
    smt_method_list_t    *reply_list;       //  Local sorted list of replies
    volatile qbyte        reply_backlog;    //  Local count of reply backlog
    smt_position_list_t  *position_stack;   //  Stack of dialog positions
    smt_thread_t         *watched_thread;   //  Thread we are watching
    smt_thread_t         *watching_thread;  //  Thread watching us
    smt_timer_request_by_thread_t
                         *timer_request_list;   //  List of timer requests     
    smt_socket_request_by_thread_t
                         *socket_request_list;  //  List of socket requests    
    smt_socket_request_t
                         *write_request;    //  To optimise on polling
    const char           **event_name;      //  Event names for debugging
    icl_trace_t          *trace;
#if (defined (BASE_STATS) || defined (BASE_STATS_SMT_THREAD))
    icl_stats_t
        *method_same_os_thread_stat,
        *method_other_os_thread_stat;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_thread_by_status.h"
#   include "smt_thread_queue.h"
#   include "smt_thread_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define smt_thread_new(manager,catcher,destructor)  smt_thread_new_ (__FILE__, __LINE__, manager, catcher, destructor)
smt_thread_t *
    smt_thread_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_thread_p_fn * manager,              //  The thread's manager function
smt_thread_fn * catcher,                //  The thread's catcher function
smt_thread_fn * destructor              //  The thread's destructor function
);

int
    smt_thread_share (
smt_thread_t * self,                    //  Reference to object
smt_thread_t * share                    //  Not documented
);

int
    smt_thread_watch (
smt_thread_t * self,                    //  Reference to object
smt_thread_t * watch                    //  Not documented
);

#define smt_thread_show(item,opcode,trace_file)  smt_thread_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_thread_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

int
    smt_thread_clear_method_queue (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_method_accept (
smt_thread_t * self                     //  Reference to object
);

qbyte
    smt_thread_reply_backlog (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_insert_into_waiting_list (
smt_thread_t * self                     //  Reference to object
);

smt_thread_t *
    smt_thread_waiting_list_pop (
smt_os_thread_t * os_thread             //  The OS thread
);

int
    smt_thread_wake (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_activate (
smt_thread_t * self                     //  Reference to object
);

static inline int
    smt_thread_activity (
smt_thread_t * self                     //  Reference to object
);

void
    smt_thread_continue (
smt_thread_t * self                     //  Not documented
);

int
    smt_thread_purge (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_shut_down (
smt_thread_t * self                     //  Reference to object
);

void
    smt_thread_shut_down_all_newborn (
void);

void
    smt_thread_position_push (
smt_thread_t * self                     //  Not documented
);

int
    smt_thread_position_pop (
smt_thread_t * self                     //  Not documented
);

void
    smt_thread_set_next_event (
smt_thread_t * self,                    //  Not documented
smt_event_t event                       //  Not documented
);

void
    smt_thread_raise_exception (
smt_thread_t * self,                    //  Not documented
smt_event_t event                       //  Not documented
);

int
    smt_thread_exception_raised (
smt_thread_t * self                     //  Reference to object
);

char *
    smt_thread_error (
smt_thread_t * self                     //  Not documented
);

void
    smt_thread_selftest (
void);

void
    smt_thread_initialise (
void);

void
    smt_thread_remove_from_all_containers (
smt_thread_t * self                     //  The itemThe itemThe item
);

#define smt_thread_destroy(self)        smt_thread_destroy_ (self, __FILE__, __LINE__)
void
    smt_thread_destroy_ (
smt_thread_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    smt_thread_read_lock (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_write_lock (
smt_thread_t * self                     //  Reference to object
);

int
    smt_thread_unlock (
smt_thread_t * self                     //  Reference to object
);

#define smt_thread_link(self)           smt_thread_link_ (self, __FILE__, __LINE__)
smt_thread_t *
    smt_thread_link_ (
smt_thread_t * self,                    //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_thread_unlink(self)         smt_thread_unlink_ (self, __FILE__, __LINE__)
void
    smt_thread_unlink_ (
smt_thread_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_thread_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_thread_new_in_scope(self_p,_scope,manager,catcher,destructor)  smt_thread_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, manager, catcher, destructor)
void
    smt_thread_new_in_scope_ (
smt_thread_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_thread_p_fn * manager,              //  The thread's manager function
smt_thread_fn * catcher,                //  The thread's catcher function
smt_thread_fn * destructor              //  The thread's destructor function
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_THREAD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_THREAD_INLINE)
#   define INCLUDE_SMT_THREAD_INLINE
#   define INCLUDE_SMT_THREAD_ACTIVE

#   include "icl_trace.h"
#   include "smt_os_thread.h"
#   include "smt_position.h"
#   include "smt_method.h"
#   include "smt_timer_request.h"
#   include "smt_socket.h"
#   include "smt_socket_request.h"
#   include "smt_signal.h"
#   include "smt_thread_by_status.h"
#   include "smt_thread_queue.h"
#   include "smt_thread_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    smt_thread_activity

    Type: Component method
    Accepts a smt_thread_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    smt_thread_activity (
    smt_thread_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

#if (defined (BASE_TRACE_SMT_THREAD)   || defined (BASE_TRACE_SMT_THREAD_ACTIVITY))
    icl_trace_record (self?self->trace:NULL, smt_thread_dump, 14);
#endif

SMT_THREAD_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->_exception_raised                   //  Exception was raised               
   || self->shutdown_request                    //  Shutdown was requested             
   || self->signal_raised                       //  Signal was raised
   || self->_next_event != SMT_NULL_EVENT       //  Already have an event      
   || self->module != SMT_TERM_MODULE           //  Inside action list
   || !smt_method_list_isempty  (self->reply_list)      //  Incoming
   || !smt_method_queue_isempty (self->reply_queue));   //  Incoming
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_TRACE_SMT_THREAD)   || defined (BASE_TRACE_SMT_THREAD_ACTIVITY))
    icl_trace_record (self?self->trace:NULL, smt_thread_dump, 0x10000 + 14);
#endif


    return (rc);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_THREAD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_THREAD_ROOT
#   endif
# endif
#endif
