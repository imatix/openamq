/*---------------------------------------------------------------------------
 *  amq_stdc_channel.h - Interface of CHANNEL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_CHANNEL_H_INCLUDED
#define AMQ_STDC_CHANNEL_H_INCLUDED

#include "amq_stdc_private.h"

typedef struct tag_channel_context_t* channel_t;

#include "amq_stdc_connection.h"
#include "amq_stdc_handle.h"

#include "channel_fsm.i"

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

apr_status_t channel_get_handle (
    channel_t     context,
    apr_uint16_t  handle_id,
    handle_t      *handle);

#endif
