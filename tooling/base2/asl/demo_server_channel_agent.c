/*---------------------------------------------------------------------------
    demo_server_channel_agent.c - functions for demo_server_channel_agent.

    Generated from demo_server_channel_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "demo_server_channel_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define consume_m ((demo_server_channel_agent_consume_t *) thread->method->data)

#define cancel_m ((demo_server_channel_agent_cancel_t *) thread->method->data)

#define destroy_m ((demo_server_channel_agent_destroy_t *) thread->method->data)

enum {
    shutdown_event                                 = 1,
    cancel_m_event                                 = 2,
    destroy_m_event                                = 3,
    consume_m_event                                = 4,
};

#define laststate                                  2
#define lastevent                                  4
#define lastplace                                  11

#define demo_server_channel_agent_agent_name       "Demo-Server-Channel-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define shutdown_event_name                        "Shutdown"
#define cancel_m_event_name                        "Cancel-M"
#define destroy_m_event_name                       "Destroy-M"
#define consume_m_event_name                       "Consume-M"

#define cancel_action_name                         "Cancel"
#define unnamed_2_action_name                      "Unnamed-2"
#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_5_action_name                      "Unnamed-5"
#define consume_action_name                        "Consume"
#define unnamed_7_action_name                      "Unnamed-7"
#define unnamed_8_action_name                      "Unnamed-8"
#define unnamed_9_action_name                      "Unnamed-9"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    demo_server_method_t *  method;
  }
demo_server_channel_agent_consume_t;

typedef struct
  {
    demo_server_method_t *  method;
  }
demo_server_channel_agent_cancel_t;

typedef struct
  {
    char *         file;
    size_t         line;
  }
demo_server_channel_agent_destroy_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        demo_server_channel_t
            *demo_server_channel;
    
} demo_server_channel_agent_class_context_t;

static icl_cache_t
    *s_consume_cache = NULL;
static icl_cache_t
    *s_cancel_cache = NULL;
static icl_cache_t
    *s_destroy_cache = NULL;

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
    cancel_m_event_name,
    destroy_m_event_name,
    consume_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT)   ||  defined (BASE_TRACE)  || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT)   ||  defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    cancel_action_name,
    unnamed_2_action_name,
    destroy_action_name,
    free_action_name,
    unnamed_5_action_name,
    consume_action_name,
    unnamed_7_action_name,
    unnamed_8_action_name,
    unnamed_9_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_CANCEL))
static icl_stats_t *s_demo_server_channel_agent_cancel_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_2))
static icl_stats_t *s_demo_server_channel_agent_unnamed_2_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_DESTROY))
static icl_stats_t *s_demo_server_channel_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_FREE))
static icl_stats_t *s_demo_server_channel_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_5))
static icl_stats_t *s_demo_server_channel_agent_unnamed_5_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_CONSUME))
static icl_stats_t *s_demo_server_channel_agent_consume_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_7))
static icl_stats_t *s_demo_server_channel_agent_unnamed_7_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_8))
static icl_stats_t *s_demo_server_channel_agent_unnamed_8_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_9))
static icl_stats_t *s_demo_server_channel_agent_unnamed_9_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    demo_server_channel_agent_manager  (smt_thread_t **thread);
static int    demo_server_channel_agent_catcher  (smt_thread_t *thread);
static int    demo_server_channel_agent_class_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT)   ||  defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_cancel                   (smt_thread_t *thread);
static int    action_unnamed_2                (smt_thread_t *thread);
static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_5                (smt_thread_t *thread);
static int    action_consume                  (smt_thread_t *thread);
static int    action_unnamed_7                (smt_thread_t *thread);
static int    action_unnamed_8                (smt_thread_t *thread);
static int    action_unnamed_9                (smt_thread_t *thread);
static int
demo_server_channel_agent_consume_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_method_t *  method);
int
demo_server_channel_agent_consume_destructor (void *data);
static int
demo_server_channel_agent_cancel_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_method_t *  method);
int
demo_server_channel_agent_cancel_destructor (void *data);
static int
demo_server_channel_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
demo_server_channel_agent_destroy_destructor (void *data);
//  Function definitions                                                       

static int
demo_server_channel_agent_consume_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_method_t *  method)
{
    int
        rc = 0;
    demo_server_channel_agent_consume_t
        *_message;
        
    //  Possess code for consume
    

    method = demo_server_method_link (method);
    
    

    _message = icl_mem_cache_alloc (s_consume_cache);
    _message->method                    = method;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_channel_agent_consume_destructor);
    
    return rc;
}

int
demo_server_channel_agent_consume_destructor (void *data)
{
    int
        rc = 0;
demo_server_method_t * method;

    method = ((demo_server_channel_agent_consume_t *) data)-> method;



    demo_server_method_unlink (&method);
    
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_channel_agent_cancel_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_method_t *  method)
{
    int
        rc = 0;
    demo_server_channel_agent_cancel_t
        *_message;
        
    //  Possess code for cancel
    

    method = demo_server_method_link (method);
    
    

    _message = icl_mem_cache_alloc (s_cancel_cache);
    _message->method                    = method;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_channel_agent_cancel_destructor);
    
    return rc;
}

int
demo_server_channel_agent_cancel_destructor (void *data)
{
    int
        rc = 0;
demo_server_method_t * method;

    method = ((demo_server_channel_agent_cancel_t *) data)-> method;



    demo_server_method_unlink (&method);
    
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_channel_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    demo_server_channel_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_channel_agent_destroy_destructor);
    
    return rc;
}

int
demo_server_channel_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((demo_server_channel_agent_destroy_t *) data)-> file;
    line = ((demo_server_channel_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


int
demo_server_channel_agent_consume (
    smt_thread_t * thread,
    demo_server_method_t * method)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_channel_agent_manager);
        rc = demo_server_channel_agent_consume_send (
                thread->reply_queue, consume_m_event,
                method);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_channel_agent_cancel (
    smt_thread_t * thread,
    demo_server_method_t * method)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_channel_agent_manager);
        rc = demo_server_channel_agent_cancel_send (
                thread->reply_queue, cancel_m_event,
                method);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_channel_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_channel_agent_manager);
        rc = demo_server_channel_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

void
demo_server_channel_agent_term (void)
{

}

int 
demo_server_channel_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (demo_server_channel_agent_class_context_t));
    s_consume_cache = icl_cache_get (sizeof (demo_server_channel_agent_consume_t));
    s_cancel_cache = icl_cache_get (sizeof (demo_server_channel_agent_cancel_t));
    s_destroy_cache = icl_cache_get (sizeof (demo_server_channel_agent_destroy_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, demo_server_channel_agent_term);
    
    return rc;
}

void
demo_server_channel_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
demo_server_channel_agent_class_thread_new (
    demo_server_channel_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_server_channel_agent_manager,
                             demo_server_channel_agent_catcher,
                             demo_server_channel_agent_class_destroy);
                             

#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_server_channel_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (demo_server_channel_agent_class_context_t));
        ((demo_server_channel_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)

        tcb->demo_server_channel = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
demo_server_channel_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == demo_server_channel_agent_manager);
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
            case 11:
            case 6:
            case 9:
                thread->place = SMT_TERM_PLACE;
                break;
            case 4:
            case 5:
                thread->place = 2;
                break;
            case 10:
            case 3:
            case 8:
                thread->place = 7;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)

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
        if (thread->place < 7)                  //  Main state
            thread->place = 2;
        else
            thread->place = 7;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case consume_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 4; break;
                case cancel_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 5; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 6; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 7:                         //  shutting down state                 
{
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case cancel_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 8; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 9; break;
                case consume_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 10; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 11; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 10:
        case 11:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 11)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 8:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_cancel (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_unnamed_2 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_unnamed_5 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_consume (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_unnamed_7 (thread);

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
        case 5:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_cancel (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
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
        case 6:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_unnamed_9 (thread);

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
demo_server_channel_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
demo_server_channel_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_server_channel_agent",
                 class_thread_name);

#endif
    if (--((demo_server_channel_agent_class_context_t *) thread->context)->links == 0)
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
        if (thread->place < 7)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 4;   *nextstate_id = 1;   break;
                case 5: *event_id = 2;   *nextstate_id = 1;   break;
                case 6: *event_id = 3;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 8: *event_id = 2;   *nextstate_id = 2;   break;
                case 9: *event_id = 3;   break;
                case 10: *event_id = 4;   *nextstate_id = 2;   break;
                case 11: *event_id = 1;   break;
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
    icl_console_print ("demo_server_channel_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_CHANNEL_AGENT))

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
            "demo_server_channel_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "demo_server_channel_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "demo_server_channel_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_CHANNEL_AGENT)    ||  defined (DEMO_SERVER_CHANNEL_AGENT_TRACE))
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
                                       "demo_server_channel_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "demo_server_channel_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "demo_server_channel_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_cancel (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_CANCEL))
    icl_stats_inc ("demo_server_channel_agent_cancel", &s_demo_server_channel_agent_cancel_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{


#define self tcb->demo_server_channel
demo_server_method_t *
    method;

    method = cancel_m->method;
        {
demo_queue_t
    *queue;

queue = demo_queue_search (method->payload.basic_cancel.consumer_tag);
if (queue) {
    if (queue->exclusive && queue->channel == self) {
        demo_server_agent_basic_cancel_ok (
            self->connection->thread, 
            self->number, 
            queue->name);
        queue->feed_on = FALSE;
        demo_queue_unlink (&queue);
    }
    else
        demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
            "Access refused to consumer",
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
}
else
    demo_server_channel_error (self, ASL_NOT_FOUND, 
        "Invalid consumer tag", 
        DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_2 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_2))
    icl_stats_inc ("demo_server_channel_agent_unnamed_2", &s_demo_server_channel_agent_unnamed_2_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_DESTROY))
    icl_stats_inc ("demo_server_channel_agent_destroy", &s_demo_server_channel_agent_destroy_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{


#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
demo_server_connection_unlink (&self->connection);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_FREE))
    icl_stats_inc ("demo_server_channel_agent_free", &s_demo_server_channel_agent_free_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{


#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_server_channel object");
    demo_server_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_server_channel_free (self);
    self = NULL;
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_5 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_5))
    icl_stats_inc ("demo_server_channel_agent_unnamed_5", &s_demo_server_channel_agent_unnamed_5_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_consume (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_CONSUME))
    icl_stats_inc ("demo_server_channel_agent_consume", &s_demo_server_channel_agent_consume_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{


#define self tcb->demo_server_channel
demo_server_method_t *
    method;

    method = consume_m->method;
        {
demo_queue_t
    *queue;

queue = demo_queue_search (method->payload.basic_consume.queue);
if (queue) {
    if (queue->exclusive && queue->channel == self) {
        demo_server_agent_basic_consume_ok (
            self->connection->thread, 
            self->number, 
            queue->name);
        queue->feed_on = TRUE;
        demo_queue_unlink (&queue);
    }
    else
        demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
            "Access refused to queue",
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
}
else
    demo_server_channel_error (self, ASL_NOT_FOUND, 
        "No such queue defined", 
        DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_7 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_7))
    icl_stats_inc ("demo_server_channel_agent_unnamed_7", &s_demo_server_channel_agent_unnamed_7_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_8 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_8))
    icl_stats_inc ("demo_server_channel_agent_unnamed_8", &s_demo_server_channel_agent_unnamed_8_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_9 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_CHANNEL_AGENT_UNNAMED_9))
    icl_stats_inc ("demo_server_channel_agent_unnamed_9", &s_demo_server_channel_agent_unnamed_9_stats);
#endif        
#define tcb ((demo_server_channel_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

