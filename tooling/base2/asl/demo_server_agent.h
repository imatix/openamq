/*---------------------------------------------------------------------------
    demo_server_agent.h - prototypes for demo_server_agent.

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

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_DEMO_SERVER_AGENT_SAFE
#   define INCLUDE_DEMO_SERVER_AGENT_SAFE
#   define INCLUDE_DEMO_SERVER_AGENT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_SERVER_AGENT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "demo_broker.h"
#   include "demo_lease.h"
#   include "demo_server_method.h"

#   undef INCLUDE_DEMO_SERVER_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_AGENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_SERVER_AGENT_UNSAFE
#   define INCLUDE_DEMO_SERVER_AGENT_UNSAFE
#   define INCLUDE_DEMO_SERVER_AGENT_ACTIVE

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "demo_broker.h"
#   include "demo_lease.h"
#   include "demo_server_method.h"
#ifdef __cplusplus
extern "C" {
#endif

int
demo_server_agent_kill_connection (
    smt_thread_t * thread);

int
demo_server_agent_connection_start_ok (
    smt_thread_t * thread);

int
demo_server_agent_connection_tune_ok (
    smt_thread_t * thread);

int
demo_server_agent_connection_open (
    smt_thread_t * thread);

int
demo_server_agent_send_channel_open_ok (
    smt_thread_t * thread);

int
demo_server_agent_send_channel_close_ok (
    smt_thread_t * thread);

int
demo_server_agent_connection_close_ok (
    smt_thread_t * thread);

int
demo_server_agent_shutdown (
    smt_thread_t * thread);

int
demo_server_agent_set_trace (
    smt_thread_t * thread,
    int                   trace);

int
demo_server_agent_connection_error (
    smt_thread_t * thread,
    dbyte                 reply_code,
    char *                reply_text,
    dbyte                 faulting_method_id,
    dbyte                 faulting_class_id);

int
demo_server_agent_channel_error (
    smt_thread_t * thread,
    demo_server_channel_t * channel,
    dbyte                 reply_code,
    char *                reply_text,
    dbyte                 faulting_method_id,
    dbyte                 faulting_class_id);

int
demo_server_agent_channel_flow (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active);

int
demo_server_agent_channel_flow_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    Bool                  active);

int
demo_server_agent_exchange_declare_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr);

int
demo_server_agent_exchange_delete_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr);

int
demo_server_agent_queue_declare_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                queue,
    icl_longstr_t *       properties);

int
demo_server_agent_queue_bind_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr);

int
demo_server_agent_queue_purge_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    qbyte                 message_count);

int
demo_server_agent_queue_delete_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    qbyte                 message_count);

int
demo_server_agent_basic_consume_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                consumer_tag);

int
demo_server_agent_basic_cancel_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                consumer_tag);

int
demo_server_agent_basic_deliver (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_content_basic_t * content,
    char *                exchange,
    char *                routing_key,
    int64_t               delivery_tag,
    void *                callback);

int
demo_server_agent_basic_get_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_content_basic_t * content,
    char *                exchange,
    char *                routing_key,
    qbyte                 message_count,
    void *                callback);

int
demo_server_agent_basic_get_empty (
    smt_thread_t * thread,
    dbyte                 channel_nbr);

int
demo_server_agent_direct_put_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                lease);

int
demo_server_agent_direct_get_ok (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    char *                lease);

int
demo_server_agent_push (
    smt_thread_t * thread,
    dbyte                 channel_nbr,
    demo_server_method_t * method);

int
demo_server_agent_direct_out (
    smt_thread_t * thread,
    demo_content_basic_t * content,
    byte                  options);

int  demo_server_agent_init (
    smt_thread_t ** thread_p);
void demo_server_agent_term (void);
void demo_server_agent_animate (Bool animate);
smt_thread_t *demo_server_agent_master_thread_new (void);
smt_thread_t *demo_server_agent_connection_thread_new (void);
smt_thread_t *demo_server_agent_input_thread_new (
    smt_thread_t * share);
smt_thread_t *demo_server_agent_heartbeat_thread_new (
    smt_thread_t * share);

#ifdef __cplusplus
}
#endif


#   undef INCLUDE_DEMO_SERVER_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_AGENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_SERVER_AGENT_INLINE)
#   define INCLUDE_DEMO_SERVER_AGENT_INLINE
#   define INCLUDE_DEMO_SERVER_AGENT_ACTIVE

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "demo_broker.h"
#   include "demo_lease.h"
#   include "demo_server_method.h"

#   undef INCLUDE_DEMO_SERVER_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_AGENT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_SERVER_AGENT_ROOT
#   endif
# endif
#endif
