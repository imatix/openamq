/*---------------------------------------------------------------------------
    tstsock.c - functions for tstsock.

    Generated from tstsock.smt by smt_gen.gsl using GSL/4.

    
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
#include "tstsock.h"

#define main_after_init_state                      1
#define main_state_2_state                         2
#define main_state_3_state                         3
#define main_state_4_state                         4

enum {
    ok_event                                       = 1,
    input_event                                    = 2,
    read_event                                     = 3,
    write_event                                    = 4,
    shutdown_event                                 = 5,
};

#define laststate                                  4
#define lastevent                                  5
#define lastplace                                  13

#define tstsock_agent_name                         "Tstsock"

#define main_thread_name                           "Main"

#define after_init_state_name                      "After-Init"
#define state_2_state_name                         "State-2"
#define state_3_state_name                         "State-3"
#define state_4_state_name                         "State-4"

#define ok_event_name                              "Ok"
#define input_event_name                           "Input"
#define read_event_name                            "Read"
#define write_event_name                           "Write"
#define shutdown_event_name                        "Shutdown"

#define report_result_action_name                  "Report-Result"
#define display_buffers_action_name                "Display-Buffers"
#define test_failed_connect_action_name            "Test-Failed-Connect"
#define free_conn_socket_action_name               "Free-Conn-Socket"
#define test_successful_connect_action_name        "Test-Successful-Connect"
#define unnamed_5_action_name                      "Unnamed-5"
#define free_slave_socket_action_name              "Free-Slave-Socket"
#define test_sending___receiving_data_part_1_action_name  "Test-Sending---Receiving-Data-Part-1"
#define test_sending___receiving_data_part_2_action_name  "Test-Sending---Receiving-Data-Part-2"
#define test_sending_large_data_action_name        "Test-Sending-Large-Data"
#define test_sending_with_cork_action_name         "Test-Sending-With-Cork"
#define test_timeout_action_name                   "Test-Timeout"
#define test_write_then_close_action_name          "Test-Write-Then-Close"
#define test_write_to_invalid_socket_action_name   "Test-Write-To-Invalid-Socket"
#define test_read_on_broken_pipe_action_name       "Test-Read-On-Broken-Pipe"
#define read_then_close_part_1_action_name         "Read-Then-Close-Part-1"
#define read_then_close_part_2_action_name         "Read-Then-Close-Part-2"
#define input_with_event_part_1_action_name        "Input-With-Event-Part-1"
#define read_with_event_action_name                "Read-With-Event"
#define write_with_event_action_name               "Write-With-Event"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised


#define TSTSOCK_PORT     "5001"
#define TSTSOCK_PORT_ERR "5002"

#define LARGE_SIZE 100000
    
static icl_cache_t
    *s_main_context_cache = NULL;

typedef struct {
    int
        links;

    byte *upbuf;
    byte *downbuf;
        
} tstsock_main_context_t;


static const char *thread_name [] = {
    "<NULL>",
    main_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    after_init_state_name,
    state_2_state_name,
    state_3_state_name,
    state_4_state_name
};

static const char *event_name [] = {
    "<NULL>",
    ok_event_name,
    input_event_name,
    read_event_name,
    write_event_name,
    shutdown_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK)                  ||  defined (BASE_TRACE)  || defined (BASE_TRACE_TSTSOCK)   ||  defined (TSTSOCK_TRACE))
static const char *action_name [] = {
    "<NULL>",
    report_result_action_name,
    display_buffers_action_name,
    test_failed_connect_action_name,
    free_conn_socket_action_name,
    test_successful_connect_action_name,
    unnamed_5_action_name,
    free_slave_socket_action_name,
    test_sending___receiving_data_part_1_action_name,
    test_sending___receiving_data_part_2_action_name,
    test_sending_large_data_action_name,
    test_sending_with_cork_action_name,
    test_timeout_action_name,
    test_write_then_close_action_name,
    test_write_to_invalid_socket_action_name,
    test_read_on_broken_pipe_action_name,
    read_then_close_part_1_action_name,
    read_then_close_part_2_action_name,
    input_with_event_part_1_action_name,
    read_with_event_action_name,
    write_with_event_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_REPORT_RESULT))
static icl_stats_t *s_tstsock_report_result_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_DISPLAY_BUFFERS))
static icl_stats_t *s_tstsock_display_buffers_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_FAILED_CONNECT))
static icl_stats_t *s_tstsock_test_failed_connect_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_FREE_CONN_SOCKET))
static icl_stats_t *s_tstsock_free_conn_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SUCCESSFUL_CONNECT))
static icl_stats_t *s_tstsock_test_successful_connect_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_UNNAMED_5))
static icl_stats_t *s_tstsock_unnamed_5_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_FREE_SLAVE_SOCKET))
static icl_stats_t *s_tstsock_free_slave_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING___RECEIVING_DATA_PART_1))
static icl_stats_t *s_tstsock_test_sending___receiving_data_part_1_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING___RECEIVING_DATA_PART_2))
static icl_stats_t *s_tstsock_test_sending___receiving_data_part_2_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING_LARGE_DATA))
static icl_stats_t *s_tstsock_test_sending_large_data_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING_WITH_CORK))
static icl_stats_t *s_tstsock_test_sending_with_cork_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_TIMEOUT))
static icl_stats_t *s_tstsock_test_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_WRITE_THEN_CLOSE))
static icl_stats_t *s_tstsock_test_write_then_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_WRITE_TO_INVALID_SOCKET))
static icl_stats_t *s_tstsock_test_write_to_invalid_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_READ_ON_BROKEN_PIPE))
static icl_stats_t *s_tstsock_test_read_on_broken_pipe_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_THEN_CLOSE_PART_1))
static icl_stats_t *s_tstsock_read_then_close_part_1_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_THEN_CLOSE_PART_2))
static icl_stats_t *s_tstsock_read_then_close_part_2_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_INPUT_WITH_EVENT_PART_1))
static icl_stats_t *s_tstsock_input_with_event_part_1_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_WITH_EVENT))
static icl_stats_t *s_tstsock_read_with_event_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_WRITE_WITH_EVENT))
static icl_stats_t *s_tstsock_write_with_event_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    tstsock_manager                 (smt_thread_t **thread);
static int    tstsock_catcher                 (smt_thread_t *thread);
static int    tstsock_main_destroy            (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_TSTSOCK)                    ||  defined (TSTSOCK_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_report_result            (smt_thread_t *thread);
static int    action_display_buffers          (smt_thread_t *thread);
static int    action_test_failed_connect      (smt_thread_t *thread);
static int    action_free_conn_socket         (smt_thread_t *thread);
static int    action_test_successful_connect  (smt_thread_t *thread);
static int    action_unnamed_5                (smt_thread_t *thread);
static int    action_free_slave_socket        (smt_thread_t *thread);
static int    action_test_sending___receiving_data_part_1  (smt_thread_t *thread);
static int    action_test_sending___receiving_data_part_2  (smt_thread_t *thread);
static int    action_test_sending_large_data  (smt_thread_t *thread);
static int    action_test_sending_with_cork   (smt_thread_t *thread);
static int    action_test_timeout             (smt_thread_t *thread);
static int    action_test_write_then_close    (smt_thread_t *thread);
static int    action_test_write_to_invalid_socket  (smt_thread_t *thread);
static int    action_test_read_on_broken_pipe  (smt_thread_t *thread);
static int    action_read_then_close_part_1   (smt_thread_t *thread);
static int    action_read_then_close_part_2   (smt_thread_t *thread);
static int    action_input_with_event_part_1  (smt_thread_t *thread);
static int    action_read_with_event          (smt_thread_t *thread);
static int    action_write_with_event         (smt_thread_t *thread);
//  Function definitions                                                       


#define BUFFER_MAX      1024

smt_socket_t
    *master_sock = NULL,
    *slave_sock = NULL,
    *conn_sock = NULL;

int
main (int argc, char *argv [])
{
    icl_console_mode (ICL_CONSOLE_DATE, FALSE);
    icl_console_mode (ICL_CONSOLE_TIME, FALSE);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);

    icl_system_initialise (argc, argv);
    smt_socket_show_animation         (TRUE);
    smt_socket_request_show_animation (TRUE);
    smt_initialise ();
    tstsock_init ();
    smt_wait (0);
    icl_system_terminate ();
    return (0);
}
    
void
tstsock_term (void)
{

}

int 
tstsock_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_main_context_cache = icl_cache_get (sizeof (tstsock_main_context_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    smt_thread_t
        *thread;

    thread = tstsock_main_thread_new ();
    if (thread) {
        smt_thread_wake (thread);
        smt_thread_unlink (&thread);
    }
    
    }
    icl_system_register (NULL, tstsock_term);
    
    return rc;
}

void
tstsock_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
    s_animate = animate;
#endif
}

smt_thread_t *
tstsock_main_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (tstsock_manager,
                             tstsock_catcher,
                             tstsock_main_destroy);
                             

#if (defined (TSTSOCK_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "tstsock",
                main_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_main_context_cache);
        memset (thread->context, 0, sizeof (tstsock_main_context_t));
        ((tstsock_main_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((tstsock_main_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
tstsock_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == tstsock_manager);
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
            case 12:
                thread->place = SMT_TERM_PLACE;
                break;
            case 4:
                thread->place = 2;
                break;
            case 7:
            case 3:
                thread->place = 5;
                break;
            case 6:
            case 10:
                thread->place = 8;
                break;
            case 9:
            case 13:
                thread->place = 11;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Main thread
            if (!thread->shutting_down) {
{
#define tcb ((tstsock_main_context_t *) thread->context)

    tcb->upbuf   = icl_mem_alloc (BUFFER_MAX);
    tcb->downbuf = icl_mem_alloc (BUFFER_MAX);
    thread->animate = TRUE;
    the_next_event = ok_event;
        
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
        if (thread->place < 5)                  //  After Init state
            thread->place = 2;
        else
        if (thread->place < 8)                  //  State 2 state
            thread->place = 5;
        else
        if (thread->place < 11)                 //  State 3 state
            thread->place = 8;
        else
            thread->place = 11;
    }

    switch (thread->place) {
        case 2:                         //  after init state                    
{
#define tcb ((tstsock_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 4; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 5:                         //  state 2 state                       
{
#define tcb ((tstsock_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case input_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 6; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 8:                         //  state 3 state                       
{
#define tcb ((tstsock_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case read_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 9; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 10; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 11:                        //  state 4 state                       
{
#define tcb ((tstsock_main_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case write_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 12; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (TSTSOCK_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_test_failed_connect (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_free_conn_socket (thread);

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

                case 4:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_test_successful_connect (thread);

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

                case 5:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 6:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 6);
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

                case 7:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 8:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_free_conn_socket (thread);

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

                case 9:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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

                case 10:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 8);
#endif
                    rc = action_test_sending___receiving_data_part_1 (thread);

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

                case 11:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 12:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_test_sending___receiving_data_part_2 (thread);

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

                case 13:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 14:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_display_buffers (thread);

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

                case 15:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_test_sending_large_data (thread);

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

                case 16:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 17:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_test_sending_with_cork (thread);

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

                case 18:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 19:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_display_buffers (thread);

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

                case 20:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_test_timeout (thread);

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

                case 21:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 22:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_test_write_then_close (thread);

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

                case 23:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 24:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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

                case 25:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_test_write_to_invalid_socket (thread);

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

                case 26:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 27:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_test_read_on_broken_pipe (thread);

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

                case 28:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 29:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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

                case 30:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_read_then_close_part_1 (thread);

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

                case 31:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 32:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 17);
#endif
                    rc = action_read_then_close_part_2 (thread);

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

                case 33:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 34:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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

                case 35:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 18);
#endif
                    rc = action_input_with_event_part_1 (thread);

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

                case 36:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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

                case 37:                        //  After actions              
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 19);
#endif
                    rc = action_read_with_event (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 20);
#endif
                    rc = action_write_with_event (thread);

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
        case 12:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_result (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_free_conn_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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
        case 13:
        case 10:
        case 7:
        case 4:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_free_conn_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (TSTSOCK_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_free_slave_socket (thread);

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
tstsock_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
tstsock_main_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (TSTSOCK_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "tstsock",
                 main_thread_name);

#endif
{
#define tcb ((tstsock_main_context_t *) thread->context)

        icl_mem_free (tcb->upbuf);
        icl_mem_free (tcb->downbuf);
        smt_socket_destroy (&master_sock);
        smt_socket_destroy (&slave_sock);
        smt_socket_destroy (&conn_sock);
    
#undef  tcb
}
    if (--((tstsock_main_context_t *) thread->context)->links == 0)
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
                case 4: *event_id = 5;   *nextstate_id = 1;   break;
            }
          }
        else
        if (thread->place < 8)
          {
            *state_id = 2;
            switch (thread->place) {
                case 6: *event_id = 2;   *nextstate_id = 3;   break;
                case 7: *event_id = 5;   *nextstate_id = 2;   break;
            }
          }
        else
        if (thread->place < 11)
          {
            *state_id = 3;
            switch (thread->place) {
                case 9: *event_id = 3;   *nextstate_id = 4;   break;
                case 10: *event_id = 5;   *nextstate_id = 3;   break;
            }
          }
        else
          {
            *state_id = 4;
            switch (thread->place) {
                case 12: *event_id = 4;   break;
                case 13: *event_id = 5;   *nextstate_id = 4;   break;
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
    icl_console_print ("tstsock: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_TSTSOCK))

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
            "tstsock",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "tstsock",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "tstsock",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_TSTSOCK)                      ||  defined (TSTSOCK_TRACE))
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
                                       "tstsock",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "tstsock",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "tstsock",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_report_result (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_REPORT_RESULT))
    icl_stats_inc ("tstsock_report_result", &s_tstsock_report_result_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("%s", smt_thread_error (thread));
    
}
#undef  tcb
    return _rc;
}

static int
action_display_buffers (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_DISPLAY_BUFFERS))
    icl_stats_inc ("tstsock_display_buffers", &s_tstsock_display_buffers_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

        tcb->upbuf [slave_sock->io_size] = 0;
        icl_console_print ("Up buffer: <%s>", tcb->upbuf);
        tcb->downbuf [conn_sock->io_size] = 0;
        icl_console_print ("Down buffer: <%s>", tcb->downbuf);
    
}
#undef  tcb
    return _rc;
}

static int
action_test_failed_connect (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_FAILED_CONNECT))
    icl_stats_inc ("tstsock_test_failed_connect", &s_tstsock_test_failed_connect_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test failed connect.");
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_free_conn_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_FREE_CONN_SOCKET))
    icl_stats_inc ("tstsock_free_conn_socket", &s_tstsock_free_conn_socket_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_destroy (&conn_sock);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_successful_connect (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SUCCESSFUL_CONNECT))
    icl_stats_inc ("tstsock_test_successful_connect", &s_tstsock_test_successful_connect_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test successful connect.");
    master_sock = smt_socket_new ();
    smt_socket_passive (master_sock, thread, TSTSOCK_PORT, NULL, 5);
                
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_5 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_UNNAMED_5))
    icl_stats_inc ("tstsock_unnamed_5", &s_tstsock_unnamed_5_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_free_slave_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_FREE_SLAVE_SOCKET))
    icl_stats_inc ("tstsock_free_slave_socket", &s_tstsock_free_slave_socket_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_destroy (&slave_sock);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_sending___receiving_data_part_1 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING___RECEIVING_DATA_PART_1))
    icl_stats_inc ("tstsock_test_sending___receiving_data_part_1", &s_tstsock_test_sending___receiving_data_part_1_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_sending___receiving_data_part_2 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING___RECEIVING_DATA_PART_2))
    icl_stats_inc ("tstsock_test_sending___receiving_data_part_2", &s_tstsock_test_sending___receiving_data_part_2_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test sending & receiving data.");
    smt_socket_request_read  (thread, conn_sock, 1000, 1, 255, tcb->downbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 2, (byte *) "Up", SMT_NULL_EVENT);
 
    smt_socket_request_write (thread, slave_sock, 2000, 4, (byte *) "Down", SMT_NULL_EVENT);
    smt_socket_request_read  (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_sending_large_data (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING_LARGE_DATA))
    icl_stats_inc ("tstsock_test_sending_large_data", &s_tstsock_test_sending_large_data_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    static byte
        data [LARGE_SIZE];
        
    icl_console_print ("Test sending large data.");
    smt_socket_request_write (thread, conn_sock, 2000, LARGE_SIZE, data, SMT_NULL_EVENT);
 
    smt_socket_request_read  (thread, slave_sock, 1000, LARGE_SIZE, LARGE_SIZE, data, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_sending_with_cork (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_SENDING_WITH_CORK))
    icl_stats_inc ("tstsock_test_sending_with_cork", &s_tstsock_test_sending_with_cork_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test sending with cork.");
    smt_socket_request_write (thread, conn_sock, 2000, 3, (byte *) "One", SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 3, (byte *) "Two", SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 5, (byte *) "Three", SMT_NULL_EVENT);
 
    smt_socket_request_read  (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_TIMEOUT))
    icl_stats_inc ("tstsock_test_timeout", &s_tstsock_test_timeout_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test timeout.");
    smt_socket_request_read  (thread, conn_sock, 2000, 255, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 1000, 5, (byte *) "Start", SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_write_then_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_WRITE_THEN_CLOSE))
    icl_stats_inc ("tstsock_test_write_then_close", &s_tstsock_test_write_then_close_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test write then close.");
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
    smt_socket_request_close (thread, &conn_sock, 5000, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_write_to_invalid_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_WRITE_TO_INVALID_SOCKET))
    icl_stats_inc ("tstsock_test_write_to_invalid_socket", &s_tstsock_test_write_to_invalid_socket_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test write to invalid socket.");
    slave_sock = smt_socket_new ();
    smt_socket_passive (slave_sock, thread, TSTSOCK_PORT_ERR, NULL, 5);
    smt_socket_request_write (thread, slave_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_test_read_on_broken_pipe (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_TEST_READ_ON_BROKEN_PIPE))
    icl_stats_inc ("tstsock_test_read_on_broken_pipe", &s_tstsock_test_read_on_broken_pipe_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test read on broken pipe.");
    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_read_then_close_part_1 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_THEN_CLOSE_PART_1))
    icl_stats_inc ("tstsock_read_then_close_part_1", &s_tstsock_read_then_close_part_1_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Test read then close.");
    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_read_then_close_part_2 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_THEN_CLOSE_PART_2))
    icl_stats_inc ("tstsock_read_then_close_part_2", &s_tstsock_read_then_close_part_2_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_request_read  (thread, conn_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_close (thread, &conn_sock, 5000, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_input_with_event_part_1 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_INPUT_WITH_EVENT_PART_1))
    icl_stats_inc ("tstsock_input_with_event_part_1", &s_tstsock_input_with_event_part_1_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    icl_console_print ("Input with event.");
    smt_socket_request_accept (thread, master_sock, 0, input_event, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_read_with_event (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_READ_WITH_EVENT))
    icl_stats_inc ("tstsock_read_with_event", &s_tstsock_read_with_event_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, read_event);
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
                
}
#undef  tcb
    return _rc;
}

static int
action_write_with_event (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_TSTSOCK)  ||  defined (BASE_STATS_TSTSOCK_WRITE_WITH_EVENT))
    icl_stats_inc ("tstsock_write_with_event", &s_tstsock_write_with_event_stats);
#endif        
#define tcb ((tstsock_main_context_t *) thread->context)
{

    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", write_event);
                
}
#undef  tcb
    return _rc;
}

