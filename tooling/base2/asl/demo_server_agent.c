/*---------------------------------------------------------------------------
    demo_server_agent.c - functions for demo_server_agent.

    Generated from demo_server_agent.smt by smt_gen.gsl using GSL/4.

    
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
#include "demo_server_agent.h"

#define master_initialise_master_state             1
#define master_accepting_connections_state         2

#define connection_initialise_connection_state     3
#define connection_direct_connection_state         4
#define connection_close_the_connection_state      5
#define connection_connection_active_state         6
#define connection_send_content_body_if_any_state  7

#define input_start_state                          8
#define input_read_method_state                    9
#define input_connection_closing_state             10
#define input_have_incoming_method_state           11
#define input_have_connection_method_state         12
#define input_have_channel_method_state            13
#define input_read_basic_content_state             14

#define heartbeat_monitor_heartbeat_state          15

#define set_trace_m ((demo_server_agent_set_trace_t *) thread->method->data)

#define connection_error_m ((demo_server_agent_connection_error_t *) thread->method->data)

#define channel_error_m ((demo_server_agent_channel_error_t *) thread->method->data)

#define channel_flow_m ((demo_server_agent_channel_flow_t *) thread->method->data)

#define channel_flow_ok_m ((demo_server_agent_channel_flow_ok_t *) thread->method->data)

#define exchange_declare_ok_m ((demo_server_agent_exchange_declare_ok_t *) thread->method->data)

#define exchange_delete_ok_m ((demo_server_agent_exchange_delete_ok_t *) thread->method->data)

#define queue_declare_ok_m ((demo_server_agent_queue_declare_ok_t *) thread->method->data)

#define queue_bind_ok_m ((demo_server_agent_queue_bind_ok_t *) thread->method->data)

#define queue_purge_ok_m ((demo_server_agent_queue_purge_ok_t *) thread->method->data)

#define queue_delete_ok_m ((demo_server_agent_queue_delete_ok_t *) thread->method->data)

#define basic_consume_ok_m ((demo_server_agent_basic_consume_ok_t *) thread->method->data)

#define basic_cancel_ok_m ((demo_server_agent_basic_cancel_ok_t *) thread->method->data)

#define basic_deliver_m ((demo_server_agent_basic_deliver_t *) thread->method->data)

#define basic_get_ok_m ((demo_server_agent_basic_get_ok_t *) thread->method->data)

#define basic_get_empty_m ((demo_server_agent_basic_get_empty_t *) thread->method->data)

#define direct_put_ok_m ((demo_server_agent_direct_put_ok_t *) thread->method->data)

#define direct_get_ok_m ((demo_server_agent_direct_get_ok_t *) thread->method->data)

#define push_m ((demo_server_agent_push_t *) thread->method->data)

#define direct_out_m ((demo_server_agent_direct_out_t *) thread->method->data)

enum {
    ok_event                                       = 1,
    ready_event                                    = 2,
    new_connection_event                           = 3,
    socket_error_event                             = 4,
    smt_error_event                                = 5,
    shutdown_m_event                               = 6,
    shutdown_event                                 = 7,
    have_protocol_header_event                     = 8,
    direct_protocol_event                          = 9,
    direct_in_event                                = 10,
    heartbeat_event                                = 11,
    socket_closing_event                           = 12,
    direct_out_m_event                             = 13,
    socket_timeout_event                           = 14,
    connection_close_event                         = 15,
    active_close_event                             = 16,
    passive_close_event                            = 17,
    close_timeout_event                            = 18,
    connection_error_event                         = 19,
    connection_close_ok_m_event                    = 20,
    connection_abort_event                         = 21,
    external_exception_event                       = 22,
    setup_timeout_event                            = 23,
    set_trace_m_event                              = 24,
    bad_lease_event                                = 25,
    bad_protocol_event                             = 26,
    version_mismatch_event                         = 27,
    connection_start_ok_m_event                    = 28,
    connection_tune_ok_m_event                     = 29,
    connection_open_m_event                        = 30,
    discard_method_event                           = 31,
    channel_flow_m_event                           = 32,
    channel_flow_ok_m_event                        = 33,
    exchange_declare_ok_m_event                    = 34,
    exchange_delete_ok_m_event                     = 35,
    queue_declare_ok_m_event                       = 36,
    queue_bind_ok_m_event                          = 37,
    queue_purge_ok_m_event                         = 38,
    queue_delete_ok_m_event                        = 39,
    basic_consume_ok_m_event                       = 40,
    basic_cancel_ok_m_event                        = 41,
    basic_deliver_m_event                          = 42,
    start_event                                    = 43,
    continue_event                                 = 44,
    finished_event                                 = 45,
    basic_get_ok_m_event                           = 46,
    basic_get_empty_m_event                        = 47,
    direct_put_ok_m_event                          = 48,
    direct_get_ok_m_event                          = 49,
    push_m_event                                   = 50,
    channel_error_m_event                          = 51,
    send_channel_open_ok_m_event                   = 52,
    send_channel_close_ok_m_event                  = 53,
    connection_error_m_event                       = 54,
    kill_connection_m_event                        = 55,
    read_method_event                              = 56,
    closing_event                                  = 57,
    method_event                                   = 58,
    discard_frame_event                            = 59,
    connection_close_ok_event                      = 60,
    async_connection_error_event                   = 61,
    connection_class_event                         = 62,
    connection_start_ok_event                      = 63,
    connection_tune_ok_event                       = 64,
    connection_open_event                          = 65,
    channel_class_event                            = 66,
    channel_open_event                             = 67,
    channel_close_event                            = 68,
    channel_close_ok_event                         = 69,
    channel_flow_event                             = 70,
    channel_flow_ok_event                          = 71,
    basic_content_event                            = 72,
    header_event                                   = 73,
    expect_body_event                              = 74,
    other_method_event                             = 75,
    peer_alive_event                               = 76,
    peer_slowing_event                             = 77,
    peer_stopped_event                             = 78,
};

#define laststate                                  15
#define lastevent                                  78
#define lastplace                                  219

#define demo_server_agent_agent_name               "Demo-Server-Agent"

#define master_thread_name                         "Master"
#define connection_thread_name                     "Connection"
#define input_thread_name                          "Input"
#define heartbeat_thread_name                      "Heartbeat"

#define initialise_master_state_name               "Initialise-Master"
#define accepting_connections_state_name           "Accepting-Connections"
#define initialise_connection_state_name           "Initialise-Connection"
#define direct_connection_state_name               "Direct-Connection"
#define close_the_connection_state_name            "Close-The-Connection"
#define connection_active_state_name               "Connection-Active"
#define send_content_body_if_any_state_name        "Send-Content-Body-If-Any"
#define start_state_name                           "Start"
#define read_method_state_name                     "Read-Method"
#define connection_closing_state_name              "Connection-Closing"
#define have_incoming_method_state_name            "Have-Incoming-Method"
#define have_connection_method_state_name          "Have-Connection-Method"
#define have_channel_method_state_name             "Have-Channel-Method"
#define read_basic_content_state_name              "Read-Basic-Content"
#define monitor_heartbeat_state_name               "Monitor-Heartbeat"

#define ok_event_name                              "Ok"
#define ready_event_name                           "Ready"
#define new_connection_event_name                  "New-Connection"
#define socket_error_event_name                    "Socket-Error"
#define smt_error_event_name                       "Smt-Error"
#define shutdown_m_event_name                      "Shutdown-M"
#define shutdown_event_name                        "Shutdown"
#define have_protocol_header_event_name            "Have-Protocol-Header"
#define direct_protocol_event_name                 "Direct-Protocol"
#define direct_in_event_name                       "Direct-In"
#define heartbeat_event_name                       "Heartbeat"
#define socket_closing_event_name                  "Socket-Closing"
#define direct_out_m_event_name                    "Direct-Out-M"
#define socket_timeout_event_name                  "Socket-Timeout"
#define connection_close_event_name                "Connection-Close"
#define active_close_event_name                    "Active-Close"
#define passive_close_event_name                   "Passive-Close"
#define close_timeout_event_name                   "Close-Timeout"
#define connection_error_event_name                "Connection-Error"
#define connection_close_ok_m_event_name           "Connection-Close-Ok-M"
#define connection_abort_event_name                "Connection-Abort"
#define external_exception_event_name              "External-Exception"
#define setup_timeout_event_name                   "Setup-Timeout"
#define set_trace_m_event_name                     "Set-Trace-M"
#define bad_lease_event_name                       "Bad-Lease"
#define bad_protocol_event_name                    "Bad-Protocol"
#define version_mismatch_event_name                "Version-Mismatch"
#define connection_start_ok_m_event_name           "Connection-Start-Ok-M"
#define connection_tune_ok_m_event_name            "Connection-Tune-Ok-M"
#define connection_open_m_event_name               "Connection-Open-M"
#define discard_method_event_name                  "Discard-Method"
#define channel_flow_m_event_name                  "Channel-Flow-M"
#define channel_flow_ok_m_event_name               "Channel-Flow-Ok-M"
#define exchange_declare_ok_m_event_name           "Exchange-Declare-Ok-M"
#define exchange_delete_ok_m_event_name            "Exchange-Delete-Ok-M"
#define queue_declare_ok_m_event_name              "Queue-Declare-Ok-M"
#define queue_bind_ok_m_event_name                 "Queue-Bind-Ok-M"
#define queue_purge_ok_m_event_name                "Queue-Purge-Ok-M"
#define queue_delete_ok_m_event_name               "Queue-Delete-Ok-M"
#define basic_consume_ok_m_event_name              "Basic-Consume-Ok-M"
#define basic_cancel_ok_m_event_name               "Basic-Cancel-Ok-M"
#define basic_deliver_m_event_name                 "Basic-Deliver-M"
#define start_event_name                           "Start"
#define continue_event_name                        "Continue"
#define finished_event_name                        "Finished"
#define basic_get_ok_m_event_name                  "Basic-Get-Ok-M"
#define basic_get_empty_m_event_name               "Basic-Get-Empty-M"
#define direct_put_ok_m_event_name                 "Direct-Put-Ok-M"
#define direct_get_ok_m_event_name                 "Direct-Get-Ok-M"
#define push_m_event_name                          "Push-M"
#define channel_error_m_event_name                 "Channel-Error-M"
#define send_channel_open_ok_m_event_name          "Send-Channel-Open-Ok-M"
#define send_channel_close_ok_m_event_name         "Send-Channel-Close-Ok-M"
#define connection_error_m_event_name              "Connection-Error-M"
#define kill_connection_m_event_name               "Kill-Connection-M"
#define read_method_event_name                     "Read-Method"
#define closing_event_name                         "Closing"
#define method_event_name                          "Method"
#define discard_frame_event_name                   "Discard-Frame"
#define connection_close_ok_event_name             "Connection-Close-Ok"
#define async_connection_error_event_name          "Async-Connection-Error"
#define connection_class_event_name                "Connection-Class"
#define connection_start_ok_event_name             "Connection-Start-Ok"
#define connection_tune_ok_event_name              "Connection-Tune-Ok"
#define connection_open_event_name                 "Connection-Open"
#define channel_class_event_name                   "Channel-Class"
#define channel_open_event_name                    "Channel-Open"
#define channel_close_event_name                   "Channel-Close"
#define channel_close_ok_event_name                "Channel-Close-Ok"
#define channel_flow_event_name                    "Channel-Flow"
#define channel_flow_ok_event_name                 "Channel-Flow-Ok"
#define basic_content_event_name                   "Basic-Content"
#define header_event_name                          "Header"
#define expect_body_event_name                     "Expect-Body"
#define other_method_event_name                    "Other-Method"
#define peer_alive_event_name                      "Peer-Alive"
#define peer_slowing_event_name                    "Peer-Slowing"
#define peer_stopped_event_name                    "Peer-Stopped"

#define initialise_server_action_name              "Initialise-Server"
#define allow_thread_to_settle_action_name         "Allow-Thread-To-Settle"
#define report_server_ready_action_name            "Report-Server-Ready"
#define accept_connection_action_name              "Accept-Connection"
#define create_connection_thread_action_name       "Create-Connection-Thread"
#define report_accept_error_action_name            "Report-Accept-Error"
#define report_smt_error_action_name               "Report-Smt-Error"
#define close_master_socket_action_name            "Close-Master-Socket"
#define start_setup_timer_action_name              "Start-Setup-Timer"
#define read_protocol_header_action_name           "Read-Protocol-Header"
#define check_protocol_header_action_name          "Check-Protocol-Header"
#define write_desired_header_action_name           "Write-Desired-Header"
#define accept_direct_connection_action_name       "Accept-Direct-Connection"
#define read_direct_request_action_name            "Read-Direct-Request"
#define check_direct_lease_action_name             "Check-Direct-Lease"
#define report_bad_protocol_action_name            "Report-Bad-Protocol"
#define report_version_mismatch_action_name        "Report-Version-Mismatch"
#define start_input_thread_action_name             "Start-Input-Thread"
#define send_connection_start_action_name          "Send-Connection-Start"
#define send_connection_tune_action_name           "Send-Connection-Tune"
#define start_heartbeat_thread_action_name         "Start-Heartbeat-Thread"
#define start_heartbeat_timer_action_name          "Start-Heartbeat-Timer"
#define report_setup_timeout_action_name           "Report-Setup-Timeout"
#define send_channel_flow_action_name              "Send-Channel-Flow"
#define send_channel_flow_ok_action_name           "Send-Channel-Flow-Ok"
#define send_exchange_declare_ok_action_name       "Send-Exchange-Declare-Ok"
#define send_exchange_delete_ok_action_name        "Send-Exchange-Delete-Ok"
#define send_queue_declare_ok_action_name          "Send-Queue-Declare-Ok"
#define send_queue_bind_ok_action_name             "Send-Queue-Bind-Ok"
#define send_queue_purge_ok_action_name            "Send-Queue-Purge-Ok"
#define send_queue_delete_ok_action_name           "Send-Queue-Delete-Ok"
#define send_basic_consume_ok_action_name          "Send-Basic-Consume-Ok"
#define send_basic_cancel_ok_action_name           "Send-Basic-Cancel-Ok"
#define send_basic_deliver_action_name             "Send-Basic-Deliver"
#define send_basic_get_ok_action_name              "Send-Basic-Get-Ok"
#define send_basic_get_empty_action_name           "Send-Basic-Get-Empty"
#define send_direct_put_ok_action_name             "Send-Direct-Put-Ok"
#define send_direct_get_ok_action_name             "Send-Direct-Get-Ok"
#define send_connection_open_ok_action_name        "Send-Connection-Open-Ok"
#define send_channel_open_ok_action_name           "Send-Channel-Open-Ok"
#define send_channel_close_ok_action_name          "Send-Channel-Close-Ok"
#define push_method_to_client_action_name          "Push-Method-To-Client"
#define send_channel_close_action_name             "Send-Channel-Close"
#define send_content_header_action_name            "Send-Content-Header"
#define send_content_body_frame_action_name        "Send-Content-Body-Frame"
#define shut_down_slave_threads_action_name        "Shut-Down-Slave-Threads"
#define send_connection_close_action_name          "Send-Connection-Close"
#define execute_connection_close_action_name       "Execute-Connection-Close"
#define send_connection_close_ok_action_name       "Send-Connection-Close-Ok"
#define flush_the_connection_action_name           "Flush-The-Connection"
#define report_socket_error_action_name            "Report-Socket-Error"
#define report_socket_timeout_action_name          "Report-Socket-Timeout"
#define send_heartbeat_frame_action_name           "Send-Heartbeat-Frame"
#define deliver_direct_contents_action_name        "Deliver-Direct-Contents"
#define write_direct_content_action_name           "Write-Direct-Content"
#define write_direct_heartbeat_action_name         "Write-Direct-Heartbeat"
#define read_frame_header_action_name              "Read-Frame-Header"
#define read_frame_payload_action_name             "Read-Frame-Payload"
#define check_frame_format_action_name             "Check-Frame-Format"
#define decode_method_action_name                  "Decode-Method"
#define check_method_class_action_name             "Check-Method-Class"
#define check_connection_method_action_name        "Check-Connection-Method"
#define check_channel_method_action_name           "Check-Channel-Method"
#define execute_functional_method_action_name      "Execute-Functional-Method"
#define execute_connection_start_ok_action_name    "Execute-Connection-Start-Ok"
#define notify_connection_of_start_ok_action_name  "Notify-Connection-Of-Start-Ok"
#define execute_connection_tune_ok_action_name     "Execute-Connection-Tune-Ok"
#define notify_connection_of_tune_ok_action_name   "Notify-Connection-Of-Tune-Ok"
#define execute_connection_open_action_name        "Execute-Connection-Open"
#define notify_connection_of_open_action_name      "Notify-Connection-Of-Open"
#define notify_connection_of_passive_close_action_name  "Notify-Connection-Of-Passive-Close"
#define notify_connection_of_close_ok_action_name  "Notify-Connection-Of-Close-Ok"
#define execute_channel_open_action_name           "Execute-Channel-Open"
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
#define wait_for_heartbeat_interval_action_name    "Wait-For-Heartbeat-Interval"
#define check_peer_liveness_action_name            "Check-Peer-Liveness"
#define report_peer_slowing_action_name            "Report-Peer-Slowing"
#define shut_down_agent_action_name                "Shut-Down-Agent"
#define report_peer_stopped_action_name            "Report-Peer-Stopped"
#define dialog_call_close_the_connection_passive_close_action_name  "Dialog-Call-Close-The-Connection-Passive-Close"
#define dialog_return_action_name                  "Dialog-Return"
#define unnamed_29_action_name                     "Unnamed-29"
#define unnamed_30_action_name                     "Unnamed-30"
#define unnamed_31_action_name                     "Unnamed-31"
#define unnamed_34_action_name                     "Unnamed-34"
#define dialog_call_close_the_connection_active_close_action_name  "Dialog-Call-Close-The-Connection-Active-Close"
#define unnamed_36_action_name                     "Unnamed-36"
#define dialog_call_send_content_body_if_any_start_action_name  "Dialog-Call-Send-Content-Body-If-Any-Start"
#define unnamed_58_action_name                     "Unnamed-58"
#define unnamed_67_action_name                     "Unnamed-67"
#define unnamed_68_action_name                     "Unnamed-68"
#define unnamed_69_action_name                     "Unnamed-69"
#define unnamed_70_action_name                     "Unnamed-70"
#define unnamed_71_action_name                     "Unnamed-71"
#define dialog_call_read_method_read_method_action_name  "Dialog-Call-Read-Method-Read-Method"
#define dialog_call_connection_closing_closing_action_name  "Dialog-Call-Connection-Closing-Closing"
#define dialog_call_read_basic_content_header_action_name  "Dialog-Call-Read-Basic-Content-Header"

#define the_next_event                             thread->_next_event
#define exception_raised                           thread->_exception_raised

typedef struct
  {
    int            trace;
  }
demo_server_agent_set_trace_t;

typedef struct
  {
    dbyte          reply_code;
    char *         reply_text;
    dbyte          faulting_method_id;
    dbyte          faulting_class_id;
  }
demo_server_agent_connection_error_t;

typedef struct
  {
    demo_server_channel_t *  channel;
    dbyte          reply_code;
    char *         reply_text;
    dbyte          faulting_method_id;
    dbyte          faulting_class_id;
  }
demo_server_agent_channel_error_t;

typedef struct
  {
    dbyte          channel_nbr;
    Bool           active;
  }
demo_server_agent_channel_flow_t;

typedef struct
  {
    dbyte          channel_nbr;
    Bool           active;
  }
demo_server_agent_channel_flow_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
  }
demo_server_agent_exchange_declare_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
  }
demo_server_agent_exchange_delete_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         queue;
    icl_longstr_t *  properties;
  }
demo_server_agent_queue_declare_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
  }
demo_server_agent_queue_bind_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    qbyte          message_count;
  }
demo_server_agent_queue_purge_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    qbyte          message_count;
  }
demo_server_agent_queue_delete_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         consumer_tag;
  }
demo_server_agent_basic_consume_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         consumer_tag;
  }
demo_server_agent_basic_cancel_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    demo_content_basic_t *  content;
    char *         exchange;
    char *         routing_key;
    int64_t        delivery_tag;
    void *         callback;
  }
demo_server_agent_basic_deliver_t;

typedef struct
  {
    dbyte          channel_nbr;
    demo_content_basic_t *  content;
    char *         exchange;
    char *         routing_key;
    qbyte          message_count;
    void *         callback;
  }
demo_server_agent_basic_get_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
  }
demo_server_agent_basic_get_empty_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         lease;
  }
demo_server_agent_direct_put_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    char *         lease;
  }
demo_server_agent_direct_get_ok_t;

typedef struct
  {
    dbyte          channel_nbr;
    demo_server_method_t *  method;
  }
demo_server_agent_push_t;

typedef struct
  {
    demo_content_basic_t *  content;
    byte           options;
  }
demo_server_agent_direct_out_t;


#define BASE_ANIMATE            1
#define FULL_LIVENESS           4       //  Four missing heartbeats -> dead

//  Channel states
#define ASL_CHANNEL_CLOSED      0
#define ASL_CHANNEL_OPEN        1
#define ASL_CHANNEL_CLOSING     2
#define ASL_CHANNEL_OPENING     3

#include "version.h"


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
    demo_server_connection_t
        *connection;                    //  Connection object
    demo_server_channel_t
        *channel;                       //  Channel object
    int
        channel_state;                  //  Internal channel state
    demo_server_method_t
        *method_in,                     //  Last received method
        *method_out;                    //  Last sent method
    asl_reader_t
        reader;                         //  Body reader
    ipr_bucket_t
        *bucket_in;                     //  Last received bucket
    icl_shortstr_t
        strerror;                       //  Reply text for method errors
    byte
        protocol_header [8];            //  Used to initiate connection
    asl_frame_header_t
        frame_header;                   //  Header for frame i/o
    dbyte
        out_class_id,                   //  Last class we sent
        out_method_id;                  //  Last method we sent
    qbyte
        frame_max,                      //  Negotiated maximum frame size
        heartbeat;                      //  Connection heartbeat interval
    int
        liveness,                       //  If this reaches zero, peer is dead
        trace;                          //  Trace level
    smt_timer_request_t
        *setup_timer;                   //  Connection setup timer

    //  Direct Protocol handling
    int
        type;                           //  Direct protocol type
    byte
        rlength;                        //  Request length from socket
    icl_shortstr_t
        request;                        //  Request string
    demo_lease_t
        *lease;                         //  Lease to sink or feed
    size_t
        batching;                       //  Configured batch size
    int
        direct_heartbeat;               //  Direct heartbeat, seconds

} demo_server_agent_connection_context_t;

static icl_cache_t
    *s_master_context_cache = NULL;

typedef struct {
    int
        links;

        smt_socket_t
            *master_socket,             //  Socket for connect requests
            *client_socket;             //  Newly-accepted socket
    
} demo_server_agent_master_context_t;

static icl_cache_t
    *s_set_trace_cache = NULL;
static icl_cache_t
    *s_connection_error_cache = NULL;
static icl_cache_t
    *s_channel_error_cache = NULL;
static icl_cache_t
    *s_channel_flow_cache = NULL;
static icl_cache_t
    *s_channel_flow_ok_cache = NULL;
static icl_cache_t
    *s_exchange_declare_ok_cache = NULL;
static icl_cache_t
    *s_exchange_delete_ok_cache = NULL;
static icl_cache_t
    *s_queue_declare_ok_cache = NULL;
static icl_cache_t
    *s_queue_bind_ok_cache = NULL;
static icl_cache_t
    *s_queue_purge_ok_cache = NULL;
static icl_cache_t
    *s_queue_delete_ok_cache = NULL;
static icl_cache_t
    *s_basic_consume_ok_cache = NULL;
static icl_cache_t
    *s_basic_cancel_ok_cache = NULL;
static icl_cache_t
    *s_basic_deliver_cache = NULL;
static icl_cache_t
    *s_basic_get_ok_cache = NULL;
static icl_cache_t
    *s_basic_get_empty_cache = NULL;
static icl_cache_t
    *s_direct_put_ok_cache = NULL;
static icl_cache_t
    *s_direct_get_ok_cache = NULL;
static icl_cache_t
    *s_push_cache = NULL;
static icl_cache_t
    *s_direct_out_cache = NULL;

static const char *thread_name [] = {
    "<NULL>",
    master_thread_name,
    connection_thread_name,
    input_thread_name,
    heartbeat_thread_name
};

static const char *state_name [] = {
    "<NULL>",
    initialise_master_state_name,
    accepting_connections_state_name,
    initialise_connection_state_name,
    direct_connection_state_name,
    close_the_connection_state_name,
    connection_active_state_name,
    send_content_body_if_any_state_name,
    start_state_name,
    read_method_state_name,
    connection_closing_state_name,
    have_incoming_method_state_name,
    have_connection_method_state_name,
    have_channel_method_state_name,
    read_basic_content_state_name,
    monitor_heartbeat_state_name
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
    have_protocol_header_event_name,
    direct_protocol_event_name,
    direct_in_event_name,
    heartbeat_event_name,
    socket_closing_event_name,
    direct_out_m_event_name,
    socket_timeout_event_name,
    connection_close_event_name,
    active_close_event_name,
    passive_close_event_name,
    close_timeout_event_name,
    connection_error_event_name,
    connection_close_ok_m_event_name,
    connection_abort_event_name,
    external_exception_event_name,
    setup_timeout_event_name,
    set_trace_m_event_name,
    bad_lease_event_name,
    bad_protocol_event_name,
    version_mismatch_event_name,
    connection_start_ok_m_event_name,
    connection_tune_ok_m_event_name,
    connection_open_m_event_name,
    discard_method_event_name,
    channel_flow_m_event_name,
    channel_flow_ok_m_event_name,
    exchange_declare_ok_m_event_name,
    exchange_delete_ok_m_event_name,
    queue_declare_ok_m_event_name,
    queue_bind_ok_m_event_name,
    queue_purge_ok_m_event_name,
    queue_delete_ok_m_event_name,
    basic_consume_ok_m_event_name,
    basic_cancel_ok_m_event_name,
    basic_deliver_m_event_name,
    start_event_name,
    continue_event_name,
    finished_event_name,
    basic_get_ok_m_event_name,
    basic_get_empty_m_event_name,
    direct_put_ok_m_event_name,
    direct_get_ok_m_event_name,
    push_m_event_name,
    channel_error_m_event_name,
    send_channel_open_ok_m_event_name,
    send_channel_close_ok_m_event_name,
    connection_error_m_event_name,
    kill_connection_m_event_name,
    read_method_event_name,
    closing_event_name,
    method_event_name,
    discard_frame_event_name,
    connection_close_ok_event_name,
    async_connection_error_event_name,
    connection_class_event_name,
    connection_start_ok_event_name,
    connection_tune_ok_event_name,
    connection_open_event_name,
    channel_class_event_name,
    channel_open_event_name,
    channel_close_event_name,
    channel_close_ok_event_name,
    channel_flow_event_name,
    channel_flow_ok_event_name,
    basic_content_event_name,
    header_event_name,
    expect_body_event_name,
    other_method_event_name,
    peer_alive_event_name,
    peer_slowing_event_name,
    peer_stopped_event_name
};

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT)        ||  defined (BASE_TRACE)  || defined (BASE_TRACE_DEMO_SERVER_AGENT)   ||  defined (DEMO_SERVER_AGENT_TRACE))
static const char *action_name [] = {
    "<NULL>",
    initialise_server_action_name,
    allow_thread_to_settle_action_name,
    report_server_ready_action_name,
    accept_connection_action_name,
    create_connection_thread_action_name,
    report_accept_error_action_name,
    report_smt_error_action_name,
    close_master_socket_action_name,
    start_setup_timer_action_name,
    read_protocol_header_action_name,
    check_protocol_header_action_name,
    write_desired_header_action_name,
    accept_direct_connection_action_name,
    read_direct_request_action_name,
    check_direct_lease_action_name,
    report_bad_protocol_action_name,
    report_version_mismatch_action_name,
    start_input_thread_action_name,
    send_connection_start_action_name,
    send_connection_tune_action_name,
    start_heartbeat_thread_action_name,
    start_heartbeat_timer_action_name,
    report_setup_timeout_action_name,
    send_channel_flow_action_name,
    send_channel_flow_ok_action_name,
    send_exchange_declare_ok_action_name,
    send_exchange_delete_ok_action_name,
    send_queue_declare_ok_action_name,
    send_queue_bind_ok_action_name,
    send_queue_purge_ok_action_name,
    send_queue_delete_ok_action_name,
    send_basic_consume_ok_action_name,
    send_basic_cancel_ok_action_name,
    send_basic_deliver_action_name,
    send_basic_get_ok_action_name,
    send_basic_get_empty_action_name,
    send_direct_put_ok_action_name,
    send_direct_get_ok_action_name,
    send_connection_open_ok_action_name,
    send_channel_open_ok_action_name,
    send_channel_close_ok_action_name,
    push_method_to_client_action_name,
    send_channel_close_action_name,
    send_content_header_action_name,
    send_content_body_frame_action_name,
    shut_down_slave_threads_action_name,
    send_connection_close_action_name,
    execute_connection_close_action_name,
    send_connection_close_ok_action_name,
    flush_the_connection_action_name,
    report_socket_error_action_name,
    report_socket_timeout_action_name,
    send_heartbeat_frame_action_name,
    deliver_direct_contents_action_name,
    write_direct_content_action_name,
    write_direct_heartbeat_action_name,
    read_frame_header_action_name,
    read_frame_payload_action_name,
    check_frame_format_action_name,
    decode_method_action_name,
    check_method_class_action_name,
    check_connection_method_action_name,
    check_channel_method_action_name,
    execute_functional_method_action_name,
    execute_connection_start_ok_action_name,
    notify_connection_of_start_ok_action_name,
    execute_connection_tune_ok_action_name,
    notify_connection_of_tune_ok_action_name,
    execute_connection_open_action_name,
    notify_connection_of_open_action_name,
    notify_connection_of_passive_close_action_name,
    notify_connection_of_close_ok_action_name,
    execute_channel_open_action_name,
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
    wait_for_heartbeat_interval_action_name,
    check_peer_liveness_action_name,
    report_peer_slowing_action_name,
    shut_down_agent_action_name,
    report_peer_stopped_action_name,
    dialog_call_close_the_connection_passive_close_action_name,
    dialog_return_action_name,
    unnamed_29_action_name,
    unnamed_30_action_name,
    unnamed_31_action_name,
    unnamed_34_action_name,
    dialog_call_close_the_connection_active_close_action_name,
    unnamed_36_action_name,
    dialog_call_send_content_body_if_any_start_action_name,
    unnamed_58_action_name,
    unnamed_67_action_name,
    unnamed_68_action_name,
    unnamed_69_action_name,
    unnamed_70_action_name,
    unnamed_71_action_name,
    dialog_call_read_method_read_method_action_name,
    dialog_call_connection_closing_closing_action_name,
    dialog_call_read_basic_content_header_action_name
};
#endif

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
static Bool
    s_animate = FALSE;
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_INITIALISE_SERVER))
static icl_stats_t *s_demo_server_agent_initialise_server_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ALLOW_THREAD_TO_SETTLE))
static icl_stats_t *s_demo_server_agent_allow_thread_to_settle_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SERVER_READY))
static icl_stats_t *s_demo_server_agent_report_server_ready_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ACCEPT_CONNECTION))
static icl_stats_t *s_demo_server_agent_accept_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CREATE_CONNECTION_THREAD))
static icl_stats_t *s_demo_server_agent_create_connection_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_ACCEPT_ERROR))
static icl_stats_t *s_demo_server_agent_report_accept_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SMT_ERROR))
static icl_stats_t *s_demo_server_agent_report_smt_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CLOSE_MASTER_SOCKET))
static icl_stats_t *s_demo_server_agent_close_master_socket_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_SETUP_TIMER))
static icl_stats_t *s_demo_server_agent_start_setup_timer_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_PROTOCOL_HEADER))
static icl_stats_t *s_demo_server_agent_read_protocol_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_PROTOCOL_HEADER))
static icl_stats_t *s_demo_server_agent_check_protocol_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DESIRED_HEADER))
static icl_stats_t *s_demo_server_agent_write_desired_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ACCEPT_DIRECT_CONNECTION))
static icl_stats_t *s_demo_server_agent_accept_direct_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_DIRECT_REQUEST))
static icl_stats_t *s_demo_server_agent_read_direct_request_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_DIRECT_LEASE))
static icl_stats_t *s_demo_server_agent_check_direct_lease_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_BAD_PROTOCOL))
static icl_stats_t *s_demo_server_agent_report_bad_protocol_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_VERSION_MISMATCH))
static icl_stats_t *s_demo_server_agent_report_version_mismatch_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_INPUT_THREAD))
static icl_stats_t *s_demo_server_agent_start_input_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_START))
static icl_stats_t *s_demo_server_agent_send_connection_start_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_TUNE))
static icl_stats_t *s_demo_server_agent_send_connection_tune_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_HEARTBEAT_THREAD))
static icl_stats_t *s_demo_server_agent_start_heartbeat_thread_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_HEARTBEAT_TIMER))
static icl_stats_t *s_demo_server_agent_start_heartbeat_timer_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SETUP_TIMEOUT))
static icl_stats_t *s_demo_server_agent_report_setup_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_FLOW))
static icl_stats_t *s_demo_server_agent_send_channel_flow_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_FLOW_OK))
static icl_stats_t *s_demo_server_agent_send_channel_flow_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_EXCHANGE_DECLARE_OK))
static icl_stats_t *s_demo_server_agent_send_exchange_declare_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_EXCHANGE_DELETE_OK))
static icl_stats_t *s_demo_server_agent_send_exchange_delete_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_DECLARE_OK))
static icl_stats_t *s_demo_server_agent_send_queue_declare_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_BIND_OK))
static icl_stats_t *s_demo_server_agent_send_queue_bind_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_PURGE_OK))
static icl_stats_t *s_demo_server_agent_send_queue_purge_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_DELETE_OK))
static icl_stats_t *s_demo_server_agent_send_queue_delete_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_CONSUME_OK))
static icl_stats_t *s_demo_server_agent_send_basic_consume_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_CANCEL_OK))
static icl_stats_t *s_demo_server_agent_send_basic_cancel_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_DELIVER))
static icl_stats_t *s_demo_server_agent_send_basic_deliver_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_GET_OK))
static icl_stats_t *s_demo_server_agent_send_basic_get_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_GET_EMPTY))
static icl_stats_t *s_demo_server_agent_send_basic_get_empty_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_DIRECT_PUT_OK))
static icl_stats_t *s_demo_server_agent_send_direct_put_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_DIRECT_GET_OK))
static icl_stats_t *s_demo_server_agent_send_direct_get_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_OPEN_OK))
static icl_stats_t *s_demo_server_agent_send_connection_open_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_OPEN_OK))
static icl_stats_t *s_demo_server_agent_send_channel_open_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_CLOSE_OK))
static icl_stats_t *s_demo_server_agent_send_channel_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_PUSH_METHOD_TO_CLIENT))
static icl_stats_t *s_demo_server_agent_push_method_to_client_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_CLOSE))
static icl_stats_t *s_demo_server_agent_send_channel_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONTENT_HEADER))
static icl_stats_t *s_demo_server_agent_send_content_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONTENT_BODY_FRAME))
static icl_stats_t *s_demo_server_agent_send_content_body_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SHUT_DOWN_SLAVE_THREADS))
static icl_stats_t *s_demo_server_agent_shut_down_slave_threads_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_CLOSE))
static icl_stats_t *s_demo_server_agent_send_connection_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_CLOSE))
static icl_stats_t *s_demo_server_agent_execute_connection_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_CLOSE_OK))
static icl_stats_t *s_demo_server_agent_send_connection_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_FLUSH_THE_CONNECTION))
static icl_stats_t *s_demo_server_agent_flush_the_connection_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SOCKET_ERROR))
static icl_stats_t *s_demo_server_agent_report_socket_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SOCKET_TIMEOUT))
static icl_stats_t *s_demo_server_agent_report_socket_timeout_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_HEARTBEAT_FRAME))
static icl_stats_t *s_demo_server_agent_send_heartbeat_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DELIVER_DIRECT_CONTENTS))
static icl_stats_t *s_demo_server_agent_deliver_direct_contents_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DIRECT_CONTENT))
static icl_stats_t *s_demo_server_agent_write_direct_content_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DIRECT_HEARTBEAT))
static icl_stats_t *s_demo_server_agent_write_direct_heartbeat_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_FRAME_HEADER))
static icl_stats_t *s_demo_server_agent_read_frame_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_FRAME_PAYLOAD))
static icl_stats_t *s_demo_server_agent_read_frame_payload_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_FRAME_FORMAT))
static icl_stats_t *s_demo_server_agent_check_frame_format_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DECODE_METHOD))
static icl_stats_t *s_demo_server_agent_decode_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_METHOD_CLASS))
static icl_stats_t *s_demo_server_agent_check_method_class_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_CONNECTION_METHOD))
static icl_stats_t *s_demo_server_agent_check_connection_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_CHANNEL_METHOD))
static icl_stats_t *s_demo_server_agent_check_channel_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_FUNCTIONAL_METHOD))
static icl_stats_t *s_demo_server_agent_execute_functional_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_START_OK))
static icl_stats_t *s_demo_server_agent_execute_connection_start_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_START_OK))
static icl_stats_t *s_demo_server_agent_notify_connection_of_start_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_TUNE_OK))
static icl_stats_t *s_demo_server_agent_execute_connection_tune_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_TUNE_OK))
static icl_stats_t *s_demo_server_agent_notify_connection_of_tune_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_OPEN))
static icl_stats_t *s_demo_server_agent_execute_connection_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_OPEN))
static icl_stats_t *s_demo_server_agent_notify_connection_of_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_PASSIVE_CLOSE))
static icl_stats_t *s_demo_server_agent_notify_connection_of_passive_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_CLOSE_OK))
static icl_stats_t *s_demo_server_agent_notify_connection_of_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_OPEN))
static icl_stats_t *s_demo_server_agent_execute_channel_open_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_CLOSE))
static icl_stats_t *s_demo_server_agent_execute_channel_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_CLOSE_OK))
static icl_stats_t *s_demo_server_agent_execute_channel_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_FLOW))
static icl_stats_t *s_demo_server_agent_execute_channel_flow_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_FLOW_OK))
static icl_stats_t *s_demo_server_agent_execute_channel_flow_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONTENT_BASIC_HEADER))
static icl_stats_t *s_demo_server_agent_expect_content_basic_header_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_IF_BASIC_BODY_COMPLETE))
static icl_stats_t *s_demo_server_agent_check_if_basic_body_complete_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONTENT_BASIC_BODY))
static icl_stats_t *s_demo_server_agent_expect_content_basic_body_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_EXCEPTION_METHOD))
static icl_stats_t *s_demo_server_agent_expect_exception_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_EXCEPTION))
static icl_stats_t *s_demo_server_agent_notify_connection_of_exception_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_ERROR))
static icl_stats_t *s_demo_server_agent_notify_connection_of_error_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_METHOD_FRAME))
static icl_stats_t *s_demo_server_agent_expect_method_frame_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONNECTION_CLOSE_OK))
static icl_stats_t *s_demo_server_agent_expect_connection_close_ok_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WAIT_FOR_HEARTBEAT_INTERVAL))
static icl_stats_t *s_demo_server_agent_wait_for_heartbeat_interval_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_PEER_LIVENESS))
static icl_stats_t *s_demo_server_agent_check_peer_liveness_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_PEER_SLOWING))
static icl_stats_t *s_demo_server_agent_report_peer_slowing_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SHUT_DOWN_AGENT))
static icl_stats_t *s_demo_server_agent_shut_down_agent_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_PEER_STOPPED))
static icl_stats_t *s_demo_server_agent_report_peer_stopped_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_PASSIVE_CLOSE))
static icl_stats_t *s_demo_server_agent_dialog_call_close_the_connection_passive_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_RETURN))
static icl_stats_t *s_demo_server_agent_dialog_return_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_29))
static icl_stats_t *s_demo_server_agent_unnamed_29_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_30))
static icl_stats_t *s_demo_server_agent_unnamed_30_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_31))
static icl_stats_t *s_demo_server_agent_unnamed_31_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_34))
static icl_stats_t *s_demo_server_agent_unnamed_34_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_ACTIVE_CLOSE))
static icl_stats_t *s_demo_server_agent_dialog_call_close_the_connection_active_close_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_36))
static icl_stats_t *s_demo_server_agent_unnamed_36_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_SEND_CONTENT_BODY_IF_ANY_START))
static icl_stats_t *s_demo_server_agent_dialog_call_send_content_body_if_any_start_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_58))
static icl_stats_t *s_demo_server_agent_unnamed_58_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_67))
static icl_stats_t *s_demo_server_agent_unnamed_67_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_68))
static icl_stats_t *s_demo_server_agent_unnamed_68_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_69))
static icl_stats_t *s_demo_server_agent_unnamed_69_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_70))
static icl_stats_t *s_demo_server_agent_unnamed_70_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_71))
static icl_stats_t *s_demo_server_agent_unnamed_71_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_READ_METHOD_READ_METHOD))
static icl_stats_t *s_demo_server_agent_dialog_call_read_method_read_method_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CONNECTION_CLOSING_CLOSING))
static icl_stats_t *s_demo_server_agent_dialog_call_connection_closing_closing_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_READ_BASIC_CONTENT_HEADER))
static icl_stats_t *s_demo_server_agent_dialog_call_read_basic_content_header_stats = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//  Function prototypes                                                        

static int    demo_server_agent_manager       (smt_thread_t **thread);
static int    demo_server_agent_catcher       (smt_thread_t *thread);
static int    demo_server_agent_master_destroy  (smt_thread_t *thread);
static int    demo_server_agent_connection_destroy  (smt_thread_t *thread);
static int    demo_server_agent_input_destroy  (smt_thread_t *thread);
static int    demo_server_agent_heartbeat_destroy  (smt_thread_t *thread);
static void   find_thread_state_next_state    (int          *thread_type,
                                               smt_state_t  *state_id,
                                               smt_event_t  *event_id,
                                               smt_state_t  *nextstate_id,
                                               smt_thread_t *thread);
static void   report_unrecognised_event_error (smt_thread_t *thread);
static void   report_state_machine_error      (smt_thread_t *thread);

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
static void   animate_start_new_event         (smt_thread_t *thread,
                                               smt_event_t  saveevent_id);
static void   animate_action                  (smt_thread_t *thread_id,
                                               int          action_id);
static int    find_thread_type                (smt_place_t  place);
#endif
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_SERVER_AGENT)          ||  defined (DEMO_SERVER_AGENT_TRACE))
static void   s_dump                          (icl_os_thread_t apr_os_thread,
                                               apr_time_t      time,
                                               qbyte           info);

#endif

static int    action_initialise_server        (smt_thread_t *thread);
static int    action_allow_thread_to_settle   (smt_thread_t *thread);
static int    action_report_server_ready      (smt_thread_t *thread);
static int    action_accept_connection        (smt_thread_t *thread);
static int    action_create_connection_thread  (smt_thread_t *thread);
static int    action_report_accept_error      (smt_thread_t *thread);
static int    action_report_smt_error         (smt_thread_t *thread);
static int    action_close_master_socket      (smt_thread_t *thread);
static int    action_start_setup_timer        (smt_thread_t *thread);
static int    action_read_protocol_header     (smt_thread_t *thread);
static int    action_check_protocol_header    (smt_thread_t *thread);
static int    action_write_desired_header     (smt_thread_t *thread);
static int    action_accept_direct_connection  (smt_thread_t *thread);
static int    action_read_direct_request      (smt_thread_t *thread);
static int    action_check_direct_lease       (smt_thread_t *thread);
static int    action_report_bad_protocol      (smt_thread_t *thread);
static int    action_report_version_mismatch  (smt_thread_t *thread);
static int    action_start_input_thread       (smt_thread_t *thread);
static int    action_send_connection_start    (smt_thread_t *thread);
static int    action_send_connection_tune     (smt_thread_t *thread);
static int    action_start_heartbeat_thread   (smt_thread_t *thread);
static int    action_start_heartbeat_timer    (smt_thread_t *thread);
static int    action_report_setup_timeout     (smt_thread_t *thread);
static int    action_send_channel_flow        (smt_thread_t *thread);
static int    action_send_channel_flow_ok     (smt_thread_t *thread);
static int    action_send_exchange_declare_ok  (smt_thread_t *thread);
static int    action_send_exchange_delete_ok  (smt_thread_t *thread);
static int    action_send_queue_declare_ok    (smt_thread_t *thread);
static int    action_send_queue_bind_ok       (smt_thread_t *thread);
static int    action_send_queue_purge_ok      (smt_thread_t *thread);
static int    action_send_queue_delete_ok     (smt_thread_t *thread);
static int    action_send_basic_consume_ok    (smt_thread_t *thread);
static int    action_send_basic_cancel_ok     (smt_thread_t *thread);
static int    action_send_basic_deliver       (smt_thread_t *thread);
static int    action_send_basic_get_ok        (smt_thread_t *thread);
static int    action_send_basic_get_empty     (smt_thread_t *thread);
static int    action_send_direct_put_ok       (smt_thread_t *thread);
static int    action_send_direct_get_ok       (smt_thread_t *thread);
static int    action_send_connection_open_ok  (smt_thread_t *thread);
static int    action_send_channel_open_ok     (smt_thread_t *thread);
static int    action_send_channel_close_ok    (smt_thread_t *thread);
static int    action_push_method_to_client    (smt_thread_t *thread);
static int    action_send_channel_close       (smt_thread_t *thread);
static int    action_send_content_header      (smt_thread_t *thread);
static int    action_send_content_body_frame  (smt_thread_t *thread);
static int    action_shut_down_slave_threads  (smt_thread_t *thread);
static int    action_send_connection_close    (smt_thread_t *thread);
static int    action_execute_connection_close  (smt_thread_t *thread);
static int    action_send_connection_close_ok  (smt_thread_t *thread);
static int    action_flush_the_connection     (smt_thread_t *thread);
static int    action_report_socket_error      (smt_thread_t *thread);
static int    action_report_socket_timeout    (smt_thread_t *thread);
static int    action_send_heartbeat_frame     (smt_thread_t *thread);
static int    action_deliver_direct_contents  (smt_thread_t *thread);
static int    action_write_direct_content     (smt_thread_t *thread);
static int    action_write_direct_heartbeat   (smt_thread_t *thread);
static int    action_read_frame_header        (smt_thread_t *thread);
static int    action_read_frame_payload       (smt_thread_t *thread);
static int    action_check_frame_format       (smt_thread_t *thread);
static int    action_decode_method            (smt_thread_t *thread);
static int    action_check_method_class       (smt_thread_t *thread);
static int    action_check_connection_method  (smt_thread_t *thread);
static int    action_check_channel_method     (smt_thread_t *thread);
static int    action_execute_functional_method  (smt_thread_t *thread);
static int    action_execute_connection_start_ok  (smt_thread_t *thread);
static int    action_notify_connection_of_start_ok  (smt_thread_t *thread);
static int    action_execute_connection_tune_ok  (smt_thread_t *thread);
static int    action_notify_connection_of_tune_ok  (smt_thread_t *thread);
static int    action_execute_connection_open  (smt_thread_t *thread);
static int    action_notify_connection_of_open  (smt_thread_t *thread);
static int    action_notify_connection_of_passive_close  (smt_thread_t *thread);
static int    action_notify_connection_of_close_ok  (smt_thread_t *thread);
static int    action_execute_channel_open     (smt_thread_t *thread);
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
static int    action_wait_for_heartbeat_interval  (smt_thread_t *thread);
static int    action_check_peer_liveness      (smt_thread_t *thread);
static int    action_report_peer_slowing      (smt_thread_t *thread);
static int    action_shut_down_agent          (smt_thread_t *thread);
static int    action_report_peer_stopped      (smt_thread_t *thread);
static int    action_dialog_call_close_the_connection_passive_close  (smt_thread_t *thread);
static int    action_dialog_return            (smt_thread_t *thread);
static int    action_unnamed_29               (smt_thread_t *thread);
static int    action_unnamed_30               (smt_thread_t *thread);
static int    action_unnamed_31               (smt_thread_t *thread);
static int    action_unnamed_34               (smt_thread_t *thread);
static int    action_dialog_call_close_the_connection_active_close  (smt_thread_t *thread);
static int    action_unnamed_36               (smt_thread_t *thread);
static int    action_dialog_call_send_content_body_if_any_start  (smt_thread_t *thread);
static int    action_unnamed_58               (smt_thread_t *thread);
static int    action_unnamed_67               (smt_thread_t *thread);
static int    action_unnamed_68               (smt_thread_t *thread);
static int    action_unnamed_69               (smt_thread_t *thread);
static int    action_unnamed_70               (smt_thread_t *thread);
static int    action_unnamed_71               (smt_thread_t *thread);
static int    action_dialog_call_read_method_read_method  (smt_thread_t *thread);
static int    action_dialog_call_connection_closing_closing  (smt_thread_t *thread);
static int    action_dialog_call_read_basic_content_header  (smt_thread_t *thread);
static int
demo_server_agent_set_trace_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    int            trace);
int
demo_server_agent_set_trace_destructor (void *data);
static int
demo_server_agent_connection_error_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          reply_code,
    char *         reply_text,
    dbyte          faulting_method_id,
    dbyte          faulting_class_id);
int
demo_server_agent_connection_error_destructor (void *data);
static int
demo_server_agent_channel_error_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_channel_t *  channel,
    dbyte          reply_code,
    char *         reply_text,
    dbyte          faulting_method_id,
    dbyte          faulting_class_id);
int
demo_server_agent_channel_error_destructor (void *data);
static int
demo_server_agent_channel_flow_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active);
int
demo_server_agent_channel_flow_destructor (void *data);
static int
demo_server_agent_channel_flow_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active);
int
demo_server_agent_channel_flow_ok_destructor (void *data);
static int
demo_server_agent_exchange_declare_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr);
int
demo_server_agent_exchange_declare_ok_destructor (void *data);
static int
demo_server_agent_exchange_delete_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr);
int
demo_server_agent_exchange_delete_ok_destructor (void *data);
static int
demo_server_agent_queue_declare_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    icl_longstr_t *  properties);
int
demo_server_agent_queue_declare_ok_destructor (void *data);
static int
demo_server_agent_queue_bind_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr);
int
demo_server_agent_queue_bind_ok_destructor (void *data);
static int
demo_server_agent_queue_purge_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    qbyte          message_count);
int
demo_server_agent_queue_purge_ok_destructor (void *data);
static int
demo_server_agent_queue_delete_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    qbyte          message_count);
int
demo_server_agent_queue_delete_ok_destructor (void *data);
static int
demo_server_agent_basic_consume_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag);
int
demo_server_agent_basic_consume_ok_destructor (void *data);
static int
demo_server_agent_basic_cancel_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag);
int
demo_server_agent_basic_cancel_ok_destructor (void *data);
static int
demo_server_agent_basic_deliver_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key,
    int64_t        delivery_tag,
    void *         callback);
int
demo_server_agent_basic_deliver_destructor (void *data);
static int
demo_server_agent_basic_get_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key,
    qbyte          message_count,
    void *         callback);
int
demo_server_agent_basic_get_ok_destructor (void *data);
static int
demo_server_agent_basic_get_empty_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr);
int
demo_server_agent_basic_get_empty_destructor (void *data);
static int
demo_server_agent_direct_put_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         lease);
int
demo_server_agent_direct_put_ok_destructor (void *data);
static int
demo_server_agent_direct_get_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         lease);
int
demo_server_agent_direct_get_ok_destructor (void *data);
static int
demo_server_agent_push_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_server_method_t *  method);
int
demo_server_agent_push_destructor (void *data);
static int
demo_server_agent_direct_out_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_content_basic_t *  content,
    byte           options);
int
demo_server_agent_direct_out_destructor (void *data);
//  Function definitions                                                       

static int
demo_server_agent_set_trace_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    int            trace)
{
    int
        rc = 0;
    demo_server_agent_set_trace_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_set_trace_cache);
    _message->trace                     = trace;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_HIGH,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_set_trace_destructor);
    
    return rc;
}

int
demo_server_agent_set_trace_destructor (void *data)
{
    int
        rc = 0;
int trace;

    trace = ((demo_server_agent_set_trace_t *) data)-> trace;


    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_connection_error_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          reply_code,
    char *         reply_text,
    dbyte          faulting_method_id,
    dbyte          faulting_class_id)
{
    int
        rc = 0;
    demo_server_agent_connection_error_t
        *_message;
        

    _message = icl_mem_cache_alloc (s_connection_error_cache);
    _message->reply_code                = reply_code;
    _message->reply_text                = reply_text;
    _message->faulting_method_id        = faulting_method_id;
    _message->faulting_class_id         = faulting_class_id;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_HIGH,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_connection_error_destructor);
    
    return rc;
}

int
demo_server_agent_connection_error_destructor (void *data)
{
    int
        rc = 0;
dbyte reply_code;
char * reply_text;
dbyte faulting_method_id;
dbyte faulting_class_id;

    reply_code = ((demo_server_agent_connection_error_t *) data)-> reply_code;
    reply_text = ((demo_server_agent_connection_error_t *) data)-> reply_text;
    faulting_method_id = ((demo_server_agent_connection_error_t *) data)-> faulting_method_id;
    faulting_class_id = ((demo_server_agent_connection_error_t *) data)-> faulting_class_id;


    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_channel_error_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_server_channel_t *  channel,
    dbyte          reply_code,
    char *         reply_text,
    dbyte          faulting_method_id,
    dbyte          faulting_class_id)
{
    int
        rc = 0;
    demo_server_agent_channel_error_t
        *_message;
        
    //  Possess code for channel error
    
        channel = demo_server_channel_link (channel);
    

    _message = icl_mem_cache_alloc (s_channel_error_cache);
    _message->channel                   = channel;
    _message->reply_code                = reply_code;
    _message->reply_text                = reply_text;
    _message->faulting_method_id        = faulting_method_id;
    _message->faulting_class_id         = faulting_class_id;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_HIGH,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_channel_error_destructor);
    
    return rc;
}

int
demo_server_agent_channel_error_destructor (void *data)
{
    int
        rc = 0;
demo_server_channel_t * channel;
dbyte reply_code;
char * reply_text;
dbyte faulting_method_id;
dbyte faulting_class_id;

    channel = ((demo_server_agent_channel_error_t *) data)-> channel;
    reply_code = ((demo_server_agent_channel_error_t *) data)-> reply_code;
    reply_text = ((demo_server_agent_channel_error_t *) data)-> reply_text;
    faulting_method_id = ((demo_server_agent_channel_error_t *) data)-> faulting_method_id;
    faulting_class_id = ((demo_server_agent_channel_error_t *) data)-> faulting_class_id;


        demo_server_channel_unlink (&channel);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_channel_flow_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active)
{
    int
        rc = 0;
    demo_server_agent_channel_flow_t
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
                          demo_server_agent_channel_flow_destructor);
    
    return rc;
}

int
demo_server_agent_channel_flow_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
Bool active;

    channel_nbr = ((demo_server_agent_channel_flow_t *) data)-> channel_nbr;
    active = ((demo_server_agent_channel_flow_t *) data)-> active;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_channel_flow_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    Bool           active)
{
    int
        rc = 0;
    demo_server_agent_channel_flow_ok_t
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
                          demo_server_agent_channel_flow_ok_destructor);
    
    return rc;
}

int
demo_server_agent_channel_flow_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
Bool active;

    channel_nbr = ((demo_server_agent_channel_flow_ok_t *) data)-> channel_nbr;
    active = ((demo_server_agent_channel_flow_ok_t *) data)-> active;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_exchange_declare_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_server_agent_exchange_declare_ok_t
        *_message;
        
    //  Possess code for exchange declare-ok
    

    _message = icl_mem_cache_alloc (s_exchange_declare_ok_cache);
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_exchange_declare_ok_destructor);
    
    return rc;
}

int
demo_server_agent_exchange_declare_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;

    channel_nbr = ((demo_server_agent_exchange_declare_ok_t *) data)-> channel_nbr;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_exchange_delete_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_server_agent_exchange_delete_ok_t
        *_message;
        
    //  Possess code for exchange delete-ok
    

    _message = icl_mem_cache_alloc (s_exchange_delete_ok_cache);
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_exchange_delete_ok_destructor);
    
    return rc;
}

int
demo_server_agent_exchange_delete_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;

    channel_nbr = ((demo_server_agent_exchange_delete_ok_t *) data)-> channel_nbr;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_queue_declare_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         queue,
    icl_longstr_t *  properties)
{
    int
        rc = 0;
    demo_server_agent_queue_declare_ok_t
        *_message;
        
    //  Possess code for queue declare-ok
    
        queue = icl_mem_strdup (queue);
        properties = icl_longstr_dup (properties);
    

    _message = icl_mem_cache_alloc (s_queue_declare_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->queue                     = queue;
    _message->properties                = properties;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_queue_declare_ok_destructor);
    
    return rc;
}

int
demo_server_agent_queue_declare_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * queue;
icl_longstr_t * properties;

    channel_nbr = ((demo_server_agent_queue_declare_ok_t *) data)-> channel_nbr;
    queue = ((demo_server_agent_queue_declare_ok_t *) data)-> queue;
    properties = ((demo_server_agent_queue_declare_ok_t *) data)-> properties;


        icl_mem_free (queue);
        icl_longstr_destroy (&properties);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_queue_bind_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_server_agent_queue_bind_ok_t
        *_message;
        
    //  Possess code for queue bind-ok
    

    _message = icl_mem_cache_alloc (s_queue_bind_ok_cache);
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_queue_bind_ok_destructor);
    
    return rc;
}

int
demo_server_agent_queue_bind_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;

    channel_nbr = ((demo_server_agent_queue_bind_ok_t *) data)-> channel_nbr;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_queue_purge_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    qbyte          message_count)
{
    int
        rc = 0;
    demo_server_agent_queue_purge_ok_t
        *_message;
        
    //  Possess code for queue purge-ok
    

    _message = icl_mem_cache_alloc (s_queue_purge_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->message_count             = message_count;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_queue_purge_ok_destructor);
    
    return rc;
}

int
demo_server_agent_queue_purge_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
qbyte message_count;

    channel_nbr = ((demo_server_agent_queue_purge_ok_t *) data)-> channel_nbr;
    message_count = ((demo_server_agent_queue_purge_ok_t *) data)-> message_count;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_queue_delete_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    qbyte          message_count)
{
    int
        rc = 0;
    demo_server_agent_queue_delete_ok_t
        *_message;
        
    //  Possess code for queue delete-ok
    

    _message = icl_mem_cache_alloc (s_queue_delete_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->message_count             = message_count;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_queue_delete_ok_destructor);
    
    return rc;
}

int
demo_server_agent_queue_delete_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
qbyte message_count;

    channel_nbr = ((demo_server_agent_queue_delete_ok_t *) data)-> channel_nbr;
    message_count = ((demo_server_agent_queue_delete_ok_t *) data)-> message_count;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_basic_consume_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag)
{
    int
        rc = 0;
    demo_server_agent_basic_consume_ok_t
        *_message;
        
    //  Possess code for basic consume-ok
    
        consumer_tag = icl_mem_strdup (consumer_tag);
    

    _message = icl_mem_cache_alloc (s_basic_consume_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->consumer_tag              = consumer_tag;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_basic_consume_ok_destructor);
    
    return rc;
}

int
demo_server_agent_basic_consume_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * consumer_tag;

    channel_nbr = ((demo_server_agent_basic_consume_ok_t *) data)-> channel_nbr;
    consumer_tag = ((demo_server_agent_basic_consume_ok_t *) data)-> consumer_tag;


        icl_mem_free (consumer_tag);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_basic_cancel_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         consumer_tag)
{
    int
        rc = 0;
    demo_server_agent_basic_cancel_ok_t
        *_message;
        
    //  Possess code for basic cancel-ok
    
        consumer_tag = icl_mem_strdup (consumer_tag);
    

    _message = icl_mem_cache_alloc (s_basic_cancel_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->consumer_tag              = consumer_tag;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_basic_cancel_ok_destructor);
    
    return rc;
}

int
demo_server_agent_basic_cancel_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * consumer_tag;

    channel_nbr = ((demo_server_agent_basic_cancel_ok_t *) data)-> channel_nbr;
    consumer_tag = ((demo_server_agent_basic_cancel_ok_t *) data)-> consumer_tag;


        icl_mem_free (consumer_tag);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_basic_deliver_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key,
    int64_t        delivery_tag,
    void *         callback)
{
    int
        rc = 0;
    demo_server_agent_basic_deliver_t
        *_message;
        
    //  Possess code for basic deliver
    
        assert (content);
        content  = demo_content_basic_link (content);
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
    

    _message = icl_mem_cache_alloc (s_basic_deliver_cache);
    _message->channel_nbr               = channel_nbr;
    _message->content                   = content;
    _message->exchange                  = exchange;
    _message->routing_key               = routing_key;
    _message->delivery_tag              = delivery_tag;
    _message->callback                  = callback;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_basic_deliver_destructor);
    
    return rc;
}

int
demo_server_agent_basic_deliver_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
demo_content_basic_t * content;
char * exchange;
char * routing_key;
int64_t delivery_tag;
void * callback;

    channel_nbr = ((demo_server_agent_basic_deliver_t *) data)-> channel_nbr;
    content = ((demo_server_agent_basic_deliver_t *) data)-> content;
    exchange = ((demo_server_agent_basic_deliver_t *) data)-> exchange;
    routing_key = ((demo_server_agent_basic_deliver_t *) data)-> routing_key;
    delivery_tag = ((demo_server_agent_basic_deliver_t *) data)-> delivery_tag;
    callback = ((demo_server_agent_basic_deliver_t *) data)-> callback;


        demo_content_basic_unlink (&content);
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_basic_get_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_content_basic_t *  content,
    char *         exchange,
    char *         routing_key,
    qbyte          message_count,
    void *         callback)
{
    int
        rc = 0;
    demo_server_agent_basic_get_ok_t
        *_message;
        
    //  Possess code for basic get-ok
    
        assert (content);
        content  = demo_content_basic_link (content);
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
    

    _message = icl_mem_cache_alloc (s_basic_get_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->content                   = content;
    _message->exchange                  = exchange;
    _message->routing_key               = routing_key;
    _message->message_count             = message_count;
    _message->callback                  = callback;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_basic_get_ok_destructor);
    
    return rc;
}

int
demo_server_agent_basic_get_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
demo_content_basic_t * content;
char * exchange;
char * routing_key;
qbyte message_count;
void * callback;

    channel_nbr = ((demo_server_agent_basic_get_ok_t *) data)-> channel_nbr;
    content = ((demo_server_agent_basic_get_ok_t *) data)-> content;
    exchange = ((demo_server_agent_basic_get_ok_t *) data)-> exchange;
    routing_key = ((demo_server_agent_basic_get_ok_t *) data)-> routing_key;
    message_count = ((demo_server_agent_basic_get_ok_t *) data)-> message_count;
    callback = ((demo_server_agent_basic_get_ok_t *) data)-> callback;


        demo_content_basic_unlink (&content);
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_basic_get_empty_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr)
{
    int
        rc = 0;
    demo_server_agent_basic_get_empty_t
        *_message;
        
    //  Possess code for basic get-empty
    

    _message = icl_mem_cache_alloc (s_basic_get_empty_cache);
    _message->channel_nbr               = channel_nbr;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_basic_get_empty_destructor);
    
    return rc;
}

int
demo_server_agent_basic_get_empty_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;

    channel_nbr = ((demo_server_agent_basic_get_empty_t *) data)-> channel_nbr;



    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_direct_put_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         lease)
{
    int
        rc = 0;
    demo_server_agent_direct_put_ok_t
        *_message;
        
    //  Possess code for direct put-ok
    
        lease = icl_mem_strdup (lease);
    

    _message = icl_mem_cache_alloc (s_direct_put_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->lease                     = lease;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_direct_put_ok_destructor);
    
    return rc;
}

int
demo_server_agent_direct_put_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * lease;

    channel_nbr = ((demo_server_agent_direct_put_ok_t *) data)-> channel_nbr;
    lease = ((demo_server_agent_direct_put_ok_t *) data)-> lease;


        icl_mem_free (lease);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_direct_get_ok_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    char *         lease)
{
    int
        rc = 0;
    demo_server_agent_direct_get_ok_t
        *_message;
        
    //  Possess code for direct get-ok
    
        lease = icl_mem_strdup (lease);
    

    _message = icl_mem_cache_alloc (s_direct_get_ok_cache);
    _message->channel_nbr               = channel_nbr;
    _message->lease                     = lease;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_direct_get_ok_destructor);
    
    return rc;
}

int
demo_server_agent_direct_get_ok_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
char * lease;

    channel_nbr = ((demo_server_agent_direct_get_ok_t *) data)-> channel_nbr;
    lease = ((demo_server_agent_direct_get_ok_t *) data)-> lease;


        icl_mem_free (lease);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_push_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    dbyte          channel_nbr,
    demo_server_method_t *  method)
{
    int
        rc = 0;
    demo_server_agent_push_t
        *_message;
        
    //  Possess code for push
    
        method = demo_server_method_link (method);
    

    _message = icl_mem_cache_alloc (s_push_cache);
    _message->channel_nbr               = channel_nbr;
    _message->method                    = method;

    rc = smt_method_send (_queue,           //  The queue
                          _event,           //  The event
                          SMT_PRIORITY_NORMAL,
                          SMT_OK,           //  Always OK
                          _message,         //  The message
                          demo_server_agent_push_destructor);
    
    return rc;
}

int
demo_server_agent_push_destructor (void *data)
{
    int
        rc = 0;
dbyte channel_nbr;
demo_server_method_t * method;

    channel_nbr = ((demo_server_agent_push_t *) data)-> channel_nbr;
    method = ((demo_server_agent_push_t *) data)-> method;


        demo_server_method_unlink (&method);
    

    icl_mem_free (data);
    return rc;
}


static int
demo_server_agent_direct_out_send (
    smt_method_queue_t *_queue,
    smt_event_t        _event,
    demo_content_basic_t *  content,
    byte           options)
{
    int
        rc = 0;
    demo_server_agent_direct_out_t
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
                          demo_server_agent_direct_out_destructor);
    
    return rc;
}

int
demo_server_agent_direct_out_destructor (void *data)
{
    int
        rc = 0;
demo_content_basic_t * content;
byte options;

    content = ((demo_server_agent_direct_out_t *) data)-> content;
    options = ((demo_server_agent_direct_out_t *) data)-> options;


        demo_content_basic_unlink (&content);
    

    icl_mem_free (data);
    return rc;
}


int
demo_server_agent_kill_connection (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                kill_connection_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_connection_start_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_start_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_connection_tune_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_tune_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_connection_open (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                connection_open_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_send_channel_open_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = smt_method_send (
                thread->reply_queue,
                send_channel_open_ok_m_event,
                SMT_PRIORITY_HIGH,
                SMT_OK, NULL, NULL);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_send_channel_close_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
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
demo_server_agent_connection_close_ok (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
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
demo_server_agent_shutdown (
    smt_thread_t * thread)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
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
demo_server_agent_set_trace (
    smt_thread_t * thread,
    int                   trace)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_set_trace_send (
                thread->reply_queue, set_trace_m_event,
                trace);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_connection_error (
    smt_thread_t * thread,
    dbyte                 reply_code,
    char *                reply_text,
    dbyte                 faulting_method_id,
    dbyte                 faulting_class_id)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_connection_error_send (
                thread->reply_queue, connection_error_m_event,
                reply_code,
                reply_text,
                faulting_method_id,
                faulting_class_id);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_channel_error (
    smt_thread_t * thread,
    demo_server_channel_t * channel,
    dbyte                 reply_code,
    char *                reply_text,
    dbyte                 faulting_method_id,
    dbyte                 faulting_class_id)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_channel_error_send (
                thread->reply_queue, channel_error_m_event,
                channel,
                reply_code,
                reply_text,
                faulting_method_id,
                faulting_class_id);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_channel_flow (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_channel_flow_send (
                thread->reply_queue, channel_flow_m_event,
                channel_nbr,
                active);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_channel_flow_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_channel_flow_ok_send (
                thread->reply_queue, channel_flow_ok_m_event,
                channel_nbr,
                active);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_exchange_declare_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_exchange_declare_ok_send (
                thread->reply_queue, exchange_declare_ok_m_event,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_exchange_delete_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_exchange_delete_ok_send (
                thread->reply_queue, exchange_delete_ok_m_event,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_queue_declare_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue,
    icl_longstr_t *       properties)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_queue_declare_ok_send (
                thread->reply_queue, queue_declare_ok_m_event,
                channel_nbr,
                queue,
                properties);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_queue_bind_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_queue_bind_ok_send (
                thread->reply_queue, queue_bind_ok_m_event,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_queue_purge_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    qbyte                 message_count)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_queue_purge_ok_send (
                thread->reply_queue, queue_purge_ok_m_event,
                channel_nbr,
                message_count);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_queue_delete_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    qbyte                 message_count)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_queue_delete_ok_send (
                thread->reply_queue, queue_delete_ok_m_event,
                channel_nbr,
                message_count);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_basic_consume_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                consumer_tag)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_basic_consume_ok_send (
                thread->reply_queue, basic_consume_ok_m_event,
                channel_nbr,
                consumer_tag);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_basic_cancel_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                consumer_tag)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_basic_cancel_ok_send (
                thread->reply_queue, basic_cancel_ok_m_event,
                channel_nbr,
                consumer_tag);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_basic_deliver (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_content_basic_t * content,
    char *                exchange,
    char *                routing_key,
    int64_t               delivery_tag,
    void *                callback)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_basic_deliver_send (
                thread->reply_queue, basic_deliver_m_event,
                channel_nbr,
                content,
                exchange,
                routing_key,
                delivery_tag,
                callback);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_basic_get_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_content_basic_t * content,
    char *                exchange,
    char *                routing_key,
    qbyte                 message_count,
    void *                callback)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_basic_get_ok_send (
                thread->reply_queue, basic_get_ok_m_event,
                channel_nbr,
                content,
                exchange,
                routing_key,
                message_count,
                callback);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_basic_get_empty (
    smt_thread_t * thread,
    dbyte                 channel_nbr)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_basic_get_empty_send (
                thread->reply_queue, basic_get_empty_m_event,
                channel_nbr);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_direct_put_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                lease)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_direct_put_ok_send (
                thread->reply_queue, direct_put_ok_m_event,
                channel_nbr,
                lease);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_direct_get_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                lease)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_direct_get_ok_send (
                thread->reply_queue, direct_get_ok_m_event,
                channel_nbr,
                lease);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_push (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_server_method_t * method)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_push_send (
                thread->reply_queue, push_m_event,
                channel_nbr,
                method);
    }
    else
        rc = -1;
        
    return rc;
}

int
demo_server_agent_direct_out (
    smt_thread_t * thread,
    demo_content_basic_t * content,
    byte                  options)
{
    int
        rc = 0;

    //  Trying to send to a null of zombied thread returns an error            
    if (thread && !thread->zombie) {
        assert (thread->manager == demo_server_agent_manager);
        rc = demo_server_agent_direct_out_send (
                thread->reply_queue, direct_out_m_event,
                content,
                options);
    }
    else
        rc = -1;
        
    return rc;
}


#define tcb ((demo_server_agent_connection_context_t *) thread->context)

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

    //  Raise an exception in the server connection object so that
    //  the input thread will pick up on this at the next frame boundary
    //  and abort processing any current method/content
    demo_server_connection_raise_exception (tcb->connection,
        reply_code, reply_text, faulting_method_id, faulting_class_id);
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
        smt_log_print (demo_broker->alert_log,
            "E: %s error: %d %s "
            "(faulting method %u, %u), %s "  "(%s, %s, %s, %s)",
            context, reply_code, reply_text,
            faulting_class_id, faulting_method_id, action_taken,
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    else
        smt_log_print (demo_broker->alert_log,
            "E: %s error: %d %s, %s "  "(%s, %s, %s, %s)",
            context, reply_code, reply_text, action_taken,
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
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
    //  Trap possible async exception before executing method
    if (demo_server_connection_exception_raised (tcb->connection))
        smt_thread_raise_exception (thread, async_connection_error_event);
    else
    if (tcb->method_in->class_id  == class_id
    &&  tcb->method_in->method_id == method_id) {
        demo_server_method_execute (tcb->method_in, tcb->connection,
            channel_method ? tcb->channel : NULL);
        if (demo_server_connection_exception_raised (tcb->connection))
            s_connection_exception (thread,
                tcb->connection->reply_code,
                tcb->connection->reply_text,
                tcb->connection->faulting_class_id,
                tcb->connection->faulting_method_id);
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
            demo_server_method_execute (tcb->method_in, tcb->connection,
                tcb->channel);
            if (demo_server_connection_exception_raised (tcb->connection))
                s_connection_exception (thread,
                    tcb->connection->reply_code,
                    tcb->connection->reply_text,
                    tcb->connection->faulting_class_id,
                    tcb->connection->faulting_method_id);
    }
    else
        s_connection_exception (thread,
            ASL_SYNTAX_ERROR, "Method invalid - unknown method id",
            method_id, class_id);
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
        demo_server_method_dump (tcb->method_out, log, "I: send ");

    //  Send the method
    payload = demo_server_method_encode (tcb->method_out);
    s_write_payload (thread, channel, ASL_FRAME_METHOD, &payload);

    tcb->out_class_id  = tcb->method_out->class_id;
    tcb->out_method_id = tcb->method_out->method_id;

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
    tcb->connection->traffic_out += size;
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
    tcb->connection->traffic_out += bucket->cur_size;
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
        tcb->connection->traffic_in += size;
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
        tcb->connection->traffic_in += size;
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
demo_server_agent_term (void)
{

}

int 
demo_server_agent_init (
    smt_thread_t ** thread_p)
{
    int
        rc = SMT_OK;                    //  Return code

    s_connection_context_cache = icl_cache_get (sizeof (demo_server_agent_connection_context_t));
    s_master_context_cache = icl_cache_get (sizeof (demo_server_agent_master_context_t));
    s_set_trace_cache = icl_cache_get (sizeof (demo_server_agent_set_trace_t));
    s_connection_error_cache = icl_cache_get (sizeof (demo_server_agent_connection_error_t));
    s_channel_error_cache = icl_cache_get (sizeof (demo_server_agent_channel_error_t));
    s_channel_flow_cache = icl_cache_get (sizeof (demo_server_agent_channel_flow_t));
    s_channel_flow_ok_cache = icl_cache_get (sizeof (demo_server_agent_channel_flow_ok_t));
    s_exchange_declare_ok_cache = icl_cache_get (sizeof (demo_server_agent_exchange_declare_ok_t));
    s_exchange_delete_ok_cache = icl_cache_get (sizeof (demo_server_agent_exchange_delete_ok_t));
    s_queue_declare_ok_cache = icl_cache_get (sizeof (demo_server_agent_queue_declare_ok_t));
    s_queue_bind_ok_cache = icl_cache_get (sizeof (demo_server_agent_queue_bind_ok_t));
    s_queue_purge_ok_cache = icl_cache_get (sizeof (demo_server_agent_queue_purge_ok_t));
    s_queue_delete_ok_cache = icl_cache_get (sizeof (demo_server_agent_queue_delete_ok_t));
    s_basic_consume_ok_cache = icl_cache_get (sizeof (demo_server_agent_basic_consume_ok_t));
    s_basic_cancel_ok_cache = icl_cache_get (sizeof (demo_server_agent_basic_cancel_ok_t));
    s_basic_deliver_cache = icl_cache_get (sizeof (demo_server_agent_basic_deliver_t));
    s_basic_get_ok_cache = icl_cache_get (sizeof (demo_server_agent_basic_get_ok_t));
    s_basic_get_empty_cache = icl_cache_get (sizeof (demo_server_agent_basic_get_empty_t));
    s_direct_put_ok_cache = icl_cache_get (sizeof (demo_server_agent_direct_put_ok_t));
    s_direct_get_ok_cache = icl_cache_get (sizeof (demo_server_agent_direct_get_ok_t));
    s_push_cache = icl_cache_get (sizeof (demo_server_agent_push_t));
    s_direct_out_cache = icl_cache_get (sizeof (demo_server_agent_direct_out_t));

    //  Initialise basic stuff.
    smt_initialise ();
    
    {

    smt_thread_t
        *thread;

    rc = SMT_OK;
    if (demo_server_config_trace (demo_server_config) >= ASL_TRACE_HIGH)
        smt_socket_request_trace (TRUE);
    thread = demo_server_agent_master_thread_new ();
    *thread_p = thread;
    smt_thread_wake (thread);
    smt_thread_unlink (&thread);

    }
    icl_system_register (NULL, demo_server_agent_term);
    
    return rc;
}

void
demo_server_agent_animate (Bool animate)
{
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    s_animate = animate;
#endif
}

smt_thread_t *
demo_server_agent_master_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_server_agent_manager,
                             demo_server_agent_catcher,
                             demo_server_agent_master_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_server_agent",
                master_thread_name);
#endif

        thread->place   = 1;
        thread->context = icl_mem_cache_alloc (s_master_context_cache);
        memset (thread->context, 0, sizeof (demo_server_agent_master_context_t));
        ((demo_server_agent_master_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((demo_server_agent_master_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_server_agent_connection_thread_new (void)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_server_agent_manager,
                             demo_server_agent_catcher,
                             demo_server_agent_connection_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_server_agent",
                connection_thread_name);
#endif

        thread->place   = 14;
        thread->context = icl_mem_cache_alloc (s_connection_context_cache);
        memset (thread->context, 0, sizeof (demo_server_agent_connection_context_t));
        ((demo_server_agent_connection_context_t *) thread->context)->links = 1;
        thread->event_name = event_name;
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_server_agent_input_thread_new (
    smt_thread_t * share)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_server_agent_manager,
                             demo_server_agent_catcher,
                             demo_server_agent_input_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_server_agent",
                input_thread_name);
#endif

        thread->place   = 125;
        smt_thread_share (thread, share);
        thread->event_name = event_name;
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


smt_thread_t *
demo_server_agent_heartbeat_thread_new (
    smt_thread_t * share)
{
    smt_thread_t
        *thread;

    thread = smt_thread_new (demo_server_agent_manager,
                             demo_server_agent_catcher,
                             demo_server_agent_heartbeat_destroy);
                             
    thread->schedule = SMT_SCHEDULE_POLL;

#if (defined (DEMO_SERVER_AGENT_TRACE))
    thread->trace = icl_trace_new (ICL_TRACE_SIZE);
#endif

    if (thread) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
        thread->animate = s_animate;
        if (s_animate)
            icl_console_print ("%4lu> %-15s/%-15s <Created>",
                thread->id,
                "demo_server_agent",
                heartbeat_thread_name);
#endif

        thread->place   = 213;
        smt_thread_share (thread, share);
        thread->event_name = event_name;
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
    }
    return thread;
}


static int
demo_server_agent_manager (smt_thread_t **thread_p)
{
    smt_thread_t
        *thread = *thread_p;
        
    int
        rc;
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))

    smt_event_t
        saveevent_id = SMT_NULL_EVENT;
#endif
        
    assert (thread->manager == demo_server_agent_manager);
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
            case 140:
            case 141:
            case 142:
            case 193:
            case 11:
            case 12:
            case 13:
            case 4:
            case 5:
            case 6:
            case 200:
            case 155:
            case 156:
            case 157:
            case 205:
            case 42:
            case 207:
            case 44:
            case 45:
            case 46:
            case 47:
            case 212:
            case 99:
            case 177:
            case 178:
            case 165:
            case 166:
            case 62:
            case 63:
            case 64:
            case 65:
            case 66:
            case 67:
            case 128:
            case 129:
            case 130:
            case 71:
            case 132:
            case 48:
            case 49:
            case 50:
            case 51:
            case 196:
            case 217:
            case 54:
            case 19:
            case 20:
            case 21:
            case 22:
            case 27:
            case 28:
            case 206:
            case 179:
            case 208:
            case 209:
            case 210:
            case 211:
            case 184:
            case 185:
            case 186:
            case 38:
            case 171:
            case 158:
            case 167:
            case 168:
            case 169:
            case 113:
            case 114:
            case 115:
            case 116:
            case 117:
            case 118:
            case 119:
            case 120:
            case 133:
            case 134:
            case 135:
            case 124:
            case 33:
            case 143:
            case 218:
            case 219:
            case 30:
            case 150:
            case 151:
            case 152:
            case 153:
            case 154:
            case 95:
            case 195:
            case 97:
            case 98:
            case 198:
            case 100:
            case 101:
            case 102:
            case 103:
            case 104:
            case 199:
            case 131:
            case 181:
            case 108:
            case 26:
            case 180:
            case 194:
            case 29:
            case 170:
            case 31:
            case 32:
            case 183:
            case 34:
            case 35:
            case 197:
            case 145:
            case 172:
            case 182:
            case 138:
            case 139:
            case 144:
                thread->place = SMT_TERM_PLACE;
                break;
            case 3:
            case 9:
            case 8:
            case 10:
                thread->place = 7;
                break;
            case 58:
            case 59:
            case 69:
            case 72:
            case 56:
            case 70:
            case 57:
            case 68:
            case 61:
            case 60:
                thread->place = 55;
                break;
            case 89:
            case 74:
            case 88:
            case 106:
            case 105:
            case 92:
            case 91:
            case 75:
            case 96:
            case 90:
            case 78:
            case 79:
            case 82:
            case 83:
            case 93:
            case 85:
            case 80:
            case 81:
            case 25:
            case 86:
            case 87:
            case 107:
            case 76:
            case 77:
            case 84:
            case 94:
                thread->place = 73;
                break;
            case 148:
            case 149:
            case 147:
                thread->place = 146;
                break;
            case 40:
            case 43:
            case 52:
            case 41:
            case 53:
            case 18:
                thread->place = 39;
                break;
            case 161:
                thread->place = 187;
                break;
            case 160:
                thread->place = 173;
                break;
            case 188:
            case 176:
            case 164:
            case 204:
            case 189:
            case 190:
            case 191:
            case 192:
            case 127:
            case 174:
            case 175:
            case 162:
            case 163:
                thread->place = 159;
                break;
            case 37:
            case 36:
            case 16:
            case 24:
            case 17:
            case 23:
                thread->place = 15;
                break;
            case 215:
            case 216:
                thread->place = 214;
                break;
            case 203:
            case 202:
                thread->place = 201;
                break;
            case 137:
                thread->place = 136;
                break;
            case 123:
            case 122:
            case 111:
            case 112:
            case 121:
            case 110:
                thread->place = 109;
                break;
        }    
    else
    if (thread->module == 0) {
        if (thread->place == 1) { //                Master thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_server_agent_master_context_t *) thread->context)

        if (demo_server_config_trace (demo_server_config) >= ASL_TRACE_MED)
            thread->animate = TRUE;
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
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        apr_sockaddr_t
            *sockaddr;                  //  Address of peer
        char
            *address_ip;                //  IP address of peer
        icl_shortstr_t
            address_full;               //  Formatted address

        //  Create demo_server_connection object
        tcb->connection = demo_server_connection_new (
            demo_broker->connections, thread);
        demo_server_connection_list_push (
            demo_broker->connection_list,
            tcb->connection);

        tcb->channel_state = ASL_CHANNEL_CLOSED;
        tcb->liveness      = FULL_LIVENESS;
        tcb->frame_max     = tcb->connection->frame_max;
        tcb->heartbeat     = tcb->connection->heartbeat;
        tcb->trace         = tcb->connection->trace;
        tcb->direct_heartbeat = 5;

        //  Set peer address for reporting
        apr_socket_addr_get (&sockaddr, APR_REMOTE,
            tcb->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&address_ip, sockaddr);
        icl_shortstr_fmt (address_full, "%s:%u", address_ip, sockaddr->port);
        demo_server_connection_set_address (tcb->connection, address_full);

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
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
        if (thread->place < 39)                 //  Initialise Connection state
            thread->place = 15;
        else
        if (thread->place < 55)                 //  Direct Connection state
            thread->place = 39;
        else
        if (thread->place < 73)                 //  Close The Connection state
            thread->place = 55;
        else
        if (thread->place < 109)                //  Connection Active state
            thread->place = 73;
        else
        if (thread->place < 125)                //  Send Content Body If Any state
            thread->place = 109;
        else
        if (thread->place == 125) { //              Input thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, ok_event);
    
#undef  tcb
}
            }
            thread->place = 126;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
        if (thread->place < 136)                //  Start state
            thread->place = 126;
        else
        if (thread->place < 146)                //  Read Method state
            thread->place = 136;
        else
        if (thread->place < 159)                //  Connection Closing state
            thread->place = 146;
        else
        if (thread->place < 173)                //  Have Incoming Method state
            thread->place = 159;
        else
        if (thread->place < 187)                //  Have Connection Method state
            thread->place = 173;
        else
        if (thread->place < 201)                //  Have Channel Method state
            thread->place = 187;
        else
        if (thread->place < 213)                //  Read Basic Content state
            thread->place = 201;
        else
        if (thread->place == 213) { //              Heartbeat thread
            if (!thread->shutting_down) {
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, peer_alive_event);
    
#undef  tcb
}
            }
            thread->place = 214;
            if (thread->_next_event == SMT_NULL_EVENT
            &&  smt_thread_method_accept (thread) == SMT_EMPTY) {
                thread->module = SMT_TERM_MODULE;
                return SMT_SLEEPING;
            }
        }
        else
            thread->place = 214;
    }

    switch (thread->place) {
        case 2:                         //  initialise master state             
{
#define tcb ((demo_server_agent_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 3; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 4; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 5; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 1);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 6; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 7:                         //  accepting connections state         
{
#define tcb ((demo_server_agent_master_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ready_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 2);
#endif
                    thread->place = 8; break;
                case new_connection_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 3);
#endif
                    thread->place = 9; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 10; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 11; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 12; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 2);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 13; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 15:                        //  initialise connection state         
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 16; break;
                case have_protocol_header_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 8);
#endif
                    thread->place = 17; break;
                case direct_protocol_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 9);
#endif
                    thread->place = 18; break;
                case bad_lease_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 25);
#endif
                    thread->place = 19; break;
                case bad_protocol_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 26);
#endif
                    thread->place = 20; break;
                case version_mismatch_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 27);
#endif
                    thread->place = 21; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 22; break;
                case connection_start_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 28);
#endif
                    thread->place = 23; break;
                case connection_tune_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 29);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 29);
#endif
                    thread->place = 24; break;
                case connection_open_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 30);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 30);
#endif
                    thread->place = 25; break;
                case connection_error_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
                    thread->place = 26; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 27; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 28; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 29; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 30; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 31; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 32; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 33; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 34; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 35; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 36; break;
                case set_trace_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 37; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 3);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 38; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 39:                        //  direct connection state             
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case direct_in_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 10);
#endif
                    thread->place = 40; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 41; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 42; break;
                case direct_out_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 13);
#endif
                    thread->place = 43; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 44; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 45; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 46; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 47; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 48; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 49; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 50; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 51; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 52; break;
                case set_trace_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 53; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 4);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 54; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 55:                        //  close the connection state          
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case active_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 16);
#endif
                    thread->place = 56; break;
                case passive_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 17);
#endif
                    thread->place = 57; break;
                case close_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 18);
#endif
                    thread->place = 58; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 59; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 60; break;
                case connection_close_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 20);
#endif
                    thread->place = 61; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 62; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 63; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 64; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 65; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 66; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 67; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 68; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 69; break;
                case set_trace_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 70; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 5);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 71; break;
                default:
                    thread->place = 72;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 73:                        //  connection active state             
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case discard_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
                    thread->place = 74; break;
                case channel_flow_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 32);
#endif
                    thread->place = 75; break;
                case channel_flow_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 33);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 33);
#endif
                    thread->place = 76; break;
                case exchange_declare_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 34);
#endif
                    thread->place = 77; break;
                case exchange_delete_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 35);
#endif
                    thread->place = 78; break;
                case queue_declare_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 36);
#endif
                    thread->place = 79; break;
                case queue_bind_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 37);
#endif
                    thread->place = 80; break;
                case queue_purge_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 38);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 38);
#endif
                    thread->place = 81; break;
                case queue_delete_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 39);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 39);
#endif
                    thread->place = 82; break;
                case basic_consume_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 40);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 40);
#endif
                    thread->place = 83; break;
                case basic_cancel_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 41);
#endif
                    thread->place = 84; break;
                case basic_deliver_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 42);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 42);
#endif
                    thread->place = 85; break;
                case basic_get_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 46);
#endif
                    thread->place = 86; break;
                case basic_get_empty_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 47);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 47);
#endif
                    thread->place = 87; break;
                case direct_put_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 48);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 48);
#endif
                    thread->place = 88; break;
                case direct_get_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 49);
#endif
                    thread->place = 89; break;
                case push_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 50);
#endif
                    thread->place = 90; break;
                case channel_error_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 51);
#endif
                    thread->place = 91; break;
                case send_channel_open_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 52);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 52);
#endif
                    thread->place = 92; break;
                case send_channel_close_ok_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 53);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 53);
#endif
                    thread->place = 93; break;
                case connection_error_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 54);
#endif
                    thread->place = 94; break;
                case shutdown_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 6);
#endif
                    thread->place = 95; break;
                case kill_connection_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 55);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 55);
#endif
                    thread->place = 96; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 97; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 98; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 99; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 100; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 101; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 102; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 103; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 104; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 105; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 106; break;
                case set_trace_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 107; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 6);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 108; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 109:                       //  send content body if any state      
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case start_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 43);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 43);
#endif
                    thread->place = 110; break;
                case continue_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 44);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 44);
#endif
                    thread->place = 111; break;
                case finished_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 45);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 45);
#endif
                    thread->place = 112; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 113; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 114; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 115; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 116; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 117; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 118; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 119; break;
                case external_exception_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 22);
#endif
                    thread->place = 120; break;
                case heartbeat_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 11);
#endif
                    thread->place = 121; break;
                case setup_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 23);
#endif
                    thread->place = 122; break;
                case set_trace_m_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 24);
#endif
                    thread->place = 123; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 7);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 124; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 126:                       //  start state                         
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 1);
#endif
                    thread->place = 127; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 128; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 129; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 130; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 131; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 132; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 133; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 134; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 8);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 135; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 136:                       //  read method state                   
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case read_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 56);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 56);
#endif
                    thread->place = 137; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 138; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 139; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 140; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 141; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 142; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 143; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 144; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 9);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 145; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 146:                       //  connection closing state            
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 57);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 57);
#endif
                    thread->place = 147; break;
                case method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
                    thread->place = 148; break;
                case discard_frame_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 59);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 59);
#endif
                    thread->place = 149; break;
                case connection_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
                    thread->place = 150; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 151; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 152; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 153; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 154; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 155; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 156; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 157; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 10);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 158; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 159:                       //  have incoming method state          
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connection_class_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 62);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 62);
#endif
                    thread->place = 160; break;
                case channel_class_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 66);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 66);
#endif
                    thread->place = 161; break;
                case basic_content_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 72);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 72);
#endif
                    thread->place = 162; break;
                case other_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 75);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 75);
#endif
                    thread->place = 163; break;
                case discard_method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 31);
#endif
                    thread->place = 164; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 165; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 166; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 167; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 168; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 169; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 170; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 171; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 11);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 172; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 173:                       //  have connection method state        
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case connection_start_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 63);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 63);
#endif
                    thread->place = 174; break;
                case connection_tune_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 64);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 64);
#endif
                    thread->place = 175; break;
                case connection_open_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 65);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 65);
#endif
                    thread->place = 176; break;
                case connection_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 15);
#endif
                    thread->place = 177; break;
                case connection_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 60);
#endif
                    thread->place = 178; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 179; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 180; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 181; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 182; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 183; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 184; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 185; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 12);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 186; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 187:                       //  have channel method state           
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case channel_open_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 67);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 67);
#endif
                    thread->place = 188; break;
                case channel_close_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 68);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 68);
#endif
                    thread->place = 189; break;
                case channel_close_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 69);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 69);
#endif
                    thread->place = 190; break;
                case channel_flow_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 70);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 70);
#endif
                    thread->place = 191; break;
                case channel_flow_ok_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 71);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 71);
#endif
                    thread->place = 192; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 193; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 194; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 195; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 196; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 197; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 198; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 199; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 13);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 200; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 201:                       //  read basic content state            
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case header_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 73);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 73);
#endif
                    thread->place = 202; break;
                case expect_body_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 74);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 74);
#endif
                    thread->place = 203; break;
                case method_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 58);
#endif
                    thread->place = 204; break;
                case socket_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 4);
#endif
                    thread->place = 205; break;
                case socket_timeout_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 14);
#endif
                    thread->place = 206; break;
                case socket_closing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 12);
#endif
                    thread->place = 207; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 208; break;
                case connection_abort_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 21);
#endif
                    thread->place = 209; break;
                case connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 19);
#endif
                    thread->place = 210; break;
                case async_connection_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 61);
#endif
                    thread->place = 211; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 14);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 212; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
        case 214:                       //  monitor heartbeat state             
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

#undef  tcb
}
            switch (thread->_next_event) {
                case SMT_NULL_EVENT:    //  In multi-thread mode, accidental   
                    break;              //  awakening is possible.             
                case peer_alive_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 76);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 76);
#endif
                    thread->place = 215; break;
                case peer_slowing_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 77);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 77);
#endif
                    thread->place = 216; break;
                case peer_stopped_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 78);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 78);
#endif
                    thread->place = 217; break;
                case smt_error_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 5);
#endif
                    thread->place = 218; break;
                case shutdown_event:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 15);
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + laststate + 7);
#endif
                    thread->place = 219; break;
                default:
                    report_unrecognised_event_error (thread);
                    return SMT_THREAD_ERROR;
            };
            if (thread->_next_event != SMT_NULL_EVENT) {
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                saveevent_id = thread->_next_event;
#endif
                thread->_next_event = SMT_NULL_EVENT;
                thread->module = 1;
            }
            break;
    }
                                  
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    if (thread->animate && thread->module == 1)
        animate_start_new_event (thread, saveevent_id);
#endif

    switch (thread->place) {
        case 3:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 1);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 2);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 2);
#endif
                    rc = action_allow_thread_to_settle (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 3);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 5);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 6);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 4);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 8);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
        case 69:
        case 71:
        case 72:
        case 52:
        case 54:
        case 74:
        case 122:
        case 124:
        case 106:
        case 108:
        case 38:
        case 158:
        case 145:
        case 186:
        case 200:
        case 212:
        case 172:
        case 135:
        case 219:
            switch (thread->module) {
                case 1:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    if (thread->place == 13
                    ||  thread->place == 6
                    ||  thread->place == 71
                    ||  thread->place == 54
                    ||  thread->place == 124
                    ||  thread->place == 108
                    ||  thread->place == 38
                    ||  thread->place == 158
                    ||  thread->place == 145
                    ||  thread->place == 186
                    ||  thread->place == 200
                    ||  thread->place == 212
                    ||  thread->place == 172
                    ||  thread->place == 135
                    ||  thread->place == 219)
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 9);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 9);
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

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 10);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 10);
#endif
                    rc = action_read_protocol_header (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 17:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 11);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 11);
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

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 18);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 18);
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

                case 3:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 19);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 19);
#endif
                    rc = action_send_connection_start (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 18:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 13);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 13);
#endif
                    rc = action_accept_direct_connection (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 14);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 14);
#endif
                    rc = action_read_direct_request (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 15);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 15);
#endif
                    rc = action_check_direct_lease (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 40:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 54);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 54);
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
        case 41:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 56);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 56);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 56);
#endif
                    rc = action_write_direct_heartbeat (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 44:
        case 19:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 43:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 55);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 55);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 55);
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
        case 45:
        case 62:
        case 64:
        case 113:
        case 115:
        case 97:
        case 99:
        case 28:
        case 30:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
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
        case 46:
        case 63:
        case 114:
        case 98:
        case 29:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
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
        case 47:
        case 65:
        case 116:
        case 100:
        case 31:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 47);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 47);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 47);
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
        case 57:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 48);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 48);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 49);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 49);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 58:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 93);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 93);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 93);
#endif
                    rc = action_unnamed_29 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 59:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 94);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 94);
#endif
                    rc = action_unnamed_30 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 60:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 95);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 95);
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

                case 3:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 61:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 66:
        case 67:
        case 50:
        case 51:
        case 119:
        case 120:
        case 103:
        case 104:
        case 27:
        case 34:
        case 35:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
        case 68:
        case 121:
        case 105:
        case 36:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 53);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 53);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 22);
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
        case 70:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 96);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 96);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 96);
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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 48:
        case 117:
        case 101:
        case 32:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 91);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 91);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 91);
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
        case 49:
        case 118:
        case 102:
        case 26:
        case 33:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 97);
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
        case 53:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 98);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 98);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 98);
#endif
                    rc = action_unnamed_36 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 20:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_write_desired_header (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 16);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 16);
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

                case 4:                         //  After actions              
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 12);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 12);
#endif
                    rc = action_write_desired_header (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 50);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 50);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 17);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 17);
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

                case 4:                         //  After actions              
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 46);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 46);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 23);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 23);
#endif
                    rc = action_report_setup_timeout (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 23:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 20);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 20);
#endif
                    rc = action_send_connection_tune (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 24:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 21);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 21);
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

                case 2:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 22);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 22);
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
        case 25:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 39);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 39);
#endif
                    rc = action_send_connection_open_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 75:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 24);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 24);
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
        case 76:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 25);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 25);
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
        case 77:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 26);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 26);
#endif
                    rc = action_send_exchange_declare_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 78:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 27);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 27);
#endif
                    rc = action_send_exchange_delete_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 79:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 28);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 28);
#endif
                    rc = action_send_queue_declare_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 80:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 29);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 29);
#endif
                    rc = action_send_queue_bind_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 81:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 30);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 30);
#endif
                    rc = action_send_queue_purge_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 82:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 31);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 31);
#endif
                    rc = action_send_queue_delete_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 83:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 32);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 32);
#endif
                    rc = action_send_basic_consume_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 33);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 33);
#endif
                    rc = action_send_basic_cancel_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 110:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 44);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 44);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 44);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 45);
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
        case 111:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 45);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 45);
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
        case 112:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 123:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 100);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 100);
#endif
                    rc = action_unnamed_58 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 85:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 34);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 34);
#endif
                    rc = action_send_basic_deliver (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 99);
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
        case 86:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 35);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 35);
#endif
                    rc = action_send_basic_get_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 99);
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
        case 87:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 36);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 36);
#endif
                    rc = action_send_basic_get_empty (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 88:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 37);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 37);
#endif
                    rc = action_send_direct_put_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 89:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 38);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 38);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 38);
#endif
                    rc = action_send_direct_get_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 90:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 42);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 42);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 42);
#endif
                    rc = action_push_method_to_client (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 99);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 99);
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
        case 91:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 43);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 43);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 43);
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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 92:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 40);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 40);
#endif
                    rc = action_send_channel_open_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 93:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 41);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 41);
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
        case 94:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 101);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 101);
#endif
                    rc = action_unnamed_67 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 95:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 102);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 102);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 102);
#endif
                    rc = action_unnamed_68 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 97);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 97);
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
        case 96:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 103);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 103);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 103);
#endif
                    rc = action_unnamed_69 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 107:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 104);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 104);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 104);
#endif
                    rc = action_unnamed_70 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 37:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 105);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 105);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 105);
#endif
                    rc = action_unnamed_71 (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
        case 137:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 58);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 59);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 60);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 138:
        case 140:
        case 152:
        case 154:
        case 179:
        case 181:
        case 193:
        case 195:
        case 205:
        case 207:
        case 165:
        case 167:
        case 128:
        case 130:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 82);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 51);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 51);
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
        case 139:
        case 153:
        case 180:
        case 194:
        case 206:
        case 166:
        case 129:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 82);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 52);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 52);
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
        case 141:
        case 155:
        case 182:
        case 196:
        case 208:
        case 168:
        case 131:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 82);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 7);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
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

                case 3:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 142:
        case 151:
        case 156:
        case 183:
        case 197:
        case 209:
        case 169:
        case 132:
        case 218:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 82);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 82);
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
        case 147:
        case 149:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 58);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 59);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 84);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 84);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 84);
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
        case 148:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 60);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 85);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 85);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 85);
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
        case 150:
        case 178:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 72);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 72);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 72);
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
        case 157:
        case 144:
        case 185:
        case 199:
        case 211:
        case 171:
        case 134:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 107);
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

                case 2:                         //  After actions              
                    thread->module = SMT_TERM_MODULE;
                    thread->_next_event = SMT_TERM_EVENT;
                    break;
                default:
                    report_state_machine_error (thread);
                    return SMT_THREAD_ERROR;
        }
        break;
        case 143:
        case 184:
        case 198:
        case 210:
        case 170:
        case 133:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 83);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 83);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 107);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 107);
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
        case 127:
        case 164:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 160:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 62);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 62);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 62);
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
        case 174:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 65);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 65);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 65);
#endif
                    rc = action_execute_connection_start_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 66);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 66);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 66);
#endif
                    rc = action_notify_connection_of_start_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 175:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 67);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 67);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 67);
#endif
                    rc = action_execute_connection_tune_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 68);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 68);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 68);
#endif
                    rc = action_notify_connection_of_tune_ok (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 176:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 69);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 69);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 69);
#endif
                    rc = action_execute_connection_open (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 70);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 70);
#endif
                    rc = action_notify_connection_of_open (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 177:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 71);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 71);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 71);
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
        case 161:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 63);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 63);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 63);
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
        case 188:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 73);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 73);
#endif
                    rc = action_execute_channel_open (thread);

#if (defined (SMT_ACTION_LIMIT))
                    if (icl_atomic_dec32 (&smt_actions_remaining) == 0) {
                        icl_console_print ("BREAKING!!!");
                        abort ();
                    }
#endif
                    
                    if (rc == SMT_BREAK) {
                        rc = SMT_OK;
                        break;
                    }
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 189:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 74);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 74);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 74);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 190:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 75);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 75);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 75);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 191:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 76);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 76);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 76);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 192:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 77);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 77);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 77);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 202:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 58);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 59);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 78);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 78);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 78);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 79);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 203:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 57);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 57);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 58);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 58);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 59);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 59);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 80);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 80);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 79);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 79);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 92);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 92);
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
        case 204:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 60);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 60);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 81);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 81);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 81);
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
        case 162:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 108);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 108);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 108);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 64);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 163:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 64);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 64);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 106);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 106);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 61);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 61);
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
        case 215:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 86);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 87);
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
        case 216:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 88);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 88);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 86);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 86);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 87);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 87);
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
        case 217:
            switch (thread->module) {
                case 1:
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 89);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 89);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 89);
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
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
                    icl_trace_record (
                        NULL, s_dump, thread->id * 0x10000 + laststate + lastevent + 90);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
                    icl_trace_record (
                        thread->trace, s_dump, thread->id * 0x10000 + laststate + lastevent + 90);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
                    if (thread->animate)
                        animate_action (thread, 90);
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
demo_server_agent_catcher (smt_thread_t *thread)
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
demo_server_agent_master_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_server_agent",
                 master_thread_name);

#endif
{
#define tcb ((demo_server_agent_master_context_t *) thread->context)

        smt_socket_destroy (&tcb->client_socket);
        smt_socket_destroy (&tcb->master_socket);
    
#undef  tcb
}
    if (--((demo_server_agent_master_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_server_agent_connection_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_server_agent",
                 connection_thread_name);

#endif
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        tcb->connection_thread = NULL;

        //  Destroy demo_server_connection object
        demo_server_connection_destroy (&tcb->connection);

        //  Free resources
        demo_lease_unlink (&tcb->lease);
        demo_server_method_unlink (&tcb->method_in);
        demo_server_method_unlink (&tcb->method_out);
        demo_server_channel_destroy (&tcb->channel);
        ipr_bucket_unlink (&tcb->bucket_in);
        smt_socket_destroy (&tcb->socket);
    
#undef  tcb
}
    if (--((demo_server_agent_connection_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_server_agent_input_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_server_agent",
                 input_thread_name);

#endif
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        tcb->input_thread = NULL;
    
#undef  tcb
}
    if (--((demo_server_agent_connection_context_t *) thread->context)->links == 0)
        icl_mem_free (thread->context);
    return 0;
}


static int    
demo_server_agent_heartbeat_destroy (smt_thread_t *thread)
{
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (NULL, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (DEMO_SERVER_AGENT_TRACE))
    //  Don't trust thread->os_thread while destroying thread.
    icl_trace_record (thread->trace, s_dump, thread->id * 0x10000 + 0);
#endif
#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))
    if (thread->animate)
        icl_console_print ("%4lu> %-15s/%-15s <Destroyed>",
                  thread->id,
                  "demo_server_agent",
                 heartbeat_thread_name);

#endif
{
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

        tcb->heartbeat_thread = NULL;

    
#undef  tcb
}
    if (--((demo_server_agent_connection_context_t *) thread->context)->links == 0)
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
    if (thread->place < 125)
      {
        *thread_type = 2;
        if (thread->place < 39)
          {
            *state_id = 3;
            switch (thread->place) {
                case 16: *event_id = 1;   *nextstate_id = 3;   break;
                case 17: *event_id = 8;   *nextstate_id = 3;   break;
                case 18: *event_id = 9;   *nextstate_id = 4;   break;
                case 19: *event_id = 25;   break;
                case 20: *event_id = 26;   break;
                case 21: *event_id = 27;   break;
                case 22: *event_id = 23;   break;
                case 23: *event_id = 28;   *nextstate_id = 3;   break;
                case 24: *event_id = 29;   *nextstate_id = 3;   break;
                case 25: *event_id = 30;   *nextstate_id = 6;   break;
                case 26: *event_id = 54;   break;
                case 27: *event_id = 6;   break;
                case 28: *event_id = 4;   break;
                case 29: *event_id = 14;   break;
                case 30: *event_id = 12;   break;
                case 31: *event_id = 5;   break;
                case 32: *event_id = 15;   break;
                case 33: *event_id = 19;   break;
                case 34: *event_id = 21;   break;
                case 35: *event_id = 22;   break;
                case 36: *event_id = 11;   *nextstate_id = 3;   break;
                case 37: *event_id = 24;   *nextstate_id = 3;   break;
                case 38: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 55)
          {
            *state_id = 4;
            switch (thread->place) {
                case 40: *event_id = 10;   *nextstate_id = 4;   break;
                case 41: *event_id = 11;   *nextstate_id = 4;   break;
                case 42: *event_id = 12;   break;
                case 43: *event_id = 13;   *nextstate_id = 4;   break;
                case 44: *event_id = 6;   break;
                case 45: *event_id = 4;   break;
                case 46: *event_id = 14;   break;
                case 47: *event_id = 5;   break;
                case 48: *event_id = 15;   break;
                case 49: *event_id = 19;   break;
                case 50: *event_id = 21;   break;
                case 51: *event_id = 22;   break;
                case 52: *event_id = 23;   *nextstate_id = 4;   break;
                case 53: *event_id = 24;   *nextstate_id = 4;   break;
                case 54: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 73)
          {
            *state_id = 5;
            switch (thread->place) {
                case 56: *event_id = 16;   *nextstate_id = 5;   break;
                case 57: *event_id = 17;   *nextstate_id = 5;   break;
                case 58: *event_id = 18;   *nextstate_id = 5;   break;
                case 59: *event_id = 19;   *nextstate_id = 5;   break;
                case 60: *event_id = 15;   *nextstate_id = 5;   break;
                case 61: *event_id = 20;   *nextstate_id = 5;   break;
                case 62: *event_id = 4;   break;
                case 63: *event_id = 14;   break;
                case 64: *event_id = 12;   break;
                case 65: *event_id = 5;   break;
                case 66: *event_id = 21;   break;
                case 67: *event_id = 22;   break;
                case 68: *event_id = 11;   *nextstate_id = 5;   break;
                case 69: *event_id = 23;   *nextstate_id = 5;   break;
                case 70: *event_id = 24;   *nextstate_id = 5;   break;
                case 71: *event_id = 7;   break;
                default:
                                     *nextstate_id = 5;   break;
            }
          }
        else
        if (thread->place < 109)
          {
            *state_id = 6;
            switch (thread->place) {
                case 74: *event_id = 31;   *nextstate_id = 6;   break;
                case 75: *event_id = 32;   *nextstate_id = 6;   break;
                case 76: *event_id = 33;   *nextstate_id = 6;   break;
                case 77: *event_id = 34;   *nextstate_id = 6;   break;
                case 78: *event_id = 35;   *nextstate_id = 6;   break;
                case 79: *event_id = 36;   *nextstate_id = 6;   break;
                case 80: *event_id = 37;   *nextstate_id = 6;   break;
                case 81: *event_id = 38;   *nextstate_id = 6;   break;
                case 82: *event_id = 39;   *nextstate_id = 6;   break;
                case 83: *event_id = 40;   *nextstate_id = 6;   break;
                case 84: *event_id = 41;   *nextstate_id = 6;   break;
                case 85: *event_id = 42;   *nextstate_id = 6;   break;
                case 86: *event_id = 46;   *nextstate_id = 6;   break;
                case 87: *event_id = 47;   *nextstate_id = 6;   break;
                case 88: *event_id = 48;   *nextstate_id = 6;   break;
                case 89: *event_id = 49;   *nextstate_id = 6;   break;
                case 90: *event_id = 50;   *nextstate_id = 6;   break;
                case 91: *event_id = 51;   *nextstate_id = 6;   break;
                case 92: *event_id = 52;   *nextstate_id = 6;   break;
                case 93: *event_id = 53;   *nextstate_id = 6;   break;
                case 94: *event_id = 54;   *nextstate_id = 6;   break;
                case 95: *event_id = 6;   break;
                case 96: *event_id = 55;   *nextstate_id = 6;   break;
                case 97: *event_id = 4;   break;
                case 98: *event_id = 14;   break;
                case 99: *event_id = 12;   break;
                case 100: *event_id = 5;   break;
                case 101: *event_id = 15;   break;
                case 102: *event_id = 19;   break;
                case 103: *event_id = 21;   break;
                case 104: *event_id = 22;   break;
                case 105: *event_id = 11;   *nextstate_id = 6;   break;
                case 106: *event_id = 23;   *nextstate_id = 6;   break;
                case 107: *event_id = 24;   *nextstate_id = 6;   break;
                case 108: *event_id = 7;   break;
            }
          }
        else
          {
            *state_id = 7;
            switch (thread->place) {
                case 110: *event_id = 43;   *nextstate_id = 7;   break;
                case 111: *event_id = 44;   *nextstate_id = 7;   break;
                case 112: *event_id = 45;   *nextstate_id = 7;   break;
                case 113: *event_id = 4;   break;
                case 114: *event_id = 14;   break;
                case 115: *event_id = 12;   break;
                case 116: *event_id = 5;   break;
                case 117: *event_id = 15;   break;
                case 118: *event_id = 19;   break;
                case 119: *event_id = 21;   break;
                case 120: *event_id = 22;   break;
                case 121: *event_id = 11;   *nextstate_id = 7;   break;
                case 122: *event_id = 23;   *nextstate_id = 7;   break;
                case 123: *event_id = 24;   *nextstate_id = 7;   break;
                case 124: *event_id = 7;   break;
            }
          }
      }
    else
    if (thread->place < 213)
      {
        *thread_type = 3;
        if (thread->place < 136)
          {
            *state_id = 8;
            switch (thread->place) {
                case 127: *event_id = 1;   *nextstate_id = 11;   break;
                case 128: *event_id = 4;   break;
                case 129: *event_id = 14;   break;
                case 130: *event_id = 12;   break;
                case 131: *event_id = 5;   break;
                case 132: *event_id = 21;   break;
                case 133: *event_id = 19;   break;
                case 134: *event_id = 61;   break;
                case 135: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 146)
          {
            *state_id = 9;
            switch (thread->place) {
                case 137: *event_id = 56;   *nextstate_id = 9;   break;
                case 138: *event_id = 4;   break;
                case 139: *event_id = 14;   break;
                case 140: *event_id = 12;   break;
                case 141: *event_id = 5;   break;
                case 142: *event_id = 21;   break;
                case 143: *event_id = 19;   break;
                case 144: *event_id = 61;   break;
                case 145: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 159)
          {
            *state_id = 10;
            switch (thread->place) {
                case 147: *event_id = 57;   *nextstate_id = 10;   break;
                case 148: *event_id = 58;   *nextstate_id = 10;   break;
                case 149: *event_id = 59;   *nextstate_id = 10;   break;
                case 150: *event_id = 60;   break;
                case 151: *event_id = 19;   break;
                case 152: *event_id = 4;   break;
                case 153: *event_id = 14;   break;
                case 154: *event_id = 12;   break;
                case 155: *event_id = 5;   break;
                case 156: *event_id = 21;   break;
                case 157: *event_id = 61;   break;
                case 158: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 173)
          {
            *state_id = 11;
            switch (thread->place) {
                case 160: *event_id = 62;   *nextstate_id = 12;   break;
                case 161: *event_id = 66;   *nextstate_id = 13;   break;
                case 162: *event_id = 72;   *nextstate_id = 11;   break;
                case 163: *event_id = 75;   *nextstate_id = 11;   break;
                case 164: *event_id = 31;   *nextstate_id = 11;   break;
                case 165: *event_id = 4;   break;
                case 166: *event_id = 14;   break;
                case 167: *event_id = 12;   break;
                case 168: *event_id = 5;   break;
                case 169: *event_id = 21;   break;
                case 170: *event_id = 19;   break;
                case 171: *event_id = 61;   break;
                case 172: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 187)
          {
            *state_id = 12;
            switch (thread->place) {
                case 174: *event_id = 63;   *nextstate_id = 11;   break;
                case 175: *event_id = 64;   *nextstate_id = 11;   break;
                case 176: *event_id = 65;   *nextstate_id = 11;   break;
                case 177: *event_id = 15;   break;
                case 178: *event_id = 60;   break;
                case 179: *event_id = 4;   break;
                case 180: *event_id = 14;   break;
                case 181: *event_id = 12;   break;
                case 182: *event_id = 5;   break;
                case 183: *event_id = 21;   break;
                case 184: *event_id = 19;   break;
                case 185: *event_id = 61;   break;
                case 186: *event_id = 7;   break;
            }
          }
        else
        if (thread->place < 201)
          {
            *state_id = 13;
            switch (thread->place) {
                case 188: *event_id = 67;   *nextstate_id = 11;   break;
                case 189: *event_id = 68;   *nextstate_id = 11;   break;
                case 190: *event_id = 69;   *nextstate_id = 11;   break;
                case 191: *event_id = 70;   *nextstate_id = 11;   break;
                case 192: *event_id = 71;   *nextstate_id = 11;   break;
                case 193: *event_id = 4;   break;
                case 194: *event_id = 14;   break;
                case 195: *event_id = 12;   break;
                case 196: *event_id = 5;   break;
                case 197: *event_id = 21;   break;
                case 198: *event_id = 19;   break;
                case 199: *event_id = 61;   break;
                case 200: *event_id = 7;   break;
            }
          }
        else
          {
            *state_id = 14;
            switch (thread->place) {
                case 202: *event_id = 73;   *nextstate_id = 14;   break;
                case 203: *event_id = 74;   *nextstate_id = 14;   break;
                case 204: *event_id = 58;   *nextstate_id = 11;   break;
                case 205: *event_id = 4;   break;
                case 206: *event_id = 14;   break;
                case 207: *event_id = 12;   break;
                case 208: *event_id = 5;   break;
                case 209: *event_id = 21;   break;
                case 210: *event_id = 19;   break;
                case 211: *event_id = 61;   break;
                case 212: *event_id = 7;   break;
            }
          }
      }
    else
      {
        *thread_type = 4;
          {
            *state_id = 15;
            switch (thread->place) {
                case 215: *event_id = 76;   *nextstate_id = 15;   break;
                case 216: *event_id = 77;   *nextstate_id = 15;   break;
                case 217: *event_id = 78;   break;
                case 218: *event_id = 5;   break;
                case 219: *event_id = 7;   break;
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
    icl_console_print ("demo_server_agent: Unrecognised event '%s' in '%s' thread number: %u, state: %s",
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

#if (defined (BASE_ANIMATE) || defined (BASE_ANIMATE_DEMO_SERVER_AGENT))

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
            "demo_server_agent",
            thread_name [thread_type],
            state_name [state_id]);
  icl_console_print ("%4lu> %-15s/%-15s    (--) %-32s->%s",
            thread->id,
            "demo_server_agent",
            thread_name [thread_type],
            event_name [event_id != SMT_NULL_EVENT ? event_id : saveevent_id],
            nextstate_id ? state_name [nextstate_id] : "");
}

static void
animate_action (smt_thread_t *thread, int action_id)
{
    icl_console_print ("%4lu> %-15s/%-15s           + %s",
              thread->id,
              "demo_server_agent",
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
    if (place < 125)
        thread_type = 2;
    else
    if (place < 213)
        thread_type = 3;
    else
        thread_type = 4;
    return thread_type;
}

#endif
#if (defined (BASE_TRACE) || defined (BASE_TRACE_DEMO_SERVER_AGENT)            ||  defined (DEMO_SERVER_AGENT_TRACE))
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
                                       "demo_server_agent",
                                       action_name [action_id]);
    }
    else
    if (state_or_event_or_action > laststate) {
        event_id = state_or_event_or_action - laststate;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s    (--) %-32s",
                                       thread_id,
                                       "demo_server_agent",
                                       event_name [event_id]);
    }
    else {
        state_id = state_or_event_or_action;
        icl_console_print_thread_time (thread, time,
                                       "%4lu> %-15s %s:",
                                       thread_id,
                                       "demo_server_agent",
                                       state_id ? state_name [state_id] : "<Destroyed>");
    }
}

#endif
static int
action_initialise_server (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_INITIALISE_SERVER))
    icl_stats_inc ("demo_server_agent_initialise_server", &s_demo_server_agent_initialise_server_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
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

        port  = demo_server_config_port  (demo_server_config);
        user  = demo_server_config_user  (demo_server_config);
        group = demo_server_config_group (demo_server_config);
        if (atoi (port) < 1024
        ||  atoi (port) > 65535) {
            smt_log_print (demo_broker->alert_log,
                "E: port %s is not valid for this service", port);
            exit (1);                   //  FIXME: see BASE2-289
        }
        else {
            apr_pool_create (&pool, icl_global_pool);
            apr_pool_tag (pool, "demo_server_initialise_server)");

            //  Report hostname and all visible network interfaces
            if (apr_gethostname (hostname, ICL_SHORTSTR_MAX, pool) == APR_SUCCESS) {
                apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
                if (sockaddr) {
                    apr_sockaddr_ip_get (&address, sockaddr);
                    smt_log_print (demo_broker->alert_log,
                        "I: hostname is %s (%s)", hostname, address);
                }
                else
                    smt_log_print (demo_broker->alert_log,
                        "E: cannot translate host name '%s'", hostname);
            }
            //  Report which interfaces server is going to listen to
            icl_shortstr_cpy (hostname, demo_server_config_listen (demo_server_config));
            if (streq (hostname, "*")) {
                smt_log_print (demo_broker->alert_log,
                    "I: listening on port %s, all network interfaces", port);
                icl_shortstr_cpy (hostname, APR_ANYADDR);
            }
            else
                smt_log_print (demo_broker->alert_log,
                    "I: listening on port %s, interface %s", port, hostname);

            apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
            if (sockaddr) {
                apr_sockaddr_ip_get (&address, sockaddr);
                icl_shortstr_fmt (demo_broker->host, "%s:%s", address, port);
            }
            else
                smt_log_print (demo_broker->alert_log,
                    "E: cannot translate host name '%s'", hostname);

            apr_pool_destroy (pool);
            tcb->master_socket = smt_socket_new ();
            if (tcb->master_socket) {
                //  Set Nagle's algorithm on or off
                rc = smt_socket_set_nodelay (tcb->master_socket,
                    demo_server_config_tcp_nodelay (demo_server_config));
                if (rc)
                    smt_log_print (demo_broker->alert_log,
                        "W: could not set TCP/IP NODELAY option: "
                        "Socket Error");

                //  Report and set socket buffers if wanted
                new_size = demo_server_config_tcp_rcvbuf (demo_server_config);
                if (new_size) {
                    rc = smt_socket_set_rcvbuf (tcb->master_socket, new_size, &old_size);
                    if (!rcv_warned) {
                        rcv_warned = TRUE;
                        if (rc)
                            smt_log_print (demo_broker->alert_log,
                                "W: could not set TCP/IP receive buffer size: "
                                "Socket Error");
                        else
                            smt_log_print (demo_broker->alert_log,
                                "I: TCP/IP receive buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                new_size = demo_server_config_tcp_sndbuf (demo_server_config);
                if (new_size) {
                    rc = smt_socket_set_sndbuf (tcb->master_socket, new_size, &old_size);
                    if (!snd_warned) {
                        snd_warned = TRUE;
                        if (rc)
                            smt_log_print (demo_broker->alert_log,
                                "W: could not set TCP/IP send buffer size: "
                                "Socket Error");
                        else
                            smt_log_print (demo_broker->alert_log,
                                "I: TCP/IP sending buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                //  Start listening for connections on socket
                rc = smt_socket_passive (tcb->master_socket, thread, port, hostname, 500);

                //  Try to drop to safe user/group, complain if not possible
                if (*user && *group) {
                    if (ipr_uid_run_as (user, group)) {
                        smt_log_print (demo_broker->alert_log,
                            "E: could not run as user=%s group=%s", user, group);
                        exit (1);
                    }
                    else
                        smt_log_print (demo_broker->alert_log,
                            "I: server is now running as user=%s group=%s", user, group);
                }
            }
            if (!tcb->master_socket || (rc != SMT_OK)) {
                smt_log_print (demo_broker->alert_log,
                    "E: could not open port %s: %s", port,
                    smt_thread_error (thread));
                exit (1);               //  FIXME: see BASE2-289
            }
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_allow_thread_to_settle (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ALLOW_THREAD_TO_SETTLE))
    icl_stats_inc ("demo_server_agent_allow_thread_to_settle", &s_demo_server_agent_allow_thread_to_settle_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SERVER_READY))
    icl_stats_inc ("demo_server_agent_report_server_ready", &s_demo_server_agent_report_server_ready_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "I: server ready for incoming DEMO connections");
        if (demo_server_config_direct (demo_server_config))
            smt_log_print (demo_broker->alert_log,
                "I: Direct Mode enabled for all client applications by default");
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ACCEPT_CONNECTION))
    icl_stats_inc ("demo_server_agent_accept_connection", &s_demo_server_agent_accept_connection_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CREATE_CONNECTION_THREAD))
    icl_stats_inc ("demo_server_agent_create_connection_thread", &s_demo_server_agent_create_connection_thread_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
{

        smt_thread_t
            *connection_thread;
        demo_server_agent_connection_context_t
            *connection_tcb;

        connection_thread = demo_server_agent_connection_thread_new ();
        connection_thread->priority = SMT_PRIORITY_HIGH;

        connection_tcb                    = connection_thread->context;
        connection_tcb->socket            = tcb->client_socket;
        connection_tcb->connection_thread = connection_thread;
        connection_tcb->input_thread      = NULL;
        connection_tcb->heartbeat_thread  = NULL;
        tcb->client_socket                = NULL;

        smt_thread_wake   ( connection_thread);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_ACCEPT_ERROR))
    icl_stats_inc ("demo_server_agent_report_accept_error", &s_demo_server_agent_report_accept_error_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
{

        int
            accept_retry_timeout;

        smt_log_print (demo_broker->alert_log,
            "E: could not accept connection: %s",
            smt_thread_error (thread));

        //  If we ran out of files wait the configured amount of time and retry
        if (thread->result == SMT_SOCKET_ERROR &&
           (APR_STATUS_IS_ENFILE(thread->error) ||
            APR_STATUS_IS_EMFILE(thread->error))) {
            accept_retry_timeout =
                demo_server_config_accept_retry_timeout (demo_server_config);
            smt_log_print (demo_broker->alert_log,
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SMT_ERROR))
    icl_stats_inc ("demo_server_agent_report_smt_error", &s_demo_server_agent_report_smt_error_stats);
#endif        
{

        smt_log_print (demo_broker->alert_log,
            "E: SMT error: %s", smt_thread_error (thread));
    
}
    return _rc;
}

static int
action_close_master_socket (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CLOSE_MASTER_SOCKET))
    icl_stats_inc ("demo_server_agent_close_master_socket", &s_demo_server_agent_close_master_socket_stats);
#endif        
#define tcb ((demo_server_agent_master_context_t *) thread->context)
{

        if (tcb->master_socket)
            smt_socket_request_close (thread, &tcb->master_socket, 0, SMT_NULL_EVENT);
    
}
#undef  tcb
    return _rc;
}

static int
action_start_setup_timer (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_SETUP_TIMER))
    icl_stats_inc ("demo_server_agent_start_setup_timer", &s_demo_server_agent_start_setup_timer_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Schedule a timeout event to protect against misbehaving clients
        tcb->setup_timer = smt_timer_request_delay (thread,
            demo_server_config_setup_timeout (demo_server_config) * 1000 * 1000,
            setup_timeout_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_protocol_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_PROTOCOL_HEADER))
    icl_stats_inc ("demo_server_agent_read_protocol_header", &s_demo_server_agent_read_protocol_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        assert (strlen ("DEMO") == 4);
        smt_socket_request_read (thread, tcb->socket, 0, 8, 8,
            tcb->protocol_header, have_protocol_header_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_check_protocol_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_PROTOCOL_HEADER))
    icl_stats_inc ("demo_server_agent_check_protocol_header", &s_demo_server_agent_check_protocol_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check the first 4 bytes are "DEMO"
        if (memcmp (tcb->protocol_header, "DEMO", 4) != 0)
            smt_thread_raise_exception (thread, bad_protocol_event);
        else
        //  Next we check the protocol number & version
        if (memcmp (tcb->protocol_header + 4, DP_HEADER, 4) == 0)
            smt_thread_raise_exception (thread, direct_protocol_event);
        else
        if (memcmp (tcb->protocol_header + 4, "\x00\x00\x09\x01", 4)
        &&  memcmp (tcb->protocol_header + 4, "\x01\x01\x00\x09", 4))
            smt_thread_raise_exception (thread, version_mismatch_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_write_desired_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DESIRED_HEADER))
    icl_stats_inc ("demo_server_agent_write_desired_header", &s_demo_server_agent_write_desired_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Write our own protocol header to the socket
        s_socket_write (thread, (byte *) "DEMO\x00\x00\x09\x01", 8);
    
}
#undef  tcb
    return _rc;
}

static int
action_accept_direct_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_ACCEPT_DIRECT_CONNECTION))
    icl_stats_inc ("demo_server_agent_accept_direct_connection", &s_demo_server_agent_accept_direct_connection_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        tcb->batching = demo_server_config_batching (demo_server_config);
        if (tcb->batching < 2048)
            tcb->batching = 0;
        s_socket_write_shortstr (thread, "200 OK DP/0.1");
        s_socket_read (thread, &tcb->rlength, 1);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_direct_request (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_DIRECT_REQUEST))
    icl_stats_inc ("demo_server_agent_read_direct_request", &s_demo_server_agent_read_direct_request_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        s_socket_read (thread, (byte *) tcb->request, (int) (tcb->rlength));
    
}
#undef  tcb
    return _rc;
}

static int
action_check_direct_lease (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_DIRECT_LEASE))
    icl_stats_inc ("demo_server_agent_check_direct_lease", &s_demo_server_agent_check_direct_lease_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        icl_shortstr_t
            response;

        tcb->request [tcb->rlength] = 0;
        tcb->lease = demo_lease_search (tcb->request);
        if (tcb->lease) {
            tcb->type = tcb->lease->type;
            if (demo_lease_acquire (tcb->lease, thread, tcb->socket)) {
                s_socket_write_shortstr (thread, "402 BAD-LEASE Lease already acquired");
                demo_lease_unlink (&tcb->lease);
                smt_thread_raise_exception (thread, bad_lease_event);
            }
            else
            if (tcb->type == DP_SINK) {
                //  Confirm OK to client
                icl_shortstr_fmt (response, "200 OK Sink '%s' ready", tcb->lease->sink->name);
                s_socket_write_shortstr (thread, response);

                //  Slurp a bucket of data from the network
                //  Min. size for a packed 6-DMP content is 12 bytes
                //  We always read as much as we can, limited by ipr_bucket
                tcb->bucket_in = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
                smt_socket_request_read (thread, tcb->socket, 0,
                    12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                    direct_in_event);
            }
            else
            if (tcb->type == DP_FEED) {
                //  Confirm OK to client
                icl_shortstr_fmt (response, "200 OK Feed '%s' ready", tcb->lease->feed->name);
                s_socket_write_shortstr (thread, response);
                //  Start feed heartbeat
                smt_timer_request_delay (thread, tcb->direct_heartbeat * 1000 * 1000, heartbeat_event);
            }
        }
        else {
            s_socket_write_shortstr (thread, "402 BAD-LEASE Lease is not valid for this connection");
            smt_thread_raise_exception (thread, bad_lease_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_report_bad_protocol (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_BAD_PROTOCOL))
    icl_stats_inc ("demo_server_agent_report_bad_protocol", &s_demo_server_agent_report_bad_protocol_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "E: client at '%s' sent invalid protocol header",
            tcb->connection->client_address);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_version_mismatch (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_VERSION_MISMATCH))
    icl_stats_inc ("demo_server_agent_report_version_mismatch", &s_demo_server_agent_report_version_mismatch_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "E: version mismatch: client at '%s' sent (%u, %u, %u, %u)",
            tcb->connection->client_address,
            tcb->protocol_header [4],
            tcb->protocol_header [5],
            tcb->protocol_header [6],
            tcb->protocol_header [7]);
    
}
#undef  tcb
    return _rc;
}

static int
action_start_input_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_INPUT_THREAD))
    icl_stats_inc ("demo_server_agent_start_input_thread", &s_demo_server_agent_start_input_thread_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_thread_t
            *input_thread;

        //  Start input thread
        input_thread = demo_server_agent_input_thread_new (thread);

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
action_send_connection_start (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_START))
    icl_stats_inc ("demo_server_agent_send_connection_start", &s_demo_server_agent_send_connection_start_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        icl_longstr_t
            *properties,                //  Connection properties
            *mechanisms,                //  Supported authentication mechanisms
            *locales;                   //  Supported locales
        asl_field_list_t
            *field_list;                //  List for connection properties
        icl_shortstr_t
            system;                     //  System (platform) name

        field_list = asl_field_list_new (NULL);
        asl_field_new_string (field_list, "host", demo_broker->host);
        asl_field_new_string (field_list, "product", "ASL Demo Server");
        asl_field_new_string (field_list, "version", VERSION);
        asl_field_new_string (field_list, "copyright", COPYRIGHT);
        asl_field_new_string (field_list, "platform", icl_system_name (system));
        asl_field_new_string (field_list, "information", BUILDMODEL);
        asl_field_new_string (field_list, "connection", tcb->connection->id);
        asl_field_new_string (field_list, "identifier", demo_broker->identifier);
        asl_field_new_integer (field_list, "direct", demo_server_config_direct (demo_server_config));

        properties = asl_field_list_flatten (field_list);
        asl_field_list_destroy (&field_list);

        mechanisms = icl_longstr_new ("PLAIN", 5);
        locales    = icl_longstr_new ("en_US", 5);

        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_start (
            0, 9, properties, mechanisms,
            locales);
        s_send_method_out (thread, 0, demo_broker->debug_log);

        icl_longstr_destroy (&properties);
        icl_longstr_destroy (&mechanisms);
        icl_longstr_destroy (&locales);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_tune (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_TUNE))
    icl_stats_inc ("demo_server_agent_send_connection_tune", &s_demo_server_agent_send_connection_tune_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_tune (
            tcb->connection->channel_max,
            tcb->connection->frame_max,
            tcb->connection->heartbeat);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    
}
#undef  tcb
    return _rc;
}

static int
action_start_heartbeat_thread (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_HEARTBEAT_THREAD))
    icl_stats_inc ("demo_server_agent_start_heartbeat_thread", &s_demo_server_agent_start_heartbeat_thread_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_thread_t
            *heartbeat_thread;

        //  If client is heartbeat enabled, start heartbeat thread
        if (tcb->heartbeat) {
            heartbeat_thread = demo_server_agent_heartbeat_thread_new (thread);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_START_HEARTBEAT_TIMER))
    icl_stats_inc ("demo_server_agent_start_heartbeat_timer", &s_demo_server_agent_start_heartbeat_timer_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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
action_report_setup_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SETUP_TIMEOUT))
    icl_stats_inc ("demo_server_agent_report_setup_timeout", &s_demo_server_agent_report_setup_timeout_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "E: client timed out during connection setup, closing connection "  "(%s)", tcb->connection->client_address);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_flow (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_FLOW))
    icl_stats_inc ("demo_server_agent_send_channel_flow", &s_demo_server_agent_send_channel_flow_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (channel_flow_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_channel_flow (
                    channel_flow_m->active);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_FLOW_OK))
    icl_stats_inc ("demo_server_agent_send_channel_flow_ok", &s_demo_server_agent_send_channel_flow_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (channel_flow_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_channel_flow_ok (
                    channel_flow_ok_m->active);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_exchange_declare_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_EXCHANGE_DECLARE_OK))
    icl_stats_inc ("demo_server_agent_send_exchange_declare_ok", &s_demo_server_agent_send_exchange_declare_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (exchange_declare_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_exchange_declare_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_exchange_delete_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_EXCHANGE_DELETE_OK))
    icl_stats_inc ("demo_server_agent_send_exchange_delete_ok", &s_demo_server_agent_send_exchange_delete_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (exchange_delete_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_exchange_delete_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_declare_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_DECLARE_OK))
    icl_stats_inc ("demo_server_agent_send_queue_declare_ok", &s_demo_server_agent_send_queue_declare_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_declare_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_declare_ok (
                    queue_declare_ok_m->queue,
                    queue_declare_ok_m->properties);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_bind_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_BIND_OK))
    icl_stats_inc ("demo_server_agent_send_queue_bind_ok", &s_demo_server_agent_send_queue_bind_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_bind_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_bind_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_purge_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_PURGE_OK))
    icl_stats_inc ("demo_server_agent_send_queue_purge_ok", &s_demo_server_agent_send_queue_purge_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_purge_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_purge_ok (
                    queue_purge_ok_m->message_count);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_queue_delete_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_QUEUE_DELETE_OK))
    icl_stats_inc ("demo_server_agent_send_queue_delete_ok", &s_demo_server_agent_send_queue_delete_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (queue_delete_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_delete_ok (
                    queue_delete_ok_m->message_count);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_consume_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_CONSUME_OK))
    icl_stats_inc ("demo_server_agent_send_basic_consume_ok", &s_demo_server_agent_send_basic_consume_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_consume_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_consume_ok (
                    basic_consume_ok_m->consumer_tag);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_cancel_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_CANCEL_OK))
    icl_stats_inc ("demo_server_agent_send_basic_cancel_ok", &s_demo_server_agent_send_basic_cancel_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_cancel_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_cancel_ok (
                    basic_cancel_ok_m->consumer_tag);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_deliver (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_DELIVER))
    icl_stats_inc ("demo_server_agent_send_basic_deliver", &s_demo_server_agent_send_basic_deliver_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_deliver_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_deliver (
                    basic_deliver_m->exchange,
                    basic_deliver_m->routing_key,
                    basic_deliver_m->delivery_tag);
            tcb->method_out->content =
                demo_content_basic_link (
                    basic_deliver_m->content
                );
            assert (tcb->method_out->content);
            tcb->connection->contents_out++;
            ipr_meter_count (demo_broker->ometer);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_get_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_GET_OK))
    icl_stats_inc ("demo_server_agent_send_basic_get_ok", &s_demo_server_agent_send_basic_get_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_get_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_get_ok (
                    basic_get_ok_m->exchange,
                    basic_get_ok_m->routing_key,
                    basic_get_ok_m->message_count);
            tcb->method_out->content =
                demo_content_basic_link (
                    basic_get_ok_m->content
                );
            assert (tcb->method_out->content);
            tcb->connection->contents_out++;
            ipr_meter_count (demo_broker->ometer);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_basic_get_empty (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_BASIC_GET_EMPTY))
    icl_stats_inc ("demo_server_agent_send_basic_get_empty", &s_demo_server_agent_send_basic_get_empty_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (basic_get_empty_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_get_empty (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_put_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_DIRECT_PUT_OK))
    icl_stats_inc ("demo_server_agent_send_direct_put_ok", &s_demo_server_agent_send_direct_put_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (direct_put_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_direct_put_ok (
                    direct_put_ok_m->lease);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_direct_get_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_DIRECT_GET_OK))
    icl_stats_inc ("demo_server_agent_send_direct_get_ok", &s_demo_server_agent_send_direct_get_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (direct_get_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_direct_get_ok (
                    direct_get_ok_m->lease);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_open_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_OPEN_OK))
    icl_stats_inc ("demo_server_agent_send_connection_open_ok", &s_demo_server_agent_send_connection_open_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_open_ok (NULL);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_open_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_OPEN_OK))
    icl_stats_inc ("demo_server_agent_send_channel_open_ok", &s_demo_server_agent_send_channel_open_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        icl_longstr_t
            *channel_id;                //  Unique channel ID sent to client

        //  Assert channel is OPENING
        assert (tcb->channel_state == ASL_CHANNEL_OPENING);
        //  We only support a single channel, so just send the same ID each time
        channel_id = icl_longstr_new ("0-1#", 4);
        //  Send the channel open ok
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out =
            demo_server_method_new_channel_open_ok (channel_id);
        s_send_method_out (thread, 1, demo_broker->debug_log);
        icl_longstr_destroy (&channel_id);
        //  Channel is now OPEN
        tcb->channel_state = ASL_CHANNEL_OPEN;
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_CLOSE_OK))
    icl_stats_inc ("demo_server_agent_send_channel_close_ok", &s_demo_server_agent_send_channel_close_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Assert channel is CLOSING
        assert (tcb->channel_state == ASL_CHANNEL_CLOSING);
        //  Send the channel close ok
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_channel_close_ok ();
        s_send_method_out (thread, 1, demo_broker->debug_log);
        //  Destroy the channel object
        demo_server_channel_destroy (&tcb->channel);
        //  Channel is now CLOSED
        tcb->channel_state = ASL_CHANNEL_CLOSED;
    
}
#undef  tcb
    return _rc;
}

static int
action_push_method_to_client (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_PUSH_METHOD_TO_CLIENT))
    icl_stats_inc ("demo_server_agent_push_method_to_client", &s_demo_server_agent_push_method_to_client_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is OPEN
        assert (push_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Send method as-is, with no reconstruction
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out = demo_server_method_link (push_m->method);
            s_send_method_out (thread, 1, demo_broker->debug_log);
        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_channel_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CHANNEL_CLOSE))
    icl_stats_inc ("demo_server_agent_send_channel_close", &s_demo_server_agent_send_channel_close_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Assert channel is OPEN
        assert (channel_error_m->channel == tcb->channel);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Report the error
            s_report_error (thread, "Channel", "closing channel",
                channel_error_m->reply_code,
                channel_error_m->reply_text,
                channel_error_m->faulting_class_id,
                channel_error_m->faulting_method_id);
            //  Send the method
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out = demo_server_method_new_channel_close (
                channel_error_m->reply_code,
                channel_error_m->reply_text,
                channel_error_m->faulting_class_id,
                channel_error_m->faulting_method_id);
            s_send_method_out (thread, 1, demo_broker->debug_log);
            //  Channel is now CLOSING
            tcb->channel_state = ASL_CHANNEL_CLOSING;
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_content_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONTENT_HEADER))
    icl_stats_inc ("demo_server_agent_send_content_header", &s_demo_server_agent_send_content_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->content) {
            //  Chronometer on content sent to wire
            demo_content_basic_chrono_add (tcb->method_out->content);
            if (tcb->method_out->class_id == DEMO_SERVER_BASIC) {
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONTENT_BODY_FRAME))
    icl_stats_inc ("demo_server_agent_send_content_body_frame", &s_demo_server_agent_send_content_body_frame_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->class_id == DEMO_SERVER_BASIC)
            payload = demo_content_basic_replay_body (
                tcb->method_out->content, &tcb->reader);
        if (payload) {
            s_write_payload (thread, 1, ASL_FRAME_BODY, &payload);
            smt_thread_set_next_event (thread, continue_event);
        }
        else {
            demo_server_channel_earn (tcb->channel);
            smt_thread_set_next_event (thread, finished_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_shut_down_slave_threads (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SHUT_DOWN_SLAVE_THREADS))
    icl_stats_inc ("demo_server_agent_shut_down_slave_threads", &s_demo_server_agent_shut_down_slave_threads_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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
action_send_connection_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_CLOSE))
    icl_stats_inc ("demo_server_agent_send_connection_close", &s_demo_server_agent_send_connection_close_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Schedule a timeout event to protect against misbehaving clients
        smt_timer_request_delay (thread,
            demo_server_config_close_timeout (demo_server_config) * 1000 * 1000,
            close_timeout_event);
        //  Send connection close
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_close (
            tcb->connection->reply_code,
            tcb->connection->reply_text,
            tcb->connection->faulting_class_id,
            tcb->connection->faulting_method_id);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_CLOSE))
    icl_stats_inc ("demo_server_agent_execute_connection_close", &s_demo_server_agent_execute_connection_close_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_method_execute (tcb->method_in, tcb->connection,
            NULL);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_connection_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_CONNECTION_CLOSE_OK))
    icl_stats_inc ("demo_server_agent_send_connection_close_ok", &s_demo_server_agent_send_connection_close_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_close_ok ();
        s_send_method_out (thread, 0, demo_broker->debug_log);
    
}
#undef  tcb
    return _rc;
}

static int
action_flush_the_connection (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_FLUSH_THE_CONNECTION))
    icl_stats_inc ("demo_server_agent_flush_the_connection", &s_demo_server_agent_flush_the_connection_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SOCKET_ERROR))
    icl_stats_inc ("demo_server_agent_report_socket_error", &s_demo_server_agent_report_socket_error_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "E: %s, connection to client lost "  "(%s, %s, %s, %s)",
            smt_thread_error (thread),
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    
}
#undef  tcb
    return _rc;
}

static int
action_report_socket_timeout (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_SOCKET_TIMEOUT))
    icl_stats_inc ("demo_server_agent_report_socket_timeout", &s_demo_server_agent_report_socket_timeout_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "E: socket timeout, connection to client lost "  "(%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    
}
#undef  tcb
    return _rc;
}

static int
action_send_heartbeat_frame (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SEND_HEARTBEAT_FRAME))
    icl_stats_inc ("demo_server_agent_send_heartbeat_frame", &s_demo_server_agent_send_heartbeat_frame_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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
action_deliver_direct_contents (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DELIVER_DIRECT_CONTENTS))
    icl_stats_inc ("demo_server_agent_deliver_direct_contents", &s_demo_server_agent_deliver_direct_contents_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_content_basic_t
            *content;
        size_t
            pending,
            offset;
        byte
            options;                    //  Publish options octet

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
            tcb->lease->connection->traffic_in += tcb->bucket_in->cur_size;
            content = demo_content_basic_wire_get (tcb->bucket_in, &offset, &options);
            while (content) {
                if (tcb->trace >= ASL_TRACE_LOW) {
                    smt_log_print (demo_broker->debug_log,
                        "I: direct to sink '%s'", tcb->lease->sink->name);
                    icl_console_print ("I: direct to sink '%s'", tcb->lease->sink->name);
                }
                //  Chronometer on content received from wire
                demo_content_basic_chrono_add (content);

                ipr_meter_count (demo_broker->imeter);
                demo_lease_sink (tcb->lease, content, options);
                demo_content_basic_unlink (&content);
                content = demo_content_basic_wire_get (tcb->bucket_in, &offset, &options);
                icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_in));
                tcb->lease->connection->contents_in++;
            }
            //  Reset bucket and get fresh contents data
            tcb->bucket_in->cur_size = 0;
            smt_socket_request_read (thread, tcb->socket, 0,
                12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                direct_in_event);
        }
    
}
#undef  tcb
    return _rc;
}

static int
action_write_direct_content (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DIRECT_CONTENT))
    icl_stats_inc ("demo_server_agent_write_direct_content", &s_demo_server_agent_write_direct_content_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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
                assert (rc == 0);       //  We can't have a message bigger than this
            }
        }
        else {
            //  Batching has been disabled
            batch = FALSE;
            bucket = ipr_bucket_new (demo_content_basic_wire_size (direct_out_m->content));
            rc = demo_content_basic_wire_put (
                direct_out_m->content, bucket, direct_out_m->options);
            assert (rc == 0);
        }
        ipr_meter_count (demo_broker->ometer);
        icl_atomic_dec32 ((volatile qbyte *) &(tcb->lease->pending));
        icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_out));
        tcb->lease->connection->contents_out++;
        if (tcb->trace >= ASL_TRACE_LOW) {
            smt_log_print (demo_broker->debug_log,
                "I: direct from feed '%s'", tcb->lease->feed->name);
            icl_console_print ("I: direct from feed '%s'", tcb->lease->feed->name);
        }
        while (batch && rc == 0 && thread->reply_list->smt_method.list_next != &thread->reply_list->smt_method) {
            method = smt_method_list_pop (thread->reply_list);
            if (method->event == direct_out_m_event) {
                //  Chronometer on content sent to wire
                demo_content_basic_chrono_add (
                    ((demo_server_agent_direct_out_t *) method->data)->content);
                rc = demo_content_basic_wire_put (
                    ((demo_server_agent_direct_out_t *) method->data)->content, bucket, direct_out_m->options);
                if (rc == 0) {
                    ipr_meter_count (demo_broker->ometer);
                    icl_atomic_dec32 ((volatile qbyte *) &(tcb->lease->pending));
                    icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_out));
                    tcb->lease->connection->contents_out++;
                    if (tcb->trace >= ASL_TRACE_LOW) {
                        smt_log_print (demo_broker->debug_log,
                            "I: direct from feed '%s'", tcb->lease->feed->name);
                        icl_console_print ("I: direct from feed '%s'", tcb->lease->feed->name);
                    }
                    smt_method_unlink (&method);
                }
            }
            if (method) {               //  Not bundled, push back & stop
                smt_method_list_push (thread->reply_list, method);
                smt_method_unlink (&method);
                break;
            }
        }
        tcb->lease->connection->traffic_out += bucket->cur_size;
        smt_socket_request_write_bucket (
            thread, tcb->socket, 0, bucket, SMT_NULL_EVENT);
        ipr_bucket_unlink (&bucket);
    
}
#undef  tcb
    return _rc;
}

static int
action_write_direct_heartbeat (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WRITE_DIRECT_HEARTBEAT))
    icl_stats_inc ("demo_server_agent_write_direct_heartbeat", &s_demo_server_agent_write_direct_heartbeat_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        static byte
            null_message [4] = { 0, 0, 0, 0 };

        //  Send a null message and restart the heartbeat timer
        smt_socket_request_write (thread, tcb->socket, 0, 4, null_message, SMT_NULL_EVENT);
        smt_timer_request_delay  (thread, tcb->direct_heartbeat * 1000 * 1000, heartbeat_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_frame_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_FRAME_HEADER))
    icl_stats_inc ("demo_server_agent_read_frame_header", &s_demo_server_agent_read_frame_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        ipr_bucket_unlink (&tcb->bucket_in);
        s_socket_read_bucket (thread, &tcb->bucket_in, ASL_HEADER_SIZE);
    
}
#undef  tcb
    return _rc;
}

static int
action_read_frame_payload (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_READ_FRAME_PAYLOAD))
    icl_stats_inc ("demo_server_agent_read_frame_payload", &s_demo_server_agent_read_frame_payload_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_FRAME_FORMAT))
    icl_stats_inc ("demo_server_agent_check_frame_format", &s_demo_server_agent_check_frame_format_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DECODE_METHOD))
    icl_stats_inc ("demo_server_agent_decode_method", &s_demo_server_agent_decode_method_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Loop if we received a heartbeat frame
        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, read_method_event);
        else
        //  Decode method
        if (tcb->frame_header.type == ASL_FRAME_METHOD) {
            demo_server_method_unlink (&tcb->method_in);
            tcb->method_in = demo_server_method_decode (tcb->bucket_in, tcb->strerror);
            if (tcb->method_in) {
                if (tcb->trace >= ASL_TRACE_LOW)
                    demo_server_method_dump (tcb->method_in,
                    demo_broker->debug_log, "I: recv ");
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_METHOD_CLASS))
    icl_stats_inc ("demo_server_agent_check_method_class", &s_demo_server_agent_check_method_class_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id == DEMO_SERVER_CONNECTION)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id == DEMO_SERVER_CHANNEL)
            smt_thread_set_next_event (thread, channel_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_BASIC
        &&  tcb->method_in->method_id == DEMO_SERVER_BASIC_PUBLISH)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_CONNECTION_METHOD))
    icl_stats_inc ("demo_server_agent_check_connection_method", &s_demo_server_agent_check_connection_method_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is valid
        if (tcb->frame_header.channel != 0)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_START_OK)
            smt_thread_set_next_event (thread, connection_start_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_TUNE_OK)
            smt_thread_set_next_event (thread, connection_tune_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_OPEN)
            smt_thread_set_next_event (thread, connection_open_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_close_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_CHANNEL_METHOD))
    icl_stats_inc ("demo_server_agent_check_channel_method", &s_demo_server_agent_check_channel_method_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_OPEN)
            smt_thread_set_next_event (thread, channel_open_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE)
            smt_thread_set_next_event (thread, channel_close_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE_OK)
            smt_thread_set_next_event (thread, channel_close_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_FLOW)
            smt_thread_set_next_event (thread, channel_flow_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_FLOW_OK)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_FUNCTIONAL_METHOD))
    icl_stats_inc ("demo_server_agent_execute_functional_method", &s_demo_server_agent_execute_functional_method_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Trap possible async exception before executing method
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
        else
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
action_execute_connection_start_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_START_OK))
    icl_stats_inc ("demo_server_agent_execute_connection_start_ok", &s_demo_server_agent_execute_connection_start_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_start_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_START_OK))
    icl_stats_inc ("demo_server_agent_notify_connection_of_start_ok", &s_demo_server_agent_notify_connection_of_start_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_agent_connection_start_ok (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_tune_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_TUNE_OK))
    icl_stats_inc ("demo_server_agent_execute_connection_tune_ok", &s_demo_server_agent_execute_connection_tune_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_TUNE_OK);
        tcb->frame_max = tcb->connection->frame_max;
        tcb->heartbeat = tcb->connection->heartbeat;
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_tune_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_TUNE_OK))
    icl_stats_inc ("demo_server_agent_notify_connection_of_tune_ok", &s_demo_server_agent_notify_connection_of_tune_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_agent_connection_tune_ok (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_connection_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CONNECTION_OPEN))
    icl_stats_inc ("demo_server_agent_execute_connection_open", &s_demo_server_agent_execute_connection_open_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_OPEN);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_OPEN))
    icl_stats_inc ("demo_server_agent_notify_connection_of_open", &s_demo_server_agent_notify_connection_of_open_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_agent_connection_open (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_notify_connection_of_passive_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_PASSIVE_CLOSE))
    icl_stats_inc ("demo_server_agent_notify_connection_of_passive_close", &s_demo_server_agent_notify_connection_of_passive_close_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Report error if any
        s_report_error (thread, "Connection", "closing connection",
            tcb->method_in->payload.connection_close.reply_code,
            tcb->method_in->payload.connection_close.reply_text,
            tcb->method_in->payload.connection_close.class_id,
            tcb->method_in->payload.connection_close.method_id);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_CLOSE_OK))
    icl_stats_inc ("demo_server_agent_notify_connection_of_close_ok", &s_demo_server_agent_notify_connection_of_close_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        demo_server_agent_connection_close_ok (tcb->connection_thread);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_open (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_OPEN))
    icl_stats_inc ("demo_server_agent_execute_channel_open", &s_demo_server_agent_execute_channel_open_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is CLOSED
        if (tcb->channel_state == ASL_CHANNEL_CLOSED) {
            //  Create the channel
            tcb->channel = demo_server_channel_new (
                tcb->connection->channels, tcb->connection,
                1);
            //  Channel is now OPENING
            tcb->channel_state = ASL_CHANNEL_OPENING;
            //  Execute the channel open
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_OPEN);
            //  Queue the channel open ok
            if (!smt_thread_exception_raised (thread))
                demo_server_agent_send_channel_open_ok (tcb->connection_thread);
        }
        else
            //  Channel is not CLOSED
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is already open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_OPEN);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_CLOSE))
    icl_stats_inc ("demo_server_agent_execute_channel_close", &s_demo_server_agent_execute_channel_close_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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
            //  Execute the channel close
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
            //  Queue the channel close ok
            if (!smt_thread_exception_raised (thread))
                demo_server_agent_send_channel_close_ok (tcb->connection_thread);
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_close_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_CLOSE_OK))
    icl_stats_inc ("demo_server_agent_execute_channel_close_ok", &s_demo_server_agent_execute_channel_close_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        //  Check channel is CLOSING
        if (tcb->channel_state == ASL_CHANNEL_CLOSING) {
            //  Destroy the channel object
            demo_server_channel_destroy (&tcb->channel);
            //  Channel is now CLOSED
            tcb->channel_state = ASL_CHANNEL_CLOSED;
        }
        else
            //  Channel is not CLOSING
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not closing",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_flow (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_FLOW))
    icl_stats_inc ("demo_server_agent_execute_channel_flow", &s_demo_server_agent_execute_channel_flow_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW);
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW);
    
}
#undef  tcb
    return _rc;
}

static int
action_execute_channel_flow_ok (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXECUTE_CHANNEL_FLOW_OK))
    icl_stats_inc ("demo_server_agent_execute_channel_flow_ok", &s_demo_server_agent_execute_channel_flow_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW_OK);
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW_OK);
    
}
#undef  tcb
    return _rc;
}

static int
action_expect_content_basic_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONTENT_BASIC_HEADER))
    icl_stats_inc ("demo_server_agent_expect_content_basic_header", &s_demo_server_agent_expect_content_basic_header_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, header_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->frame_header.type == ASL_FRAME_HEADER) {
            ipr_meter_count (demo_broker->imeter);
            tcb->connection->contents_in++;
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
                tcb->method_in->content)->class_id != DEMO_SERVER_BASIC)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_IF_BASIC_BODY_COMPLETE))
    icl_stats_inc ("demo_server_agent_check_if_basic_body_complete", &s_demo_server_agent_check_if_basic_body_complete_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONTENT_BASIC_BODY))
    icl_stats_inc ("demo_server_agent_expect_content_basic_body", &s_demo_server_agent_expect_content_basic_body_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, expect_body_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_EXCEPTION_METHOD))
    icl_stats_inc ("demo_server_agent_expect_exception_method", &s_demo_server_agent_expect_exception_method_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_CHANNEL &&
            tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_EXCEPTION))
    icl_stats_inc ("demo_server_agent_notify_connection_of_exception", &s_demo_server_agent_notify_connection_of_exception_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_NOTIFY_CONNECTION_OF_ERROR))
    icl_stats_inc ("demo_server_agent_notify_connection_of_error", &s_demo_server_agent_notify_connection_of_error_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_METHOD_FRAME))
    icl_stats_inc ("demo_server_agent_expect_method_frame", &s_demo_server_agent_expect_method_frame_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_EXPECT_CONNECTION_CLOSE_OK))
    icl_stats_inc ("demo_server_agent_expect_connection_close_ok", &s_demo_server_agent_expect_connection_close_ok_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_close_ok_event);
        else
            smt_thread_set_next_event (thread, discard_frame_event);
    
}
#undef  tcb
    return _rc;
}

static int
action_wait_for_heartbeat_interval (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_WAIT_FOR_HEARTBEAT_INTERVAL))
    icl_stats_inc ("demo_server_agent_wait_for_heartbeat_interval", &s_demo_server_agent_wait_for_heartbeat_interval_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_CHECK_PEER_LIVENESS))
    icl_stats_inc ("demo_server_agent_check_peer_liveness", &s_demo_server_agent_check_peer_liveness_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_PEER_SLOWING))
    icl_stats_inc ("demo_server_agent_report_peer_slowing", &s_demo_server_agent_report_peer_slowing_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "W: client heartbeat slowing (%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    
}
#undef  tcb
    return _rc;
}

static int
action_shut_down_agent (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_SHUT_DOWN_AGENT))
    icl_stats_inc ("demo_server_agent_shut_down_agent", &s_demo_server_agent_shut_down_agent_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
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

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_REPORT_PEER_STOPPED))
    icl_stats_inc ("demo_server_agent_report_peer_stopped", &s_demo_server_agent_report_peer_stopped_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

        smt_log_print (demo_broker->alert_log,
            "W: client heartbeat stopped, closing connection (%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_close_the_connection_passive_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_PASSIVE_CLOSE))
    icl_stats_inc ("demo_server_agent_dialog_call_close_the_connection_passive_close", &s_demo_server_agent_dialog_call_close_the_connection_passive_close_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 55;
    smt_thread_set_next_event (thread, passive_close_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_return (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_RETURN))
    icl_stats_inc ("demo_server_agent_dialog_return", &s_demo_server_agent_dialog_return_stats);
#endif        
    if (smt_thread_position_pop (thread) == SMT_EMPTY)
        _rc = SMT_EMPTY;
    else
        _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_29 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_29))
    icl_stats_inc ("demo_server_agent_unnamed_29", &s_demo_server_agent_unnamed_29_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            smt_log_print (demo_broker->alert_log,
                "E: timed out waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_30 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_30))
    icl_stats_inc ("demo_server_agent_unnamed_30", &s_demo_server_agent_unnamed_30_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            smt_log_print (demo_broker->alert_log,
                "E: connection error while "   "waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_31 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_31))
    icl_stats_inc ("demo_server_agent_unnamed_31", &s_demo_server_agent_unnamed_31_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            smt_log_print (demo_broker->alert_log,
                "E: received connection close while "   "waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_34 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_34))
    icl_stats_inc ("demo_server_agent_unnamed_34", &s_demo_server_agent_unnamed_34_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_close_the_connection_active_close (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CLOSE_THE_CONNECTION_ACTIVE_CLOSE))
    icl_stats_inc ("demo_server_agent_dialog_call_close_the_connection_active_close", &s_demo_server_agent_dialog_call_close_the_connection_active_close_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 55;
    smt_thread_set_next_event (thread, active_close_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_36 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_36))
    icl_stats_inc ("demo_server_agent_unnamed_36", &s_demo_server_agent_unnamed_36_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_send_content_body_if_any_start (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_SEND_CONTENT_BODY_IF_ANY_START))
    icl_stats_inc ("demo_server_agent_dialog_call_send_content_body_if_any_start", &s_demo_server_agent_dialog_call_send_content_body_if_any_start_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 109;
    smt_thread_set_next_event (thread, start_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_unnamed_58 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_58))
    icl_stats_inc ("demo_server_agent_unnamed_58", &s_demo_server_agent_unnamed_58_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_67 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_67))
    icl_stats_inc ("demo_server_agent_unnamed_67", &s_demo_server_agent_unnamed_67_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            s_connection_exception (thread,
                connection_error_m->reply_code,
                connection_error_m->reply_text,
                connection_error_m->faulting_class_id,
                connection_error_m->faulting_method_id);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_68 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_68))
    icl_stats_inc ("demo_server_agent_unnamed_68", &s_demo_server_agent_unnamed_68_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            demo_server_connection_raise_exception (tcb->connection,
                100, "Operator requested server shutdown", 0, 0);
            
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_69 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_69))
    icl_stats_inc ("demo_server_agent_unnamed_69", &s_demo_server_agent_unnamed_69_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            s_connection_exception (thread,
                ASL_CONNECTION_FORCED,
                "Operator killed connection explicitly", 0, 0);
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_70 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_70))
    icl_stats_inc ("demo_server_agent_unnamed_70", &s_demo_server_agent_unnamed_70_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        
}
#undef  tcb
    return _rc;
}

static int
action_unnamed_71 (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_UNNAMED_71))
    icl_stats_inc ("demo_server_agent_unnamed_71", &s_demo_server_agent_unnamed_71_stats);
#endif        
#define tcb ((demo_server_agent_connection_context_t *) thread->context)
{

            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        
}
#undef  tcb
    return _rc;
}

static int
action_dialog_call_read_method_read_method (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_READ_METHOD_READ_METHOD))
    icl_stats_inc ("demo_server_agent_dialog_call_read_method_read_method", &s_demo_server_agent_dialog_call_read_method_read_method_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 136;
    smt_thread_set_next_event (thread, read_method_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_call_connection_closing_closing (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_CONNECTION_CLOSING_CLOSING))
    icl_stats_inc ("demo_server_agent_dialog_call_connection_closing_closing", &s_demo_server_agent_dialog_call_connection_closing_closing_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 146;
    smt_thread_set_next_event (thread, closing_event);
    _rc = SMT_BREAK;
    return _rc;
}

static int
action_dialog_call_read_basic_content_header (smt_thread_t *thread)
{
    int
        _rc = SMT_OK;

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT)  ||  defined (BASE_STATS_DEMO_SERVER_AGENT_DIALOG_CALL_READ_BASIC_CONTENT_HEADER))
    icl_stats_inc ("demo_server_agent_dialog_call_read_basic_content_header", &s_demo_server_agent_dialog_call_read_basic_content_header_stats);
#endif        
    thread->module++;
    smt_thread_position_push (thread);
    thread->module = 0;
    thread->place = 201;
    smt_thread_set_next_event (thread, header_event);
    _rc = SMT_BREAK;
    return _rc;
}

