/*---------------------------------------------------------------------------
    http_agent.c - functions for http_agent.

    Generated from http_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "http_agent.h"

#define master_initialise_master_state             1
#define master_accepting_connections_state         2

#define connection_after_init_state                3
#define connection_have_http_request_state         4
#define connection_waiting_for_server_state        5
#define connection_sending_dynamic_state           6
#define connection_sending_static_state            7
#define connection_response_sent_state             8

enum {
    ok_event                                       = 1,
    ready_event                                    = 2,
    new_connection_event                           = 3,
    socket_error_event                             = 4,
    smt_error_event                                = 5,
    shutdown_m_event                               = 6,
    shutdown_event                                 = 7,
    unimplemented_event                            = 8,
    bucket_sent_event                              = 9,
    finished_event                                 = 10,
    persistent_event                               = 11,
    incomplete_event                               = 12,
    http_error_event                               = 13,
    socket_timeout_event                           = 14,
    access_abort_event                             = 15,
    access_retry_event                             = 16,
    exception_event                                = 17,
    socket_closing_event                           = 18,
    empty_event                                    = 19,
    driver_reply_m_event                           = 20,
    driver_abort_m_event                           = 21,
};

#define laststate                                  8
#define lastevent                                  21
#define lastplace                                  91

#define http_agent_agent_name                      "Http-Agent"

#define master_thread_name                         "Master"
#define connection_thread_name                     "Connection"

#define initialise_master_state_name               "Initialise-Master"
#define accepting_connections_state_name           "Accepting-Connections"
#define after_init_state_name                      "After-Init"
#define have_http_request_state_name               "Have-Http-Request"
#define waiting_for_server_state_name              "Waiting-For-Server"
#define sending_dynamic_state_name                 "Sending-Dynamic"
#define sending_static_state_name                  "Sending-Static"
#define response_sent_state_name                   "Response-Sent"

#define ok_event_name                              "Ok"
#define ready_event_name                           "Ready"
#define new_connection_event_name                  "New-Connection"
#define socket_error_event_name                    "Socket-Error"
#define smt_error_event_name                       "Smt-Error"
#define shutdown_m_event_name                      "Shutdown-M"
#define shutdown_event_name                        "Shutdown"
#define unimplemented_event_name                   "Unimplemented"
#define bucket_sent_event_name                     "Bucket-Sent"
#define finished_event_name                        "Finished"
#define persistent_event_name                      "Persistent"
#define incomplete_event_name                      "Incomplete"
#define http_error_event_name                      "Http-Error"
#define socket_timeout_event_name                  "Socket-Timeout"
#define access_abort_event_name                    "Access-Abort"
#define access_retry_event_name                    "Access-Retry"
#define exception_event_name                       "Exception"
#define socket_closing_event_name                  "Socket-Closing"
#define empty_event_name                           "Empty"
#define driver_reply_m_event_name                  "Driver-Reply-M"
#define driver_abort_m_event_name                  "Driver-Abort-M"

#define initialise_server_action_name              "Initialise-Server"
#define allow_server_to_settle_action_name         "Allow-Server-To-Settle"
#define report_server_ready_action_name            "Report-Server-Ready"
#define accept_connection_action_name              "Accept-Connection"
#define create_connection_thread_action_name       "Create-Connection-Thread"
#define report_accept_error_action_name            "Report-Accept-Error"
#define report_smt_error_action_name               "Report-Smt-Error"
#define close_master_socket_action_name            "Close-Master-Socket"
#define initialise_http_request_action_name        "Initialise-Http-Request"
#define do_access_handlers_open_action_name        "Do-Access-Handlers-Open"
#define prepare_to_read_header_action_name         "Prepare-To-Read-Header"
#define read_input_from_socket_action_name         "Read-Input-From-Socket"
#define check_if_header_complete_action_name       "Check-If-Header-Complete"
#define parse_http_request_action_name             "Parse-Http-Request"
#define do_access_handlers_before_action_name      "Do-Access-Handlers-Before"
#define check_if_content_complete_action_name      "Check-If-Content-Complete"
#define pass_request_to_driver_action_name         "Pass-Request-To-Driver"
#define read_content_from_socket_action_name       "Read-Content-From-Socket"
#define process_content_body_action_name           "Process-Content-Body"
#define format_http_reply_action_name              "Format-Http-Reply"
#define get_next_dynamic_bucket_action_name        "Get-Next-Dynamic-Bucket"
#define pause_client_for_abort_action_name         "Pause-Client-For-Abort"
#define flush_the_connection_action_name           "Flush-The-Connection"
#define do_access_handlers_close_action_name       "Do-Access-Handlers-Close"
#define send_bucket_to_client_action_name          "Send-Bucket-To-Client"
#define open_static_content_action_name            "Open-Static-Content"
#define get_next_static_bucket_action_name         "Get-Next-Static-Bucket"
#define do_access_handlers_after_action_name       "Do-Access-Handlers-After"
#define check_if_persistent_connection_action_name  "Check-If-Persistent-Connection"
#define close_static_content_action_name           "Close-Static-Content"
#define pause_client_for_retry_action_name         "Pause-Client-For-Retry"
#define unnamed_31_action_name                     "Unnamed-31"
#define unnamed_32_action_name                     "Unnamed-32"
#define unnamed_33_action_name                     "Unnamed-33"
#define unnamed_34_action_name                     "Unnamed-34"
#define unnamed_37_action_name                     "Unnamed-37"
#define unnamed_38_action_name                     "Unnamed-38"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised


#define BASE_ANIMATE    1

static icl_cache_t
    *s_master_context_cache = NULL;

typedef struct {
    int
        links;

        http_server_t
            *server;                    //  Parent server instance
        smt_log_t
            *alert_log,                 //  Alert log file
            *debug_log;                 //  Debug log file
        smt_socket_t
            *master_socket,             //  Socket for connect requests
            *client_socket;             //  Newly-accepted socket
    
} http_agent_master_context_t;

static icl_cache_t
    *s_connection_context_cache = NULL;

typedef struct {
    int
        links;

        http_server_t
            *server;                    //  Parent server instance
        smt_log_t
            *alert_log,                 //  Alert log file
            *debug_log;                 //  Debug log file
        smt_socket_t
            *socket;                    //  Socket to client
        ipr_bucket_t
            *bucket;                    //  Data read from client
        icl_shortstr_t
            address;                    //  IP address of client
        qbyte
            read_timeout,               //  Socket read timeout
            write_timeout;              //  Socket write timeout
        http_request_t
            *request;                   //  Parsed request context
        http_response_t
            *response;                  //  Response to client
        http_access_context_t
            *access_context;            //  Access module work area
        http_driver_context_t
            *driver_context;            //  Driver module work area
        FILE
            *response_stream;           //  File being sent to client
    
} http_agent_connection_context_t;


static const char *thread_name [] = {
    "<NULL>",
    master_thread_name,
    connection_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    initialise_master_state_name,
    accepting_connections_state_name,
    after_init_state_name,
    have_http_request_state_name,
    waiting_for_server_state_name,
    sending_dynamic_state_name,
    sending_static_state_name,
    response_sent_state_name
};

static const char *event_name [] = {
    "<NULL>",
    ok_event_name,
    ready_event_name,
    new_connection_event_name,
    socket_error_event_name,
    smt_error_event_name,
    shutdown_m_event_name,
    shutdown_event_name,
    unimplemented_event_name,
    bucket_sent_event_name,
    finished_event_name,
    persistent_event_name,
    incomplete_event_name,
    http_error_event_name,
    socket_timeout_event_name,
    access_abort_event_name,
    access_retry_event_name,
    exception_event_name,
    socket_closing_event_name,
    empty_event_name,
    driver_reply_m_event_name,
    driver_abort_m_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT)               ||  defined (BASE_TRACE)  || defined (BASE_TRACE_HTTP_AGENT)   ||  defined (HTTP_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    initialise_server_action_name,
    allow_server_to_settle_action_name,
    report_server_ready_action_name,
    accept_connection_action_name,
    create_connection_thread_action_name,
    report_accept_error_action_name,
    report_smt_error_action_name,
    close_master_socket_action_name,
    initialise_http_request_action_name,
    do_access_handlers_open_action_name,
    prepare_to_read_header_action_name,
    read_input_from_socket_action_name,
    check_if_header_complete_action_name,
    parse_http_request_action_name,
    do_access_handlers_before_action_name,
    check_if_content_complete_action_name,
    pass_request_to_driver_action_name,
    read_content_from_socket_action_name,
    process_content_body_action_name,
    format_http_reply_action_name,
    get_next_dynamic_bucket_action_name,
    pause_client_for_abort_action_name,
    flush_the_connection_action_name,
    do_access_handlers_close_action_name,
    send_bucket_to_client_action_name,
    open_static_content_action_name,
    get_next_static_bucket_action_name,
    do_access_handlers_after_action_name,
    check_if_persistent_connection_action_name,
    close_static_content_action_name,
    pause_client_for_retry_action_name,
    unnamed_31_action_name,
    unnamed_32_action_name,
    unnamed_33_action_name,
    unnamed_34_action_name,
    unnamed_37_action_name,
    unnamed_38_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_INITIALISE_SERVER))
static icl_stats_t *s_http_agent_initialise_server_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_ALLOW_SERVER_TO_SETTLE))
static icl_stats_t *s_http_agent_allow_server_to_settle_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_SERVER_READY))
static icl_stats_t *s_http_agent_report_server_ready_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_ACCEPT_CONNECTION))
static icl_stats_t *s_http_agent_accept_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CREATE_CONNECTION_THREAD))
static icl_stats_t *s_http_agent_create_connection_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_ACCEPT_ERROR))
static icl_stats_t *s_http_agent_report_accept_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_SMT_ERROR))
static icl_stats_t *s_http_agent_report_smt_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CLOSE_MASTER_SOCKET))
static icl_stats_t *s_http_agent_close_master_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_INITIALISE_HTTP_REQUEST))
static icl_stats_t *s_http_agent_initialise_http_request_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_OPEN))
static icl_stats_t *s_http_agent_do_access_handlers_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PREPARE_TO_READ_HEADER))
static icl_stats_t *s_http_agent_prepare_to_read_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_READ_INPUT_FROM_SOCKET))
static icl_stats_t *s_http_agent_read_input_from_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_HEADER_COMPLETE))
static icl_stats_t *s_http_agent_check_if_header_complete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PARSE_HTTP_REQUEST))
static icl_stats_t *s_http_agent_parse_http_request_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_BEFORE))
static icl_stats_t *s_http_agent_do_access_handlers_before_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_CONTENT_COMPLETE))
static icl_stats_t *s_http_agent_check_if_content_complete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PASS_REQUEST_TO_DRIVER))
static icl_stats_t *s_http_agent_pass_request_to_driver_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_READ_CONTENT_FROM_SOCKET))
static icl_stats_t *s_http_agent_read_content_from_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PROCESS_CONTENT_BODY))
static icl_stats_t *s_http_agent_process_content_body_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_FORMAT_HTTP_REPLY))
static icl_stats_t *s_http_agent_format_http_reply_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_GET_NEXT_DYNAMIC_BUCKET))
static icl_stats_t *s_http_agent_get_next_dynamic_bucket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PAUSE_CLIENT_FOR_ABORT))
static icl_stats_t *s_http_agent_pause_client_for_abort_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_FLUSH_THE_CONNECTION))
static icl_stats_t *s_http_agent_flush_the_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_CLOSE))
static icl_stats_t *s_http_agent_do_access_handlers_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_SEND_BUCKET_TO_CLIENT))
static icl_stats_t *s_http_agent_send_bucket_to_client_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_OPEN_STATIC_CONTENT))
static icl_stats_t *s_http_agent_open_static_content_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_GET_NEXT_STATIC_BUCKET))
static icl_stats_t *s_http_agent_get_next_static_bucket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_AFTER))
static icl_stats_t *s_http_agent_do_access_handlers_after_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_PERSISTENT_CONNECTION))
static icl_stats_t *s_http_agent_check_if_persistent_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CLOSE_STATIC_CONTENT))
static icl_stats_t *s_http_agent_close_static_content_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PAUSE_CLIENT_FOR_RETRY))
static icl_stats_t *s_http_agent_pause_client_for_retry_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_31))
static icl_stats_t *s_http_agent_unnamed_31_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_32))
static icl_stats_t *s_http_agent_unnamed_32_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_33))
static icl_stats_t *s_http_agent_unnamed_33_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_34))
static icl_stats_t *s_http_agent_unnamed_34_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_37))
static icl_stats_t *s_http_agent_unnamed_37_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_38))
static icl_stats_t *s_http_agent_unnamed_38_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    http_agent_manager              (smt_thread_t **thread);
static int    http_agent_catcher              (smt_thread_t *thread);
static int    http_agent_master_destroy       (smt_thread_t *thread);
static int    http_agent_connection_destroy   (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_HTTP_AGENT)                 ||  defined (HTTP_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_initialise_server        (smt_thread_t *thread);
static int    action_allow_server_to_settle   (smt_thread_t *thread);
static int    action_report_server_ready      (smt_thread_t *thread);
static int    action_accept_connection        (smt_thread_t *thread);
static int    action_create_connection_thread  (smt_thread_t *thread);
static int    action_report_accept_error      (smt_thread_t *thread);
static int    action_report_smt_error         (smt_thread_t *thread);
static int    action_close_master_socket      (smt_thread_t *thread);
static int    action_initialise_http_request  (smt_thread_t *thread);
static int    action_do_access_handlers_open  (smt_thread_t *thread);
static int    action_prepare_to_read_header   (smt_thread_t *thread);
static int    action_read_input_from_socket   (smt_thread_t *thread);
static int    action_check_if_header_complete  (smt_thread_t *thread);
static int    action_parse_http_request       (smt_thread_t *thread);
static int    action_do_access_handlers_before  (smt_thread_t *thread);
static int    action_check_if_content_complete  (smt_thread_t *thread);
static int    action_pass_request_to_driver   (smt_thread_t *thread);
static int    action_read_content_from_socket  (smt_thread_t *thread);
static int    action_process_content_body     (smt_thread_t *thread);
static int    action_format_http_reply        (smt_thread_t *thread);
static int    action_get_next_dynamic_bucket  (smt_thread_t *thread);
static int    action_pause_client_for_abort   (smt_thread_t *thread);
static int    action_flush_the_connection     (smt_thread_t *thread);
static int    action_do_access_handlers_close  (smt_thread_t *thread);
static int    action_send_bucket_to_client    (smt_thread_t *thread);
static int    action_open_static_content      (smt_thread_t *thread);
static int    action_get_next_static_bucket   (smt_thread_t *thread);
static int    action_do_access_handlers_after  (smt_thread_t *thread);
static int    action_check_if_persistent_connection  (smt_thread_t *thread);
static int    action_close_static_content     (smt_thread_t *thread);
static int    action_pause_client_for_retry   (smt_thread_t *thread);
static int    action_unnamed_31               (smt_thread_t *thread);
static int    action_unnamed_32               (smt_thread_t *thread);
static int    action_unnamed_33               (smt_thread_t *thread);
static int    action_unnamed_34               (smt_thread_t *thread);
static int    action_unnamed_37               (smt_thread_t *thread);
static int    action_unnamed_38               (smt_thread_t *thread);
//  Function definitions                                                       

int
http_agent_driver_reply (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                driver_reply_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
http_agent_driver_abort (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == http_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                driver_abort_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

void
http_agent_term (void)
{

}

int 
http_agent_init (
    http_server_t * server)
{
    int
        rc = SMT_OK;                    //  Return code

    s_master_context_cache = icl_cache_get (sizeof (http_agent_master_context_t));
    s_connection_context_cache = icl_cache_get (sizeof (http_agent_connection_context_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    //
    smt_thread_t
        *master_thread;
    http_agent_master_context_t
        *master_tcb;

    //  Currently we poke the values into the tcb
    master_thread = http_agent_master_thread_new ();
    master_tcb = master_thread->context;
    master_tcb->server = http_server_link (server);
    master_tcb->alert_log = smt_log_link (server->alert_log);
    master_tcb->debug_log = smt_log_link (server->debug_log);

    smt_thread_wake    (master_thread);
    smt_thread_unlink (&master_thread);

    }
    icl_system_register (NULL, http_agent_term);
    
    return rc;
}

void
http_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
http_agent_master_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (http_agent_manager,
                             http_agent_catcher,
                             http_agent_master_destroy);
                             

#if (defined (HTTP_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "http_agent",
                master_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_master_context_cache);
        memset (thread->context, 0, sizeof (http_agent_master_context_t));
        ((http_agent_master_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((http_agent_master_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
http_agent_connection_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (http_agent_manager,
                             http_agent_catcher,
                             http_agent_connection_destroy);
                             

#if (defined (HTTP_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "http_agent",
                connection_thread_name);
#endif

        thread->place   = 14;
        thread->context = icl_mem_cache_alloc (s_connection_context_cache);
        memset (thread->context, 0, sizeof (http_agent_connection_context_t));
        ((http_agent_connection_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
http_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == http_agent_manager);
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
            case 63:
            case 31:
            case 65:
            case 78:
            case 11:
            case 12:
            case 13:
            case 4:
            case 5:
            case 6:
            case 48:
            case 19:
            case 50:
            case 51:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 34:
            case 35:
            case 82:
            case 37:
            case 84:
            case 42:
            case 62:
            case 87:
            case 88:
            case 89:
            case 90:
            case 91:
            case 49:
            case 71:
            case 61:
            case 38:
            case 74:
            case 75:
            case 76:
            case 77:
            case 64:
            case 47:
            case 36:
            case 58:
            case 44:
                thread->place = SMT_TERM_PLACE;
                break;
            case 10:
            case 9:
            case 8:
            case 3:
                thread->place = 7;
                break;
            case 20:
                thread->place = 15;
                break;
            case 21:
            case 80:
            case 86:
            case 32:
            case 60:
            case 17:
            case 46:
            case 29:
            case 16:
            case 81:
            case 73:
            case 33:
                thread->place = 27;
                break;
            case 56:
            case 69:
            case 85:
                thread->place = 79;
                break;
            case 43:
            case 30:
            case 53:
            case 70:
            case 57:
            case 83:
            case 59:
            case 41:
            case 18:
            case 54:
            case 40:
                thread->place = 52;
                break;
            case 68:
            case 72:
            case 55:
            case 67:
                thread->place = 66;
                break;
            case 45:
            case 28:
                thread->place = 39;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Master thread
            if (!thread->shutting_down) {
{
#define tcb ((http_agent_master_context_t *) thread->context)

        thread->animate = http_config_animate (http_config);
        smt_socket_request_trace (http_config_trace (http_config));
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
        if (thread->place < 7)                  //  Initialise Master state
            thread->place = 2;
        else
        if (thread->place < 14)                 //  Accepting Connections state
            thread->place = 7;
        else
        if (thread->place == 14) { //               Connection thread
            if (!thread->shutting_down) {
{
#define tcb ((http_agent_connection_context_t *) thread->context)

        thread->animate    = http_config_animate (http_config);
        tcb->read_timeout  = http_config_read_timeout (http_config) * 1000;
        tcb->write_timeout = http_config_write_timeout (http_config) * 1000;
        icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->clients));
        the_next_event = ok_event;
    
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
        if (thread->place < 27)                 //  After Init state
            thread->place = 15;
        else
        if (thread->place < 39)                 //  Have Http Request state
            thread->place = 27;
        else
        if (thread->place < 52)                 //  Waiting For Server state
            thread->place = 39;
        else
        if (thread->place < 66)                 //  Sending Dynamic state
            thread->place = 52;
        else
        if (thread->place < 79)                 //  Sending Static state
            thread->place = 66;
        else
            thread->place = 79;
    }

    switch (thread->place) {
        case 2:                         //  initialise master state             
{
#define tcb ((http_agent_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 4; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 5; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 6; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 7:                         //  accepting connections state         
{
#define tcb ((http_agent_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ready_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 8; break;
                case new_connection_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 9; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 10; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 11; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 12; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 15:                        //  after init state                    
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 16; break;
                case incomplete_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 17; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 18; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 19; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 20; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 21; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 22; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 23; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 24; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 25; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 26; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 27:                        //  have http request state             
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 28; break;
                case incomplete_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 29; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 30; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 31; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 32; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 33; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 34; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 35; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 36; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 37; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 38; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 39:                        //  waiting for server state            
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case unimplemented_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 40; break;
                case driver_reply_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
                    thread->place = 41; break;
                case driver_abort_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 42; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 43; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 44; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 45; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 46; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 47; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 48; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 49; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 50; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 51; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 52:                        //  sending dynamic state               
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 53; break;
                case bucket_sent_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 54; break;
                case finished_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 55; break;
                case empty_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 56; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 57; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 58; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 59; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 60; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 61; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 62; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 63; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 64; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 65; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 66:                        //  sending static state                
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 67; break;
                case bucket_sent_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 68; break;
                case finished_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 69; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 70; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 71; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 72; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 73; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 74; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 75; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 76; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 77; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 78; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 79:                        //  response sent state                 
{
#define tcb ((http_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case persistent_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 80; break;
                case incomplete_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 81; break;
                case finished_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 82; break;
                case http_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 83; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 84; break;
                case access_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 85; break;
                case access_retry_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 86; break;
                case exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 87; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 88; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 89; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 90; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 91; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_initialise_server (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_allow_server_to_settle (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_report_server_ready (thread);

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
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 8:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_create_connection_thread (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_report_accept_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
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
        case 11:
        case 4:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_report_smt_error (thread);

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
        case 12:
        case 5:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 8);
#endif
                    rc = action_close_master_socket (thread);

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
        case 13:
        case 6:
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
        case 16:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_initialise_http_request (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_do_access_handlers_open (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_prepare_to_read_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_read_input_from_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_check_if_header_complete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_parse_http_request (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_do_access_handlers_before (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_check_if_content_complete (thread);

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

                case 9:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 28:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 17);
#endif
                    rc = action_pass_request_to_driver (thread);

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
        case 40:
        case 83:
        case 70:
        case 57:
        case 41:
        case 43:
        case 30:
        case 18:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 20);
#endif
                    rc = action_format_http_reply (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 21);
#endif
                    rc = action_get_next_dynamic_bucket (thread);

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
        case 53:
        case 67:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 25);
#endif
                    rc = action_send_bucket_to_client (thread);

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
        case 54:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 21);
#endif
                    rc = action_get_next_dynamic_bucket (thread);

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
        case 55:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 26);
#endif
                    rc = action_open_static_content (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 27);
#endif
                    rc = action_get_next_static_bucket (thread);

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
        case 68:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 27);
#endif
                    rc = action_get_next_static_bucket (thread);

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
        case 69:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 30);
#endif
                    rc = action_close_static_content (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 28);
#endif
                    rc = action_do_access_handlers_after (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 29);
#endif
                    rc = action_check_if_persistent_connection (thread);

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
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 80:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_prepare_to_read_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_read_input_from_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_check_if_header_complete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_parse_http_request (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_do_access_handlers_before (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_check_if_content_complete (thread);

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

                case 7:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 81:
        case 17:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_read_input_from_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_check_if_header_complete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_parse_http_request (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_do_access_handlers_before (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_check_if_content_complete (thread);

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

                case 6:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 82:
        case 84:
        case 87:
        case 71:
        case 74:
        case 58:
        case 61:
        case 44:
        case 47:
        case 31:
        case 34:
        case 19:
        case 22:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 23);
#endif
                    rc = action_flush_the_connection (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 85:
        case 72:
        case 59:
        case 42:
        case 45:
        case 32:
        case 20:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 22);
#endif
                    rc = action_pause_client_for_abort (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 23);
#endif
                    rc = action_flush_the_connection (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
                    if (thread->place == 42)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 86:
        case 73:
        case 60:
        case 46:
        case 33:
        case 21:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 31);
#endif
                    rc = action_pause_client_for_retry (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_do_access_handlers_before (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_check_if_content_complete (thread);

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
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 88:
        case 89:
        case 91:
        case 75:
        case 76:
        case 78:
        case 62:
        case 63:
        case 65:
        case 48:
        case 49:
        case 51:
        case 35:
        case 36:
        case 38:
        case 23:
        case 24:
        case 26:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 90:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 32);
#endif
                    rc = action_unnamed_31 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 77:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 33);
#endif
                    rc = action_unnamed_32 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 56:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 28);
#endif
                    rc = action_do_access_handlers_after (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 29);
#endif
                    rc = action_check_if_persistent_connection (thread);

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
        case 64:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 34);
#endif
                    rc = action_unnamed_33 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 50:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 35);
#endif
                    rc = action_unnamed_34 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 29:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 18);
#endif
                    rc = action_read_content_from_socket (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 19);
#endif
                    rc = action_process_content_body (thread);

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
        case 37:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 36);
#endif
                    rc = action_unnamed_37 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
        case 25:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 37);
#endif
                    rc = action_unnamed_38 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (HTTP_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_do_access_handlers_close (thread);

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
http_agent_catcher (smt_thread_t *thread)
{
    switch (thread->result) {
        case 0:
            return SMT_OK;
        case SMT_SOCKET_CLOSING:
            thread->_exception_event = socket_closing_event; break;
        case SMT_SOCKET_ERROR:
            thread->_exception_event = socket_error_event; break;
        case SMT_TIMEOUT:
            thread->_exception_event = socket_timeout_event; break;
        default:
            thread->_exception_event = smt_error_event;
    }
    thread->_exception_raised = TRUE;
    
    return SMT_OK;
}


static int    
http_agent_master_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (HTTP_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "http_agent",
                 master_thread_name);

#endif
{
#define tcb ((http_agent_master_context_t *) thread->context)

        smt_log_unlink     (&tcb->alert_log);
        smt_log_unlink     (&tcb->debug_log);
        http_server_unlink (&tcb->server);
        smt_socket_destroy (&tcb->client_socket);
        smt_socket_destroy (&tcb->master_socket);
    
#undef  tcb
}
    if (--((http_agent_master_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
http_agent_connection_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (HTTP_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "http_agent",
                 connection_thread_name);

#endif
{
#define tcb ((http_agent_connection_context_t *) thread->context)

        icl_atomic_dec32 ((volatile qbyte *) &(tcb->server->clients));
        http_server_unlink (&tcb->server);
        smt_log_unlink (&tcb->alert_log);
        smt_log_unlink (&tcb->debug_log);
        smt_socket_destroy (&tcb->socket);
        ipr_bucket_unlink (&tcb->bucket);
        http_request_unlink (&tcb->request);
        http_response_unlink (&tcb->response);
        http_access_context_destroy (&tcb->access_context);
        http_driver_context_destroy (&tcb->driver_context);
    
#undef  tcb
}
    if (--((http_agent_connection_context_t *) thread->context)->links == 0)
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
        if (thread->place < 7)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 5;   break;
                case 5: *event_id = 6;   break;
                case 6: *event_id = 7;   break;
            }
          }
        else
          {
            *state_id = 2;
            switch (thread->place) {
                case 8: *event_id = 2;   *nextstate_id = 2;   break;
                case 9: *event_id = 3;   *nextstate_id = 2;   break;
                case 10: *event_id = 4;   *nextstate_id = 2;   break;
                case 11: *event_id = 5;   break;
                case 12: *event_id = 6;   break;
                case 13: *event_id = 7;   break;
            }
          }
      }
    else
      {
        *thread_type = 2;
        if (thread->place < 27)
          {
            *state_id = 3;
            switch (thread->place) {
                case 16: *event_id = 1;   *nextstate_id = 4;   break;
                case 17: *event_id = 12;   *nextstate_id = 4;   break;
                case 18: *event_id = 13;   *nextstate_id = 6;   break;
                case 19: *event_id = 14;   break;
                case 20: *event_id = 15;   *nextstate_id = 3;   break;
                case 21: *event_id = 16;   *nextstate_id = 4;   break;
                case 22: *event_id = 17;   break;
                case 23: *event_id = 4;   break;
                case 24: *event_id = 18;   break;
                case 25: *event_id = 5;   break;
                case 26: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 39)
          {
            *state_id = 4;
            switch (thread->place) {
                case 28: *event_id = 1;   *nextstate_id = 5;   break;
                case 29: *event_id = 12;   *nextstate_id = 4;   break;
                case 30: *event_id = 13;   *nextstate_id = 6;   break;
                case 31: *event_id = 14;   break;
                case 32: *event_id = 15;   *nextstate_id = 4;   break;
                case 33: *event_id = 16;   *nextstate_id = 4;   break;
                case 34: *event_id = 17;   break;
                case 35: *event_id = 4;   break;
                case 36: *event_id = 18;   break;
                case 37: *event_id = 5;   break;
                case 38: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 52)
          {
            *state_id = 5;
            switch (thread->place) {
                case 40: *event_id = 8;   *nextstate_id = 6;   break;
                case 41: *event_id = 20;   *nextstate_id = 6;   break;
                case 42: *event_id = 21;   break;
                case 43: *event_id = 13;   *nextstate_id = 6;   break;
                case 44: *event_id = 14;   break;
                case 45: *event_id = 15;   *nextstate_id = 5;   break;
                case 46: *event_id = 16;   *nextstate_id = 4;   break;
                case 47: *event_id = 17;   break;
                case 48: *event_id = 4;   break;
                case 49: *event_id = 18;   break;
                case 50: *event_id = 5;   break;
                case 51: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 66)
          {
            *state_id = 6;
            switch (thread->place) {
                case 53: *event_id = 1;   *nextstate_id = 6;   break;
                case 54: *event_id = 9;   *nextstate_id = 6;   break;
                case 55: *event_id = 10;   *nextstate_id = 7;   break;
                case 56: *event_id = 19;   *nextstate_id = 8;   break;
                case 57: *event_id = 13;   *nextstate_id = 6;   break;
                case 58: *event_id = 14;   break;
                case 59: *event_id = 15;   *nextstate_id = 6;   break;
                case 60: *event_id = 16;   *nextstate_id = 4;   break;
                case 61: *event_id = 17;   break;
                case 62: *event_id = 4;   break;
                case 63: *event_id = 18;   break;
                case 64: *event_id = 5;   break;
                case 65: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 79)
          {
            *state_id = 7;
            switch (thread->place) {
                case 67: *event_id = 1;   *nextstate_id = 7;   break;
                case 68: *event_id = 9;   *nextstate_id = 7;   break;
                case 69: *event_id = 10;   *nextstate_id = 8;   break;
                case 70: *event_id = 13;   *nextstate_id = 6;   break;
                case 71: *event_id = 14;   break;
                case 72: *event_id = 15;   *nextstate_id = 7;   break;
                case 73: *event_id = 16;   *nextstate_id = 4;   break;
                case 74: *event_id = 17;   break;
                case 75: *event_id = 4;   break;
                case 76: *event_id = 18;   break;
                case 77: *event_id = 5;   break;
                case 78: *event_id = 7;   break;
            }
          }
        else
          {
            *state_id = 8;
            switch (thread->place) {
                case 80: *event_id = 11;   *nextstate_id = 4;   break;
                case 81: *event_id = 12;   *nextstate_id = 4;   break;
                case 82: *event_id = 10;   break;
                case 83: *event_id = 13;   *nextstate_id = 6;   break;
                case 84: *event_id = 14;   break;
                case 85: *event_id = 15;   *nextstate_id = 8;   break;
                case 86: *event_id = 16;   *nextstate_id = 4;   break;
                case 87: *event_id = 17;   break;
                case 88: *event_id = 4;   break;
                case 89: *event_id = 18;   break;
                case 90: *event_id = 5;   break;
                case 91: *event_id = 7;   break;
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
    icl_console_print ("http_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_HTTP_AGENT))

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
            "http_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "http_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "http_agent",
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_HTTP_AGENT)                   ||  defined (HTTP_AGENT_TRACE))
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
                                       "http_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "http_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "http_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_initialise_server (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_INITIALISE_SERVER))
    icl_stats_inc ("http_agent_initialise_server", &s_http_agent_initialise_server_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        icl_shortstr_t
            hostname;                   //  Holds host name
        apr_pool_t
            *pool;                      //  APR pool for host addresses
        apr_sockaddr_t
            *sockaddr;                  //  The associated socket addresses
        char
            *port,                      //  Port to bind to
            *address,                   //  To hold socket address string
            *user,                      //  User to run as
            *group;                     //  Group to run as
        int
            rc = SMT_OK,
            new_size,                   //  New buffer size
            old_size;                   //  Old buffer size
        static Bool
            rcv_warned = FALSE,
            snd_warned = FALSE;

        port  = http_config_port  (http_config);
        user  = http_config_user  (http_config);
        group = http_config_group (http_config);
        if (atoi (port) < 80
        ||  atoi (port) > 65535) {
            smt_log_print (tcb->alert_log, "E: port %s is not valid for HTTP service", port);
            exit (1);                   //  FIXME: see BASE2-289
        }
        else {
            apr_pool_create (&pool, icl_global_pool);
            apr_pool_tag (pool, "http_initialise_server)");

            //  Report hostname and all visible network interfaces
            if (apr_gethostname (hostname, ICL_SHORTSTR_MAX, pool) == APR_SUCCESS) {
                apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
                if (sockaddr) {
                    apr_sockaddr_ip_get (&address, sockaddr);
                    smt_log_print (tcb->alert_log, "I: server host name is %s (%s)", hostname, address);
                }
                else
                    smt_log_print (tcb->alert_log, "E: cannot translate host name '%s'", hostname);
            }
            //  Report which interfaces server is going to listen to
            icl_shortstr_cpy (hostname, http_config_listen (http_config));
            if (streq (hostname, "*")) {
                smt_log_print (tcb->alert_log, "I: listening on port %s, all network interfaces", port);
                icl_shortstr_cpy (hostname, APR_ANYADDR);
            }
            else
                smt_log_print (tcb->alert_log, "I: listening on port %s, interface %s", port, hostname);

            apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
            if (sockaddr)
                apr_sockaddr_ip_get (&address, sockaddr);
            else
                smt_log_print (tcb->alert_log, "E: cannot translate host name '%s'", hostname);

            apr_pool_destroy (pool);
            tcb->master_socket = smt_socket_new ();
            if (tcb->master_socket) {
                //  Set Nagle's algorithm on or off
                rc = smt_socket_set_nodelay (tcb->master_socket, http_config_tcp_nodelay (http_config));
                if (rc)
                    smt_log_print (tcb->alert_log, "W: could not set TCP/IP NODELAY option");

                //  Report and set socket buffers if wanted
                new_size = http_config_tcp_rcvbuf (http_config);
                if (new_size) {
                    rc = smt_socket_set_rcvbuf (tcb->master_socket, new_size, &old_size);
                    if (!rcv_warned) {
                        rcv_warned = TRUE;
                        if (rc)
                            smt_log_print (tcb->alert_log,
                                "W: could not set TCP/IP receive buffer size");
                        else
                            smt_log_print (tcb->alert_log,
                                "I: TCP/IP receive buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                new_size = http_config_tcp_sndbuf (http_config);
                if (new_size) {
                    rc = smt_socket_set_sndbuf (tcb->master_socket, new_size, &old_size);
                    if (!snd_warned) {
                        snd_warned = TRUE;
                        if (rc)
                            smt_log_print (tcb->alert_log,
                                "W: could not set TCP/IP send buffer size");
                        else
                            smt_log_print (tcb->alert_log,
                                "I: TCP/IP sending buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                //  Start listening for connections on socket
                rc = smt_socket_passive (tcb->master_socket, thread, port, hostname, 500);

                //  Try to drop to safe user/group, complain if not possible
                if (*user && *group) {
                    if (ipr_uid_run_as (user, group)) {
                        smt_log_print (tcb->alert_log,
                            "E: could not run as user=%s group=%s", user, group);
                        exit (1);
                    }
                    else
                        smt_log_print (tcb->alert_log,
                            "I: server is now running as user=%s group=%s", user, group);
                }
            }
            if (!tcb->master_socket || (rc != SMT_OK)) {
                smt_log_print (tcb->alert_log,
                    "E: could not open port %s: %s", port, smt_thread_error (thread));
                exit (1);               //  FIXME: see BASE2-289
            }
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_allow_server_to_settle (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_ALLOW_SERVER_TO_SETTLE))
    icl_stats_inc ("http_agent_allow_server_to_settle", &s_http_agent_allow_server_to_settle_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        smt_timer_request_delay (thread, 1 * 1000 * 1000, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_server_ready (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_SERVER_READY))
    icl_stats_inc ("http_agent_report_server_ready", &s_http_agent_report_server_ready_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        smt_log_print (tcb->debug_log, "I: ready for incoming HTTP requests");
        smt_thread_set_next_event (thread, ready_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_accept_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_ACCEPT_CONNECTION))
    icl_stats_inc ("http_agent_accept_connection", &s_http_agent_accept_connection_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        smt_socket_request_accept (thread, tcb->master_socket, 0,
            new_connection_event, &tcb->client_socket);
    
}
#undef  tcb
    return _rc;
}

static int
action_create_connection_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CREATE_CONNECTION_THREAD))
    icl_stats_inc ("http_agent_create_connection_thread", &s_http_agent_create_connection_thread_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        smt_thread_t
            *connection_thread;
        http_agent_connection_context_t
            *connection_tcb;

        connection_thread = http_agent_connection_thread_new ();
        connection_thread->priority = SMT_PRIORITY_HIGH;

        connection_tcb            = connection_thread->context;
        connection_tcb->socket    = tcb->client_socket;
        connection_tcb->server    = http_server_link (tcb->server);
        connection_tcb->alert_log = smt_log_link (tcb->alert_log);
        connection_tcb->debug_log = smt_log_link (tcb->debug_log);
        tcb->client_socket        = NULL;

        smt_thread_wake    (connection_thread);
        smt_thread_unlink (&connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_accept_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_ACCEPT_ERROR))
    icl_stats_inc ("http_agent_report_accept_error", &s_http_agent_report_accept_error_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        int
            accept_retry_timeout;

        smt_log_print (tcb->alert_log,
            "E: could not accept connection: %s", smt_thread_error (thread));

        //  If we ran out of files wait the configured amount of time and retry
        if (thread->result == SMT_SOCKET_ERROR &&
           (APR_STATUS_IS_ENFILE(thread->error) ||
            APR_STATUS_IS_EMFILE(thread->error))) {
            accept_retry_timeout = http_config_accept_retry_timeout (http_config);
            smt_log_print (tcb->alert_log,
                "E: waiting %d seconds before accepting new connections",
                accept_retry_timeout);
            smt_timer_request_delay (thread, accept_retry_timeout * 1000 * 1000,
                SMT_NULL_EVENT);
        }
        //  Otherwise, some other error happened, assume it's bad
        else {
            abort ();
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_report_smt_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_REPORT_SMT_ERROR))
    icl_stats_inc ("http_agent_report_smt_error", &s_http_agent_report_smt_error_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        smt_log_print (tcb->alert_log,
            "E: SMT error: %s", smt_thread_error (thread));
    
}
#undef  tcb
    return _rc;
}

static int
action_close_master_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CLOSE_MASTER_SOCKET))
    icl_stats_inc ("http_agent_close_master_socket", &s_http_agent_close_master_socket_stats);
#endif        
#define tcb ((http_agent_master_context_t *) thread->context)
{

        if (tcb->master_socket)
            smt_socket_request_close (thread, &tcb->master_socket, 0, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_initialise_http_request (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_INITIALISE_HTTP_REQUEST))
    icl_stats_inc ("http_agent_initialise_http_request", &s_http_agent_initialise_http_request_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        apr_sockaddr_t
            *sockaddr;
        char
            *address;

        //  Get client IP address
        apr_socket_addr_get (&sockaddr, APR_REMOTE, tcb->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&address, sockaddr);
        icl_shortstr_cpy (tcb->address, address);
        apr_socket_opt_set (tcb->socket->pollfd.desc.s, APR_TCP_NODELAY, 1);
    
}
#undef  tcb
    return _rc;
}

static int
action_do_access_handlers_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_OPEN))
    icl_stats_inc ("http_agent_do_access_handlers_open", &s_http_agent_do_access_handlers_open_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc = HTTP_ACCESS_CONTINUE;

        assert (!tcb->access_context);
        tcb->access_context = http_access_context_new (tcb->server);
        http_access_context_set_thread  (tcb->access_context, thread);
        http_access_context_set_address (tcb->access_context, tcb->address);
        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_open (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_prepare_to_read_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PREPARE_TO_READ_HEADER))
    icl_stats_inc ("http_agent_prepare_to_read_header", &s_http_agent_prepare_to_read_header_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  Allocate new bucket for request data
        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_new (http_config_header_max (http_config));
    
}
#undef  tcb
    return _rc;
}

static int
action_read_input_from_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_READ_INPUT_FROM_SOCKET))
    icl_stats_inc ("http_agent_read_input_from_socket", &s_http_agent_read_input_from_socket_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        size_t
            available;                  //  Bytes available in bucket

        assert (tcb->bucket);
        available = tcb->bucket->max_size - tcb->bucket->cur_size - 1;
        if (available)
            smt_socket_request_read (
                thread,
                tcb->socket,
                tcb->read_timeout,
                1,
                available,
                tcb->bucket->data + tcb->bucket->cur_size,
                SMT_NULL_EVENT);
        else {
            smt_log_print (tcb->alert_log,
                "E: (%s) invalid HTTP request - header too large", tcb->address);
            smt_thread_raise_exception (thread, exception_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_check_if_header_complete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_HEADER_COMPLETE))
    icl_stats_inc ("http_agent_check_if_header_complete", &s_http_agent_check_if_header_complete_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  Update bucket size with amount of data read
        tcb->bucket->cur_size += tcb->socket->io_size;
        //  Look for complete header, ending in blank line
        if (ipr_bucket_find (tcb->bucket, 0, (byte *) "\r\n\r\n", 4)) {
            ipr_meter_count (tcb->server->imeter);
            icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->requests));
        }
        else
            smt_thread_raise_exception (thread, incomplete_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_parse_http_request (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PARSE_HTTP_REQUEST))
    icl_stats_inc ("http_agent_parse_http_request", &s_http_agent_parse_http_request_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  Clean-up any old context for persistent connections
        http_request_unlink (&tcb->request);
        http_response_unlink (&tcb->response);

        //  Terminate bucket data to allow string processing
        tcb->bucket->data [tcb->bucket->cur_size] = 0;

        //  Create & parse request, and response
        tcb->request  = http_request_new (tcb->server, tcb->socket, tcb->bucket);
        tcb->response = http_response_new (tcb->server, tcb->request);
        http_request_set_address (tcb->request, tcb->address);

        http_access_context_set_request  (tcb->access_context, tcb->request);
        http_access_context_set_response (tcb->access_context, tcb->response);

        //  Bucket is now known as tcb->request->header
        ipr_bucket_unlink (&tcb->bucket);

        if (tcb->response->reply_code != HTTP_REPLY_OK)
            smt_thread_raise_exception (thread, http_error_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_do_access_handlers_before (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_BEFORE))
    icl_stats_inc ("http_agent_do_access_handlers_before", &s_http_agent_do_access_handlers_before_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc = HTTP_ACCESS_CONTINUE;

        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_before (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            else
            if (rc == HTTP_ACCESS_RETRY) {
                //  Wait a short time and then retry the access modules
                smt_thread_raise_exception (thread, access_retry_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
        if (rc == HTTP_ACCESS_CONTINUE
        &&  tcb->response->reply_code != HTTP_REPLY_OK)
            smt_thread_raise_exception (thread, http_error_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_if_content_complete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_CONTENT_COMPLETE))
    icl_stats_inc ("http_agent_check_if_content_complete", &s_http_agent_check_if_content_complete_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  If there is content, prepare to get it all
        if (http_request_content_parse (tcb->request))
            the_next_event = incomplete_event;
        else
            the_next_event = ok_event;
    
}
#undef  tcb
    return _rc;
}

static int
action_pass_request_to_driver (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PASS_REQUEST_TO_DRIVER))
    icl_stats_inc ("http_agent_pass_request_to_driver", &s_http_agent_pass_request_to_driver_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        http_driver_module_t
            *driver_module = NULL;      //  Driver, if found
        ipr_looseref_t
            *looseref;                  //  Driver module reference

        looseref = ipr_looseref_list_first (tcb->server->http_driver_module_list);
        while (looseref) {
            driver_module = (http_driver_module_t *) (looseref->object);
            if (ipr_str_prefixed (tcb->request->path, driver_module->path)) {
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
        if (looseref) {
            assert (!tcb->request->pathroot);
            if (tcb->driver_context)
                http_driver_context_init (tcb->driver_context, tcb->request, tcb->response);
            else
                tcb->driver_context = http_driver_context_new (
                    thread, tcb->server, tcb->request, tcb->response);

            assert (tcb->request->path);
            tcb->request->pathroot = icl_mem_strdup (driver_module->path);
            tcb->request->pathinfo = icl_mem_strdup (tcb->request->path + strlen (driver_module->path));
            if (tcb->request->method == HTTP_METHOD_GET)
                http_driver_module_request_get (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_HEAD)
                http_driver_module_request_head (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_POST)
                http_driver_module_request_post (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_PUT)
                http_driver_module_request_put (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_DELETE)
                http_driver_module_request_delete (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_MOVE)
                http_driver_module_request_move (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_COPY)
                http_driver_module_request_copy (driver_module, tcb->driver_context);
            else
                assert (0);         //  Method has already been validated
        }
        else {
            http_response_set_error (tcb->response, HTTP_REPLY_NOTIMPLEMENTED,
                "The '%s' method is not implemented on this service", tcb->request->request_method);
            the_next_event = unimplemented_event;
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_read_content_from_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_READ_CONTENT_FROM_SOCKET))
    icl_stats_inc ("http_agent_read_content_from_socket", &s_http_agent_read_content_from_socket_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        size_t
            read_min, read_max;

        if (tcb->request->content)
            read_min = read_max = http_request_content_pending (tcb->request);
        else {
            read_max = MIN (IPR_BUCKET_MAX_SIZE, http_request_content_pending (tcb->request));
            read_min = MIN (read_max, (uint) http_request_content_pending (tcb->request));
        }
        tcb->bucket = ipr_bucket_new (read_max);
        smt_socket_request_read (
            thread,
            tcb->socket,
            tcb->read_timeout,
            read_min,
            read_max,
            tcb->bucket->data,
            SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_process_content_body (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PROCESS_CONTENT_BODY))
    icl_stats_inc ("http_agent_process_content_body", &s_http_agent_process_content_body_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  Update bucket size with amount of data read
        tcb->bucket->cur_size += tcb->socket->io_size;
        http_request_content_append (tcb->request, tcb->bucket);
        ipr_bucket_unlink (&tcb->bucket);

        if (http_request_content_pending (tcb->request))
            the_next_event = incomplete_event;
        else
            the_next_event = ok_event;
    
}
#undef  tcb
    return _rc;
}

static int
action_format_http_reply (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_FORMAT_HTTP_REPLY))
    icl_stats_inc ("http_agent_format_http_reply", &s_http_agent_format_http_reply_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        http_response_format_reply (tcb->response);
    
}
#undef  tcb
    return _rc;
}

static int
action_get_next_dynamic_bucket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_GET_NEXT_DYNAMIC_BUCKET))
    icl_stats_inc ("http_agent_get_next_dynamic_bucket", &s_http_agent_get_next_dynamic_bucket_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_list_pop (tcb->response->bucket_list);
        if (tcb->bucket)
            the_next_event = ok_event;
        else
            the_next_event = finished_event;
    
}
#undef  tcb
    return _rc;
}

static int
action_pause_client_for_abort (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PAUSE_CLIENT_FOR_ABORT))
    icl_stats_inc ("http_agent_pause_client_for_abort", &s_http_agent_pause_client_for_abort_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  We wait for a while, then kill the connection.  This slows down a
        //  client that we perceive to be hostile
        smt_timer_request_delay (thread,
            http_config_abort_delay (http_config) * 1000 * 1000, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_flush_the_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_FLUSH_THE_CONNECTION))
    icl_stats_inc ("http_agent_flush_the_connection", &s_http_agent_flush_the_connection_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        if (tcb->socket)
            smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_do_access_handlers_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_CLOSE))
    icl_stats_inc ("http_agent_do_access_handlers_close", &s_http_agent_do_access_handlers_close_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_looseref_t
            *looseref;                      //  Access module reference

        //  Do close handler only if open was done (that allocates the context)
        if (tcb->access_context) {
            looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
            while (looseref) {
                http_access_module_request_close (
                    (http_access_module_t *) (looseref->object), tcb->access_context);
                looseref = ipr_looseref_list_next (&looseref);
            }
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_send_bucket_to_client (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_SEND_BUCKET_TO_CLIENT))
    icl_stats_inc ("http_agent_send_bucket_to_client", &s_http_agent_send_bucket_to_client_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        smt_socket_request_write_bucket (
            thread,
            tcb->socket,
            tcb->write_timeout,
            tcb->bucket,
            bucket_sent_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_open_static_content (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_OPEN_STATIC_CONTENT))
    icl_stats_inc ("http_agent_open_static_content", &s_http_agent_open_static_content_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        if (tcb->response->filename) {
            tcb->response_stream = fopen (tcb->response->filename, "rb");
            if (!tcb->response_stream) {
                smt_log_print (tcb->alert_log,
                    "E: cannot access '%s'", tcb->response->filename);
                http_response_set_error (tcb->response, HTTP_REPLY_INTERNALERROR,
                    "Cannot access static resource content");
                smt_thread_raise_exception (thread, http_error_event);
            }
        }
        else
            smt_thread_raise_exception (thread, empty_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_get_next_static_bucket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_GET_NEXT_STATIC_BUCKET))
    icl_stats_inc ("http_agent_get_next_static_bucket", &s_http_agent_get_next_static_bucket_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_new (128000);
        ipr_bucket_load (tcb->bucket, tcb->response_stream);
        if (tcb->bucket->cur_size)
            the_next_event = ok_event;
        else
            the_next_event = finished_event;
    
}
#undef  tcb
    return _rc;
}

static int
action_do_access_handlers_after (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_DO_ACCESS_HANDLERS_AFTER))
    icl_stats_inc ("http_agent_do_access_handlers_after", &s_http_agent_do_access_handlers_after_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc;

        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_after (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_check_if_persistent_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CHECK_IF_PERSISTENT_CONNECTION))
    icl_stats_inc ("http_agent_check_if_persistent_connection", &s_http_agent_check_if_persistent_connection_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        ipr_meter_count (tcb->server->ometer);
        icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->responses));
        if (tcb->request->persistent)
            the_next_event = persistent_event;
        else
            the_next_event = finished_event;
    
}
#undef  tcb
    return _rc;
}

static int
action_close_static_content (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_CLOSE_STATIC_CONTENT))
    icl_stats_inc ("http_agent_close_static_content", &s_http_agent_close_static_content_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        fclose (tcb->response_stream);
    
}
#undef  tcb
    return _rc;
}

static int
action_pause_client_for_retry (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_PAUSE_CLIENT_FOR_RETRY))
    icl_stats_inc ("http_agent_pause_client_for_retry", &s_http_agent_pause_client_for_retry_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

        //  We wait for a second, then retry access control. This gives time for
        //  whatever is happening in the background to refresh our password data.
        //  The access module is responsible for checking whether the background
        //  process is finished safely, and ending the retry loop
        smt_timer_request_delay (thread, 1 * 1000 * 1000, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_31 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_31))
    icl_stats_inc ("http_agent_unnamed_31", &s_http_agent_unnamed_31_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_32 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_32))
    icl_stats_inc ("http_agent_unnamed_32", &s_http_agent_unnamed_32_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_33 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_33))
    icl_stats_inc ("http_agent_unnamed_33", &s_http_agent_unnamed_33_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_34 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_34))
    icl_stats_inc ("http_agent_unnamed_34", &s_http_agent_unnamed_34_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_37 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_37))
    icl_stats_inc ("http_agent_unnamed_37", &s_http_agent_unnamed_37_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_38 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_HTTP_AGENT)  ||  defined (BASE_STATS_HTTP_AGENT_UNNAMED_38))
    icl_stats_inc ("http_agent_unnamed_38", &s_http_agent_unnamed_38_stats);
#endif        
#define tcb ((http_agent_connection_context_t *) thread->context)
{

            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            
}
#undef  tcb
    return _rc;
}

