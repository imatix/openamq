/*---------------------------------------------------------------------------
 *  amq_stdc_connection.c - Definition of CONNECTION object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_connection.h"
#include "connection_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_channel_list_item_t
{
    channel_t
        channel;
    apr_uint16_t
        channel_id;
    struct tag_channel_list_item_t
        *next;
} channel_list_item_t;

#define CONNECTION_OBJECT_ID context->id

DEFINE_CONNECTION_CONTEXT_BEGIN
    apr_socket_t
        *socket;                    /*  Connection socket                    */
    apr_uint16_t
        id;                         /*  Connection id - not a part of AMQP   */
                                    /*  protocol - to be used for debugging  */
                                    /*  purposes                             */
    channel_list_item_t
        *channels;                  /*  Linked list of all channels          */
    apr_thread_t
        *receiver;                  /*  Receiver thread                      */
    apr_byte_t
        async;                      /*  If 0, confirmation for connection    */
                                    /*  opening was requested by client      */
    apr_uint16_t
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for CONNECTION CLOSE                 */
    char
        *host;                      /*  Virtual host to connect to           */
    char
        *client_name;               /*  Client name to be used               */
    apr_byte_t
        stop;                       /*  When 1, threads associated with      */
                                    /*  connection should terminate          */
    global_t
        global;                     /*  Pointer to global object             */
DEFINE_CONNECTION_CONTEXT_END

