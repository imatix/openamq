/*---------------------------------------------------------------------------
 *  amq_stdc_channel.c - Definition of CHANNEL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_channel.h"
#include "channel_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_handle_list_item_t
{
    handle_t
        handle;
    apr_uint16_t
        handle_id;
    struct tag_handle_list_item_t
        *next;
} handle_list_item_t;

#define CHANNEL_OBJECT_ID context->id

DEFINE_CHANNEL_CONTEXT_BEGIN
    apr_socket_t
        *socket;                    /*  Socket (same as connection socket)   */
    apr_uint16_t
        id;                         /*  Channel id                           */
    apr_uint16_t
        connection_id;              /*  Connection id                        */
    global_t
        global;                     /*  Global object handle                 */
    connection_t
        connection;                 /*  Connection channel belongs to        */
    handle_list_item_t
        *handles;                   /*  Linked list of all handles           */
    apr_uint16_t
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for CHANNEL CLOSE                    */
DEFINE_CHANNEL_CONTEXT_END

inline static apr_status_t do_construct (
    channel_context_t *context
    )
{
    context->handles = NULL;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    channel_context_t *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Remove channel from connection's list                                */
    result = connection_remove_channel (context->connection,
        (channel_t) context);
    TEST(result, connection_remove_channel, buffer)
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: channel_get_handle

    Synopsis:
    Converts handle id into handle handle.
    Used by connection_get_handle.

    Arguments:
        ctx                channel handle
        handle_id          id to map
        handle             out parameter; mapped handle
    -------------------------------------------------------------------------*/
apr_status_t channel_get_handle (
    channel_t     ctx,
    apr_uint16_t  handle_id,
    handle_t      *handle)
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    channel_context_t
        *context = (channel_context_t*) ctx;
    handle_list_item_t
        *item;

    result = channel_sync_begin (context);
    TEST(result, channel_sync_begin, buffer);

    item = context->handles;
    while (1) {
        if (!item) {
            if (handle) *handle = NULL;
            break;
        }
        if (item->handle_id == handle_id) {
            if (handle) *handle = item->handle;
            break;
        }
        item = item->next;
    }

    result = channel_sync_end (context);
    TEST(result, channel_sync_end, buffer);

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    channel_context_t  *context,
    global_t           global,
    connection_t       connection,
    apr_socket_t       *socket,
    apr_uint16_t       connection_id,
    apr_uint16_t       channel_id,
    apr_byte_t         transacted,
    apr_byte_t         restartable,
    apr_byte_t         async,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    apr_uint16_t
        confirm_tag;
    
    /*  Save values that will be needed in future                            */
    context->global = global;
    context->connection = connection;
    context->socket = socket;
    context->connection_id = connection_id;
    context->id = channel_id;
    /*  Register that we will be waiting for channel open completion         */
    if (lock)
        *lock = NULL;
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            channel_id, 0, &confirm_tag, lock);
        TEST(result, register_lock, buffer)
    }
    /*  Send channel open                                                    */
    result = amqp_channel_open (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, transacted, restartable, 0, "", "");
    TEST(result, amqp_channel_open, buffer);
    return APR_SUCCESS;
}

inline static apr_status_t do_create_handle (
    channel_context_t  *context,
    apr_uint16_t       service_type,
    apr_byte_t         producer,
    apr_byte_t         consumer,
    apr_byte_t         browser,
    apr_byte_t         temporary,
    const char         *dest_name,
    const char         *mime_type,
    const char         *encoding,
    apr_byte_t         async,
    lock_t             *created_lock,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    handle_t
        handle;
    handle_list_item_t
        *item;
    apr_uint16_t
        handle_id;

    /*  Assign new handle id                                                 */
    result = global_assign_new_handle_id (context->global, &handle_id);
    TEST(result, connection_assign_new_handle_id, buffer)
    /*  Create handle object                                                 */
    result = handle_create (&handle);
    TEST(result, handle_create, buffer)
    /*  Insert it into linked list of existing handles                       */
    item = (handle_list_item_t*)
        amq_malloc (sizeof (handle_list_item_t));
    if (!item) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    item->handle = handle;
    item->handle_id = handle_id;
    item->next = context->handles;
    context->handles = item;
    /*  Start it                                                         */
    if (!temporary) {
        result = handle_init (handle, context->global, (channel_t) context,
            context->socket, context->connection_id, context->id, handle_id,
            service_type, producer, consumer, browser, dest_name,
            mime_type, encoding, async, lock);
        TEST(result, handle_init, buffer)
        if (created_lock)
            *created_lock = NULL;
    }
    else {
        result = handle_init_temporary (handle, context->global,
            (channel_t) context, context->socket, context->id, handle_id,
            service_type, producer, consumer, browser, dest_name,
            mime_type, encoding, async, created_lock, lock);
        TEST(result, handle_init_temporary, buffer)
    }
    return APR_SUCCESS;
}

