/*---------------------------------------------------------------------------
 *  amqp_global.c - Definition of GLOBAL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amqp_global.h"
#include "global_fsm.d"

/*****************************************************************************/
/* Globals                                                                   */

apr_byte_t global_context_exists = 0;

/*****************************************************************************/
/* State machine definitions                                                 */

/*  Be aware ! There's no point in using locks as a return values from       */
/*  functions on global level. Lock has it's meaning only within a           */
/*  connection.                                                              */

typedef struct tag_connection_list_item_t
{
    connection_t
        connection;
    struct tag_connection_list_item_t
        *next;
} connection_list_item_t;

#define GLOBAL_OBJECT_ID 0

DEFINE_GLOBAL_CONTEXT_BEGIN
    connection_list_item_t
        *connections;               /*  Linked list of all connections       */
DEFINE_GLOBAL_CONTEXT_END

DEFINE_GLOBAL_CONSTRUCTOR_BEGIN
DEFINE_GLOBAL_CONSTRUCTOR_MIDDLE
    if (global_context_exists) {
        printf ("Global context already exists.\n");
        exit (1);
    }
    context->connections = NULL;
    global_context_exists = 1;
DEFINE_GLOBAL_CONSTRUCTOR_END

DEFINE_GLOBAL_DESTRUCTOR_BEGIN
DEFINE_GLOBAL_DESTRUCTOR_MIDDLE
    global_context_exists = 0;
DEFINE_GLOBAL_DESTRUCTOR_END

/*****************************************************************************/
/*  Helper functions (public)                                                */

/*  -------------------------------------------------------------------------
    Function: global_remove_connection

    Synopsis:
    Removes connection from global object's list of opened connections.
    Called by connection during shutdown.

    Arguments:
        ctx                 global object handle
        connection          connection to remove
    -------------------------------------------------------------------------*/
apr_status_t global_remove_connection (
    global_t ctx,
    connection_t connection
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    global_context_t
        *context = (global_context_t*) ctx;
    connection_list_item_t
        *item;
    connection_list_item_t
        **prev;

    result = global_sync_begin (context);
    TEST(result, global_sync_begin, buffer);
    item = context->connections;
    prev = &(context->connections);
    while (1) {
        if (!item) {
            printf ("Connection specified in global_remove_connection doesn't exist.\n");
            exit (1);
        }
        if (item->connection == connection) {
            *prev = item->next;
            free (item);
            break;
        }
        prev = &(item->next);
        item = item->next;
    }
    result = global_sync_end (context);
    TEST(result, global_sync_end, buffer);

    return APR_SUCCESS;
}

/*****************************************************************************/
/*  State machine actions                                                    */

inline static apr_status_t do_init (
    global_context_t *context
    )
{
    /*  Does nothing for now, just switches the state                        */
    return APR_SUCCESS;
}

inline static apr_status_t do_create_connection (
    global_context_t *context,
    const char *server,
    const char *host,
    const char *client_name,
    apr_byte_t async,
    amqp_lock_t *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    amqp_connection_t
        connection;
    connection_list_item_t
        *item;

    /* Create connection object                                              */
    result = connection_create (&connection);
    TEST(result, connection_create, buffer)
    /* Add it into the linked list                                           */
    item = (connection_list_item_t*)
        malloc (sizeof (connection_list_item_t));
    if (!item) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    item->connection = connection;
    item->next = context->connections;
    context->connections = item;
    /*  Start it                                                             */
    result = connection_init (connection, (global_t) context,
        server, host, client_name, async, lock);
    TEST(result, connection_init, buffer)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    global_context_t *context,
    amqp_lock_t *lock
    )
{
    /*  Break all the existing locks                                         */
    /*  Disable creation of new locks                                        */
    /*  Wait till there are no threads accessing API                         */
    /*  Wait till all connections are decently closed                        */
    /*  Return some kind of global lock                                      */
    if (lock)
        *lock = NULL; /* ... */
    return APR_SUCCESS;
}

inline static apr_status_t do_duplicate_terminate (
    global_context_t *context,
    amqp_lock_t *lock
    )
{
    /*  Maybe duplicate attemp to terminate woudn't be deadly in this case   */
    /*  ... to be rethought ...                                              */
    printf ("Global object is already being terminated.\n");
    exit (1);
    return APR_SUCCESS; 
}
