/*---------------------------------------------------------------------------
 *  amq_stdc_connection.h - Interface of CONNECTION object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_CONNECTION_H_INCLUDED
#define AMQ_STDC_CONNECTION_H_INCLUDED

#include "amq_stdc_private.h"

typedef struct tag_connection_fsm_context_t* connection_fsm_t;

#include "amq_stdc_global_fsm.h"
#include "amq_stdc_channel_fsm.h"
#include "amq_stdc_handle_fsm.h"

#include "amq_stdc_connection_fsm.i"

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

apr_status_t connection_get_channel (
    connection_fsm_t  context,
    apr_uint16_t      channel_id,
    channel_fsm_t     *channel);

apr_status_t connection_get_handle (
    connection_fsm_t  context,
    apr_uint16_t      handle_id,
    handle_fsm_t      *handle
    );

#endif
