/*---------------------------------------------------------------------------
    smt_echo.c - functions for smt_echo.

    Generated from smt_echo.smt by smt_gen.gsl using GSL/4.

    
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
#include "smt_echo.h"

#define master_after_init_state                    1
#define master_new_connection_state                2

#define slave_after_init_state                     1
#define slave_active_connection_state              3

enum {
    ok_event                                       = 1,
    shutdown_event                                 = 2,
    exception_event                                = 3,
    socket_error_event                             = 4,
    other_error_event                              = 5,
    interrupt_event                                = 6,
};

#define laststate                                  3
#define lastevent                                  6
#define lastplace                                  28

#define smt_echo_agent_name                        "Smt-Echo"

#define master_thread_name                         "Master"
#define slave_thread_name                          "Slave"

#define after_init_state_name                      "After-Init"
#define new_connection_state_name                  "New-Connection"
#define after_init_state_name                      "After-Init"
#define active_connection_state_name               "Active-Connection"

#define ok_event_name                              "Ok"
#define shutdown_event_name                        "Shutdown"
#define exception_event_name                       "Exception"
#define socket_error_event_name                    "Socket-Error"
#define other_error_event_name                     "Other-Error"
#define interrupt_event_name                       "Interrupt"

#define report_error_action_name                   "Report-Error"
#define accept_connection_action_name              "Accept-Connection"
#define create_slave_thread_action_name            "Create-Slave-Thread"
#define request_read_from_socket_action_name       "Request-Read-From-Socket"
#define write_socket_data_action_name              "Write-Socket-Data"
#define send_interrupted_message_action_name       "Send-Interrupted-Message"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised


#include "smt_socket.h"

static icl_cache_t
    *s_master_context_cache = NULL;

typedef struct {
    int
        links;

        smt_socket_t 
            *master_socket;             //  Master socket
        smt_socket_t 
            *client_socket;             //  Connected socket
    
} smt_echo_master_context_t;

static icl_cache_t
    *s_slave_context_cache = NULL;

typedef struct {
    int
        links;

        smt_socket_t 
            *client_socket;             //  Client socket from master thread 
        byte 
            buffer [4096];              //  Buffer for network data
    
} smt_echo_slave_context_t;


static const char *thread_name [] = {
    "<NULL>",
    master_thread_name,
    slave_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    after_init_state_name,
    new_connection_state_name,
    active_connection_state_name
};

static const char *event_name [] = {
    "<NULL>",
    ok_event_name,
    shutdown_event_name,
    exception_event_name,
    socket_error_event_name,
    other_error_event_name,
    interrupt_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO)                 ||  defined (BASE_TRACE)  || defined (BASE_TRACE_SMT_ECHO)   ||  defined (SMT_ECHO_TRACE))
static const char *action_name [] = {
    "<NULL>",
    report_error_action_name,
    accept_connection_action_name,
    create_slave_thread_action_name,
    request_read_from_socket_action_name,
    write_socket_data_action_name,
    send_interrupted_message_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_REPORT_ERROR))
static icl_stats_t *s_smt_echo_report_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_ACCEPT_CONNECTION))
static icl_stats_t *s_smt_echo_accept_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_CREATE_SLAVE_THREAD))
static icl_stats_t *s_smt_echo_create_slave_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_REQUEST_READ_FROM_SOCKET))
static icl_stats_t *s_smt_echo_request_read_from_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_WRITE_SOCKET_DATA))
static icl_stats_t *s_smt_echo_write_socket_data_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_SEND_INTERRUPTED_MESSAGE))
static icl_stats_t *s_smt_echo_send_interrupted_message_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    smt_echo_manager                (smt_thread_t **thread);
static int    smt_echo_catcher                (smt_thread_t *thread);
static int    smt_echo_master_destroy         (smt_thread_t *thread);
static int    smt_echo_slave_destroy          (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_ECHO)                   ||  defined (SMT_ECHO_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_report_error             (smt_thread_t *thread);
static int    action_accept_connection        (smt_thread_t *thread);
static int    action_create_slave_thread      (smt_thread_t *thread);
static int    action_request_read_from_socket  (smt_thread_t *thread);
static int    action_write_socket_data        (smt_thread_t *thread);
static int    action_send_interrupted_message  (smt_thread_t *thread);
//  Function definitions                                                       


int main (int argc, char *argv [])
{
    int
        rc;
        
    icl_system_initialise (argc, argv);
    if (argc != 2)
        icl_console_print ("Syntax: smt_echo <service/port>");
    else {
        smt_initialise ();
        
        rc = smt_echo_init (argv [1]);
        smt_wait (0);
        smt_echo_term ();
        smt_wait (0);
    }
    icl_system_terminate ();
    return (0);
}

void
smt_echo_term (void)
{

}

int 
smt_echo_init (
    char * port)
{
    int
        rc = SMT_OK;                    //  Return code

    s_master_context_cache = icl_cache_get (sizeof (smt_echo_master_context_t));
    s_slave_context_cache = icl_cache_get (sizeof (smt_echo_slave_context_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    smt_thread_t
        *thread;                        //  Master thread
    smt_echo_master_context_t
        *tcb;                           //  Master thread context
        
    smt_echo_animate (TRUE);
    //  Deregister SIGINT default shutdown signal
    smt_signal_deregister (SIGINT);
    //  Create and initialise master thread
    thread = smt_echo_master_thread_new ();
    if (thread) {
        tcb = thread->context;
        //  Create listening socket
        tcb->master_socket = smt_socket_new ();
        if (tcb->master_socket &&
            (smt_socket_passive (tcb->master_socket,
                thread, port, NULL, 5) == 0)) {
            icl_console_print ("I: opened echo port %s", port);
            //  Wake newly created thread and drop reference to it
            smt_thread_wake (thread);
            smt_thread_unlink (&thread);
        } else {
            icl_console_print ("Could not open ECHO port %s", port);
            icl_console_print ("Reason: %s", smt_thread_error (thread));
            smt_thread_destroy (&thread);
            rc = SMT_SOCKET_ERROR;
        }
    }
    else {
        icl_console_print ("Error starting echo agent.");
        rc = SMT_THREAD_INVALID;
    }

    }
    icl_system_register (NULL, smt_echo_term);
    
    return rc;
}

void
smt_echo_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
    s_animate = animate;
#endif
}

smt_thread_t *
smt_echo_master_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_echo_manager,
                             smt_echo_catcher,
                             smt_echo_master_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (SMT_ECHO_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_echo",
                master_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_master_context_cache);
        memset (thread->context, 0, sizeof (smt_echo_master_context_t));
        ((smt_echo_master_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_echo_master_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
smt_echo_slave_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_echo_manager,
                             smt_echo_catcher,
                             smt_echo_slave_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (SMT_ECHO_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_echo",
                slave_thread_name);
#endif

        thread->place   = 14;
        thread->context = icl_mem_cache_alloc (s_slave_context_cache);
        memset (thread->context, 0, sizeof (smt_echo_slave_context_t));
        ((smt_echo_slave_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_echo_slave_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
smt_echo_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == smt_echo_manager);
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
            case 17:
            case 18:
            case 10:
            case 11:
            case 12:
            case 13:
            case 4:
            case 5:
            case 6:
            case 7:
            case 20:
            case 21:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 19:
                thread->place = SMT_TERM_PLACE;
                break;
            case 16:
            case 23:
                thread->place = 22;
                break;
            case 9:
            case 3:
                thread->place = 8;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Master thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_echo_master_context_t *) thread->context)

        //  Start the FSM
        smt_thread_set_next_event (thread, ok_event);
    
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
        if (thread->place < 8)                  //  After Init state
            thread->place = 2;
        else
        if (thread->place < 14)                 //  New Connection state
            thread->place = 8;
        else
        if (thread->place == 14) { //               Slave thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_echo_slave_context_t *) thread->context)

        //  Register to receive interrupt_event on SIGINT
        smt_signal_register (SIGINT, thread, interrupt_event);
        //  Start the FSM
        smt_thread_set_next_event (thread, ok_event);
    
#undef  tcb
}
            }
            thread->place = 15;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
        if (thread->place < 22)                 //  After Init state
            thread->place = 15;
        else
            thread->place = 22;
    }

    switch (thread->place) {
        case 2:                         //  after init state                    
{
#define tcb ((smt_echo_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 5; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case other_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 8:                         //  new connection state                
{
#define tcb ((smt_echo_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 9; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 10; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 11; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 12; break;
                case other_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 15:                        //  after init state                    
{
#define tcb ((smt_echo_slave_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 16; break;
                case interrupt_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 17; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 18; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 19; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 20; break;
                case other_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 21; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 22:                        //  active connection state             
{
#define tcb ((smt_echo_slave_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 23; break;
                case interrupt_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 24; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 25; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 26; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 27; break;
                case other_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_ECHO_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 28; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_accept_connection (thread);

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

                case 2:                         //  After actions              
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_create_slave_thread (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_accept_connection (thread);

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
        case 10:
        case 11:
        case 4:
        case 5:
        case 25:
        case 26:
        case 18:
        case 19:
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
        case 12:
        case 13:
        case 6:
        case 7:
        case 27:
        case 28:
        case 20:
        case 21:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_report_error (thread);

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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 16:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_request_read_from_socket (thread);

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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 23:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_write_socket_data (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_request_read_from_socket (thread);

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
        case 24:
        case 17:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (SMT_ECHO_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_send_interrupted_message (thread);

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

                case 2:                         //  After actions              
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
smt_echo_catcher (smt_thread_t *thread)
{
    switch (thread->result) {
        case 0:
            return SMT_OK;
        case SMT_SOCKET_ERROR:
            thread->_exception_event = socket_error_event; break;
        default:
            thread->_exception_event = other_error_event;
    }
    thread->_exception_raised = TRUE;
    
    return SMT_OK;
}


static int    
smt_echo_master_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_ECHO_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_echo",
                 master_thread_name);

#endif
{
#define tcb ((smt_echo_master_context_t *) thread->context)

        //  Deallocate socket created during initialisation
        smt_socket_destroy (&tcb->master_socket);
    
#undef  tcb
}
    if (--((smt_echo_master_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
smt_echo_slave_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_ECHO_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_echo",
                 slave_thread_name);

#endif
{
#define tcb ((smt_echo_slave_context_t *) thread->context)

        //  Deallocate client socket we got from the master thread
        smt_socket_destroy (&tcb->client_socket);
    
#undef  tcb
}
    if (--((smt_echo_slave_context_t *) thread->context)->links == 0)
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
    
    if (thread->place < 14)
      {
        *thread_type = 1;
        if (thread->place < 8)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 2;   break;
                case 5: *event_id = 3;   break;
                case 6: *event_id = 4;   break;
                case 7: *event_id = 5;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 9: *event_id = 1;   *nextstate_id = 2;   break;
                case 10: *event_id = 2;   break;
                case 11: *event_id = 3;   break;
                case 12: *event_id = 4;   break;
                case 13: *event_id = 5;   break;
            }
          }
      }
    else
      {
        *thread_type = 2;
        if (thread->place < 22)
          {
            *state_id = 1;
            switch (thread->place) {
                case 16: *event_id = 1;   *nextstate_id = 3;   break;
                case 17: *event_id = 6;   break;
                case 18: *event_id = 2;   break;
                case 19: *event_id = 3;   break;
                case 20: *event_id = 4;   break;
                case 21: *event_id = 5;   break;
            }
          }
        else
          {
            *state_id = 3;
            switch (thread->place) {
                case 23: *event_id = 1;   *nextstate_id = 3;   break;
                case 24: *event_id = 6;   break;
                case 25: *event_id = 2;   break;
                case 26: *event_id = 3;   break;
                case 27: *event_id = 4;   break;
                case 28: *event_id = 5;   break;
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
    icl_console_print ("smt_echo: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_ECHO))

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
            "smt_echo",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "smt_echo",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "smt_echo",
              thread_name [find_thread_type (thread->place)],
              action_name [action_id]);
}

static int
find_thread_type (smt_place_t place)
{
    int
        thread_type = 0;
        
    if (place < 14)
        thread_type = 1;
    else
        thread_type = 2;
    return thread_type;
}

#endif
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_ECHO)                     ||  defined (SMT_ECHO_TRACE))
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
                                       "smt_echo",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "smt_echo",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "smt_echo",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_report_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_REPORT_ERROR))
    icl_stats_inc ("smt_echo_report_error", &s_smt_echo_report_error_stats);
#endif        
{

    icl_console_print (smt_thread_error (thread));

}
    return _rc;
}

static int
action_accept_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_ACCEPT_CONNECTION))
    icl_stats_inc ("smt_echo_accept_connection", &s_smt_echo_accept_connection_stats);
#endif        
#define tcb ((smt_echo_master_context_t *) thread->context)
{

    //  Create an ACCEPT request.
    //  SMT will accept new connections on master_socket and send our thread 
    //  an "ok" event on connection.
    smt_socket_request_accept (thread,
                               tcb->master_socket,
                               0,
                               ok_event,
                               &tcb->client_socket);

}
#undef  tcb
    return _rc;
}

static int
action_create_slave_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_CREATE_SLAVE_THREAD))
    icl_stats_inc ("smt_echo_create_slave_thread", &s_smt_echo_create_slave_thread_stats);
#endif        
#define tcb ((smt_echo_master_context_t *) thread->context)
{

    smt_thread_t
        *slave_thread;                  //  Newly created slave thread
    smt_echo_slave_context_t
        *slave_tcb;                     //  And it's context block
    
    //  Create slave thread
    slave_thread = smt_echo_slave_thread_new ();
    if (slave_thread) {
        slave_tcb = slave_thread->context;
        //  Pass client_socket returned by ACCEPT request to slave thread
        slave_tcb->client_socket = tcb->client_socket;
        //  Wake the newly created thread
        smt_thread_wake (slave_thread);
        smt_thread_unlink (&slave_thread);
    }

}
#undef  tcb
    return _rc;
}

static int
action_request_read_from_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_REQUEST_READ_FROM_SOCKET))
    icl_stats_inc ("smt_echo_request_read_from_socket", &s_smt_echo_request_read_from_socket_stats);
#endif        
#define tcb ((smt_echo_slave_context_t *) thread->context)
{

    //  Create an asynchronous READ request.
    //  Requests that SMT read 1-4096 bytes from client_socket into tcb->buffer
    //  and deliver an "ok" event to us when done.
    smt_socket_request_read (thread, tcb->client_socket, 0,
                             1, 4096, tcb->buffer,
                             ok_event);

}
#undef  tcb
    return _rc;
}

static int
action_write_socket_data (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_WRITE_SOCKET_DATA))
    icl_stats_inc ("smt_echo_write_socket_data", &s_smt_echo_write_socket_data_stats);
#endif        
#define tcb ((smt_echo_slave_context_t *) thread->context)
{

    //  Create a synchronous WRITE request.
    //  Requests that SMT write io_size bytes from tcb->buffer to
    //  client_socket.  Blocks the thread until the request completes.
    //  XXX io_size comes from the previously completed socket request
    //  XXX this is confusing, there should be a better way to get this
    //  XXX information.
    smt_socket_request_write (thread, tcb->client_socket, 0, 
                              tcb->client_socket->io_size, tcb->buffer, 
                              SMT_NULL_EVENT);

}
#undef  tcb
    return _rc;
}

static int
action_send_interrupted_message (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_ECHO)  ||  defined (BASE_STATS_SMT_ECHO_SEND_INTERRUPTED_MESSAGE))
    icl_stats_inc ("smt_echo_send_interrupted_message", &s_smt_echo_send_interrupted_message_stats);
#endif        
#define tcb ((smt_echo_slave_context_t *) thread->context)
{

    //  Called on receipt of an "interrupt" event.
    //  Send a "server is shutting down" message to the client.
    #define INTERRUPTED_MESSAGE "Server interrupted!\n"
    smt_socket_request_write (thread, tcb->client_socket, 0, 
                              sizeof (INTERRUPTED_MESSAGE), 
                              (byte *)INTERRUPTED_MESSAGE, 
                              SMT_NULL_EVENT);

}
#undef  tcb
    return _rc;
}

