/*---------------------------------------------------------------------------
 *  amq_stdc_client.h - prototype for AMQ client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_CLIENT_H_INCLUDED
#define AMQ_STDC_CLIENT_H_INCLUDED

#include <base_apr.h>

/*  TODO: to be removed !!!  */
#include "amq_stdc_framing.h"

/*---------------------------------------------------------------------------
 *  Public API prototype
 *---------------------------------------------------------------------------*/

typedef struct tag_connection_fsm_context_t* amq_stdc_connection_t;
typedef struct tag_channel_fsm_context_t*    amq_stdc_channel_t;
typedef struct tag_handle_fsm_context_t*     amq_stdc_handle_t;

typedef enum
{
    amq_stdc_service_type_undefined = 0,
    amq_stdc_service_type_queue     = 1,
    amq_stdc_service_type_topic     = 2,
    amq_stdc_service_type_peer      = 3
} amq_stdc_service_type_t;

/*  Possible models of dealing with heartbeats                               */
/*  "1." comment describes incomming heartbeats policy                       */
/*  "2." comment describes outgoing heartbeats policy                        */
typedef enum
{
    /*  1. No heartbeats required from server                                */
    /*  2. No heartbeats send to server                                      */
    amq_stdc_heartbeats_off,

    /*  1. Heartbeats are required every N seconds,                          */
    /*     if not received connection is closed                              */
    /*  2. Heatbeats are sent to server every N seconds                      */
    amq_stdc_heartbeats_on,

    /*  1. Same as "on", but when heartbeat doesn't arrive in time,          */
    /*     requests explicitely for a heartbeat and closes connection only   */
    /*     in case it doesn't arrive                                         */
    /*  2. Has no distinct meaning for outgoing heartbeats, same as "on"     */
    amq_stdc_heartbeats_active
} amq_stdc_heartbeat_model_t;

apr_status_t amq_stdc_init ();

apr_status_t amq_stdc_term ();

apr_status_t amq_stdc_open_connection (
    const char                  *server,
    const char                  *host,
    const char                  *client_name,
    amq_stdc_heartbeat_model_t  out_heartbeat_model,
    amq_stdc_heartbeat_model_t  in_heartbeat_model,
    apr_interval_time_t         in_heartbeat_interval,
    apr_byte_t                  async,
    amq_stdc_connection_t       *connection
    );

apr_status_t amq_stdc_close_connection (
    amq_stdc_connection_t  connection
    );

apr_status_t amq_stdc_open_channel (
    amq_stdc_connection_t  connection,
    apr_byte_t         transacted,
    apr_byte_t         restartable,
    apr_byte_t         async,
    amq_stdc_channel_t     *channel);

apr_status_t amq_stdc_acknowledge (
    amq_stdc_channel_t  channel,
    apr_uint32_t    message_nbr,
    apr_byte_t      async
    );

apr_status_t amq_stdc_commit (
    amq_stdc_channel_t  channel,
    apr_byte_t      async
    );

apr_status_t amq_stdc_rollback (
    amq_stdc_channel_t  channel,
    apr_byte_t      async
    );

apr_status_t amq_stdc_close_channel (
    amq_stdc_channel_t  channel
    );

apr_status_t amq_stdc_open_handle (
    amq_stdc_channel_t       channel,
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
    amq_stdc_handle_t        *handle
    );

apr_status_t amq_stdc_consume (
    amq_stdc_handle_t  handle,
    apr_uint16_t   prefetch,
    apr_byte_t     no_local,
    apr_byte_t     unreliable,
    const char     *dest_name,
    const char     *identifier,
    const char     *mime_type,
    apr_byte_t     async
    );

apr_status_t amq_stdc_get_message (
    amq_stdc_handle_t  handle,
    amqp_frame_t   **message
    );

apr_status_t amq_stdc_destroy_message (
    amqp_frame_t  *message
    );

apr_status_t amq_stdc_send_message (
    amq_stdc_handle_t  handle,
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
    );

apr_status_t amq_stdc_flow (
    amq_stdc_handle_t  handle,
    apr_byte_t     pause,
    apr_byte_t     async
    );

apr_status_t amq_stdc_cancel_subscription (
    amq_stdc_handle_t  handle,
    const char     *dest_name,
    const char     *identifier,
    apr_byte_t     async
    );

apr_status_t amq_stdc_unget_message (
    amq_stdc_handle_t  handle,
    apr_uint32_t   message_nbr,
    apr_byte_t     async
    );

apr_status_t amq_stdc_close_handle (
    amq_stdc_handle_t  handle
    );

apr_status_t amq_stdc_query (
    amq_stdc_handle_t  handle,
    apr_uint32_t   message_nbr,
    const char     *dest_name,
    const char     *mime_type,
    apr_byte_t     partial,
    char           **resultset
    );

apr_status_t amq_stdc_destroy_query (
    char  *query
    );

apr_status_t amq_stdc_browse (
    amq_stdc_handle_t  handle,
    apr_uint32_t   message_nbr,
    apr_byte_t     async,
    amqp_frame_t   **message
    );

#endif
