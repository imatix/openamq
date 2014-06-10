/*---------------------------------------------------------------------------
    http_access_dos_agent.c - functions for http_access_dos_agent.

    Generated from http_access_dos_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "http_access_dos_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define destroy_m ((http_access_dos_agent_destroy_t *) thread->method->data)

#define announce_m ((http_access_dos_agent_announce_t *) thread->method->data)

#define open_m ((http_access_dos_agent_open_t *) thread->method->data)

#define close_m ((http_access_dos_agent_close_t *) thread->method->data)

#define _http_access_module_unbind_m ((http_access_dos_agent__http_access_module_unbind_t *) thread->method->data)

#define before_m ((http_access_dos_agent_before_t *) thread->method->data)

#define after_m ((http_access_dos_agent_after_t *) thread->method->data)

enum {
    shutdown_event                                 = 1,
    destroy_m_event                                = 2,
    announce_m_event                               = 3,
    open_m_event                                   = 4,
    close_m_event                                  = 5,
    _http_access_module_unbind_m_event             = 6,
    before_m_event                                 = 7,
    after_m_event                                  = 8,
};

#define laststate                                  2
#define lastevent                                  8
#define lastplace                                  19

#define http_access_dos_agent_agent_name           "Http-Access-Dos-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define shutdown_event_name                        "Shutdown"
#define destroy_m_event_name                       "Destroy-M"
#define announce_m_event_name                      "Announce-M"
#define open_m_event_name                          "Open-M"
#define close_m_event_name                         "Close-M"
#define _http_access_module_unbind_m_event_name    "-Http-Access-Module-Unbind-M"
#define before_m_event_name                        "Before-M"
#define after_m_event_name                         "After-M"

#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_3_action_name                      "Unnamed-3"
#define unnamed_4_action_name                      "Unnamed-4"
#define announce_action_name                       "Announce"
#define unnamed_6_action_name                      "Unnamed-6"
#define open_action_name                           "Open"
#define unnamed_8_action_name                      "Unnamed-8"
#define close_action_name                          "Close"
#define unnamed_10_action_name                     "Unnamed-10"
#define _http_access_module_unbind_action_name     "-Http-Access-Module-Unbind"
#define unnamed_12_action_name                     "Unnamed-12"
#define before_action_name                         "Before"
#define unnamed_14_action_name                     "Unnamed-14"
#define after_action_name                          "After"
#define unnamed_16_action_name                     "Unnamed-16"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    char *         file;
    size_t         line;
  }
http_access_dos_agent_destroy_t;

typedef struct
  {
    http_access_module_t *  portal;
    smt_log_t *    log;
  }
http_access_dos_agent_announce_t;

typedef struct
  {
    http_access_module_t *  portal;
    http_access_context_t *  context;
  }
http_access_dos_agent_open_t;

typedef struct
  {
    http_access_module_t *  portal;
    http_access_context_t *  context;
  }
http_access_dos_agent_close_t;

typedef struct
  {
    http_access_module_t *  portal;
  }
http_access_dos_agent__http_access_module_unbind_t;

typedef struct
  {
    http_access_module_t *  portal;
    http_access_context_t *  context;
  }
http_access_dos_agent_before_t;

typedef struct
  {
    http_access_module_t *  portal;
    http_access_context_t *  context;
  }
http_access_dos_agent_after_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        http_access_dos_t
            *http_access_dos;
    
} http_access_dos_agent_class_context_t;

static icl_cache_t
    *s_destroy_cache = NULL;
static icl_cache_t
    *s_announce_cache = NULL;
static icl_cache_t
    *s_open_cache = NULL;
static icl_cache_t
    *s_close_cache = NULL;
static icl_cache_t
    *s__http_access_module_unbind_cache = NULL;
static icl_cache_t
    *s_before_cache = NULL;
static icl_cache_t
    *s_after_cache = NULL;

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
    announce_m_event_name,
    open_m_event_name,
    close_m_event_name,
    _http_access_module_unbind_m_event_name,
    before_m_event_name,
    after_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT)    ||  defined (BASE_TRACE)  || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT)   ||  defined (HTTP_ACCESS_DOS_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    destroy_action_name,
    free_action_name,
    unnamed_3_action_name,
    unnamed_4_action_name,
    announce_action_name,
    unnamed_6_action_name,
    open_action_name,
    unnamed_8_action_name,
    close_action_name,
    unnamed_10_action_name,
    _http_access_module_unbind_action_name,
    unnamed_12_action_name,
    before_action_name,
    unnamed_14_action_name,
    after_action_name,
    unnamed_16_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_DESTROY))
static icl_stats_t *s_http_access_dos_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_FREE))
static icl_stats_t *s_http_access_dos_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_3))
static icl_stats_t *s_http_access_dos_agent_unnamed_3_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_4))
static icl_stats_t *s_http_access_dos_agent_unnamed_4_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_ANNOUNCE))
static icl_stats_t *s_http_access_dos_agent_announce_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_6))
static icl_stats_t *s_http_access_dos_agent_unnamed_6_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_OPEN))
static icl_stats_t *s_http_access_dos_agent_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_8))
static icl_stats_t *s_http_access_dos_agent_unnamed_8_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_CLOSE))
static icl_stats_t *s_http_access_dos_agent_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_10))
static icl_stats_t *s_http_access_dos_agent_unnamed_10_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT__HTTP_ACCESS_MODULE_UNBIND))
static icl_stats_t *s_http_access_dos_agent__http_access_module_unbind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_12))
static icl_stats_t *s_http_access_dos_agent_unnamed_12_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_BEFORE))
static icl_stats_t *s_http_access_dos_agent_before_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_14))
static icl_stats_t *s_http_access_dos_agent_unnamed_14_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_AFTER))
static icl_stats_t *s_http_access_dos_agent_after_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_16))
static icl_stats_t *s_http_access_dos_agent_unnamed_16_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    http_access_dos_agent_manager   (smt_thread_t **thread);
static int    http_access_dos_agent_catcher   (smt_thread_t *thread);
static int    http_access_dos_agent_class_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT)      ||  defined (HTTP_ACCESS_DOS_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_3                (smt_thread_t *thread);
static int    action_unnamed_4                (smt_thread_t *thread);
static int    action_announce                 (smt_thread_t *thread);
static int    action_unnamed_6                (smt_thread_t *thread);
static int    action_open                     (smt_thread_t *thread);
static int    action_unnamed_8                (smt_thread_t *thread);
static int    action_close                    (smt_thread_t *thread);
static int    action_unnamed_10               (smt_thread_t *thread);
static int    action__http_access_module_unbind  (smt_thread_t *thread);
static int    action_unnamed_12               (smt_thread_t *thread);
static int    action_before                   (smt_thread_t *thread);
static int    action_unnamed_14               (smt_thread_t *thread);
static int    action_after                    (smt_thread_t *thread);
static int    action_unnamed_16               (smt_thread_t *thread);
static int
http_access_dos_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
http_access_dos_agent_destroy_destructor (void *data);
static int
http_access_dos_agent_announce_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    smt_log_t *    log);
int
http_access_dos_agent_announce_destructor (void *data);
static int
http_access_dos_agent_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context);
int
http_access_dos_agent_open_destructor (void *data);
static int
http_access_dos_agent_close_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context);
int
http_access_dos_agent_close_destructor (void *data);
static int
http_access_dos_agent__http_access_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal);
int
http_access_dos_agent__http_access_module_unbind_destructor (void *data);
static int
http_access_dos_agent_before_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context);
int
http_access_dos_agent_before_destructor (void *data);
static int
http_access_dos_agent_after_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context);
int
http_access_dos_agent_after_destructor (void *data);
//  Function definitions                                                       

static int
http_access_dos_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    http_access_dos_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_destroy_destructor);
    
    return rc;
}

int
http_access_dos_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((http_access_dos_agent_destroy_t *) data)-> file;
    line = ((http_access_dos_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent_announce_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    smt_log_t *    log)
{
    int
        rc = 0;
    http_access_dos_agent_announce_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_announce_cache);
    _message->portal                    = portal;
    _message->log                       = log;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_announce_destructor);
    
    return rc;
}

int
http_access_dos_agent_announce_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
smt_log_t * log;

    portal = ((http_access_dos_agent_announce_t *) data)-> portal;
    log = ((http_access_dos_agent_announce_t *) data)-> log;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context)
{
    int
        rc = 0;
    http_access_dos_agent_open_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_open_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_open_destructor);
    
    return rc;
}

int
http_access_dos_agent_open_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
http_access_context_t * context;

    portal = ((http_access_dos_agent_open_t *) data)-> portal;
    context = ((http_access_dos_agent_open_t *) data)-> context;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent_close_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context)
{
    int
        rc = 0;
    http_access_dos_agent_close_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_close_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_close_destructor);
    
    return rc;
}

int
http_access_dos_agent_close_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
http_access_context_t * context;

    portal = ((http_access_dos_agent_close_t *) data)-> portal;
    context = ((http_access_dos_agent_close_t *) data)-> context;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent__http_access_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal)
{
    int
        rc = 0;
    http_access_dos_agent__http_access_module_unbind_t
        *_message;
        

    _message = icl_mem_cache_alloc (s__http_access_module_unbind_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent__http_access_module_unbind_destructor);
    
    return rc;
}

int
http_access_dos_agent__http_access_module_unbind_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;

    portal = ((http_access_dos_agent__http_access_module_unbind_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent_before_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context)
{
    int
        rc = 0;
    http_access_dos_agent_before_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_before_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_before_destructor);
    
    return rc;
}

int
http_access_dos_agent_before_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
http_access_context_t * context;

    portal = ((http_access_dos_agent_before_t *) data)-> portal;
    context = ((http_access_dos_agent_before_t *) data)-> context;


    icl_mem_free (data);
    return rc;
}


static int
http_access_dos_agent_after_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    http_access_context_t *  context)
{
    int
        rc = 0;
    http_access_dos_agent_after_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_after_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_access_dos_agent_after_destructor);
    
    return rc;
}

int
http_access_dos_agent_after_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
http_access_context_t * context;

    portal = ((http_access_dos_agent_after_t *) data)-> portal;
    context = ((http_access_dos_agent_after_t *) data)-> context;


    icl_mem_free (data);
    return rc;
}


int
http_access_dos_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent_announce (
    smt_thread_t * thread,
    http_access_module_t * portal,
    smt_log_t *           log)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_announce_send (
                thread->reply_queue, announce_m_event,
                portal,
                log);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent_open (
    smt_thread_t * thread,
    http_access_module_t * portal,
    http_access_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_open_send (
                thread->reply_queue, open_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent_close (
    smt_thread_t * thread,
    http_access_module_t * portal,
    http_access_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_close_send (
                thread->reply_queue, close_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent__http_access_module_unbind (
    smt_thread_t * thread,
    http_access_module_t * portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent__http_access_module_unbind_send (
                thread->reply_queue, _http_access_module_unbind_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent_before (
    smt_thread_t * thread,
    http_access_module_t * portal,
    http_access_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_before_send (
                thread->reply_queue, before_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_access_dos_agent_after (
    smt_thread_t * thread,
    http_access_module_t * portal,
    http_access_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_access_dos_agent_manager);
        rc = http_access_dos_agent_after_send (
                thread->reply_queue, after_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

void
http_access_dos_agent_term (void)
{

}

int 
http_access_dos_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (http_access_dos_agent_class_context_t));
    s_destroy_cache = icl_cache_get (sizeof (http_access_dos_agent_destroy_t));
    s_announce_cache = icl_cache_get (sizeof (http_access_dos_agent_announce_t));
    s_open_cache = icl_cache_get (sizeof (http_access_dos_agent_open_t));
    s_close_cache = icl_cache_get (sizeof (http_access_dos_agent_close_t));
    s__http_access_module_unbind_cache = icl_cache_get (sizeof (http_access_dos_agent__http_access_module_unbind_t));
    s_before_cache = icl_cache_get (sizeof (http_access_dos_agent_before_t));
    s_after_cache = icl_cache_get (sizeof (http_access_dos_agent_after_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, http_access_dos_agent_term);
    
    return rc;
}

void
http_access_dos_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
http_access_dos_agent_class_thread_new (
    http_access_dos_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (http_access_dos_agent_manager,
                             http_access_dos_agent_catcher,
                             http_access_dos_agent_class_destroy);
                             

#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "http_access_dos_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (http_access_dos_agent_class_context_t));
        ((http_access_dos_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)

        tcb->http_access_dos = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
http_access_dos_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == http_access_dos_agent_manager);
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
            case 12:
            case 19:
                thread->place = SMT_TERM_PLACE;
                break;
            case 9:
            case 8:
            case 5:
            case 6:
            case 7:
            case 10:
                thread->place = 2;
                break;
            case 18:
            case 16:
            case 17:
            case 3:
            case 15:
            case 13:
            case 14:
                thread->place = 11;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)

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
        if (thread->place < 11)                 //  Main state
            thread->place = 2;
        else
            thread->place = 11;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                case announce_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 5; break;
                case open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                case _http_access_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 8; break;
                case before_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 9; break;
                case after_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 10; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 11:                        //  shutting down state                 
{
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 12; break;
                case announce_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 13; break;
                case open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 14; break;
                case close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 15; break;
                case _http_access_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 16; break;
                case before_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 17; break;
                case after_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 18; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 19; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 19)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 12:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_announce (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action__http_access_module_unbind (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_before (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
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
        case 10:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_after (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_unnamed_16 (thread);

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
http_access_dos_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
http_access_dos_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (HTTP_ACCESS_DOS_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "http_access_dos_agent",
                 class_thread_name);

#endif
    if (--((http_access_dos_agent_class_context_t *) thread->context)->links == 0)
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
        if (thread->place < 11)
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
                case 10: *event_id = 8;   *nextstate_id = 1;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 12: *event_id = 2;   break;
                case 13: *event_id = 3;   *nextstate_id = 2;   break;
                case 14: *event_id = 4;   *nextstate_id = 2;   break;
                case 15: *event_id = 5;   *nextstate_id = 2;   break;
                case 16: *event_id = 6;   *nextstate_id = 2;   break;
                case 17: *event_id = 7;   *nextstate_id = 2;   break;
                case 18: *event_id = 8;   *nextstate_id = 2;   break;
                case 19: *event_id = 1;   break;
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
    icl_console_print ("http_access_dos_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_ACCESS_DOS_AGENT))

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
            "http_access_dos_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "http_access_dos_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "http_access_dos_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_ACCESS_DOS_AGENT)        ||  defined (HTTP_ACCESS_DOS_AGENT_TRACE))
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
                                       "http_access_dos_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "http_access_dos_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "http_access_dos_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_DESTROY))
    icl_stats_inc ("http_access_dos_agent_destroy", &s_http_access_dos_agent_destroy_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->server_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
}
ipr_looseref_list_destroy (&self->http_access_module_list);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_FREE))
    icl_stats_inc ("http_access_dos_agent_free", &s_http_access_dos_agent_free_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_access_dos object");
    http_access_dos_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_access_dos_free (self);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_3))
    icl_stats_inc ("http_access_dos_agent_unnamed_3", &s_http_access_dos_agent_unnamed_3_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_4))
    icl_stats_inc ("http_access_dos_agent_unnamed_4", &s_http_access_dos_agent_unnamed_4_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_announce (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_ANNOUNCE))
    icl_stats_inc ("http_access_dos_agent_announce", &s_http_access_dos_agent_announce_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;
smt_log_t *
    log;

    portal = announce_m->portal;
    log = announce_m->log;
        {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_6))
    icl_stats_inc ("http_access_dos_agent_unnamed_6", &s_http_access_dos_agent_unnamed_6_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_OPEN))
    icl_stats_inc ("http_access_dos_agent_open", &s_http_access_dos_agent_open_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = open_m->portal;
    context = open_m->context;
        {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_8))
    icl_stats_inc ("http_access_dos_agent_unnamed_8", &s_http_access_dos_agent_unnamed_8_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_CLOSE))
    icl_stats_inc ("http_access_dos_agent_close", &s_http_access_dos_agent_close_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = close_m->portal;
    context = close_m->context;
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_10))
    icl_stats_inc ("http_access_dos_agent_unnamed_10", &s_http_access_dos_agent_unnamed_10_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action__http_access_module_unbind (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT__HTTP_ACCESS_MODULE_UNBIND))
    icl_stats_inc ("http_access_dos_agent__http_access_module_unbind", &s_http_access_dos_agent__http_access_module_unbind_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;

    portal = _http_access_module_unbind_m->portal;
        {
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    http_access_module_unlink (&portal);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_12))
    icl_stats_inc ("http_access_dos_agent_unnamed_12", &s_http_access_dos_agent_unnamed_12_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_before (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_BEFORE))
    icl_stats_inc ("http_access_dos_agent_before", &s_http_access_dos_agent_before_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = before_m->portal;
    context = before_m->context;
        {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_14))
    icl_stats_inc ("http_access_dos_agent_unnamed_14", &s_http_access_dos_agent_unnamed_14_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_after (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_AFTER))
    icl_stats_inc ("http_access_dos_agent_after", &s_http_access_dos_agent_after_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{


#define self tcb->http_access_dos
http_access_module_t *
    portal;
http_access_context_t *
    context;

    portal = after_m->portal;
    context = after_m->context;
        {
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_16 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT)  ||  defined (BASE_STATS_HTTP_ACCESS_DOS_AGENT_UNNAMED_16))
    icl_stats_inc ("http_access_dos_agent_unnamed_16", &s_http_access_dos_agent_unnamed_16_stats);
#endif        
#define tcb ((http_access_dos_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

