/*---------------------------------------------------------------------------
 *  amqp_channel.h - Interface of CHANNEL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef __AMQP_CHANNEL_H__
#define __AMQP_CHANNEL_H__

#include "amqp_common.h"

typedef void* channel_t;

#include "amqp_connection.h"
#include "amqp_handle.h"

#include "channel_fsm.i"

/*****************************************************************************/
/*  Helper functions (public)                                                */

apr_status_t channel_get_handle (
    channel_t context,
    apr_uint16_t handle_id,
    handle_t *handle);

apr_status_t channel_remove_handle (
    channel_t context,
    handle_t handle
    );

#endif
