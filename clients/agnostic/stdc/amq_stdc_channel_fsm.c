/*---------------------------------------------------------------------------
 *  amq_stdc_channel.c - Definition of CHANNEL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_channel_fsm.h"
#include "amq_stdc_channel_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_handle_list_item_t
{
    handle_fsm_t
        handle;
    apr_uint16_t
        handle_id;
    struct tag_handle_list_item_t
        *next;
} handle_list_item_t;

#define CHANNEL_OBJECT_ID context->id

DEFINE_CHANNEL_FSM_CONTEXT_BEGIN
    apr_socket_t
        *socket;                    /*  Socket (same as connection socket)   */
    apr_uint16_t
        id;                         /*  Channel id                           */
    apr_uint16_t
        connection_id;              /*  Connection id                        */
    global_fsm_t
        global;                     /*  Global object handle                 */
    connection_fsm_t
        connection;                 /*  Connection channel belongs to        */
    handle_list_item_t
        *handles;                   /*  Linked list of all handles           */
    apr_uint16_t
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for CHANNEL CLOSE                    */
DEFINE_CHANNEL_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    channel_fsm_context_t *context
    )
{
    context->handles = NULL;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    channel_fsm_context_t *context
    )
{
    apr_status_t
        result;

    /*  Remove channel from connection's list                                */
    result = connection_fsm_remove_channel (context->connection, context);
    AMQ_ASSERT_STATUS (result, connection_fsm_remove_channel)
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
    channel_fsm_t     context,
    apr_uint16_t      handle_id,
    handle_fsm_t      *handle)
{
    apr_status_t
        result;
    handle_list_item_t
        *item;

    result = channel_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, channel_fsm_sync_begin);

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

    result = channel_fsm_sync_end (context);
    AMQ_ASSERT_STATUS (result, channel_fsm_sync_end);

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions (for documentation see amq_stdc_fsms.xml)
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    channel_fsm_context_t  *context,
    global_fsm_t           global,
    connection_fsm_t       connection,
    apr_socket_t           *socket,
    apr_uint16_t           connection_id,
    apr_uint16_t           channel_id,
    apr_byte_t             transacted,
    apr_byte_t             restartable,
    apr_byte_t             async,
    lock_t                 *lock
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
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send channel open                                                    */
    result = amqp_channel_open (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, transacted, restartable, 0, "", "");
    AMQ_ASSERT_STATUS (result, amqp_channel_open);
    return APR_SUCCESS;
}

inline static apr_status_t do_create_handle (
    channel_fsm_context_t  *context,
    apr_uint16_t           service_type,
    apr_byte_t             producer,
    apr_byte_t             consumer,
    apr_byte_t             browser,
    apr_byte_t             temporary,
    const char             *dest_name,
    const char             *mime_type,
    const char             *encoding,
    apr_byte_t             async,
    lock_t                 *created_lock,
    lock_t                 *lock
    )
{
    apr_status_t
        result;
    handle_fsm_t
        handle;
    handle_list_item_t
        *item;
    apr_uint16_t
        handle_id;

    /*  Assign new handle id                                                 */
    result = global_fsm_assign_new_handle_id (context->global, &handle_id);
    AMQ_ASSERT_STATUS (result, global_fsm_assign_new_handle_id)

    /*  Create handle object                                                 */
    result = handle_fsm_create (&handle);
    AMQ_ASSERT_STATUS (result, handle_fsm_create)

    /*  Insert it into linked list of existing handles                       */
    item = (handle_list_item_t*)
        amq_malloc (sizeof (handle_list_item_t));
    if (!item)
        AMQ_ASSERT (Not enough memory)
    item->handle = handle;
    item->handle_id = handle_id;
    item->next = context->handles;
    context->handles = item;

    /*  Start it                                                         */
    if (!temporary) {
        result = handle_fsm_init (handle, context->global, context,
            context->socket, context->connection_id, context->id, handle_id,
            service_type, producer, consumer, browser, dest_name,
            mime_type, encoding, async, lock);
        AMQ_ASSERT_STATUS (result, handle_fsm_init)
        if (created_lock)
            *created_lock = NULL;
    }
    else {
        result = handle_fsm_init_temporary (handle, context->global,
            context, context->socket, context->id, handle_id,
            service_type, producer, consumer, browser, dest_name,
            mime_type, encoding, async, created_lock, lock);
        AMQ_ASSERT_STATUS (result, handle_fsm_init_temporary)
    }
    return APR_SUCCESS;
}

