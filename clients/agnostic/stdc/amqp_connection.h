/*---------------------------------------------------------------------------
 *  amqp_connection.h - Interface of CONNECTION object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef __AMQP_CONNECTION_H__
#define __AMQP_CONNECTION_H__

#include "amqp_common.h"

typedef void* amqp_lock_t;
typedef void* connection_t;

#include "amqp_global.h"
#include "amqp_channel.h"
#include "amqp_handle.h"

#include "connection_fsm.i"

/*****************************************************************************/
/*  Helper functions (public)                                                */

apr_status_t register_lock (
    connection_t context,
    apr_uint16_t channel_id,
    apr_uint16_t handle_id,
    apr_uint16_t *lock_id,
    amqp_lock_t *lock
    );

apr_status_t release_lock (
    connection_t context,
    apr_uint16_t lock_id,
    void *result
    );

apr_status_t wait_for_lock (
    amqp_lock_t lock,
    void **result
    );

apr_status_t release_all_locks (
    connection_t context,
    apr_uint16_t channel_id,
    apr_uint16_t handle_id,
    apr_uint16_t except_lock_id,
    apr_status_t error
    );

apr_status_t connection_get_channel (
    connection_t context,
    apr_uint16_t channel_id,
    channel_t *channel);

apr_status_t connection_get_handle (
    connection_t context,
    apr_uint16_t handle_id,
    handle_t *handle
    );

apr_status_t connection_remove_channel (
    connection_t context,
    channel_t channel
    );

apr_status_t connection_assign_new_handle_id (
    connection_t context,
    apr_uint16_t *handle_id
    );

#endif
