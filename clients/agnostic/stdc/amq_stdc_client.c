/*---------------------------------------------------------------------------
 *  amq_stdc_client.c - implementation of AMQ client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_private.h"
#include "amq_stdc_client.h"
#include "amq_stdc_global_fsm.h"

static global_fsm_t
    global;

/*  -------------------------------------------------------------------------
    Function: amq_stdc_init

    Synopsis:
    Initialises API module.
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_init ()
{
    apr_status_t
        result;

    result = global_fsm_create (&global);
    AMQ_ASSERT_STATUS (result, global_fsm_create)
    result = global_fsm_init (global);
    AMQ_ASSERT_STATUS (result, global_fsm_init)
 
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_term

    Synopsis:
    Deinitialises API module. If there are objects still opened attempts to
    shut them down gracefuly.
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_term ()
{
    apr_status_t
        result;
    lock_t
        lock;

    result = global_fsm_terminate (global, &lock);
    AMQ_ASSERT_STATUS (result, global_fsm_terminate)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    amq_stats ();
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_open_connection

    Synopsis:
    Creates new connection object and opens a connection to server.

    Arguments:
        server                server to connect to
        host                  virtual host to connect to
        client_name           client name to use when connecting
        out_heartbeat_model   specifies how heartbeats are to be sent to server
        in_heartbeat_mode     specifies how heartbeats are to be received
                              from server
        in_heartbeat_interval interval in which client wants server to send
                              heartbeats  
        async                 if 1, doesn't wait for confirmation
        connection            out parameter; new connection object
    -------------------------------------------------------------------------*/
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
    lock_t
        lock;

    result = global_fsm_create_connection (global, server, host, client_name,
        async, &lock);
    AMQ_ASSERT_STATUS (result, global_fsm_create_connection);
    result = wait_for_lock (lock, (void**) out);
    AMQ_ASSERT_STATUS (result, wait_for_lock);

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_close_connection

    Synopsis:
    Closes connection to server.

    Arguments:
        connection          connection object to close
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_close_connection (
    amq_stdc_connection_t  context
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = connection_fsm_terminate (context, &lock);
    AMQ_ASSERT_STATUS (result, connection_fsm_terminate)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    result = connection_fsm_destroy (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_destroy)
 
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_open_channel

    Synopsis:
    Opens channel.

    Arguments:
        connection          parent connection for the channel
        transacted          whether channel is to be transacted
        restartable         whether channel supports restartable messages
        async               if 1, don't wait for confirmation
        channel             output parameter; new channel object
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_open_channel (
    amq_stdc_connection_t  context,
    apr_byte_t             transacted,
    apr_byte_t             restartable,
    apr_byte_t             async,
    amq_stdc_channel_t     *out)
{
    apr_status_t
        result;
    lock_t
        lock;

    result = connection_fsm_create_channel (context, transacted, restartable,
        async, &lock);
    AMQ_ASSERT_STATUS (result, connection_fsm_create_channel);
    result = wait_for_lock (lock, (void**) out);
    AMQ_ASSERT_STATUS (result, wait_for_lock);

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_acknowledge

    Synopsis:
    Acknowledges message(s).

    Arguments:
        channel             channel to do acknowledge on
        message_nbr         highest message number to be acknowledged
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_acknowledge (
    amq_stdc_channel_t  context,
    apr_uint32_t        message_nbr,
    apr_byte_t          async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = channel_fsm_acknowledge (context, message_nbr, async, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_acknowledge)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_commit

    Synopsis:
    Commits work done.

    Arguments:
        channel             channel to do commit on
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_commit (
    amq_stdc_channel_t  context,
    apr_byte_t          async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = channel_fsm_commit (context, async, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_commit)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_rollback

    Synopsis:
    Rolls back work done.

    Arguments:
        channel             channel to do rollback on
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_rollback (
    amq_stdc_channel_t  context,
    apr_byte_t          async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = channel_fsm_rollback (context, async, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_rollback)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_close_channel

    Synopsis:
    Closes channel.

    Arguments:
        channel             channel object to close
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_close_channel (
    amq_stdc_channel_t  context
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = channel_fsm_terminate (context, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_terminate)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    result = channel_fsm_destroy (context);
    AMQ_ASSERT_STATUS (result, channel_fsm_destroy)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_open_handle

    Synopsis:
    Opens handle.

    Arguments:
        channel               parent channel for the handle
        service_type          queue/topic/peer
        producer              if 1, client is allowed to send messages
        consumer              if 1, client is allowed to receive messages
        browser               if 1, client is allowed to browse for messages
        temporary             if 1, temporary destination will be created
        desn_name             destination name
        mime_type             MIME type
        encoding              content encoding
        async                 if 1, doesn't wait for confirmation
        dest_name_out         out parameter; name of newly created temporary
                              destination; filled only when temporary = 1
        handle                out parameter; new handle object
    -------------------------------------------------------------------------*/
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
    lock_t
        lock;
    lock_t
        created_lock;

    result = channel_fsm_create_handle (context, service_type, producer,
        consumer, browser, temporary, dest_name, mime_type, encoding, async,
        &created_lock, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_create_handle);

    /*  Wait for HANDLE CREATED                                              */
    result = wait_for_lock (created_lock, (void**) dest_name_out);
    AMQ_ASSERT_STATUS (result, wait_for_lock);

    /*  Wait for confirmation                                                */
    result = wait_for_lock (lock, (void**) out);
    AMQ_ASSERT_STATUS (result, wait_for_lock);

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_consume

    Synopsis:
    Starts message consumption.

    Arguments:
        handle                handle object
        prefetch              number of messages to prefetch
        no_local              if 1, messages sent from this connection won't be
                              received
        unreliable            if 1, client won't acknowledge messages
        dest_name             destination name
        idnetifier            durable subscription name
        mime_type             MIME type
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_consume (
    amq_stdc_handle_t  context,
    apr_uint16_t       prefetch,
    apr_byte_t         no_local,
    apr_byte_t         unreliable,
    const char         *dest_name,
    const char         *identifier,
    const char         *mime_type,
    apr_byte_t         async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_consume (context, prefetch, no_local, unreliable,
        dest_name, identifier, mime_type, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_consume)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_receive_message

    Synopsis:
    Gets one message. If no message is available, this is a blocking call.

    Arguments:
        handle                handle object
        message               out parameter; message returned
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_get_message (
    amq_stdc_handle_t  handle,
    amqp_frame_t       **message
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_get_message (handle, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_get_message)
    result = wait_for_lock (lock, (void**) message);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_destory_message

    Synopsis:
    Deallocates resources associated with message.

    Arguments:
        message              message to be destroyed
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_destroy_message (
    amqp_frame_t  *message
    )
{
    amq_free ((void*) message);
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_send_message

    Synopsis:
    Sends message to server.

    Arguments:
        handle                handle object
        out_of_band           if 1, message data are to be transferred
                              out of band
        recovery              if 1, this is a recovery - only a partial message
        streaming             if 1, it is a stream
        dest_name             destination name
        persistent            if 1, message is going to be persistent
        priority              priority of message
        expiration            expiration time of message
        mime_type             MIME type
        encoding              content encoding
        identifier            durable subscription name        
        data_size             number of bytes to send
        data                  position from which to read data
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_send_message (
    amq_stdc_handle_t  context,
    apr_byte_t         out_of_band,
    apr_byte_t         recovery,
    apr_byte_t         streaming,
    const char         *dest_name,
    apr_byte_t         persistent,
    apr_byte_t         priority,
    apr_uint32_t       expiration,
    const char         *mime_type,
    const char         *encoding,
    const char         *identifier,
    apr_size_t         data_size,
    void               *data,
    apr_byte_t         async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_send_message (context, out_of_band, recovery,
        streaming, dest_name, persistent, priority, expiration, mime_type,
        encoding, identifier, data_size, data, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_send_message)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_flow

    Synopsis:
    Suspends or restards message flow.

    Arguments:
        handle                handle object
        pause                 if 1, suspend, if 0, restart
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_flow (
    amq_stdc_handle_t  context,
    apr_byte_t         pause,
    apr_byte_t         async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_flow (context, pause, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_flow)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_cancel_subscription

    Synopsis:
    Cancels durable subscription

    Arguments:
        handle                handle object
        dest_name             destination name
        identifier            subscription name
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_cancel_subscription (
    amq_stdc_handle_t  context,
    const char         *dest_name,
    const char         *identifier,
    apr_byte_t         async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_cancel (context, dest_name, identifier, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_cancel)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_unget_message

    Synopsis:
    Returns a message to server

    Arguments:
        handle                handle object
        message_nbr           number of message to return          
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_unget_message (
    amq_stdc_handle_t  context,
    apr_uint32_t       message_nbr,
    apr_byte_t         async
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_unget (context, message_nbr, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_unget)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_close_handle

    Synopsis:
    Closes handle.

    Arguments:
        handle              handle object
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_close_handle (
    amq_stdc_handle_t  context
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = handle_fsm_terminate (context, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_terminate)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    result = handle_fsm_destroy (context);
    AMQ_ASSERT_STATUS (result, handle_fsm_destroy)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_query

    Synopsis:
    Issues a query to server.

    Arguments:
        handle              handle object
        message_nbr         request for messages with number over this value;
                            0 means all messages
        dest_name           destination name
        mime_type           MIME type
        partial             if 1 reads only single batch of results from server
        resultset           out parameter; returned resultset
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_query (
    amq_stdc_handle_t  context,
    apr_uint32_t       message_nbr,
    const char         *dest_name,
    const char         *mime_type,
    apr_byte_t         partial,
    char               **resultset
    )
{
    apr_status_t
        result;
    lock_t
        lock;

    result = get_exclusive_access_to_query_dialogue (context);
    AMQ_ASSERT_STATUS (result, get_exclusive_access_to_query_dialogue);
    result = handle_fsm_query (context, message_nbr, dest_name, mime_type,
        &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_query)
    result = wait_for_lock (lock, (void**) resultset);
    AMQ_ASSERT_STATUS (result, wait_for_lock)

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_destroy_query

    Synopsis:
    Deallocates resources associated with query result.

    Arguments:
        query                query resultset to destroy
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_destroy_query (
    char  *query
    )
{
    amq_free ((void*) query);
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_browse

    Synopsis:
    Browses for message.

    Arguments:
        handle              handle object
        message_nbr         message to browse
        async               if 0, API waits for command confirmation
        message             out parameter; message returned; it set to NULL
                            message will be returned via standard
                            amq_stdc_get_message function
    -------------------------------------------------------------------------*/
apr_status_t amq_stdc_browse (
    amq_stdc_handle_t  context,
    apr_uint32_t       message_nbr,
    apr_byte_t         async,
    amqp_frame_t       **message
    )
{
    apr_status_t
        result;
    lock_t
        lock;
 
    result = handle_fsm_browse (context, message_nbr, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_browse)
    result = wait_for_lock (lock, (void**) message);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}
