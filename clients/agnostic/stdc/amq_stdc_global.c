/*---------------------------------------------------------------------------
 *  amq_stdc_global.c - Definition of GLOBAL object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_global.h"
#include "global_fsm.d"

/*---------------------------------------------------------------------------
 *  Globals
 *---------------------------------------------------------------------------*/

apr_byte_t global_context_exists = 0;

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_lock_context_t
{
    apr_thread_mutex_t
        *mutex;
    apr_uint16_t
        lock_id;
    apr_uint16_t
        connection_id;
    apr_uint16_t
        channel_id;
    apr_uint16_t
        handle_id;
    void
        *result;
    apr_status_t
        error;
    struct tag_lock_context_t *next;
} lock_context_t;

typedef struct tag_connection_list_item_t
{
    connection_t
        connection;
    struct tag_connection_list_item_t
        *next;
} connection_list_item_t;

#define GLOBAL_OBJECT_ID 0

DEFINE_GLOBAL_CONTEXT_BEGIN
    apr_uint16_t
        last_lock_id;               /*  Last lock id used                    */
    apr_uint16_t
        last_connection_id;         /*  Last connection id used - connection */
                                    /*  id is not a part of AMQP protocol    */
                                    /*  To be used for debugging purposes    */
    apr_uint16_t
        last_channel_id;            /*  Last channel id used                 */
    apr_uint16_t
        last_handle_id;             /*  Last handle id used                  */
    lock_context_t
        *locks;                     /*  Linked list of existing locks        */
    connection_list_item_t
        *connections;               /*  Linked list of all connections       */
DEFINE_GLOBAL_CONTEXT_END

