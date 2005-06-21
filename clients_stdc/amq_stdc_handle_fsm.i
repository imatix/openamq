/*---------------------------------------------------------------------------
 *  amq_stdc_handle_fsm.i - header for HANDLE_FSM state machine
 *
 *  Generated from amq_stdc_handle_fsm.xml by amq_stdc_fsms.gsl using GSL/4.
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef __HANDLE_FSM_FSM_H__
#define __HANDLE_FSM_FSM_H__

apr_status_t handle_fsm_create (
    handle_fsm_t *out
    );

apr_status_t handle_fsm_destroy (
    handle_fsm_t context
    );

apr_status_t handle_fsm_init (
    handle_fsm_t context,
    global_fsm_t global,
    connection_fsm_t connection,
    channel_fsm_t channel,
    dbyte connection_id,
    dbyte channel_id,
    dbyte handle_id,
    dbyte service_type,
    byte producer,
    byte consumer,
    byte browser,
    const char * dest_name,
    const char * mime_type,
    const char * encoding,
    amq_stdc_table_t options,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_init_temporary (
    handle_fsm_t context,
    global_fsm_t global,
    connection_fsm_t connection,
    channel_fsm_t channel,
    dbyte connection_id,
    dbyte channel_id,
    dbyte handle_id,
    dbyte service_type,
    byte producer,
    byte consumer,
    byte browser,
    const char * dest_name,
    const char * mime_type,
    const char * encoding,
    amq_stdc_table_t options,
    byte async,
    amq_stdc_lock_t * created_lock,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_created (
    handle_fsm_t context,
    dbyte dest_name_size,
    const char * dest_name
    );

apr_status_t handle_fsm_consume (
    handle_fsm_t context,
    dbyte prefetch,
    byte no_local,
    byte unreliable,
    const char * dest_name,
    const char * identifier,
    const char * selector,
    const char * mime_type,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_send_message (
    handle_fsm_t context,
    byte out_of_band,
    byte recovery,
    byte streaming,
    const char * dest_name,
    byte persistent,
    byte priority,
    qbyte expiration,
    const char * mime_type,
    const char * encoding,
    const char * identifier,
    apr_size_t data_size,
    void * data,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_flow (
    handle_fsm_t context,
    byte pause,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_cancel (
    handle_fsm_t context,
    const char * dest_name,
    const char * identifier,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_unget (
    handle_fsm_t context,
    qbyte message_nbr,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_query (
    handle_fsm_t context,
    qbyte message_nbr,
    const char * dest_name,
    const char * selector,
    const char * mime_type,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_index (
    handle_fsm_t context,
    qbyte message_nbr,
    qbyte message_list_size,
    const char * message_list
    );

apr_status_t handle_fsm_browse (
    handle_fsm_t context,
    qbyte message_nbr,
    byte async,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_terminate (
    handle_fsm_t context,
    amq_stdc_lock_t * lock
    );

apr_status_t handle_fsm_close (
    handle_fsm_t context
    );

#endif
