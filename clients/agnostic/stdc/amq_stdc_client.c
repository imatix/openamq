/*---------------------------------------------------------------------------
 *  amq_stdc_client.c - implementation of AMQ client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_private.h"
#include "amq_stdc_client.h"
#include "amq_stdc_global.h"

static global_t
    global;

apr_status_t amq_stdc_init ()
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    result = global_create (&global);
    TEST(result, global_create, buffer)
    result = global_init (global);
    TEST(result, global_create, buffer)
 
    return APR_SUCCESS;
}

apr_status_t amq_stdc_term ()
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = global_terminate (global, &lock);
    TEST(result, global_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    amq_stats ();
    return APR_SUCCESS;
}

apr_status_t amq_stdc_open_connection (
    const char                  *server,
    const char                  *host,
    const char                  *client_name,
    amq_stdc_heartbeat_model_t  out_heartbeat_model,
    amq_stdc_heartbeat_model_t  in_heartbeat_model,
    apr_interval_time_t         in_heartbeat_interval,
    apr_byte_t                  async,
    amq_stdc_connection_t       *out
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = global_create_connection (global, server, host, client_name,
        async, &lock);
    TEST(result, global_create_connection, buffer);
    result = wait_for_lock (lock, (void**) out);
    TEST(result, global_create_connection, buffer);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_close_connection (
    amq_stdc_connection_t  context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = connection_terminate (context, &lock);
    TEST(result, connection_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    result = connection_destroy (context);
    TEST(result, connection_destroy, buffer)
 
    return APR_SUCCESS;
}

apr_status_t amq_stdc_open_channel (
    amq_stdc_connection_t  context,
    apr_byte_t         transacted,
    apr_byte_t         restartable,
    apr_byte_t         async,
    amq_stdc_channel_t     *out)
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = connection_create_channel (context, transacted, restartable,
        async, &lock);
    TEST(result, connection_create_channel, buffer);
    result = wait_for_lock (lock, (void**) out);
    TEST(result, wait_for_lock, buffer);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_acknowledge (
    amq_stdc_channel_t  context,
    apr_uint32_t    message_nbr,
    apr_byte_t      async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = channel_acknowledge (context, message_nbr, async, &lock);
    TEST(result, channel_acknowledge, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_commit (
    amq_stdc_channel_t  context,
    apr_byte_t      async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = channel_commit (context, async, &lock);
    TEST(result, channel_commit, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_rollback (
    amq_stdc_channel_t  context,
    apr_byte_t      async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = channel_rollback (context, async, &lock);
    TEST(result, channel_rollback, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_close_channel (
    amq_stdc_channel_t  context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = channel_terminate (context, &lock);
    TEST(result, channel_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    result = channel_destroy (context);
    TEST(result, channel_destroy, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_open_handle (
    amq_stdc_channel_t       context,
    amq_stdc_service_type_t  service_type,
    apr_byte_t               producer,
    apr_byte_t               consumer,
    apr_byte_t               browser,
    apr_byte_t               temporary,
    char                     *dest_name,
    char                     *mime_type,
    char                     *encoding,
    apr_byte_t               async,
    char                     **dest_name_out,
    amq_stdc_handle_t        *out
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;
    lock_t
        created_lock;

    result = channel_create_handle (context, service_type, producer, consumer,
        browser, temporary, dest_name, mime_type, encoding, async,
        &created_lock, &lock);
    TEST(result, channel_create_handle, buffer);
    /*  Wait for HANDLE CREATED                                              */
    result = wait_for_lock (created_lock, (void**) dest_name_out);
    TEST(result, wait_for_lock, buffer);
    /*  Wait for confirmation                                                */
    result = wait_for_lock (lock, (void**) out);
    TEST(result, wait_for_lock, buffer);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_consume (
    amq_stdc_handle_t  context,
    apr_uint16_t   prefetch,
    apr_byte_t     no_local,
    apr_byte_t     unreliable,
    const char     *dest_name,
    const char     *identifier,
    const char     *mime_type,
    apr_byte_t     async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_consume (context, prefetch, no_local, unreliable,
        dest_name, identifier, mime_type, async, &lock);
    TEST(result, handle_consume, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_get_message (
    amq_stdc_handle_t  handle,
    amqp_frame_t   **message
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_get_message (handle, &lock);
    TEST(result, handle_get_message, buffer)
    result = wait_for_lock (lock, (void**) message);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_destroy_message (
    amqp_frame_t  *message
    )
{
    amq_free ((void*) message);
    return APR_SUCCESS;
}

apr_status_t amq_stdc_send_message (
    amq_stdc_handle_t  context,
    apr_byte_t     out_of_band,
    apr_byte_t     recovery,
    apr_byte_t     streaming,
    const char     *dest_name,
    apr_byte_t     persistent,
    apr_byte_t     priority,
    apr_uint32_t   expiration,
    const char     *mime_type,
    const char     *encoding,
    const char     *identifier,
    apr_size_t     data_size,
    void           *data,
    apr_byte_t     async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_send_message (context, out_of_band, recovery,
        streaming, dest_name, persistent, priority, expiration, mime_type,
        encoding, identifier, data_size, data, async, &lock);
    TEST(result, handle_put_message, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_flow (
    amq_stdc_handle_t  context,
    apr_byte_t     pause,
    apr_byte_t     async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_flow (context, pause, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_cancel_subscription (
    amq_stdc_handle_t  context,
    const char     *dest_name,
    const char     *identifier,
    apr_byte_t     async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_cancel (context, dest_name, identifier, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_unget_message (
    amq_stdc_handle_t  context,
    apr_uint32_t   message_nbr,
    apr_byte_t     async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_unget (context, message_nbr, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_close_handle (
    amq_stdc_handle_t  context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = handle_terminate (context, &lock);
    TEST(result, handle_term, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    result = handle_destroy (context);
    TEST(result, handle_destroy, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_query (
    amq_stdc_handle_t  context,
    apr_uint32_t   message_nbr,
    const char     *dest_name,
    const char     *mime_type,
    apr_byte_t     partial,
    char           **resultset
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;

    result = get_exclusive_access_to_query_dialogue (context);
    TEST(result, get_exclusive_access_to_query_dialogue, buffer);
    result = handle_query (context, message_nbr, dest_name, mime_type, &lock);
    TEST(result, handle_create_query, buffer)
    result = wait_for_lock (lock, (void**) resultset);
    TEST(result, wait_for_lock, buffer)

    return APR_SUCCESS;
}

apr_status_t amq_stdc_destroy_query (
    char  *query
    )
{
    amq_free ((void*) query);
    return APR_SUCCESS;
}

apr_status_t amq_stdc_browse (
    amq_stdc_handle_t  context,
    apr_uint32_t   message_nbr,
    apr_byte_t     async,
    amqp_frame_t   **message
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_t
        lock;
 
    result = handle_browse (context, message_nbr, async, &lock);
    TEST(result, handle_term, buffer)
    result = wait_for_lock (lock, (void**) message);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}
