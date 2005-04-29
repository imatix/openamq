/*---------------------------------------------------------------------------
 *  amq_stdc_handle.c - Definition of HANDLE object and QUERY object
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_handle_fsm.h"
#include "amq_stdc_handle_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_browse_list_item_t
{
    qbyte
        message_nbr;
    dbyte
        lock_id;
    struct tag_browse_list_item_t
        *next;
} browse_list_item_t;

#define HANDLE_FSM_OBJECT_ID context->id

DEFINE_HANDLE_FSM_CONTEXT_BEGIN
    dbyte
        id;                         /*  Handle id                            */
    global_fsm_t
        global;                     /*  Global object handle                 */
    connection_fsm_t
        connection;                 /*  Connection handle belongs to         */
    channel_fsm_t
        channel;                    /*  Channel handle belongs to            */
    dbyte
        connection_id;              /*  Id of connection handle belongs to   */
    dbyte
        channel_id;                 /*  Id of channel handle belongs to      */
    dbyte
        created_tag;                /*  Id of lock that's used for waiting   */
                                    /*  for HANDLE CREATED                   */
    dbyte
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for HANDLE CLOSE                     */
    dbyte
        index_tag;                  /*  Id of lock that's used for waiting   */
                                    /*  for HANDLE INDEX                     */
    apr_thread_mutex_t
        *query_sync;                /*  Critical section allowing only       */
                                    /*  single thread to perform             */
                                    /*  HANDLE QUERY at the same time        */ 
    char
        *dest_name;                 /*  Name of temporary destination        */
    browse_list_item_t
        *browse_requests;           /*  Linked list of requests for          */
                                    /*  synchronous HANDLE BROWSEs           */
