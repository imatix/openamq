/*---------------------------------------------------------------------------
 *  amq_stdc_connection.h - Interface of CONNECTION object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_CONNECTION_H_INCLUDED
#define AMQ_STDC_CONNECTION_H_INCLUDED

#include "amq_stdc_private.h"

typedef struct tag_connection_context_t* connection_t;

#include "amq_stdc_global.h"
#include "amq_stdc_channel.h"
#include "amq_stdc_handle.h"

#include "connection_fsm.i"

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

apr_status_t connection_get_channel (
    connection_t  context,
    apr_uint16_t  channel_id,
    channel_t     *channel);

apr_status_t connection_get_handle (
    connection_t  context,
    apr_uint16_t  handle_id,
    handle_t      *handle
    );

#endif
