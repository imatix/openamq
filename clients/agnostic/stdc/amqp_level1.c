
#include "amqp_common.h"
#include "amqp_level1.h"
#include "amqp_global.h"

static global_t
    global;

apr_status_t amqp_init1 ()
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

apr_status_t amqp_term1 ()
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = global_terminate (global, &lock);
    TEST(result, global_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    return APR_SUCCESS;
}

apr_status_t amqp_open_connection (
    const char *server,
    const char *host,
    const char *client_name,
    amqp_heartbeat_model_t out_heartbeat_model,
    amqp_heartbeat_model_t in_heartbeat_model,
    apr_interval_time_t in_heartbeat_interval,
    apr_byte_t async,
    amqp_connection_t *out
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = global_create_connection (global, server, host, client_name,
        async, &lock);
    TEST(result, global_create_connection, buffer);
    result = wait_for_lock (lock, (void**) out);
    TEST(result, global_create_connection, buffer);

    return APR_SUCCESS;
}

apr_status_t amqp_close_connection (
    amqp_connection_t context,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = connection_terminate (context, &lock);
    TEST(result, connection_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
 
    /*  Whose responsibility is to call connection_destroy ?  */
    /*  Calling it here in async mode would cause block till */
    /*  connection threads are stopped, thus making it sync. */
   
    return APR_SUCCESS;
}

apr_status_t amqp_open_channel (
    amqp_connection_t context,
    apr_byte_t transacted,
    apr_byte_t restartable,
    apr_byte_t async,
    amqp_channel_t *out)
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = connection_create_channel (context, transacted, restartable,
        async, &lock);
    TEST(result, connection_create_channel, buffer);
    result = wait_for_lock (lock, (void**) out);
    TEST(result, wait_for_lock, buffer);

    return APR_SUCCESS;
}

apr_status_t amqp_acknowledge (
    amqp_channel_t context,
    apr_uint32_t message_nbr,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = channel_acknowledge (context, message_nbr, async, &lock);
    TEST(result, channel_acknowledge, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_commit (
    amqp_channel_t context,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = channel_commit (context, async, &lock);
    TEST(result, channel_commit, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_rollback (
    amqp_channel_t context,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = channel_rollback (context, async, &lock);
    TEST(result, channel_rollback, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_close_channel (
    amqp_channel_t context,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = channel_terminate (context, &lock);
    TEST(result, channel_terminate, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_open_handle (
    amqp_channel_t context,
    amqp_service_type_t service_type,
    apr_byte_t producer,
    apr_byte_t consumer,
    apr_byte_t browser,
    apr_byte_t temporary,
    char *dest_name,
    char *mime_type,
    char *encoding,
    apr_byte_t async,
    char **dest_name_out,
    amqp_handle_t *out
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;
    amqp_lock_t
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

apr_status_t amqp_consume (
    amqp_handle_t context,
    apr_uint16_t prefetch,
    apr_byte_t no_local,
    apr_byte_t unreliable,
    const char *dest_name,
    const char *identifier,
    const char *mime_type,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_consume (context, prefetch, no_local, unreliable,
        dest_name, identifier, mime_type, async, &lock);
    TEST(result, handle_consume, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_receive_message (
    amqp_handle_t handle,
    amqp_frame_t **message
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_get_message (handle, &lock);
    TEST(result, handle_get_message, buffer)
    result = wait_for_lock (lock, (void**) message);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_destroy_message (
    amqp_frame_t *message
    )
{
    free ((void*) message);
    return APR_SUCCESS;
}

apr_status_t amqp_send_message (
    amqp_handle_t context,
    apr_byte_t out_of_band,
    apr_byte_t recovery,
    apr_byte_t streaming,
    const char* dest_name,
    apr_byte_t persistent,
    apr_byte_t priority,
    apr_uint32_t expiration,
    const char* mime_type,
    const char* encoding,
    const char* identifier,
    apr_size_t data_size,
    void *data,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_send_message (context, out_of_band, recovery,
        streaming, dest_name, persistent, priority, expiration, mime_type,
        encoding, identifier, data_size, data, async, &lock);
    TEST(result, handle_put_message, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_flow (
    amqp_handle_t context,
    apr_byte_t pause,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_flow (context, pause, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_cancel_subscription (
    amqp_handle_t context,
    const char *dest_name,
    const char *identifier,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_cancel (context, dest_name, identifier, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_unget_message (
    amqp_handle_t context,
    apr_uint32_t message_nbr, /* use message handle here ? */
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_unget (context, message_nbr, async, &lock);
    TEST(result, handle_flow, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_close_handle (
    amqp_handle_t context,
    apr_byte_t async
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = handle_terminate (context, &lock);
    TEST(result, handle_term, buffer)
    result = wait_for_lock (lock, NULL);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}

apr_status_t amqp_query (
    amqp_handle_t context,
    apr_uint32_t message_nbr,
    const char *dest_name,
    const char *mime_type,
    apr_byte_t partial,
    amqp_query_t *query
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;

    result = get_exclusive_access_to_query_dialogue (context);
    TEST(result, get_exclusive_access_to_query_dialogue, buffer);
    result = handle_create_query (context, message_nbr, dest_name, mime_type,
        &lock);
    TEST(result, handle_create_query, buffer)
    result = wait_for_lock (lock, (void**) query);
    TEST(result, wait_for_lock, buffer)

    return APR_SUCCESS;
}

apr_status_t amqp_get_query_result (
    amqp_query_t context,
    apr_uint32_t *message_nbr
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
 
    result = query_get_result (context, message_nbr);
    TEST(result, query_result, buffer)

    return APR_SUCCESS;
}

apr_status_t amqp_restart_query (
    amqp_query_t context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
 
    result = query_restart (context);
    TEST(result, query_result, buffer)

    return APR_SUCCESS;
}

apr_status_t amqp_close_query (
    amqp_query_t context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
 
    result = query_destroy (context);
    TEST(result, query_result, buffer)

    return APR_SUCCESS;    
}

apr_status_t amqp_browse (
    amqp_handle_t context,
    apr_uint32_t message_nbr,
    apr_byte_t async,
    amqp_frame_t **message
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_lock_t
        lock;
 
    result = handle_browse (context, message_nbr, message ? 1 : 0,
        async, &lock);
    TEST(result, handle_term, buffer)
    result = wait_for_lock (lock, (void**) message);
    TEST(result, wait_for_lock, buffer)
    
    return APR_SUCCESS;
}
