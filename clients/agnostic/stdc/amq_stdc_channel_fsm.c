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

typedef struct tag_lock_list_item_t
{
    dbyte
        lock_id;
    struct tag_lock_list_item_t
        *prev;
    struct tag_lock_list_item_t
        *next;
} lock_list_item_t;

typedef struct tag_message_list_item_t
{
    amq_stdc_message_desc_t
        desc;                          /*  Message descriptor                */
    message_fsm_t
        message;                       /*  Message object                    */ 
    char
        dest_name [256];               /*  Buffer to hold destination name   */
                                       /*  while message is opened           */
    char
        identifier [256];              /*  Buffer to hold message identifier */
                                       /*  while message is opened           */
    struct tag_message_list_item_t
        *prev;
    struct tag_message_list_item_t
        *next;
} message_list_item_t;

typedef struct tag_handle_list_item_t
{
    handle_fsm_t
        handle;
    dbyte
        handle_id;
    struct tag_handle_list_item_t
        *next;
} handle_list_item_t;

#define CHANNEL_FSM_OBJECT_ID context->id

DEFINE_CHANNEL_FSM_CONTEXT_BEGIN
    dbyte
        id;                         /*  Channel id                           */
    dbyte
        connection_id;              /*  Connection id                        */
    global_fsm_t
        global;                     /*  Global object handle                 */
    connection_fsm_t
        connection;                 /*  Connection channel belongs to        */
    handle_list_item_t
        *handles;                   /*  Linked list of all handles           */
    dbyte
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for CHANNEL CLOSE                    */
    lock_list_item_t
        *first_lock;                /*  First lock waiting for message       */
    lock_list_item_t
        *last_lock;                 /*  Last lock waiting for message        */
    message_list_item_t
        *first_message;             /*  First item of list of received       */
                                    /*  messages                             */
    message_list_item_t
        *last_message;              /*  Last item of list of received        */
                                    /*  messages                             */
    message_list_item_t
        *read_message;              /*  Message actually being read          */
                                    /*  NULL if no message is being read     */         
