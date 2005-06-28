/*---------------------------------------------------------------------------
 *  amq_stdc_message_fsm.c - Definition of MESSAGE object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_message_fsm.h"
#include "amq_stdc_message_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

#define MESSAGE_FSM_OBJECT_ID context->message_desc ? \
    context->message_desc->message_nbr : -1

typedef struct tag_read_request_t
{
    dbyte
        lock_id;
    char
        *dest;
    qbyte
        size;
    struct tag_read_request_t
        *next;
} read_request_t;

DEFINE_MESSAGE_FSM_CONTEXT_BEGIN
    global_fsm_t
        global;
    channel_fsm_t
        channel;
    dbyte
        connection_id;
    dbyte
        channel_id;
    amq_stdc_message_desc_t
        *message_desc;
    content_chunk_t
        *current_chunk;
    qbyte
        current_pos;
    content_chunk_t
        *first_chunk;
    content_chunk_t
        *last_chunk;
    qbyte
        size;                          /*  Sum of all chunk sizes            */
    byte
        completed;                     /*  If 1, whole message was already   */
                                       /*  filled in                         */
    read_request_t
        *first_request;
    read_request_t
        *last_request;
DEFINE_MESSAGE_FSM_CONTEXT_END

struct tag_inpipe_context_t
{
    message_fsm_context_t message;
} inpipe_context_t;

struct tag_outpipe_context_t
{
    message_fsm_context_t message;
} outpipe_context_t;

struct tag_stream_context_t
{
    message_fsm_context_t message;
} stream_context_t;