inline static apr_status_t do_construct (
    global_context_t *context
    )
{
    if (global_context_exists) {
        printf ("Global context already exists.\n");
        exit (1);
    }
    context->last_lock_id = 0;
    context->last_connection_id = 0;
    context->last_channel_id = 0;
    context->last_handle_id = 0;
    context->connections = NULL;
    global_context_exists = 1;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    global_context_t *context
    )
{
    global_context_exists = 0;
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: register_lock

    Synopsis:
    Creates lock that can be waited for.

    Arguments:
        ctx                 global object handle
        connection_id       connection owning the lock; when shuting down the
                            connection, lock will be released; if 0,
                            lock doesn't belong to any connection
        channel_id          channel owning the lock; when shuting down the
                            channel, lock will be released; if 0, lock doesn't
                            belong to any channel
        handle_id           handle owning the lock; when shuting down the
                            handle, lock will be released; if 0, lock doesn't
                            belong to any handle
        lock_id             out parameter, lock id, unique within the
                            connection; may be used as confirm tag
        lock                out parameter; newly created lock
    -------------------------------------------------------------------------*/
apr_status_t register_lock (
    global_t      ctx,
    apr_uint16_t  connection_id,
    apr_uint16_t  channel_id,
    apr_uint16_t  handle_id,
    apr_uint16_t  *lock_id,
    lock_t        *lock
    )
{
    apr_status_t
        result;
    lock_context_t
        *temp;
    char
        buffer [BUFFER_SIZE];
    global_context_t
        *context = (global_context_t*) ctx;
    apr_uint16_t
        id;

    result = global_sync_begin (context);
    TEST(result, global_sync_begin, buffer)
    id = ++context->last_lock_id;
    temp = context->locks;
    context->locks = amq_malloc (sizeof (lock_context_t));
    if (context->locks == NULL)
        exit (1);
    result = apr_thread_mutex_create (&(context->locks->mutex),
        APR_THREAD_MUTEX_UNNESTED, context->pool); /*  Not in pool !!! */
    TEST(result, apr_thread_mutex_create, buffer)
    result = apr_thread_mutex_lock (context->locks->mutex);
    TEST(result, apr_thread_mutex_lock, buffer)
    context->locks->lock_id = id;
    context->locks->connection_id = connection_id;
    context->locks->channel_id = channel_id;
    context->locks->handle_id = handle_id;
    context->locks->next = temp;
    context->locks->result = NULL;
    context->locks->error = APR_SUCCESS;
    result = global_sync_end (context);
    TEST(result, global_sync_end, buffer)

    if (lock) *lock = (lock_t) (context->locks);
    if (lock_id) *lock_id = id;
#   ifdef AMQTRACE_LOCKS
        printf ("# Lock %ld registered.\n", (long)id);
#   endif
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: release_lock

    Synopsis:
    Releases a lock, so that the thread waiting for it can be resumed.

    Arguments:
        ctx                 global object handle
        lock_id             id of lock to release
        res                 generic handle that will be passed to thread
                            waiting for the lock
    -------------------------------------------------------------------------*/
apr_status_t release_lock (
    global_t      ctx,
    apr_uint16_t  lock_id,
    void          *res
    )
{
    lock_context_t
        *temp,
        **last;
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    global_context_t
        *context = (global_context_t*) ctx;

    result = global_sync_begin (context);
    TEST(result, global_sync_begin, buffer)
    temp = (lock_context_t*) (context->locks);
    last = (lock_context_t**) &(context->locks);
    while (1) {
        if (!temp) {
            /*  Confirmation that nobody is waiting for arrived. Why?        */
            printf ("Unexpected confirmation arrived !\n");
            exit (1);
        }
        /*  Confirmation that someone is waiting for arrived.                */
        if (temp->lock_id == lock_id) {
            /*  Remove item from the linked list                             */
            *last = temp->next;
            /*  Add result value to the lock                                 */
            temp->result = res;
            /*  Resume execution of waiting thread                           */
#           ifdef AMQTRACE_LOCKS
                printf ("# Lock %ld released.\n", (long)lock_id);
#           endif
            result = apr_thread_mutex_unlock (temp->mutex);
            if (!temp) {
                printf ("Unexpected confirmation arrived !\n");
                exit (1);
            }
            break;
        }
        last = &(temp->next);
        temp = temp->next;
    }
    result = global_sync_end (context);
    TEST(result, global_sync_end, buffer)
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: wait_for_lock

    Synopsis:
    Waits till lock is released.

    Arguments:
        lck                 lock object to be waited for
        res                 out parameter; generic handle provided by thread
                            releasing the lock
    -------------------------------------------------------------------------*/
apr_status_t wait_for_lock (
    lock_t  lck,
    void    **res
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    lock_context_t
        *lock = (lock_context_t*) lck;

    /*  No lock - no problem                                                 */
    if (!lck) {
        if (res) *res = NULL;
        return APR_SUCCESS;
    }

#   ifdef AMQTRACE_LOCKS
        printf ("# Waiting for lock %ld beginning.\n", (long) (lock->lock_id));
#   endif
    if (lock->mutex) {
        /*  Wait till requested confirmation is received                     */
        result = apr_thread_mutex_lock (lock->mutex);
        TEST(result, apr_thread_mutex_lock, buffer)
    }
#   ifdef AMQTRACE_LOCKS
        printf ("# Waiting for lock %ld ended.\n", (long) (lock->lock_id));
#   endif
    if (lock->error != APR_SUCCESS) {
        result = lock->error;
        if (res)
            *res = NULL;
    }
    else {
        if (res)
            *res = lock->result;
    }
    /*  TODO : free resources... destroy (mutex if not external) destroy(pool_local)  etc. */
    amq_free ((void*) lock);
    return result;
}

/*  -------------------------------------------------------------------------
    Function: release_all_locks

    Synopsis:
    Releases all locks associated with specified connection, channel or handle.

    Arguments:
        ctx                 global object handle
        connection_id       release all locks associated with this connection;
                            if 0, does nothing
        channel_id          release all locks associated with this channel;
                            if 0, does nothing
        handle_id           release all locks associated with this handle;
                            if 0, does nothing
    -------------------------------------------------------------------------*/    
apr_status_t release_all_locks (
    global_t      ctx,
    apr_uint16_t  connection_id,
    apr_uint16_t  channel_id,
    apr_uint16_t  handle_id,
    apr_uint16_t  except_lock_id,
    apr_status_t  error
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    global_context_t
        *context = (global_context_t*) ctx;
    lock_context_t
        *lock;

    result = global_sync_begin (context);
    TEST(result, global_sync_begin, buffer)

    lock = context->locks;
    while (lock) {
        if (((connection_id == 0 && channel_id == 0 && handle_id == 0) ||
              (lock->connection_id == connection_id) ||
              (lock->channel_id == channel_id) ||
              (lock->handle_id == handle_id)) &&
              lock->lock_id != except_lock_id) {
            lock->error = error;
            result = apr_thread_mutex_unlock (lock->mutex);
            TEST(result, apr_thread_mutex_unlock, buffer);
            /* deallocate resources; if not external, destroy mutex etc. */
        }
        lock = lock->next;
    }

    result = global_sync_end (context);
    TEST(result, global_sync_end, buffer)
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    global_context_t  *context
    )
{
    /*  Does nothing for now, just switches the state                        */
    return APR_SUCCESS;
}

inline static apr_status_t do_create_connection (
    global_context_t  *context,
    const char        *server,
    const char        *host,
    const char        *client_name,
    apr_byte_t        async,
    lock_t            *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    connection_t
        connection;
    connection_list_item_t
        *item;

    /* Create connection object                                              */
    result = connection_create (&connection);
    TEST(result, connection_create, buffer)
    /* Add it into the linked list                                           */
    item = (connection_list_item_t*)
        amq_malloc (sizeof (connection_list_item_t));
    if (!item) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    item->connection = connection;
    item->next = context->connections;
    context->connections = item;
    /*  Start it                                                             */
    result = connection_init (connection, (global_t) context,
        ++(context->last_connection_id), server, host, client_name,
        async, lock);
    TEST(result, connection_init, buffer)
    return APR_SUCCESS;
}

inline static apr_status_t do_remove_connection (
    global_t      ctx,
    connection_t  connection
    )
{
    global_context_t
        *context = (global_context_t*) ctx;
    connection_list_item_t
        *item;
    connection_list_item_t
        **prev;

    item = context->connections;
    prev = &(context->connections);
    while (1) {
        if (!item) {
            printf ("Connection specified in global_remove_connection "
                "doesn't exist.\n");
            exit (1);
        }
        if (item->connection == connection) {
            *prev = item->next;
            amq_free (item);
            break;
        }
        prev = &(item->next);
        item = item->next;
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_assign_new_handle_id (
    global_t      ctx,
    apr_uint16_t  *handle_id
    )
{
    global_context_t
        *context = (global_context_t*) ctx;

    context->last_handle_id++;
    if (handle_id) *handle_id = context->last_handle_id;

    return APR_SUCCESS;
}

inline static apr_status_t do_assign_new_channel_id (
    global_t      ctx,
    apr_uint16_t  *channel_id
    )
{
    global_context_t
        *context = (global_context_t*) ctx;

    context->last_channel_id++;
    if (channel_id) *channel_id = context->last_channel_id;

    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    global_context_t  *context,
    lock_t            *lock
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
    global_context_t  *context,
    lock_t            *lock
    )
{
    /*  Maybe duplicate attemp to terminate woudn't be deadly in this case   */
    /*  ... to be rethought ...                                              */
    printf ("Global object is already being terminated.\n");
    exit (1);
    return APR_SUCCESS; 
}
