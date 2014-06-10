/*---------------------------------------------------------------------------
    smt_log_agent.c - functions for smt_log_agent.

    Generated from smt_log_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "ipr.h"                        //  For iPR classes
#include "smt.h"                        //  For SMT classes
#include "smt_log_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define destroy_m ((smt_log_agent_destroy_t *) thread->method->data)

#define open_m ((smt_log_agent_open_t *) thread->method->data)

#define set_syslog_m ((smt_log_agent_set_syslog_t *) thread->method->data)

#define write_bucket_m ((smt_log_agent_write_bucket_t *) thread->method->data)

enum {
    shutdown_event                                 = 1,
    destroy_m_event                                = 2,
    open_m_event                                   = 3,
    close_m_event                                  = 4,
    set_syslog_m_event                             = 5,
    write_bucket_m_event                           = 6,
    cycle_m_event                                  = 7,
};

#define laststate                                  2
#define lastevent                                  7
#define lastplace                                  17

#define smt_log_agent_agent_name                   "Smt-Log-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define shutdown_event_name                        "Shutdown"
#define destroy_m_event_name                       "Destroy-M"
#define open_m_event_name                          "Open-M"
#define close_m_event_name                         "Close-M"
#define set_syslog_m_event_name                    "Set-Syslog-M"
#define write_bucket_m_event_name                  "Write-Bucket-M"
#define cycle_m_event_name                         "Cycle-M"

#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_3_action_name                      "Unnamed-3"
#define unnamed_4_action_name                      "Unnamed-4"
#define open_action_name                           "Open"
#define unnamed_6_action_name                      "Unnamed-6"
#define close_action_name                          "Close"
#define unnamed_8_action_name                      "Unnamed-8"
#define set_syslog_action_name                     "Set-Syslog"
#define unnamed_10_action_name                     "Unnamed-10"
#define write_bucket_action_name                   "Write-Bucket"
#define unnamed_12_action_name                     "Unnamed-12"
#define cycle_action_name                          "Cycle"
#define unnamed_14_action_name                     "Unnamed-14"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    char *         file;
    size_t         line;
  }
smt_log_agent_destroy_t;

typedef struct
  {
    char *         filename;
    char *         log_path;
    char *         cycle_cmd;
  }
smt_log_agent_open_t;

typedef struct
  {
    int            priority;
  }
smt_log_agent_set_syslog_t;

typedef struct
  {
    ipr_bucket_t *  bucket;
  }
smt_log_agent_write_bucket_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        smt_log_t
            *smt_log;
    
} smt_log_agent_class_context_t;

static icl_cache_t
    *s_destroy_cache = NULL;
static icl_cache_t
    *s_open_cache = NULL;
static icl_cache_t
    *s_set_syslog_cache = NULL;
static icl_cache_t
    *s_write_bucket_cache = NULL;

static const char *thread_name [] = {
    "<NULL>",
    class_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    main_state_name,
    shutting_down_state_name
};

static const char *event_name [] = {
    "<NULL>",
    shutdown_event_name,
    destroy_m_event_name,
    open_m_event_name,
    close_m_event_name,
    set_syslog_m_event_name,
    write_bucket_m_event_name,
    cycle_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT)            ||  defined (BASE_TRACE)  || defined (BASE_TRACE_SMT_LOG_AGENT)   ||  defined (SMT_LOG_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    destroy_action_name,
    free_action_name,
    unnamed_3_action_name,
    unnamed_4_action_name,
    open_action_name,
    unnamed_6_action_name,
    close_action_name,
    unnamed_8_action_name,
    set_syslog_action_name,
    unnamed_10_action_name,
    write_bucket_action_name,
    unnamed_12_action_name,
    cycle_action_name,
    unnamed_14_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_DESTROY))
static icl_stats_t *s_smt_log_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_FREE))
static icl_stats_t *s_smt_log_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_3))
static icl_stats_t *s_smt_log_agent_unnamed_3_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_4))
static icl_stats_t *s_smt_log_agent_unnamed_4_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_OPEN))
static icl_stats_t *s_smt_log_agent_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_6))
static icl_stats_t *s_smt_log_agent_unnamed_6_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_CLOSE))
static icl_stats_t *s_smt_log_agent_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_8))
static icl_stats_t *s_smt_log_agent_unnamed_8_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_SET_SYSLOG))
static icl_stats_t *s_smt_log_agent_set_syslog_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_10))
static icl_stats_t *s_smt_log_agent_unnamed_10_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_WRITE_BUCKET))
static icl_stats_t *s_smt_log_agent_write_bucket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_12))
static icl_stats_t *s_smt_log_agent_unnamed_12_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_CYCLE))
static icl_stats_t *s_smt_log_agent_cycle_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_14))
static icl_stats_t *s_smt_log_agent_unnamed_14_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    smt_log_agent_manager           (smt_thread_t **thread);
static int    smt_log_agent_catcher           (smt_thread_t *thread);
static int    smt_log_agent_class_destroy     (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_LOG_AGENT)              ||  defined (SMT_LOG_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_3                (smt_thread_t *thread);
static int    action_unnamed_4                (smt_thread_t *thread);
static int    action_open                     (smt_thread_t *thread);
static int    action_unnamed_6                (smt_thread_t *thread);
static int    action_close                    (smt_thread_t *thread);
static int    action_unnamed_8                (smt_thread_t *thread);
static int    action_set_syslog               (smt_thread_t *thread);
static int    action_unnamed_10               (smt_thread_t *thread);
static int    action_write_bucket             (smt_thread_t *thread);
static int    action_unnamed_12               (smt_thread_t *thread);
static int    action_cycle                    (smt_thread_t *thread);
static int    action_unnamed_14               (smt_thread_t *thread);
static int
smt_log_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
smt_log_agent_destroy_destructor (void *data);
static int
smt_log_agent_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         filename,
    char *         log_path,
    char *         cycle_cmd);
int
smt_log_agent_open_destructor (void *data);
static int
smt_log_agent_set_syslog_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    int            priority);
int
smt_log_agent_set_syslog_destructor (void *data);
static int
smt_log_agent_write_bucket_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    ipr_bucket_t *  bucket);
int
smt_log_agent_write_bucket_destructor (void *data);
//  Function definitions                                                       

static int
smt_log_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    smt_log_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_log_agent_destroy_destructor);
    
    return rc;
}

int
smt_log_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((smt_log_agent_destroy_t *) data)-> file;
    line = ((smt_log_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


static int
smt_log_agent_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         filename,
    char *         log_path,
    char *         cycle_cmd)
{
    int
        rc = 0;
    smt_log_agent_open_t
        *_message;
        
    //  Possess code for open
    

    filename = icl_mem_strdup (filename);
    log_path = icl_mem_strdup (log_path);
    cycle_cmd = icl_mem_strdup (cycle_cmd);
    
    

    _message = icl_mem_cache_alloc (s_open_cache);
    _message->filename                  = filename;
    _message->log_path                  = log_path;
    _message->cycle_cmd                 = cycle_cmd;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_log_agent_open_destructor);
    
    return rc;
}

int
smt_log_agent_open_destructor (void *data)
{
    int
        rc = 0;
char * filename;
char * log_path;
char * cycle_cmd;

    filename = ((smt_log_agent_open_t *) data)-> filename;
    log_path = ((smt_log_agent_open_t *) data)-> log_path;
    cycle_cmd = ((smt_log_agent_open_t *) data)-> cycle_cmd;



    icl_mem_free (filename);
    icl_mem_free (log_path);
    icl_mem_free (cycle_cmd);
    
    

    icl_mem_free (data);
    return rc;
}


static int
smt_log_agent_set_syslog_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    int            priority)
{
    int
        rc = 0;
    smt_log_agent_set_syslog_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_set_syslog_cache);
    _message->priority                  = priority;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_log_agent_set_syslog_destructor);
    
    return rc;
}

int
smt_log_agent_set_syslog_destructor (void *data)
{
    int
        rc = 0;
int priority;

    priority = ((smt_log_agent_set_syslog_t *) data)-> priority;


    icl_mem_free (data);
    return rc;
}


static int
smt_log_agent_write_bucket_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    ipr_bucket_t *  bucket)
{
    int
        rc = 0;
    smt_log_agent_write_bucket_t
        *_message;
        
    //  Possess code for write bucket
    

    bucket = ipr_bucket_link (bucket);
    
    

    _message = icl_mem_cache_alloc (s_write_bucket_cache);
    _message->bucket                    = bucket;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_log_agent_write_bucket_destructor);
    
    return rc;
}

int
smt_log_agent_write_bucket_destructor (void *data)
{
    int
        rc = 0;
ipr_bucket_t * bucket;

    bucket = ((smt_log_agent_write_bucket_t *) data)-> bucket;



    ipr_bucket_unlink (&bucket);
    
    

    icl_mem_free (data);
    return rc;
}


int
smt_log_agent_close (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                close_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_log_agent_cycle (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                cycle_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_log_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_log_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_log_agent_open (
    smt_thread_t * thread,
    char *                filename,
    char *                log_path,
    char *                cycle_cmd)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_log_agent_open_send (
                thread->reply_queue, open_m_event,
                filename,
                log_path,
                cycle_cmd);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_log_agent_set_syslog (
    smt_thread_t * thread,
    int                   priority)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_log_agent_set_syslog_send (
                thread->reply_queue, set_syslog_m_event,
                priority);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_log_agent_write_bucket (
    smt_thread_t * thread,
    ipr_bucket_t *        bucket)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_log_agent_manager);
        rc = smt_log_agent_write_bucket_send (
                thread->reply_queue, write_bucket_m_event,
                bucket);
    }
    else
        rc = -1;
        
    return rc;
}

void
smt_log_agent_term (void)
{

}

int 
smt_log_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (smt_log_agent_class_context_t));
    s_destroy_cache = icl_cache_get (sizeof (smt_log_agent_destroy_t));
    s_open_cache = icl_cache_get (sizeof (smt_log_agent_open_t));
    s_set_syslog_cache = icl_cache_get (sizeof (smt_log_agent_set_syslog_t));
    s_write_bucket_cache = icl_cache_get (sizeof (smt_log_agent_write_bucket_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, smt_log_agent_term);
    
    return rc;
}

void
smt_log_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
smt_log_agent_class_thread_new (
    smt_log_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_log_agent_manager,
                             smt_log_agent_catcher,
                             smt_log_agent_class_destroy);
                             

#if (defined (SMT_LOG_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_log_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (smt_log_agent_class_context_t));
        ((smt_log_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_log_agent_class_context_t *) thread->context)

        tcb->smt_log = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
smt_log_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == smt_log_agent_manager);
                                                 //  Ensure correct manager    

    //  First deal with shutdown request.  These are asynchronous.             
    if (thread->shutdown_request) {
        //  Cancel any outstanding requests
        smt_thread_purge (thread);
        
        thread->shutdown_request = FALSE;
        if (! thread->shutting_down) {
            thread->shutting_down     = TRUE;
            thread->signal_raised     = FALSE;
            thread->_exception_raised = FALSE;
            thread->_next_event       = shutdown_event;
            thread->module            = 0;
        }
    }
    
    //  Convert signal to exception
    if (thread->signal_raised) {
        //  Cancel any outstanding requests
        smt_thread_purge (thread);
        
        thread->signal_raised = FALSE;
        thread->_exception_raised = TRUE;
        thread->_exception_event  = thread->signal_event;
    }

    //  Return straight away if there are pending operations.  This can happen
    //  if a thread gets woken at just the wrong moment.
    if (thread->monitor
    ||  thread->pending_count)
        return SMT_SLEEPING;

    //  Collect method before testing for exception because method may
    //  provoke an exception.
    if ((!thread->_exception_raised)
    &&    thread->module == SMT_TERM_MODULE
    &&    thread->_next_event == SMT_NULL_EVENT
    &&    smt_thread_method_accept (thread) == SMT_EMPTY)
        return SMT_SLEEPING;
        
    //  Finally trap exception and convert to a normal event.                  
    if (thread->_exception_raised) {
        thread->_exception_raised = FALSE;
        thread->_next_event       = thread->_exception_event;
        thread->_exception_event  = SMT_NULL_EVENT;
        thread->module            = 0;
    }

    if (thread->module == SMT_TERM_MODULE)
        switch (thread->place) {
            case 4:
            case 11:
            case 17:
                thread->place = SMT_TERM_PLACE;
                break;
            case 9:
            case 8:
            case 7:
            case 6:
            case 5:
                thread->place = 2;
                break;
            case 15:
            case 13:
            case 16:
            case 3:
            case 14:
            case 12:
                thread->place = 10;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_log_agent_class_context_t *) thread->context)

#undef  tcb
}
            }
            thread->place = 2;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
        if (thread->place < 10)                 //  Main state
            thread->place = 2;
        else
            thread->place = 10;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((smt_log_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                case open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 5; break;
                case close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case set_syslog_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                case write_bucket_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 8; break;
                case cycle_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 9; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 10:                        //  shutting down state                 
{
#define tcb ((smt_log_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 11; break;
                case open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 12; break;
                case close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 13; break;
                case set_syslog_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 14; break;
                case write_bucket_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 15; break;
                case cycle_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 16; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 17; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 17)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 11:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_destroy (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_free (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_unnamed_3 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 4:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 4:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_destroy (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_free (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_unnamed_4 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 4:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 5:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_open (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_unnamed_6 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 6:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_close (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 8);
#endif
                    rc = action_unnamed_8 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 7:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_set_syslog (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_unnamed_10 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 8:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_write_bucket (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_unnamed_12 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 9:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_cycle (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_unnamed_14 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
                    if (rc != SMT_OK)
                        return rc;
                    
                    thread->module++;
            
                    if (thread->pending_count
                    ||  thread->_exception_raised
                    ||  thread->signal_raised
                    ||  thread->shutdown_request
                    ||  smt_break_requested)
                        break;

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
    }

    if (thread->_next_event == SMT_TERM_EVENT) {
        //  Because we optimise links, we need to grab ourselves one now.
        thread = smt_thread_link (thread);
        smt_thread_destroy (thread_p);
        return SMT_FINISHED;
    }
    
    return SMT_OK;
}

static int
smt_log_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
smt_log_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_LOG_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_log_agent",
                 class_thread_name);

#endif
    if (--((smt_log_agent_class_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static void
find_thread_state_next_state (
    int             *thread_type,
    smt_state_t     *state_id,
    smt_event_t     *event_id,
    smt_state_t     *nextstate_id,
    smt_thread_t    *thread)
{
    *thread_type  = 0;
    *state_id     = 0;
    *event_id     = 0;
    *nextstate_id = 0;
    
      {
        *thread_type = 1;
        if (thread->place < 10)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 2;   break;
                case 5: *event_id = 3;   *nextstate_id = 1;   break;
                case 6: *event_id = 4;   *nextstate_id = 1;   break;
                case 7: *event_id = 5;   *nextstate_id = 1;   break;
                case 8: *event_id = 6;   *nextstate_id = 1;   break;
                case 9: *event_id = 7;   *nextstate_id = 1;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 11: *event_id = 2;   break;
                case 12: *event_id = 3;   *nextstate_id = 2;   break;
                case 13: *event_id = 4;   *nextstate_id = 2;   break;
                case 14: *event_id = 5;   *nextstate_id = 2;   break;
                case 15: *event_id = 6;   *nextstate_id = 2;   break;
                case 16: *event_id = 7;   *nextstate_id = 2;   break;
                case 17: *event_id = 1;   break;
            }
          }
      }
}

//  Action functions                                                           


static void
report_unrecognised_event_error (smt_thread_t *thread)
{
    int
        thread_type;
    smt_state_t
        state_id;
    smt_event_t
        event_id;
    smt_state_t
        nextstate_id;
    char
        num_buffer[8],
        *event_name_or_number;
        
    find_thread_state_next_state (&thread_type,
                                  &state_id,
                                  &event_id,
                                  &nextstate_id,
                                  thread);
    if (thread->_next_event >= 0
    &&  thread->_next_event <  tblsize (event_name))
        event_name_or_number = (char *) event_name[thread->_next_event];
    else {
        apr_snprintf (num_buffer, sizeof (num_buffer), "%u", (unsigned) thread->_next_event);
        event_name_or_number = num_buffer;
    }
    icl_console_print ("smt_log_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
                       event_name_or_number,
                       thread_name [thread_type],
                       thread->id,
                       state_name  [state_id]);
    abort ();
}

static void
report_state_machine_error (smt_thread_t *thread)
{
    int
        thread_type;
    smt_state_t
        state_id;
    smt_event_t
        event_id;
    smt_state_t
        nextstate_id;
        
    find_thread_state_next_state (&thread_type,
                                  &state_id,
                                  &event_id,
                                  &nextstate_id,
                                  thread);
    icl_console_print ("State machine error: Event: %s Thread: %s, state: %s",
              event_name  [event_id],
              thread_name [thread_type],
              state_name  [state_id]);
}

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_LOG_AGENT))

static void
animate_start_new_event (smt_thread_t *thread, smt_event_t saveevent_id)
{
    int
        thread_type;
    smt_state_t
        state_id;
    smt_event_t
        event_id;
    smt_state_t
        nextstate_id;
        
    find_thread_state_next_state (&thread_type,
                                  &state_id,
                                  &event_id,
                                  &nextstate_id,
                                  thread);
                                  
  icl_console_print ("%4lu> %-15s/%-15s %s:",
            thread->id,
            "smt_log_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "smt_log_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "smt_log_agent",
              thread_name [find_thread_type (thread->place)],
              action_name [action_id]);
}

static int
find_thread_type (smt_place_t place)
{
    int
        thread_type = 0;
        
        thread_type = 1;
    return thread_type;
}

#endif
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_LOG_AGENT)                ||  defined (SMT_LOG_AGENT_TRACE))
static void s_dump (
    icl_os_thread_t thread,
    apr_time_t      time,
    qbyte           info)
{
    smt_thread_id_t
        thread_id = info / 0x10000;
    smt_place_t
        state_or_event_or_action = info % 0x10000;
    int
        action_id;
    smt_event_t
        event_id;
    smt_state_t
        state_id;
        
    if (state_or_event_or_action > laststate + lastevent) {
        action_id = state_or_event_or_action - laststate - lastevent;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s           + %s",
                                       thread_id,
                                       "smt_log_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "smt_log_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "smt_log_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_DESTROY))
    icl_stats_inc ("smt_log_agent_destroy", &s_smt_log_agent_destroy_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_free (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_FREE))
    icl_stats_inc ("smt_log_agent_free", &s_smt_log_agent_free_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_log object");
    smt_log_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_log_free (self);
    self = NULL;
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_3 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_3))
    icl_stats_inc ("smt_log_agent_unnamed_3", &s_smt_log_agent_unnamed_3_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_4 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_4))
    icl_stats_inc ("smt_log_agent_unnamed_4", &s_smt_log_agent_unnamed_4_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_OPEN))
    icl_stats_inc ("smt_log_agent_open", &s_smt_log_agent_open_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log
char *
    filename;
char *
    log_path;
char *
    cycle_cmd;

    filename = open_m->filename;
    log_path = open_m->log_path;
    cycle_cmd = open_m->cycle_cmd;
        {
icl_shortstr_cpy (self->filename,  filename);
icl_shortstr_cpy (self->log_path,  log_path);
icl_shortstr_cpy (self->cycle_cmd, cycle_cmd);

//  Ensure that logging directories exist
if (*self->log_path && !ipr_file_is_directory (self->log_path))
    ipr_dir_create (self->log_path);

if (ipr_file_is_writeable (self->log_path)) {
    ipr_file_set_path (self->filename, self->log_path);

    //  Archive the previous log file if exists
    ipr_log_cycle (self->filename, NULL, self->cycle_cmd);
    self->output = fopen (self->filename, "w");
    if (!self->output)
        icl_console_print ("E: can't open %s: %s", self->filename, strerror (errno));
}
else
    icl_console_print ("E: cannot write to '%s/%s' - discarding data", self->log_path, filename);
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_6 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_6))
    icl_stats_inc ("smt_log_agent_unnamed_6", &s_smt_log_agent_unnamed_6_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_CLOSE))
    icl_stats_inc ("smt_log_agent_close", &s_smt_log_agent_close_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log

        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
    self->output = NULL;
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_8 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_8))
    icl_stats_inc ("smt_log_agent_unnamed_8", &s_smt_log_agent_unnamed_8_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_set_syslog (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_SET_SYSLOG))
    icl_stats_inc ("smt_log_agent_set_syslog", &s_smt_log_agent_set_syslog_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log
int
    priority;

    priority = set_syslog_m->priority;
        {
assert (!self->output);
self->tosyslog = TRUE;
self->priority = priority;
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_10 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_10))
    icl_stats_inc ("smt_log_agent_unnamed_10", &s_smt_log_agent_unnamed_10_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_write_bucket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_WRITE_BUCKET))
    icl_stats_inc ("smt_log_agent_write_bucket", &s_smt_log_agent_write_bucket_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log
ipr_bucket_t *
    bucket;

    bucket = write_bucket_m->bucket;
        {
if (self->tosyslog)
    ipr_log_sysprint (self->priority, (char *) bucket->data);
else
if (self->output) {
    //  Always printf into %s to guard against %'s in data
    fprintf (self->output, "%s", (char *) (bucket->data));
    fprintf (self->output, "\n");
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_12 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_12))
    icl_stats_inc ("smt_log_agent_unnamed_12", &s_smt_log_agent_unnamed_12_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_cycle (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_CYCLE))
    icl_stats_inc ("smt_log_agent_cycle", &s_smt_log_agent_cycle_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{


#define self tcb->smt_log

        {
if (self->output) {
    fflush (self->output);
    fclose (self->output);
}
//  Archive the previous log file if exists
ipr_log_cycle (self->filename, NULL, self->cycle_cmd);
self->output = fopen (self->filename, "w");
if (!self->output)
    icl_console_print ("E: can't open %s: %s", self->filename, strerror (errno));
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_14 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_LOG_AGENT)  ||  defined (BASE_STATS_SMT_LOG_AGENT_UNNAMED_14))
    icl_stats_inc ("smt_log_agent_unnamed_14", &s_smt_log_agent_unnamed_14_stats);
#endif        
#define tcb ((smt_log_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

