
#ifndef __AMQP_LEVEL1_H__
#define __AMQP_LEVEL1_H__

#include <base_apr.h>

/*  to be removed !!!  */
#include <amqp_level0.h>

typedef void* amqp_connection_t;
typedef void* amqp_channel_t;
typedef void* amqp_handle_t;
typedef void* amqp_stream_t;
typedef void* amqp_query_t;

typedef enum
{
    amqp_service_type_undefined = 0,
    amqp_service_type_queue = 1,
    amqp_service_type_topic = 2,
    amqp_service_type_peer = 3
} amqp_service_type_t;

/* not used yet */
typedef enum
{
    amqp_stream_type_undefined = 0,
    /*  Allows only read/write, no navigating through the stream             */
    amqp_stream_type_pipe = 1,
    /*  Allows to navigate through the stream in addition to read/write      */
    amqp_stream_type_full = 2
} amqp_stream_type_t;

/* not used yet */
typedef enum
{
    amqp_origin_beginning,
    amqp_origin_current,
    amqp_origin_end /* do we really want this one ? */
} amqp_origin_t;

/* not used yet */
typedef enum
{
   amqp_sync = 0,
   amqp_async = 1
} amqp_sync_t;

typedef enum
{
    /*  1. No heartbeats required from server                                */
    /*  2. No heartbeats send to server                                      */
    amqp_heartbeats_off,
    /*  1. Heartbeats are required every N seconds,                          */
    /*     if not received connection is closed                              */
    /*  2. Heatbeats are sent to server every N seconds                      */
    amqp_heartbeats_on,
    /*  1. Same as "on", but when heartbeat doesn't arrive in time,          */
    /*     requests explicitely for a heartbeat and closes connection only   */
    /*     in case it doesn't arrive                                         */
    /*  2. Has no distinct meaning for outgoing heartbeats, same as "on"     */
    amqp_heartbeats_active
} amqp_heartbeat_model_t;

/*  -------------------------------------------------------------------------
    Function: amqp_init1

    Synopsis:
    Initialises API module.
    -------------------------------------------------------------------------*/
apr_status_t amqp_init1 ();

/*  -------------------------------------------------------------------------
    Function: amqp_term1

    Synopsis:
    Deinitialises API module. If there are objects still opened attempts to
    shut them down gracefuly.
    -------------------------------------------------------------------------*/
apr_status_t amqp_term1 ();

/*  -------------------------------------------------------------------------
    Function: amqp_open_connection

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
apr_status_t amqp_open_connection (
    const char *server,
    const char *host,
    const char *client_name,
    amqp_heartbeat_model_t out_heartbeat_model,
    amqp_heartbeat_model_t in_heartbeat_model,
    apr_interval_time_t in_heartbeat_interval,
    apr_byte_t async,
    amqp_connection_t *connection
    );

/*  -------------------------------------------------------------------------
    Function: amqp_close_connection

    Synopsis:
    Closes connection to server.

    Arguments:
        connection          connection object to close
        async               if 1, doesn't wait till connection is physically
                            closed
    -------------------------------------------------------------------------*/
