/*---------------------------------------------------------------------------
 *  amqp_handle.h - Interface of HANDLE object and QUERY object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef __AMQP_HANDLE_H__
#define __AMQP_HANDLE_H__

#include "amqp_common.h"

typedef void* handle_t;
typedef void* query_t;

#include "amqp_channel.h"

#include "handle_fsm.i"
#include "query_fsm.i"

/*****************************************************************************/
/*  Helper functions (public)                                                */

apr_status_t get_exclusive_access_to_query_dialogue (
    handle_t *ctx
    );

#endif