inline static apr_status_t do_remove_handle (
    channel_t  ctx,
    handle_t   handle
    )
{
    channel_context_t
        *context = (channel_context_t*) ctx;
    handle_list_item_t
        *item;
    handle_list_item_t
        **prev;

    item = context->handles;
    prev = &(context->handles);
    while (1) {
        if (!item) {
            printf ("Handle specified in channel_remove_handle "
                "doesn't exist.\n");
            exit (1);
        }
        if (item->handle == handle) {
            *prev = item->next;
            amq_free (item);
            break;
        }
        prev = &(item->next);
        item = item->next;
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_acknowledge (
    channel_context_t  *context,
    apr_uint32_t       message_nbr,
    apr_byte_t         async,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    apr_uint16_t
        confirm_tag;

    confirm_tag = 0;
    if (lock)
        *lock = NULL;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        TEST(result, register_lock, buffer)
    }
    result = amqp_channel_ack (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, message_nbr);
    TEST(result, amqp_channel_ack, buffer);
    return APR_SUCCESS;
}

inline static apr_status_t do_commit (
    channel_context_t  *context,
    apr_byte_t         async,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    apr_uint16_t
        confirm_tag;

    confirm_tag = 0;
    if (lock)
        *lock = NULL;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        TEST(result, register_lock, buffer)
    }
    result = amqp_channel_commit (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, 0, "");
    TEST(result, amqp_channel_commit, buffer);
    return APR_SUCCESS;
}

inline static apr_status_t do_rollback (
    channel_context_t  *context,
    apr_byte_t         async,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    apr_uint16_t
        confirm_tag;

    confirm_tag = 0;
    if (lock)
        *lock = NULL;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
           context->id, 0, &confirm_tag, lock);
        TEST(result, register_lock, buffer)
    }
    result = amqp_channel_rollback (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, 0, "");
    TEST(result, amqp_channel_rollback, buffer);
    return APR_SUCCESS;
}

inline static apr_status_t do_reply (
    channel_context_t  *context,
    apr_uint16_t       confirm_tag,
    apr_uint16_t       reply_code,
    const char         *reply_text
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    result = release_lock (context->global, confirm_tag,
        (void*) context);
    TEST(result, release_lock, buffer)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    channel_context_t  *context,
    lock_t             *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Close handles, rollback transaction                                  */
    /*  ... to be done...  */
    /*  Send CHANNEL CLOSE                                                   */
    result = register_lock (context->global, context->connection_id,
        context->id, 0, &(context->shutdown_tag), lock);
    TEST(result, register_lock, buffer)
    result = amqp_channel_close (context->socket, buffer, BUFFER_SIZE,
        context->id, 200, "Channel close requested by client");
    TEST(result, amqp_channel_close, buffer)
    return APR_SUCCESS; 
}

inline static apr_status_t do_duplicate_terminate (
    channel_context_t  *context,
    lock_t             *lock
    )
{
    /*  This must be an explicit error; if not defined, it would fall        */
    /*  through default state and send CHANNEL CLOSE once more               */
    printf ("Channel is already being terminated.\n");
    exit (1);
    return APR_SUCCESS; 
}

inline static apr_status_t do_client_requested_close (
    channel_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Release with error all threads waiting on channel, except the        */
    /*  requesting channel termination                                       */
    result = release_all_locks (context->global, 0,
        context->id, 0, context->shutdown_tag, AMQ_OBJECT_CLOSED);
    TEST(result, release_all_locks, buffer)
    /*  Resume client thread waiting for channel termination                 */
    result = release_lock (context->global, context->shutdown_tag, NULL);
    TEST(result, release_lock, buffer)
    return APR_SUCCESS;    
}

inline static apr_status_t do_server_requested_close (
    channel_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Release with error all threads waiting on channel                    */
    result = release_all_locks (context->global, 0,
        context->id, 0, 0, AMQ_OBJECT_CLOSED);
    TEST(result, release_all_locks, buffer)
    /*  Close handles, rollback transaction                                  */
    /*  ... to be done...  */
    /*  Reply with CHANNEL CLOSE                                             */
    result = amqp_channel_close (context->socket, buffer, BUFFER_SIZE,
        context->id, 200, "Channel close requested by client");
    TEST(result, amqp_channel_close, buffer)
    return APR_SUCCESS;
}
