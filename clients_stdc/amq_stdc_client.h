/*---------------------------------------------------------------------------
 *  amq_stdc_client.h - prototype for AMQ client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQ_STDC_CLIENT_H_INCLUDED
#define AMQ_STDC_CLIENT_H_INCLUDED

#include "base.h"
#include "base_apr.h"

#include "amq_stdc_error.h"
#include "amq_stdc_table.h"

/*---------------------------------------------------------------------------
 *  Public API prototype
 *---------------------------------------------------------------------------*/

typedef struct tag_connection_fsm_context_t* amq_stdc_connection_t;
typedef struct tag_channel_fsm_context_t*    amq_stdc_channel_t;
typedef struct tag_message_fsm_context_t*    amq_stdc_message_t;
typedef struct tag_inpipe_context_t*         amq_stdc_inpipe_t;
typedef struct tag_outpipe_context_t*        amq_stdc_outpipe_t;
typedef struct tag_stream_context_t*         amq_stdc_stream_t;

typedef struct
{
    qbyte
        message_nbr;
    byte
        delivered;
    byte
        redelivered;
    byte
        streaming;
    const char
        *dest_name;
    const char
        *mime_type;
    const char
        *encoding;
    const char
        *identifier;
    amq_stdc_table_t
        headers;
} amq_stdc_message_desc_t;

typedef enum
{
    amq_stdc_service_type_undefined = 0,
    amq_stdc_service_type_queue     = 1,
    amq_stdc_service_type_topic     = 2
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
    dbyte                       port,
    const char                  *host,
    const char                  *client_name,
    amq_stdc_heartbeat_model_t  out_heartbeat_model,
    amq_stdc_heartbeat_model_t  in_heartbeat_model,
    apr_interval_time_t         in_heartbeat_interval,
    dbyte                       options_size,
    const char                  *options,
    byte                        async,
    amq_stdc_connection_t       *connection
    );

apr_status_t amq_stdc_close_connection (
    amq_stdc_connection_t  connection
    );

apr_status_t amq_stdc_open_channel (
    amq_stdc_connection_t  connection,
    byte                   transacted,
    byte                   restartable,
    dbyte                  options_size,
    const char             *options,
    const char             *out_of_band,
    byte                   async,
    amq_stdc_channel_t     *channel
    );

apr_status_t amq_stdc_open_handle (
    amq_stdc_channel_t       channel,
    amq_stdc_service_type_t  service_type,
    byte                     producer,
    byte                     consumer,
    byte                     browser,
    const char               *mime_type,
    const char               *encoding,
    dbyte                    options_size,
    const char               *options,
    byte                     async,
    dbyte                    *handle_id
    );

apr_status_t amq_stdc_close_handle (
    amq_stdc_channel_t  channel,
    dbyte               handle_id
    );

apr_status_t amq_stdc_acknowledge (
    amq_stdc_channel_t  channel,
    qbyte               message_nbr,
    byte                async
    );

apr_status_t amq_stdc_commit (
    amq_stdc_channel_t  channel,
    dbyte               options_size,
    const char          *options,
    byte                async
    );

apr_status_t amq_stdc_rollback (
    amq_stdc_channel_t  channel,
    dbyte               options_size,
    const char          *options,
    byte                async
    );

apr_status_t amq_stdc_send_message (
    amq_stdc_channel_t  channel,
    dbyte               handle_id,
    byte                out_of_band,
    byte                recovery,
    const char          *dest_name,
    byte                persistent,
    byte                immediate,
    byte                priority,
    qbyte               expiration,
    const char          *mime_type,
    const char          *encoding,
    const char          *identifier,
    dbyte               headers_size,
    const char          *headers,
    apr_size_t          data_size,
    void                *data,
    byte                async
    );

apr_status_t amq_stdc_consume (
    amq_stdc_channel_t  channel,
    dbyte               handle_id,
    dbyte               prefetch,
    byte                no_local,
    byte                no_ack,
    byte                dynamic,
    const char          *dest_name,
    dbyte               selector_size,
    const char          *selector,
    byte                async,
    char                **dest_name_out
    );

apr_status_t amq_stdc_get_message (
    amq_stdc_channel_t       channel,
    byte                     wait,
    amq_stdc_message_desc_t  **message_desc,
    amq_stdc_message_t       *message
    );

apr_status_t amq_stdc_browse (
    amq_stdc_channel_t       channel,
    dbyte                    handle_id,
    qbyte                    message_nbr,
    byte                     async,
    amq_stdc_message_desc_t  **message_desc,
    amq_stdc_message_t       *message
    );

apr_status_t amq_stdc_flow (
    amq_stdc_channel_t  channel,
    dbyte               handle_id,
    byte                pause,
    byte                async
    );

apr_status_t amq_stdc_unget_message (
    amq_stdc_channel_t  message,
    dbyte               handle_id,
    qbyte               message_nbr,
    byte                async
    );

apr_status_t amq_stdc_query (
    amq_stdc_channel_t  channel,
    dbyte               handle_id,
    qbyte               message_nbr,
    const char          *dest_name,
    const char          *selector,
    const char          *mime_type,
    byte                partial,
    char                **resultset
    );

apr_status_t amq_stdc_close_channel (
    amq_stdc_channel_t  channel
    );

/*---------------------------------------------------------------------------*/

apr_status_t amq_stdc_destroy_query (
    char  *query
    );

/*---------------------------------------------------------------------------*/

size_t amq_stdc_open_inpipe (
    amq_stdc_message_t  message,
    amq_stdc_inpipe_t   *inpipe
    );

size_t amq_stdc_open_outpipe (
    amq_stdc_message_t  message,
    amq_stdc_outpipe_t   *outpipe
    );

size_t amq_stdc_open_stream (
    amq_stdc_message_t  message,
    amq_stdc_stream_t   *stream
    );

apr_status_t amq_stdc_close_message (
    amq_stdc_message_t  message,
    byte                async
    );

/*---------------------------------------------------------------------------*/

size_t amq_stdc_pread (
    amq_stdc_inpipe_t   inpipe,
    void                *destination,
    size_t              size,
    byte                wait,
    byte                complete
    );

size_t amq_stdc_pskip (
    amq_stdc_inpipe_t   inpipe,
    size_t              size,
    byte                wait,
    byte                complete
    );

byte amq_stdc_peof (
    amq_stdc_inpipe_t  inpipe
    );

/*---------------------------------------------------------------------------*/

#endif
