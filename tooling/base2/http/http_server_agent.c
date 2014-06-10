/*---------------------------------------------------------------------------
    http_server_agent.c - functions for http_server_agent.

    Generated from http_server_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "http_server_agent.h"

#define class_main_state                           1
#define class_shutting_down_state                  2

#define destroy_m ((http_server_agent_destroy_t *) thread->method->data)

#define ban_address_m ((http_server_agent_ban_address_t *) thread->method->data)

#define new_password_m ((http_server_agent_new_password_t *) thread->method->data)

#define _http_access_module_unbind_m ((http_server_agent__http_access_module_unbind_t *) thread->method->data)

#define _http_driver_module_unbind_m ((http_server_agent__http_driver_module_unbind_t *) thread->method->data)

enum {
    monitor_event                                  = 1,
    shutdown_event                                 = 2,
    destroy_m_event                                = 3,
    start_m_event                                  = 4,
    ban_address_m_event                            = 5,
    sync_hashes_m_event                            = 6,
    new_password_m_event                           = 7,
    _http_access_module_unbind_m_event             = 8,
    _http_driver_module_unbind_m_event             = 9,
};

#define laststate                                  2
#define lastevent                                  9
#define lastplace                                  21

#define http_server_agent_agent_name               "Http-Server-Agent"

#define class_thread_name                          "Class"

#define main_state_name                            "Main"
#define shutting_down_state_name                   "Shutting-Down"

#define monitor_event_name                         "Monitor"
#define shutdown_event_name                        "Shutdown"
#define destroy_m_event_name                       "Destroy-M"
#define start_m_event_name                         "Start-M"
#define ban_address_m_event_name                   "Ban-Address-M"
#define sync_hashes_m_event_name                   "Sync-Hashes-M"
#define new_password_m_event_name                  "New-Password-M"
#define _http_access_module_unbind_m_event_name    "-Http-Access-Module-Unbind-M"
#define _http_driver_module_unbind_m_event_name    "-Http-Driver-Module-Unbind-M"

#define monitor_action_name                        "Monitor"
#define destroy_action_name                        "Destroy"
#define free_action_name                           "Free"
#define unnamed_4_action_name                      "Unnamed-4"
#define unnamed_5_action_name                      "Unnamed-5"
#define start_action_name                          "Start"
#define unnamed_7_action_name                      "Unnamed-7"
#define ban_address_action_name                    "Ban-Address"
#define unnamed_9_action_name                      "Unnamed-9"
#define sync_hashes_action_name                    "Sync-Hashes"
#define unnamed_11_action_name                     "Unnamed-11"
#define new_password_action_name                   "New-Password"
#define unnamed_13_action_name                     "Unnamed-13"
#define _http_access_module_unbind_action_name     "-Http-Access-Module-Unbind"
#define unnamed_15_action_name                     "Unnamed-15"
#define _http_driver_module_unbind_action_name     "-Http-Driver-Module-Unbind"
#define unnamed_17_action_name                     "Unnamed-17"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    char *         file;
    size_t         line;
  }
http_server_agent_destroy_t;

typedef struct
  {
    char *         address;
  }
http_server_agent_ban_address_t;

typedef struct
  {
    http_access_module_t *  portal;
    char *         key;
    char *         hash;
    Bool           digest;
  }
http_server_agent_new_password_t;

typedef struct
  {
    http_access_module_t *  portal;
  }
http_server_agent__http_access_module_unbind_t;

typedef struct
  {
    http_driver_module_t *  portal;
  }
http_server_agent__http_driver_module_unbind_t;

static icl_cache_t
    *s_class_context_cache = NULL;

typedef struct {
    int
        links;

        http_server_t
            *http_server;
    
} http_server_agent_class_context_t;

static icl_cache_t
    *s_destroy_cache = NULL;
static icl_cache_t
    *s_ban_address_cache = NULL;
static icl_cache_t
    *s_new_password_cache = NULL;
static icl_cache_t
    *s__http_access_module_unbind_cache = NULL;
static icl_cache_t
    *s__http_driver_module_unbind_cache = NULL;

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
    monitor_event_name,
    shutdown_event_name,
    destroy_m_event_name,
    start_m_event_name,
    ban_address_m_event_name,
    sync_hashes_m_event_name,
    new_password_m_event_name,
    _http_access_module_unbind_m_event_name,
    _http_driver_module_unbind_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT)        ||  defined (BASE_TRACE)  || defined (BASE_TRACE_HTTP_SERVER_AGENT)   ||  defined (HTTP_SERVER_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    monitor_action_name,
    destroy_action_name,
    free_action_name,
    unnamed_4_action_name,
    unnamed_5_action_name,
    start_action_name,
    unnamed_7_action_name,
    ban_address_action_name,
    unnamed_9_action_name,
    sync_hashes_action_name,
    unnamed_11_action_name,
    new_password_action_name,
    unnamed_13_action_name,
    _http_access_module_unbind_action_name,
    unnamed_15_action_name,
    _http_driver_module_unbind_action_name,
    unnamed_17_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_MONITOR))
static icl_stats_t *s_http_server_agent_monitor_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_DESTROY))
static icl_stats_t *s_http_server_agent_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_FREE))
static icl_stats_t *s_http_server_agent_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_4))
static icl_stats_t *s_http_server_agent_unnamed_4_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_5))
static icl_stats_t *s_http_server_agent_unnamed_5_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_START))
static icl_stats_t *s_http_server_agent_start_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_7))
static icl_stats_t *s_http_server_agent_unnamed_7_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_BAN_ADDRESS))
static icl_stats_t *s_http_server_agent_ban_address_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_9))
static icl_stats_t *s_http_server_agent_unnamed_9_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_SYNC_HASHES))
static icl_stats_t *s_http_server_agent_sync_hashes_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_11))
static icl_stats_t *s_http_server_agent_unnamed_11_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_NEW_PASSWORD))
static icl_stats_t *s_http_server_agent_new_password_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_13))
static icl_stats_t *s_http_server_agent_unnamed_13_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT__HTTP_ACCESS_MODULE_UNBIND))
static icl_stats_t *s_http_server_agent__http_access_module_unbind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_15))
static icl_stats_t *s_http_server_agent_unnamed_15_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT__HTTP_DRIVER_MODULE_UNBIND))
static icl_stats_t *s_http_server_agent__http_driver_module_unbind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_17))
static icl_stats_t *s_http_server_agent_unnamed_17_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    http_server_agent_manager       (smt_thread_t **thread);
static int    http_server_agent_catcher       (smt_thread_t *thread);
static int    http_server_agent_class_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_HTTP_SERVER_AGENT)          ||  defined (HTTP_SERVER_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_monitor                  (smt_thread_t *thread);
static int    action_destroy                  (smt_thread_t *thread);
static int    action_free                     (smt_thread_t *thread);
static int    action_unnamed_4                (smt_thread_t *thread);
static int    action_unnamed_5                (smt_thread_t *thread);
static int    action_start                    (smt_thread_t *thread);
static int    action_unnamed_7                (smt_thread_t *thread);
static int    action_ban_address              (smt_thread_t *thread);
static int    action_unnamed_9                (smt_thread_t *thread);
static int    action_sync_hashes              (smt_thread_t *thread);
static int    action_unnamed_11               (smt_thread_t *thread);
static int    action_new_password             (smt_thread_t *thread);
static int    action_unnamed_13               (smt_thread_t *thread);
static int    action__http_access_module_unbind  (smt_thread_t *thread);
static int    action_unnamed_15               (smt_thread_t *thread);
static int    action__http_driver_module_unbind  (smt_thread_t *thread);
static int    action_unnamed_17               (smt_thread_t *thread);
static int
http_server_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line);
int
http_server_agent_destroy_destructor (void *data);
static int
http_server_agent_ban_address_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         address);
int
http_server_agent_ban_address_destructor (void *data);
static int
http_server_agent_new_password_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    char *         key,
    char *         hash,
    Bool           digest);
int
http_server_agent_new_password_destructor (void *data);
static int
http_server_agent__http_access_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal);
int
http_server_agent__http_access_module_unbind_destructor (void *data);
static int
http_server_agent__http_driver_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal);
int
http_server_agent__http_driver_module_unbind_destructor (void *data);
//  Function definitions                                                       

static int
http_server_agent_destroy_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         file,
    size_t         line)
{
    int
        rc = 0;
    http_server_agent_destroy_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_destroy_cache);
    _message->file                      = file;
    _message->line                      = line;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_server_agent_destroy_destructor);
    
    return rc;
}

int
http_server_agent_destroy_destructor (void *data)
{
    int
        rc = 0;
char * file;
size_t line;

    file = ((http_server_agent_destroy_t *) data)-> file;
    line = ((http_server_agent_destroy_t *) data)-> line;


    icl_mem_free (data);
    return rc;
}


static int
http_server_agent_ban_address_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    char *         address)
{
    int
        rc = 0;
    http_server_agent_ban_address_t
        *_message;
        
    //  Possess code for ban address
    

    address = icl_mem_strdup (address);
    
    

    _message = icl_mem_cache_alloc (s_ban_address_cache);
    _message->address                   = address;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_server_agent_ban_address_destructor);
    
    return rc;
}

int
http_server_agent_ban_address_destructor (void *data)
{
    int
        rc = 0;
char * address;

    address = ((http_server_agent_ban_address_t *) data)-> address;



    icl_mem_free (address);
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_server_agent_new_password_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal,
    char *         key,
    char *         hash,
    Bool           digest)
{
    int
        rc = 0;
    http_server_agent_new_password_t
        *_message;
        
    //  Possess code for new password
    

    
        key = icl_mem_strdup (key);
        hash = icl_mem_strdup (hash);
        
    
    

    _message = icl_mem_cache_alloc (s_new_password_cache);
    _message->portal                    = portal;
    _message->key                       = key;
    _message->hash                      = hash;
    _message->digest                    = digest;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_server_agent_new_password_destructor);
    
    return rc;
}

int
http_server_agent_new_password_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;
char * key;
char * hash;
Bool digest;

    portal = ((http_server_agent_new_password_t *) data)-> portal;
    key = ((http_server_agent_new_password_t *) data)-> key;
    hash = ((http_server_agent_new_password_t *) data)-> hash;
    digest = ((http_server_agent_new_password_t *) data)-> digest;



    
        icl_mem_strfree (&key);
        icl_mem_strfree (&hash);
        
    
    

    icl_mem_free (data);
    return rc;
}


static int
http_server_agent__http_access_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_access_module_t *  portal)
{
    int
        rc = 0;
    http_server_agent__http_access_module_unbind_t
        *_message;
        

    _message = icl_mem_cache_alloc (s__http_access_module_unbind_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_server_agent__http_access_module_unbind_destructor);
    
    return rc;
}

int
http_server_agent__http_access_module_unbind_destructor (void *data)
{
    int
        rc = 0;
http_access_module_t * portal;

    portal = ((http_server_agent__http_access_module_unbind_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


static int
http_server_agent__http_driver_module_unbind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    http_driver_module_t *  portal)
{
    int
        rc = 0;
    http_server_agent__http_driver_module_unbind_t
        *_message;
        

    _message = icl_mem_cache_alloc (s__http_driver_module_unbind_cache);
    _message->portal                    = portal;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          http_server_agent__http_driver_module_unbind_destructor);
    
    return rc;
}

int
http_server_agent__http_driver_module_unbind_destructor (void *data)
{
    int
        rc = 0;
http_driver_module_t * portal;

    portal = ((http_server_agent__http_driver_module_unbind_t *) data)-> portal;


    icl_mem_free (data);
    return rc;
}


int
http_server_agent_start (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                start_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent_sync_hashes (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                sync_hashes_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = http_server_agent_destroy_send (
                thread->reply_queue, destroy_m_event,
                file,
                line);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent_ban_address (
    smt_thread_t * thread,
    char *                address)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = http_server_agent_ban_address_send (
                thread->reply_queue, ban_address_m_event,
                address);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent_new_password (
    smt_thread_t * thread,
    http_access_module_t * portal,
    char *                key,
    char *                hash,
    Bool                  digest)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = http_server_agent_new_password_send (
                thread->reply_queue, new_password_m_event,
                portal,
                key,
                hash,
                digest);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent__http_access_module_unbind (
    smt_thread_t * thread,
    http_access_module_t * portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = http_server_agent__http_access_module_unbind_send (
                thread->reply_queue, _http_access_module_unbind_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_server_agent__http_driver_module_unbind (
    smt_thread_t * thread,
    http_driver_module_t * portal)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_server_agent_manager);
        rc = http_server_agent__http_driver_module_unbind_send (
                thread->reply_queue, _http_driver_module_unbind_m_event,
                portal);
    }
    else
        rc = -1;
        
    return rc;
}

void
http_server_agent_term (void)
{

}

int 
http_server_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_class_context_cache = icl_cache_get (sizeof (http_server_agent_class_context_t));
    s_destroy_cache = icl_cache_get (sizeof (http_server_agent_destroy_t));
    s_ban_address_cache = icl_cache_get (sizeof (http_server_agent_ban_address_t));
    s_new_password_cache = icl_cache_get (sizeof (http_server_agent_new_password_t));
    s__http_access_module_unbind_cache = icl_cache_get (sizeof (http_server_agent__http_access_module_unbind_t));
    s__http_driver_module_unbind_cache = icl_cache_get (sizeof (http_server_agent__http_driver_module_unbind_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    }
    icl_system_register (NULL, http_server_agent_term);
    
    return rc;
}

void
http_server_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
http_server_agent_class_thread_new (
    http_server_t * self)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (http_server_agent_manager,
                             http_server_agent_catcher,
                             http_server_agent_class_destroy);
                             

#if (defined (HTTP_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "http_server_agent",
                class_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_class_context_cache);
        memset (thread->context, 0, sizeof (http_server_agent_class_context_t));
        ((http_server_agent_class_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((http_server_agent_class_context_t *) thread->context)

        tcb->http_server = self;
    
#undef  tcb
}
    }
    return thread;
}


static int
http_server_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == http_server_agent_manager);
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
            case 14:
            case 21:
                thread->place = SMT_TERM_PLACE;
                break;
            case 9:
            case 6:
            case 7:
            case 11:
            case 3:
            case 8:
            case 10:
                thread->place = 2;
                break;
            case 20:
            case 18:
            case 15:
            case 16:
            case 19:
            case 4:
            case 13:
            case 17:
                thread->place = 12;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Class thread
            if (!thread->shutting_down) {
{
#define tcb ((http_server_agent_class_context_t *) thread->context)

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
        if (thread->place < 12)                 //  Main state
            thread->place = 2;
        else
            thread->place = 12;
    }

    switch (thread->place) {
        case 2:                         //  main state                          
{
#define tcb ((http_server_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case monitor_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 4; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 5; break;
                case start_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 6; break;
                case ban_address_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 7; break;
                case sync_hashes_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 8; break;
                case new_password_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 9; break;
                case _http_access_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 10; break;
                case _http_driver_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 11; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 12:                        //  shutting down state                 
{
#define tcb ((http_server_agent_class_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case monitor_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 13; break;
                case destroy_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 14; break;
                case start_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 15; break;
                case ban_address_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 16; break;
                case sync_hashes_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 17; break;
                case new_password_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 18; break;
                case _http_access_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 19; break;
                case _http_driver_module_unbind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 20; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 21; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_monitor (thread);

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
        case 4:
        case 13:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 21)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 14:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
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
        case 6:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_start (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
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
        case 7:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 8);
#endif
                    rc = action_ban_address (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_sync_hashes (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_unnamed_11 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_new_password (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_unnamed_13 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_unnamed_15 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 17);
#endif
                    rc = action_unnamed_17 (thread);

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
http_server_agent_catcher (smt_thread_t *thread)
{
    return SMT_OK;
}


static int    
http_server_agent_class_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (HTTP_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "http_server_agent",
                 class_thread_name);

#endif
    if (--((http_server_agent_class_context_t *) thread->context)->links == 0)
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
        if (thread->place < 12)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 1;   break;
                case 4: *event_id = 2;   *nextstate_id = 2;   break;
                case 5: *event_id = 3;   break;
                case 6: *event_id = 4;   *nextstate_id = 1;   break;
                case 7: *event_id = 5;   *nextstate_id = 1;   break;
                case 8: *event_id = 6;   *nextstate_id = 1;   break;
                case 9: *event_id = 7;   *nextstate_id = 1;   break;
                case 10: *event_id = 8;   *nextstate_id = 1;   break;
                case 11: *event_id = 9;   *nextstate_id = 1;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 13: *event_id = 1;   *nextstate_id = 2;   break;
                case 14: *event_id = 3;   break;
                case 15: *event_id = 4;   *nextstate_id = 2;   break;
                case 16: *event_id = 5;   *nextstate_id = 2;   break;
                case 17: *event_id = 6;   *nextstate_id = 2;   break;
                case 18: *event_id = 7;   *nextstate_id = 2;   break;
                case 19: *event_id = 8;   *nextstate_id = 2;   break;
                case 20: *event_id = 9;   *nextstate_id = 2;   break;
                case 21: *event_id = 2;   break;
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
    icl_console_print ("http_server_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_SERVER_AGENT))

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
            "http_server_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "http_server_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "http_server_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_SERVER_AGENT)            ||  defined (HTTP_SERVER_AGENT_TRACE))
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
                                       "http_server_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "http_server_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "http_server_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_monitor (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_MONITOR))
    icl_stats_inc ("http_server_agent_monitor", &s_http_server_agent_monitor_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            
#define self tcb->http_server

        {
//  Security timer minimum is 1 so we're always GT 0 here
self->security_timer--;
if (self->security_timer == 0) {
    self->security_timer = http_config_nervosity (http_config);
    http_server_sync_hashes (self);
    if (self->password_guard) {
        self->password_guard -= http_config_nervosity (http_config);
        if (self->password_guard <= 0) {
            self->password_guard = http_config_password_guard (http_config);
            ipr_password_table_guard_sync (self->basic);
            ipr_password_table_guard_sync (self->digest);
        }
    }
}
//  Cycle log files at midnight
if (ipr_time_now () > 235900) {
    if (!self->cycled) {
        self->cycled = TRUE;        //  Don't cycle again today
        smt_log_cycle (self->access_log);
        smt_log_cycle (self->debug_log);
        smt_log_cycle (self->alert_log);
    }
}
else
    self->cycled = FALSE;

if (self->monitor_timer) {
    self->monitor_timer--;
    if (self->monitor_timer == 0) {
        Bool
            new_stats = FALSE;      //  Do we have new stats to print?

        self->monitor_timer = http_config_monitor (http_config);
        if (ipr_meter_mark (self->imeter, http_config_monitor (http_config))) {
            smt_log_print (self->debug_log,
                "I: request  rate=%d mean=%d peak=%d",
                self->imeter->current,
                self->imeter->average,
                self->imeter->maximum);
            new_stats = TRUE;
        }
        if (ipr_meter_mark (self->ometer, http_config_monitor (http_config))) {
            smt_log_print (self->debug_log,
                "I: response rate=%d mean=%d peak=%d iomean=%d",
                self->ometer->current,
                self->ometer->average,
                self->ometer->maximum,
                self->ometer->average + self->imeter->average);
            new_stats = TRUE;
        }
        smt_log_print (self->debug_log, "I: clients=%d requests=%d responses=%d",
                self->clients,
                self->requests,
                self->responses);

        //  Record statistics if necessary
        if (self->stats && new_stats) {
            fprintf (self->stats, "%zu\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                ipr_bucket_used () / 1024,
                self->imeter->current, self->ometer->current,
                self->imeter->average, self->ometer->average,
                self->clients, self->requests, self->responses);
        }
    }
}
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);

if (self->auto_crash_timer) {
    if (--self->auto_crash_timer == 0) {
        smt_log_print (self->alert_log,
            "W: ************************  AUTO-CRASH  ************************");
        smt_log_print (self->alert_log,
            "W: server is now emulating a system crash, and will exit brutally.");
        exit (0);
    }
}
if (self->auto_block_timer) {
    if (--self->auto_block_timer == 0) {
        smt_log_print (self->alert_log,
            "W: ************************  AUTO-BLOCK  ************************");
        smt_log_print (self->alert_log,
            "W: server is now emulating a blockage, and will freeze for 5 minutes.");
        sleep (300);
    }
}
        }
        
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_destroy (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_DESTROY))
    icl_stats_inc ("http_server_agent_destroy", &s_http_server_agent_destroy_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
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
    //  Destroy own portals
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_list);
    //  Destroy own portal factories
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_factory_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_factory_list);
}
}

{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_list);
    //  Destroy own portal factories
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_factory_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_factory_list);
}
}

//  Stop child objects
ipr_meter_destroy (&self->imeter);
ipr_meter_destroy (&self->ometer);
smt_log_destroy (&self->access_log);
smt_log_destroy (&self->alert_log);
smt_log_destroy (&self->debug_log);
ipr_dict_table_destroy (&self->content_types);
ipr_dict_table_destroy (&self->blacklist);
ipr_password_table_destroy (&self->basic);
ipr_password_table_destroy (&self->digest);
if (self->stats)
    fclose (self->stats);
http_agent_term ();
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_FREE))
    icl_stats_inc ("http_server_agent_free", &s_http_server_agent_free_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_server object");
    http_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_server_free (self);
    self = NULL;
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_4 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_4))
    icl_stats_inc ("http_server_agent_unnamed_4", &s_http_server_agent_unnamed_4_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_5 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_5))
    icl_stats_inc ("http_server_agent_unnamed_5", &s_http_server_agent_unnamed_5_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_start (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_START))
    icl_stats_inc ("http_server_agent_start", &s_http_server_agent_start_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server

        {
//  We will run the broker monitor once per second, unconditionally
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_7))
    icl_stats_inc ("http_server_agent_unnamed_7", &s_http_server_agent_unnamed_7_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_ban_address (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_BAN_ADDRESS))
    icl_stats_inc ("http_server_agent_ban_address", &s_http_server_agent_ban_address_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
char *
    address;

    address = ban_address_m->address;
        {
//  Insert into loaded blacklist table, append to file if new address
if (ipr_dict_assume (self->blacklist, address, NULL)) {
    //  Append to blacklist file
    FILE
        *file_stream = fopen (http_config_blacklist (http_config), "a");
    if (file_stream) {
        fprintf (file_stream, "%s\n", address);
        fclose (file_stream);
    }
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_9 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_9))
    icl_stats_inc ("http_server_agent_unnamed_9", &s_http_server_agent_unnamed_9_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_sync_hashes (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_SYNC_HASHES))
    icl_stats_inc ("http_server_agent_sync_hashes", &s_http_server_agent_sync_hashes_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server

        {
icl_shortstr_t
    time_string;                    //  Modification time of file, for display
char
    *filename;

//  Load/reload blacklist data
filename = http_config_blacklist (http_config);
if (ipr_dict_table_file_sync (&self->blacklist, filename, "(.+)(.*)") > 0)
    smt_log_print (self->alert_log,
        "I: blacklist file '%s' loaded (%s, %ld entries)", filename,
        ipr_time_mime (self->blacklist->file_time, time_string), self->blacklist->nbr_items);

//  Load/reload Basic authentication data and set policies if necessary
if (self->basic == NULL) {
    self->basic = ipr_password_table_new (http_config_basic_auth (http_config));
    ipr_password_table_guard_set (self->basic,
        http_config_hit_quota    (http_config),
        http_config_miss_quota   (http_config),
        http_config_password_ttl (http_config));
    if (self->basic->file_time)
        smt_log_print (self->alert_log,
            "I: basic auth file '%s' loaded (%s, %ld entries)", self->basic->file_name,
            ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);
    else
        smt_log_print (self->alert_log,
            "I: basic auth file '%s' not found", self->basic->file_name);
}
else
if (ipr_password_table_sync (self->basic))
    smt_log_print (self->alert_log,
        "I: basic auth file '%s' reloaded (%s, %ld entries)", self->basic->file_name,
        ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);

if (self->digest == NULL) {
    self->digest = ipr_password_table_new (http_config_digest_auth (http_config));
    ipr_password_table_guard_set (self->digest,
        http_config_hit_quota    (http_config),
        http_config_miss_quota   (http_config),
        http_config_password_ttl (http_config));
    if (self->digest->file_time)
        smt_log_print (self->alert_log,
            "I: digest auth file '%s' loaded (%s, %ld entries)", self->digest->file_name,
            ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
    else
        smt_log_print (self->alert_log,
            "I: digest auth file '%s' not found", self->digest->file_name);

    ipr_password_table_set_nonce_ttl   (self->digest, http_config_nonce_ttl (http_config));
    ipr_password_table_set_nonce_quota (self->digest, http_config_nonce_quota (http_config));
}
else
if (ipr_password_table_sync (self->digest))
    smt_log_print (self->alert_log,
        "I: digest auth file '%s' reloaded (%s, %ld entries)", self->digest->file_name,
        ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_11 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_11))
    icl_stats_inc ("http_server_agent_unnamed_11", &s_http_server_agent_unnamed_11_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

static int
action_new_password (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_NEW_PASSWORD))
    icl_stats_inc ("http_server_agent_new_password", &s_http_server_agent_new_password_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
http_access_module_t *
    portal;
char *
    key;
char *
    hash;
Bool
    digest;

    portal = new_password_m->portal;
    key = new_password_m->key;
    hash = new_password_m->hash;
    digest = new_password_m->digest;
        {
ipr_password_table_t
    *table;

table = digest? self->digest: self->basic;
ipr_password_assume (table, key, hash);
ipr_password_table_save (table);
smt_log_print (self->alert_log,
    "I: %s auth file '%s' saved (%ld entries)",
    digest? "digest": "basic", table->file_name, table->nbr_items);
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_13 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_13))
    icl_stats_inc ("http_server_agent_unnamed_13", &s_http_server_agent_unnamed_13_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT__HTTP_ACCESS_MODULE_UNBIND))
    icl_stats_inc ("http_server_agent__http_access_module_unbind", &s_http_server_agent__http_access_module_unbind_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
http_access_module_t *
    portal;

    portal = _http_access_module_unbind_m->portal;
        {
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    http_access_module_unlink (&portal);
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_15 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_15))
    icl_stats_inc ("http_server_agent_unnamed_15", &s_http_server_agent_unnamed_15_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT__HTTP_DRIVER_MODULE_UNBIND))
    icl_stats_inc ("http_server_agent__http_driver_module_unbind", &s_http_server_agent__http_driver_module_unbind_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{


#define self tcb->http_server
http_driver_module_t *
    portal;

    portal = _http_driver_module_unbind_m->portal;
        {
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    http_driver_module_unlink (&portal);
}
        }
#undef self
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_17 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT)  ||  defined (BASE_STATS_HTTP_SERVER_AGENT_UNNAMED_17))
    icl_stats_inc ("http_server_agent_unnamed_17", &s_http_server_agent_unnamed_17_stats);
#endif        
#define tcb ((http_server_agent_class_context_t *) thread->context)
{

            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        
}
#undef  tcb
    return _rc;
}

