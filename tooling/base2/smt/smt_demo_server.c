/*---------------------------------------------------------------------------
    smt_demo_server.c - functions for smt_demo_server.

    Generated from smt_demo_server.smt by smt_gen.gsl using GSL/4.

    
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
#include "smt_demo_server.h"

#define main_start_state                           1

#define sort_m ((smt_demo_server_sort_t *) thread->method->data)

enum {
    destroy_event                                  = 1,
    sort_m_event                                   = 2,
    shutdown_event                                 = 3,
};

#define laststate                                  1
#define lastevent                                  3
#define lastplace                                  5

#define smt_demo_server_agent_name                 "Smt-Demo-Server"

#define main_thread_name                           "Main"

#define start_state_name                           "Start"

#define destroy_event_name                         "Destroy"
#define sort_m_event_name                          "Sort-M"
#define shutdown_event_name                        "Shutdown"

#define sort_two_strings_action_name               "Sort-Two-Strings"
#define reply_sorted_strings_action_name           "Reply-Sorted-Strings"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    smt_thread_t *  reply_thread;
    smt_event_t    reply_event;
    char *         string_1;
    char *         string_2;
  }
smt_demo_server_sort_t;

static icl_cache_t
    *s_main_context_cache = NULL;

typedef struct {
    int
        links;

    int dummy;
        
} smt_demo_server_main_context_t;

static icl_cache_t
    *s_sort_response_cache = NULL;
static icl_cache_t
    *s_sort_cache = NULL;

static const char *thread_name [] = {
    "<NULL>",
    main_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    start_state_name
};

static const char *event_name [] = {
    "<NULL>",
    destroy_event_name,
    sort_m_event_name,
    shutdown_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER)          ||  defined (BASE_TRACE)  || defined (BASE_TRACE_SMT_DEMO_SERVER)   ||  defined (SMT_DEMO_SERVER_TRACE))
static const char *action_name [] = {
    "<NULL>",
    sort_two_strings_action_name,
    reply_sorted_strings_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_SERVER)  ||  defined (BASE_STATS_SMT_DEMO_SERVER_SORT_TWO_STRINGS))
static icl_stats_t *s_smt_demo_server_sort_two_strings_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_SERVER)  ||  defined (BASE_STATS_SMT_DEMO_SERVER_REPLY_SORTED_STRINGS))
static icl_stats_t *s_smt_demo_server_reply_sorted_strings_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    smt_demo_server_manager         (smt_thread_t **thread);
static int    smt_demo_server_catcher         (smt_thread_t *thread);
static int    smt_demo_server_main_destroy    (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_DEMO_SERVER)            ||  defined (SMT_DEMO_SERVER_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_sort_two_strings         (smt_thread_t *thread);
static int    action_reply_sorted_strings     (smt_thread_t *thread);
static int
smt_demo_server_sort_response_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         string_1,
    char *         string_2);
int
smt_demo_server_sort_response_destructor (void *data);
static int
smt_demo_server_sort_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_thread_t *  reply_thread,
    smt_event_t    reply_event,
    char *         string_1,
    char *         string_2);
int
smt_demo_server_sort_destructor (void *data);
//  Function definitions                                                       

static int
smt_demo_server_sort_response_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         string_1,
    char *         string_2)
{
    int
        rc = 0;
    smt_demo_server_sort_response_t
        *_message;
        
    //  Possess code for sort response
    
        string_1 = icl_mem_strdup (string_1);
        string_2 = icl_mem_strdup (string_2);
        

    _message = icl_mem_cache_alloc (s_sort_response_cache);
    _message->string_1                  = string_1;
    _message->string_2                  = string_2;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_server_sort_response_destructor);
    
    return rc;
}

int
smt_demo_server_sort_response_destructor (void *data)
{
    int
        rc = 0;
char * string_1;
char * string_2;

    string_1 = ((smt_demo_server_sort_response_t *) data)-> string_1;
    string_2 = ((smt_demo_server_sort_response_t *) data)-> string_2;


        icl_mem_free (string_1);
        icl_mem_free (string_2);
        

    icl_mem_free (data);
    return rc;
}


static int
smt_demo_server_sort_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_thread_t *  reply_thread,
    smt_event_t    reply_event,
    char *         string_1,
    char *         string_2)
{
    int
        rc = 0;
    smt_demo_server_sort_t
        *_message;
        
    //  Possess code for sort
    
        string_1 = icl_mem_strdup (string_1);
        string_2 = icl_mem_strdup (string_2);
        

    _message = icl_mem_cache_alloc (s_sort_cache);
    _message->reply_thread              = reply_thread;
    _message->reply_event               = reply_event;
    _message->string_1                  = string_1;
    _message->string_2                  = string_2;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_server_sort_destructor);
    
    return rc;
}

int
smt_demo_server_sort_destructor (void *data)
{
    int
        rc = 0;
smt_thread_t * reply_thread;
smt_event_t reply_event;
char * string_1;
char * string_2;

    reply_thread = ((smt_demo_server_sort_t *) data)-> reply_thread;
    reply_event = ((smt_demo_server_sort_t *) data)-> reply_event;
    string_1 = ((smt_demo_server_sort_t *) data)-> string_1;
    string_2 = ((smt_demo_server_sort_t *) data)-> string_2;


        icl_mem_free (string_1);
        icl_mem_free (string_2);
        

    icl_mem_free (data);
    return rc;
}


int
smt_demo_server_destroy (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_server_manager);
        rc = smt_method_send (
                thread->reply_queue,
                destroy_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_demo_server_sort (
    smt_thread_t * thread,
    smt_thread_t *        reply_thread,
    smt_event_t           reply_event,
    char *                string_1,
    char *                string_2)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_server_manager);
        rc = smt_demo_server_sort_send (
                thread->reply_queue, sort_m_event,
                reply_thread,
                reply_event,
                string_1,
                string_2);
    }
    else
        rc = -1;
        
    return rc;
}

void
smt_demo_server_term (void)
{

}

int 
smt_demo_server_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_main_context_cache = icl_cache_get (sizeof (smt_demo_server_main_context_t));
    s_sort_response_cache = icl_cache_get (sizeof (smt_demo_server_sort_response_t));
    s_sort_cache = icl_cache_get (sizeof (smt_demo_server_sort_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, smt_demo_server_term);
    
    return rc;
}

void
smt_demo_server_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
    s_animate = animate;
#endif
}

smt_thread_t *
smt_demo_server_main_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_demo_server_manager,
                             smt_demo_server_catcher,
                             smt_demo_server_main_destroy);
                             

#if (defined (SMT_DEMO_SERVER_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_demo_server",
                main_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_main_context_cache);
        memset (thread->context, 0, sizeof (smt_demo_server_main_context_t));
        ((smt_demo_server_main_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_demo_server_main_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
smt_demo_server_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == smt_demo_server_manager);
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
            case 3:
            case 5:
                thread->place = SMT_TERM_PLACE;
                break;
            case 4:
                thread->place = 2;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Main thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_demo_server_main_context_t *) thread->context)

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
            thread->place = 2;
    }

    switch (thread->place) {
        case 2:                         //  start state                         
{
#define tcb ((smt_demo_server_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case destroy_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case sort_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 5; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 5:
            switch (thread->module) {
                case 1:                         //  After actions              
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_sort_two_strings (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_reply_sorted_strings (thread);

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
smt_demo_server_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
smt_demo_server_main_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_DEMO_SERVER_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_demo_server",
                 main_thread_name);

#endif
    if (--((smt_demo_server_main_context_t *) thread->context)->links == 0)
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
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   break;
                case 4: *event_id = 2;   *nextstate_id = 1;   break;
                case 5: *event_id = 3;   break;
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
    icl_console_print ("smt_demo_server: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_SERVER))

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
            "smt_demo_server",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "smt_demo_server",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "smt_demo_server",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_SERVER)              ||  defined (SMT_DEMO_SERVER_TRACE))
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
                                       "smt_demo_server",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "smt_demo_server",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "smt_demo_server",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_sort_two_strings (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_SERVER)  ||  defined (BASE_STATS_SMT_DEMO_SERVER_SORT_TWO_STRINGS))
    icl_stats_inc ("smt_demo_server_sort_two_strings", &s_smt_demo_server_sort_two_strings_stats);
#endif        
#define tcb ((smt_demo_server_main_context_t *) thread->context)
{

    char *swap;
    
    if (strcmp (sort_m->string_1,
                sort_m->string_2) > 0) {
        swap = sort_m->string_1;
        sort_m->string_1 =
            sort_m->string_2;
        sort_m->string_2 = swap;
    }
    
}
#undef  tcb
    return _rc;
}

static int
action_reply_sorted_strings (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_SERVER)  ||  defined (BASE_STATS_SMT_DEMO_SERVER_REPLY_SORTED_STRINGS))
    icl_stats_inc ("smt_demo_server_reply_sorted_strings", &s_smt_demo_server_reply_sorted_strings_stats);
#endif        
#define tcb ((smt_demo_server_main_context_t *) thread->context)
{

    smt_demo_server_sort_response_send (    
        sort_m->reply_thread->reply_queue,
        sort_m->reply_event,
        sort_m->string_1,
        sort_m->string_2);
    
}
#undef  tcb
    return _rc;
}

