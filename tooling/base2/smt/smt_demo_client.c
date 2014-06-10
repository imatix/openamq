/*---------------------------------------------------------------------------
    smt_demo_client.c - functions for smt_demo_client.

    Generated from smt_demo_client.smt by smt_gen.gsl using GSL/4.

    
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
#include "smt_demo_client.h"

#define main_start_state                           1
#define main_after_sort_state                      2

enum {
    ok_event                                       = 1,
    shutdown_event                                 = 2,
};

#define laststate                                  2
#define lastevent                                  2
#define lastplace                                  7

#define smt_demo_client_agent_name                 "Smt-Demo-Client"

#define main_thread_name                           "Main"

#define start_state_name                           "Start"
#define after_sort_state_name                      "After-Sort"

#define ok_event_name                              "Ok"
#define shutdown_event_name                        "Shutdown"

#define create_server_thread_action_name           "Create-Server-Thread"
#define request_string_sort_action_name            "Request-String-Sort"
#define display_sorted_strings_action_name         "Display-Sorted-Strings"
#define destroy_server_thread_action_name          "Destroy-Server-Thread"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised


#include "smt_demo_server.h"
    
static icl_cache_t
    *s_main_context_cache = NULL;

typedef struct {
    int
        links;

    char * string_1;
    char * string_2;
    smt_thread_t *server_thread;
        
} smt_demo_client_main_context_t;


static const char *thread_name [] = {
    "<NULL>",
    main_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    start_state_name,
    after_sort_state_name
};

static const char *event_name [] = {
    "<NULL>",
    ok_event_name,
    shutdown_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT)          ||  defined (BASE_TRACE)  || defined (BASE_TRACE_SMT_DEMO_CLIENT)   ||  defined (SMT_DEMO_CLIENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    create_server_thread_action_name,
    request_string_sort_action_name,
    display_sorted_strings_action_name,
    destroy_server_thread_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_CREATE_SERVER_THREAD))
static icl_stats_t *s_smt_demo_client_create_server_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_REQUEST_STRING_SORT))
static icl_stats_t *s_smt_demo_client_request_string_sort_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_DISPLAY_SORTED_STRINGS))
static icl_stats_t *s_smt_demo_client_display_sorted_strings_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_DESTROY_SERVER_THREAD))
static icl_stats_t *s_smt_demo_client_destroy_server_thread_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    smt_demo_client_manager         (smt_thread_t **thread);
static int    smt_demo_client_catcher         (smt_thread_t *thread);
static int    smt_demo_client_main_destroy    (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_DEMO_CLIENT)            ||  defined (SMT_DEMO_CLIENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_create_server_thread     (smt_thread_t *thread);
static int    action_request_string_sort      (smt_thread_t *thread);
static int    action_display_sorted_strings   (smt_thread_t *thread);
static int    action_destroy_server_thread    (smt_thread_t *thread);
//  Function definitions                                                       


int main (int argc, char *argv [])
{
    int
        count;
        
    icl_system_initialise (argc, argv);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
    if (argc != 3) {
        icl_console_print ("Syntax: smt_demo_client <string1> <string2>");
        return (1);
    }
    smt_initialise ();
    smt_demo_server_init ();
    for (count = 0; count < 1; count++) {
        smt_demo_client_init (argv [1], argv [2]);
    }
    smt_wait (0);
    
    icl_system_terminate ();
    return (0);
}
    
void
smt_demo_client_term (void)
{

}

int 
smt_demo_client_init (
    char * string_1,
    char * string_2)
{
    int
        rc = SMT_OK;                    //  Return code

    s_main_context_cache = icl_cache_get (sizeof (smt_demo_client_main_context_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    smt_thread_t * client_thread;
    smt_demo_client_main_context_t * client_context;
    
    smt_demo_client_animate (TRUE);
    smt_demo_server_animate (TRUE);
    
    client_thread = smt_demo_client_main_thread_new ();
    if (client_thread) {
        client_context = client_thread->context;
        client_context->string_1 = string_1;
        client_context->string_2 = string_2;
        smt_thread_set_next_event (client_thread, ok_event);
        smt_thread_wake (client_thread);
        smt_thread_unlink (&client_thread);
    }
    
    }
    icl_system_register (NULL, smt_demo_client_term);
    
    return rc;
}

void
smt_demo_client_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
    s_animate = animate;
#endif
}

smt_thread_t *
smt_demo_client_main_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_demo_client_manager,
                             smt_demo_client_catcher,
                             smt_demo_client_main_destroy);
                             

#if (defined (SMT_DEMO_CLIENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_demo_client",
                main_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_main_context_cache);
        memset (thread->context, 0, sizeof (smt_demo_client_main_context_t));
        ((smt_demo_client_main_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_demo_client_main_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
smt_demo_client_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == smt_demo_client_manager);
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
            case 6:
            case 7:
                thread->place = SMT_TERM_PLACE;
                break;
            case 3:
                thread->place = 5;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Main thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_demo_client_main_context_t *) thread->context)

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
        if (thread->place < 5)                  //  Start state
            thread->place = 2;
        else
            thread->place = 5;
    }

    switch (thread->place) {
        case 2:                         //  start state                         
{
#define tcb ((smt_demo_client_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 5:                         //  after sort state                    
{
#define tcb ((smt_demo_client_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 6; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 7; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_create_server_thread (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_request_string_sort (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_display_sorted_strings (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_destroy_server_thread (thread);

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
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 7:
        case 4:
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
smt_demo_client_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
smt_demo_client_main_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_DEMO_CLIENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_demo_client",
                 main_thread_name);

#endif
    if (--((smt_demo_client_main_context_t *) thread->context)->links == 0)
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
        if (thread->place < 5)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 2;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 6: *event_id = 1;   break;
                case 7: *event_id = 2;   break;
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
    icl_console_print ("smt_demo_client: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_CLIENT))

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
            "smt_demo_client",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "smt_demo_client",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "smt_demo_client",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_CLIENT)              ||  defined (SMT_DEMO_CLIENT_TRACE))
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
                                       "smt_demo_client",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "smt_demo_client",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "smt_demo_client",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_create_server_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_CREATE_SERVER_THREAD))
    icl_stats_inc ("smt_demo_client_create_server_thread", &s_smt_demo_client_create_server_thread_stats);
#endif        
#define tcb ((smt_demo_client_main_context_t *) thread->context)
{

    tcb->server_thread = smt_demo_server_main_thread_new ();
    if (tcb->server_thread)
        smt_thread_wake (tcb->server_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_request_string_sort (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_REQUEST_STRING_SORT))
    icl_stats_inc ("smt_demo_client_request_string_sort", &s_smt_demo_client_request_string_sort_stats);
#endif        
#define tcb ((smt_demo_client_main_context_t *) thread->context)
{

    smt_demo_server_sort (tcb->server_thread,
                          thread,
                          ok_event,
                          tcb->string_1,
                          tcb->string_2);
    
}
#undef  tcb
    return _rc;
}

static int
action_display_sorted_strings (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_DISPLAY_SORTED_STRINGS))
    icl_stats_inc ("smt_demo_client_display_sorted_strings", &s_smt_demo_client_display_sorted_strings_stats);
#endif        
#define tcb ((smt_demo_client_main_context_t *) thread->context)
{

    icl_console_print ("Response: <%s> <%s>", 
              smt_demo_server_sort_response_m->string_1,
              smt_demo_server_sort_response_m->string_2);
    
}
#undef  tcb
    return _rc;
}

static int
action_destroy_server_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT)  ||  defined (BASE_STATS_SMT_DEMO_CLIENT_DESTROY_SERVER_THREAD))
    icl_stats_inc ("smt_demo_client_destroy_server_thread", &s_smt_demo_client_destroy_server_thread_stats);
#endif        
#define tcb ((smt_demo_client_main_context_t *) thread->context)
{

        smt_demo_server_destroy (tcb->server_thread);
        smt_thread_unlink       (&tcb->server_thread);
    
}
#undef  tcb
    return _rc;
}