apr_status_t amqp_close_connection (
    amqp_connection_t connection,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_open_channel

    Synopsis:
    Opens channel.

    Arguments:
        connection          parent connection for the channel
        transacted          whether channel is to be transacted
        restartable         whether channel supports restartable messages
        async               if 1, don't wait for confirmation
        channel             output parameter; new channel object
    -------------------------------------------------------------------------*/
apr_status_t amqp_open_channel (
    amqp_connection_t connection,
    apr_byte_t transacted,
    apr_byte_t restartable,
    apr_byte_t async,
    amqp_channel_t *channel);

/*  -------------------------------------------------------------------------
    Function: amqp_acknowledge

    Synopsis:
    Acknowledges message(s).

    Arguments:
        channel             channel to do acknowledge on
        message_nbr         highest message number to be acknowledged
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_acknowledge (
    amqp_channel_t channel,
    apr_uint32_t message_nbr,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_commit

    Synopsis:
    Commits work done.

    Arguments:
        channel             channel to do commit on
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_commit (
    amqp_channel_t channel,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_rollback

    Synopsis:
    Rolls back work done.

    Arguments:
        channel             channel to do rollback on
        async               if 1, don't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_rollback (
    amqp_channel_t channel,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_close_channel

    Synopsis:
    Closes channel.

    Arguments:
        channel             channel object to close
        async               if 1, doesn't wait till channel is physically
                            closed
    -------------------------------------------------------------------------*/
apr_status_t amqp_close_channel (
    amqp_channel_t channel,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_open_handle

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
apr_status_t amqp_open_handle (
    amqp_channel_t channel,
    amqp_service_type_t service_type,
    apr_byte_t producer,
    apr_byte_t consumer,
    apr_byte_t browser,
    apr_byte_t temporary,
    char* dest_name,
    char* mime_type,
    char* encoding,
    apr_byte_t async,
    char **dest_name_out,
    amqp_handle_t *handle
    );

/*  -------------------------------------------------------------------------
    Function: amqp_consume

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
apr_status_t amqp_consume (
    amqp_handle_t handle,
    apr_uint16_t prefetch,
    apr_byte_t no_local,
    apr_byte_t unreliable,
    const char *dest_name,
    const char *identifier,
    const char *mime_type,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_receive_message

    Synopsis:
    Gets one message. If no message is available, this is a blocking call.

    Arguments:
        handle                handle object
        message               out parameter; message returned
    -------------------------------------------------------------------------*/
apr_status_t amqp_receive_message (
    amqp_handle_t handle,
    amqp_frame_t **message
    );

/*  -------------------------------------------------------------------------
    Function: amqp_destory_message

    Synopsis:
    Deallocates resources associated with message.

    Arguments:
        message              message to be destroyed
    -------------------------------------------------------------------------*/
apr_status_t amqp_destroy_message (
    amqp_frame_t *message
    );

/*  -------------------------------------------------------------------------
    Function: amqp_send_message

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
apr_status_t amqp_send_message (
    amqp_handle_t handle,
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
    );

/*  -------------------------------------------------------------------------
    Function: amqp_flow

    Synopsis:
    Suspends or restards message flow.

    Arguments:
        handle                handle object
        pause                 if 1, suspend, if 0, restart
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_flow (
    amqp_handle_t handle,
    apr_byte_t pause,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_cancel_subscription

    Synopsis:
    Cancels durable subscription

    Arguments:
        handle                handle object
        dest_name             destination name
        identifier            subscription name
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_cancel_subscription (
    amqp_handle_t handle,
    const char *dest_name,
    const char *identifier,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_unget

    Synopsis:
    Returns a message to server

    Arguments:
        handle                handle object
        message_nbr           number of message to return          
        async                 if 1, doesn't wait for confirmation
    -------------------------------------------------------------------------*/
apr_status_t amqp_unget_message (
    amqp_handle_t handle,
    apr_uint32_t message_nbr, /* use message handle here ? */
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_close_handle

    Synopsis:
    Closes handle.

    Arguments:
        handle              handle object
        async               if 1, doesn't wait till handle is physically
                            closed
    -------------------------------------------------------------------------*/
apr_status_t amqp_close_handle (
    amqp_handle_t handle,
    apr_byte_t async
    );

/*  -------------------------------------------------------------------------
    Function: amqp_query

    Synopsis:
    Issues a query to server.

    Arguments:
        handle              handle object
        message_nbr         request for messages with number over this value;
                            0 means all messages
        dest_name           destination name
        mime_type           MIME type
        partial             if 1 reads only single batch of results from server
        query               out parameter; returned resultset
    -------------------------------------------------------------------------*/
apr_status_t amqp_query (
    amqp_handle_t handle,
    apr_uint32_t message_nbr,
    const char *dest_name,
    const char *mime_type,
    apr_byte_t partial,
    amqp_query_t *query
    );

/*  -------------------------------------------------------------------------
    Function: amqp_get_query_result

    Synopsis:
    Gets next message number from query resultset.

    Arguments:
        query               query object
        message_nbr         output parameter; next message number
    -------------------------------------------------------------------------*/
apr_status_t amqp_get_query_result (
    amqp_query_t query,
    apr_uint32_t *message_nbr
    );

/*  -------------------------------------------------------------------------
    Function: amqp_restart_query

    Synopsis:
    Rewinds the resultset, so that amqp_get_query_result starts to return
    message numbers from beginning. No new query is performed.

    Arguments:
        query                query object
    -------------------------------------------------------------------------*/
apr_status_t amqp_restart_query (
    amqp_query_t query
    );

/*  -------------------------------------------------------------------------
    Function: amqp_close_query

    Synopsis:
    Closes channel.

    Arguments:
        channel             query object to close
    -------------------------------------------------------------------------*/
apr_status_t amqp_close_query (
    amqp_query_t query
    );

/*  -------------------------------------------------------------------------
    Function: amqp_browse

    Synopsis:
    Browses for message.

    Arguments:
        handle              handle object
        message_nbr         message to browse
        async               if 0, API waits for command confirmation
        message             out parameter; message returned; it set to NULL
                            message will be returned via standard
                            amqp_get_message function
    -------------------------------------------------------------------------*/
apr_status_t amqp_browse (
    amqp_handle_t handle,
    apr_uint32_t message_nbr,
    apr_byte_t async,
    amqp_frame_t **message
    );

/*apr_status_t amqp_stream_read (
    amqp_stream_t stream,
    void *destination,
    apr_size_t *size
    );

apr_status_t amqp_stream_write (
    amqp_stream_t stream,
    void *source,
    apr_size_t size
    );

apr_status_t amqp_stream_tell (
    amqp_stream_t stream,
    apr_uint32_t *offset
    );

apr_status_t amqp_stream_seek (
    amqp_stream_t stream,
    amqp_origin_t origin,
    apr_uint32_t offset
    );

apr_status_t amqp_stream_close (
    amqp_stream_t stream
    );*/

/*  Error handling
  maybe client should be able to register callback functions to be informed about
  connection/channel/handle closure as fast as possible
  and still: there's no other way how to propagate error values from these closures to client */

#endif
