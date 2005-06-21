/*---------------------------------------------------------------------------
 *  amq_stdc_handle_fsm.d - definition file for HANDLE_FSM state machine
 *  Not to be compiled directly.
 *  Include it into your c file and expand the macros.
 *  Define AMQTRACE_FSMS prior to including this file to trace control flow.
 *
 *  Generated from amq_stdc_handle_fsm.xml by amq_stdc_fsms.gsl using GSL/4.
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifdef AMQTRACE_FSMS
#define HANDLE_FSM_TRACE(old_state, event, action, new_state) \
    fprintf (stderr,\
        "HANDLE_FSM (%ld)\n"\
        "    " #old_state ":\n"\
        "        (--) " #event "  " #new_state "\n"\
        "            + " #action "\n",\
        (long) (HANDLE_FSM_OBJECT_ID)\
        );
#else
#define HANDLE_FSM_TRACE(old_state, event, action, new_state)
#endif

typedef enum
{
    handle_fsm_state_initial,
    handle_fsm_state_expect_created,
    handle_fsm_state_active,
    handle_fsm_state_expect_close,
    handle_fsm_state_terminal
} handle_fsm_state_t;

#define DEFINE_HANDLE_FSM_CONTEXT_BEGIN \
typedef struct tag_handle_fsm_context_t\
{\
    handle_fsm_state_t\
        state;\
    apr_thread_mutex_t\
        *sync;\
    apr_pool_t\
        *pool;

#define DEFINE_HANDLE_FSM_CONTEXT_END \
} handle_fsm_context_t;\
\
inline static apr_status_t handle_fsm_sync_begin (handle_fsm_context_t *context)\
{\
    apr_status_t\
        result;\
\
    result = apr_thread_mutex_lock (context->sync);\
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_lock)\
    return APR_SUCCESS;\
}\
\
inline static apr_status_t handle_fsm_sync_end (handle_fsm_context_t *context)\
{\
    apr_status_t\
        result;\
\
    result = apr_thread_mutex_unlock (context->sync);\
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_unlock)\
    return APR_SUCCESS;\
}\
inline static apr_status_t do_construct (\
    handle_fsm_context_t *context\
    );\
\
inline static apr_status_t do_destruct (\
    handle_fsm_context_t *context\
    );\
\
\
inline static apr_status_t do_init (\
    handle_fsm_context_t *context,\
    global_fsm_t global,\
    connection_fsm_t connection,\
    channel_fsm_t channel,\
    dbyte connection_id,\
    dbyte channel_id,\
    dbyte handle_id,\
    dbyte service_type,\
    byte producer,\
    byte consumer,\
    byte browser,\
    const char * dest_name,\
    const char * mime_type,\
    const char * encoding,\
    amq_stdc_table_t options,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_init_temporary (\
    handle_fsm_context_t *context,\
    global_fsm_t global,\
    connection_fsm_t connection,\
    channel_fsm_t channel,\
    dbyte connection_id,\
    dbyte channel_id,\
    dbyte handle_id,\
    dbyte service_type,\
    byte producer,\
    byte consumer,\
    byte browser,\
    const char * dest_name,\
    const char * mime_type,\
    const char * encoding,\
    amq_stdc_table_t options,\
    byte async,\
    amq_stdc_lock_t * created_lock,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_created (\
    handle_fsm_context_t *context,\
    dbyte dest_name_size,\
    const char * dest_name\
    );\
\
inline static apr_status_t do_consume (\
    handle_fsm_context_t *context,\
    dbyte prefetch,\
    byte no_local,\
    byte unreliable,\
    const char * dest_name,\
    const char * identifier,\
    const char * selector,\
    const char * mime_type,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_send_message (\
    handle_fsm_context_t *context,\
    byte out_of_band,\
    byte recovery,\
    byte streaming,\
    const char * dest_name,\
    byte persistent,\
    byte priority,\
    qbyte expiration,\
    const char * mime_type,\
    const char * encoding,\
    const char * identifier,\
    apr_size_t data_size,\
    void * data,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_flow (\
    handle_fsm_context_t *context,\
    byte pause,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_cancel (\
    handle_fsm_context_t *context,\
    const char * dest_name,\
    const char * identifier,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_unget (\
    handle_fsm_context_t *context,\
    qbyte message_nbr,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_query (\
    handle_fsm_context_t *context,\
    qbyte message_nbr,\
    const char * dest_name,\
    const char * selector,\
    const char * mime_type,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_index (\
    handle_fsm_context_t *context,\
    qbyte message_nbr,\
    qbyte message_list_size,\
    const char * message_list\
    );\
\
inline static apr_status_t do_browse (\
    handle_fsm_context_t *context,\
    qbyte message_nbr,\
    byte async,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_duplicate_terminate (\
    handle_fsm_context_t *context,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_client_requested_close (\
    handle_fsm_context_t *context\
    );\
\
inline static apr_status_t do_terminate (\
    handle_fsm_context_t *context,\
    amq_stdc_lock_t * lock\
    );\
\
inline static apr_status_t do_server_requested_close (\
    handle_fsm_context_t *context\
    );\
\
apr_status_t handle_fsm_init (\
    handle_fsm_t ctx,\
    global_fsm_t global,\
    connection_fsm_t connection,\
    channel_fsm_t channel,\
    dbyte connection_id,\
    dbyte channel_id,\
    dbyte handle_id,\
    dbyte service_type,\
    byte producer,\
    byte consumer,\
    byte browser,\
    const char * dest_name,\
    const char * mime_type,\
    const char * encoding,\
    amq_stdc_table_t options,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_initial:\
        HANDLE_FSM_TRACE(initial, init, do-init, ->  active)\
        result = do_init (\
            context,\
            global,\
            connection,\
            channel,\
            connection_id,\
            channel_id,\
            handle_id,\
            service_type,\
            producer,\
            consumer,\
            browser,\
            dest_name,\
            mime_type,\
            encoding,\
            options,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_init)\
        context->state = handle_fsm_state_active;\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_init_temporary (\
    handle_fsm_t ctx,\
    global_fsm_t global,\
    connection_fsm_t connection,\
    channel_fsm_t channel,\
    dbyte connection_id,\
    dbyte channel_id,\
    dbyte handle_id,\
    dbyte service_type,\
    byte producer,\
    byte consumer,\
    byte browser,\
    const char * dest_name,\
    const char * mime_type,\
    const char * encoding,\
    amq_stdc_table_t options,\
    byte async,\
    amq_stdc_lock_t * created_lock,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_initial:\
        HANDLE_FSM_TRACE(initial, init-temporary, do-init-temporary, ->  expect-created)\
        result = do_init_temporary (\
            context,\
            global,\
            connection,\
            channel,\
            connection_id,\
            channel_id,\
            handle_id,\
            service_type,\
            producer,\
            consumer,\
            browser,\
            dest_name,\
            mime_type,\
            encoding,\
            options,\
            async,\
            created_lock,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_init_temporary)\
        context->state = handle_fsm_state_expect_created;\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_created (\
    handle_fsm_t ctx,\
    dbyte dest_name_size,\
    const char * dest_name\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_expect_created:\
        HANDLE_FSM_TRACE(expect-created, created, do-created, ->  active)\
        result = do_created (\
            context,\
            dest_name_size,\
            dest_name\
            );\
        AMQ_ASSERT_STATUS (result, do_created)\
        context->state = handle_fsm_state_active;\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_consume (\
    handle_fsm_t ctx,\
    dbyte prefetch,\
    byte no_local,\
    byte unreliable,\
    const char * dest_name,\
    const char * identifier,\
    const char * selector,\
    const char * mime_type,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, consume, do-consume, )\
        result = do_consume (\
            context,\
            prefetch,\
            no_local,\
            unreliable,\
            dest_name,\
            identifier,\
            selector,\
            mime_type,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_consume)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_send_message (\
    handle_fsm_t ctx,\
    byte out_of_band,\
    byte recovery,\
    byte streaming,\
    const char * dest_name,\
    byte persistent,\
    byte priority,\
    qbyte expiration,\
    const char * mime_type,\
    const char * encoding,\
    const char * identifier,\
    apr_size_t data_size,\
    void * data,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, send-message, do-send-message, )\
        result = do_send_message (\
            context,\
            out_of_band,\
            recovery,\
            streaming,\
            dest_name,\
            persistent,\
            priority,\
            expiration,\
            mime_type,\
            encoding,\
            identifier,\
            data_size,\
            data,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_send_message)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_flow (\
    handle_fsm_t ctx,\
    byte pause,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, flow, do-flow, )\
        result = do_flow (\
            context,\
            pause,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_flow)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_cancel (\
    handle_fsm_t ctx,\
    const char * dest_name,\
    const char * identifier,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, cancel, do-cancel, )\
        result = do_cancel (\
            context,\
            dest_name,\
            identifier,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_cancel)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_unget (\
    handle_fsm_t ctx,\
    qbyte message_nbr,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, unget, do-unget, )\
        result = do_unget (\
            context,\
            message_nbr,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_unget)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_query (\
    handle_fsm_t ctx,\
    qbyte message_nbr,\
    const char * dest_name,\
    const char * selector,\
    const char * mime_type,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, query, do-query, )\
        result = do_query (\
            context,\
            message_nbr,\
            dest_name,\
            selector,\
            mime_type,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_query)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_index (\
    handle_fsm_t ctx,\
    qbyte message_nbr,\
    qbyte message_list_size,\
    const char * message_list\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, index, do-index, )\
        result = do_index (\
            context,\
            message_nbr,\
            message_list_size,\
            message_list\
            );\
        AMQ_ASSERT_STATUS (result, do_index)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_browse (\
    handle_fsm_t ctx,\
    qbyte message_nbr,\
    byte async,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_active:\
        HANDLE_FSM_TRACE(active, browse, do-browse, )\
        result = do_browse (\
            context,\
            message_nbr,\
            async,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_browse)\
        break;\
    default:\
        assert (0);\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_terminate (\
    handle_fsm_t ctx,\
    amq_stdc_lock_t * lock\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_expect_close:\
        HANDLE_FSM_TRACE(expect-close, terminate, do-duplicate-terminate, )\
        result = do_duplicate_terminate (\
            context,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_duplicate_terminate)\
        break;\
    default:\
        HANDLE_FSM_TRACE(default, terminate, do-terminate, ->  expect-close)\
        result = do_terminate (\
            context,\
            lock\
            );\
        AMQ_ASSERT_STATUS (result, do_terminate)\
        context->state = handle_fsm_state_expect_close;\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_close (\
    handle_fsm_t ctx\
    )\
{\
    apr_status_t\
        result;\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    result = handle_fsm_sync_begin (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_begin);\
    switch (context->state)\
    {\
    case handle_fsm_state_expect_close:\
        HANDLE_FSM_TRACE(expect-close, close, do-client-requested-close, ->  terminal)\
        result = do_client_requested_close (\
            context\
            );\
        AMQ_ASSERT_STATUS (result, do_client_requested_close)\
        context->state = handle_fsm_state_terminal;\
        break;\
    default:\
        HANDLE_FSM_TRACE(default, close, do-server-requested-close, ->  terminal)\
        result = do_server_requested_close (\
            context\
            );\
        AMQ_ASSERT_STATUS (result, do_server_requested_close)\
        context->state = handle_fsm_state_terminal;\
    }\
    result = handle_fsm_sync_end (context);\
    AMQ_ASSERT_STATUS (result, handle_fsm_sync_end);\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_create (\
    handle_fsm_t *out\
    )\
{\
   apr_status_t\
        result;\
    handle_fsm_context_t\
        *context;\
    apr_pool_t\
        *pool;\
\
    result = apr_pool_create (&pool, NULL);\
    AMQ_ASSERT_STATUS (result, apr_pool_create)\
    context = (handle_fsm_context_t*) apr_palloc (pool,\
        sizeof (handle_fsm_context_t));\
    if (!context)\
        AMQ_ASSERT (Not enough memory for handle_fsm object)\
    context->state = handle_fsm_state_initial;\
    context->pool = pool;\
    result = apr_thread_mutex_create (&(context->sync),\
        APR_THREAD_MUTEX_NESTED, pool);\
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_create)\
    result = do_construct (context);\
    AMQ_ASSERT_STATUS (result, do_construct)\
    if (out) *out = (handle_fsm_t) context;\
    return APR_SUCCESS;\
}\
\
apr_status_t handle_fsm_destroy (\
    handle_fsm_t ctx\
    )\
{\
    handle_fsm_context_t\
        *context = (handle_fsm_context_t*) ctx;\
    do_destruct (context);\
    apr_thread_mutex_destroy (context->sync);\
    apr_pool_destroy (context->pool);\
    return APR_SUCCESS;\
}

