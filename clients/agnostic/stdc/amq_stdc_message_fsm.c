/*---------------------------------------------------------------------------
 *  amq_stdc_message_fsm.c - Definition of GLOBAL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_message_fsm.h"
#include "amq_stdc_message_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

/*

Possible reading options:
   Wait for message (out of scope of stream object, affects only dispatching mechanism) (1) 
   Wait for whole message before access to stream (message ?) is granted (2) may be a separate function
   Wait while enough data to read specified size (3)
   Normal/Pipe (2)

Levels where these options may be set:
   1. When getting the message
   2. When opening the stram (not present yet)
   3 .When reading the data

Maximalistic proposal:

   get_message (wait);
   open_stream (pipe);
   ( wait_for_all )
   read (wait);

*/

typedef struct tag_fragment_list_item_t
{
    char
        *data;
    qbyte
        data_size;
    struct tag_fragment_list_item_t
        *prev;
    struct tag_fragment_list_item_t
        *next;
} fragment_list_item_t;

#define MESSAGE_FSM_OBJECT_ID 0 /* TODO: message number here */

DEFINE_MESSAGE_FSM_CONTEXT_BEGIN
    fragment_list_item_t
        *current_fragment;
    qbyte
        current_pos;
    fragment_list_item_t
        *first_fragment;
    fragment_list_item_t
        *last_fragment;
    /*  TODO: byte completed  */
DEFINE_MESSAGE_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    message_fsm_context_t *context
    )
{
    if (sizeof (fragment_list_item_t) > FRAGMENT_HEADER_SIZE)
        AMQ_ASSERT (Fragment header to small to hold the data)
    context->current_fragment = NULL;
    context->current_pos = 0;
    context->first_fragment = NULL;
    context->last_fragment = NULL;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    message_fsm_context_t *context
    )
{
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions (for documentation see amq_stdc_fsms.xml)
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    message_fsm_context_t *context
    )
{
    /*  Does nothing so far                                                  */
    return APR_SUCCESS;
}

inline static apr_status_t do_receive_fragment (
    message_fsm_context_t *context,
    char                  *body,
    qbyte                 body_size
    )
{
    fragment_list_item_t
        *fragment = (fragment_list_item_t*) body;

    /*  Fill in fragment attributes                                          */
    fragment->data = body + FRAGMENT_HEADER_SIZE;
    fragment->data_size = body_size;
    
    /*  Insert fragment into the fragment list                               */
    fragment->next = NULL;
    fragment->prev = context->last_fragment;
    if (context->last_fragment)
        context->last_fragment->next = fragment;
    context->last_fragment = fragment;
    if (!context->first_fragment)
        context->first_fragment = fragment;

    /*  If there was no current position beforehand, set it to the           */
    /*  beginning of the stream                                              */
    if (!context->current_fragment) {
        context->current_fragment = fragment;
        context->current_pos = 0;
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_ignore_fragment (
    message_fsm_context_t *context,
    char                  *body,
    qbyte                 body_size
    )
{
    /*  Data were received that nobody wants to read; deallocate it          */
    amq_free ((void*) body);
    return APR_SUCCESS;
}

inline static apr_status_t do_read (
    message_fsm_context_t *context,
    char                  *destination,
    qbyte                 size,
    qbyte                 *out_size
    )
{
    qbyte
        left_in_fragment;
    qbyte
        to_read;
    qbyte
        already_read = 0;

    while (1) {
        if (!context->current_fragment)
            break;
        left_in_fragment =
            context->current_fragment->data_size - context->current_pos;
        to_read = left_in_fragment < size ? left_in_fragment : size;
        if (destination)
            memcpy (destination + already_read,
                context->current_fragment->data +
                context->current_pos, to_read);
        context->current_pos += to_read;
        size -= to_read;
        already_read += to_read;
        if (!size)
            break;
        context->current_fragment = context->current_fragment->next;
        context->current_pos = 0;
    }

    if (out_size)
        *out_size = already_read;

    return APR_SUCCESS;
}

inline static apr_status_t do_term (
    message_fsm_context_t *context
    )
{
    /*  TODO: cleanup fragment list, release all locks waiting on the object */
    /*  etc.                                                                 */
    return APR_SUCCESS;
}