DEFINE_HANDLE_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    handle_fsm_context_t *context
    )
{
    apr_status_t
        result;

    context->browse_requests = NULL;
    result = apr_thread_mutex_create (&(context->query_sync),
        APR_THREAD_MUTEX_UNNESTED, context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_create)
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    handle_fsm_context_t *context
    )
{
    apr_status_t
        result;

    /*  Remove handle from channel's list                                    */
    result = channel_fsm_remove_handle (context->channel, context);
    AMQ_ASSERT_STATUS (result, channel_fsm_remove_handle)
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: get_exclusive_access_to_query_dialogue

    Synopsis:
    Waits till currently running query is ended, then suspends all other
    threads that may be requesting to do a query.

    Arguments:
        context              handle object
    -------------------------------------------------------------------------*/

apr_status_t get_exclusive_access_to_query_dialogue (
    handle_fsm_t  context
    )
{
    apr_status_t
        result;

    /*  Wait for exclusive access to query/index dialogue                    */
    result = apr_thread_mutex_lock (context->query_sync);
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_lock);
#   ifdef AMQTRACE_LOCKS
        printf ("# Exclusive access to QUERY/INDEX dialogue gained\n");
#   endif
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions  (for documentation see amq_stdc_fsms.xml)
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    handle_fsm_context_t  *context,
    global_fsm_t          global,
    connection_fsm_t      connection,
    channel_fsm_t         channel,
    dbyte                 connection_id,
    dbyte                 channel_id,
    dbyte                 handle_id,
    dbyte                 service_type,
    byte                  producer,
    byte                  consumer,
    byte                  browser,
    const char            *dest_name,
    const char            *mime_type,
    const char            *encoding,
    amq_stdc_table_t      options,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result; 
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    byte
        dest_name_size = strlen (dest_name);
    byte
        mime_type_size = strlen (mime_type);
    byte
        encoding_size = strlen (encoding);

    /*  Save values that will be needed in future                            */
    context->global = global;
    context->connection = connection;
    context->channel = channel;
    context->connection_id = connection_id;
    context->channel_id = channel_id;
    context->id = handle_id;

    /*  Register that we will be waiting for handle open completion          */
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            channel_id, handle_id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE OPEN                                                     */    
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_OPEN_CONSTANT_SIZE +
        dest_name_size + mime_type_size + encoding_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_open (chunk, chunk_size, channel_id,
        handle_id, service_type, confirm_tag, producer, consumer, browser, 0,
        dest_name_size, dest_name, mime_type_size, mime_type, encoding_size,
        encoding, amq_stdc_table_size (options),
        amq_stdc_table_data (options));
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_init_temporary (
    handle_fsm_context_t  *context,
    global_fsm_t          global,
    connection_fsm_t      connection,
    channel_fsm_t         channel,
    dbyte                 connection_id,
    dbyte                 channel_id,
    dbyte                 handle_id,
    dbyte                 service_type,
    byte                  producer,
    byte                  consumer,
    byte                  browser,
    const char            *dest_name,
    const char            *mime_type,
    const char            *encoding,
    amq_stdc_table_t      options,
    byte                  async,
    amq_stdc_lock_t       *created_lock,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    byte
        dest_name_size = strlen (dest_name);
    byte
        mime_type_size = strlen (mime_type);
    byte
        encoding_size = strlen (encoding);
    
    /*  Save values that will be needed in future                            */
    context->global = global;
    context->connection = connection;
    context->channel = channel;
    context->connection_id = connection_id;
    context->channel_id = channel_id;
    context->id = handle_id;

    /*  Register that we will be waiting for handle open completion          */
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            channel_id, handle_id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  We have to wait for HANDLE CREATED                                   */
    result = register_lock (context->global, context->connection_id,
        channel_id, handle_id, &(context->created_tag), created_lock);

    /*  Send HANDLE OPEN                                                     */    
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_OPEN_CONSTANT_SIZE +
        dest_name_size + mime_type_size + encoding_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_open (chunk, chunk_size, channel_id,
        handle_id, service_type, confirm_tag, producer, consumer, browser, 1,
        dest_name_size, dest_name, mime_type_size, mime_type, encoding_size,
        encoding, amq_stdc_table_size (options),
        amq_stdc_table_data (options));
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_created (
    handle_fsm_context_t  *context,
    dbyte                 dest_name_size,
    const char            *dest_name
    )
{
    apr_status_t
        result;

    /*  Store temporary destination name                                     */
    context->dest_name = (char*) apr_palloc (context->pool,
        dest_name_size + 1);
    strncpy (context->dest_name, dest_name, dest_name_size);
    (context->dest_name) [dest_name_size] = 0;

    /*  Unsuspend thread waiting for temporary queue creation                */
    result = release_lock (context->global, context->created_tag,
        (void*) context->dest_name);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS; 
}

inline static apr_status_t do_consume (
    handle_fsm_context_t  *context,
    dbyte                 prefetch,
    byte                  no_local,
    byte                  unreliable,
    const char            *dest_name,
    const char            *identifier,
    const char            *selector,
    const char            *mime_type,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    byte
        dest_name_size = strlen (dest_name);
    byte
        identifier_size = strlen (identifier);
    dbyte
        selector_size = strlen (selector);
    byte
        mime_type_size = strlen (mime_type);

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE CONSUME                                                  */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_CONSUME_CONSTANT_SIZE + dest_name_size +
        identifier_size + selector_size + mime_type_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_consume (chunk, chunk_size, context->id,
        confirm_tag, prefetch, no_local, unreliable, dest_name_size,
        dest_name, identifier_size, identifier, selector_size, selector,
        mime_type_size, mime_type);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_send_message (
    handle_fsm_context_t  *context,
    byte                  out_of_band,
    byte                  recovery,
    byte                  streaming,
    const char            *dest_name,
    byte                  persistent,
    byte                  priority,
    qbyte                 expiration,
    const char            *mime_type,
    const char            *encoding,
    const char            *identifier,
    apr_size_t            data_size,
    void                  *data,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        command_size;
    qbyte
        header_size;
    dbyte
        confirm_tag;
    byte
        dest_name_size = strlen (dest_name);
    byte
        mime_type_size = strlen (mime_type);
    byte
        encoding_size = strlen (encoding);
    byte
        identifier_size = strlen (identifier);
    
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE SEND + MESSAGE HEAD + data                               */
    command_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_SEND_CONSTANT_SIZE + dest_name_size;
    header_size = AMQ_STDC_MESSAGE_HEAD_CONSTANT_SIZE + mime_type_size +
        encoding_size + identifier_size;
    chunk = (char*) amq_malloc (command_size + header_size + data_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    command_size = amq_stdc_encode_handle_send (chunk, command_size,
        context->id, confirm_tag, header_size + data_size, 0, out_of_band,
        recovery, streaming, dest_name_size, dest_name);
    if (!command_size)
        AMQ_ASSERT (Framing error)
    header_size = amq_stdc_encode_message_head (chunk + command_size,
        header_size, data_size, persistent, priority, expiration,
        mime_type_size, mime_type, encoding_size, encoding, identifier_size,
        identifier, 0, "");
    if (!header_size)
        AMQ_ASSERT (Framing error)
    memcpy ((void*) (chunk + command_size + header_size), (void*) data,
        data_size);
    result = connection_fsm_send_chunk (context->connection, chunk,
        command_size + header_size + data_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_flow (
    handle_fsm_context_t  *context,
    byte                  pause,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE FLOW                                                     */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_FLOW_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_flow (chunk, chunk_size, context->id,
          confirm_tag, pause);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_cancel (
    handle_fsm_context_t  *context,
    const char            *dest_name,
    const char            *identifier,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    byte
        dest_name_size = strlen (dest_name);
    byte
        identifier_size = strlen (identifier);

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE CANCEL                                                   */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_CANCEL_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_cancel (chunk, chunk_size, context->id,
        confirm_tag, dest_name_size, dest_name, identifier_size, identifier);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_unget (
    handle_fsm_context_t  *context,
    qbyte                 message_nbr,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE UNGET                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_UNGET_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_unget (chunk, chunk_size, context->id,
        confirm_tag, message_nbr);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_query (
    handle_fsm_context_t  *context,
    qbyte                 message_nbr,
    const char            *dest_name,
    const char            *selector,
    const char            *mime_type,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    byte
        dest_name_size = strlen (dest_name);
    dbyte
        selector_size = strlen (selector);
    byte
        mime_type_size = strlen (mime_type);

    /*  Send HANDLE QUERY                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_QUERY_CONSTANT_SIZE +
        dest_name_size + selector_size + mime_type_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_query (chunk, chunk_size, context->id,
        message_nbr, dest_name_size, dest_name, selector_size, selector,
        mime_type_size, mime_type);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    /*  Create lock to wait for HANDLE INDEX                                 */
    result = register_lock (context->global, context->connection_id,
        context->channel_id, context->id, &(context->index_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    return APR_SUCCESS;
}

inline static apr_status_t do_index (
    handle_fsm_context_t  *context,
    qbyte                 message_nbr,
    qbyte                 message_list_size,
    const char            *message_list
    )
{
    apr_status_t
        result;
    char
        *out;
    
    if (!context->index_tag)
        AMQ_ASSERT (HANDLE INDEX received when no HANDLE QUERY was issued)

    /*  Make copy of result                                                  */
    out = (char*) amq_malloc (message_list_size + 1);
    if (!out)
        AMQ_ASSERT (Not enough memory)
    strncpy (out, message_list, message_list_size);
    out [message_list_size] = 0;

    /*  Resume thread waiting for the HANDLE INDEX                           */
    result = release_lock (context->global, context->index_tag,
        (void*) out);
    AMQ_ASSERT_STATUS (result, release_lock)

    /*  Release exclusive access to QUERY/INDEX dialogue                     */
    context->index_tag = 0;
#   ifdef AMQTRACE_LOCKS
        printf ("# Exclusive access to QUERY/INDEX dialogue released\n");
#   endif
    result = apr_thread_mutex_unlock (context->query_sync);
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_unlock)
    return APR_SUCCESS;    
}

inline static apr_status_t do_browse (
    handle_fsm_context_t  *context,
    qbyte                 message_nbr,
    byte                  async,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    browse_list_item_t
        *new_item;
    dbyte
        confirm_tag;

    if (!async) {

        /*  Register that we are waiting for reply                           */
        result = register_lock (context->global, context->connection_id,
            context->channel_id, context->id, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)

        /*  Register that we are waiting for specific message                */
        new_item = (browse_list_item_t*)
            amq_malloc (sizeof (browse_list_item_t));
        if (!new_item)
            AMQ_ASSERT (Not enough memory)
        new_item->message_nbr = message_nbr;
        new_item->lock_id = confirm_tag;
        new_item->next = context->browse_requests;
        context->browse_requests = new_item;
    }

    /*  Send HANDLE BROWSE                                                   */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_BROWSE_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_browse (chunk, chunk_size, context->id,
        confirm_tag, message_nbr);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS; 
}

inline static apr_status_t do_reply (
    handle_fsm_context_t  *context,
    dbyte                 confirm_tag,
    dbyte                 reply_code,
    dbyte                 reply_text_size,
    const char            *reply_text
    )
{

    apr_status_t
        result;
    byte
        processed;
    browse_list_item_t
        *browse_request;
    browse_list_item_t
        **prev_browse_request;
    
    /*  If this is negative response to HANDLE BROWSE, remove                */
    /*  corresponding item from browse request list and return NULL          */
    browse_request = context->browse_requests;
    prev_browse_request = &(context->browse_requests);
    processed = 0;
    while (browse_request)
    {
        if (browse_request->lock_id == confirm_tag) {
            *prev_browse_request = browse_request->next;
            amq_free ((void*) browse_request);
            result = release_lock (context->global, confirm_tag, NULL);
            AMQ_ASSERT_STATUS (result, release_lock)
            processed = 1;
            break;
        }
        prev_browse_request = &(browse_request->next);
        browse_request = browse_request->next;
    }
    if (!processed) {

        /*  Resume the thread waiting for this confirmation                  */
        result = release_lock (context->global, confirm_tag,
            (void*) context);
        AMQ_ASSERT_STATUS (result, release_lock)
    }
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    handle_fsm_context_t  *context,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;

    result = register_lock (context->global, context->connection_id,
        context->channel_id, 0, &(context->shutdown_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send HANDLE CLOSE                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_close (chunk, chunk_size, context->id,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS; 
}

inline static apr_status_t do_duplicate_terminate (
    handle_fsm_context_t  *context,
    amq_stdc_lock_t       *lock
    )
{
    /*  This must be an explicit error; if not defined, it would fall        */
    /*  through default state and send HANDLE CLOSE once more                */
    AMQ_ASSERT (Handle is already being terminated)

    return APR_SUCCESS; 
}

inline static apr_status_t do_client_requested_close (
    handle_fsm_context_t  *context
    )
{
    apr_status_t
        result;

    /*  Release with error all threads waiting on channel, except the one    */
    /*  requesting channel termination                                       */
    result = release_all_locks (context->global, 0, 0, context->id,
        context->shutdown_tag, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  Resume client thread waiting for handle termination                  */
    result = release_lock (context->global, context->shutdown_tag, NULL);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;    
}

inline static apr_status_t do_server_requested_close (
    handle_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;

    /*  Release with error all threads waiting on channel                    */
    result = release_all_locks (context->global, 0, 0, context->id,
        0, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  Send HANDLE CLOSE                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_close (chunk, chunk_size, context->id,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;    
}
