/*---------------------------------------------------------------------------
 *  amq_stdc_handle.h - Interface of HANDLE object and QUERY object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_HANDLE_H_INCLUDED
#define AMQ_STDC_HANDLE_H_INCLUDED

#include "amq_stdc_private.h"

typedef struct tag_handle_fsm_context_t* handle_fsm_t;

#include "amq_stdc_channel_fsm.h"

#include "amq_stdc_handle_fsm.i"

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

apr_status_t get_exclusive_access_to_query_dialogue (
    handle_fsm_t  ctx
    );

#endif