inline static apr_status_t do_remove_handle (
    channel_fsm_t  context,
    handle_fsm_t   handle
    )
{
    handle_list_item_t
        *item;
    handle_list_item_t
        **prev;

    item = context->handles;
    prev = &(context->handles);
    while (1) {
        if (!item)
            AMQ_ASSERT (Handle specified does not exist)
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
    channel_fsm_context_t  *context,
    apr_uint32_t           message_nbr,
    apr_byte_t             async,
    lock_t                 *lock
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
        AMQ_ASSERT_STATUS (result, register_lock)
    }
    result = amqp_channel_ack (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, message_nbr);
    AMQ_ASSERT_STATUS (result, amqp_channel_ack);
    return APR_SUCCESS;
}

inline static apr_status_t do_commit (
    channel_fsm_context_t  *context,
    apr_byte_t             async,
    lock_t                 *lock
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
        AMQ_ASSERT_STATUS (result, register_lock)
    }
    result = amqp_channel_commit (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, 0, "");
    AMQ_ASSERT_STATUS (result, amqp_channel_commit);
    return APR_SUCCESS;
}

inline static apr_status_t do_rollback (
    channel_fsm_context_t  *context,
    apr_byte_t             async,
    lock_t                 *lock
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
        AMQ_ASSERT_STATUS (result, register_lock)
    }
    result = amqp_channel_rollback (context->socket, buffer, BUFFER_SIZE,
        context->id, confirm_tag, 0, "");
    AMQ_ASSERT_STATUS (result, amqp_channel_rollback);
    return APR_SUCCESS;
}

inline static apr_status_t do_reply (
    channel_fsm_context_t  *context,
    apr_uint16_t           confirm_tag,
    apr_uint16_t           reply_code,
    const char             *reply_text
    )
{
    apr_status_t
        result;

    result = release_lock (context->global, confirm_tag,
        (void*) context);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    channel_fsm_context_t  *context,
    lock_t                 *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  TODO: Close handles, rollback transaction                            */

    /*  Send CHANNEL CLOSE                                                   */
    result = register_lock (context->global, context->connection_id,
        context->id, 0, &(context->shutdown_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)
    result = amqp_channel_close (context->socket, buffer, BUFFER_SIZE,
        context->id, 200, "Channel close requested by client");
    AMQ_ASSERT_STATUS (result, amqp_channel_close)
    return APR_SUCCESS; 
}

inline static apr_status_t do_duplicate_terminate (
    channel_fsm_context_t  *context,
    lock_t                 *lock
    )
{
    /*  This must be an explicit error; if not defined, it would fall        */
    /*  through default state and send CHANNEL CLOSE once more               */
    AMQ_ASSERT (Channel is already being terminated)

    return APR_SUCCESS; 
}

inline static apr_status_t do_client_requested_close (
    channel_fsm_context_t  *context
    )
{
    apr_status_t
        result;

    /*  Release with error all threads waiting on channel, except the        */
    /*  requesting channel termination                                       */
    result = release_all_locks (context->global, 0,
        context->id, 0, context->shutdown_tag, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  Resume client thread waiting for channel termination                 */
    result = release_lock (context->global, context->shutdown_tag, NULL);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;    
}

inline static apr_status_t do_server_requested_close (
    channel_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Release with error all threads waiting on channel                    */
    result = release_all_locks (context->global, 0,
        context->id, 0, 0, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  TODO: Close handles, rollback transaction                            */

    /*  Reply with CHANNEL CLOSE                                             */
    result = amqp_channel_close (context->socket, buffer, BUFFER_SIZE,
        context->id, 200, "Channel close requested by client");
    AMQ_ASSERT_STATUS (result, amqp_channel_close)
    return APR_SUCCESS;
}
