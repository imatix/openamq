/*---------------------------------------------------------------------------
 *  amq_stdc_global_fsm.h - Interface of GLOBAL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_GLOBAL_H_INCLUDED
#define AMQ_STDC_GLOBAL_H_INCLUDED

#include "amq_stdc_private.h"

typedef struct tag_lock_context_t*       lock_t;
typedef struct tag_global_fsm_context_t* global_fsm_t;

#include "amq_stdc_connection_fsm.h"

#include "amq_stdc_global_fsm.i"

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

apr_status_t register_lock (
    global_fsm_t  context,
    dbyte         connection_id,
    dbyte         channel_id,
    dbyte         handle_id,
    dbyte         *lock_id,
    lock_t        *lock
    );

apr_status_t register_dummy_lock (
    global_fsm_t context,
    void         *result,
    lock_t       *lock
    );

apr_status_t release_lock (
    global_fsm_t  context,
    dbyte         lock_id,
    void          *result
    );

apr_status_t wait_for_lock (
    lock_t  lock,
    void    **result
    );

apr_status_t release_all_locks (
    global_fsm_t  context,
    dbyte         connection_id,
    dbyte         channel_id,
    dbyte         handle_id,
    dbyte         except_lock_id,
    apr_status_t  error
    );

#endif
