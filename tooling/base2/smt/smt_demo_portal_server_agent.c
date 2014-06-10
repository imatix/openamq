/*---------------------------------------------------------------------------
    smt_demo_portal_server_agent.c - functions for smt_demo_portal_server_agent.

    Generated from smt_demo_portal_server_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "smt_demo_portal_server_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define print_m ((smt_demo_portal_server_agent_print_t *) thread->method->data)

#define destroy_m ((smt_demo_portal_server_agent_destroy_t *) thread->method->data)

#define _smt_demo_portal_unbind_m ((smt_demo_portal_server_agent__smt_demo_portal_unbind_t *) thread->method->data)

#define hello_m ((smt_demo_portal_server_agent_hello_t *) thread->method->data)

enum {
    shutdown_event                                 = 1,
    destroy_m_event                                = 2,
    print_m_event                                  = 3,
    _smt_demo_portal_unbind_m_event                = 4,
    hello_m_event                                  = 5,
};

#define laststate                                  2
#define lastevent                                  5
#define lastplace                                  13

#define smt_demo_portal_server_agent_agent_name    "Smt-Demo-Portal-Server-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define shutdown_event_name                        "Shutdown"
#define destroy_m_event_name                       "Destroy-M"
#define print_m_event_name                         "Print-M"
#define _smt_demo_portal_unbind_m_event_name       "-Smt-Demo-Portal-Unbind-M"
#define hello_m_event_name                         "Hello-M"

#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_3_action_name                      "Unnamed-3"
#define print_action_name                          "Print"
#define unnamed_5_action_name                      "Unnamed-5"
#define unnamed_6_action_name                      "Unnamed-6"
#define _smt_demo_portal_unbind_action_name        "-Smt-Demo-Portal-Unbind"
#define unnamed_8_action_name                      "Unnamed-8"
#define hello_action_name                          "Hello"
#define unnamed_10_action_name                     "Unnamed-10"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    smt_demo_portal_t *  portal;
    char *         string;
  }
smt_demo_portal_server_agent_print_t;

typedef struct
  {
    char *         file;
    size_t         line;
  }
smt_demo_portal_server_agent_destroy_t;

typedef struct
  {
    smt_demo_portal_t *  portal;
  }
smt_demo_portal_server_agent__smt_demo_portal_unbind_t;

typedef struct
  {
    smt_demo_portal_t *  portal;
  }
smt_demo_portal_server_agent_hello_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        smt_demo_portal_server_t
            *smt_demo_portal_server;
    
} smt_demo_portal_server_agent_class_context_t;

static icl_cache_t
    *s_print_cache = NULL;
static icl_cache_t
    *s_destroy_cache = NULL;
static icl_cache_t
    *s__smt_demo_portal_unbind_cache = NULL;
static icl_cache_t
    *s_hello_cache = NULL;

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
    print_m_event_name,
    _smt_demo_portal_unbind_m_event_name,
    hello_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT)   ||  defined (BASE_TRACE)  || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT)   ||  defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    destroy_action_name,
    free_action_name,
    unnamed_3_action_name,
    print_action_name,
    unnamed_5_action_name,
    unnamed_6_action_name,
    _smt_demo_portal_unbind_action_name,
    unnamed_8_action_name,
    hello_action_name,
    unnamed_10_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_DESTROY))
static icl_stats_t *s_smt_demo_portal_server_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_FREE))
static icl_stats_t *s_smt_demo_portal_server_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_3))
static icl_stats_t *s_smt_demo_portal_server_agent_unnamed_3_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_PRINT))
static icl_stats_t *s_smt_demo_portal_server_agent_print_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_5))
static icl_stats_t *s_smt_demo_portal_server_agent_unnamed_5_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_6))
static icl_stats_t *s_smt_demo_portal_server_agent_unnamed_6_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT__SMT_DEMO_PORTAL_UNBIND))
static icl_stats_t *s_smt_demo_portal_server_agent__smt_demo_portal_unbind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_8))
static icl_stats_t *s_smt_demo_portal_server_agent_unnamed_8_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_HELLO))
static icl_stats_t *s_smt_demo_portal_server_agent_hello_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_10))
static icl_stats_t *s_smt_demo_portal_server_agent_unnamed_10_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    smt_demo_portal_server_agent_manager  (smt_thread_t **thread);
static int    smt_demo_portal_server_agent_catcher  (smt_thread_t *thread);
static int    smt_demo_portal_server_agent_class_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT)   ||  defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_3                (smt_thread_t *thread);
static int    action_print                    (smt_thread_t *thread);
static int    action_unnamed_5                (smt_thread_t *thread);
static int    action_unnamed_6                (smt_thread_t *thread);
static int    action__smt_demo_portal_unbind  (smt_thread_t *thread);
static int    action_unnamed_8                (smt_thread_t *thread);
static int    action_hello                    (smt_thread_t *thread);
static int    action_unnamed_10               (smt_thread_t *thread);
static int
smt_demo_portal_server_agent_print_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal,
    char *         string);
int
smt_demo_portal_server_agent_print_destructor (void *data);
static int
smt_demo_portal_server_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
smt_demo_portal_server_agent_destroy_destructor (void *data);
static int
smt_demo_portal_server_agent__smt_demo_portal_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal);
int
smt_demo_portal_server_agent__smt_demo_portal_unbind_destructor (void *data);
static int
smt_demo_portal_server_agent_hello_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal);
int
smt_demo_portal_server_agent_hello_destructor (void *data);
//  Function definitions                                                       

static int
smt_demo_portal_server_agent_print_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal,
    char *         string)
{
    int
        rc = 0;
    smt_demo_portal_server_agent_print_t
        *_message;
        
    //  Possess code for print
    

    
        string = icl_mem_strdup (string);
        
    
    

    _message = icl_mem_cache_alloc (s_print_cache);
    _message->portal                    = portal;
    _message->string                    = string;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_portal_server_agent_print_destructor);
    
    return rc;
}

int
smt_demo_portal_server_agent_print_destructor (void *data)
{
    int
        rc = 0;
smt_demo_portal_t * portal;
char * string;

    portal = ((smt_demo_portal_server_agent_print_t *) data)-> portal;
    string = ((smt_demo_portal_server_agent_print_t *) data)-> string;



    
        icl_mem_free (string);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
smt_demo_portal_server_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    smt_demo_portal_server_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_portal_server_agent_destroy_destructor);
    
    return rc;
}

int
smt_demo_portal_server_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((smt_demo_portal_server_agent_destroy_t *) data)-> file;
    line = ((smt_demo_portal_server_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


static int
smt_demo_portal_server_agent__smt_demo_portal_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal)
{
    int
        rc = 0;
    smt_demo_portal_server_agent__smt_demo_portal_unbind_t
        *_message;
        

    _message = icl_mem_cache_alloc (s__smt_demo_portal_unbind_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_portal_server_agent__smt_demo_portal_unbind_destructor);
    
    return rc;
}

int
smt_demo_portal_server_agent__smt_demo_portal_unbind_destructor (void *data)
{
    int
        rc = 0;
smt_demo_portal_t * portal;

    portal = ((smt_demo_portal_server_agent__smt_demo_portal_unbind_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


static int
smt_demo_portal_server_agent_hello_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_demo_portal_t *  portal)
{
    int
        rc = 0;
    smt_demo_portal_server_agent_hello_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_hello_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          smt_demo_portal_server_agent_hello_destructor);
    
    return rc;
}

int
smt_demo_portal_server_agent_hello_destructor (void *data)
{
    int
        rc = 0;
smt_demo_portal_t * portal;

    portal = ((smt_demo_portal_server_agent_hello_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


int
smt_demo_portal_server_agent_print (
    smt_thread_t * thread,
    smt_demo_portal_t *   portal,
    char *                string)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_portal_server_agent_manager);
        rc = smt_demo_portal_server_agent_print_send (
                thread->reply_queue, print_m_event,
                portal,
                string);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_demo_portal_server_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_portal_server_agent_manager);
        rc = smt_demo_portal_server_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_demo_portal_server_agent__smt_demo_portal_unbind (
    smt_thread_t * thread,
    smt_demo_portal_t *   portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_portal_server_agent_manager);
        rc = smt_demo_portal_server_agent__smt_demo_portal_unbind_send (
                thread->reply_queue, _smt_demo_portal_unbind_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

int
smt_demo_portal_server_agent_hello (
    smt_thread_t * thread,
    smt_demo_portal_t *   portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == smt_demo_portal_server_agent_manager);
        rc = smt_demo_portal_server_agent_hello_send (
                thread->reply_queue, hello_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

void
smt_demo_portal_server_agent_term (void)
{

}

int 
smt_demo_portal_server_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (smt_demo_portal_server_agent_class_context_t));
    s_print_cache = icl_cache_get (sizeof (smt_demo_portal_server_agent_print_t));
    s_destroy_cache = icl_cache_get (sizeof (smt_demo_portal_server_agent_destroy_t));
    s__smt_demo_portal_unbind_cache = icl_cache_get (sizeof (smt_demo_portal_server_agent__smt_demo_portal_unbind_t));
    s_hello_cache = icl_cache_get (sizeof (smt_demo_portal_server_agent_hello_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, smt_demo_portal_server_agent_term);
    
    return rc;
}

void
smt_demo_portal_server_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
smt_demo_portal_server_agent_class_thread_new (
    smt_demo_portal_server_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (smt_demo_portal_server_agent_manager,
                             smt_demo_portal_server_agent_catcher,
                             smt_demo_portal_server_agent_class_destroy);
                             

#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "smt_demo_portal_server_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (smt_demo_portal_server_agent_class_context_t));
        ((smt_demo_portal_server_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)

        tcb->smt_demo_portal_server = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
smt_demo_portal_server_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == smt_demo_portal_server_agent_manager);
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
            case 5:
            case 9:
            case 13:
                thread->place = SMT_TERM_PLACE;
                break;
            case 6:
            case 4:
            case 7:
                thread->place = 2;
                break;
            case 12:
            case 3:
            case 11:
            case 10:
                thread->place = 8;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)

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
        if (thread->place < 8)                  //  Main state
            thread->place = 2;
        else
            thread->place = 8;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case print_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 4; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 5; break;
                case _smt_demo_portal_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case hello_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 8:                         //  shutting down state                 
{
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 9; break;
                case print_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 10; break;
                case _smt_demo_portal_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 11; break;
                case hello_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 12; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 10:
        case 11:
        case 12:
        case 13:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 13)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 9:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_print (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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

                case 4:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 6:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action__smt_demo_portal_unbind (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_hello (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
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
smt_demo_portal_server_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
smt_demo_portal_server_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "smt_demo_portal_server_agent",
                 class_thread_name);

#endif
    if (--((smt_demo_portal_server_agent_class_context_t *) thread->context)->links == 0)
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
        if (thread->place < 8)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 3;   *nextstate_id = 1;   break;
                case 5: *event_id = 2;   break;
                case 6: *event_id = 4;   *nextstate_id = 1;   break;
                case 7: *event_id = 5;   *nextstate_id = 1;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 9: *event_id = 2;   break;
                case 10: *event_id = 3;   *nextstate_id = 2;   break;
                case 11: *event_id = 4;   *nextstate_id = 2;   break;
                case 12: *event_id = 5;   *nextstate_id = 2;   break;
                case 13: *event_id = 1;   break;
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
    icl_console_print ("smt_demo_portal_server_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_SMT_DEMO_PORTAL_SERVER_AGENT))

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
            "smt_demo_portal_server_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "smt_demo_portal_server_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "smt_demo_portal_server_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_DEMO_PORTAL_SERVER_AGENT)   ||  defined (SMT_DEMO_PORTAL_SERVER_AGENT_TRACE))
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
                                       "smt_demo_portal_server_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "smt_demo_portal_server_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "smt_demo_portal_server_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_DESTROY))
    icl_stats_inc ("smt_demo_portal_server_agent_destroy", &s_smt_demo_portal_server_agent_destroy_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{


#define self tcb->smt_demo_portal_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
smt_demo_portal_t
    *smt_demo_portal;               //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->smt_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (self->smt_demo_portal_list))) {
        smt_demo_portal->server_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
}
ipr_looseref_list_destroy (&self->smt_demo_portal_list);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_FREE))
    icl_stats_inc ("smt_demo_portal_server_agent_free", &s_smt_demo_portal_server_agent_free_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{


#define self tcb->smt_demo_portal_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on smt_demo_portal_server object");
    smt_demo_portal_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    smt_demo_portal_server_free (self);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_3))
    icl_stats_inc ("smt_demo_portal_server_agent_unnamed_3", &s_smt_demo_portal_server_agent_unnamed_3_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_print (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_PRINT))
    icl_stats_inc ("smt_demo_portal_server_agent_print", &s_smt_demo_portal_server_agent_print_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{


#define self tcb->smt_demo_portal_server
smt_demo_portal_t *
    portal;
char *
    string;

    portal = print_m->portal;
    string = print_m->string;
        {
icl_console_print ("SERVER PRINT: %s", string);
smt_demo_portal_response_mogrify (portal, string, NULL, TRUE);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_5))
    icl_stats_inc ("smt_demo_portal_server_agent_unnamed_5", &s_smt_demo_portal_server_agent_unnamed_5_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_6 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_6))
    icl_stats_inc ("smt_demo_portal_server_agent_unnamed_6", &s_smt_demo_portal_server_agent_unnamed_6_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action__smt_demo_portal_unbind (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT__SMT_DEMO_PORTAL_UNBIND))
    icl_stats_inc ("smt_demo_portal_server_agent__smt_demo_portal_unbind", &s_smt_demo_portal_server_agent__smt_demo_portal_unbind_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{


#define self tcb->smt_demo_portal_server
smt_demo_portal_t *
    portal;

    portal = _smt_demo_portal_unbind_m->portal;
        {
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    smt_demo_portal_unlink (&portal);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_8))
    icl_stats_inc ("smt_demo_portal_server_agent_unnamed_8", &s_smt_demo_portal_server_agent_unnamed_8_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_hello (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_HELLO))
    icl_stats_inc ("smt_demo_portal_server_agent_hello", &s_smt_demo_portal_server_agent_hello_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{


#define self tcb->smt_demo_portal_server
smt_demo_portal_t *
    portal;

    portal = hello_m->portal;
        {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT)  ||  defined (BASE_STATS_SMT_DEMO_PORTAL_SERVER_AGENT_UNNAMED_10))
    icl_stats_inc ("smt_demo_portal_server_agent_unnamed_10", &s_smt_demo_portal_server_agent_unnamed_10_stats);
#endif        
#define tcb ((smt_demo_portal_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

