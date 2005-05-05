/*---------------------------------------------------------------------------
 *  amq_stdc_client.c - implementation of AMQ client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 *  DESIGN SUMMARY
 *
 *  Implementation of AMQ client API is composed of several finite state
 *  machines (stateful, of course) plus a lightweight wrapper (stateless)
 *  that converts client requests as well as commands sent by server to state
 *  machine events. Wrapper converting client requests resides in
 *  amq_stdc_client.c, coversion of server commands is done by
 *  dispatcher thread (see s_receiver_thread in amq_stdc_connection_fsm.c).
 *  Thread synchronisation is done on state machine level: any state machine
 *  may be accessed only by a single thread at any given moment. Thus it is
 *  important for threads to spent as little time as possible within the
 *  state machine, not to block other threads that may require access to the
 *  same state machine. This means no blocking calls should be done within
 *  state machines. There are three types of blocking calls to consider.
 *  First we have reading from socket. This is done within dispatcher thread
 *  and only subsequently is received command converted to event and sent to
 *  the apropriate state machine. Next there is writing to socket. It may be
 *  blocking when socket buffer is full. Special sender thread would be an
 *  universal solution. (Not implemented yet.) Third type of blocking is when
 *  command is sent to server and response from server is required to
 *  continue. This problem is solved using concept of locks. Wrapper layer
 *  issues event which is processed on spot (command is sent to server) and
 *  lock is returned. Wrapper layer may wait for the lock, that is released
 *  once response from server is received. Reply data are passed at the same
 *  time from thread unlocking the lock to the thread waiting for the lock.
 *  Implementation of locks may be found in amq_stads_global_fsm.c.
 *  There are four state machines altogether within client API: global state
 *  machine (holds state common to the whole of API), connection state machine
 *  (holds state associated with connection), channel state machine (holds
 *  state associated with channel) and handle state machine (holds state
 *  associated with handle). There may be several connection FSM instances
 *  existing at the same time, several channel FSM instaces per connection and
 *  handle FSM instances per channel.
 *  State machine definitions are in amq_stdc_fsms.xml. Processing this file
 *  using amq_stdc_fsms.gsl generates <fsm-name>.i and <fsm-name>.d files for
 *  every state machine, which are in turn included into <fsm-name>.h and
 *  <fsm-name>.c. <fsm-name>.c contains implementation of actions of
 *  corresponding state machine.
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
    amq_stdc_lock_t
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
        port                  port to use
        host                  virtual host to connect to
        client_name           client name to use when connecting
        out_heartbeat_model   specifies how heartbeats are to be sent to server
        in_heartbeat_mode     specifies how heartbeats are to be received
                              from server
        in_heartbeat_interval interval in which client wants server to send
                              heartbeats
        options               options table passed to CONNECTION OPEN command  
        async                 if 1, doesn't wait for confirmation
        connection            out parameter; new connection object
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_open_connection (
    const char                  *server,
    dbyte                       port,
    const char                  *host,
    const char                  *client_name,
    amq_stdc_heartbeat_model_t  out_heartbeat_model,
    amq_stdc_heartbeat_model_t  in_heartbeat_model,
    apr_interval_time_t         in_heartbeat_interval,
    amq_stdc_table_t            options,
    byte                        async,
    amq_stdc_connection_t       *out
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = global_fsm_create_connection (global, server, port, host,
        client_name, options, async, out, &lock);
    AMQ_ASSERT_STATUS (result, global_fsm_create_connection);
    result = wait_for_lock (lock, NULL);
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
    amq_stdc_lock_t
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
        options             options table to be passed to CHANNEL OPEN
        out_of_band         specifies how out of band transfer should work
        async               if 1, don't wait for confirmation
        channel             output parameter; new channel object
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_open_channel (
    amq_stdc_connection_t  context,
    byte                   transacted,
    byte                   restartable,
    amq_stdc_table_t       options,
    const char             *out_of_band,
    byte                   async,
    amq_stdc_channel_t     *out)
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = connection_fsm_create_channel (context, transacted, restartable,
        options, out_of_band, async, out, &lock);
    AMQ_ASSERT_STATUS (result, connection_fsm_create_channel);
    result = wait_for_lock (lock, NULL);
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
    qbyte               message_nbr,
    byte                async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
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
        options             options table to be passed to CHANNEL COMMIT
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_commit (
    amq_stdc_channel_t  context,
    amq_stdc_table_t    options,
    byte                async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = channel_fsm_commit (context, options, async, &lock);
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
        options             options table to be passed to CHANNEL ROLLBACK
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_rollback (
    amq_stdc_channel_t  context,
    amq_stdc_table_t    options,
    byte                async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = channel_fsm_rollback (context, options, async, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_rollback)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: amq_stdc_get_message
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_get_message (
    amq_stdc_channel_t       channel,
    byte                     wait,
    amq_stdc_message_desc_t  **message_desc,
    amq_stdc_message_t       *message
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;
    void
        *msg;

    result = channel_fsm_get_message (channel, wait, message_desc, message,
        &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_get_message)
    if (wait) {
        result = wait_for_lock (lock, &msg);
        AMQ_ASSERT_STATUS (result, wait_for_lock)
    }

    if (message)
        *message = *((amq_stdc_message_t*)
            (msg + sizeof (amq_stdc_message_desc_t)));
    if (message_desc)
        *message_desc = (amq_stdc_message_desc_t*) msg;

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
    amq_stdc_lock_t
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
        dest_name             destination name
        mime_type             MIME type
        encoding              content encoding
        options               options table passed to HANDLE OPEN command
        async                 if 1, doesn't wait for confirmation
        dest_name_out         out parameter; name of newly created temporary
                              destination; filled only when temporary = 1
        handle                out parameter; new handle object
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_open_handle (
    amq_stdc_channel_t       context,
    amq_stdc_service_type_t  service_type,
    byte                     producer,
    byte                     consumer,
    byte                     browser,
    byte                     temporary,
    char                     *dest_name,
    char                     *mime_type,
    char                     *encoding,
    amq_stdc_table_t         options,
    byte                     async,
    char                     **dest_name_out,
    amq_stdc_handle_t        *out
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;
    amq_stdc_lock_t
        created_lock;

    result = channel_fsm_create_handle (context, service_type, producer,
        consumer, browser, temporary, dest_name, mime_type, encoding, options,
        async, out, &created_lock, &lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_create_handle);

    /*  Wait for HANDLE CREATED                                              */
    result = wait_for_lock (created_lock, (void**) dest_name_out);
    AMQ_ASSERT_STATUS (result, wait_for_lock);

    /*  Wait for confirmation                                                */
    result = wait_for_lock (lock, NULL);
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
        selector              selector string
        mime_type             MIME type
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_consume (
    amq_stdc_handle_t  context,
    dbyte              prefetch,
    byte               no_local,
    byte               unreliable,
    const char         *dest_name,
    const char         *identifier,
    const char         *selector,
    const char         *mime_type,
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = handle_fsm_consume (context, prefetch, no_local, unreliable,
        dest_name, identifier, selector, mime_type, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_consume)
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
    
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
    byte               out_of_band,
    byte               recovery,
    byte               streaming,
    const char         *dest_name,
    byte               persistent,
    byte               priority,
    qbyte              expiration,
    const char         *mime_type,
    const char         *encoding,
    const char         *identifier,
    apr_size_t         data_size,
    void               *data,
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
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
    byte               pause,
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
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
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
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
    qbyte              message_nbr,
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
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
    amq_stdc_lock_t
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
        selector            selector string
        mime_type           MIME type
        partial             if 1 reads only single batch of results from server
        resultset           out parameter; returned resultset
    -------------------------------------------------------------------------*/

apr_status_t amq_stdc_query (
    amq_stdc_handle_t  context,
    qbyte              message_nbr,
    const char         *dest_name,
    const char         *selector,
    const char         *mime_type,
    byte               partial,
    char               **resultset
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;

    result = get_exclusive_access_to_query_dialogue (context);
    AMQ_ASSERT_STATUS (result, get_exclusive_access_to_query_dialogue);
    result = handle_fsm_query (context, message_nbr, dest_name, selector,
        mime_type, &lock);
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
    qbyte              message_nbr,
    byte               async
    )
{
    apr_status_t
        result;
    amq_stdc_lock_t
        lock;
 
    result = handle_fsm_browse (context, message_nbr, async, &lock);
    AMQ_ASSERT_STATUS (result, handle_fsm_browse)
    /*  TODO: Message should be returned here                                */
    /*  If NULL is returned, message is not found, error                     */
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)
   
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------*/





size_t amq_stdc_read (
    amq_stdc_message_t message,
    void *destination,
    size_t size
    )
{
    apr_status_t
        result;
    qbyte
        out_size;

    result = message_fsm_read (message, destination, size, &out_size);
    AMQ_ASSERT_STATUS (result, message_fsm_read)

    return out_size;
}

size_t amq_stdc_skip (
    amq_stdc_message_t message,
    size_t size
    )
{
    apr_status_t
        result;
    qbyte
        out_size;

    result = message_fsm_read (message, NULL, size, &out_size);
    AMQ_ASSERT_STATUS (result, message_fsm_read)

    return out_size;
}

apr_status_t amq_stdc_close_message (
    amq_stdc_message_t  message,
    byte                async
    )
{
    apr_status_t
        result;

    result = message_fsm_terminate (message);
    AMQ_ASSERT_STATUS (result, message_fsm_read)

    return APR_SUCCESS;
}
