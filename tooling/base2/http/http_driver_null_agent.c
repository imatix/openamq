/*---------------------------------------------------------------------------
    http_driver_null_agent.c - functions for http_driver_null_agent.

    Generated from http_driver_null_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "http_driver_null_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define announce_m ((http_driver_null_agent_announce_t *) thread->method->data)

#define destroy_m ((http_driver_null_agent_destroy_t *) thread->method->data)

#define _http_driver_module_unbind_m ((http_driver_null_agent__http_driver_module_unbind_t *) thread->method->data)

#define get_m ((http_driver_null_agent_get_t *) thread->method->data)

#define head_m ((http_driver_null_agent_head_t *) thread->method->data)

#define post_m ((http_driver_null_agent_post_t *) thread->method->data)

#define put_m ((http_driver_null_agent_put_t *) thread->method->data)

#define delete_m ((http_driver_null_agent_delete_t *) thread->method->data)

#define move_m ((http_driver_null_agent_move_t *) thread->method->data)

#define copy_m ((http_driver_null_agent_copy_t *) thread->method->data)

enum {
    shutdown_event                                 = 1,
    destroy_m_event                                = 2,
    announce_m_event                               = 3,
    _http_driver_module_unbind_m_event             = 4,
    get_m_event                                    = 5,
    head_m_event                                   = 6,
    post_m_event                                   = 7,
    put_m_event                                    = 8,
    delete_m_event                                 = 9,
    move_m_event                                   = 10,
    copy_m_event                                   = 11,
};

#define laststate                                  2
#define lastevent                                  11
#define lastplace                                  25

#define http_driver_null_agent_agent_name          "Http-Driver-Null-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define shutdown_event_name                        "Shutdown"
#define destroy_m_event_name                       "Destroy-M"
#define announce_m_event_name                      "Announce-M"
#define _http_driver_module_unbind_m_event_name    "-Http-Driver-Module-Unbind-M"
#define get_m_event_name                           "Get-M"
#define head_m_event_name                          "Head-M"
#define post_m_event_name                          "Post-M"
#define put_m_event_name                           "Put-M"
#define delete_m_event_name                        "Delete-M"
#define move_m_event_name                          "Move-M"
#define copy_m_event_name                          "Copy-M"

#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_3_action_name                      "Unnamed-3"
#define announce_action_name                       "Announce"
#define unnamed_5_action_name                      "Unnamed-5"
#define unnamed_6_action_name                      "Unnamed-6"
#define _http_driver_module_unbind_action_name     "-Http-Driver-Module-Unbind"
#define unnamed_8_action_name                      "Unnamed-8"
#define get_action_name                            "Get"
#define unnamed_10_action_name                     "Unnamed-10"
#define head_action_name                           "Head"
#define unnamed_12_action_name                     "Unnamed-12"
#define post_action_name                           "Post"
#define unnamed_14_action_name                     "Unnamed-14"
#define put_action_name                            "Put"
#define unnamed_16_action_name                     "Unnamed-16"
#define delete_action_name                         "Delete"
#define unnamed_18_action_name                     "Unnamed-18"
#define move_action_name                           "Move"
#define unnamed_20_action_name                     "Unnamed-20"
#define copy_action_name                           "Copy"
#define unnamed_22_action_name                     "Unnamed-22"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    http_driver_module_t *  portal;
    smt_log_t *    log;
  }
http_driver_null_agent_announce_t;

typedef struct
  {
    char *         file;
    size_t         line;
  }
http_driver_null_agent_destroy_t;

typedef struct
  {
    http_driver_module_t *  portal;
  }
http_driver_null_agent__http_driver_module_unbind_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_get_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_head_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_post_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_put_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_delete_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_move_t;

typedef struct
  {
    http_driver_module_t *  portal;
    http_driver_context_t *  context;
  }
http_driver_null_agent_copy_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        http_driver_null_t
            *http_driver_null;
    
} http_driver_null_agent_class_context_t;

static icl_cache_t
    *s_announce_cache = NULL;
static icl_cache_t
    *s_destroy_cache = NULL;
static icl_cache_t
    *s__http_driver_module_unbind_cache = NULL;
static icl_cache_t
    *s_get_cache = NULL;
static icl_cache_t
    *s_head_cache = NULL;
static icl_cache_t
    *s_post_cache = NULL;
static icl_cache_t
    *s_put_cache = NULL;
static icl_cache_t
    *s_delete_cache = NULL;
static icl_cache_t
    *s_move_cache = NULL;
static icl_cache_t
    *s_copy_cache = NULL;

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
    _http_driver_module_unbind_m_event_name,
    get_m_event_name,
    head_m_event_name,
    post_m_event_name,
    put_m_event_name,
    delete_m_event_name,
    move_m_event_name,
    copy_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT)   ||  defined (BASE_TRACE)  || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT)   ||  defined (HTTP_DRIVER_NULL_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    destroy_action_name,
    free_action_name,
    unnamed_3_action_name,
    announce_action_name,
    unnamed_5_action_name,
    unnamed_6_action_name,
    _http_driver_module_unbind_action_name,
    unnamed_8_action_name,
    get_action_name,
    unnamed_10_action_name,
    head_action_name,
    unnamed_12_action_name,
    post_action_name,
    unnamed_14_action_name,
    put_action_name,
    unnamed_16_action_name,
    delete_action_name,
    unnamed_18_action_name,
    move_action_name,
    unnamed_20_action_name,
    copy_action_name,
    unnamed_22_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_DESTROY))
static icl_stats_t *s_http_driver_null_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_FREE))
static icl_stats_t *s_http_driver_null_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_3))
static icl_stats_t *s_http_driver_null_agent_unnamed_3_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_ANNOUNCE))
static icl_stats_t *s_http_driver_null_agent_announce_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_5))
static icl_stats_t *s_http_driver_null_agent_unnamed_5_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_6))
static icl_stats_t *s_http_driver_null_agent_unnamed_6_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT__HTTP_DRIVER_MODULE_UNBIND))
static icl_stats_t *s_http_driver_null_agent__http_driver_module_unbind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_8))
static icl_stats_t *s_http_driver_null_agent_unnamed_8_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_GET))
static icl_stats_t *s_http_driver_null_agent_get_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_10))
static icl_stats_t *s_http_driver_null_agent_unnamed_10_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_HEAD))
static icl_stats_t *s_http_driver_null_agent_head_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_12))
static icl_stats_t *s_http_driver_null_agent_unnamed_12_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_POST))
static icl_stats_t *s_http_driver_null_agent_post_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_14))
static icl_stats_t *s_http_driver_null_agent_unnamed_14_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_PUT))
static icl_stats_t *s_http_driver_null_agent_put_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_16))
static icl_stats_t *s_http_driver_null_agent_unnamed_16_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_DELETE))
static icl_stats_t *s_http_driver_null_agent_delete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_18))
static icl_stats_t *s_http_driver_null_agent_unnamed_18_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_MOVE))
static icl_stats_t *s_http_driver_null_agent_move_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_20))
static icl_stats_t *s_http_driver_null_agent_unnamed_20_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_COPY))
static icl_stats_t *s_http_driver_null_agent_copy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_22))
static icl_stats_t *s_http_driver_null_agent_unnamed_22_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    http_driver_null_agent_manager  (smt_thread_t **thread);
static int    http_driver_null_agent_catcher  (smt_thread_t *thread);
static int    http_driver_null_agent_class_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT)     ||  defined (HTTP_DRIVER_NULL_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_3                (smt_thread_t *thread);
static int    action_announce                 (smt_thread_t *thread);
static int    action_unnamed_5                (smt_thread_t *thread);
static int    action_unnamed_6                (smt_thread_t *thread);
static int    action__http_driver_module_unbind  (smt_thread_t *thread);
static int    action_unnamed_8                (smt_thread_t *thread);
static int    action_get                      (smt_thread_t *thread);
static int    action_unnamed_10               (smt_thread_t *thread);
static int    action_head                     (smt_thread_t *thread);
static int    action_unnamed_12               (smt_thread_t *thread);
static int    action_post                     (smt_thread_t *thread);
static int    action_unnamed_14               (smt_thread_t *thread);
static int    action_put                      (smt_thread_t *thread);
static int    action_unnamed_16               (smt_thread_t *thread);
static int    action_delete                   (smt_thread_t *thread);
static int    action_unnamed_18               (smt_thread_t *thread);
static int    action_move                     (smt_thread_t *thread);
static int    action_unnamed_20               (smt_thread_t *thread);
static int    action_copy                     (smt_thread_t *thread);
static int    action_unnamed_22               (smt_thread_t *thread);
static int
http_driver_null_agent_announce_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    smt_log_t *    log);
int
http_driver_null_agent_announce_destructor (void *data);
static int
http_driver_null_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
http_driver_null_agent_destroy_destructor (void *data);
static int
http_driver_null_agent__http_driver_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal);
int
http_driver_null_agent__http_driver_module_unbind_destructor (void *data);
static int
http_driver_null_agent_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_get_destructor (void *data);
static int
http_driver_null_agent_head_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_head_destructor (void *data);
static int
http_driver_null_agent_post_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_post_destructor (void *data);
static int
http_driver_null_agent_put_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_put_destructor (void *data);
static int
http_driver_null_agent_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_delete_destructor (void *data);
static int
http_driver_null_agent_move_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_move_destructor (void *data);
static int
http_driver_null_agent_copy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context);
int
http_driver_null_agent_copy_destructor (void *data);
//  Function definitions                                                       

static int
http_driver_null_agent_announce_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    smt_log_t *    log)
{
    int
        rc = 0;
    http_driver_null_agent_announce_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_announce_cache);
    _message->portal                    = portal;
    _message->log                       = log;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_announce_destructor);
    
    return rc;
}

int
http_driver_null_agent_announce_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
smt_log_t * log;

    portal = ((http_driver_null_agent_announce_t *) data)-> portal;
    log = ((http_driver_null_agent_announce_t *) data)-> log;


    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    http_driver_null_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_destroy_destructor);
    
    return rc;
}

int
http_driver_null_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((http_driver_null_agent_destroy_t *) data)-> file;
    line = ((http_driver_null_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent__http_driver_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal)
{
    int
        rc = 0;
    http_driver_null_agent__http_driver_module_unbind_t
        *_message;
        

    _message = icl_mem_cache_alloc (s__http_driver_module_unbind_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent__http_driver_module_unbind_destructor);
    
    return rc;
}

int
http_driver_null_agent__http_driver_module_unbind_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;

    portal = ((http_driver_null_agent__http_driver_module_unbind_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_get_t
        *_message;
        
    //  Possess code for get
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_get_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_get_destructor);
    
    return rc;
}

int
http_driver_null_agent_get_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_get_t *) data)-> portal;
    context = ((http_driver_null_agent_get_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_head_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_head_t
        *_message;
        
    //  Possess code for head
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_head_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_head_destructor);
    
    return rc;
}

int
http_driver_null_agent_head_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_head_t *) data)-> portal;
    context = ((http_driver_null_agent_head_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_post_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_post_t
        *_message;
        
    //  Possess code for post
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_post_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_post_destructor);
    
    return rc;
}

int
http_driver_null_agent_post_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_post_t *) data)-> portal;
    context = ((http_driver_null_agent_post_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_put_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_put_t
        *_message;
        
    //  Possess code for put
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_put_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_put_destructor);
    
    return rc;
}

int
http_driver_null_agent_put_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_put_t *) data)-> portal;
    context = ((http_driver_null_agent_put_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_delete_t
        *_message;
        
    //  Possess code for delete
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_delete_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_delete_destructor);
    
    return rc;
}

int
http_driver_null_agent_delete_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_delete_t *) data)-> portal;
    context = ((http_driver_null_agent_delete_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_move_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_move_t
        *_message;
        
    //  Possess code for move
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_move_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_move_destructor);
    
    return rc;
}

int
http_driver_null_agent_move_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_move_t *) data)-> portal;
    context = ((http_driver_null_agent_move_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_driver_null_agent_copy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal,
    http_driver_context_t *  context)
{
    int
        rc = 0;
    http_driver_null_agent_copy_t
        *_message;
        
    //  Possess code for copy
    

    
        assert (context);
        context = http_driver_context_link (context);
        
    
    

    _message = icl_mem_cache_alloc (s_copy_cache);
    _message->portal                    = portal;
    _message->context                   = context;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_driver_null_agent_copy_destructor);
    
    return rc;
}

int
http_driver_null_agent_copy_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;
http_driver_context_t * context;

    portal = ((http_driver_null_agent_copy_t *) data)-> portal;
    context = ((http_driver_null_agent_copy_t *) data)-> context;



    
        http_driver_context_unlink (&context);
        
    
    

    icl_mem_free (data);
    return rc;
}


int
http_driver_null_agent_announce (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    smt_log_t *           log)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_announce_send (
                thread->reply_queue, announce_m_event,
                portal,
                log);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent__http_driver_module_unbind (
    smt_thread_t * thread,
    http_driver_module_t * portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent__http_driver_module_unbind_send (
                thread->reply_queue, _http_driver_module_unbind_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_get (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_get_send (
                thread->reply_queue, get_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_head (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_head_send (
                thread->reply_queue, head_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_post (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_post_send (
                thread->reply_queue, post_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_put (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_put_send (
                thread->reply_queue, put_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_delete (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_delete_send (
                thread->reply_queue, delete_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_move (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_move_send (
                thread->reply_queue, move_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_driver_null_agent_copy (
    smt_thread_t * thread,
    http_driver_module_t * portal,
    http_driver_context_t * context)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_driver_null_agent_manager);
        rc = http_driver_null_agent_copy_send (
                thread->reply_queue, copy_m_event,
                portal,
                context);
    }
    else
        rc = -1;
        
    return rc;
}

void
http_driver_null_agent_term (void)
{

}

int 
http_driver_null_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (http_driver_null_agent_class_context_t));
    s_announce_cache = icl_cache_get (sizeof (http_driver_null_agent_announce_t));
    s_destroy_cache = icl_cache_get (sizeof (http_driver_null_agent_destroy_t));
    s__http_driver_module_unbind_cache = icl_cache_get (sizeof (http_driver_null_agent__http_driver_module_unbind_t));
    s_get_cache = icl_cache_get (sizeof (http_driver_null_agent_get_t));
    s_head_cache = icl_cache_get (sizeof (http_driver_null_agent_head_t));
    s_post_cache = icl_cache_get (sizeof (http_driver_null_agent_post_t));
    s_put_cache = icl_cache_get (sizeof (http_driver_null_agent_put_t));
    s_delete_cache = icl_cache_get (sizeof (http_driver_null_agent_delete_t));
    s_move_cache = icl_cache_get (sizeof (http_driver_null_agent_move_t));
    s_copy_cache = icl_cache_get (sizeof (http_driver_null_agent_copy_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, http_driver_null_agent_term);
    
    return rc;
}

void
http_driver_null_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
http_driver_null_agent_class_thread_new (
    http_driver_null_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (http_driver_null_agent_manager,
                             http_driver_null_agent_catcher,
                             http_driver_null_agent_class_destroy);
                             

#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "http_driver_null_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (http_driver_null_agent_class_context_t));
        ((http_driver_null_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)

        tcb->http_driver_null = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
http_driver_null_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == http_driver_null_agent_manager);
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
            case 15:
            case 25:
                thread->place = SMT_TERM_PLACE;
                break;
            case 12:
            case 11:
            case 10:
            case 8:
            case 6:
            case 7:
            case 9:
            case 4:
            case 13:
                thread->place = 2;
                break;
            case 24:
            case 23:
            case 21:
            case 22:
            case 20:
            case 3:
            case 19:
            case 16:
            case 17:
            case 18:
                thread->place = 14;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)

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
        if (thread->place < 14)                 //  Main state
            thread->place = 2;
        else
            thread->place = 14;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case announce_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 4; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 5; break;
                case _http_driver_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case get_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                case head_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 8; break;
                case post_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 9; break;
                case put_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 10; break;
                case delete_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 11; break;
                case move_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 12; break;
                case copy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 14:                        //  shutting down state                 
{
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 15; break;
                case announce_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 16; break;
                case _http_driver_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 17; break;
                case get_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 18; break;
                case head_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 19; break;
                case post_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 20; break;
                case put_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 21; break;
                case delete_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 22; break;
                case move_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 23; break;
                case copy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 24; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 25; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 25)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 15:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action__http_driver_module_unbind (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_get (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_head (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_post (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_put (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
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
        case 11:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 17);
#endif
                    rc = action_delete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 18);
#endif
                    rc = action_unnamed_18 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 19);
#endif
                    rc = action_move (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 20);
#endif
                    rc = action_unnamed_20 (thread);

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
        case 13:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 21);
#endif
                    rc = action_copy (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
                    if (thread->animate)
                        animate_action (thread, 22);
#endif
                    rc = action_unnamed_22 (thread);

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
http_driver_null_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
http_driver_null_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (HTTP_DRIVER_NULL_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "http_driver_null_agent",
                 class_thread_name);

#endif
    if (--((http_driver_null_agent_class_context_t *) thread->context)->links == 0)
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
        if (thread->place < 14)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 3;   *nextstate_id = 1;   break;
                case 5: *event_id = 2;   break;
                case 6: *event_id = 4;   *nextstate_id = 1;   break;
                case 7: *event_id = 5;   *nextstate_id = 1;   break;
                case 8: *event_id = 6;   *nextstate_id = 1;   break;
                case 9: *event_id = 7;   *nextstate_id = 1;   break;
                case 10: *event_id = 8;   *nextstate_id = 1;   break;
                case 11: *event_id = 9;   *nextstate_id = 1;   break;
                case 12: *event_id = 10;   *nextstate_id = 1;   break;
                case 13: *event_id = 11;   *nextstate_id = 1;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 15: *event_id = 2;   break;
                case 16: *event_id = 3;   *nextstate_id = 2;   break;
                case 17: *event_id = 4;   *nextstate_id = 2;   break;
                case 18: *event_id = 5;   *nextstate_id = 2;   break;
                case 19: *event_id = 6;   *nextstate_id = 2;   break;
                case 20: *event_id = 7;   *nextstate_id = 2;   break;
                case 21: *event_id = 8;   *nextstate_id = 2;   break;
                case 22: *event_id = 9;   *nextstate_id = 2;   break;
                case 23: *event_id = 10;   *nextstate_id = 2;   break;
                case 24: *event_id = 11;   *nextstate_id = 2;   break;
                case 25: *event_id = 1;   break;
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
    icl_console_print ("http_driver_null_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_DRIVER_NULL_AGENT))

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
            "http_driver_null_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "http_driver_null_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "http_driver_null_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_DRIVER_NULL_AGENT)       ||  defined (HTTP_DRIVER_NULL_AGENT_TRACE))
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
                                       "http_driver_null_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "http_driver_null_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "http_driver_null_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_DESTROY))
    icl_stats_inc ("http_driver_null_agent_destroy", &s_http_driver_null_agent_destroy_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_driver_module_t
    *http_driver_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->server_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
}
ipr_looseref_list_destroy (&self->http_driver_module_list);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_FREE))
    icl_stats_inc ("http_driver_null_agent_free", &s_http_driver_null_agent_free_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_driver_null object");
    http_driver_null_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_driver_null_free (self);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_3))
    icl_stats_inc ("http_driver_null_agent_unnamed_3", &s_http_driver_null_agent_unnamed_3_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_ANNOUNCE))
    icl_stats_inc ("http_driver_null_agent_announce", &s_http_driver_null_agent_announce_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
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
action_unnamed_5 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_5))
    icl_stats_inc ("http_driver_null_agent_unnamed_5", &s_http_driver_null_agent_unnamed_5_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_6))
    icl_stats_inc ("http_driver_null_agent_unnamed_6", &s_http_driver_null_agent_unnamed_6_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action__http_driver_module_unbind (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT__HTTP_DRIVER_MODULE_UNBIND))
    icl_stats_inc ("http_driver_null_agent__http_driver_module_unbind", &s_http_driver_null_agent__http_driver_module_unbind_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;

    portal = _http_driver_module_unbind_m->portal;
        {
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    http_driver_module_unlink (&portal);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_8))
    icl_stats_inc ("http_driver_null_agent_unnamed_8", &s_http_driver_null_agent_unnamed_8_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_get (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_GET))
    icl_stats_inc ("http_driver_null_agent_get", &s_http_driver_null_agent_get_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = get_m->portal;
    context = get_m->context;
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_10))
    icl_stats_inc ("http_driver_null_agent_unnamed_10", &s_http_driver_null_agent_unnamed_10_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_head (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_HEAD))
    icl_stats_inc ("http_driver_null_agent_head", &s_http_driver_null_agent_head_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = head_m->portal;
    context = head_m->context;
        {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_12))
    icl_stats_inc ("http_driver_null_agent_unnamed_12", &s_http_driver_null_agent_unnamed_12_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_post (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_POST))
    icl_stats_inc ("http_driver_null_agent_post", &s_http_driver_null_agent_post_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = post_m->portal;
    context = post_m->context;
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_14))
    icl_stats_inc ("http_driver_null_agent_unnamed_14", &s_http_driver_null_agent_unnamed_14_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_put (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_PUT))
    icl_stats_inc ("http_driver_null_agent_put", &s_http_driver_null_agent_put_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = put_m->portal;
    context = put_m->context;
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_16))
    icl_stats_inc ("http_driver_null_agent_unnamed_16", &s_http_driver_null_agent_unnamed_16_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_delete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_DELETE))
    icl_stats_inc ("http_driver_null_agent_delete", &s_http_driver_null_agent_delete_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = delete_m->portal;
    context = delete_m->context;
        {
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_18 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_18))
    icl_stats_inc ("http_driver_null_agent_unnamed_18", &s_http_driver_null_agent_unnamed_18_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_move (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_MOVE))
    icl_stats_inc ("http_driver_null_agent_move", &s_http_driver_null_agent_move_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = move_m->portal;
    context = move_m->context;
        {
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_20 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_20))
    icl_stats_inc ("http_driver_null_agent_unnamed_20", &s_http_driver_null_agent_unnamed_20_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_copy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_COPY))
    icl_stats_inc ("http_driver_null_agent_copy", &s_http_driver_null_agent_copy_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{


#define self tcb->http_driver_null
http_driver_module_t *
    portal;
http_driver_context_t *
    context;

    portal = copy_m->portal;
    context = copy_m->context;
        {
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_22 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT)  ||  defined (BASE_STATS_HTTP_DRIVER_NULL_AGENT_UNNAMED_22))
    icl_stats_inc ("http_driver_null_agent_unnamed_22", &s_http_driver_null_agent_unnamed_22_stats);
#endif        
#define tcb ((http_driver_null_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

