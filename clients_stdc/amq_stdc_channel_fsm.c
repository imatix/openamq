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

typedef struct tag_browse_list_item_t
{
    qbyte
        message_nbr;
    dbyte
        lock_id;
    struct tag_browse_list_item_t
        *next;
} browse_list_item_t;

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
                                       /*  Should be first item of the       */
                                       /*  structure so that message_desc    */
                                       /*  can be casted directly to         */
                                       /*  message_list_item_t               */
    message_fsm_t
        message;                       /*  Message object                    */ 
    char
        dest_name [256];               /*  Buffer to hold destination name   */
                                       /*  while message is opened           */
    char
        identifier [256];              /*  Buffer to hold message identifier */
                                       /*  while message is opened           */
    char
        mime_type [256];               /*  Buffer to hold MIME type          */
                                       /*  while message is opened           */
    char
        encoding [256];                /*  Buffer to hold encoding           */
                                       /*  while message is opened           */
    byte
        dispatched;                    /*  If 1, message has been already    */
                                       /*  delivered to client, cannot be    */
                                       /*  delivered for a second time       */
                                       /*  It is clients responsibility      */
                                       /*  to remove it from the list        */
    struct tag_message_list_item_t
        *prev;
    struct tag_message_list_item_t
        *next;
} message_list_item_t;

typedef struct tag_subscription_list_item_t
{
    dbyte
        created_tag;
    char
        *dest_name;
    struct tag_subscription_list_item_t
        *next;
} subscription_list_item_t;

