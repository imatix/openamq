
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

typedef enum
{
    amqp_stream_type_undefined = 0,
    /*  Allows only read/write, no navigating through the stream             */
    amqp_stream_type_pipe = 1,
    /*  Allows to navigate through the stream in addition to read/write      */
    amqp_stream_type_full = 2
} amqp_stream_type_t;

typedef enum
{
    amqp_origin_beginning,
    amqp_origin_current,
    amqp_origin_end /* do we really want this one ? */
} amqp_origin_t;

/* ? */
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

apr_status_t amqp_init1 ();
apr_status_t amqp_term1 ();

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

apr_status_t amqp_close_connection (
    amqp_connection_t connection,
    apr_byte_t async
    );

apr_status_t amqp_open_channel (
    amqp_connection_t connection,
    apr_byte_t transacted,
    apr_byte_t restartable,
    apr_byte_t async,
    amqp_channel_t *channel);

apr_status_t amqp_acknowledge (
    amqp_channel_t channel,
    apr_uint32_t message_nbr,
    apr_byte_t async
    );

apr_status_t amqp_commit (
    amqp_channel_t channel,
    apr_byte_t async
    );

apr_status_t amqp_rollback (
    amqp_channel_t channel,
    apr_byte_t async
    );

apr_status_t amqp_close_channel (
    amqp_channel_t channel,
    apr_byte_t async
    );

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

apr_status_t amqp_receive_message (
    amqp_handle_t handle,
    amqp_frame_t **message
    );

apr_status_t amqp_destroy_message (
    amqp_frame_t *message
    );

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

apr_status_t amqp_flow (
    amqp_handle_t handle,
    apr_byte_t pause,
    apr_byte_t async
    );

apr_status_t amqp_cancel_subscription (
    amqp_handle_t handle,
    const char *dest_name,
    const char *identifier,
    apr_byte_t async
    );

apr_status_t amqp_unget_message (
    amqp_handle_t handle,
    apr_uint32_t message_nbr, /* use message handle here ? */
    apr_byte_t async
    );

apr_status_t amqp_close_handle (
    amqp_handle_t handle,
    apr_byte_t async
    );

apr_status_t amqp_query (
    amqp_handle_t handle,
    apr_uint32_t message_nbr,
    const char *dest_name,
    const char *mime_type,
    apr_byte_t partial,
    amqp_query_t *query
    );

apr_status_t amqp_get_query_result (
    amqp_query_t query,
    apr_uint32_t *message_nbr
    );

apr_status_t amqp_restart_query (
    amqp_query_t query
    );

apr_status_t amqp_close_query (
    amqp_query_t query
    );

apr_status_t amqp_browse (
    amqp_handle_t handle,
    apr_uint32_t message_nbr,
    /*  If 0, API waits for command confirmation                             */
    apr_byte_t async,
    /*  If NULL, message is returned via standard amqp_get_message function  */
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

apr_status_t amqp_streem_tell (
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