inline static apr_status_t do_construct (
    message_fsm_context_t *context
    )
{
    context->global = NULL;
    context->channel = NULL;
    context->connection_id = 0;
    context->channel_id = 0;
    context->message_desc = NULL;
    context->current_pos = 0;
    context->first_chunk = NULL;
    context->last_chunk = NULL;
    context->first_request = NULL;
    context->last_request = NULL;
    context->completed = 0;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    message_fsm_context_t *context
    )
{
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Private functions
 *---------------------------------------------------------------------------*/

apr_status_t s_copy_data_from_pipe (
    message_fsm_context_t  *context,
    char                   *destination,
    qbyte                  size,
    qbyte                  *out_size
    )
{
    qbyte
        left_in_chunk;
    qbyte
        to_read;
    qbyte
        already_read = 0;

    while (1) {
        if (!context->current_chunk)
            break;
        left_in_chunk =
            context->current_chunk->size - context->current_pos;
        to_read = left_in_chunk < size ? left_in_chunk : size;
        if (destination)
            memcpy (destination + already_read,
                context->current_chunk->data +
                context->current_pos, to_read);
        context->current_pos += to_read;
        size -= to_read;
        already_read += to_read;
        if (!size)
            break;
        /*  TODO: deallocate chunk here                                  */
        context->current_chunk = context->current_chunk->next;
        context->current_pos = 0;
    }
    context->size -= already_read;
    if (out_size)
        *out_size = already_read;

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions (for documentation see amq_stdc_fsms.xml)
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    message_fsm_context_t    *context,
    global_fsm_t             global,
    channel_fsm_t            channel,
    dbyte                    connection_id,
    dbyte                    channel_id,
    amq_stdc_message_desc_t  *message_desc
    )
{
    context->global = global;
    context->channel = channel;
    context->connection_id = connection_id;
    context->channel_id = channel_id;
    context->message_desc = message_desc;
    return APR_SUCCESS;
}

inline static apr_status_t do_receive_content_chunk (
    message_fsm_context_t  *context,
    content_chunk_t        *chunk
    )
{
    apr_status_t
        result;
    read_request_t
        *request;
    qbyte
        out_size;

    /*  Insert chunk into the chunk list                                     */
    chunk->next = NULL;
    chunk->prev = context->last_chunk;
    if (context->last_chunk)
        context->last_chunk->next = chunk;
    context->last_chunk = chunk;
    if (!context->first_chunk)
        context->first_chunk = chunk;
    if (!context->current_chunk)
        context->current_chunk = chunk;
    context->size += chunk->size;

    /*  Release first waiting thread if enough data is available or when     */
    /*  or when EOF is reached                                               */
    if (context->first_request && (context->completed || context->size >=
          context->first_request->size)) {

        request = context->first_request;

        result = s_copy_data_from_pipe (context, request->dest, request->size,
            &out_size);
        AMQ_ASSERT_STATUS (result, s_copy_data_from_pipe)
        result = release_lock (context->global, request->lock_id,
            (void*) out_size);
        AMQ_ASSERT_STATUS (result, release_lock)
        context->first_request = request->next;
        amq_free ((void*) request);
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_receive_last_content_chunk (
    message_fsm_context_t  *context,
    content_chunk_t        *chunk
    )
{
    context->completed = 1;
    return do_receive_content_chunk (context, chunk);
}

inline static apr_status_t do_ignore_content_chunk (
    message_fsm_context_t *context,
    content_chunk_t *chunk
    )
{
    /*  Data were received that nobody wants to read; deallocate it          */
    amq_free ((void*) chunk);
    return APR_SUCCESS;
}

inline static apr_status_t do_open_inpipe (
    message_fsm_context_t *context
    )
{
    return APR_SUCCESS;
}

inline static apr_status_t do_pread (
    message_fsm_context_t *context,
    char                  *destination,
    qbyte                 size,
    byte                  wait,
    byte                  complete,
    qbyte                 *out_size,
    amq_stdc_lock_t       *lock
    )
{
    apr_status_t
        result;
    read_request_t
        *request;

    if ((wait && (context->size >= size || context->completed)) ||
          (!wait && !complete) ||
          (!wait && complete && (context->size >= size || context->completed))) {

        /*  Enough data there in the pipe                                    */
        result = s_copy_data_from_pipe (context, destination, size, out_size);
        AMQ_ASSERT_STATUS (result, s_copy_data_from_pipe)
        if (lock)
            *lock = NULL;
    }
    else if (wait) {

        /*  Not enough data in pipe                                          */
        if (out_size)
            *out_size = 0;
        if (lock)
            *lock = NULL;
    }
    else {

        /*  Blocking version of the call                                     */
        request = (read_request_t*) amq_malloc (sizeof (read_request_t));
        if (!request)
            AMQ_ASSERT (Not enough memory)
        request->dest = destination;
        request->size = size;
        request->next = NULL;

        result = register_lock (context->global, context->connection_id,
            context->channel_id, &(request->lock_id), lock);
        AMQ_ASSERT_STATUS (result, register_lock)

        if (context->last_request)
            context->last_request->next = request;
        context->last_request = request;
        if (!context->first_request)
            context->first_request = request;
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_peof (
    message_fsm_context_t  *context,
    byte                   *out
    )
{
    if (out)
        *out = (context->completed && !context->current_chunk) ? 1 : 0;

    return APR_SUCCESS;
}

inline static apr_status_t do_term (
    message_fsm_context_t *context
    )
{
    apr_status_t
        result;
    content_chunk_t
        *chunk;
    read_request_t
        *request;

    /*  Deallocate message body                                              */
    while (context->first_chunk) {
        chunk = context->first_chunk;
        context->first_chunk = context->first_chunk->next;
        amq_free ((void*) chunk);
    }
    context->current_chunk = NULL;
    context->last_chunk = NULL;

    /*  Deallocate read requests                                             */
    while (context->first_request) {
        request = context->first_request;
        context->first_request = context->first_request->next;
        amq_free ((void*) request);
    }
    context->last_request = NULL;

    /*  Deallocate message descriptor stored by channel object               */
    result = channel_fsm_remove_message_desc (context->channel,
        context->message_desc);
    AMQ_ASSERT_STATUS (result, channel_fsm_remove_message_desc);
   
    /*  TODO: release all locks waiting on the object                        */
    /*  This means some "message id" parameter should be added to locking    */
    /*  subsystem                                                            */

    return APR_SUCCESS;
}