DEFINE_CHANNEL_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    channel_fsm_context_t *context
    )
{
    context->first_message = NULL;
    context->last_message = NULL;
    context->read_message = NULL;
    context->first_lock = NULL;
    context->last_lock = NULL;
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
 *  Helper functions (private)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: s_pair_lock_and_message

    Synopsis:
    Pairs thread witing for message with one of the waiting messages

    Arguments:
        context              channel object
    -------------------------------------------------------------------------*/

static apr_status_t s_pair_lock_and_message (
    channel_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    lock_list_item_t
        *lock;
    message_list_item_t
        *message;

    /*  This function is called from event hadlers where context is already  */
    /*  locked, so no need to lock again                                     */
    if (context->first_lock && context->first_message) {

        /*  There's a thread waiting for a message and a message present     */
        lock = context->first_lock;
        message = context->first_message; 
       
        /*  Remove lock from lock list                                       */
        context->first_lock = lock->next;
        if (lock->next)
            lock->next->prev = NULL;
        else
            context->last_lock = NULL;

        /*  Remove message from message list                                 */
        context->first_message = message->next;
        if (message->next)
            message->next->prev = NULL;
        else
            context->last_message = NULL;

        /*  Resume thread waiting for the message                            */
        result = release_lock (context->global, lock->lock_id,
            (void*) &(message->desc));
        AMQ_ASSERT_STATUS (result, release_lock);

        /*  Deallocate lock list item                                        */
        /*  Message list item will be deallocated when client destroys the   */
        /*  message (amq_stdc_close_message)                                 */
        amq_free ((void*) lock);
    }
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
        ctx                  channel handle
        handle_id            id to map
        handle               out parameter; mapped handle
    -------------------------------------------------------------------------*/

apr_status_t channel_get_handle (
    channel_fsm_t     context,
    dbyte             handle_id,
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
    dbyte                  connection_id,
    dbyte                  channel_id,
    byte                   transacted,
    byte                   restartable,
    amq_stdc_table_t       options,
    const char             *out_of_band,
    byte                   async,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    dbyte
        confirm_tag;
    byte
        out_of_band_size = strlen (out_of_band);
    qbyte
        chunk_size;
    char
        *chunk;
    
    /*  Save values that will be needed in future                            */
    context->global = global;
    context->connection = connection;
    context->connection_id = connection_id;
    context->id = channel_id;

    /*  Register that we will be waiting for channel open completion         */
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            channel_id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send CHANNEL OPEN                                                    */    
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_CHANNEL_OPEN_CONSTANT_SIZE +
        amq_stdc_table_size (options) + out_of_band_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_open (chunk, chunk_size, context->id,
        confirm_tag, transacted, restartable, amq_stdc_table_size (options),
        amq_stdc_table_data (options), out_of_band_size, out_of_band);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_create_handle (
    channel_fsm_context_t  *context,
    dbyte                  service_type,
    byte                   producer,
    byte                   consumer,
    byte                   browser,
    byte                   temporary,
    const char             *dest_name,
    const char             *mime_type,
    const char             *encoding,
    amq_stdc_table_t       options,
    byte                   async,
    amq_stdc_lock_t        *created_lock,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    handle_fsm_t
        handle;
    handle_list_item_t
        *item;
    dbyte
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
        result = handle_fsm_init (handle, context->global, context->connection,
            context, context->connection_id, context->id, handle_id,
            service_type, producer, consumer, browser, dest_name,
            mime_type, encoding, options, async, lock);
        AMQ_ASSERT_STATUS (result, handle_fsm_init)
        if (created_lock)
            *created_lock = NULL;
    }
    else {
        result = handle_fsm_init_temporary (handle, context->global,
            context->connection, context, context->connection_id,
            context->id, handle_id, service_type, producer, consumer, browser,
            dest_name, mime_type, encoding, options, async, created_lock, lock);
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
    qbyte                  message_nbr,
    byte                   async,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;
    dbyte
        confirm_tag;

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send CHANNEL ACK                                                     */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_CHANNEL_ACK_CONSTANT_SIZE ;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_ack (chunk, chunk_size, context->id,
        confirm_tag, message_nbr);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_commit (
    channel_fsm_context_t  *context,
    amq_stdc_table_t       options,
    byte                   async,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;
    dbyte
        confirm_tag;

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send CHANNEL COMMIT                                                  */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CHANNEL_COMMIT_CONSTANT_SIZE + amq_stdc_table_size (options);
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_commit (chunk, chunk_size, context->id,
        confirm_tag, amq_stdc_table_size (options),
        amq_stdc_table_data (options));
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_rollback (
    channel_fsm_context_t  *context,
    amq_stdc_table_t       options,
    byte                   async,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;
    dbyte
        confirm_tag;

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
           context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send CHANNEL ROLLBACK                                                */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CHANNEL_ROLLBACK_CONSTANT_SIZE +
        amq_stdc_table_size (options);
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_rollback (chunk, chunk_size,
        context->id, confirm_tag, amq_stdc_table_size (options),
        amq_stdc_table_data (options));
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_receive_message (
    channel_fsm_context_t     *context,
    amq_stdc_handle_notify_t  *command,
    const char                *header_buffer,
    qbyte                     header_buffer_size,
    char                      *body,
    qbyte                     body_size
    )
{
    apr_status_t
        result;
    message_list_item_t
        *message;
    amq_stdc_message_head_t
        header;
    size_t
        size;

#if 0
    browse_list_item_t
        *browse_request;
    browse_list_item_t
        **prev_browse_request;

    processed = 0;
    if (message->fields.handle_notify.delivered == 0) {

        /*  It is a reply to browse command                                  */
        browse_request = context->browse_requests;
        prev_browse_request = &(context->browse_requests);
        while (browse_request) {
            if (browse_request->message_nbr ==
                  message->fields.handle_notify.message_nbr) {

                /*  It is a reply to sync browse command                     */
                /*  Make a copy of message and unsuspend the thread          */
                /*  that's waiting for it                                    */
                new_item = (message_list_item_t*)
                    amq_malloc (sizeof (message_list_item_t));
                memcpy ((void*) (new_item->buffer), (void*) message,
                    BUFFER_SIZE);
                new_item->frame = (amqp_frame_t*) (new_item->buffer);
                new_item->next = NULL;
                new_item->prev = NULL;
                result = release_lock (context->global,
                    browse_request->lock_id, (void*) new_item);

                /*  Remove the request from the list                         */
                *prev_browse_request = browse_request->next;
                amq_free ((void*) browse_request);
                processed = 1;
                break;
            }
            prev_browse_request = &(browse_request->next);
            browse_request = browse_request->next;
        }
    }

    if (!processed) {
#endif

    /*  Is it a first fragment of a message ?                                */
    if (!context->read_message) {

        /*  Allocate and fill in new message item                            */
        message = (message_list_item_t*)
            amq_malloc (sizeof (message_list_item_t));

        /*  Decode message header                                            */
        size = amq_stdc_decode_message_head (header_buffer, header_buffer_size,
            &header);
        if (!size)
            AMQ_ASSERT (Corrupted frame received from server)

        /*  Fill in message descriptor                                       */
        message->desc.message_nbr = command->message_nbr;
        memcpy (message->dest_name, command->dest_name,
            command->dest_name_size);
        (message->dest_name) [command->dest_name_size] = 0;
        message->desc.dest_name = message->dest_name;
        memcpy (message->identifier, header.identifier,
            header.identifier_size);
        (message->identifier) [header.identifier_size] = 0;
        message->desc.identifier = message->identifier;
        result = message_fsm_create (&(message->message));
        AMQ_ASSERT_STATUS (result, message_fsm_create)
        result = message_fsm_init (message->message);
        AMQ_ASSERT_STATUS (result, message_fsm_init)

        /*  Append it to the end of the queue                                */
        message->next = NULL;
        message->prev = context->last_message;
        if (context->last_message)
            context->last_message->next = message;
        context->last_message = message;
        if (!context->first_message)
            context->first_message = message;

        /*  Switch handle to reading 'state'                                 */
        context->read_message = message;
    }
    else {
        message = context->read_message;

        /*  Check whether fragment received belongs to the same message that */
        /*  we are currently reading                                         */
        if (message->desc.message_nbr != command->message_nbr)
            AMQ_ASSERT (Message fragment from another message received)
    }

    if (command->partial) {
        result = message_fsm_receive_fragment (message->message, body,
            body_size);
        AMQ_ASSERT_STATUS (result, message_fsm_receive_fragment)
    }
    else {
        result = message_fsm_receive_last_fragment (message->message, body,
            body_size);
        AMQ_ASSERT_STATUS (result, message_fsm_receive_last_fragment)
        /* TODO:                                                             */        
        /* if (message_fsm_terminated (message->desc.stream))                */
        /*     message_fsm_destroy (message->desc.stream);                   */

        /*  Switch handle to non-reading 'state'                             */
        context->read_message = NULL;
    }

    /*  If there's a thread waiting for message, dispatch it                 */
    /*  TODO: Maybe only first fragment should be paired, is it so ?         */
    result = s_pair_lock_and_message (context);
    AMQ_ASSERT_STATUS (result, s_pair_lock_and_message)

    return APR_SUCCESS;
}

inline static apr_status_t do_get_message (
    channel_fsm_context_t    *context,
    byte                     wait,
    amq_stdc_message_desc_t  **message_desc,
    amq_stdc_message_t       *message,
    amq_stdc_lock_t          *lock
    )
{
    apr_status_t
        result;
    lock_list_item_t
        *new_item;
    
    if (wait) {
        /*  Allocate and fill in new "request for message" item              */
        new_item = (lock_list_item_t*) amq_malloc (sizeof (lock_list_item_t));
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &(new_item->lock_id), lock);
        AMQ_ASSERT_STATUS (result, register_lock);

        /*  Append it to the end of the queue                                */
        new_item->next = NULL;
        new_item->prev = context->last_lock;
        if (context->last_lock)
            context->last_lock->next = new_item;
        context->last_lock = new_item;
        if (!context->first_lock)
            context->first_lock = new_item;

        /*  Try to pair request with the message possibly already received   */
        result = s_pair_lock_and_message (context);
        AMQ_ASSERT_STATUS (result, s_pair_lock_and_message)
    }
    else {
        /* No wait mode, return immediately whether message is               */
        /* present or not                                                    */
        if (context->first_message) {

            /*  Message present                                              */
            if (message)
                *message = context->first_message->message;
            if (message_desc)
                *message_desc = &(context->first_message->desc);

            /*  Remove message from the message list                         */
            context->first_message = context->first_message->next;
            if (context->first_message)
                context->first_message->prev = NULL;
            else
                context->last_message = NULL;
        }
        else {
            /*  No message is present                                        */
            if (message_desc)
                *message_desc = NULL;
            if (message)
                *message = NULL;
        }
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_reply (
    channel_fsm_context_t  *context,
    dbyte                  confirm_tag,
    dbyte                  reply_code,
    dbyte                  reply_text_size,
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
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;

    /*  TODO: Close handles, rollback transaction                            */

    /*  Release all threads waiting within the channel                       */
    result = register_lock (context->global, context->connection_id,
        context->id, 0, &(context->shutdown_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send CHANNEL CLOSE                                                   */    
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CHANNEL_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_close (chunk, chunk_size, context->id,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS; 
}

inline static apr_status_t do_duplicate_terminate (
    channel_fsm_context_t  *context,
    amq_stdc_lock_t        *lock
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
    qbyte
        chunk_size;
    char
        *chunk;

    /*  Release with error all threads waiting on channel                    */
    result = release_all_locks (context->global, 0,
        context->id, 0, 0, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  TODO: Close handles, rollback transaction                            */

    /*  Send CHANNEL CLOSE                                                   */    
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CHANNEL_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_close (chunk, chunk_size, context->id,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}