inline static apr_status_t do_construct (
    connection_context_t *context
    )
{
    context->shutdown_tag = 0;
    context->channels = NULL;
    context->receiver = NULL;
    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    connection_context_t *context
    )
{
    apr_status_t
        result;
    apr_status_t
        thread_result;
    char
        buffer [BUFFER_SIZE];

    /*  Wait while theads associated with connection stop                    */
    if (context->receiver) {
        result = apr_thread_join (&thread_result, context->receiver);
        TEST(result, apr_thread_join, buffer)
        TEST(thread_result, receiver_thread, buffer)
    }
    /*  Remove connection from global list                                   */
    result = global_remove_connection (context->global,
        (connection_t) context);
    TEST(result, global_remove_connection, buffer)
    /* ... do deallocation properly ... */
    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Helper functions (private)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: s_receiver_thread

    Synopsis:
    Thread body for receiver. Receives commands from server and
    dispatches them to apropriate state machines.
    
    Arguments:
        thread              thread handle
        data                corresponding connection object handle
    -------------------------------------------------------------------------*/
static void *s_receiver_thread (
    apr_thread_t  *thread,
    void          *data
    )
{
   apr_status_t
       result;
   char
       buffer [BUFFER_SIZE];
   connection_context_t
       *context = (connection_context_t*) data;
   amqp_frame_t
       frame;
   channel_t
       channel;
   handle_t
       handle;
   
   while (1)
   {
       /*  This is the single point where data are read from socket          */
       /*  Therefore there's no need to synchronise it                       */
       result = amqp_recv_struct (context->socket, buffer, BUFFER_SIZE, &frame);   
       TEST(result, amqp_recv_struct, buffer);
       
       /*  Signalise that frame was received from server to heartbeat thread */
       /*  ... to be done ... */
     
       /*  Dispatch the frame                                                */
       switch (frame.type)
       {
       case amqp_connection_challenge_type:
           result = connection_challenge (context);
           TEST(result, connection_challenge, buffer);
           break;
       case amqp_connection_tune_type:
           result = connection_tune (context);
           TEST(result, connection_tune, buffer);
           break;
       case amqp_connection_ping_type:
           /*  special handling - to be done  */
           break;
       case amqp_connection_reply_type:
           result = connection_reply (context, 
                frame.fields.connection_reply.confirm_tag,
                frame.fields.connection_reply.reply_code,
                frame.fields.connection_reply.reply_text);
           TEST(result, connection_reply, buffer)
           break;
       case amqp_connection_close_type:
           result = connection_close (context);
           TEST(result, connection_close, buffer)
           break;
       case amqp_channel_reply_type:
           result =connection_get_channel (
               context, frame.fields.channel_reply.channel_id, &channel);
           TEST(result, connection_get_channel, buffer)
           result = channel_reply (channel, 
                frame.fields.channel_reply.confirm_tag,
                frame.fields.channel_reply.reply_code,
                frame.fields.channel_reply.reply_text);
           TEST(result, channel_reply, buffer)
           break;
       case amqp_channel_close_type:
           result = connection_get_channel (context,
               frame.fields.channel_reply.channel_id, &channel);
           TEST(result, connection_get_channel, buffer)
           result = channel_close (channel);
           TEST(result, channel_close, buffer)
           break;
       case amqp_handle_created_type:
           result =connection_get_handle (
               context, frame.fields.handle_created.handle_id, &handle);
           TEST(result, channel_get_handle, buffer)
           result = handle_created (handle,
               frame.fields.handle_created.dest_name);
           TEST(result, handle_created, buffer)
           break;
       case amqp_handle_notify_type:
           result =connection_get_handle (
               context, frame.fields.handle_notify.handle_id, &handle);
           TEST(result, channel_get_handle, buffer)
           result = handle_receive_message (handle, &frame);
           TEST(result, handle_receive_message, buffer)
           break;
       case amqp_handle_index_type:
           result =connection_get_handle (
               context, frame.fields.handle_index.handle_id, &handle);
           TEST(result, channel_get_handle, buffer)
           result = handle_index (handle,
               frame.fields.handle_index.message_nbr,
               frame.fields.handle_index.message_list);
           TEST(result, handle_index, buffer)
           break;
       case amqp_handle_prepare_type:
           break;
       case amqp_handle_ready_type:
           break;
       case amqp_handle_reply_type:
           result =connection_get_handle (
               context, frame.fields.handle_reply.handle_id, &handle);
           TEST(result, channel_get_handle, buffer)
           result = handle_reply (handle,
                frame.fields.handle_reply.confirm_tag,
                frame.fields.handle_reply.reply_code,
                frame.fields.handle_reply.reply_text);
           TEST(result, handle_reply, buffer)
           break;
       case amqp_handle_close_type:
           result = connection_get_handle (context,
               frame.fields.handle_reply.handle_id, &handle);
           TEST(result, channel_get_handle, buffer)
           result = handle_close (handle);
           TEST(result, handle_close, buffer)
           break;
       default:
           printf ("Unexpected frame type received from server.\n");
           exit (1);
       }

       /*  Conection requested to stop                                       */
       if (context->stop == 1)
           break;
   }

   /*  cleanup in case of async closing ?  */

   return NULL;

}

/*  -------------------------------------------------------------------------
    Function: s_open_socket

    Synopsis:
    Opens socket to specified server.

    Arguments:
        context             connection object
        server              server to open connection to
    -------------------------------------------------------------------------*/
static apr_status_t s_open_socket (
    connection_context_t  *context,
    const char            *server
    )
{
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer[APRMAXHOSTLEN + 6];      /*  Buffer to build hostname in      */
    char
        *addr = NULL;                   /*  Contains hostname                */
    char
        *scope_id = NULL;               /*  Contains scope id                */
    apr_port_t
        port;                           /*  APR port object                  */
    apr_sockaddr_t
        *sockaddr = NULL;               /*  APR socket address object        */

    sprintf (buffer, "%s:7654", server);
    result = apr_parse_addr_port (&addr, &scope_id, &port,
        buffer, context->pool);
    TEST(result, apr_parse_addr_port, buffer)
    result = apr_sockaddr_info_get (&sockaddr, addr, AF_UNSPEC, port,
        APR_IPV4_ADDR_OK, context->pool);
    TEST(result, apr_socket_info_get, buffer)
    result = apr_socket_create ( &(context->socket), APR_INET, SOCK_STREAM,
        APR_PROTO_TCP, context->pool);
    TEST(result, apr_socket_create, buffer)
    result = apr_socket_connect (context->socket, sockaddr);
    TEST(result, apr_socket_connect, buffer)

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  Helper functions (public)
 *---------------------------------------------------------------------------*/

/*  -------------------------------------------------------------------------
    Function: connection_get_channel

    Synopsis:
    Converts channel id into channel handle.
    Used by command dispatcher to redirect AMQP channelcommands to apropriate
    Level 1 channel object.

    Arguments:
        ctx                 connection handle
        channel_id          id to map
        channel             out parameter; mapped channel
    -------------------------------------------------------------------------*/    
apr_status_t connection_get_channel (
    connection_t  ctx,
    apr_uint16_t  channel_id,
    channel_t     *channel
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    connection_context_t
        *context = (connection_context_t*) ctx;
    channel_list_item_t
        *item;

    result = connection_sync_begin (context);
    TEST(result, connection_sync_begin, buffer);

    item = context->channels;
    while (1) {
        if (!item) {
            if (channel) *channel = NULL;
            break;
        }
        if (item->channel_id == channel_id) {
            if (channel) *channel = item->channel;
            break;
        }
        item = item->next;
    }

    result = connection_sync_end (context);
    TEST(result, connection_sync_end, buffer);

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: connection_get_handle

    Synopsis:
    Converts handle id into handle handle.
    Used by command dispatcher to redirect AMQP handle commands to apropriate
    Level 1 handle object.

    Arguments:
        ctx                connection handle
        handle_id          id to map
        handle             out parameter; mapped handle
    -------------------------------------------------------------------------*/
apr_status_t connection_get_handle (
    connection_t  ctx,
    apr_uint16_t  handle_id,
    handle_t      *handle
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    connection_context_t
        *context = (connection_context_t*) ctx;
    channel_list_item_t
        *item;

    result = connection_sync_begin (context);
    TEST(result, connection_sync_begin, buffer);

    item = context->channels;
    while (1) {
        if (!item) {
            if (handle) *handle = NULL;
            break;
        }
        channel_get_handle (item->channel, handle_id, handle);
        if (*handle)
            break;
        item = item->next;
    }

    result = connection_sync_end (context);
    TEST(result, connection_sync_end, buffer);

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    connection_context_t  *context,
    global_t              global,
    apr_uint16_t          connection_id,
    const char            *server,
    const char            *host,
    const char            *client_name,
    apr_byte_t            async,
    lock_t                *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    apr_threadattr_t
        *threadattr;

    /*  Store info that will be needed later                                 */
    context->global = global;
    context->id = connection_id;
    context->async = async;
    context->host = apr_palloc (context->pool, strlen (host));
    if (context->host == NULL) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    strcpy (context->host, host);
    context->client_name = apr_palloc (context->pool,
        strlen (client_name));
    if (context->client_name == NULL) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    strcpy (context->client_name, client_name);
    /*  Open socket                                                          */
    result = s_open_socket (context, server);
    TEST(result, open_socket, buffer);
    /*  Run receiver thread                                                  */
    result = apr_threadattr_create (&threadattr, context->pool);
    TEST(result, apr_threadattr_create, buffer)
    result = apr_thread_create (&(context->receiver),
        threadattr, s_receiver_thread, (void*) context, context->pool);
    TEST(result, apr_thread_create, buffer)
    /*  Run sender thread                                                    */
    /*  ... to be done ...  */
    /*  Register that we will be waiting for connection open completion      */
    /*  No need to store lock_id, it's always equal to 1                     */
    result = register_lock (context->global, context->id, 0, 0, NULL, lock) ;
    /*  Send initiation bytes                                                */
    result = amqp_init (context->socket);
    TEST(result, amqp_init, buffer);
    return APR_SUCCESS;
}

inline static apr_status_t do_challenge (
    connection_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    result = amqp_connection_response (context->socket, buffer,
        BUFFER_SIZE, "plain", 0, "");
    TEST(result, amqp_connection_response, buffer)
    return APR_SUCCESS;
}

inline static apr_status_t do_tune (
    connection_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    result = amqp_connection_open ( context->socket, buffer, BUFFER_SIZE,
        context->async ? 0 : 1, context->host, context->client_name, 0, "");
    TEST(result, amqp_connection_open, buffer);

    /* If client doesn't require confirmation, we still have to wait till    */
    /* client-server handshaking is completed                                */
    if (context->async)
        release_lock (context->global, 1, (void*) context);
    return APR_SUCCESS;
}

inline static apr_status_t do_create_channel (
    connection_context_t  *context,
    apr_byte_t            transacted,
    apr_byte_t            restartable,
    apr_byte_t            async,
    lock_t                *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];
    channel_t
        channel;
    channel_list_item_t
        *item;
    apr_uint16_t
        channel_id;

    /*  Assign new channel id                                                */
    result = global_assign_new_channel_id (context->global, &channel_id);
    TEST(result, assign_new_channel_id, buffer)
    /*  Create channel object                                                */
    result = channel_create (&channel);
    TEST(result, channel_create, buffer)
    /*  Insert it into the list of existing channels                         */
    item = (channel_list_item_t*)
        amq_malloc (sizeof (channel_list_item_t));
    if (!item) {
        printf ("Not enough memory.\n");
        exit (1);
    }
    item->channel = channel;
    item->channel_id = channel_id;
    item->next = context->channels;
    context->channels = item;
    /*  Start it                                                             */
    result = channel_init (channel, context->global, (connection_t) context,
        context->socket, context->id, channel_id, transacted, restartable,
        async, lock);
    TEST(result, channel_init, buffer)
    return APR_SUCCESS;    
}

inline static apr_status_t do_remove_channel (
    connection_t  ctx,
    channel_t     channel
    )
{
    connection_context_t
        *context = (connection_context_t*) ctx;
    channel_list_item_t
        *item;
    channel_list_item_t
        **prev;

    item = context->channels;
    prev = &(context->channels);
    while (1) {
        if (!item) {
            printf ("Channel specified in connection_remove_channel "
                "doesn't exist.\n");
            exit (1);
        }
        if (item->channel == channel) {
            *prev = item->next;
            amq_free (item);
            break;
        }
        prev = &(item->next);
        item = item->next;
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_reply (
    connection_context_t  *context,
    apr_uint16_t          confirm_tag,
    apr_uint16_t          reply_code,
    const char            *reply_text
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    result = release_lock (context->global, confirm_tag, (void*) context);
    TEST(result, release_lock, buffer)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    connection_context_t  *context,
    lock_t                *lock
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Close handles, rollback transactions, close channels                 */
    /*  ... to be done...  */
    /*  Send CONNECTION CLOSE                                                */
    result = register_lock (context->global, context->id, 0, 0,
        &(context->shutdown_tag), lock);
    TEST(result, register_lock, buffer)
    result = amqp_connection_close (context->socket, buffer, BUFFER_SIZE,
        200, "Connection close requested by client");
    TEST(result, amqp_connection_close, buffer)
    return APR_SUCCESS;    
}

inline static apr_status_t do_duplicate_terminate (
    connection_context_t  *context,
    lock_t                *lock
    )
{
    /*  This must be an explicit error; if not defined, it would fall        */
    /*  through default state and send CONNECTION CLOSE once more            */
    printf ("Connection is already being terminated.\n");
    exit (1);
    return APR_SUCCESS;    
}

inline static apr_status_t do_client_requested_close (
    connection_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Stop connection threads                                              */
    context->stop = 1;
    /*  Resume with error all waiting threads except one requesting          */
    /*  connection termination                                               */
    result = release_all_locks (context->global, context->id, 0, 0,
        context->shutdown_tag, AMQ_OBJECT_CLOSED);
    TEST(result, release_all_locks, buffer)
    /*  Resume client thread waiting for connection termination              */
    result = release_lock (context->global, context->shutdown_tag, NULL);
    TEST(result, release_lock, buffer)
    return APR_SUCCESS;         
}

inline static apr_status_t do_server_requested_close (
    connection_context_t  *context
    )
{
    apr_status_t
        result;
    char
        buffer [BUFFER_SIZE];

    /*  Server request connection shutdown                                   */
    /*  Resume with error all waiting threads                                */
    result = release_all_locks (context->global, context->id, 0, 0, 0,
        AMQ_OBJECT_CLOSED);
    TEST(result, release_all_locks, buffer)
    /*  Close handles, rollback transactions, close channels                 */
    /*  ... to be done...  */
    /*  Reply with CONNECTION CLOSE                                          */
    result = amqp_connection_close (context->socket, buffer, BUFFER_SIZE,
        200, "Connection close requested by client");
    TEST(result, amqp_connection_close, buffer)
    /*  Shutdown connection threads                                          */
    context->stop = 1;
    return APR_SUCCESS;         
}