typedef struct tag_handle_list_item_t
{
    dbyte
        handle_id;
    dbyte
        shutdown_tag;
    subscription_list_item_t
        *subscriptions;
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
    dbyte
        index_tag;                  /*  Id of lock that's used for waiting   */
                                    /*  for HANDLE INDEX                     */
    apr_thread_mutex_t
        *query_sync;                /*  Critical section allowing only       */
                                    /*  single thread to perform             */
                                    /*  HANDLE QUERY at the same time        */
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
    browse_list_item_t
        *browse_requests;           /*  Linked list of requests for          */
                                    /*  synchronous HANDLE BROWSEs           */
DEFINE_CHANNEL_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    channel_fsm_context_t *context
    )
{
    apr_status_t
        result;

    context->first_message = NULL;
    context->last_message = NULL;
    context->read_message = NULL;
    context->first_lock = NULL;
    context->last_lock = NULL;
    context->handles = NULL;
    context->browse_requests = NULL;
    context->shutdown_tag = 0;
    context->index_tag = 0;

    result = apr_thread_mutex_create (&(context->query_sync),
        APR_THREAD_MUTEX_UNNESTED, context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_create)

    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    channel_fsm_context_t *context
    )
{
    apr_status_t
        result;

    /*  TODO: Close all handles, clear handle list, close all messages,      */
    /*  clear message desc list, etc.                                        */

    /*  Deallocate all messages still existing                               */
    while (context->first_message) {
        result = message_fsm_terminate (context->first_message->message);
        AMQ_ASSERT_STATUS (result, message_fsm_terminate)
    }

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
    if (context->first_lock) {
        /*  There's a thread waiting for a message                           */
        lock = context->first_lock;
        message = context->first_message; 
   
        /*  Find whether there is message undispatched so far                */
        while (message && message->dispatched)
            message = message->next;

        /*  If there is no undispatched message, do nothing                  */
        if (!message)
            return APR_SUCCESS;

        /*  Remove lock from lock list                                       */
        context->first_lock = lock->next;
        if (lock->next)
            lock->next->prev = NULL;
        else
            context->last_lock = NULL;

        /*  Resume thread waiting for the message                            */
        result = release_lock (context->global, lock->lock_id,
            (void*) &(message->desc));
        AMQ_ASSERT_STATUS (result, release_lock);

        /*  Deallocate lock list item                                        */
        /*  (Message list item will be deallocated when client destroys the  */
        /*  message)                                                         */
        amq_free ((void*) lock);
    }

    /*  If no thread is waiting for message, do nothing                      */
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
        context              channel handle
    -------------------------------------------------------------------------*/

apr_status_t get_exclusive_access_to_query_dialogue (
    channel_fsm_t  context
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

/*  -------------------------------------------------------------------------
    Function: channel_has_handle

    Synopsis:
    Determines whether channel has handle with specified id opened.
    If so returns 1, otherwise 0.
    Used by connection_get_channel_from_handle

    Arguments:
        ctx                  channel handle
        handle_id            id to map
    -------------------------------------------------------------------------*/

byte channel_has_handle (
    channel_fsm_t     context,
    dbyte             handle_id
    )
{
    apr_status_t
        result;
    handle_list_item_t
        *item;
    byte
        rc = 0;

    result = channel_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, channel_fsm_sync_begin);

    item = context->handles;
    while (1) {
        if (!item)
            break;
        if (item->handle_id == handle_id) {
            rc = 1;
            break;
        }
        item = item->next;
    }

    result = channel_fsm_sync_end (context);
    AMQ_ASSERT_STATUS (result, channel_fsm_sync_end);

    return rc;
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
    dbyte                  options_size,
    const char             *options,
    const char             *out_of_band,
    byte                   async,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    dbyte
        confirm_tag;
    qbyte
        out_of_band_size = strlen (out_of_band);
    qbyte
        chunk_size;
    char
        *chunk;

    if (out_of_band_size > 255)
        AMQ_ASSERT (Out of band field longer than 255 characters)
    
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
        options_size + out_of_band_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_open (chunk, chunk_size, context->id,
        confirm_tag, transacted, restartable, options_size, options,
        out_of_band_size, out_of_band);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_open_handle (
    channel_fsm_context_t  *context,
    dbyte                  service_type,
    byte                   producer,
    byte                   consumer,
    byte                   browser,
    const char             *mime_type,
    const char             *encoding,
    dbyte                  options_size,
    const char             *options,
    byte                   async,
    dbyte                  *handle_id,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    dbyte
        id;
    handle_list_item_t
        *item;
    char
        *chunk;
    qbyte
        chunk_size;
    dbyte
        confirm_tag;
    qbyte
        mime_type_size = strlen (mime_type);
    qbyte
        encoding_size = strlen (encoding);

    if (mime_type_size > 255)
        AMQ_ASSERT (MIME type field exceeds 255 characters)
    if (encoding_size > 255)
        AMQ_ASSERT (Encoding field name exceeds 255 characters)

    if (lock)
        *lock = NULL;

    /*  Assign new handle id                                                 */
    result = global_fsm_assign_new_handle_id (context->global, &id);
    AMQ_ASSERT_STATUS (result, global_fsm_assign_new_handle_id)

    /*  Insert it into linked list of existing handles                       */
    item = (handle_list_item_t*)
        amq_malloc (sizeof (handle_list_item_t));
    if (!item)
        AMQ_ASSERT (Not enough memory)
    item->handle_id = id;
    item->shutdown_tag = 0;
    item->next = context->handles;
    item->subscriptions = NULL;
    context->handles = item;

    /*  Register that we will be waiting for handle open completion          */
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE OPEN                                                     */    
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_OPEN_CONSTANT_SIZE +
        mime_type_size + encoding_size + options_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_open (chunk, chunk_size, context->id,
        id, service_type, confirm_tag, producer, consumer, browser,
        mime_type_size, mime_type, encoding_size,
        encoding, options_size, options);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    if (handle_id)
        *handle_id = id;

    return APR_SUCCESS;
}

inline static apr_status_t do_created (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    dbyte                  dest_name_size,
    const char             *dest_name
    )
{
    apr_status_t
        result;
    handle_list_item_t
        *item;

    /*  Find apropriate handle                                               */
    item = context->handles;
    while (1) {
        if (!item)
            AMQ_ASSERT (Handle specified does not exist)
        if (item->handle_id == handle_id)
            break;
        item = item->next;
    }
    if (!item->subscriptions || !item->subscriptions->created_tag)
        AMQ_ASSERT (Unexpected HANDLE CREATED arrived)

    /*  Store destination name                                               */
    item->subscriptions->dest_name = (char*) amq_malloc (dest_name_size + 1);
    if (!item->subscriptions->dest_name)
        AMQ_ASSERT (Not enough memory)
    memcpy ((void*) item->subscriptions->dest_name, (void*) dest_name,
        dest_name_size);
    (item->subscriptions->dest_name) [dest_name_size] = 0;

    /*  Unsuspend thread waiting for temporary queue creation                */
    result = release_lock (context->global, item->subscriptions->created_tag,
        (void*) item->subscriptions->dest_name);
    AMQ_ASSERT_STATUS (result, release_lock)
    item->subscriptions->created_tag = 0;

    return APR_SUCCESS; 
}

inline static apr_status_t do_close_handle (
    channel_fsm_t    context,
    dbyte            handle_id,
    amq_stdc_lock_t  *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    handle_list_item_t
        *item;

    /*  Find apropriate handle                                               */
    item = context->handles;
    while (1) {
        if (!item)
            AMQ_ASSERT (Handle specified does not exist)
        if (item->handle_id == handle_id)
            break;
        item = item->next;
    }

    result = register_lock (context->global, context->connection_id,
        context->id, 0, &(item->shutdown_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send HANDLE CLOSE                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_close (chunk, chunk_size, handle_id,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_handle_closed (
    channel_fsm_t  context,
    dbyte          handle_id
    )
{
    apr_status_t
        result;
    handle_list_item_t
        *item;
    handle_list_item_t
        **prev;
    subscription_list_item_t
        *subscription;

    item = context->handles;
    prev = &(context->handles);
    while (1) {
        if (!item)
            AMQ_ASSERT (Handle specified does not exist)
        if (item->handle_id == handle_id)
            break;
        prev = &(item->next);
        item = item->next;
    }

    /* TODO: Unlock all locks waiting on this handle                         */
    /* Should be specified whether close_handle drops all pending results    */
    /* for QUERY, BROWSE, etc.                                               */

    /*  Resume thread waiting for close completion                           */
    if (item->shutdown_tag)
        result = release_lock (context->global, item->shutdown_tag, NULL);
    AMQ_ASSERT_STATUS (result, release_lock)

    /*  Remove handle from the list                                          */
    while (1) {
        subscription = item->subscriptions;
        if (!subscription)
            break;
        item->subscriptions = subscription->next;
        if (subscription->dest_name)
            amq_free ((void*) subscription->dest_name);    
        amq_free ((void*) subscription);
    }    
    *prev = item->next;
        amq_free (item);

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
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_commit (
    channel_fsm_context_t  *context,
    dbyte                  options_size,
    const char             *options,
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
        AMQ_STDC_CHANNEL_COMMIT_CONSTANT_SIZE + options_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_commit (chunk, chunk_size, context->id,
        confirm_tag, options_size, options);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_rollback (
    channel_fsm_context_t  *context,
    dbyte                  options_size,
    const char             *options,
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
        AMQ_STDC_CHANNEL_ROLLBACK_CONSTANT_SIZE + options_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_channel_rollback (chunk, chunk_size,
        context->id, confirm_tag, options_size, options);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);    

    return APR_SUCCESS;
}

inline static apr_status_t do_send_message (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    byte                   out_of_band,
    byte                   recovery,
    const char             *dest_name,
    byte                   persistent,
    byte                   immediate,
    byte                   priority,
    qbyte                  expiration,
    const char             *mime_type,
    const char             *encoding,
    const char             *identifier,
    dbyte                  headers_size,
    const char             *headers,
    apr_size_t             data_size,
    void                   *data,
    byte                   async,
    amq_stdc_lock_t        *lock
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
    qbyte
        dest_name_size = strlen (dest_name);
    qbyte
        mime_type_size = strlen (mime_type);
    qbyte
        encoding_size = strlen (encoding);
    qbyte
        identifier_size = strlen (identifier);

    if (dest_name_size > 255)
        AMQ_ASSERT (Destination name field exceeds 255 characters)
    if (mime_type_size > 255)
        AMQ_ASSERT (MIME type field exceeds 255 characters)
    if (encoding_size > 255)
        AMQ_ASSERT (Encoding field exceeds 255 characters)
    if (identifier_size > 255)
        AMQ_ASSERT (Identifier field exceeds 255 characters)
    
    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE SEND + MESSAGE HEAD + data                               */
    command_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_SEND_CONSTANT_SIZE + dest_name_size;
    header_size = AMQ_STDC_MESSAGE_HEAD_CONSTANT_SIZE + mime_type_size +
        encoding_size + identifier_size + headers_size;
    chunk = (char*) amq_malloc (command_size + header_size + data_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    command_size = amq_stdc_encode_handle_send (chunk, command_size,
        handle_id, confirm_tag, header_size + data_size, 0, out_of_band,
        recovery, immediate, dest_name_size, dest_name);
    if (!command_size)
        AMQ_ASSERT (Framing error)
    header_size = amq_stdc_encode_message_head (chunk + command_size,
        header_size, data_size, persistent, priority, expiration,
        mime_type_size, mime_type, encoding_size, encoding, identifier_size,
        identifier, headers_size, headers);
    if (!header_size)
        AMQ_ASSERT (Framing error)
    memcpy ((void*) (chunk + command_size + header_size), (void*) data,
        data_size);
    result = connection_fsm_send_chunk (context->connection, chunk,
        command_size + header_size + data_size, NULL, 0, 1,
        async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_consume (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    dbyte                  prefetch,
    byte                   no_local,
    byte                   no_ack,
    byte                   dynamic,
    const char             *dest_name,
    dbyte                  selector_size,
    const char             *selector,
    byte                   async,
    amq_stdc_lock_t        *created_lock,
    amq_stdc_lock_t        *lock
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
    qbyte
        dest_name_size = strlen (dest_name);
    subscription_list_item_t
        *subscription;
    handle_list_item_t
        *item;

    if (dest_name_size > 255)
        AMQ_ASSERT (Destination name field exceeds 255 characters)

    if (created_lock)
        *created_lock = NULL;

    confirm_tag = 0;
    if (!async) {
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  We have to wait for HANDLE CREATED                                   */
    if (dynamic && dest_name_size == 0) {

        /*  Find apropriate handle                                           */  
        item = context->handles;
        while (1) {
            if (!item)
                AMQ_ASSERT (Handle specified does not exist)
            if (item->handle_id == handle_id)
                break;
            item = item->next;
        }

        /*  Create subscription object                                       */
        subscription = (subscription_list_item_t*)
            amq_malloc (sizeof (subscription_list_item_t));
        if (!subscription)
            AMQ_ASSERT (Not enough memory)
        subscription->next = item->subscriptions;
        subscription->dest_name = NULL;
        result = register_lock (context->global, context->connection_id,
            context->id, 0, &(subscription->created_tag), created_lock);
        AMQ_ASSERT_STATUS (result, register_lock)
        item->subscriptions = subscription;
    }

    /*  Send HANDLE CONSUME                                                  */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_HANDLE_CONSUME_CONSTANT_SIZE + dest_name_size +
        selector_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_consume (chunk, chunk_size, handle_id,
        confirm_tag, prefetch, no_local, no_ack, dynamic, 0, dest_name_size,
        dest_name, selector_size, selector);

    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_receive_fragment (
    channel_fsm_context_t     *context,
    amq_stdc_handle_notify_t  *command,
    const char                *header_buffer,
    qbyte                     header_buffer_size
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
    browse_list_item_t
        *browse_request;
    browse_list_item_t
        **prev_browse_request;
    byte
        processed = 0;
    byte
        first_fragment = !context->read_message;

    if (first_fragment) {

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
        message->desc.delivered = command->delivered;
        message->desc.redelivered = command->redelivered;
        memcpy (message->dest_name, command->dest_name,
            command->dest_name_size);
        (message->dest_name) [command->dest_name_size] = 0;
        message->desc.dest_name = message->dest_name;
        memcpy (message->mime_type, header.mime_type,
            header.mime_type_size);
        (message->mime_type) [header.mime_type_size] = 0;
        message->desc.mime_type = message->mime_type;
        memcpy (message->encoding, header.encoding,
            header.encoding_size);
        (message->encoding) [header.encoding_size] = 0;
        message->desc.encoding = message->encoding;
        memcpy (message->identifier, header.identifier,
            header.identifier_size);
        (message->identifier) [header.identifier_size] = 0;
        message->desc.identifier = message->identifier;
        result = amq_stdc_table_create (header.headers_size, header.headers,
            &(message->desc.headers));
        AMQ_ASSERT_STATUS (result, amq_stdc_table_create);
        result = message_fsm_create (&(message->message));
        AMQ_ASSERT_STATUS (result, message_fsm_create)
        result = message_fsm_init (message->message, context->global, context,
            context->connection_id, context->id, &(message->desc));
        AMQ_ASSERT_STATUS (result, message_fsm_init)
        message->dispatched = 0;

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

    /*  Pair message with corresponding request                              */
    if (first_fragment) {

        if (command->delivered == 0) {

            /*  It is a reply to browse command                              */
            browse_request = context->browse_requests;
            prev_browse_request = &(context->browse_requests);
            while (browse_request) {
                if (browse_request->message_nbr ==
                      command->message_nbr) {

                    /*  It is a reply to sync browse command                 */
                    result = release_lock (context->global,
                        browse_request->lock_id, (void*) message);
                    AMQ_ASSERT_STATUS (result, release_lock)

                    /*  Remove the request from the list                     */
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
            /*  If there's a thread waiting for message, dispatch it         */
            result = s_pair_lock_and_message (context);
            AMQ_ASSERT_STATUS (result, s_pair_lock_and_message)
        }
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_receive_content_chunk (
    channel_fsm_context_t     *context,
    content_chunk_t           *chunk,
    byte                      last
    )
{
    apr_status_t
        result;

    /*  There must be message being read when chunk is received              */
    assert (context->read_message);

    if (!last) {
        result = message_fsm_receive_content_chunk (
            context->read_message->message, chunk);
        AMQ_ASSERT_STATUS (result, message_fsm_receive_content_chunk)
    }
    else {
        result = message_fsm_receive_last_content_chunk (
            context->read_message->message, chunk);
        AMQ_ASSERT_STATUS (result, message_fsm_receive_last_content_chunk)
        /* TODO:                                                             */        
        /* if (message_fsm_terminated (message->desc.stream))                */
        /*     message_fsm_destroy (message->desc.stream);                   */

        /*  Switch handle to non-reading 'state'                             */
        context->read_message = NULL;
    }

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

inline static apr_status_t do_remove_message_desc (
    channel_fsm_context_t    *context,
    amq_stdc_message_desc_t  *message_desc
    )
{
    apr_status_t
        result;
    message_list_item_t
        *item = (message_list_item_t*) message_desc;   

    /*  Remove item from the list                                            */
    if (!item->prev)
        context->first_message = item->next;
    else
        item->prev->next = item->next;
    if (!item->next)
        context->last_message = item->prev;
    else
        item->next->prev = item->prev;

    /*  Deallocate it                                                        */
    result = amq_stdc_table_destroy (item->desc.headers);
    AMQ_ASSERT_STATUS (result, amq_stdc_table_destroy);
    amq_free ((void*) item);

    return APR_SUCCESS;
}

inline static apr_status_t do_flow (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    byte                   pause,
    byte                   async,
    amq_stdc_lock_t        *lock
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
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE FLOW                                                     */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_FLOW_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_flow (chunk, chunk_size, handle_id,
          confirm_tag, pause);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_unget (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    qbyte                  message_nbr,
    byte                   async,
    amq_stdc_lock_t        *lock
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
            context->id, 0, &confirm_tag, lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /*  Send HANDLE UNGET                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_UNGET_CONSTANT_SIZE;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_unget (chunk, chunk_size, handle_id,
        confirm_tag, message_nbr);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_query (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    qbyte                  message_nbr,
    const char             *dest_name,
    const char             *selector,
    const char             *mime_type,
    amq_stdc_lock_t        *lock
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;
    qbyte
        dest_name_size = strlen (dest_name);
    qbyte
        selector_size = strlen (selector);
    qbyte
        mime_type_size = strlen (mime_type);

    if (dest_name_size > 255)
        AMQ_ASSERT (Destination name field exceeds 255 characters)
    if (selector_size > 65535)
        AMQ_ASSERT (Selector field exceeds 65535 characters)
    if (mime_type_size > 255)
        AMQ_ASSERT (MIME type field exceeds 255 characters)

    /*  Create lock to wait for HANDLE INDEX                                 */
    result = register_lock (context->global, context->connection_id,
        context->id, 0, &(context->index_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send HANDLE QUERY                                                    */
    chunk_size = COMMAND_SIZE_MAX_SIZE + AMQ_STDC_HANDLE_QUERY_CONSTANT_SIZE +
        dest_name_size + selector_size + mime_type_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_handle_query (chunk, chunk_size, handle_id,
        message_nbr, dest_name_size, dest_name, selector_size, selector,
        mime_type_size, mime_type);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context->connection, chunk,
        chunk_size, NULL, 0, 1, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_index (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    qbyte                  message_nbr,
    qbyte                  message_list_size,
    const char             *message_list
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
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    qbyte                  message_nbr,
    byte                   async,
    amq_stdc_lock_t        *lock
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
            context->id, 0, &confirm_tag, lock);
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
        chunk_size, NULL, 0, 1, async ? lock : NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS; 
}

inline static apr_status_t do_reply (
    channel_fsm_context_t  *context,
    dbyte                  handle_id,
    dbyte                  confirm_tag,
    dbyte                  reply_code,
    dbyte                  reply_text_size,
    const char             *reply_text
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
        result = release_lock (context->global, confirm_tag, NULL);
        AMQ_ASSERT_STATUS (result, release_lock)
    }
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
        chunk_size, NULL, 0, 1, NULL);
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
        context->id, context->shutdown_tag, AMQ_OBJECT_CLOSED);
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
        context->id, 0, AMQ_OBJECT_CLOSED);
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
        chunk_size, NULL, 0, 1, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}
