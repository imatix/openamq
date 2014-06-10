/*---------------------------------------------------------------------------
    smt_os_thread.h - smt_os_thread component

    The smt_os_thread class manipulates OS threads for SMT.

    It is for SMT internal use only.
    Generated from smt_os_thread.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_OS_THREAD_SAFE
#   define INCLUDE_SMT_OS_THREAD_SAFE
#   define INCLUDE_SMT_OS_THREAD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_OS_THREAD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_os_thread_t smt_os_thread_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
#    define SMT_OS_THREAD_HISTORY_LENGTH 32
#endif

#define SMT_OS_THREAD_ALIVE             0xFABB
#define SMT_OS_THREAD_DEAD              0xDEAD

#define SMT_OS_THREAD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_os_thread\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_OS_THREAD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_os_thread at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_OS_THREAD_ALIVE, self->object_tag);\
        smt_os_thread_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#include "base_apr.h"
//  Scheduling macros

#define SMT_SCHEDULE_NORMAL     0
#define SMT_SCHEDULE_POLL       1

//  Types

typedef qbyte smt_poll_tag_t;

//  OS thread status

#define SMT_OS_THREAD_NEWBORN   0       //  Obvious
#define SMT_OS_THREAD_SLEEPING  1       //  Idle
#define SMT_OS_THREAD_POLLING   2       //  Waiting on socket activity
#define SMT_OS_THREAD_ACTIVE    3       //  Busy
#define SMT_OS_THREAD_ZOMBIE    4       //  Dead
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_os_thread_by_status.h"
#   include "smt_os_thread_list.h"
#   include "icl_system.h"
#   include "icl_mem.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_OS_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_OS_THREAD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_OS_THREAD_UNSAFE
#   define INCLUDE_SMT_OS_THREAD_UNSAFE
#   define INCLUDE_SMT_OS_THREAD_ACTIVE

#   include "smt_thread.h"
#   include "icl_system.h"
#   include "icl_mem.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_os_thread_animating;
//  Global variables

extern volatile qbyte
    smt_signal_raised;                  //  Set in handler
extern Bool
    smt_break_requested;                //  Interrupt SMT operation?
extern volatile qbyte
    smt_actions_remaining;              //  Action count for debugging
extern smt_os_thread_t
    *smt_os_thread;                     //  One of the threads

//  Structure of the smt_os_thread object

struct _smt_os_thread_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [SMT_OS_THREAD_HISTORY_LENGTH];
    int
        history_line  [SMT_OS_THREAD_HISTORY_LENGTH];
    char
        *history_type [SMT_OS_THREAD_HISTORY_LENGTH];
    int
        history_links [SMT_OS_THREAD_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
    apr_pool_t          *pool;
#if (defined (BASE_THREADSAFE))
    apr_threadattr_t    *threadattr;
    apr_thread_t        *thread;
#endif
    char * file;
    size_t line;
    Bool   started;
    apr_thread_start_t startup;
volatile smt_os_thread_by_status_t *by_status_head;
volatile smt_os_thread_t *by_status_prev;
volatile smt_os_thread_t *by_status_next;
volatile smt_os_thread_list_t *list_head;
volatile smt_os_thread_t *list_prev;
volatile smt_os_thread_t *list_next;
#if (defined (BASE_THREADSAFE))
    apr_os_thread_t
        apr_os_thread;                  //  System dependent OS thread id
#endif
    Bool
        shutting_down;                  //  Thread is shutting down            
    Bool
        shutdown_request;               //  Signal thread to shut down
    Bool
        terminate_request;              //  Signal thread to terminate
    int
        socket_request_count,
        timer_request_count;
    smt_timer_request_list_t
        *timer_request_list;
    smt_poll_tag_t
        last_poll_tag;                  //  Incremented at each socket poll  
    apr_pollset_t
        *pollset,
        *waking_pollset;
    volatile qbyte
        status;                         //  OS thread status
    smt_thread_list_t
        *thread_list;                   //  All threadlets for this OS thread

//  The dynamic lists and queues of threadlets.  There are four of these:
//      1. waiting_list:  private (non-threadsafe) list of threadlets that are
//                        waiting to be serviced; pointers are maintained so
//                        that list can be maintained sorted by priority.
//      2. waking_list:   private (non-threadsafe) list of threadlets that are
//                        are waiting to be serviced but have socket handles
//                        to be checked first.
//      3. thread_queue:  threadsafe queue of threadlets that have been woken
//                        from another thread.
    smt_thread_by_status_t
        *waiting_list,                  //  Waiting threadlets
        *waking_list;                   //  Waking threadlets
    smt_thread_t
        *waiting_list_entry             //  Insertion point by priority
            [SMT_PRIORITY_LAST];
#if (defined (BASE_THREADSAFE))
    smt_thread_queue_t
        *thread_queue;                  //  Waiting & waking threadlets
    smt_os_thread_by_status_t
        *sleeping_queue;                //  The queue where this thread sleeps
#    if (defined (__WINDOWS__))
    apr_socket_t
        *sock_out,
        *sock_in;
#    else
    apr_file_t
        *pipe_in,                       //  Pipe is used to wake up            
        *pipe_out;                      //  a sleeping thread.                 
#    endif
//  Condition variable and mutex for thread synchronisation when no socket
//  requests.
    icl_cond_t
        *cond;
    icl_mutex_t
        *mutex;
    Bool
        waiting;
#endif
#if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    icl_stats_t
        *thread_count_stat,
        *state_wake_stat,
        *state_poll_stat,
        *state_sleep_stat,
        *state_active_stat;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_os_thread_by_status.h"
#   include "smt_os_thread_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline void
    smt_os_thread_remove_thread (
smt_thread_t * thread                   //  Not documented
);

void
    smt_request_break (
void);

void
    smt_shut_down (
void);

int
    smt_wait (
qbyte msecs                             //  Timeout for wait
);

#define smt_os_thread_wake(self)        smt_os_thread_wake_ (self, __FILE__, __LINE__)
int
    smt_os_thread_wake_ (
smt_os_thread_t * self,                 //  Reference to object
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

smt_os_thread_t *
    smt_os_thread_assign (
smt_thread_t * thread,                  //  The threadlet to assign
smt_os_thread_t * self,                 //  OS thread to assign to
uint schedule                           //  The sleeping thread list index
);

void
    smt_os_thread_selftest (
void);

void
    smt_set_polling_threads (
int number                              //  Number of polling OS threads
);

void
    smt_set_working_threads (
int number                              //  Number of working OS threads
);

void
    smt_set_action_limit (
int action_limit                        //  Number of actions before break
);

apr_time_t
    smt_time_now (
void);

void
    smt_initialise (
void);

void
    smt_terminate (
void);

void
    smt_os_thread_remove_from_all_containers (
smt_os_thread_t * self                  //  The itemThe item
);

#define smt_os_thread_show(item,opcode,trace_file)  smt_os_thread_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_os_thread_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

int
    smt_os_thread_join (
smt_os_thread_t * self                  //  Reference to object
);

#define smt_os_thread_destroy(self)     smt_os_thread_destroy_ (self, __FILE__, __LINE__)
void
    smt_os_thread_destroy_ (
smt_os_thread_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define smt_os_thread_link(self)        smt_os_thread_link_ (self, __FILE__, __LINE__)
smt_os_thread_t *
    smt_os_thread_link_ (
smt_os_thread_t * self,                 //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_os_thread_unlink(self)      smt_os_thread_unlink_ (self, __FILE__, __LINE__)
void
    smt_os_thread_unlink_ (
smt_os_thread_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_os_thread_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_OS_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_OS_THREAD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_OS_THREAD_INLINE)
#   define INCLUDE_SMT_OS_THREAD_INLINE
#   define INCLUDE_SMT_OS_THREAD_ACTIVE

#   include "smt_thread.h"
#   include "smt_os_thread_by_status.h"
#   include "smt_os_thread_list.h"
#   include "icl_system.h"
#   include "icl_mem.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    smt_os_thread_remove_thread

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_os_thread_remove_thread (
    smt_thread_t * thread               //  Not documented
)
{

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    if (thread->os_thread)
        icl_stats_dec (NULL, &thread->os_thread->thread_count_stat);
#    endif
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_OS_THREAD_ACTIVE
#   if defined (ICL_IMPORT_SMT_OS_THREAD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_OS_THREAD_ROOT
#   endif
# endif
#endif
