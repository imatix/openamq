/*---------------------------------------------------------------------------
    demo_client_agent.c - functions for demo_client_agent.

    Generated from demo_client_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "demo_client_agent.h"

#define connection_initialise_connection_state     1
#define connection_negotiate_connection_state      2
#define connection_connection_active_state         3
#define connection_send_content_body_if_any_state  4
#define connection_close_the_connection_state      5

#define input_start_state                          6
#define input_read_method_state                    7
#define input_connection_closing_state             8
#define input_have_incoming_method_state           9
#define input_have_connection_method_state         10
#define input_have_channel_method_state            11
#define input_read_basic_content_state             12

#define direct_direct_initialise_state             13
#define direct_direct_negotiate_state              14
#define direct_direct_connection_state             15

#define heartbeat_monitor_heartbeat_state          16

#define channel_open_m ((demo_client_agent_channel_open_t *) thread->method->data)

#define channel_close_m ((demo_client_agent_channel_close_t *) thread->method->data)

#define channel_flow_m ((demo_client_agent_channel_flow_t *) thread->method->data)

#define channel_flow_ok_m ((demo_client_agent_channel_flow_ok_t *) thread->method->data)

#define exchange_declare_m ((demo_client_agent_exchange_declare_t *) thread->method->data)

#define exchange_delete_m ((demo_client_agent_exchange_delete_t *) thread->method->data)

#define queue_declare_m ((demo_client_agent_queue_declare_t *) thread->method->data)

#define queue_bind_m ((demo_client_agent_queue_bind_t *) thread->method->data)

#define queue_purge_m ((demo_client_agent_queue_purge_t *) thread->method->data)

#define queue_delete_m ((demo_client_agent_queue_delete_t *) thread->method->data)

#define basic_consume_m ((demo_client_agent_basic_consume_t *) thread->method->data)

#define basic_cancel_m ((demo_client_agent_basic_cancel_t *) thread->method->data)

#define basic_publish_m ((demo_client_agent_basic_publish_t *) thread->method->data)

#define basic_get_m ((demo_client_agent_basic_get_t *) thread->method->data)

#define direct_put_m ((demo_client_agent_direct_put_t *) thread->method->data)

#define direct_get_m ((demo_client_agent_direct_get_t *) thread->method->data)

#define push_m ((demo_client_agent_push_t *) thread->method->data)

#define direct_out_m ((demo_client_agent_direct_out_t *) thread->method->data)

enum {
    connect_event                                  = 1,
    ok_event                                       = 2,
    version_mismatch_event                         = 3,
    bad_protocol_event                             = 4,
    setup_timeout_event                            = 5,
    interrupt_event                                = 6,
    connection_tune_m_event                        = 7,
    connection_open_ok_m_event                     = 8,
    discard_method_event                           = 9,
    channel_flow_m_event                           = 10,
    channel_flow_ok_m_event                        = 11,
    exchange_declare_m_event                       = 12,
    exchange_delete_m_event                        = 13,
    queue_declare_m_event                          = 14,
    queue_bind_m_event                             = 15,
    queue_purge_m_event                            = 16,
    queue_delete_m_event                           = 17,
    basic_consume_m_event                          = 18,
    basic_cancel_m_event                           = 19,
    basic_publish_m_event                          = 20,
    start_event                                    = 21,
    continue_event                                 = 22,
    finished_event                                 = 23,
    socket_error_event                             = 24,
    socket_timeout_event                           = 25,
    socket_closing_event                           = 26,
    smt_error_event                                = 27,
    connection_close_event                         = 28,
    active_close_event                             = 29,
    passive_close_event                            = 30,
    close_timeout_event                            = 31,
    connection_error_event                         = 32,
    connection_close_ok_m_event                    = 33,
    connection_abort_event                         = 34,
    external_exception_event                       = 35,
    heartbeat_event                                = 36,
    shutdown_event                                 = 37,
    basic_get_m_event                              = 38,
    direct_put_m_event                             = 39,
    direct_get_m_event                             = 40,
    connection_close_m_event                       = 41,
    channel_open_m_event                           = 42,
    channel_close_m_event                          = 43,
    send_channel_close_ok_m_event                  = 44,
    push_m_event                                   = 45,
    shutdown_m_event                               = 46,
    read_method_event                              = 47,
    closing_event                                  = 48,
    method_event                                   = 49,
    discard_frame_event                            = 50,
    connection_close_ok_event                      = 51,
    connection_class_event                         = 52,
    connection_tune_event                          = 53,
    connection_open_ok_event                       = 54,
    channel_class_event                            = 55,
    channel_open_ok_event                          = 56,
    channel_close_event                            = 57,
    channel_close_ok_event                         = 58,
    channel_flow_event                             = 59,
    channel_flow_ok_event                          = 60,
    basic_content_event                            = 61,
    header_event                                   = 62,
    expect_body_event                              = 63,
    other_method_event                             = 64,
    direct_in_event                                = 65,
    direct_out_m_event                             = 66,
    direct_close_m_event                           = 67,
    direct_error_event                             = 68,
    peer_alive_event                               = 69,
    peer_slowing_event                             = 70,
    peer_stopped_event                             = 71,
};

#define laststate                                  16
#define lastevent                                  71
#define lastplace                                  210

#define demo_client_agent_agent_name               "Demo-Client-Agent"

#define connection_thread_name                     "Connection"
#define input_thread_name                          "Input"
#define direct_thread_name                         "Direct"
#define heartbeat_thread_name                      "Heartbeat"

#define initialise_connection_state_name           "Initialise-Connection"
#define negotiate_connection_state_name            "Negotiate-Connection"
#define connection_active_state_name               "Connection-Active"
#define send_content_body_if_any_state_name        "Send-Content-Body-If-Any"
#define close_the_connection_state_name            "Close-The-Connection"
#define start_state_name                           "Start"
#define read_method_state_name                     "Read-Method"
#define connection_closing_state_name              "Connection-Closing"
#define have_incoming_method_state_name            "Have-Incoming-Method"
#define have_connection_method_state_name          "Have-Connection-Method"
#define have_channel_method_state_name             "Have-Channel-Method"
#define read_basic_content_state_name              "Read-Basic-Content"
#define direct_initialise_state_name               "Direct-Initialise"
#define direct_negotiate_state_name                "Direct-Negotiate"
#define direct_connection_state_name               "Direct-Connection"
#define monitor_heartbeat_state_name               "Monitor-Heartbeat"

#define connect_event_name                         "Connect"
#define ok_event_name                              "Ok"
#define version_mismatch_event_name                "Version-Mismatch"
#define bad_protocol_event_name                    "Bad-Protocol"
#define setup_timeout_event_name                   "Setup-Timeout"
#define interrupt_event_name                       "Interrupt"
#define connection_tune_m_event_name               "Connection-Tune-M"
#define connection_open_ok_m_event_name            "Connection-Open-Ok-M"
#define discard_method_event_name                  "Discard-Method"
#define channel_flow_m_event_name                  "Channel-Flow-M"
#define channel_flow_ok_m_event_name               "Channel-Flow-Ok-M"
#define exchange_declare_m_event_name              "Exchange-Declare-M"
#define exchange_delete_m_event_name               "Exchange-Delete-M"
#define queue_declare_m_event_name                 "Queue-Declare-M"
#define queue_bind_m_event_name                    "Queue-Bind-M"
#define queue_purge_m_event_name                   "Queue-Purge-M"
#define queue_delete_m_event_name                  "Queue-Delete-M"
#define basic_consume_m_event_name                 "Basic-Consume-M"
#define basic_cancel_m_event_name                  "Basic-Cancel-M"
#define basic_publish_m_event_name                 "Basic-Publish-M"
#define start_event_name                           "Start"
#define continue_event_name                        "Continue"
#define finished_event_name                        "Finished"
#define socket_error_event_name                    "Socket-Error"
#define socket_timeout_event_name                  "Socket-Timeout"
#define socket_closing_event_name                  "Socket-Closing"
#define smt_error_event_name                       "Smt-Error"
#define connection_close_event_name                "Connection-Close"
#define active_close_event_name                    "Active-Close"
#define passive_close_event_name                   "Passive-Close"
#define close_timeout_event_name                   "Close-Timeout"
#define connection_error_event_name                "Connection-Error"
#define connection_close_ok_m_event_name           "Connection-Close-Ok-M"
#define connection_abort_event_name                "Connection-Abort"
#define external_exception_event_name              "External-Exception"
#define heartbeat_event_name                       "Heartbeat"
#define shutdown_event_name                        "Shutdown"
#define basic_get_m_event_name                     "Basic-Get-M"
#define direct_put_m_event_name                    "Direct-Put-M"
#define direct_get_m_event_name                    "Direct-Get-M"
#define connection_close_m_event_name              "Connection-Close-M"
#define channel_open_m_event_name                  "Channel-Open-M"
#define channel_close_m_event_name                 "Channel-Close-M"
#define send_channel_close_ok_m_event_name         "Send-Channel-Close-Ok-M"
#define push_m_event_name                          "Push-M"
#define shutdown_m_event_name                      "Shutdown-M"
#define read_method_event_name                     "Read-Method"
#define closing_event_name                         "Closing"
#define method_event_name                          "Method"
#define discard_frame_event_name                   "Discard-Frame"
#define connection_close_ok_event_name             "Connection-Close-Ok"
#define connection_class_event_name                "Connection-Class"
#define connection_tune_event_name                 "Connection-Tune"
#define connection_open_ok_event_name              "Connection-Open-Ok"
#define channel_class_event_name                   "Channel-Class"
#define channel_open_ok_event_name                 "Channel-Open-Ok"
#define channel_close_event_name                   "Channel-Close"
#define channel_close_ok_event_name                "Channel-Close-Ok"
#define channel_flow_event_name                    "Channel-Flow"
#define channel_flow_ok_event_name                 "Channel-Flow-Ok"
#define basic_content_event_name                   "Basic-Content"
#define header_event_name                          "Header"
#define expect_body_event_name                     "Expect-Body"
#define other_method_event_name                    "Other-Method"
#define direct_in_event_name                       "Direct-In"
#define direct_out_m_event_name                    "Direct-Out-M"
#define direct_close_m_event_name                  "Direct-Close-M"
#define direct_error_event_name                    "Direct-Error"
#define peer_alive_event_name                      "Peer-Alive"
#define peer_slowing_event_name                    "Peer-Slowing"
#define peer_stopped_event_name                    "Peer-Stopped"

#define set_no_connection_error_action_name        "Set-No-Connection-Error"
#define start_setup_timer_action_name              "Start-Setup-Timer"
#define connect_to_server_action_name              "Connect-To-Server"
#define send_protocol_header_action_name           "Send-Protocol-Header"
#define read_frame_header_action_name              "Read-Frame-Header"
#define check_protocol_header_action_name          "Check-Protocol-Header"
#define read_protocol_version_action_name          "Read-Protocol-Version"
#define report_version_mismatch_action_name        "Report-Version-Mismatch"
#define report_bad_protocol_action_name            "Report-Bad-Protocol"
#define execute_connection_start_action_name       "Execute-Connection-Start"
#define start_input_thread_action_name             "Start-Input-Thread"
#define send_connection_start_ok_action_name       "Send-Connection-Start-Ok"
#define send_connection_tune_ok_action_name        "Send-Connection-Tune-Ok"
#define start_heartbeat_thread_action_name         "Start-Heartbeat-Thread"
#define start_heartbeat_timer_action_name          "Start-Heartbeat-Timer"
#define cancel_setup_timer_action_name             "Cancel-Setup-Timer"
#define send_connection_open_action_name           "Send-Connection-Open"
#define tell_session_connection_almost_action_name  "Tell-Session-Connection-Almost"
#define tell_session_connection_ready_action_name  "Tell-Session-Connection-Ready"
#define send_channel_flow_action_name              "Send-Channel-Flow"
#define send_channel_flow_ok_action_name           "Send-Channel-Flow-Ok"
#define send_exchange_declare_action_name          "Send-Exchange-Declare"
#define send_exchange_delete_action_name           "Send-Exchange-Delete"
#define send_queue_declare_action_name             "Send-Queue-Declare"
#define send_queue_bind_action_name                "Send-Queue-Bind"
#define send_queue_purge_action_name               "Send-Queue-Purge"
#define send_queue_delete_action_name              "Send-Queue-Delete"
#define send_basic_consume_action_name             "Send-Basic-Consume"
#define send_basic_cancel_action_name              "Send-Basic-Cancel"
#define send_basic_publish_action_name             "Send-Basic-Publish"
#define send_basic_get_action_name                 "Send-Basic-Get"
#define send_direct_put_action_name                "Send-Direct-Put"
#define send_direct_get_action_name                "Send-Direct-Get"
#define send_channel_open_action_name              "Send-Channel-Open"
#define send_channel_close_action_name             "Send-Channel-Close"
#define send_channel_close_ok_action_name          "Send-Channel-Close-Ok"
#define push_method_to_server_action_name          "Push-Method-To-Server"
#define send_content_header_action_name            "Send-Content-Header"
#define send_content_body_frame_action_name        "Send-Content-Body-Frame"
#define shut_down_slave_threads_action_name        "Shut-Down-Slave-Threads"
#define report_smt_error_action_name               "Report-Smt-Error"
#define send_connection_close_action_name          "Send-Connection-Close"
#define execute_connection_close_action_name       "Execute-Connection-Close"
#define send_connection_close_ok_action_name       "Send-Connection-Close-Ok"
#define flush_the_connection_action_name           "Flush-The-Connection"
#define report_socket_error_action_name            "Report-Socket-Error"
#define report_connect_error_action_name           "Report-Connect-Error"
#define report_socket_timeout_action_name          "Report-Socket-Timeout"
#define report_connect_timeout_action_name         "Report-Connect-Timeout"
#define send_heartbeat_frame_action_name           "Send-Heartbeat-Frame"
#define read_frame_payload_action_name             "Read-Frame-Payload"
#define check_frame_format_action_name             "Check-Frame-Format"
#define decode_method_action_name                  "Decode-Method"
#define check_method_class_action_name             "Check-Method-Class"
#define check_connection_method_action_name        "Check-Connection-Method"
#define check_channel_method_action_name           "Check-Channel-Method"
#define execute_functional_method_action_name      "Execute-Functional-Method"
#define execute_connection_tune_action_name        "Execute-Connection-Tune"
#define notify_connection_of_tune_action_name      "Notify-Connection-Of-Tune"
#define execute_connection_open_ok_action_name     "Execute-Connection-Open-Ok"
#define notify_connection_of_open_ok_action_name   "Notify-Connection-Of-Open-Ok"
#define notify_connection_of_passive_close_action_name  "Notify-Connection-Of-Passive-Close"
#define notify_connection_of_close_ok_action_name  "Notify-Connection-Of-Close-Ok"
#define execute_channel_open_ok_action_name        "Execute-Channel-Open-Ok"
#define execute_channel_close_action_name          "Execute-Channel-Close"
#define execute_channel_close_ok_action_name       "Execute-Channel-Close-Ok"
#define execute_channel_flow_action_name           "Execute-Channel-Flow"
#define execute_channel_flow_ok_action_name        "Execute-Channel-Flow-Ok"
#define expect_content_basic_header_action_name    "Expect-Content-Basic-Header"
#define check_if_basic_body_complete_action_name   "Check-If-Basic-Body-Complete"
#define expect_content_basic_body_action_name      "Expect-Content-Basic-Body"
#define expect_exception_method_action_name        "Expect-Exception-Method"
#define notify_connection_of_exception_action_name  "Notify-Connection-Of-Exception"
#define notify_connection_of_error_action_name     "Notify-Connection-Of-Error"
#define expect_method_frame_action_name            "Expect-Method-Frame"
#define expect_connection_close_ok_action_name     "Expect-Connection-Close-Ok"
#define direct_connect_to_server_action_name       "Direct-Connect-To-Server"
#define send_direct_protocol_header_action_name    "Send-Direct-Protocol-Header"
#define read_direct_response_action_name           "Read-Direct-Response"
#define check_direct_response_action_name          "Check-Direct-Response"
#define send_direct_lease_action_name              "Send-Direct-Lease"
#define tell_session_direct_connection_ok_action_name  "Tell-Session-Direct-Connection-Ok"
#define tell_session_direct_connection_failed_action_name  "Tell-Session-Direct-Connection-Failed"
#define prepare_direct_connection_action_name      "Prepare-Direct-Connection"
#define deliver_direct_contents_action_name        "Deliver-Direct-Contents"
#define write_direct_content_action_name           "Write-Direct-Content"
#define tell_session_direct_closed_action_name     "Tell-Session-Direct-Closed"
#define flush_direct_connection_action_name        "Flush-Direct-Connection"
#define report_direct_protocol_error_action_name   "Report-Direct-Protocol-Error"
#define report_direct_connect_error_action_name    "Report-Direct-Connect-Error"
#define report_direct_socket_error_action_name     "Report-Direct-Socket-Error"
#define report_direct_socket_timeout_action_name   "Report-Direct-Socket-Timeout"
#define report_direct_connect_timeout_action_name  "Report-Direct-Connect-Timeout"
#define wait_for_heartbeat_interval_action_name    "Wait-For-Heartbeat-Interval"
#define check_peer_liveness_action_name            "Check-Peer-Liveness"
#define report_peer_slowing_action_name            "Report-Peer-Slowing"
#define shut_down_agent_action_name                "Shut-Down-Agent"
#define report_peer_stopped_action_name            "Report-Peer-Stopped"
#define unnamed_19_action_name                     "Unnamed-19"
#define dialog_call_send_content_body_if_any_start_action_name  "Dialog-Call-Send-Content-Body-If-Any-Start"
#define dialog_return_action_name                  "Dialog-Return"
#define dialog_call_close_the_connection_passive_close_action_name  "Dialog-Call-Close-The-Connection-Passive-Close"
#define unnamed_49_action_name                     "Unnamed-49"
#define unnamed_50_action_name                     "Unnamed-50"
#define unnamed_51_action_name                     "Unnamed-51"
#define dialog_call_close_the_connection_active_close_action_name  "Dialog-Call-Close-The-Connection-Active-Close"
#define unnamed_61_action_name                     "Unnamed-61"
#define unnamed_62_action_name                     "Unnamed-62"
#define unnamed_64_action_name                     "Unnamed-64"
#define dialog_call_read_method_read_method_action_name  "Dialog-Call-Read-Method-Read-Method"
#define dialog_call_connection_closing_closing_action_name  "Dialog-Call-Connection-Closing-Closing"
#define dialog_call_read_basic_content_header_action_name  "Dialog-Call-Read-Basic-Content-Header"
#define unnamed_113_action_name                    "Unnamed-113"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    smt_method_queue_t *  method_queue;
    Bool *         alive;
    dbyte *        reply_code;
    char *         reply_text;
    dbyte          channel_nbr;
  }
demo_client_agent_channel_open_t;

typedef struct
  {
    dbyte          channel_nbr;
  }
demo_client_agent_channel_close_t;

typedef struct
  {
    dbyte          channel_nbr;
    Bool           active;
  }
demo_client_agent_channel_flow_t;

typedef struct
  {
    dbyte          channel_nbr;
    Bool           active;
  }
demo_client_agent_channel_flow_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         exchange;
    char *         type;
    Bool           passive;
    Bool           durable;
    Bool           auto_delete;
  }
demo_client_agent_exchange_declare_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         exchange;
    Bool           if_unused;
  }
demo_client_agent_exchange_delete_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
    Bool           passive;
    Bool           durable;
    Bool           exclusive;
    Bool           auto_delete;
  }
demo_client_agent_queue_declare_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
    char *         exchange;
    icl_longstr_t *  arguments;
  }
demo_client_agent_queue_bind_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
  }
demo_client_agent_queue_purge_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
    Bool           if_unused;
    Bool           if_empty;
  }
demo_client_agent_queue_delete_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
  }
demo_client_agent_basic_consume_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         consumer_tag;
  }
demo_client_agent_basic_cancel_t;

typedef struct
  {
    dbyte          channel_nbr;
    demo_content_basic_t *  content;
    char *         exchange;
    char *         routing_key;
  }
demo_client_agent_basic_publish_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
  }
demo_client_agent_basic_get_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         sink;
  }
demo_client_agent_direct_put_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         feed;
  }
demo_client_agent_direct_get_t;

typedef struct
  {
    dbyte          channel_nbr;
    demo_client_method_t *  method;
  }
demo_client_agent_push_t;

typedef struct
  {
    demo_content_basic_t *  content;
    byte           options;
  }
demo_client_agent_direct_out_t;


#define BASE_ANIMATE            1
#define FULL_LIVENESS           4       //  Four missing heartbeats -> dead

//  Channel states
#define ASL_CHANNEL_CLOSED      0
#define ASL_CHANNEL_OPEN        1
#define ASL_CHANNEL_CLOSING     2
#define ASL_CHANNEL_OPENING     3

#include "version.h"


#define METHOD_HISTORY_SIZE 255
static struct {
    char inout;
    char *name;
} s_history_table [METHOD_HISTORY_SIZE + 1];

static int
    s_history_index = 0;

int demo_postmortem (void) {
    int
        count;

    //  This function is not defined in a header; the application must define
    //  it before calling it.
    icl_console_print ("WireAPI postmortem dump:");
    for (count = 0; count < METHOD_HISTORY_SIZE; count++) {
        if (s_history_table [s_history_index].name)
            icl_console_print ("%s: %s",
                s_history_table [s_history_index].inout == 'i'? "recv": "send",
                s_history_table [s_history_index].name);
        s_history_index = (s_history_index + 1) % METHOD_HISTORY_SIZE;
    }
    return (0);
}


//  Function prototypes

//  Raises a connection exception
static void
s_connection_exception (
    smt_thread_t *thread,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Aborts the connection immediately
static void
s_connection_abort (
    smt_thread_t *thread,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Helper function for reporting errors
static void
s_report_error (
    smt_thread_t *thread,
    char *context,
    char *action_taken,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Executes an incoming method
static inline void
s_execute_method (
    smt_thread_t *thread,
    Bool channel_method,
    int class_id,
    int method_id);

//  Executes an incoming functional method
static void
s_execute_functional_method (
    smt_thread_t *thread,
    dbyte class_id,
    dbyte method_id,
    dbyte out_class_id,
    dbyte out_method_id);

//  Destructor for reply methods
static int
s_method_destructor (void *method);

//  Writes a non-null payload to the socket
static void
s_write_payload (
    smt_thread_t *thread,
    dbyte channel,
    byte type,
    ipr_bucket_t **bucket_p);

//  Writes an outgoing method to the socket
static void
s_send_method_out (
    smt_thread_t *thread,
    dbyte channel,
    smt_log_t *log);

//  Helper functions for socket input/output
static inline void
s_socket_write (
    smt_thread_t *thread,
    byte *buffer,
    size_t size);

static inline void
s_socket_write_bucket (
    smt_thread_t *thread,
    ipr_bucket_t *bucket);

static void inline
s_socket_write_shortstr (
    smt_thread_t *thread,
    char *string);

static inline void
s_socket_read (
    smt_thread_t *thread,
    byte *buffer,
    size_t size);

static inline void
s_socket_read_bucket (
    smt_thread_t *thread,
    ipr_bucket_t **bucket_p,
    size_t size);

//  Table of predecessor methods for each method.  If >0, then when we get a
//  specific method we can assert that we previously sent the predecessor on
//  the same channel.
static dbyte s_class_index [6 + 1] = {
    10, 20, 40, 50, 60, 61500, 0
};
static dbyte s_method_index [6][11] = {
    {  10, 11, 20, 21, 30, 31, 40, 41, 50, 51,  0 },
    {  10, 11, 20, 21, 40, 41,  0,  0,  0,  0,  0 },
    {  10, 11, 20, 21,  0,  0,  0,  0,  0,  0,  0 },
    {  10, 11, 20, 21, 30, 31, 40, 41,  0,  0,  0 },
    {  10, 11, 20, 21, 30, 40, 50, 51, 52,  0,  0 },
    {  10, 11, 20, 21,  0,  0,  0,  0,  0,  0,  0 }
};
static dbyte s_predecessor [6][11] = {
    {   0, 10,  0, 20,  0, 30,  0, 40,  0, 50,  0 },
    {   0, 10,  0, 20,  0, 40,  0,  0,  0,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0,  0,  0,  0,  0 },
    {   0, 10,  0, 20,  0, 30,  0, 40,  0,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0, 50, 50,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0,  0,  0,  0,  0 }
};

static icl_cache_t
    *s_connection_context_cache = NULL;

typedef struct {
    int
        links;

    smt_thread_t
        *connection_thread,             //  Master thread
        *input_thread,                  //  Socket input thread
        *heartbeat_thread;              //  Heartbeat monitor thread
    smt_socket_t
        *socket;                        //  Socket for i/o
    demo_client_channel_t
        *channel;                       //  Channel object
    demo_client_channel_table_t
        *channels;                      //  Active channels
    int
        channel_state;                  //  Internal channel state
    demo_client_method_t
        *method_in,                     //  Last received method
        *method_out;                    //  Method currently being sent
    asl_reader_t
        reader;                         //  Body reader
    ipr_bucket_t
        *bucket_in;                     //  Last received bucket
    byte
        protocol_header [8];            //  Used to initiate connection
    asl_frame_header_t
        frame_header;                   //  Header for frame i/o
    dbyte
        reply_code;                     //  Exception error code
    icl_shortstr_t
        reply_text;                     //  Exception error text
    icl_shortstr_t
        strerror;                       //  Reply text for method errors
    dbyte
        out_class_id,                   //  Last class we sent
        out_method_id;                  //  Last method we sent
    qbyte
        frame_max,                      //  Negotiated maximum frame size
        heartbeat;                      //  Connection heartbeat
    int
        liveness,                       //  If this reaches zero, peer is dead
        trace;                          //  Trace level

    //  Information used for client connection
    icl_shortstr_t
        host;                           //  Server host:port to connect to
    icl_shortstr_t
        virtual_host;                   //  Virtual host to work with
    icl_longstr_t
        *auth_data;                     //  Authentication data
    icl_shortstr_t
        instance;                       //  Name of client instance
    int
        timeout;                        //  Connection setup/close timeout
    smt_timer_request_t
        *setup_timer;                   //  Connection setup timer
    Bool
        verbose,                        //  Verbose error reporting?
        fakeslow;                       //  Simulate a slow network?
    smt_method_queue_t
        *method_queue;                  //  Connection reply queue
    dbyte
        *conn_reply_code;               //  Connection reply code
    char
        *conn_reply_text;               //  Connection reply text
    Bool
        *conn_alive;                    //  Connection alive
    Bool
        *conn_interrupt;                //  We received a SIGINT

} demo_client_agent_connection_context_t;

static icl_cache_t
    *s_direct_context_cache = NULL;

typedef struct {
    int
        links;

    int
        type;                           //  DP_SINK or DP_FEED
    smt_socket_t
        *socket;                        //  Socket for i/o
    ipr_bucket_t
        *bucket_in;                     //  Last received bucket
    byte
        protocol_header [8];            //  Used to initiate connection
    icl_shortstr_t
        host;                           //  Server host:port to connect to
    icl_shortstr_t
        lease;                          //  Lease for direct connection
    byte
        rlength;                        //  Response length from socket
    icl_shortstr_t
        response;                       //  Direct protocol response
    smt_method_queue_t
        *method_queue;                  //  Connection reply queue
    Bool
        verbose;                        //  Verbose error reporting?
    smt_timer_request_t
        *setup_timer;                   //  Connection setup timer
    int
        timeout,                        //  Connection setup/close timeout
        trace;                          //  Trace level 0-3
    qbyte
        netsize;                        //  Message header in network order
    size_t
        batching;                       //  Configured batch size
    Bool
        fakeslow;                       //  Simulate a slow network?

} demo_client_agent_direct_context_t;

static icl_cache_t
    *s_channel_open_cache = NULL;
static icl_cache_t
    *s_channel_close_cache = NULL;
static icl_cache_t
    *s_channel_flow_cache = NULL;
static icl_cache_t
    *s_channel_flow_ok_cache = NULL;
static icl_cache_t
    *s_exchange_declare_cache = NULL;
static icl_cache_t
    *s_exchange_delete_cache = NULL;
static icl_cache_t
    *s_queue_declare_cache = NULL;
static icl_cache_t
    *s_queue_bind_cache = NULL;
static icl_cache_t
    *s_queue_purge_cache = NULL;
static icl_cache_t
    *s_queue_delete_cache = NULL;
static icl_cache_t
    *s_basic_consume_cache = NULL;
static icl_cache_t
    *s_basic_cancel_cache = NULL;
static icl_cache_t
    *s_basic_publish_cache = NULL;
static icl_cache_t
    *s_basic_get_cache = NULL;
static icl_cache_t
    *s_direct_put_cache = NULL;
static icl_cache_t
    *s_direct_get_cache = NULL;
static icl_cache_t
    *s_push_cache = NULL;
static icl_cache_t
    *s_direct_out_cache = NULL;

static const char *thread_name [] = {
    "<NULL>",
    connection_thread_name,
    input_thread_name,
    direct_thread_name,
    heartbeat_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    initialise_connection_state_name,
    negotiate_connection_state_name,
    connection_active_state_name,
    send_content_body_if_any_state_name,
    close_the_connection_state_name,
    start_state_name,
    read_method_state_name,
    connection_closing_state_name,
    have_incoming_method_state_name,
    have_connection_method_state_name,
    have_channel_method_state_name,
    read_basic_content_state_name,
    direct_initialise_state_name,
    direct_negotiate_state_name,
    direct_connection_state_name,
    monitor_heartbeat_state_name
};

static const char *event_name [] = {
    "<NULL>",
    connect_event_name,
    ok_event_name,
    version_mismatch_event_name,
    bad_protocol_event_name,
    setup_timeout_event_name,
    interrupt_event_name,
    connection_tune_m_event_name,
    connection_open_ok_m_event_name,
    discard_method_event_name,
    channel_flow_m_event_name,
    channel_flow_ok_m_event_name,
    exchange_declare_m_event_name,
    exchange_delete_m_event_name,
    queue_declare_m_event_name,
    queue_bind_m_event_name,
    queue_purge_m_event_name,
    queue_delete_m_event_name,
    basic_consume_m_event_name,
    basic_cancel_m_event_name,
    basic_publish_m_event_name,
    start_event_name,
    continue_event_name,
    finished_event_name,
    socket_error_event_name,
    socket_timeout_event_name,
    socket_closing_event_name,
    smt_error_event_name,
    connection_close_event_name,
    active_close_event_name,
    passive_close_event_name,
    close_timeout_event_name,
    connection_error_event_name,
    connection_close_ok_m_event_name,
    connection_abort_event_name,
    external_exception_event_name,
    heartbeat_event_name,
    shutdown_event_name,
    basic_get_m_event_name,
    direct_put_m_event_name,
    direct_get_m_event_name,
    connection_close_m_event_name,
    channel_open_m_event_name,
    channel_close_m_event_name,
    send_channel_close_ok_m_event_name,
    push_m_event_name,
    shutdown_m_event_name,
    read_method_event_name,
    closing_event_name,
    method_event_name,
    discard_frame_event_name,
    connection_close_ok_event_name,
    connection_class_event_name,
    connection_tune_event_name,
    connection_open_ok_event_name,
    channel_class_event_name,
    channel_open_ok_event_name,
    channel_close_event_name,
    channel_close_ok_event_name,
    channel_flow_event_name,
    channel_flow_ok_event_name,
    basic_content_event_name,
    header_event_name,
    expect_body_event_name,
    other_method_event_name,
    direct_in_event_name,
    direct_out_m_event_name,
    direct_close_m_event_name,
    direct_error_event_name,
    peer_alive_event_name,
    peer_slowing_event_name,
    peer_stopped_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT)        ||  defined (BASE_TRACE)  || defined (BASE_TRACE_DEMO_CLIENT_AGENT)   ||  defined (DEMO_CLIENT_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    set_no_connection_error_action_name,
    start_setup_timer_action_name,
    connect_to_server_action_name,
    send_protocol_header_action_name,
    read_frame_header_action_name,
    check_protocol_header_action_name,
    read_protocol_version_action_name,
    report_version_mismatch_action_name,
    report_bad_protocol_action_name,
    execute_connection_start_action_name,
    start_input_thread_action_name,
    send_connection_start_ok_action_name,
    send_connection_tune_ok_action_name,
    start_heartbeat_thread_action_name,
    start_heartbeat_timer_action_name,
    cancel_setup_timer_action_name,
    send_connection_open_action_name,
    tell_session_connection_almost_action_name,
    tell_session_connection_ready_action_name,
    send_channel_flow_action_name,
    send_channel_flow_ok_action_name,
    send_exchange_declare_action_name,
    send_exchange_delete_action_name,
    send_queue_declare_action_name,
    send_queue_bind_action_name,
    send_queue_purge_action_name,
    send_queue_delete_action_name,
    send_basic_consume_action_name,
    send_basic_cancel_action_name,
    send_basic_publish_action_name,
    send_basic_get_action_name,
    send_direct_put_action_name,
    send_direct_get_action_name,
    send_channel_open_action_name,
    send_channel_close_action_name,
    send_channel_close_ok_action_name,
    push_method_to_server_action_name,
    send_content_header_action_name,
    send_content_body_frame_action_name,
    shut_down_slave_threads_action_name,
    report_smt_error_action_name,
    send_connection_close_action_name,
    execute_connection_close_action_name,
    send_connection_close_ok_action_name,
    flush_the_connection_action_name,
    report_socket_error_action_name,
    report_connect_error_action_name,
    report_socket_timeout_action_name,
    report_connect_timeout_action_name,
    send_heartbeat_frame_action_name,
    read_frame_payload_action_name,
    check_frame_format_action_name,
    decode_method_action_name,
    check_method_class_action_name,
    check_connection_method_action_name,
    check_channel_method_action_name,
    execute_functional_method_action_name,
    execute_connection_tune_action_name,
    notify_connection_of_tune_action_name,
    execute_connection_open_ok_action_name,
    notify_connection_of_open_ok_action_name,
    notify_connection_of_passive_close_action_name,
    notify_connection_of_close_ok_action_name,
    execute_channel_open_ok_action_name,
    execute_channel_close_action_name,
    execute_channel_close_ok_action_name,
    execute_channel_flow_action_name,
    execute_channel_flow_ok_action_name,
    expect_content_basic_header_action_name,
    check_if_basic_body_complete_action_name,
    expect_content_basic_body_action_name,
    expect_exception_method_action_name,
    notify_connection_of_exception_action_name,
    notify_connection_of_error_action_name,
    expect_method_frame_action_name,
    expect_connection_close_ok_action_name,
    direct_connect_to_server_action_name,
    send_direct_protocol_header_action_name,
    read_direct_response_action_name,
    check_direct_response_action_name,
    send_direct_lease_action_name,
    tell_session_direct_connection_ok_action_name,
    tell_session_direct_connection_failed_action_name,
    prepare_direct_connection_action_name,
    deliver_direct_contents_action_name,
    write_direct_content_action_name,
    tell_session_direct_closed_action_name,
    flush_direct_connection_action_name,
    report_direct_protocol_error_action_name,
    report_direct_connect_error_action_name,
    report_direct_socket_error_action_name,
    report_direct_socket_timeout_action_name,
    report_direct_connect_timeout_action_name,
    wait_for_heartbeat_interval_action_name,
    check_peer_liveness_action_name,
    report_peer_slowing_action_name,
    shut_down_agent_action_name,
    report_peer_stopped_action_name,
    unnamed_19_action_name,
    dialog_call_send_content_body_if_any_start_action_name,
    dialog_return_action_name,
    dialog_call_close_the_connection_passive_close_action_name,
    unnamed_49_action_name,
    unnamed_50_action_name,
    unnamed_51_action_name,
    dialog_call_close_the_connection_active_close_action_name,
    unnamed_61_action_name,
    unnamed_62_action_name,
    unnamed_64_action_name,
    dialog_call_read_method_read_method_action_name,
    dialog_call_connection_closing_closing_action_name,
    dialog_call_read_basic_content_header_action_name,
    unnamed_113_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SET_NO_CONNECTION_ERROR))
static icl_stats_t *s_demo_client_agent_set_no_connection_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_SETUP_TIMER))
static icl_stats_t *s_demo_client_agent_start_setup_timer_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CONNECT_TO_SERVER))
static icl_stats_t *s_demo_client_agent_connect_to_server_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_PROTOCOL_HEADER))
static icl_stats_t *s_demo_client_agent_send_protocol_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_FRAME_HEADER))
static icl_stats_t *s_demo_client_agent_read_frame_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_PROTOCOL_HEADER))
static icl_stats_t *s_demo_client_agent_check_protocol_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_PROTOCOL_VERSION))
static icl_stats_t *s_demo_client_agent_read_protocol_version_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_VERSION_MISMATCH))
static icl_stats_t *s_demo_client_agent_report_version_mismatch_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_BAD_PROTOCOL))
static icl_stats_t *s_demo_client_agent_report_bad_protocol_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_START))
static icl_stats_t *s_demo_client_agent_execute_connection_start_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_INPUT_THREAD))
static icl_stats_t *s_demo_client_agent_start_input_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_START_OK))
static icl_stats_t *s_demo_client_agent_send_connection_start_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_TUNE_OK))
static icl_stats_t *s_demo_client_agent_send_connection_tune_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_HEARTBEAT_THREAD))
static icl_stats_t *s_demo_client_agent_start_heartbeat_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_HEARTBEAT_TIMER))
static icl_stats_t *s_demo_client_agent_start_heartbeat_timer_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CANCEL_SETUP_TIMER))
static icl_stats_t *s_demo_client_agent_cancel_setup_timer_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_OPEN))
static icl_stats_t *s_demo_client_agent_send_connection_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_CONNECTION_ALMOST))
static icl_stats_t *s_demo_client_agent_tell_session_connection_almost_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_CONNECTION_READY))
static icl_stats_t *s_demo_client_agent_tell_session_connection_ready_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_FLOW))
static icl_stats_t *s_demo_client_agent_send_channel_flow_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_FLOW_OK))
static icl_stats_t *s_demo_client_agent_send_channel_flow_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_EXCHANGE_DECLARE))
static icl_stats_t *s_demo_client_agent_send_exchange_declare_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_EXCHANGE_DELETE))
static icl_stats_t *s_demo_client_agent_send_exchange_delete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_DECLARE))
static icl_stats_t *s_demo_client_agent_send_queue_declare_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_BIND))
static icl_stats_t *s_demo_client_agent_send_queue_bind_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_PURGE))
static icl_stats_t *s_demo_client_agent_send_queue_purge_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_DELETE))
static icl_stats_t *s_demo_client_agent_send_queue_delete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_CONSUME))
static icl_stats_t *s_demo_client_agent_send_basic_consume_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_CANCEL))
static icl_stats_t *s_demo_client_agent_send_basic_cancel_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_PUBLISH))
static icl_stats_t *s_demo_client_agent_send_basic_publish_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_GET))
static icl_stats_t *s_demo_client_agent_send_basic_get_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_PUT))
static icl_stats_t *s_demo_client_agent_send_direct_put_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_GET))
static icl_stats_t *s_demo_client_agent_send_direct_get_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_OPEN))
static icl_stats_t *s_demo_client_agent_send_channel_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_CLOSE))
static icl_stats_t *s_demo_client_agent_send_channel_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_CLOSE_OK))
static icl_stats_t *s_demo_client_agent_send_channel_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_PUSH_METHOD_TO_SERVER))
static icl_stats_t *s_demo_client_agent_push_method_to_server_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONTENT_HEADER))
static icl_stats_t *s_demo_client_agent_send_content_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONTENT_BODY_FRAME))
static icl_stats_t *s_demo_client_agent_send_content_body_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SHUT_DOWN_SLAVE_THREADS))
static icl_stats_t *s_demo_client_agent_shut_down_slave_threads_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SMT_ERROR))
static icl_stats_t *s_demo_client_agent_report_smt_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_CLOSE))
static icl_stats_t *s_demo_client_agent_send_connection_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_CLOSE))
static icl_stats_t *s_demo_client_agent_execute_connection_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_CLOSE_OK))
static icl_stats_t *s_demo_client_agent_send_connection_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_FLUSH_THE_CONNECTION))
static icl_stats_t *s_demo_client_agent_flush_the_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SOCKET_ERROR))
static icl_stats_t *s_demo_client_agent_report_socket_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_CONNECT_ERROR))
static icl_stats_t *s_demo_client_agent_report_connect_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SOCKET_TIMEOUT))
static icl_stats_t *s_demo_client_agent_report_socket_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_CONNECT_TIMEOUT))
static icl_stats_t *s_demo_client_agent_report_connect_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_HEARTBEAT_FRAME))
static icl_stats_t *s_demo_client_agent_send_heartbeat_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_FRAME_PAYLOAD))
static icl_stats_t *s_demo_client_agent_read_frame_payload_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_FRAME_FORMAT))
static icl_stats_t *s_demo_client_agent_check_frame_format_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DECODE_METHOD))
static icl_stats_t *s_demo_client_agent_decode_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_METHOD_CLASS))
static icl_stats_t *s_demo_client_agent_check_method_class_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_CONNECTION_METHOD))
static icl_stats_t *s_demo_client_agent_check_connection_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_CHANNEL_METHOD))
static icl_stats_t *s_demo_client_agent_check_channel_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_FUNCTIONAL_METHOD))
static icl_stats_t *s_demo_client_agent_execute_functional_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_TUNE))
static icl_stats_t *s_demo_client_agent_execute_connection_tune_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_TUNE))
static icl_stats_t *s_demo_client_agent_notify_connection_of_tune_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_OPEN_OK))
static icl_stats_t *s_demo_client_agent_execute_connection_open_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_OPEN_OK))
static icl_stats_t *s_demo_client_agent_notify_connection_of_open_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_PASSIVE_CLOSE))
static icl_stats_t *s_demo_client_agent_notify_connection_of_passive_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_CLOSE_OK))
static icl_stats_t *s_demo_client_agent_notify_connection_of_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_OPEN_OK))
static icl_stats_t *s_demo_client_agent_execute_channel_open_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_CLOSE))
static icl_stats_t *s_demo_client_agent_execute_channel_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_CLOSE_OK))
static icl_stats_t *s_demo_client_agent_execute_channel_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_FLOW))
static icl_stats_t *s_demo_client_agent_execute_channel_flow_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_FLOW_OK))
static icl_stats_t *s_demo_client_agent_execute_channel_flow_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONTENT_BASIC_HEADER))
static icl_stats_t *s_demo_client_agent_expect_content_basic_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_IF_BASIC_BODY_COMPLETE))
static icl_stats_t *s_demo_client_agent_check_if_basic_body_complete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONTENT_BASIC_BODY))
static icl_stats_t *s_demo_client_agent_expect_content_basic_body_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_EXCEPTION_METHOD))
static icl_stats_t *s_demo_client_agent_expect_exception_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_EXCEPTION))
static icl_stats_t *s_demo_client_agent_notify_connection_of_exception_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_ERROR))
static icl_stats_t *s_demo_client_agent_notify_connection_of_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_METHOD_FRAME))
static icl_stats_t *s_demo_client_agent_expect_method_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONNECTION_CLOSE_OK))
static icl_stats_t *s_demo_client_agent_expect_connection_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIRECT_CONNECT_TO_SERVER))
static icl_stats_t *s_demo_client_agent_direct_connect_to_server_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_PROTOCOL_HEADER))
static icl_stats_t *s_demo_client_agent_send_direct_protocol_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_DIRECT_RESPONSE))
static icl_stats_t *s_demo_client_agent_read_direct_response_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_DIRECT_RESPONSE))
static icl_stats_t *s_demo_client_agent_check_direct_response_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_LEASE))
static icl_stats_t *s_demo_client_agent_send_direct_lease_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CONNECTION_OK))
static icl_stats_t *s_demo_client_agent_tell_session_direct_connection_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CONNECTION_FAILED))
static icl_stats_t *s_demo_client_agent_tell_session_direct_connection_failed_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_PREPARE_DIRECT_CONNECTION))
static icl_stats_t *s_demo_client_agent_prepare_direct_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DELIVER_DIRECT_CONTENTS))
static icl_stats_t *s_demo_client_agent_deliver_direct_contents_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_WRITE_DIRECT_CONTENT))
static icl_stats_t *s_demo_client_agent_write_direct_content_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CLOSED))
static icl_stats_t *s_demo_client_agent_tell_session_direct_closed_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_FLUSH_DIRECT_CONNECTION))
static icl_stats_t *s_demo_client_agent_flush_direct_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_PROTOCOL_ERROR))
static icl_stats_t *s_demo_client_agent_report_direct_protocol_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_CONNECT_ERROR))
static icl_stats_t *s_demo_client_agent_report_direct_connect_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_SOCKET_ERROR))
static icl_stats_t *s_demo_client_agent_report_direct_socket_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_SOCKET_TIMEOUT))
static icl_stats_t *s_demo_client_agent_report_direct_socket_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_CONNECT_TIMEOUT))
static icl_stats_t *s_demo_client_agent_report_direct_connect_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_WAIT_FOR_HEARTBEAT_INTERVAL))
static icl_stats_t *s_demo_client_agent_wait_for_heartbeat_interval_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_PEER_LIVENESS))
static icl_stats_t *s_demo_client_agent_check_peer_liveness_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_PEER_SLOWING))
static icl_stats_t *s_demo_client_agent_report_peer_slowing_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SHUT_DOWN_AGENT))
static icl_stats_t *s_demo_client_agent_shut_down_agent_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_PEER_STOPPED))
static icl_stats_t *s_demo_client_agent_report_peer_stopped_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_19))
static icl_stats_t *s_demo_client_agent_unnamed_19_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_SEND_CONTENT_BODY_IF_ANY_START))
static icl_stats_t *s_demo_client_agent_dialog_call_send_content_body_if_any_start_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_RETURN))
static icl_stats_t *s_demo_client_agent_dialog_return_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_PASSIVE_CLOSE))
static icl_stats_t *s_demo_client_agent_dialog_call_close_the_connection_passive_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_49))
static icl_stats_t *s_demo_client_agent_unnamed_49_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_50))
static icl_stats_t *s_demo_client_agent_unnamed_50_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_51))
static icl_stats_t *s_demo_client_agent_unnamed_51_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_ACTIVE_CLOSE))
static icl_stats_t *s_demo_client_agent_dialog_call_close_the_connection_active_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_61))
static icl_stats_t *s_demo_client_agent_unnamed_61_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_62))
static icl_stats_t *s_demo_client_agent_unnamed_62_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_64))
static icl_stats_t *s_demo_client_agent_unnamed_64_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_READ_METHOD_READ_METHOD))
static icl_stats_t *s_demo_client_agent_dialog_call_read_method_read_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CONNECTION_CLOSING_CLOSING))
static icl_stats_t *s_demo_client_agent_dialog_call_connection_closing_closing_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_READ_BASIC_CONTENT_HEADER))
static icl_stats_t *s_demo_client_agent_dialog_call_read_basic_content_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_113))
static icl_stats_t *s_demo_client_agent_unnamed_113_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    demo_client_agent_manager       (smt_thread_t **thread);
static int    demo_client_agent_catcher       (smt_thread_t *thread);
static int    demo_client_agent_connection_destroy  (smt_thread_t *thread);
static int    demo_client_agent_input_destroy  (smt_thread_t *thread);
static int    demo_client_agent_direct_destroy  (smt_thread_t *thread);
static int    demo_client_agent_heartbeat_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_CLIENT_AGENT)          ||  defined (DEMO_CLIENT_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_set_no_connection_error  (smt_thread_t *thread);
static int    action_start_setup_timer        (smt_thread_t *thread);
static int    action_connect_to_server        (smt_thread_t *thread);
static int    action_send_protocol_header     (smt_thread_t *thread);
static int    action_read_frame_header        (smt_thread_t *thread);
static int    action_check_protocol_header    (smt_thread_t *thread);
static int    action_read_protocol_version    (smt_thread_t *thread);
static int    action_report_version_mismatch  (smt_thread_t *thread);
static int    action_report_bad_protocol      (smt_thread_t *thread);
static int    action_execute_connection_start  (smt_thread_t *thread);
static int    action_start_input_thread       (smt_thread_t *thread);
static int    action_send_connection_start_ok  (smt_thread_t *thread);
static int    action_send_connection_tune_ok  (smt_thread_t *thread);
static int    action_start_heartbeat_thread   (smt_thread_t *thread);
static int    action_start_heartbeat_timer    (smt_thread_t *thread);
static int    action_cancel_setup_timer       (smt_thread_t *thread);
static int    action_send_connection_open     (smt_thread_t *thread);
static int    action_tell_session_connection_almost  (smt_thread_t *thread);
static int    action_tell_session_connection_ready  (smt_thread_t *thread);
static int    action_send_channel_flow        (smt_thread_t *thread);
static int    action_send_channel_flow_ok     (smt_thread_t *thread);
static int    action_send_exchange_declare    (smt_thread_t *thread);
static int    action_send_exchange_delete     (smt_thread_t *thread);
static int    action_send_queue_declare       (smt_thread_t *thread);
static int    action_send_queue_bind          (smt_thread_t *thread);
static int    action_send_queue_purge         (smt_thread_t *thread);
static int    action_send_queue_delete        (smt_thread_t *thread);
static int    action_send_basic_consume       (smt_thread_t *thread);
static int    action_send_basic_cancel        (smt_thread_t *thread);
static int    action_send_basic_publish       (smt_thread_t *thread);
static int    action_send_basic_get           (smt_thread_t *thread);
static int    action_send_direct_put          (smt_thread_t *thread);
static int    action_send_direct_get          (smt_thread_t *thread);
static int    action_send_channel_open        (smt_thread_t *thread);
static int    action_send_channel_close       (smt_thread_t *thread);
static int    action_send_channel_close_ok    (smt_thread_t *thread);
static int    action_push_method_to_server    (smt_thread_t *thread);
static int    action_send_content_header      (smt_thread_t *thread);
static int    action_send_content_body_frame  (smt_thread_t *thread);
static int    action_shut_down_slave_threads  (smt_thread_t *thread);
static int    action_report_smt_error         (smt_thread_t *thread);
static int    action_send_connection_close    (smt_thread_t *thread);
static int    action_execute_connection_close  (smt_thread_t *thread);
static int    action_send_connection_close_ok  (smt_thread_t *thread);
static int    action_flush_the_connection     (smt_thread_t *thread);
static int    action_report_socket_error      (smt_thread_t *thread);
static int    action_report_connect_error     (smt_thread_t *thread);
static int    action_report_socket_timeout    (smt_thread_t *thread);
static int    action_report_connect_timeout   (smt_thread_t *thread);
static int    action_send_heartbeat_frame     (smt_thread_t *thread);
static int    action_read_frame_payload       (smt_thread_t *thread);
static int    action_check_frame_format       (smt_thread_t *thread);
static int    action_decode_method            (smt_thread_t *thread);
static int    action_check_method_class       (smt_thread_t *thread);
static int    action_check_connection_method  (smt_thread_t *thread);
static int    action_check_channel_method     (smt_thread_t *thread);
static int    action_execute_functional_method  (smt_thread_t *thread);
static int    action_execute_connection_tune  (smt_thread_t *thread);
static int    action_notify_connection_of_tune  (smt_thread_t *thread);
static int    action_execute_connection_open_ok  (smt_thread_t *thread);
static int    action_notify_connection_of_open_ok  (smt_thread_t *thread);
static int    action_notify_connection_of_passive_close  (smt_thread_t *thread);
static int    action_notify_connection_of_close_ok  (smt_thread_t *thread);
static int    action_execute_channel_open_ok  (smt_thread_t *thread);
static int    action_execute_channel_close    (smt_thread_t *thread);
static int    action_execute_channel_close_ok  (smt_thread_t *thread);
static int    action_execute_channel_flow     (smt_thread_t *thread);
static int    action_execute_channel_flow_ok  (smt_thread_t *thread);
static int    action_expect_content_basic_header  (smt_thread_t *thread);
static int    action_check_if_basic_body_complete  (smt_thread_t *thread);
static int    action_expect_content_basic_body  (smt_thread_t *thread);
static int    action_expect_exception_method  (smt_thread_t *thread);
static int    action_notify_connection_of_exception  (smt_thread_t *thread);
static int    action_notify_connection_of_error  (smt_thread_t *thread);
static int    action_expect_method_frame      (smt_thread_t *thread);
static int    action_expect_connection_close_ok  (smt_thread_t *thread);
static int    action_direct_connect_to_server  (smt_thread_t *thread);
static int    action_send_direct_protocol_header  (smt_thread_t *thread);
static int    action_read_direct_response     (smt_thread_t *thread);
static int    action_check_direct_response    (smt_thread_t *thread);
static int    action_send_direct_lease        (smt_thread_t *thread);
static int    action_tell_session_direct_connection_ok  (smt_thread_t *thread);
static int    action_tell_session_direct_connection_failed  (smt_thread_t *thread);
static int    action_prepare_direct_connection  (smt_thread_t *thread);
static int    action_deliver_direct_contents  (smt_thread_t *thread);
static int    action_write_direct_content     (smt_thread_t *thread);
static int    action_tell_session_direct_closed  (smt_thread_t *thread);
static int    action_flush_direct_connection  (smt_thread_t *thread);
static int    action_report_direct_protocol_error  (smt_thread_t *thread);
static int    action_report_direct_connect_error  (smt_thread_t *thread);
static int    action_report_direct_socket_error  (smt_thread_t *thread);
static int    action_report_direct_socket_timeout  (smt_thread_t *thread);
static int    action_report_direct_connect_timeout  (smt_thread_t *thread);
static int    action_wait_for_heartbeat_interval  (smt_thread_t *thread);
static int    action_check_peer_liveness      (smt_thread_t *thread);
static int    action_report_peer_slowing      (smt_thread_t *thread);
static int    action_shut_down_agent          (smt_thread_t *thread);
static int    action_report_peer_stopped      (smt_thread_t *thread);
static int    action_unnamed_19               (smt_thread_t *thread);
static int    action_dialog_call_send_content_body_if_any_start  (smt_thread_t *thread);
static int    action_dialog_return            (smt_thread_t *thread);
static int    action_dialog_call_close_the_connection_passive_close  (smt_thread_t *thread);
static int    action_unnamed_49               (smt_thread_t *thread);
static int    action_unnamed_50               (smt_thread_t *thread);
static int    action_unnamed_51               (smt_thread_t *thread);
static int    action_dialog_call_close_the_connection_active_close  (smt_thread_t *thread);
static int    action_unnamed_61               (smt_thread_t *thread);
static int    action_unnamed_62               (smt_thread_t *thread);
static int    action_unnamed_64               (smt_thread_t *thread);
static int    action_dialog_call_read_method_read_method  (smt_thread_t *thread);
static int    action_dialog_call_connection_closing_closing  (smt_thread_t *thread);
static int    action_dialog_call_read_basic_content_header  (smt_thread_t *thread);
static int    action_unnamed_113              (smt_thread_t *thread);
static int
demo_client_agent_channel_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_method_queue_t *  method_queue,
    Bool *         alive,
    dbyte *        reply_code,
    char *         reply_text,
    dbyte          channel_nbr);
int
demo_client_agent_channel_open_destructor (void *data);
static int
demo_client_agent_channel_close_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr);
int
demo_client_agent_channel_close_destructor (void *data);
static int
demo_client_agent_channel_flow_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active);
int
demo_client_agent_channel_flow_destructor (void *data);
static int
demo_client_agent_channel_flow_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active);
int
demo_client_agent_channel_flow_ok_destructor (void *data);
static int
demo_client_agent_exchange_declare_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         exchange,
    char *         type,
    Bool           passive,
    Bool           durable,
    Bool           auto_delete);
int
demo_client_agent_exchange_declare_destructor (void *data);
static int
demo_client_agent_exchange_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         exchange,
    Bool           if_unused);
int
demo_client_agent_exchange_delete_destructor (void *data);
static int
demo_client_agent_queue_declare_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    Bool           passive,
    Bool           durable,
    Bool           exclusive,
    Bool           auto_delete);
int
demo_client_agent_queue_declare_destructor (void *data);
static int
demo_client_agent_queue_bind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    char *         exchange,
    icl_longstr_t *  arguments);
int
demo_client_agent_queue_bind_destructor (void *data);
static int
demo_client_agent_queue_purge_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue);
int
demo_client_agent_queue_purge_destructor (void *data);
static int
demo_client_agent_queue_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    Bool           if_unused,
    Bool           if_empty);
int
demo_client_agent_queue_delete_destructor (void *data);
static int
demo_client_agent_basic_consume_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue);
int
demo_client_agent_basic_consume_destructor (void *data);
static int
demo_client_agent_basic_cancel_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag);
int
demo_client_agent_basic_cancel_destructor (void *data);
static int
demo_client_agent_basic_publish_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key);
int
demo_client_agent_basic_publish_destructor (void *data);
static int
demo_client_agent_basic_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue);
int
demo_client_agent_basic_get_destructor (void *data);
static int
demo_client_agent_direct_put_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         sink);
int
demo_client_agent_direct_put_destructor (void *data);
static int
demo_client_agent_direct_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         feed);
int
demo_client_agent_direct_get_destructor (void *data);
static int
demo_client_agent_push_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_client_method_t *  method);
int
demo_client_agent_push_destructor (void *data);
static int
demo_client_agent_direct_out_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_content_basic_t *  content,
    byte           options);
int
demo_client_agent_direct_out_destructor (void *data);
//  Function definitions                                                       

static int
demo_client_agent_channel_open_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    smt_method_queue_t *  method_queue,
    Bool *         alive,
    dbyte *        reply_code,
    char *         reply_text,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_client_agent_channel_open_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_channel_open_cache);
    _message->method_queue              = method_queue;
    _message->alive                     = alive;
    _message->reply_code                = reply_code;
    _message->reply_text                = reply_text;
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_channel_open_destructor);
    
    return rc;
}

int
demo_client_agent_channel_open_destructor (void *data)
{
    int
        rc = 0;
smt_method_queue_t * method_queue;
Bool * alive;
dbyte * reply_code;
char * reply_text;
dbyte channel_nbr;

    method_queue = ((demo_client_agent_channel_open_t *) data)-> method_queue;
    alive = ((demo_client_agent_channel_open_t *) data)-> alive;
    reply_code = ((demo_client_agent_channel_open_t *) data)-> reply_code;
    reply_text = ((demo_client_agent_channel_open_t *) data)-> reply_text;
    channel_nbr = ((demo_client_agent_channel_open_t *) data)-> channel_nbr;


    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_channel_close_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_client_agent_channel_close_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_channel_close_cache);
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_channel_close_destructor);
    
    return rc;
}

int
demo_client_agent_channel_close_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;

    channel_nbr = ((demo_client_agent_channel_close_t *) data)-> channel_nbr;


    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_channel_flow_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active)
{
    int
        rc = 0;
    demo_client_agent_channel_flow_t
        *_message;
        
    //  Possess code for channel flow
    

    _message = icl_mem_cache_alloc (s_channel_flow_cache);
    _message->channel_nbr               = channel_nbr;
    _message->active                    = active;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_channel_flow_destructor);
    
    return rc;
}

int
demo_client_agent_channel_flow_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
Bool active;

    channel_nbr = ((demo_client_agent_channel_flow_t *) data)-> channel_nbr;
    active = ((demo_client_agent_channel_flow_t *) data)-> active;



    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_channel_flow_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active)
{
    int
        rc = 0;
    demo_client_agent_channel_flow_ok_t
        *_message;
        
    //  Possess code for channel flow-ok
    

    _message = icl_mem_cache_alloc (s_channel_flow_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->active                    = active;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_channel_flow_ok_destructor);
    
    return rc;
}

int
demo_client_agent_channel_flow_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
Bool active;

    channel_nbr = ((demo_client_agent_channel_flow_ok_t *) data)-> channel_nbr;
    active = ((demo_client_agent_channel_flow_ok_t *) data)-> active;



    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_exchange_declare_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         exchange,
    char *         type,
    Bool           passive,
    Bool           durable,
    Bool           auto_delete)
{
    int
        rc = 0;
    demo_client_agent_exchange_declare_t
        *_message;
        
    //  Possess code for exchange declare
    
        exchange = icl_mem_strdup (exchange);
        type = icl_mem_strdup (type);
    

    _message = icl_mem_cache_alloc (s_exchange_declare_cache);
    _message->channel_nbr               = channel_nbr;
    _message->exchange                  = exchange;
    _message->type                      = type;
    _message->passive                   = passive;
    _message->durable                   = durable;
    _message->auto_delete               = auto_delete;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_exchange_declare_destructor);
    
    return rc;
}

int
demo_client_agent_exchange_declare_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * exchange;
char * type;
Bool passive;
Bool durable;
Bool auto_delete;

    channel_nbr = ((demo_client_agent_exchange_declare_t *) data)-> channel_nbr;
    exchange = ((demo_client_agent_exchange_declare_t *) data)-> exchange;
    type = ((demo_client_agent_exchange_declare_t *) data)-> type;
    passive = ((demo_client_agent_exchange_declare_t *) data)-> passive;
    durable = ((demo_client_agent_exchange_declare_t *) data)-> durable;
    auto_delete = ((demo_client_agent_exchange_declare_t *) data)-> auto_delete;


        icl_mem_free (exchange);
        icl_mem_free (type);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_exchange_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         exchange,
    Bool           if_unused)
{
    int
        rc = 0;
    demo_client_agent_exchange_delete_t
        *_message;
        
    //  Possess code for exchange delete
    
        exchange = icl_mem_strdup (exchange);
    

    _message = icl_mem_cache_alloc (s_exchange_delete_cache);
    _message->channel_nbr               = channel_nbr;
    _message->exchange                  = exchange;
    _message->if_unused                 = if_unused;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_exchange_delete_destructor);
    
    return rc;
}

int
demo_client_agent_exchange_delete_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * exchange;
Bool if_unused;

    channel_nbr = ((demo_client_agent_exchange_delete_t *) data)-> channel_nbr;
    exchange = ((demo_client_agent_exchange_delete_t *) data)-> exchange;
    if_unused = ((demo_client_agent_exchange_delete_t *) data)-> if_unused;


        icl_mem_free (exchange);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_queue_declare_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    Bool           passive,
    Bool           durable,
    Bool           exclusive,
    Bool           auto_delete)
{
    int
        rc = 0;
    demo_client_agent_queue_declare_t
        *_message;
        
    //  Possess code for queue declare
    
        queue = icl_mem_strdup (queue);
    

    _message = icl_mem_cache_alloc (s_queue_declare_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;
    _message->passive                   = passive;
    _message->durable                   = durable;
    _message->exclusive                 = exclusive;
    _message->auto_delete               = auto_delete;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_queue_declare_destructor);
    
    return rc;
}

int
demo_client_agent_queue_declare_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;
Bool passive;
Bool durable;
Bool exclusive;
Bool auto_delete;

    channel_nbr = ((demo_client_agent_queue_declare_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_queue_declare_t *) data)-> queue;
    passive = ((demo_client_agent_queue_declare_t *) data)-> passive;
    durable = ((demo_client_agent_queue_declare_t *) data)-> durable;
    exclusive = ((demo_client_agent_queue_declare_t *) data)-> exclusive;
    auto_delete = ((demo_client_agent_queue_declare_t *) data)-> auto_delete;


        icl_mem_free (queue);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_queue_bind_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    char *         exchange,
    icl_longstr_t *  arguments)
{
    int
        rc = 0;
    demo_client_agent_queue_bind_t
        *_message;
        
    //  Possess code for queue bind
    
        queue = icl_mem_strdup (queue);
        exchange = icl_mem_strdup (exchange);
        arguments = icl_longstr_dup (arguments);
    

    _message = icl_mem_cache_alloc (s_queue_bind_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;
    _message->exchange                  = exchange;
    _message->arguments                 = arguments;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_queue_bind_destructor);
    
    return rc;
}

int
demo_client_agent_queue_bind_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;
char * exchange;
icl_longstr_t * arguments;

    channel_nbr = ((demo_client_agent_queue_bind_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_queue_bind_t *) data)-> queue;
    exchange = ((demo_client_agent_queue_bind_t *) data)-> exchange;
    arguments = ((demo_client_agent_queue_bind_t *) data)-> arguments;


        icl_mem_free (queue);
        icl_mem_free (exchange);
        icl_longstr_destroy (&arguments);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_queue_purge_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue)
{
    int
        rc = 0;
    demo_client_agent_queue_purge_t
        *_message;
        
    //  Possess code for queue purge
    
        queue = icl_mem_strdup (queue);
    

    _message = icl_mem_cache_alloc (s_queue_purge_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_queue_purge_destructor);
    
    return rc;
}

int
demo_client_agent_queue_purge_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;

    channel_nbr = ((demo_client_agent_queue_purge_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_queue_purge_t *) data)-> queue;


        icl_mem_free (queue);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_queue_delete_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    Bool           if_unused,
    Bool           if_empty)
{
    int
        rc = 0;
    demo_client_agent_queue_delete_t
        *_message;
        
    //  Possess code for queue delete
    
        queue = icl_mem_strdup (queue);
    

    _message = icl_mem_cache_alloc (s_queue_delete_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;
    _message->if_unused                 = if_unused;
    _message->if_empty                  = if_empty;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_queue_delete_destructor);
    
    return rc;
}

int
demo_client_agent_queue_delete_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;
Bool if_unused;
Bool if_empty;

    channel_nbr = ((demo_client_agent_queue_delete_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_queue_delete_t *) data)-> queue;
    if_unused = ((demo_client_agent_queue_delete_t *) data)-> if_unused;
    if_empty = ((demo_client_agent_queue_delete_t *) data)-> if_empty;


        icl_mem_free (queue);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_basic_consume_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue)
{
    int
        rc = 0;
    demo_client_agent_basic_consume_t
        *_message;
        
    //  Possess code for basic consume
    
        queue = icl_mem_strdup (queue);
    

    _message = icl_mem_cache_alloc (s_basic_consume_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_basic_consume_destructor);
    
    return rc;
}

int
demo_client_agent_basic_consume_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;

    channel_nbr = ((demo_client_agent_basic_consume_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_basic_consume_t *) data)-> queue;


        icl_mem_free (queue);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_basic_cancel_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag)
{
    int
        rc = 0;
    demo_client_agent_basic_cancel_t
        *_message;
        
    //  Possess code for basic cancel
    
        consumer_tag = icl_mem_strdup (consumer_tag);
    

    _message = icl_mem_cache_alloc (s_basic_cancel_cache);
    _message->channel_nbr               = channel_nbr;
    _message->consumer_tag              = consumer_tag;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_basic_cancel_destructor);
    
    return rc;
}

int
demo_client_agent_basic_cancel_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * consumer_tag;

    channel_nbr = ((demo_client_agent_basic_cancel_t *) data)-> channel_nbr;
    consumer_tag = ((demo_client_agent_basic_cancel_t *) data)-> consumer_tag;


        icl_mem_free (consumer_tag);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_basic_publish_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key)
{
    int
        rc = 0;
    demo_client_agent_basic_publish_t
        *_message;
        
    //  Possess code for basic publish
    
        assert (content);
        content = demo_content_basic_link (content);
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
    

    _message = icl_mem_cache_alloc (s_basic_publish_cache);
    _message->channel_nbr               = channel_nbr;
    _message->content                   = content;
    _message->exchange                  = exchange;
    _message->routing_key               = routing_key;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_basic_publish_destructor);
    
    return rc;
}

int
demo_client_agent_basic_publish_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
demo_content_basic_t * content;
char * exchange;
char * routing_key;

    channel_nbr = ((demo_client_agent_basic_publish_t *) data)-> channel_nbr;
    content = ((demo_client_agent_basic_publish_t *) data)-> content;
    exchange = ((demo_client_agent_basic_publish_t *) data)-> exchange;
    routing_key = ((demo_client_agent_basic_publish_t *) data)-> routing_key;


        demo_content_basic_unlink (&content);
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_basic_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue)
{
    int
        rc = 0;
    demo_client_agent_basic_get_t
        *_message;
        
    //  Possess code for basic get
    
        queue = icl_mem_strdup (queue);
    

    _message = icl_mem_cache_alloc (s_basic_get_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_basic_get_destructor);
    
    return rc;
}

int
demo_client_agent_basic_get_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;

    channel_nbr = ((demo_client_agent_basic_get_t *) data)-> channel_nbr;
    queue = ((demo_client_agent_basic_get_t *) data)-> queue;


        icl_mem_free (queue);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_direct_put_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         sink)
{
    int
        rc = 0;
    demo_client_agent_direct_put_t
        *_message;
        
    //  Possess code for direct put
    
        sink = icl_mem_strdup (sink);
    

    _message = icl_mem_cache_alloc (s_direct_put_cache);
    _message->channel_nbr               = channel_nbr;
    _message->sink                      = sink;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_direct_put_destructor);
    
    return rc;
}

int
demo_client_agent_direct_put_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * sink;

    channel_nbr = ((demo_client_agent_direct_put_t *) data)-> channel_nbr;
    sink = ((demo_client_agent_direct_put_t *) data)-> sink;


        icl_mem_free (sink);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_direct_get_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         feed)
{
    int
        rc = 0;
    demo_client_agent_direct_get_t
        *_message;
        
    //  Possess code for direct get
    
        feed = icl_mem_strdup (feed);
    

    _message = icl_mem_cache_alloc (s_direct_get_cache);
    _message->channel_nbr               = channel_nbr;
    _message->feed                      = feed;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_direct_get_destructor);
    
    return rc;
}

int
demo_client_agent_direct_get_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * feed;

    channel_nbr = ((demo_client_agent_direct_get_t *) data)-> channel_nbr;
    feed = ((demo_client_agent_direct_get_t *) data)-> feed;


        icl_mem_free (feed);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_push_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_client_method_t *  method)
{
    int
        rc = 0;
    demo_client_agent_push_t
        *_message;
        
    //  Possess code for push
    
        method = demo_client_method_link (method);
    

    _message = icl_mem_cache_alloc (s_push_cache);
    _message->channel_nbr               = channel_nbr;
    _message->method                    = method;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_push_destructor);
    
    return rc;
}

int
demo_client_agent_push_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
demo_client_method_t * method;

    channel_nbr = ((demo_client_agent_push_t *) data)-> channel_nbr;
    method = ((demo_client_agent_push_t *) data)-> method;


        demo_client_method_unlink (&method);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_client_agent_direct_out_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_content_basic_t *  content,
    byte           options)
{
    int
        rc = 0;
    demo_client_agent_direct_out_t
        *_message;
        
    //  Possess code for direct out
    
        content = demo_content_basic_link (content);
    

    _message = icl_mem_cache_alloc (s_direct_out_cache);
    _message->content                   = content;
    _message->options                   = options;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_client_agent_direct_out_destructor);
    
    return rc;
}

int
demo_client_agent_direct_out_destructor (void *data)
{
    int
        rc = 0;
demo_content_basic_t * content;
byte options;

    content = ((demo_client_agent_direct_out_t *) data)-> content;
    options = ((demo_client_agent_direct_out_t *) data)-> options;


        demo_content_basic_unlink (&content);
    

    icl_mem_free (data);
    return rc;
}


int
demo_client_agent_connection_close (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_close_m_event,
                SMT_PRIORITY_NORMAL,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_connection_tune (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_tune_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_connection_open_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_open_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_send_channel_close_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                send_channel_close_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_connection_close_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_close_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_shutdown (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                shutdown_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_direct_close (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                direct_close_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_channel_open (
    smt_thread_t * thread,
    smt_method_queue_t *  method_queue,
    Bool *                alive,
    dbyte *               reply_code,
    char *                reply_text,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_channel_open_send (
                thread->reply_queue, channel_open_m_event,
                method_queue,
                alive,
                reply_code,
                reply_text,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_channel_close (
    smt_thread_t * thread,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_channel_close_send (
                thread->reply_queue, channel_close_m_event,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_channel_flow (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_channel_flow_send (
                thread->reply_queue, channel_flow_m_event,
                channel_nbr,
                active);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_channel_flow_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_channel_flow_ok_send (
                thread->reply_queue, channel_flow_ok_m_event,
                channel_nbr,
                active);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_exchange_declare (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                exchange,
    char *                type,
    Bool                  passive,
    Bool                  durable,
    Bool                  auto_delete)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_exchange_declare_send (
                thread->reply_queue, exchange_declare_m_event,
                channel_nbr,
                exchange,
                type,
                passive,
                durable,
                auto_delete);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_exchange_delete (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                exchange,
    Bool                  if_unused)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_exchange_delete_send (
                thread->reply_queue, exchange_delete_m_event,
                channel_nbr,
                exchange,
                if_unused);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_queue_declare (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue,
    Bool                  passive,
    Bool                  durable,
    Bool                  exclusive,
    Bool                  auto_delete)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_queue_declare_send (
                thread->reply_queue, queue_declare_m_event,
                channel_nbr,
                queue,
                passive,
                durable,
                exclusive,
                auto_delete);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_queue_bind (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue,
    char *                exchange,
    icl_longstr_t *       arguments)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_queue_bind_send (
                thread->reply_queue, queue_bind_m_event,
                channel_nbr,
                queue,
                exchange,
                arguments);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_queue_purge (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_queue_purge_send (
                thread->reply_queue, queue_purge_m_event,
                channel_nbr,
                queue);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_queue_delete (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue,
    Bool                  if_unused,
    Bool                  if_empty)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_queue_delete_send (
                thread->reply_queue, queue_delete_m_event,
                channel_nbr,
                queue,
                if_unused,
                if_empty);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_basic_consume (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_basic_consume_send (
                thread->reply_queue, basic_consume_m_event,
                channel_nbr,
                queue);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_basic_cancel (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                consumer_tag)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_basic_cancel_send (
                thread->reply_queue, basic_cancel_m_event,
                channel_nbr,
                consumer_tag);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_basic_publish (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_content_basic_t * content,
    char *                exchange,
    char *                routing_key)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_basic_publish_send (
                thread->reply_queue, basic_publish_m_event,
                channel_nbr,
                content,
                exchange,
                routing_key);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_basic_get (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_basic_get_send (
                thread->reply_queue, basic_get_m_event,
                channel_nbr,
                queue);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_direct_put (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                sink)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_direct_put_send (
                thread->reply_queue, direct_put_m_event,
                channel_nbr,
                sink);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_direct_get (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                feed)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_direct_get_send (
                thread->reply_queue, direct_get_m_event,
                channel_nbr,
                feed);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_push (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_client_method_t * method)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_push_send (
                thread->reply_queue, push_m_event,
                channel_nbr,
                method);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_client_agent_direct_out (
    smt_thread_t * thread,
    demo_content_basic_t * content,
    byte                  options)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_client_agent_manager);
        rc = demo_client_agent_direct_out_send (
                thread->reply_queue, direct_out_m_event,
                content,
                options);
    }
    else
        rc = -1;
        
    return rc;
}


#define tcb ((demo_client_agent_connection_context_t *) thread->context)

//  Raises a connection exception
//  Called internally by the agent when a protocol error is detected
static void
s_connection_exception (
    smt_thread_t *thread,               //  Calling thread
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    s_report_error (thread, "Connection", "closing connection",
        reply_code, reply_text, faulting_class_id, faulting_method_id);

    //  Save reply code for demo_client_connection
    tcb->reply_code = reply_code;
    icl_shortstr_cpy (tcb->reply_text, reply_text);
    smt_thread_raise_exception (thread, connection_error_event);
}

//  Aborts the connection immediately
//  Called internally by the agent if we detect an unrecoverable error
//  (e.g. malformed frame)
static void
s_connection_abort (
    smt_thread_t *thread,               //  Calling thread
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    s_report_error (thread, "Connection", "aborting connection",
        reply_code, reply_text, faulting_class_id, faulting_method_id);

    //  Save reply code for demo_client_connection
    tcb->reply_code = reply_code;
    icl_shortstr_cpy (tcb->reply_text, reply_text);
    smt_thread_raise_exception (thread, connection_abort_event);
}

//  Helper function to report connection and channel errors
static void
s_report_error (
    smt_thread_t *thread,               //  Calling thread
    char *context,                      //  Error context
    char *action_taken,                 //  Action taken
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    if (reply_code == 100)
        return;
    if ((tcb->trace >= ASL_TRACE_MED || reply_code >= 500) &&
        faulting_class_id && faulting_method_id)
        icl_console_print (
            "E: %s error: %d %s "
            "(faulting method %u, %u), %s "  "(%s)",
            context, reply_code, reply_text,
            faulting_class_id, faulting_method_id, action_taken,
            tcb->host);
    else
        icl_console_print (
            "E: %s error: %d %s, %s "  "(%s)",
            context, reply_code, reply_text, action_taken,
            tcb->host);
}

//  Executes an incoming method if it matches the requested method,
//  raises a connection exception if not.
static inline void
s_execute_method (
    smt_thread_t *thread,               //  Thread executing the method
    Bool channel_method,                //  FALSE == connection method
    int class_id,                       //  Expected class id
    int method_id                       //  Expected method id
    )
{
    if (tcb->method_in->class_id  == class_id
    &&  tcb->method_in->method_id == method_id) {
        ;                               //  Proceed
    }
    else
        s_connection_exception (thread, ASL_SYNTAX_ERROR,
            "Method not valid here",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
}

//  Executes an incoming functional method.  Checks if we sent the predecessor
//  method (if any) and raises a connection exception if not.
static void
s_execute_functional_method (
    smt_thread_t *thread,               //  Thread executing the method
    dbyte class_id,                     //  Incoming class id
    dbyte method_id,                    //  Incoming method id
    dbyte out_class_id,                 //  Last sent class id
    dbyte out_method_id)                //  Last sent method id
{
    dbyte
        pred_method_id;                 //  Predecessor method id, if any
    uint
        class_index,
        method_index;

    //  Convert input class index (20, 30, 41,...) into lookup index
    for (class_index = 0; s_class_index [class_index]; class_index++)
        if (class_id == s_class_index [class_index])
            break;

    //  Convert input method index (20, 30, 41,...) into lookup index
    for (method_index = 0; s_method_index [class_index][method_index]; method_index++)
        if (method_id == s_method_index [class_index][method_index])
            break;

    //  If we got a method that requires a predecessor, check we sent that
    if (s_method_index [class_index][method_index]) {
        pred_method_id = s_predecessor [class_index][method_index];
        if (pred_method_id > 0
        && (pred_method_id != out_method_id
        &&  out_class_id == class_id))
            s_connection_exception (thread,
                ASL_SYNTAX_ERROR, "Method not allowed at this stage",
                method_id, class_id);
        else
        {
            demo_client_method_t
                *method;

            //  Send method to demo_client_session for processing
            method = demo_client_method_link (tcb->method_in);
            if (smt_method_send (tcb->channel->method_queue, SMT_NULL_EVENT,
                SMT_PRIORITY_NORMAL, DEMO_CLIENT_REPLY_METHOD, method,
                s_method_destructor))
                demo_client_method_unlink (&method);
        }
    }
    else
        s_connection_exception (thread,
            ASL_SYNTAX_ERROR, "Method invalid - unknown method id",
            method_id, class_id);
}

//  Destructor for reply methods sent to session or connection agent
static int
s_method_destructor (void *method)
{
    demo_client_method_t
        *method_p;

    method_p = method;
    demo_client_method_unlink (&method_p);
    return 0;
}

//  Destructor for contents sent as methods
static int
s_content_destructor (void *content)
{
    demo_content_basic_t
        *content_p;

    content_p = content;
    demo_content_basic_unlink (&content_p);
    return (0);
}

//  Sends an outgoing method to the socket
static void
s_send_method_out (
    smt_thread_t *thread,
    dbyte channel,
    smt_log_t *log)
{
    ipr_bucket_t
        *payload;

    if (tcb->trace >= ASL_TRACE_LOW)
        demo_client_method_dump (tcb->method_out, log, "I: send ");

    //  Send the method
    payload = demo_client_method_encode (tcb->method_out);
    s_write_payload (thread, channel, ASL_FRAME_METHOD, &payload);

    tcb->out_class_id  = tcb->method_out->class_id;
    tcb->out_method_id = tcb->method_out->method_id;

    s_history_table [s_history_index].inout = 'o';
    s_history_table [s_history_index].name  = tcb->method_out->name;
    s_history_index = (s_history_index + 1) % METHOD_HISTORY_SIZE;
}

//  Writes a non-null payload to the socket
static void
s_write_payload (
    smt_thread_t *thread,
    dbyte channel,
    byte type,                          //  Frame type indicator
    ipr_bucket_t **payload_p)
{
    ipr_bucket_t
        *payload = NULL,
        *bucket;                        //  Header bucket
    byte
        *bucket_ptr;
    static byte
        frame_end = ASL_FRAME_END;

    if (payload_p) {
        payload = *payload_p;
        if (!payload)
            return;                     //  Ignore empty payloads
    }
    if (payload->cur_size > tcb->frame_max) {
        icl_console_print ("E: frame type=%d is too large, discarded (size=%d, max=%d)",
            type, payload->cur_size, tcb->frame_max);
        return;
    }
    //  Write ASL frame header to normal bucket
    bucket = ipr_bucket_new (ASL_HEADER_SIZE);
    bucket_ptr = bucket->data;
    PUT_OCTET (bucket_ptr, type);
    PUT_SHORT (bucket_ptr, channel);
    PUT_LONG  (bucket_ptr, payload->cur_size);
    bucket->cur_size = bucket_ptr - bucket->data;

    s_socket_write_bucket (thread, bucket);
    s_socket_write_bucket (thread, payload);
    s_socket_write        (thread, &frame_end, 1);
    ipr_bucket_unlink (&bucket);
    ipr_bucket_unlink (payload_p);
}

//  Helper functions for socket input/output
static void inline
s_socket_write (
    smt_thread_t *thread,
    byte *buffer,
    size_t size)
{
    smt_socket_request_write (
        thread,
        tcb->socket,
        0,
        size,
        buffer,
        SMT_NULL_EVENT);
}

static void inline
s_socket_write_bucket (
    smt_thread_t *thread,
    ipr_bucket_t *bucket)
{
    smt_socket_request_write_bucket (
        thread,
        tcb->socket,
        0,
        bucket,
        SMT_NULL_EVENT);
}

static void inline
s_socket_write_shortstr (
    smt_thread_t *thread,
    char *string)
{
    ipr_bucket_t
        *bucket;                        //  Formatted output

    assert (strlen (string) < 256);
    bucket = ipr_bucket_new (256);
    bucket->data [0] = (byte) strlen (string);
    memcpy (bucket->data + 1, string, strlen (string));
    bucket->cur_size = 1 + strlen (string);
    s_socket_write_bucket (thread, bucket);
    ipr_bucket_unlink (&bucket);
}

static void inline
s_socket_read (
    smt_thread_t *thread,
    byte *buffer,
    size_t size)
{
    if (size <= IPR_BUCKET_MAX_SIZE) {
        smt_socket_request_read (
            thread,
            tcb->socket,
            0,
            size,
            size,
            buffer,
            SMT_NULL_EVENT);
    }
    else
        s_connection_abort (thread,
            ASL_FRAME_ERROR, "Illegal frame size specified", 0, 0);
}

static void inline
s_socket_read_bucket (
    smt_thread_t *thread,
    ipr_bucket_t **bucket_p,
    size_t size)
{
    if (size <= IPR_BUCKET_MAX_SIZE) {
        smt_socket_request_read_bucket (
            thread,
            tcb->socket,
            0,
            size,
            size,
            bucket_p,
            SMT_NULL_EVENT);
    }
    else
        s_connection_abort (thread,
            ASL_FRAME_ERROR, "Illegal frame size specified", 0, 0);
}

#undef tcb

void
demo_client_agent_term (void)
{

}

int 
demo_client_agent_init (void)
{
    int
        rc = SMT_OK;                    //  Return code

    s_connection_context_cache = icl_cache_get (sizeof (demo_client_agent_connection_context_t));
    s_direct_context_cache = icl_cache_get (sizeof (demo_client_agent_direct_context_t));
    s_channel_open_cache = icl_cache_get (sizeof (demo_client_agent_channel_open_t));
    s_channel_close_cache = icl_cache_get (sizeof (demo_client_agent_channel_close_t));
    s_channel_flow_cache = icl_cache_get (sizeof (demo_client_agent_channel_flow_t));
    s_channel_flow_ok_cache = icl_cache_get (sizeof (demo_client_agent_channel_flow_ok_t));
    s_exchange_declare_cache = icl_cache_get (sizeof (demo_client_agent_exchange_declare_t));
    s_exchange_delete_cache = icl_cache_get (sizeof (demo_client_agent_exchange_delete_t));
    s_queue_declare_cache = icl_cache_get (sizeof (demo_client_agent_queue_declare_t));
    s_queue_bind_cache = icl_cache_get (sizeof (demo_client_agent_queue_bind_t));
    s_queue_purge_cache = icl_cache_get (sizeof (demo_client_agent_queue_purge_t));
    s_queue_delete_cache = icl_cache_get (sizeof (demo_client_agent_queue_delete_t));
    s_basic_consume_cache = icl_cache_get (sizeof (demo_client_agent_basic_consume_t));
    s_basic_cancel_cache = icl_cache_get (sizeof (demo_client_agent_basic_cancel_t));
    s_basic_publish_cache = icl_cache_get (sizeof (demo_client_agent_basic_publish_t));
    s_basic_get_cache = icl_cache_get (sizeof (demo_client_agent_basic_get_t));
    s_direct_put_cache = icl_cache_get (sizeof (demo_client_agent_direct_put_t));
    s_direct_get_cache = icl_cache_get (sizeof (demo_client_agent_direct_get_t));
    s_push_cache = icl_cache_get (sizeof (demo_client_agent_push_t));
    s_direct_out_cache = icl_cache_get (sizeof (demo_client_agent_direct_out_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    memset (s_history_table, 0, sizeof (s_history_table));

    }
    icl_system_register (NULL, demo_client_agent_term);
    
    return rc;
}

void
demo_client_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
demo_client_agent_connection_thread_new (
    smt_method_queue_t * method_queue,
    Bool * alive,
    Bool * interrupt,
    dbyte * reply_code,
    char * reply_text,
    char * host,
    char * virtual_host,
    icl_longstr_t * auth_data,
    char * instance,
    int trace,
    int timeout)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_client_agent_manager,
                             demo_client_agent_catcher,
                             demo_client_agent_connection_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_CLIENT_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_client_agent",
                connection_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_connection_context_cache);
        memset (thread->context, 0, sizeof (demo_client_agent_connection_context_t));
        ((demo_client_agent_connection_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        //  Store the reply queue for the connection object
        tcb->method_queue    = smt_method_queue_link (method_queue);
        tcb->conn_alive      = alive;
        tcb->conn_interrupt  = interrupt;
        tcb->conn_reply_code = reply_code;
        tcb->conn_reply_text = reply_text;
        tcb->verbose         = TRUE;
        icl_shortstr_cpy (tcb->host, host);
        icl_shortstr_cpy (tcb->virtual_host, virtual_host);
        tcb->auth_data  = icl_longstr_dup (auth_data);
        icl_shortstr_cpy (tcb->instance, instance);
        tcb->trace = trace;
        //  SMT timeouts are in usecs, WireAPI uses msecs
        tcb->timeout = timeout * 1000;

        smt_thread_wake (thread);
    
#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_client_agent_input_thread_new (
    smt_thread_t * share)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_client_agent_manager,
                             demo_client_agent_catcher,
                             demo_client_agent_input_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_CLIENT_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_client_agent",
                input_thread_name);
#endif

        thread->place   = 98;
        smt_thread_share (thread, share);
        thread->event_name = event_name;
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_client_agent_direct_thread_new (
    int type,
    smt_method_queue_t * method_queue,
    char * host,
    char * lease,
    int trace,
    int timeout)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_client_agent_manager,
                             demo_client_agent_catcher,
                             demo_client_agent_direct_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_CLIENT_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_client_agent",
                direct_thread_name);
#endif

        thread->place   = 178;
        thread->context = icl_mem_cache_alloc (s_direct_context_cache);
        memset (thread->context, 0, sizeof (demo_client_agent_direct_context_t));
        ((demo_client_agent_direct_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

        //
        tcb->type = type;
        tcb->method_queue = smt_method_queue_link (method_queue);
        icl_shortstr_cpy (tcb->host, host);
        icl_shortstr_cpy (tcb->lease, lease);
        tcb->trace = trace;
        tcb->timeout = timeout * 1000;  //  From msecs to SMT usecs

        if (tcb->trace)
            tcb->verbose = TRUE;
        if (tcb->trace >= ASL_TRACE_HIGH)
            smt_socket_request_trace (TRUE);
        smt_thread_wake (thread);
    
#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_client_agent_heartbeat_thread_new (
    smt_thread_t * share)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_client_agent_manager,
                             demo_client_agent_catcher,
                             demo_client_agent_heartbeat_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_CLIENT_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_client_agent",
                heartbeat_thread_name);
#endif

        thread->place   = 203;
        smt_thread_share (thread, share);
        thread->event_name = event_name;
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
demo_client_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == demo_client_agent_manager);
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
            case 12:
            case 18:
            case 19:
            case 20:
            case 21:
            case 138:
            case 139:
            case 140:
            case 141:
            case 101:
            case 102:
            case 103:
            case 104:
            case 105:
            case 106:
            case 107:
            case 160:
            case 161:
            case 162:
            case 163:
            case 197:
            case 72:
            case 73:
            case 74:
            case 75:
            case 202:
            case 188:
            case 189:
            case 190:
            case 191:
            case 192:
            case 89:
            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 136:
            case 96:
            case 207:
            case 76:
            case 77:
            case 78:
            case 79:
            case 114:
            case 81:
            case 164:
            case 165:
            case 166:
            case 147:
            case 52:
            case 149:
            case 171:
            case 172:
            case 173:
            case 57:
            case 58:
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
            case 65:
            case 137:
            case 67:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 210:
            case 35:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 184:
            case 148:
            case 13:
            case 150:
            case 15:
            case 174:
            case 110:
            case 111:
            case 112:
            case 113:
            case 181:
            case 185:
            case 135:
            case 182:
            case 121:
            case 122:
            case 123:
            case 124:
            case 125:
            case 126:
            case 127:
            case 128:
            case 115:
            case 116:
            case 151:
            case 152:
            case 193:
            case 183:
            case 145:
            case 146:
            case 175:
            case 176:
            case 199:
            case 208:
            case 198:
            case 200:
            case 153:
            case 177:
            case 201:
                thread->place = SMT_TERM_PLACE;
                break;
            case 87:
            case 83:
            case 88:
            case 85:
            case 95:
            case 97:
            case 86:
            case 84:
                thread->place = 82;
                break;
            case 48:
            case 49:
            case 40:
            case 37:
            case 43:
            case 38:
            case 54:
            case 55:
            case 56:
            case 66:
            case 39:
            case 46:
            case 45:
            case 53:
            case 50:
            case 44:
            case 42:
            case 51:
            case 41:
            case 47:
            case 23:
                thread->place = 36;
                break;
            case 119:
            case 120:
            case 118:
                thread->place = 117;
                break;
            case 196:
            case 187:
            case 195:
                thread->place = 194;
                break;
            case 180:
                thread->place = 186;
                break;
            case 131:
                thread->place = 154;
                break;
            case 130:
                thread->place = 142;
                break;
            case 155:
            case 157:
            case 143:
            case 100:
            case 156:
            case 170:
            case 144:
            case 158:
            case 134:
            case 132:
            case 133:
            case 159:
                thread->place = 129;
                break;
            case 14:
                thread->place = 2;
                break;
            case 209:
            case 205:
            case 206:
                thread->place = 204;
                break;
            case 22:
            case 3:
            case 34:
            case 17:
                thread->place = 16;
                break;
            case 169:
            case 168:
                thread->place = 167;
                break;
            case 109:
                thread->place = 108;
                break;
            case 71:
            case 80:
            case 69:
            case 70:
                thread->place = 68;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Connection thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        char
            *env_value;                 //  Get environment setting
        tcb->connection_thread = thread;
        tcb->input_thread      = NULL;
        tcb->heartbeat_thread  = NULL;
        tcb->frame_max         = ASL_FRAME_MIN_SIZE;
        tcb->liveness          = FULL_LIVENESS;

        if (tcb->trace)
            tcb->verbose = TRUE;
        if (tcb->trace >= ASL_TRACE_HIGH)
            smt_socket_request_trace (TRUE);

        tcb->channels = demo_client_channel_table_new ();
        env_value = getenv ("WIREAPI_SILENT");
        if (env_value && streq (env_value, "1") && !tcb->trace)
            tcb->verbose = FALSE;

        env_value = getenv ("WIREAPI_FAKESLOW");
        if (env_value && streq (env_value, "1")) {
            tcb->fakeslow = TRUE;
            icl_console_print ("W: WIREAPI_FAKESLOW=1 defined, simulating network congestion");
        }
        //  Register signal handlers, since we need this during connection
        //  negotiation before the heartbeat thread is started
        smt_signal_register (SIGINT, thread, interrupt_event);
        smt_signal_register (SIGTERM, thread, interrupt_event);

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, connect_event);
    
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
        if (thread->place < 16)                 //  Initialise Connection state
            thread->place = 2;
        else
        if (thread->place < 36)                 //  Negotiate Connection state
            thread->place = 16;
        else
        if (thread->place < 68)                 //  Connection Active state
            thread->place = 36;
        else
        if (thread->place < 82)                 //  Send Content Body If Any state
            thread->place = 68;
        else
        if (thread->place < 98)                 //  Close The Connection state
            thread->place = 82;
        else
        if (thread->place == 98) { //               Input thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, ok_event);
    
#undef  tcb
}
            }
            thread->place = 99;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
        if (thread->place < 108)                //  Start state
            thread->place = 99;
        else
        if (thread->place < 117)                //  Read Method state
            thread->place = 108;
        else
        if (thread->place < 129)                //  Connection Closing state
            thread->place = 117;
        else
        if (thread->place < 142)                //  Have Incoming Method state
            thread->place = 129;
        else
        if (thread->place < 154)                //  Have Connection Method state
            thread->place = 142;
        else
        if (thread->place < 167)                //  Have Channel Method state
            thread->place = 154;
        else
        if (thread->place < 178)                //  Read Basic Content state
            thread->place = 167;
        else
        if (thread->place == 178) { //              Direct thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

        char
            *env_value;                 //  Get environment setting

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        env_value = getenv ("WIREAPI_FAKESLOW");
        if (env_value && streq (env_value, "1"))
            tcb->fakeslow = TRUE;

        smt_thread_set_next_event (thread, connect_event);
    
#undef  tcb
}
            }
            thread->place = 179;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
        if (thread->place < 186)                //  Direct Initialise state
            thread->place = 179;
        else
        if (thread->place < 194)                //  Direct Negotiate state
            thread->place = 186;
        else
        if (thread->place < 203)                //  Direct Connection state
            thread->place = 194;
        else
        if (thread->place == 203) { //              Heartbeat thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        //  Register to receive interrupt_event on SIGINT, SIGTERM
        smt_signal_register (SIGINT, thread, interrupt_event);
        smt_signal_register (SIGTERM, thread, interrupt_event);
        smt_thread_set_next_event (thread, peer_alive_event);
    
#undef  tcb
}
            }
            thread->place = 204;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
            thread->place = 204;
    }

    switch (thread->place) {
        case 2:                         //  initialise connection state         
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connect_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 4; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 5; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 6; break;
                case interrupt_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 7; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 8; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 9; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 10; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 11; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 12; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 13; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 14; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 15; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 16:                        //  negotiate connection state          
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 17; break;
                case version_mismatch_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 18; break;
                case bad_protocol_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 19; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 20; break;
                case interrupt_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 21; break;
                case connection_tune_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 22; break;
                case connection_open_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 23; break;
                case connection_close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
                    thread->place = 24; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
                    thread->place = 25; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 26; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 27; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 28; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 29; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 30; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 31; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 32; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 33; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 34; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 35; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 36:                        //  connection active state             
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case discard_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 37; break;
                case channel_flow_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 38; break;
                case channel_flow_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 39; break;
                case exchange_declare_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 40; break;
                case exchange_delete_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 41; break;
                case queue_declare_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 42; break;
                case queue_bind_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 43; break;
                case queue_purge_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 44; break;
                case queue_delete_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 45; break;
                case basic_consume_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 46; break;
                case basic_cancel_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 47; break;
                case basic_publish_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
                    thread->place = 48; break;
                case basic_get_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 38);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 38);
#endif
                    thread->place = 49; break;
                case direct_put_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 39);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 39);
#endif
                    thread->place = 50; break;
                case direct_get_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 40);
#endif
                    thread->place = 51; break;
                case connection_close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
                    thread->place = 52; break;
                case channel_open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 42);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 42);
#endif
                    thread->place = 53; break;
                case channel_close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 43);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 43);
#endif
                    thread->place = 54; break;
                case send_channel_close_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 44);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 44);
#endif
                    thread->place = 55; break;
                case push_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 45);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 45);
#endif
                    thread->place = 56; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
                    thread->place = 57; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 58; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 59; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 60; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 61; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 62; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 63; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 64; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 65; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 66; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 67; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 68:                        //  send content body if any state      
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case start_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 69; break;
                case continue_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 70; break;
                case finished_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 71; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 72; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 73; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 74; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 75; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 76; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 77; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 78; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 79; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 80; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 81; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 82:                        //  close the connection state          
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case active_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 29);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 29);
#endif
                    thread->place = 83; break;
                case passive_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 30);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 30);
#endif
                    thread->place = 84; break;
                case close_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
                    thread->place = 85; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 86; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 87; break;
                case connection_close_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 33);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 33);
#endif
                    thread->place = 88; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 89; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 90; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 91; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 92; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 93; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 94; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 95; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 96; break;
                default:
                    thread->place = 97;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 99:                        //  start state                         
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 100; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 101; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 102; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 103; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 104; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 105; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 106; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 107; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 108:                       //  read method state                   
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case read_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 47);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 47);
#endif
                    thread->place = 109; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 110; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 111; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 112; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 113; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 114; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 115; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 116; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 117:                       //  connection closing state            
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 48);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 48);
#endif
                    thread->place = 118; break;
                case method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
                    thread->place = 119; break;
                case discard_frame_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 50);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 50);
#endif
                    thread->place = 120; break;
                case connection_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
                    thread->place = 121; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 122; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 123; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 124; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 125; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 126; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 127; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 128; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 129:                       //  have incoming method state          
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connection_class_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 52);
#endif
                    thread->place = 130; break;
                case channel_class_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 55);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 55);
#endif
                    thread->place = 131; break;
                case basic_content_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 132; break;
                case other_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 64);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 64);
#endif
                    thread->place = 133; break;
                case discard_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 134; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 135; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 136; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 137; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 138; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 139; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 140; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 141; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 142:                       //  have connection method state        
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connection_tune_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 53);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 53);
#endif
                    thread->place = 143; break;
                case connection_open_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
                    thread->place = 144; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 145; break;
                case connection_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
                    thread->place = 146; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 147; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 148; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 149; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 150; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 151; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 152; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 153; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 154:                       //  have channel method state           
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case channel_open_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 56);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 56);
#endif
                    thread->place = 155; break;
                case channel_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 57);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 57);
#endif
                    thread->place = 156; break;
                case channel_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
                    thread->place = 157; break;
                case channel_flow_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 59);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 59);
#endif
                    thread->place = 158; break;
                case channel_flow_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
                    thread->place = 159; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 160; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 161; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 162; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 163; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 164; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 165; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 166; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 167:                       //  read basic content state            
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case header_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 62);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 62);
#endif
                    thread->place = 168; break;
                case expect_body_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 63);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 63);
#endif
                    thread->place = 169; break;
                case method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
                    thread->place = 170; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 171; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 172; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 173; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 174; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 175; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 176; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 177; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 179:                       //  direct initialise state             
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connect_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 180; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 181; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 182; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 183; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 184; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 185; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 186:                       //  direct negotiate state              
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 187; break;
                case direct_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 68);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 68);
#endif
                    thread->place = 188; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 189; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 190; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 191; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 192; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 193; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 194:                       //  direct connection state             
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case direct_in_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 65);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 65);
#endif
                    thread->place = 195; break;
                case direct_out_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 66);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 66);
#endif
                    thread->place = 196; break;
                case direct_close_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 67);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 67);
#endif
                    thread->place = 197; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 198; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 199; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 200; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 201; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 202; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 204:                       //  monitor heartbeat state             
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case peer_alive_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 69);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 69);
#endif
                    thread->place = 205; break;
                case peer_slowing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 70);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 70);
#endif
                    thread->place = 206; break;
                case peer_stopped_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 71);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 71);
#endif
                    thread->place = 207; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 208; break;
                case interrupt_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 209; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 16);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 210; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_set_no_connection_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_start_setup_timer (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 3);
#endif
                    rc = action_connect_to_server (thread);

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
        case 17:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 4);
#endif
                    rc = action_send_protocol_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_read_frame_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 6);
#endif
                    rc = action_check_protocol_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
#endif
                    rc = action_read_frame_payload (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
#endif
                    rc = action_check_frame_format (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 53);
#endif
                    rc = action_decode_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_execute_connection_start (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
#endif
                    rc = action_start_input_thread (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_send_connection_start_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 18);
#endif
                    rc = action_tell_session_connection_almost (thread);

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

                case 11:                        //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 18:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 7);
#endif
                    rc = action_read_protocol_version (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 8);
#endif
                    rc = action_report_version_mismatch (thread);

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
        case 19:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
#endif
                    rc = action_report_bad_protocol (thread);

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
        case 20:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 49);
#endif
                    rc = action_report_connect_timeout (thread);

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
        case 21:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 99);
#endif
                    rc = action_unnamed_19 (thread);

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
        case 22:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_send_connection_tune_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_start_heartbeat_thread (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_start_heartbeat_timer (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 18);
#endif
                    rc = action_tell_session_connection_almost (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 17);
#endif
                    rc = action_send_connection_open (thread);

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
        case 23:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
#endif
                    rc = action_cancel_setup_timer (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 19);
#endif
                    rc = action_tell_session_connection_ready (thread);

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
        case 96:
        case 97:
        case 81:
        case 67:
        case 35:
        case 15:
        case 128:
        case 116:
        case 153:
        case 166:
        case 177:
        case 141:
        case 107:
        case 202:
        case 193:
        case 185:
        case 210:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 96
                    ||  thread->place == 81
                    ||  thread->place == 67
                    ||  thread->place == 35
                    ||  thread->place == 15
                    ||  thread->place == 128
                    ||  thread->place == 116
                    ||  thread->place == 153
                    ||  thread->place == 166
                    ||  thread->place == 177
                    ||  thread->place == 141
                    ||  thread->place == 107
                    ||  thread->place == 202
                    ||  thread->place == 193
                    ||  thread->place == 185
                    ||  thread->place == 210)
                        thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 38:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 20);
#endif
                    rc = action_send_channel_flow (thread);

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
        case 39:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 21);
#endif
                    rc = action_send_channel_flow_ok (thread);

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
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 22);
#endif
                    rc = action_send_exchange_declare (thread);

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
        case 41:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 23);
#endif
                    rc = action_send_exchange_delete (thread);

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
        case 42:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
#endif
                    rc = action_send_queue_declare (thread);

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
        case 43:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 25);
#endif
                    rc = action_send_queue_bind (thread);

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
        case 44:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 26);
#endif
                    rc = action_send_queue_purge (thread);

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
        case 45:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 27);
#endif
                    rc = action_send_queue_delete (thread);

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
        case 46:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 28);
#endif
                    rc = action_send_basic_consume (thread);

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
        case 47:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 29);
#endif
                    rc = action_send_basic_cancel (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 38);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 38);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 38);
#endif
                    rc = action_send_content_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 39);
#endif
                    rc = action_send_content_body_frame (thread);

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
        case 70:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 39);
#endif
                    rc = action_send_content_body_frame (thread);

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
        case 71:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 72:
        case 74:
        case 89:
        case 91:
        case 58:
        case 60:
        case 26:
        case 28:
        case 8:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
#endif
                    rc = action_report_socket_error (thread);

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
        case 73:
        case 90:
        case 59:
        case 27:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 48);
#endif
                    rc = action_report_socket_timeout (thread);

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
        case 75:
        case 92:
        case 61:
        case 29:
        case 9:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 41);
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

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 83:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 42);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 42);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 42);
#endif
                    rc = action_send_connection_close (thread);

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
        case 84:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 43);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 43);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 43);
#endif
                    rc = action_execute_connection_close (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 44);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 44);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 44);
#endif
                    rc = action_send_connection_close_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 45);
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

                case 5:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 85:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 103);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 103);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 103);
#endif
                    rc = action_unnamed_49 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 86:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 104);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 104);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 104);
#endif
                    rc = action_unnamed_50 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 87:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 105);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 105);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 105);
#endif
                    rc = action_unnamed_51 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 45);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 93:
        case 94:
        case 78:
        case 79:
        case 64:
        case 65:
        case 32:
        case 33:
        case 12:
        case 13:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_shut_down_slave_threads (thread);

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
        case 95:
        case 80:
        case 66:
        case 34:
        case 14:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
#endif
                    rc = action_send_heartbeat_frame (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_start_heartbeat_timer (thread);

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
        case 76:
        case 62:
        case 30:
        case 10:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 102);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 102);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 102);
#endif
                    rc = action_dialog_call_close_the_connection_passive_close (thread);

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
        case 77:
        case 63:
        case 24:
        case 31:
        case 11:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
#endif
                    rc = action_dialog_call_close_the_connection_active_close (thread);

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
        case 48:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 30);
#endif
                    rc = action_send_basic_publish (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 100);
#endif
                    rc = action_dialog_call_send_content_body_if_any_start (thread);

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
        case 49:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 31);
#endif
                    rc = action_send_basic_get (thread);

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
        case 50:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 32);
#endif
                    rc = action_send_direct_put (thread);

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
        case 51:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 33);
#endif
                    rc = action_send_direct_get (thread);

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
        case 52:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_set_no_connection_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
#endif
                    rc = action_dialog_call_close_the_connection_active_close (thread);

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
        case 53:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 34);
#endif
                    rc = action_send_channel_open (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 1);
#endif
                    rc = action_set_no_connection_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 35);
#endif
                    rc = action_send_channel_close (thread);

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
        case 55:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 36);
#endif
                    rc = action_send_channel_close_ok (thread);

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
        case 56:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 37);
#endif
                    rc = action_push_method_to_server (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 100);
#endif
                    rc = action_dialog_call_send_content_body_if_any_start (thread);

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
        case 57:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 107);
#endif
                    rc = action_unnamed_61 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
#endif
                    rc = action_dialog_call_close_the_connection_active_close (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 108);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 108);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 108);
#endif
                    rc = action_unnamed_62 (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
#endif
                    rc = action_dialog_call_close_the_connection_active_close (thread);

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
        case 4:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 47);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 47);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 47);
#endif
                    rc = action_report_connect_error (thread);

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
        case 5:
        case 6:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 49);
#endif
                    rc = action_report_connect_timeout (thread);

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
        case 7:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 109);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 109);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 109);
#endif
                    rc = action_unnamed_64 (thread);

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
        case 109:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_read_frame_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
#endif
                    rc = action_read_frame_payload (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
#endif
                    rc = action_check_frame_format (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 53);
#endif
                    rc = action_decode_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 110:
        case 112:
        case 123:
        case 125:
        case 147:
        case 149:
        case 160:
        case 162:
        case 171:
        case 173:
        case 135:
        case 137:
        case 101:
        case 103:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 73);
#endif
                    rc = action_notify_connection_of_exception (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
#endif
                    rc = action_report_socket_error (thread);

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
        case 111:
        case 124:
        case 148:
        case 161:
        case 172:
        case 136:
        case 102:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 73);
#endif
                    rc = action_notify_connection_of_exception (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 48);
#endif
                    rc = action_report_socket_timeout (thread);

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
        case 113:
        case 126:
        case 150:
        case 163:
        case 174:
        case 138:
        case 104:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 73);
#endif
                    rc = action_notify_connection_of_exception (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 41);
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

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 114:
        case 122:
        case 127:
        case 151:
        case 164:
        case 175:
        case 139:
        case 105:
        case 208:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 73);
#endif
                    rc = action_notify_connection_of_exception (thread);

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
        case 118:
        case 120:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_read_frame_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
#endif
                    rc = action_read_frame_payload (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
#endif
                    rc = action_check_frame_format (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 75);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 75);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 75);
#endif
                    rc = action_expect_method_frame (thread);

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

                case 5:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 119:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 53);
#endif
                    rc = action_decode_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 76);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 76);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 76);
#endif
                    rc = action_expect_connection_close_ok (thread);

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
        case 121:
        case 146:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 63);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 63);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 63);
#endif
                    rc = action_notify_connection_of_close_ok (thread);

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
        case 115:
        case 152:
        case 165:
        case 176:
        case 140:
        case 106:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 74);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 74);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 74);
#endif
                    rc = action_notify_connection_of_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 111);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 111);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 111);
#endif
                    rc = action_dialog_call_connection_closing_closing (thread);

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
        case 100:
        case 134:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 130:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 55);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 55);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 55);
#endif
                    rc = action_check_connection_method (thread);

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
        case 143:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 58);
#endif
                    rc = action_execute_connection_tune (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 59);
#endif
                    rc = action_notify_connection_of_tune (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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

                case 5:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 144:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 60);
#endif
                    rc = action_execute_connection_open_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
#endif
                    rc = action_notify_connection_of_open_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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

                case 5:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 145:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 62);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 62);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 62);
#endif
                    rc = action_notify_connection_of_passive_close (thread);

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
        case 131:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 56);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 56);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 56);
#endif
                    rc = action_check_channel_method (thread);

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
        case 155:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 64);
#endif
                    rc = action_execute_channel_open_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 156:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 65);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 65);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 65);
#endif
                    rc = action_execute_channel_close (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 157:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 66);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 66);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 66);
#endif
                    rc = action_execute_channel_close_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 158:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 67);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 67);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 67);
#endif
                    rc = action_execute_channel_flow (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 159:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 68);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 68);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 68);
#endif
                    rc = action_execute_channel_flow_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 168:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_read_frame_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
#endif
                    rc = action_read_frame_payload (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
#endif
                    rc = action_check_frame_format (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 69);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 69);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 69);
#endif
                    rc = action_expect_content_basic_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 70);
#endif
                    rc = action_check_if_basic_body_complete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 169:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 5);
#endif
                    rc = action_read_frame_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
#endif
                    rc = action_read_frame_payload (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
#endif
                    rc = action_check_frame_format (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 71);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 71);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 71);
#endif
                    rc = action_expect_content_basic_body (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 70);
#endif
                    rc = action_check_if_basic_body_complete (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_dialog_return (thread);

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
        case 170:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 53);
#endif
                    rc = action_decode_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 72);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 72);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 72);
#endif
                    rc = action_expect_exception_method (thread);

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
        case 132:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 112);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 112);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 112);
#endif
                    rc = action_dialog_call_read_basic_content_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
#endif
                    rc = action_execute_functional_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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

                case 5:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 133:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
#endif
                    rc = action_execute_functional_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 110);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 110);
#endif
                    rc = action_dialog_call_read_method_read_method (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
#endif
                    rc = action_check_method_class (thread);

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
        case 180:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 77);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 77);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 77);
#endif
                    rc = action_direct_connect_to_server (thread);

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
        case 187:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 78);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 78);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 78);
#endif
                    rc = action_send_direct_protocol_header (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 79);
#endif
                    rc = action_read_direct_response (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 80);
#endif
                    rc = action_check_direct_response (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 81);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 81);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 81);
#endif
                    rc = action_send_direct_lease (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 79);
#endif
                    rc = action_read_direct_response (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 80);
#endif
                    rc = action_check_direct_response (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 82);
#endif
                    rc = action_tell_session_direct_connection_ok (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 84);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 84);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 84);
#endif
                    rc = action_prepare_direct_connection (thread);

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
        case 195:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 85);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 85);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 85);
#endif
                    rc = action_deliver_direct_contents (thread);

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
        case 196:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 86);
#endif
                    rc = action_write_direct_content (thread);

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
        case 197:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 88);
#endif
                    rc = action_flush_direct_connection (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 87);
#endif
                    rc = action_tell_session_direct_closed (thread);

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
        case 198:
        case 190:
        case 181:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 88);
#endif
                    rc = action_flush_direct_connection (thread);

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
        case 199:
        case 182:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 91);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 91);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 91);
#endif
                    rc = action_report_direct_socket_error (thread);

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
        case 200:
        case 191:
        case 183:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
#endif
                    rc = action_report_direct_socket_timeout (thread);

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
        case 201:
        case 192:
        case 184:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 93);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 93);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 93);
#endif
                    rc = action_report_direct_connect_timeout (thread);

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
        case 188:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 89);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 89);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 89);
#endif
                    rc = action_report_direct_protocol_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 83);
#endif
                    rc = action_tell_session_direct_connection_failed (thread);

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
        case 189:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 90);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 90);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 90);
#endif
                    rc = action_report_direct_connect_error (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 83);
#endif
                    rc = action_tell_session_direct_connection_failed (thread);

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
        case 205:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 94);
#endif
                    rc = action_wait_for_heartbeat_interval (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 95);
#endif
                    rc = action_check_peer_liveness (thread);

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
        case 206:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 96);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 96);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 96);
#endif
                    rc = action_report_peer_slowing (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 94);
#endif
                    rc = action_wait_for_heartbeat_interval (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 95);
#endif
                    rc = action_check_peer_liveness (thread);

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
        case 207:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 97);
#endif
                    rc = action_shut_down_agent (thread);

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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 98);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 98);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 98);
#endif
                    rc = action_report_peer_stopped (thread);

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
        case 209:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 113);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 113);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
                    if (thread->animate)
                        animate_action (thread, 113);
#endif
                    rc = action_unnamed_113 (thread);

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
demo_client_agent_catcher (smt_thread_t *thread)
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
demo_client_agent_connection_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_client_agent",
                 connection_thread_name);

#endif
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        tcb->connection_thread = NULL;

        //  If we shut down due to an exception on the connection, we
        //  may still have a channel open.  If so, signal demo_client_session
        //  that the channel is closed
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            *tcb->channel->alive = FALSE;
            *tcb->channel->reply_code = tcb->reply_code;
            strcpy (tcb->channel->reply_text, tcb->reply_text);
            smt_method_send (
                tcb->channel->method_queue,
                SMT_NULL_EVENT,
                SMT_PRIORITY_NORMAL,
                DEMO_CLIENT_REPLY_CLOSED,
                NULL,
                NULL);
        }
        //  Signal demo_client_connection that the connection is closed
        if (tcb->conn_alive) {
            *tcb->conn_alive = FALSE;
            *tcb->conn_reply_code = tcb->reply_code;
            strcpy (tcb->conn_reply_text, tcb->reply_text);
            smt_method_send (
                tcb->method_queue,
                SMT_NULL_EVENT,
                SMT_PRIORITY_NORMAL,
                DEMO_CLIENT_REPLY_CLOSED,
                NULL,
                NULL);
        }
        //  Free resources
        demo_client_method_unlink (&tcb->method_in);
        demo_client_method_unlink (&tcb->method_out);
        ipr_bucket_unlink       (&tcb->bucket_in);
        icl_longstr_destroy     (&tcb->auth_data);
        smt_socket_destroy      (&tcb->socket);
        demo_client_channel_unlink (&tcb->channel);
        demo_client_channel_table_destroy (&tcb->channels);
        smt_method_queue_unlink (&tcb->method_queue);
        //  Deregister signal handlers, since they may still be registered
        smt_signal_deregister (SIGINT);
        smt_signal_deregister (SIGTERM);
    
#undef  tcb
}
    if (--((demo_client_agent_connection_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_client_agent_input_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_client_agent",
                 input_thread_name);

#endif
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        tcb->input_thread = NULL;
    
#undef  tcb
}
    if (--((demo_client_agent_connection_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_client_agent_direct_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_client_agent",
                 direct_thread_name);

#endif
{
#define tcb ((demo_client_agent_direct_context_t *) thread->context)

        ipr_bucket_unlink       (&tcb->bucket_in);
        smt_socket_destroy      (&tcb->socket);
        smt_method_queue_unlink (&tcb->method_queue);
    
#undef  tcb
}
    if (--((demo_client_agent_direct_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_client_agent_heartbeat_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_CLIENT_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_client_agent",
                 heartbeat_thread_name);

#endif
{
#define tcb ((demo_client_agent_connection_context_t *) thread->context)

        tcb->heartbeat_thread = NULL;

        //  Deregister signal handlers
        smt_signal_deregister (SIGINT);
        smt_signal_deregister (SIGTERM);

    
#undef  tcb
}
    if (--((demo_client_agent_connection_context_t *) thread->context)->links == 0)
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
    
    if (thread->place < 98)
      {
        *thread_type = 1;
        if (thread->place < 16)
          {
            *state_id = 1;
            switch (thread->place) {
                case 3: *event_id = 1;   *nextstate_id = 2;   break;
                case 4: *event_id = 24;   break;
                case 5: *event_id = 25;   break;
                case 6: *event_id = 5;   break;
                case 7: *event_id = 6;   break;
                case 8: *event_id = 26;   break;
                case 9: *event_id = 27;   break;
                case 10: *event_id = 28;   break;
                case 11: *event_id = 32;   break;
                case 12: *event_id = 34;   break;
                case 13: *event_id = 35;   break;
                case 14: *event_id = 36;   *nextstate_id = 1;   break;
                case 15: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 36)
          {
            *state_id = 2;
            switch (thread->place) {
                case 17: *event_id = 2;   *nextstate_id = 2;   break;
                case 18: *event_id = 3;   break;
                case 19: *event_id = 4;   break;
                case 20: *event_id = 5;   break;
                case 21: *event_id = 6;   break;
                case 22: *event_id = 7;   *nextstate_id = 2;   break;
                case 23: *event_id = 8;   *nextstate_id = 3;   break;
                case 24: *event_id = 41;   break;
                case 25: *event_id = 46;   break;
                case 26: *event_id = 24;   break;
                case 27: *event_id = 25;   break;
                case 28: *event_id = 26;   break;
                case 29: *event_id = 27;   break;
                case 30: *event_id = 28;   break;
                case 31: *event_id = 32;   break;
                case 32: *event_id = 34;   break;
                case 33: *event_id = 35;   break;
                case 34: *event_id = 36;   *nextstate_id = 2;   break;
                case 35: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 68)
          {
            *state_id = 3;
            switch (thread->place) {
                case 37: *event_id = 9;   *nextstate_id = 3;   break;
                case 38: *event_id = 10;   *nextstate_id = 3;   break;
                case 39: *event_id = 11;   *nextstate_id = 3;   break;
                case 40: *event_id = 12;   *nextstate_id = 3;   break;
                case 41: *event_id = 13;   *nextstate_id = 3;   break;
                case 42: *event_id = 14;   *nextstate_id = 3;   break;
                case 43: *event_id = 15;   *nextstate_id = 3;   break;
                case 44: *event_id = 16;   *nextstate_id = 3;   break;
                case 45: *event_id = 17;   *nextstate_id = 3;   break;
                case 46: *event_id = 18;   *nextstate_id = 3;   break;
                case 47: *event_id = 19;   *nextstate_id = 3;   break;
                case 48: *event_id = 20;   *nextstate_id = 3;   break;
                case 49: *event_id = 38;   *nextstate_id = 3;   break;
                case 50: *event_id = 39;   *nextstate_id = 3;   break;
                case 51: *event_id = 40;   *nextstate_id = 3;   break;
                case 52: *event_id = 41;   break;
                case 53: *event_id = 42;   *nextstate_id = 3;   break;
                case 54: *event_id = 43;   *nextstate_id = 3;   break;
                case 55: *event_id = 44;   *nextstate_id = 3;   break;
                case 56: *event_id = 45;   *nextstate_id = 3;   break;
                case 57: *event_id = 46;   break;
                case 58: *event_id = 24;   break;
                case 59: *event_id = 25;   break;
                case 60: *event_id = 26;   break;
                case 61: *event_id = 27;   break;
                case 62: *event_id = 28;   break;
                case 63: *event_id = 32;   break;
                case 64: *event_id = 34;   break;
                case 65: *event_id = 35;   break;
                case 66: *event_id = 36;   *nextstate_id = 3;   break;
                case 67: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 82)
          {
            *state_id = 4;
            switch (thread->place) {
                case 69: *event_id = 21;   *nextstate_id = 4;   break;
                case 70: *event_id = 22;   *nextstate_id = 4;   break;
                case 71: *event_id = 23;   *nextstate_id = 4;   break;
                case 72: *event_id = 24;   break;
                case 73: *event_id = 25;   break;
                case 74: *event_id = 26;   break;
                case 75: *event_id = 27;   break;
                case 76: *event_id = 28;   break;
                case 77: *event_id = 32;   break;
                case 78: *event_id = 34;   break;
                case 79: *event_id = 35;   break;
                case 80: *event_id = 36;   *nextstate_id = 4;   break;
                case 81: *event_id = 37;   break;
            }
          }
        else
          {
            *state_id = 5;
            switch (thread->place) {
                case 83: *event_id = 29;   *nextstate_id = 5;   break;
                case 84: *event_id = 30;   *nextstate_id = 5;   break;
                case 85: *event_id = 31;   *nextstate_id = 5;   break;
                case 86: *event_id = 32;   *nextstate_id = 5;   break;
                case 87: *event_id = 28;   *nextstate_id = 5;   break;
                case 88: *event_id = 33;   *nextstate_id = 5;   break;
                case 89: *event_id = 24;   break;
                case 90: *event_id = 25;   break;
                case 91: *event_id = 26;   break;
                case 92: *event_id = 27;   break;
                case 93: *event_id = 34;   break;
                case 94: *event_id = 35;   break;
                case 95: *event_id = 36;   *nextstate_id = 5;   break;
                case 96: *event_id = 37;   break;
                default:
                                     *nextstate_id = 5;   break;
            }
          }
      }
    else
    if (thread->place < 178)
      {
        *thread_type = 2;
        if (thread->place < 108)
          {
            *state_id = 6;
            switch (thread->place) {
                case 100: *event_id = 2;   *nextstate_id = 9;   break;
                case 101: *event_id = 24;   break;
                case 102: *event_id = 25;   break;
                case 103: *event_id = 26;   break;
                case 104: *event_id = 27;   break;
                case 105: *event_id = 34;   break;
                case 106: *event_id = 32;   break;
                case 107: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 117)
          {
            *state_id = 7;
            switch (thread->place) {
                case 109: *event_id = 47;   *nextstate_id = 7;   break;
                case 110: *event_id = 24;   break;
                case 111: *event_id = 25;   break;
                case 112: *event_id = 26;   break;
                case 113: *event_id = 27;   break;
                case 114: *event_id = 34;   break;
                case 115: *event_id = 32;   break;
                case 116: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 129)
          {
            *state_id = 8;
            switch (thread->place) {
                case 118: *event_id = 48;   *nextstate_id = 8;   break;
                case 119: *event_id = 49;   *nextstate_id = 8;   break;
                case 120: *event_id = 50;   *nextstate_id = 8;   break;
                case 121: *event_id = 51;   break;
                case 122: *event_id = 32;   break;
                case 123: *event_id = 24;   break;
                case 124: *event_id = 25;   break;
                case 125: *event_id = 26;   break;
                case 126: *event_id = 27;   break;
                case 127: *event_id = 34;   break;
                case 128: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 142)
          {
            *state_id = 9;
            switch (thread->place) {
                case 130: *event_id = 52;   *nextstate_id = 10;   break;
                case 131: *event_id = 55;   *nextstate_id = 11;   break;
                case 132: *event_id = 61;   *nextstate_id = 9;   break;
                case 133: *event_id = 64;   *nextstate_id = 9;   break;
                case 134: *event_id = 9;   *nextstate_id = 9;   break;
                case 135: *event_id = 24;   break;
                case 136: *event_id = 25;   break;
                case 137: *event_id = 26;   break;
                case 138: *event_id = 27;   break;
                case 139: *event_id = 34;   break;
                case 140: *event_id = 32;   break;
                case 141: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 154)
          {
            *state_id = 10;
            switch (thread->place) {
                case 143: *event_id = 53;   *nextstate_id = 9;   break;
                case 144: *event_id = 54;   *nextstate_id = 9;   break;
                case 145: *event_id = 28;   break;
                case 146: *event_id = 51;   break;
                case 147: *event_id = 24;   break;
                case 148: *event_id = 25;   break;
                case 149: *event_id = 26;   break;
                case 150: *event_id = 27;   break;
                case 151: *event_id = 34;   break;
                case 152: *event_id = 32;   break;
                case 153: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 167)
          {
            *state_id = 11;
            switch (thread->place) {
                case 155: *event_id = 56;   *nextstate_id = 9;   break;
                case 156: *event_id = 57;   *nextstate_id = 9;   break;
                case 157: *event_id = 58;   *nextstate_id = 9;   break;
                case 158: *event_id = 59;   *nextstate_id = 9;   break;
                case 159: *event_id = 60;   *nextstate_id = 9;   break;
                case 160: *event_id = 24;   break;
                case 161: *event_id = 25;   break;
                case 162: *event_id = 26;   break;
                case 163: *event_id = 27;   break;
                case 164: *event_id = 34;   break;
                case 165: *event_id = 32;   break;
                case 166: *event_id = 37;   break;
            }
          }
        else
          {
            *state_id = 12;
            switch (thread->place) {
                case 168: *event_id = 62;   *nextstate_id = 12;   break;
                case 169: *event_id = 63;   *nextstate_id = 12;   break;
                case 170: *event_id = 49;   *nextstate_id = 9;   break;
                case 171: *event_id = 24;   break;
                case 172: *event_id = 25;   break;
                case 173: *event_id = 26;   break;
                case 174: *event_id = 27;   break;
                case 175: *event_id = 34;   break;
                case 176: *event_id = 32;   break;
                case 177: *event_id = 37;   break;
            }
          }
      }
    else
    if (thread->place < 203)
      {
        *thread_type = 3;
        if (thread->place < 186)
          {
            *state_id = 13;
            switch (thread->place) {
                case 180: *event_id = 1;   *nextstate_id = 14;   break;
                case 181: *event_id = 26;   break;
                case 182: *event_id = 24;   break;
                case 183: *event_id = 25;   break;
                case 184: *event_id = 5;   break;
                case 185: *event_id = 37;   break;
            }
          }
        else
        if (thread->place < 194)
          {
            *state_id = 14;
            switch (thread->place) {
                case 187: *event_id = 2;   *nextstate_id = 15;   break;
                case 188: *event_id = 68;   break;
                case 189: *event_id = 24;   break;
                case 190: *event_id = 26;   break;
                case 191: *event_id = 25;   break;
                case 192: *event_id = 5;   break;
                case 193: *event_id = 37;   break;
            }
          }
        else
          {
            *state_id = 15;
            switch (thread->place) {
                case 195: *event_id = 65;   *nextstate_id = 15;   break;
                case 196: *event_id = 66;   *nextstate_id = 15;   break;
                case 197: *event_id = 67;   break;
                case 198: *event_id = 26;   break;
                case 199: *event_id = 24;   break;
                case 200: *event_id = 25;   break;
                case 201: *event_id = 5;   break;
                case 202: *event_id = 37;   break;
            }
          }
      }
    else
      {
        *thread_type = 4;
          {
            *state_id = 16;
            switch (thread->place) {
                case 205: *event_id = 69;   *nextstate_id = 16;   break;
                case 206: *event_id = 70;   *nextstate_id = 16;   break;
                case 207: *event_id = 71;   break;
                case 208: *event_id = 27;   break;
                case 209: *event_id = 6;   *nextstate_id = 16;   break;
                case 210: *event_id = 37;   break;
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
    icl_console_print ("demo_client_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_CLIENT_AGENT))

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
            "demo_client_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "demo_client_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "demo_client_agent",
              thread_name [find_thread_type (thread->place)],
              action_name [action_id]);
}

static int
find_thread_type (smt_place_t place)
{
    int
        thread_type = 0;
        
    if (place < 98)
        thread_type = 1;
    else
    if (place < 178)
        thread_type = 2;
    else
    if (place < 203)
        thread_type = 3;
    else
        thread_type = 4;
    return thread_type;
}

#endif
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_CLIENT_AGENT)            ||  defined (DEMO_CLIENT_AGENT_TRACE))
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
                                       "demo_client_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "demo_client_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "demo_client_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_set_no_connection_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SET_NO_CONNECTION_ERROR))
    icl_stats_inc ("demo_client_agent_set_no_connection_error", &s_demo_client_agent_set_no_connection_error_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        tcb->reply_code = 100;          //  No error
        icl_shortstr_cpy (tcb->reply_text, "No error");
    
}
#undef  tcb
    return _rc;
}

static int
action_start_setup_timer (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_SETUP_TIMER))
    icl_stats_inc ("demo_client_agent_start_setup_timer", &s_demo_client_agent_start_setup_timer_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Schedule a timeout event to protect against misbehaving servers
        tcb->setup_timer = smt_timer_request_delay (thread, tcb->timeout,
            setup_timeout_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_connect_to_server (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CONNECT_TO_SERVER))
    icl_stats_inc ("demo_client_agent_connect_to_server", &s_demo_client_agent_connect_to_server_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        icl_shortstr_t
            host;                       //  Copy of server address
        char
            *port;                      //  Port to connect to
        int
            rc = SMT_OK,
            new_size;                   //  New buffer size

        //  We extract the port number from the host name if specified
        icl_shortstr_cpy (host, tcb->host);
        port = strchr (host, ':');
        if (port)
            *port++ = 0;                //  Split the string
        else {
            port = "7654";
            //  Save port in tcb->host for reporting
            icl_shortstr_cat (tcb->host, ":");
            icl_shortstr_cat (tcb->host, port);
        }
        tcb->socket = smt_socket_new ();
        if (tcb->socket) {
            //  Set Nagle's algorithm on or off
            rc = smt_socket_set_nodelay (tcb->socket,
                demo_client_config_tcp_nodelay (demo_client_config));
            if (rc && tcb->verbose)
                icl_console_print ("W: could not set TCP_NODELAY on socket");

            //  Set socket buffers if wanted
            new_size = demo_client_config_tcp_rcvbuf (demo_client_config);
            if (new_size)
                rc = smt_socket_set_rcvbuf (tcb->socket, new_size, NULL);
            if (rc && tcb->verbose)
                icl_console_print ("W: could not set TCB_RCVBUF on socket");

            new_size = demo_client_config_tcp_sndbuf (demo_client_config);
            if (new_size)
                rc = smt_socket_set_sndbuf (tcb->socket, new_size, NULL);
            if (rc && tcb->verbose)
                icl_console_print ("W: could not set TCP_SNDBUF on socket");

            //  Connect to server
            //  Error return will be caught as an exception event
            rc = smt_socket_connect (tcb->socket, thread, 0, host, port,
                ok_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_send_protocol_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_PROTOCOL_HEADER))
    icl_stats_inc ("demo_client_agent_send_protocol_header", &s_demo_client_agent_send_protocol_header_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        memcpy (tcb->protocol_header, "DEMO\x00\x00\x09\x01", 8);
        smt_socket_request_write (
            thread, tcb->socket, 0, 8, tcb->protocol_header, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_frame_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_FRAME_HEADER))
    icl_stats_inc ("demo_client_agent_read_frame_header", &s_demo_client_agent_read_frame_header_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        ipr_bucket_unlink (&tcb->bucket_in);
        s_socket_read_bucket (thread, &tcb->bucket_in, ASL_HEADER_SIZE);
        //  If needed, fake a slow network by pausing whole thread for 1msec
        if (tcb->fakeslow)
            apr_sleep (1000);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_protocol_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_PROTOCOL_HEADER))
    icl_stats_inc ("demo_client_agent_check_protocol_header", &s_demo_client_agent_check_protocol_header_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  The "read frame header" action will have read the 7 byte frame
        //  header.  Inspect this to see if it looks like a valid frame header
        if (memcmp (tcb->bucket_in->data, "\001\000\000\000", 4) != 0) {
            //  If not, look at the first four bytes and see if they match
            //  our protocol
            memcpy (tcb->protocol_header, tcb->bucket_in->data, 7);
            if (memcmp (tcb->protocol_header, "DEMO", 4) == 0)
                //  Protocol does match, server rejected our version
                smt_thread_raise_exception (thread, version_mismatch_event);
            else
                //  Protocol does not match
                smt_thread_raise_exception (thread, bad_protocol_event);
        }
        //  If we get here, then we have what should be a valid frame header,
        //  so we can proceed with decoding it as normal
    
}
#undef  tcb
    return _rc;
}

static int
action_read_protocol_version (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_PROTOCOL_VERSION))
    icl_stats_inc ("demo_client_agent_read_protocol_version", &s_demo_client_agent_read_protocol_version_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  We need to read the extra byte that the server has sent, since
        //  "read frame header" did not read it for us
        smt_socket_request_read (thread, tcb->socket, 0, 1, 1,
            &tcb->protocol_header[7], SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_version_mismatch (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_VERSION_MISMATCH))
    icl_stats_inc ("demo_client_agent_report_version_mismatch", &s_demo_client_agent_report_version_mismatch_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print (
                "E: version mismatch: server at '%s' sent (%u, %u, %u, %u)",
                tcb->host,
                tcb->protocol_header [4],
                tcb->protocol_header [5],
                tcb->protocol_header [6],
                tcb->protocol_header [7]);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_bad_protocol (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_BAD_PROTOCOL))
    icl_stats_inc ("demo_client_agent_report_bad_protocol", &s_demo_client_agent_report_bad_protocol_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print (
                "E: server at '%s' sent invalid protocol header", tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_start (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_START))
    icl_stats_inc ("demo_client_agent_execute_connection_start", &s_demo_client_agent_execute_connection_start_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_CLIENT_CONNECTION, DEMO_CLIENT_CONNECTION_START);
    
}
#undef  tcb
    return _rc;
}

static int
action_start_input_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_INPUT_THREAD))
    icl_stats_inc ("demo_client_agent_start_input_thread", &s_demo_client_agent_start_input_thread_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_thread_t
            *input_thread;

        //  Start input thread
        input_thread = demo_client_agent_input_thread_new (thread);

        tcb->input_thread = input_thread;

        //  This thread needs to watch the input thread so that it
        //  doesn't get swamped by incoming methods between socket polls.
        smt_thread_watch (thread, input_thread);

        smt_thread_wake   ( input_thread);
        smt_thread_unlink (&input_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_start_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_START_OK))
    icl_stats_inc ("demo_client_agent_send_connection_start_ok", &s_demo_client_agent_send_connection_start_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        icl_longstr_t
            *properties;                //  Connection properties
        asl_field_list_t
            *field_list;                //  List for connection properties
        icl_shortstr_t
            system;                     //  System (platform) name

        field_list = asl_field_list_new (NULL);
        asl_field_new_string (field_list, "product", "ASL Demo Client");
        asl_field_new_string (field_list, "version", VERSION);
        asl_field_new_string (field_list, "copyright", COPYRIGHT);
        asl_field_new_string (field_list, "platform", icl_system_name (system));
        asl_field_new_string (field_list, "information", BUILDMODEL);
        asl_field_new_string (field_list, "instance", tcb->instance);
        properties = asl_field_list_flatten (field_list);
        asl_field_list_destroy (&field_list);

        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_connection_start_ok (
            properties, "PLAIN", tcb->auth_data, "en_US");
        s_send_method_out (thread, 0, NULL);

        icl_longstr_destroy (&properties);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_tune_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_TUNE_OK))
    icl_stats_inc ("demo_client_agent_send_connection_tune_ok", &s_demo_client_agent_send_connection_tune_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_connection_tune_ok (
            1,                          //  We only support a single channel
            tcb->frame_max,
            tcb->heartbeat);
        s_send_method_out (thread, 0, NULL);
    
}
#undef  tcb
    return _rc;
}

static int
action_start_heartbeat_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_HEARTBEAT_THREAD))
    icl_stats_inc ("demo_client_agent_start_heartbeat_thread", &s_demo_client_agent_start_heartbeat_thread_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_thread_t
            *heartbeat_thread;

        //  Deregister signal handlers before starting heartbeat thread,
        //  it will register it's own handlers.  If heartbeating is disabled
        //  we just rely on the default SMT signal handling, since we don't
        //  have an external thread to interrupt us.  (Could be fixed later)
        smt_signal_deregister (SIGINT);
        smt_signal_deregister (SIGTERM);
        //  If client is heartbeat enabled, start heartbeat thread
        if (tcb->heartbeat) {
            heartbeat_thread = demo_client_agent_heartbeat_thread_new (thread);

            tcb->heartbeat_thread = heartbeat_thread;

            smt_thread_wake   ( heartbeat_thread);
            smt_thread_unlink (&heartbeat_thread);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_start_heartbeat_timer (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_START_HEARTBEAT_TIMER))
    icl_stats_inc ("demo_client_agent_start_heartbeat_timer", &s_demo_client_agent_start_heartbeat_timer_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  If client is heartbeat enabled, (re)start outgoing heartbeat timer
        if (tcb->heartbeat)
            smt_timer_request_delay (thread, tcb->heartbeat * 1000 * 1000,
                heartbeat_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_cancel_setup_timer (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CANCEL_SETUP_TIMER))
    icl_stats_inc ("demo_client_agent_cancel_setup_timer", &s_demo_client_agent_cancel_setup_timer_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_timer_request_destroy (&tcb->setup_timer);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_OPEN))
    icl_stats_inc ("demo_client_agent_send_connection_open", &s_demo_client_agent_send_connection_open_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_connection_open (
            tcb->virtual_host, NULL, FALSE);
        s_send_method_out (thread, 0, NULL);
    
}
#undef  tcb
    return _rc;
}

static int
action_tell_session_connection_almost (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_CONNECTION_ALMOST))
    icl_stats_inc ("demo_client_agent_tell_session_connection_almost", &s_demo_client_agent_tell_session_connection_almost_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_method_t
            *method;

        method = demo_client_method_link (tcb->method_in);
        smt_method_send (
            tcb->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_ALMOST,
            method,
            s_method_destructor);
    
}
#undef  tcb
    return _rc;
}

static int
action_tell_session_connection_ready (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_CONNECTION_READY))
    icl_stats_inc ("demo_client_agent_tell_session_connection_ready", &s_demo_client_agent_tell_session_connection_ready_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_method_t
            *method;

        method = demo_client_method_link (tcb->method_in);
        smt_method_send (
            tcb->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_METHOD,
            method,
            s_method_destructor);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_flow (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_FLOW))
    icl_stats_inc ("demo_client_agent_send_channel_flow", &s_demo_client_agent_send_channel_flow_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (channel_flow_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_channel_flow (
                    channel_flow_m->active);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_flow_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_FLOW_OK))
    icl_stats_inc ("demo_client_agent_send_channel_flow_ok", &s_demo_client_agent_send_channel_flow_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (channel_flow_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_channel_flow_ok (
                    channel_flow_ok_m->active);
            s_send_method_out (thread, 1, NULL);

            //  Signal demo_client_session that async method has been sent
            if (TRUE)
                smt_method_send (
                    tcb->channel->method_queue,
                    SMT_NULL_EVENT,
                    SMT_PRIORITY_NORMAL,
                    DEMO_CLIENT_REPLY_INTERN,
                    NULL,               //  No data for method
                    NULL);              //  No destructor neccessary
        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_exchange_declare (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_EXCHANGE_DECLARE))
    icl_stats_inc ("demo_client_agent_send_exchange_declare", &s_demo_client_agent_send_exchange_declare_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (exchange_declare_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_exchange_declare (
                    exchange_declare_m->exchange,
                    exchange_declare_m->type,
                    exchange_declare_m->passive,
                    exchange_declare_m->durable,
                    exchange_declare_m->auto_delete);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_exchange_delete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_EXCHANGE_DELETE))
    icl_stats_inc ("demo_client_agent_send_exchange_delete", &s_demo_client_agent_send_exchange_delete_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (exchange_delete_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_exchange_delete (
                    exchange_delete_m->exchange,
                    exchange_delete_m->if_unused);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_declare (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_DECLARE))
    icl_stats_inc ("demo_client_agent_send_queue_declare", &s_demo_client_agent_send_queue_declare_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_declare_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_queue_declare (
                    queue_declare_m->queue,
                    queue_declare_m->passive,
                    queue_declare_m->durable,
                    queue_declare_m->exclusive,
                    queue_declare_m->auto_delete);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_bind (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_BIND))
    icl_stats_inc ("demo_client_agent_send_queue_bind", &s_demo_client_agent_send_queue_bind_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_bind_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_queue_bind (
                    queue_bind_m->queue,
                    queue_bind_m->exchange,
                    queue_bind_m->arguments);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_purge (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_PURGE))
    icl_stats_inc ("demo_client_agent_send_queue_purge", &s_demo_client_agent_send_queue_purge_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_purge_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_queue_purge (
                    queue_purge_m->queue);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_delete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_QUEUE_DELETE))
    icl_stats_inc ("demo_client_agent_send_queue_delete", &s_demo_client_agent_send_queue_delete_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_delete_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_queue_delete (
                    queue_delete_m->queue,
                    queue_delete_m->if_unused,
                    queue_delete_m->if_empty);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_consume (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_CONSUME))
    icl_stats_inc ("demo_client_agent_send_basic_consume", &s_demo_client_agent_send_basic_consume_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_consume_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_basic_consume (
                    basic_consume_m->queue);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_cancel (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_CANCEL))
    icl_stats_inc ("demo_client_agent_send_basic_cancel", &s_demo_client_agent_send_basic_cancel_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_cancel_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_basic_cancel (
                    basic_cancel_m->consumer_tag);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_publish (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_PUBLISH))
    icl_stats_inc ("demo_client_agent_send_basic_publish", &s_demo_client_agent_send_basic_publish_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_publish_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_basic_publish (
                    basic_publish_m->exchange,
                    basic_publish_m->routing_key);
            tcb->method_out->content =
                demo_content_basic_link (
                    basic_publish_m->content
                );
            assert (tcb->method_out->content);
            s_send_method_out (thread, 1, NULL);

            //  Signal demo_client_session that async method has been sent
            if (TRUE)
                smt_method_send (
                    tcb->channel->method_queue,
                    SMT_NULL_EVENT,
                    SMT_PRIORITY_NORMAL,
                    DEMO_CLIENT_REPLY_INTERN,
                    NULL,               //  No data for method
                    NULL);              //  No destructor neccessary
        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_get (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_BASIC_GET))
    icl_stats_inc ("demo_client_agent_send_basic_get", &s_demo_client_agent_send_basic_get_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_get_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_basic_get (
                    basic_get_m->queue);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_put (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_PUT))
    icl_stats_inc ("demo_client_agent_send_direct_put", &s_demo_client_agent_send_direct_put_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (direct_put_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_direct_put (
                    direct_put_m->sink);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_get (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_GET))
    icl_stats_inc ("demo_client_agent_send_direct_get", &s_demo_client_agent_send_direct_get_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (direct_get_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_client_method_new_direct_get (
                    direct_get_m->feed);
            s_send_method_out (thread, 1, NULL);

        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_OPEN))
    icl_stats_inc ("demo_client_agent_send_channel_open", &s_demo_client_agent_send_channel_open_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is CLOSED
        assert (channel_open_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_CLOSED) {
            //  Create the channel
            tcb->channel = demo_client_channel_new (
                tcb->channels, 1,
                channel_open_m->method_queue,
                channel_open_m->alive,
                channel_open_m->reply_code,
                channel_open_m->reply_text);
            //  Channel is now OPENING
            tcb->channel_state = ASL_CHANNEL_OPENING;
            //  Send the channel open
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out = demo_client_method_new_channel_open (NULL);
            s_send_method_out (thread, 1, NULL);
        }
        else
            //  Channel is not CLOSED
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is already open",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_OPEN);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_CLOSE))
    icl_stats_inc ("demo_client_agent_send_channel_close", &s_demo_client_agent_send_channel_close_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Assert channel is OPEN
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Send the method
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out = demo_client_method_new_channel_close (
                tcb->reply_code, tcb->reply_text, 0, 0);
            s_send_method_out (thread, 1, NULL);
            //  Channel is now CLOSING
            tcb->channel_state = ASL_CHANNEL_CLOSING;
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_CLOSE);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CHANNEL_CLOSE_OK))
    icl_stats_inc ("demo_client_agent_send_channel_close_ok", &s_demo_client_agent_send_channel_close_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Assert channel is CLOSING
        assert (tcb->channel_state == ASL_CHANNEL_CLOSING);
        //  Send the channel close ok
        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_channel_close_ok ();
        s_send_method_out (thread, 1, NULL);
        //  Signal demo_client_session that the channel is closed
        *tcb->channel->alive = FALSE;
        *tcb->channel->reply_code = tcb->reply_code;
        strcpy (tcb->channel->reply_text, tcb->reply_text);
        smt_method_send (
            tcb->channel->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_CLOSED,
            NULL,
            NULL);
        //  Destroy the channel object
        demo_client_channel_destroy (&tcb->channel);
        //  Channel is now CLOSED
        tcb->channel_state = ASL_CHANNEL_CLOSED;
    
}
#undef  tcb
    return _rc;
}

static int
action_push_method_to_server (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_PUSH_METHOD_TO_SERVER))
    icl_stats_inc ("demo_client_agent_push_method_to_server", &s_demo_client_agent_push_method_to_server_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (push_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Send method as-is, with no reconstruction
            demo_client_method_unlink (&tcb->method_out);
            tcb->method_out = demo_client_method_link (push_m->method);
            s_send_method_out (thread, 1, NULL);
        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_content_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONTENT_HEADER))
    icl_stats_inc ("demo_client_agent_send_content_header", &s_demo_client_agent_send_content_header_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->content) {
            //  Chronometer on content sent to wire
            demo_content_basic_chrono_add (tcb->method_out->content);
            if (tcb->method_out->class_id == DEMO_CLIENT_BASIC) {
                payload = demo_content_basic_replay_header (
                    tcb->method_out->content);
                s_write_payload (thread, 1, ASL_FRAME_HEADER, &payload);
                demo_content_basic_set_reader (
                    tcb->method_out->content, &tcb->reader, tcb->frame_max);
            }
        }
        else
            smt_thread_raise_exception (thread, finished_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_content_body_frame (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONTENT_BODY_FRAME))
    icl_stats_inc ("demo_client_agent_send_content_body_frame", &s_demo_client_agent_send_content_body_frame_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->class_id == DEMO_CLIENT_BASIC)
            payload = demo_content_basic_replay_body (
                tcb->method_out->content, &tcb->reader);
        if (payload) {
            s_write_payload (thread, 1, ASL_FRAME_BODY, &payload);
            smt_thread_set_next_event (thread, continue_event);
        }
        else
            smt_thread_set_next_event (thread, finished_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_shut_down_slave_threads (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SHUT_DOWN_SLAVE_THREADS))
    icl_stats_inc ("demo_client_agent_shut_down_slave_threads", &s_demo_client_agent_shut_down_slave_threads_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Shut down other two threads if they are still active.
        if (tcb->input_thread)
            smt_thread_shut_down (tcb->input_thread);
        if (tcb->heartbeat_thread)
            smt_thread_shut_down (tcb->heartbeat_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_smt_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SMT_ERROR))
    icl_stats_inc ("demo_client_agent_report_smt_error", &s_demo_client_agent_report_smt_error_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print ("E: SMT error: %s", smt_thread_error (thread));
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_CLOSE))
    icl_stats_inc ("demo_client_agent_send_connection_close", &s_demo_client_agent_send_connection_close_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Schedule a timeout to protect against misbehaving servers
        smt_timer_request_delay (thread, tcb->timeout, close_timeout_event);
        //  Send connection close
        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_connection_close (
            tcb->reply_code, tcb->reply_text, 0, 0);
        s_send_method_out (thread, 0, NULL);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_CLOSE))
    icl_stats_inc ("demo_client_agent_execute_connection_close", &s_demo_client_agent_execute_connection_close_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        ;                               //  Proceed
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_CONNECTION_CLOSE_OK))
    icl_stats_inc ("demo_client_agent_send_connection_close_ok", &s_demo_client_agent_send_connection_close_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_method_unlink (&tcb->method_out);
        tcb->method_out = demo_client_method_new_connection_close_ok ();
        s_send_method_out (thread, 0, NULL);
    
}
#undef  tcb
    return _rc;
}

static int
action_flush_the_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_FLUSH_THE_CONNECTION))
    icl_stats_inc ("demo_client_agent_flush_the_connection", &s_demo_client_agent_flush_the_connection_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->socket)
            smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_socket_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SOCKET_ERROR))
    icl_stats_inc ("demo_client_agent_report_socket_error", &s_demo_client_agent_report_socket_error_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print ("E: %s, connection to server lost (%s)",
                smt_thread_error (thread), tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_connect_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_CONNECT_ERROR))
    icl_stats_inc ("demo_client_agent_report_connect_error", &s_demo_client_agent_report_connect_error_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print ("E: connection to server failed: %s (%s)",
                smt_thread_error (thread), tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_socket_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_SOCKET_TIMEOUT))
    icl_stats_inc ("demo_client_agent_report_socket_timeout", &s_demo_client_agent_report_socket_timeout_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print (
                "E: socket timeout, connection to server lost (%s)",
                tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_connect_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_CONNECT_TIMEOUT))
    icl_stats_inc ("demo_client_agent_report_connect_timeout", &s_demo_client_agent_report_connect_timeout_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->verbose)
            icl_console_print (
                "E: connection to server timed out (%s)", tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_heartbeat_frame (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_HEARTBEAT_FRAME))
    icl_stats_inc ("demo_client_agent_send_heartbeat_frame", &s_demo_client_agent_send_heartbeat_frame_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        ipr_bucket_t
           *bucket;                     //  Header bucket
        byte
            *bucket_ptr;

        bucket = ipr_bucket_new (ASL_HEADER_SIZE + 1);
        bucket_ptr = bucket->data;
        PUT_OCTET (bucket_ptr, ASL_FRAME_HEARTBEAT);
        PUT_SHORT (bucket_ptr, 0);
        PUT_LONG  (bucket_ptr, 0);
        PUT_OCTET (bucket_ptr, ASL_FRAME_END);
        bucket->cur_size = bucket_ptr - bucket->data;
        s_socket_write_bucket (thread, bucket);
        ipr_bucket_unlink (&bucket);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_frame_payload (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_FRAME_PAYLOAD))
    icl_stats_inc ("demo_client_agent_read_frame_payload", &s_demo_client_agent_read_frame_payload_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        byte
            *bucket_ptr;
        icl_shortstr_t
            reply_text;

        bucket_ptr = tcb->bucket_in->data;
        tcb->frame_header.type    =  bucket_ptr [0];
        tcb->frame_header.channel = (bucket_ptr [1] << 8)  +  bucket_ptr [2];
        tcb->frame_header.size    = (bucket_ptr [3] << 24) + (bucket_ptr [4] << 16)
                                  + (bucket_ptr [5] << 8)  +  bucket_ptr [6];

        if (tcb->frame_header.size > tcb->frame_max) {
            icl_shortstr_fmt (reply_text, "Peer sent oversized frame (%d > %d)",
                tcb->frame_header.size, tcb->frame_max);
            s_connection_abort (thread, ASL_FRAME_ERROR, reply_text, 0, 0);
        }
        else {
            //  Read method payload plus frame-end
            ipr_bucket_unlink (&tcb->bucket_in);
            s_socket_read_bucket (thread, &tcb->bucket_in,
                tcb->frame_header.size + 1);
        }
        //  We have successfully read a frame header, reset peer liveness
        tcb->liveness = FULL_LIVENESS;
    
}
#undef  tcb
    return _rc;
}

static int
action_check_frame_format (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_FRAME_FORMAT))
    icl_stats_inc ("demo_client_agent_check_frame_format", &s_demo_client_agent_check_frame_format_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  The bucket must be at least 1 octet long, and end with a frame-end
        tcb->bucket_in->cur_size = tcb->socket->io_size;
        if (tcb->bucket_in->cur_size > 0
        &&  tcb->bucket_in->data [tcb->bucket_in->cur_size - 1] == ASL_FRAME_END) {
            //  Strip off frame-end octet
            tcb->bucket_in->cur_size--;
        }
        else
            s_connection_abort (thread, ASL_SYNTAX_ERROR,
                tcb->bucket_in->cur_size?
                "Badly-formatted method frame - missing frame-end octet":
                "Badly-formatted method frame - zero-sized frame",
                0, 0);
    
}
#undef  tcb
    return _rc;
}

static int
action_decode_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DECODE_METHOD))
    icl_stats_inc ("demo_client_agent_decode_method", &s_demo_client_agent_decode_method_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Loop if we received a heartbeat frame
        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, read_method_event);
        else
        //  Decode method
        if (tcb->frame_header.type == ASL_FRAME_METHOD) {
            demo_client_method_unlink (&tcb->method_in);
            tcb->method_in = demo_client_method_decode (tcb->bucket_in,
                tcb->strerror);
            if (tcb->method_in) {
                s_history_table [s_history_index].inout = 'i';
                s_history_table [s_history_index].name  = tcb->method_in->name;
                s_history_index = (s_history_index + 1) % METHOD_HISTORY_SIZE;
                if (tcb->trace >= ASL_TRACE_LOW)
                    demo_client_method_dump (tcb->method_in, NULL, "I: recv ");
            }
            else
                s_connection_abort (thread,
                    ASL_SYNTAX_ERROR, tcb->strerror, 0, 0);
        }
        else
            s_connection_exception (thread,
                ASL_SYNTAX_ERROR, "Expected a method frame", 0, 0);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_method_class (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_METHOD_CLASS))
    icl_stats_inc ("demo_client_agent_check_method_class", &s_demo_client_agent_check_method_class_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id == DEMO_CLIENT_CONNECTION)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id == DEMO_CLIENT_CHANNEL)
            smt_thread_set_next_event (thread, channel_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_CLIENT_BASIC
        &&  tcb->method_in->method_id == DEMO_CLIENT_BASIC_DELIVER)
            smt_thread_set_next_event (thread, basic_content_event);
        else
        if (tcb->method_in->class_id  == DEMO_CLIENT_BASIC
        &&  tcb->method_in->method_id == DEMO_CLIENT_BASIC_GET_OK)
            smt_thread_set_next_event (thread, basic_content_event);
        else
            smt_thread_set_next_event (thread, other_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_connection_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_CONNECTION_METHOD))
    icl_stats_inc ("demo_client_agent_check_connection_method", &s_demo_client_agent_check_connection_method_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is valid
        if (tcb->frame_header.channel != 0)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_TUNE)
            smt_thread_set_next_event (thread, connection_tune_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_OPEN_OK)
            smt_thread_set_next_event (thread, connection_open_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_close_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_close_ok_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_channel_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_CHANNEL_METHOD))
    icl_stats_inc ("demo_client_agent_check_channel_method", &s_demo_client_agent_check_channel_method_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_OPEN_OK)
            smt_thread_set_next_event (thread, channel_open_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_CLOSE)
            smt_thread_set_next_event (thread, channel_close_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_CLOSE_OK)
            smt_thread_set_next_event (thread, channel_close_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_FLOW)
            smt_thread_set_next_event (thread, channel_flow_event);
        else
        if (tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_FLOW_OK)
            smt_thread_set_next_event (thread, channel_flow_ok_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_functional_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_FUNCTIONAL_METHOD))
    icl_stats_inc ("demo_client_agent_execute_functional_method", &s_demo_client_agent_execute_functional_method_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        //  Check channel is OPEN
        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_functional_method (thread,
                tcb->method_in->class_id, tcb->method_in->method_id,
                tcb->out_class_id, tcb->out_method_id);
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_tune (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_TUNE))
    icl_stats_inc ("demo_client_agent_execute_connection_tune", &s_demo_client_agent_execute_connection_tune_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_CLIENT_CONNECTION, DEMO_CLIENT_CONNECTION_TUNE);
        if (!smt_thread_exception_raised (thread)) {
            tcb->frame_max   =
                tcb->method_in->payload.connection_tune.frame_max;
#if defined (BASE_THREADSAFE)
            tcb->heartbeat =
                tcb->method_in->payload.connection_tune.heartbeat;
#else
            //  In single-threaded builds we disable heartbeating because
            //  the application can block, and thus kill the connection
            tcb->heartbeat = 0;
#endif
            if (tcb->frame_max < ASL_FRAME_MIN_SIZE
            &&  tcb->frame_max > 0)
                tcb->frame_max = ASL_FRAME_MIN_SIZE;
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_tune (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_TUNE))
    icl_stats_inc ("demo_client_agent_notify_connection_of_tune", &s_demo_client_agent_notify_connection_of_tune_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_agent_connection_tune (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_open_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CONNECTION_OPEN_OK))
    icl_stats_inc ("demo_client_agent_execute_connection_open_ok", &s_demo_client_agent_execute_connection_open_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_CLIENT_CONNECTION, DEMO_CLIENT_CONNECTION_OPEN_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_open_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_OPEN_OK))
    icl_stats_inc ("demo_client_agent_notify_connection_of_open_ok", &s_demo_client_agent_notify_connection_of_open_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_agent_connection_open_ok (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_passive_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_PASSIVE_CLOSE))
    icl_stats_inc ("demo_client_agent_notify_connection_of_passive_close", &s_demo_client_agent_notify_connection_of_passive_close_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Report error if any
        s_report_error (thread, "Connection", "closing connection",
            tcb->method_in->payload.connection_close.reply_code,
            tcb->method_in->payload.connection_close.reply_text,
            tcb->method_in->payload.connection_close.class_id,
            tcb->method_in->payload.connection_close.method_id);
        //  Save reply code for demo_client_connection
        tcb->reply_code = tcb->method_in->payload.connection_close.reply_code;
        icl_shortstr_cpy (tcb->reply_text,
            tcb->method_in->payload.connection_close.reply_text);
        //  Notify connection thread
        smt_thread_raise_exception (tcb->connection_thread,
            connection_close_event);
        smt_thread_wake (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_CLOSE_OK))
    icl_stats_inc ("demo_client_agent_notify_connection_of_close_ok", &s_demo_client_agent_notify_connection_of_close_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        demo_client_agent_connection_close_ok (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_open_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_OPEN_OK))
    icl_stats_inc ("demo_client_agent_execute_channel_open_ok", &s_demo_client_agent_execute_channel_open_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPENING
        if (tcb->channel_state == ASL_CHANNEL_OPENING) {
            //  Channel is now OPEN
            tcb->channel_state = ASL_CHANNEL_OPEN;
            //  Signal demo_client_session that the channel is ready for use
            smt_method_send (
                tcb->channel->method_queue,
                SMT_NULL_EVENT,
                SMT_PRIORITY_NORMAL,
                DEMO_CLIENT_REPLY_INTERN,
                NULL,                   //  No data for method
                NULL);                  //  No destructor neccessary
        }
        else
            //  Channel is not OPENING
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not opening",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_OPEN_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_CLOSE))
    icl_stats_inc ("demo_client_agent_execute_channel_close", &s_demo_client_agent_execute_channel_close_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Report error if any
            s_report_error (thread, "Channel", "closing channel",
                tcb->method_in->payload.channel_close.reply_code,
                tcb->method_in->payload.channel_close.reply_text,
                tcb->method_in->payload.channel_close.class_id,
                tcb->method_in->payload.channel_close.method_id);
            //  Channel is now CLOSING
            tcb->channel_state = ASL_CHANNEL_CLOSING;
            //  Save reply code for demo_client_session
            tcb->reply_code = tcb->method_in->payload.channel_close.reply_code;
            strcpy (tcb->reply_text, tcb->method_in->payload.channel_close.reply_text);
            //  Queue the channel close ok
            demo_client_agent_send_channel_close_ok (tcb->connection_thread);
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_CLOSE);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_CLOSE_OK))
    icl_stats_inc ("demo_client_agent_execute_channel_close_ok", &s_demo_client_agent_execute_channel_close_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Check channel is CLOSING
        if (tcb->channel_state == ASL_CHANNEL_CLOSING) {
            //  Signal demo_client_session that the channel is closed
            *tcb->channel->alive = FALSE;
            *tcb->channel->reply_code = tcb->reply_code;
            strcpy (tcb->channel->reply_text, tcb->reply_text);
            smt_method_send (
                tcb->channel->method_queue,
                SMT_NULL_EVENT,
                SMT_PRIORITY_NORMAL,
                DEMO_CLIENT_REPLY_CLOSED,
                NULL,
                NULL);
            //  Destroy the channel object
            demo_client_channel_destroy (&tcb->channel);
            //  Channel is now CLOSED
            tcb->channel_state = ASL_CHANNEL_CLOSED;
        }
        else
            //  Channel is not CLOSING
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not closing",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_CLOSE_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_flow (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_FLOW))
    icl_stats_inc ("demo_client_agent_execute_channel_flow", &s_demo_client_agent_execute_channel_flow_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            s_execute_method (thread, TRUE,
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_FLOW);
            if (!smt_thread_exception_raised (thread))
                tcb->channel->active = !tcb->method_in->payload.channel_flow.active;
        }
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_FLOW);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_flow_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXECUTE_CHANNEL_FLOW_OK))
    icl_stats_inc ("demo_client_agent_execute_channel_flow_ok", &s_demo_client_agent_execute_channel_flow_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            s_execute_method (thread, TRUE,
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_FLOW_OK);
            if (!smt_thread_exception_raised (thread))
                //  Signal demo_client_session that the channel is ready for use
                smt_method_send (
                    tcb->channel->method_queue,
                    SMT_NULL_EVENT,
                    SMT_PRIORITY_NORMAL,
                    DEMO_CLIENT_REPLY_INTERN,
                    NULL,               //  No data for method
                    NULL);              //  No destructor neccessary
        }
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_CLIENT_CHANNEL, DEMO_CLIENT_CHANNEL_FLOW_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_content_basic_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONTENT_BASIC_HEADER))
    icl_stats_inc ("demo_client_agent_expect_content_basic_header", &s_demo_client_agent_expect_content_basic_header_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, header_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->frame_header.type == ASL_FRAME_HEADER) {
            tcb->method_in->content =
                demo_content_basic_new ();
            if (demo_content_basic_record_header (
                (demo_content_basic_t *)
                tcb->method_in->content,
                tcb->bucket_in))
                s_connection_exception (thread, ASL_SYNTAX_ERROR,
                    "Invalid content header frame",
                    tcb->method_in->class_id,
                    tcb->method_in->method_id);
            else
            if (((demo_content_basic_t *)
                tcb->method_in->content)->class_id != DEMO_CLIENT_BASIC)
                s_connection_exception (thread, ASL_SYNTAX_ERROR,
                    "Content class does not match method class",
                    tcb->method_in->class_id,
                    tcb->method_in->method_id);

            //  Chronometer on content received from wire
            demo_content_basic_chrono_add (tcb->method_in->content);
        }
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Expected a content header frame",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_if_basic_body_complete (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_IF_BASIC_BODY_COMPLETE))
    icl_stats_inc ("demo_client_agent_check_if_basic_body_complete", &s_demo_client_agent_check_if_basic_body_complete_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        assert ((demo_content_basic_t *) tcb->method_in);
        assert ((demo_content_basic_t *) tcb->method_in->content);

        if (((demo_content_basic_t *)
            tcb->method_in->content)->body_size
        <   ((demo_content_basic_t *)
            tcb->method_in->content)->body_expect)
            smt_thread_raise_exception (thread, expect_body_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_content_basic_body (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONTENT_BASIC_BODY))
    icl_stats_inc ("demo_client_agent_expect_content_basic_body", &s_demo_client_agent_expect_content_basic_body_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, expect_body_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->frame_header.type == ASL_FRAME_BODY)
            demo_content_basic_record_body (
                (demo_content_basic_t *)
                tcb->method_in->content, tcb->bucket_in);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Expected a content body frame",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_exception_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_EXCEPTION_METHOD))
    icl_stats_inc ("demo_client_agent_expect_exception_method", &s_demo_client_agent_expect_exception_method_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id  == DEMO_CLIENT_CONNECTION &&
            tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_CLIENT_CONNECTION &&
            tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_CLIENT_CHANNEL &&
            tcb->method_in->method_id == DEMO_CLIENT_CHANNEL_CLOSE)
            smt_thread_set_next_event (thread, channel_class_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_exception (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_EXCEPTION))
    icl_stats_inc ("demo_client_agent_notify_connection_of_exception", &s_demo_client_agent_notify_connection_of_exception_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_thread_raise_exception (tcb->connection_thread,
            external_exception_event);
        smt_thread_wake (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_NOTIFY_CONNECTION_OF_ERROR))
    icl_stats_inc ("demo_client_agent_notify_connection_of_error", &s_demo_client_agent_notify_connection_of_error_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_thread_raise_exception (tcb->connection_thread,
            connection_error_event);
        smt_thread_wake (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_method_frame (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_METHOD_FRAME))
    icl_stats_inc ("demo_client_agent_expect_method_frame", &s_demo_client_agent_expect_method_frame_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->frame_header.type != ASL_FRAME_METHOD)
            smt_thread_set_next_event (thread, discard_frame_event);
        else
            smt_thread_set_next_event (thread, method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_connection_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_EXPECT_CONNECTION_CLOSE_OK))
    icl_stats_inc ("demo_client_agent_expect_connection_close_ok", &s_demo_client_agent_expect_connection_close_ok_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id  == DEMO_CLIENT_CONNECTION &&
            tcb->method_in->method_id == DEMO_CLIENT_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_close_ok_event);
        else
            smt_thread_set_next_event (thread, discard_frame_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_direct_connect_to_server (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIRECT_CONNECT_TO_SERVER))
    icl_stats_inc ("demo_client_agent_direct_connect_to_server", &s_demo_client_agent_direct_connect_to_server_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_shortstr_t
            host;                       //  Copy of server address
        char
            *port;                      //  Port to connect to
        int
            rc = SMT_OK;                //  New buffer size

        //  We extract the port number from the host name if specified
        icl_shortstr_cpy (host, tcb->host);
        port = strchr (host, ':');
        if (port)
            *port++ = 0;                //  Split the string
        else {
            port = "7654";
            //  Save port in tcb->host for reporting
            icl_shortstr_cat (tcb->host, ":");
            icl_shortstr_cat (tcb->host, port);
        }
        tcb->socket = smt_socket_new ();
        if (tcb->socket) {
            //  Schedule a timeout event at connect time
            tcb->setup_timer = smt_timer_request_delay (
                thread, tcb->timeout, setup_timeout_event);

            //  Set Nagle's algorithm off, for lowest latency
            rc = smt_socket_set_nodelay (tcb->socket,
                demo_client_config_tcp_nodelay (demo_client_config));

            //  Connect to server
            //  Error return will be caught as an exception event
            rc = smt_socket_connect (
                tcb->socket, thread, 0, host, port, ok_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_protocol_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_PROTOCOL_HEADER))
    icl_stats_inc ("demo_client_agent_send_direct_protocol_header", &s_demo_client_agent_send_direct_protocol_header_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        //  Cancel setup timer
        smt_timer_request_destroy (&tcb->setup_timer);

        memcpy (tcb->protocol_header, "DEMO" DP_HEADER, 8);
        smt_socket_request_write (
            thread, tcb->socket, 0, 8, tcb->protocol_header, SMT_NULL_EVENT);
        //  Read start of server response
        smt_socket_request_read (
            thread, tcb->socket, 0, 1, 1, &tcb->rlength, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_direct_response (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_READ_DIRECT_RESPONSE))
    icl_stats_inc ("demo_client_agent_read_direct_response", &s_demo_client_agent_read_direct_response_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        smt_socket_request_read (
            thread, tcb->socket, 0, 1, tcb->rlength, (byte *) tcb->response, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_direct_response (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_DIRECT_RESPONSE))
    icl_stats_inc ("demo_client_agent_check_direct_response", &s_demo_client_agent_check_direct_response_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        tcb->response [tcb->rlength] = 0;
        if (memcmp (tcb->response, "200", 3) != 0)
            smt_thread_raise_exception (thread, direct_error_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_lease (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SEND_DIRECT_LEASE))
    icl_stats_inc ("demo_client_agent_send_direct_lease", &s_demo_client_agent_send_direct_lease_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        tcb->rlength = (byte) strlen (tcb->lease);
        smt_socket_request_write (
            thread, tcb->socket, 0, 1, &tcb->rlength, SMT_NULL_EVENT);
        smt_socket_request_write (
            thread, tcb->socket, 0, strlen (tcb->lease), (byte *) tcb->lease, SMT_NULL_EVENT);
        //  Read start of server response
        smt_socket_request_read (
            thread, tcb->socket, 0, 1, 1, &tcb->rlength, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_tell_session_direct_connection_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CONNECTION_OK))
    icl_stats_inc ("demo_client_agent_tell_session_direct_connection_ok", &s_demo_client_agent_tell_session_direct_connection_ok_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        smt_method_send (
            tcb->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_DIRECT_OK,
            NULL,               //  No data for method
            NULL);              //  No destructor neccessary
    
}
#undef  tcb
    return _rc;
}

static int
action_tell_session_direct_connection_failed (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CONNECTION_FAILED))
    icl_stats_inc ("demo_client_agent_tell_session_direct_connection_failed", &s_demo_client_agent_tell_session_direct_connection_failed_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        smt_method_send (
            tcb->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_DIRECT_NG,
            NULL,               //  No data for method
            NULL);              //  No destructor neccessary
    
}
#undef  tcb
    return _rc;
}

static int
action_prepare_direct_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_PREPARE_DIRECT_CONNECTION))
    icl_stats_inc ("demo_client_agent_prepare_direct_connection", &s_demo_client_agent_prepare_direct_connection_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        if (tcb->type == DP_FEED) {
            //  Slurp a bucket of data from the network
            //  Min. size for a packed 6-DMP content is 12 bytes
            //  We always read as much as we can, limited by ipr_bucket
            tcb->bucket_in = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
            smt_socket_request_read (thread, tcb->socket, 0,
                12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                direct_in_event);
        }
        tcb->batching = demo_client_config_batching (demo_client_config);
        if (tcb->batching < 2048)
            tcb->batching = 0;
    
}
#undef  tcb
    return _rc;
}

static int
action_deliver_direct_contents (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DELIVER_DIRECT_CONTENTS))
    icl_stats_inc ("demo_client_agent_deliver_direct_contents", &s_demo_client_agent_deliver_direct_contents_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        demo_content_basic_t
            *content;
        size_t
            pending,
            offset;

        //  Update bucket with amount read, check if complete
        tcb->bucket_in->cur_size += tcb->socket->io_size;
        pending = demo_content_basic_wire_pending (tcb->bucket_in);
        if (pending)
            smt_socket_request_read (
                thread, tcb->socket, 0,
                pending, pending, tcb->bucket_in->data + tcb->bucket_in->cur_size,
                direct_in_event);
        else {
            //  Process each content in the bucket
            offset = 0;
            content = demo_content_basic_wire_get (tcb->bucket_in, &offset, NULL);
            while (content) {
                //  Chronometer on content received from wire
                demo_content_basic_chrono_add (content);
                smt_method_send (
                    tcb->method_queue,
                    SMT_NULL_EVENT,
                    SMT_PRIORITY_NORMAL,
                    DEMO_CLIENT_REPLY_DIRECT_MSG,
                    content,
                    s_content_destructor);
                content = demo_content_basic_wire_get (tcb->bucket_in, &offset, NULL);
            }
            //  Reset bucket and get fresh contents data
            tcb->bucket_in->cur_size = 0;
            smt_socket_request_read (thread, tcb->socket, 0,
                12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                direct_in_event);
        }
        //  If needed, fake a slow network by pausing whole thread for 1msec
        if (tcb->fakeslow)
            apr_sleep (1000);
    
}
#undef  tcb
    return _rc;
}

static int
action_write_direct_content (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_WRITE_DIRECT_CONTENT))
    icl_stats_inc ("demo_client_agent_write_direct_content", &s_demo_client_agent_write_direct_content_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        ipr_bucket_t
            *bucket;
        smt_method_t
            *method;
        int
            rc;
        Bool
            batch;                      //  Do we batch more messages or not?

        //  Chronometer on content sent to wire
        demo_content_basic_chrono_add (direct_out_m->content);
        if (tcb->batching) {
            //  We start by putting one message into a batch-sized bucket
            bucket = ipr_bucket_new (tcb->batching);
            rc = demo_content_basic_wire_put (
                direct_out_m->content, bucket, direct_out_m->options);

            if (rc == 0)
                batch = TRUE;           //  Success, so continue batching
            else {
                //  If the message is oversized, grab a max.sized bucket and try again
                batch = FALSE;
                ipr_bucket_unlink (&bucket);
                bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
                rc = demo_content_basic_wire_put (
                    direct_out_m->content, bucket, direct_out_m->options);
                if (rc)
                    icl_system_panic ("WireAPI", "DMP message too large (>%d bytes)", IPR_BUCKET_MAX_SIZE);
                assert (rc == 0);       //  We can't have a message bigger than this
            }
        }
        else {
            //  Batching has been disabled
            batch = FALSE;
            bucket = ipr_bucket_new (demo_content_basic_wire_size (direct_out_m->content));
            rc = demo_content_basic_wire_put (
                direct_out_m->content, bucket, direct_out_m->options);
            if (rc)
                icl_system_panic ("WireAPI", "DMP message too large (>%d bytes)", IPR_BUCKET_MAX_SIZE);
            assert (rc == 0);
        }
        while (batch && rc == 0 && thread->reply_list->smt_method.list_next != &thread->reply_list->smt_method) {
            method = smt_method_list_pop (thread->reply_list);
            if (method->event == direct_out_m_event) {
                //  Chronometer on content sent to wire
                demo_content_basic_chrono_add (
                    ((demo_client_agent_direct_out_t *) method->data)->content);
                rc = demo_content_basic_wire_put (
                    ((demo_client_agent_direct_out_t *) method->data)->content, bucket, direct_out_m->options);
                if (rc == 0)
                    smt_method_unlink (&method);
            }
            if (method) {               //  Not bundled, push back & stop
                smt_method_list_push (thread->reply_list, method);
                smt_method_unlink (&method);
                break;
            }
        }
        smt_socket_request_write_bucket (
            thread, tcb->socket, 0, bucket, SMT_NULL_EVENT);
        ipr_bucket_unlink (&bucket);
    
}
#undef  tcb
    return _rc;
}

static int
action_tell_session_direct_closed (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_TELL_SESSION_DIRECT_CLOSED))
    icl_stats_inc ("demo_client_agent_tell_session_direct_closed", &s_demo_client_agent_tell_session_direct_closed_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        smt_method_send (
            tcb->method_queue,
            SMT_NULL_EVENT,
            SMT_PRIORITY_NORMAL,
            DEMO_CLIENT_REPLY_INTERN,
            NULL,               //  No data for method
            NULL);              //  No destructor neccessary
    
}
#undef  tcb
    return _rc;
}

static int
action_flush_direct_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_FLUSH_DIRECT_CONNECTION))
    icl_stats_inc ("demo_client_agent_flush_direct_connection", &s_demo_client_agent_flush_direct_connection_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        if (tcb->socket)
            smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_direct_protocol_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_PROTOCOL_ERROR))
    icl_stats_inc ("demo_client_agent_report_direct_protocol_error", &s_demo_client_agent_report_direct_protocol_error_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_console_print (
            "E: direct connection refused: %s (%s)", tcb->response, tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_direct_connect_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_CONNECT_ERROR))
    icl_stats_inc ("demo_client_agent_report_direct_connect_error", &s_demo_client_agent_report_direct_connect_error_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_console_print (
            "E: connection to server failed: %s (%s)", smt_thread_error (thread), tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_direct_socket_error (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_SOCKET_ERROR))
    icl_stats_inc ("demo_client_agent_report_direct_socket_error", &s_demo_client_agent_report_direct_socket_error_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_console_print (
            "E: %s, connection to server lost (%s)", smt_thread_error (thread), tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_direct_socket_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_SOCKET_TIMEOUT))
    icl_stats_inc ("demo_client_agent_report_direct_socket_timeout", &s_demo_client_agent_report_direct_socket_timeout_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_console_print (
            "E: socket timeout, connection to server lost (%s)",  tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_direct_connect_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_DIRECT_CONNECT_TIMEOUT))
    icl_stats_inc ("demo_client_agent_report_direct_connect_timeout", &s_demo_client_agent_report_direct_connect_timeout_stats);
#endif        
#define tcb ((demo_client_agent_direct_context_t *) thread->context)
{

        icl_console_print (
            "E: connection to server timed out (%s)", tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_wait_for_heartbeat_interval (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_WAIT_FOR_HEARTBEAT_INTERVAL))
    icl_stats_inc ("demo_client_agent_wait_for_heartbeat_interval", &s_demo_client_agent_wait_for_heartbeat_interval_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        smt_timer_request_delay (thread, tcb->heartbeat * 1000 * 1000, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_peer_liveness (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_CHECK_PEER_LIVENESS))
    icl_stats_inc ("demo_client_agent_check_peer_liveness", &s_demo_client_agent_check_peer_liveness_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        //  Decrease peer liveness.  If liveness reaches 1 log a warning, if
        //  liveness reaches zero, pronounce the peer dead
        tcb->liveness--;
        if (tcb->liveness) {
            if (tcb->liveness == 1)
                smt_thread_set_next_event (thread, peer_slowing_event);
            else
                smt_thread_set_next_event (thread, peer_alive_event);
        }
        else
            smt_thread_set_next_event (thread, peer_stopped_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_peer_slowing (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_PEER_SLOWING))
    icl_stats_inc ("demo_client_agent_report_peer_slowing", &s_demo_client_agent_report_peer_slowing_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        icl_console_print (
            "W: server heartbeat slowing (%s)", tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_shut_down_agent (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_SHUT_DOWN_AGENT))
    icl_stats_inc ("demo_client_agent_shut_down_agent", &s_demo_client_agent_shut_down_agent_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        if (tcb->input_thread)
            smt_thread_shut_down (tcb->input_thread);
        if (tcb->connection_thread)
            smt_thread_shut_down (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_peer_stopped (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_REPORT_PEER_STOPPED))
    icl_stats_inc ("demo_client_agent_report_peer_stopped", &s_demo_client_agent_report_peer_stopped_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

        icl_console_print (
            "W: server heartbeat stopped, closing connection (%s)", tcb->host);
    
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_19 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_19))
    icl_stats_inc ("demo_client_agent_unnamed_19", &s_demo_client_agent_unnamed_19_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            *tcb->conn_interrupt = TRUE;
            if (tcb->verbose)
                icl_console_print ("I: caught signal, shutting down");
            tcb->reply_code = 100;
            icl_shortstr_cpy (tcb->reply_text,
                "Operator requested client shutdown");
        
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_send_content_body_if_any_start (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_SEND_CONTENT_BODY_IF_ANY_START))
    icl_stats_inc ("demo_client_agent_dialog_call_send_content_body_if_any_start", &s_demo_client_agent_dialog_call_send_content_body_if_any_start_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 68;
    smt_thread_set_next_event (thread, start_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_return (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_RETURN))
    icl_stats_inc ("demo_client_agent_dialog_return", &s_demo_client_agent_dialog_return_stats);
#endif        
    if (smt_thread_position_pop (thread) == SMT_EMPTY)
        _rc = SMT_EMPTY;
    else
        _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_call_close_the_connection_passive_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_PASSIVE_CLOSE))
    icl_stats_inc ("demo_client_agent_dialog_call_close_the_connection_passive_close", &s_demo_client_agent_dialog_call_close_the_connection_passive_close_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 82;
    smt_thread_set_next_event (thread, passive_close_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_49 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_49))
    icl_stats_inc ("demo_client_agent_unnamed_49", &s_demo_client_agent_unnamed_49_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            if (tcb->verbose)
                icl_console_print (
                    "E: timed out waiting for connection.close-ok from server"   " (%s)",
                    tcb->host);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_50 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_50))
    icl_stats_inc ("demo_client_agent_unnamed_50", &s_demo_client_agent_unnamed_50_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            if (tcb->verbose)
                icl_console_print (
                    "E: connection error while "   "waiting for connection.close-ok from client (%s)",
                    tcb->host);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_51 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_51))
    icl_stats_inc ("demo_client_agent_unnamed_51", &s_demo_client_agent_unnamed_51_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            if (tcb->verbose)
                icl_console_print (
                    "E: received connection close while "   "waiting for connection.close-ok from client (%s)",
                    tcb->host);
            
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_close_the_connection_active_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_ACTIVE_CLOSE))
    icl_stats_inc ("demo_client_agent_dialog_call_close_the_connection_active_close", &s_demo_client_agent_dialog_call_close_the_connection_active_close_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 82;
    smt_thread_set_next_event (thread, active_close_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_61 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_61))
    icl_stats_inc ("demo_client_agent_unnamed_61", &s_demo_client_agent_unnamed_61_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            tcb->reply_code = 100;
            icl_shortstr_cpy (tcb->reply_text,
                "Operator requested client shutdown");
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_62 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_62))
    icl_stats_inc ("demo_client_agent_unnamed_62", &s_demo_client_agent_unnamed_62_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            tcb->reply_code = 100;
            icl_shortstr_cpy (tcb->reply_text,
                "Operator requested client shutdown");
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_64 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_64))
    icl_stats_inc ("demo_client_agent_unnamed_64", &s_demo_client_agent_unnamed_64_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            *tcb->conn_interrupt = TRUE;
            if (tcb->verbose)
                icl_console_print ("I: caught signal, shutting down");
            tcb->reply_code = 100;
            icl_shortstr_cpy (tcb->reply_text,
                "Operator requested client shutdown");
        
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_read_method_read_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_READ_METHOD_READ_METHOD))
    icl_stats_inc ("demo_client_agent_dialog_call_read_method_read_method", &s_demo_client_agent_dialog_call_read_method_read_method_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 108;
    smt_thread_set_next_event (thread, read_method_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_call_connection_closing_closing (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_CONNECTION_CLOSING_CLOSING))
    icl_stats_inc ("demo_client_agent_dialog_call_connection_closing_closing", &s_demo_client_agent_dialog_call_connection_closing_closing_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 117;
    smt_thread_set_next_event (thread, closing_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_call_read_basic_content_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_DIALOG_CALL_READ_BASIC_CONTENT_HEADER))
    icl_stats_inc ("demo_client_agent_dialog_call_read_basic_content_header", &s_demo_client_agent_dialog_call_read_basic_content_header_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 167;
    smt_thread_set_next_event (thread, header_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_113 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT)  ||  defined (BASE_STATS_DEMO_CLIENT_AGENT_UNNAMED_113))
    icl_stats_inc ("demo_client_agent_unnamed_113", &s_demo_client_agent_unnamed_113_stats);
#endif        
#define tcb ((demo_client_agent_connection_context_t *) thread->context)
{

            if (*tcb->conn_interrupt == FALSE) {
                icl_console_print ("I: caught signal, shutting down");
                *tcb->conn_interrupt = TRUE;
                demo_client_agent_shutdown (tcb->connection_thread);
            }
            //  Restart monitoring
            smt_thread_set_next_event (thread, peer_alive_event);
        
}
#undef  tcb
    return _rc;
}

