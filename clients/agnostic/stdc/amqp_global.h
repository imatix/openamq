/*---------------------------------------------------------------------------
 *  amqp_global.h - Interface of GLOBAL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef __AMQP_GLOBAL_H__
#define __AMQP_GLOBAL_H__

#include "amqp_common.h"

typedef void* global_t;

#include "amqp_connection.h"

#include "global_fsm.i"

/*****************************************************************************/
/*  Helper functions (public)                                                */

apr_status_t global_remove_connection (
    global_t context,
    connection_t connection
    );

#endif
