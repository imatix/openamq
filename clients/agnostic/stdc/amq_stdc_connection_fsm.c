/*---------------------------------------------------------------------------
 *  amq_stdc_connection.c - Definition of CONNECTION object 
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_connection_fsm.h"
#include "amq_stdc_connection_fsm.d"

/*---------------------------------------------------------------------------
 *  State machine definitions
 *---------------------------------------------------------------------------*/

typedef struct tag_channel_list_item_t
{
    channel_fsm_t
        channel;
    dbyte
        channel_id;
    struct tag_channel_list_item_t
        *next;
} channel_list_item_t;

typedef struct tag_chunk_list_item_t
{
    byte
        send_ping;                     /*  Set, whan sender thread has to    */
                                       /*  send PING to server; filled in    */
                                       /*  when chank is passed to sender    */
                                       /*  thread                            */
    char
        *data;                         /*  Data to send                      */
    qbyte
        size;                          /*  Number of bytes to send           */
    dbyte
        lock_id;                       /*  Id of lock waiting while chunk    */
                                       /*  is sent; if 0, no lock is waiting */
    struct tag_chunk_list_item_t
        *next;
} chunk_list_item_t;

#define CONNECTION_FSM_OBJECT_ID context->id

DEFINE_CONNECTION_FSM_CONTEXT_BEGIN
    apr_socket_t
        *socket;                    /*  Connection socket                    */
    dbyte
        id;                         /*  Connection id - not a part of AMQP   */
                                    /*  protocol - to be used for debugging  */
                                    /*  purposes                             */
    channel_list_item_t
        *channels;                  /*  Linked list of all channels          */
    apr_thread_t
        *receiver;                  /*  Receiver thread                      */
    apr_thread_t
        *sender;                    /*  Sender thread                        */
    byte
        async;                      /*  If 0, confirmation for connection    */
                                    /*  opening was requested by client      */
    dbyte
        shutdown_tag;               /*  Id of lock that's used for waiting   */
                                    /*  for CONNECTION CLOSE                 */
    char
        *host;                      /*  Virtual host to connect to           */
                                    /*  No terminating zero                  */
    byte
        host_size;                  /*  Size of 'host' string                */
    char
        *client_name;               /*  Client name to be used               */
                                    /*  No terminating zero                  */
    byte
        client_name_size;           /*  Size of 'client_name' string         */
    amq_stdc_table_t
        options;                    /*  Options table to be used             */
    dbyte
        open_tag;                   /*  Lock id that is used to wait while   */
                                    /*  connection is opened                 */
    chunk_list_item_t
        *first_chunk;               /*  First chunk of chunk linked list     */
    chunk_list_item_t
        *last_chunk;                /*  Last chunk of chunk linked list      */
    byte
        send_ping;                  /*  If 1 PING is to be sent to server    */
    apr_uint16_t
        sender_tag;                 /*  Lock sender thread uses to wait on   */
                                    /*  when it has nothing to do            */
    byte
        stop;                       /*  When 1, threads associated with      */
                                    /*  connection should terminate          */
    global_fsm_t
        global;                     /*  Pointer to global object             */
DEFINE_CONNECTION_FSM_CONTEXT_END

inline static apr_status_t do_construct (
    connection_fsm_context_t  *context
    )
{
    context->shutdown_tag = 0;
    context->channels = NULL;
    context->receiver = NULL;
    context->host = NULL;
    context->host_size = 0;
    context->client_name = NULL;
    context->client_name_size = 0;
    context->options = NULL;
    context->first_chunk = NULL;
    context->last_chunk = NULL;
    context->send_ping = 0;
    context->sender_tag = 0;

    return APR_SUCCESS;
}

inline static apr_status_t do_destruct (
    connection_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    apr_status_t
        thread_result;

    /*  Wait while theads associated with connection stop                    */
    if (context->receiver) {
        result = apr_thread_join (&thread_result, context->receiver);
        AMQ_ASSERT_STATUS (result, apr_thread_join)
        AMQ_ASSERT_STATUS (thread_result, receiver_thread)
        result = apr_thread_join (&thread_result, context->sender);
        AMQ_ASSERT_STATUS (result, apr_thread_join)
        AMQ_ASSERT_STATUS (thread_result, receiver_thread)
    }

    /*  Remove connection from global list                                   */
    result = global_fsm_remove_connection (context->global, context);
    AMQ_ASSERT_STATUS (result, global_fsm_remove_connection)

    if (context->options) {
        result = amq_stdc_table_destroy (context->options);
        AMQ_ASSERT_STATUS (result, amq_stdc_table_destroy)
    }

    /*  TODO: do deallocation properly                                       */
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
   connection_fsm_context_t
       *context = (connection_fsm_context_t*) data;
   channel_fsm_t
       channel;
   handle_fsm_t
       handle;
   apr_size_t
       size;
   char
       command_buffer [COMMAND_BUFFER_SIZE];
   char
       message_header_buffer [MESSAGE_HEADER_BUFFER_SIZE];
   char
       *command_buffer_ptr;
   char
       *message_header_buffer_ptr;
   amq_stdc_frame_t
       frame;
   qbyte
       command_size;
   qbyte
       header_size;
   qbyte
       body_size;
   char
       *fragment;
   dbyte
       dsize;
   qbyte
       qsize;
   
   while (1)
   {
       /*  This is the single point where data are read from socket          */
       /*  Therefore there's no need to synchronise it                       */
       size = sizeof (dbyte);
       result = apr_socket_recv (context->socket, (char*) &dsize, &size);
       AMQ_ASSERT_STATUS (result, apr_socket_recv)
       dsize = ntohs (dsize);
       if (dsize != 0xffff) {
           size = dsize;
       }
       else {
           size = sizeof (qbyte);
           result = apr_socket_recv (context->socket, (char*) &qsize, &size);
           AMQ_ASSERT_STATUS (result, apr_socket_recv)
           qsize = ntohl (qsize);
           size = qsize;
       }
       if (size > COMMAND_BUFFER_SIZE) {
           command_buffer_ptr = amq_malloc (size);
           if (!command_buffer_ptr)
               AMQ_ASSERT (Not enough memory)
       }
       else
           command_buffer_ptr = (char*) command_buffer;
       result = apr_socket_recv (context->socket, command_buffer_ptr, &size);
       AMQ_ASSERT_STATUS (result, apr_socket_recv)
       command_size = amq_stdc_decode_command (command_buffer_ptr, size,
           &frame);
       if (!command_size)
           AMQ_ASSERT (Corrupted frame received from server)
       
       /*  TODO:                                                             */
       /*  Signalise that frame was received from server to heartbeat thread */
     
       /*  Dispatch the frame                                                */
       switch (frame.type)
       {
       case amq_stdc_connection_challenge_type:
           result = connection_fsm_challenge (context,
               frame.fields.connection_challenge.version);
           AMQ_ASSERT_STATUS (result, connection_fsm_challenge);
           break;
       case amq_stdc_connection_tune_type:
           result = connection_fsm_tune (context);
           AMQ_ASSERT_STATUS (result, connection_fsm_tune);
           break;
       case amq_stdc_connection_ping_type:
           /*  TODO : special handling - to be done                          */
           break;
       case amq_stdc_connection_reply_type:
           result = connection_fsm_reply (context, 
                frame.fields.connection_reply.confirm_tag,
                frame.fields.connection_reply.reply_code,
                frame.fields.connection_reply.reply_text_size,
                frame.fields.connection_reply.reply_text);
           AMQ_ASSERT_STATUS (result, connection_fsm_reply)
           break;
       case amq_stdc_connection_close_type:
           result = connection_fsm_close (context);
           AMQ_ASSERT_STATUS (result, connection_fsm_close)
           break;
       case amq_stdc_channel_reply_type:
           result =connection_get_channel (
               context, frame.fields.channel_reply.channel_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_channel)
           result = channel_fsm_reply (channel, 
                frame.fields.channel_reply.confirm_tag,
                frame.fields.channel_reply.reply_code,
                frame.fields.channel_reply.reply_text_size,
                frame.fields.channel_reply.reply_text);
           AMQ_ASSERT_STATUS (result, channel_fsm_reply)
           break;
       case amq_stdc_channel_close_type:
           result = connection_get_channel (context,
               frame.fields.channel_reply.channel_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_channel)
           result = channel_fsm_close (channel);
           AMQ_ASSERT_STATUS (result, channel_fsm_close)
           break;
       case amq_stdc_handle_created_type:
           result =connection_get_channel_from_handle (
               context, frame.fields.handle_created.handle_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = handle_fsm_created (handle,
               frame.fields.handle_created.dest_name_size,
               frame.fields.handle_created.dest_name);
           AMQ_ASSERT_STATUS (result, handle_fsm_created)
           break;
       case amq_stdc_handle_notify_type:
           result =connection_get_handle (
               context, frame.fields.handle_notify.handle_id, &handle);
           AMQ_ASSERT_STATUS (result, connection_get_handle)

           /*  TODO: switch to another instream if out-of-band = 1           */

           /*  Get body size from stream                                     */
           size = sizeof (qbyte);
           result = apr_socket_recv (context->socket,
               (char*) message_header_buffer, &size);
           AMQ_ASSERT_STATUS (result, apr_socket_recv)
           body_size = ntohl (*((qbyte*) message_header_buffer));

           /*  Compute size of the message header and read the rest of it    */
           header_size = frame.fields.handle_notify.fragment_size - body_size;
           if (header_size > MESSAGE_HEADER_BUFFER_SIZE) {
               message_header_buffer_ptr = amq_malloc (header_size);
               if (!message_header_buffer_ptr)
                   AMQ_ASSERT (Not enough memory)
               *((qbyte*) message_header_buffer_ptr) =
                   *((qbyte*) message_header_buffer);
           }
           else
               message_header_buffer_ptr = (char*) message_header_buffer;
           size = header_size - sizeof (qbyte);
           result = apr_socket_recv (context->socket,
               message_header_buffer_ptr + sizeof (qbyte), &size);
           AMQ_ASSERT_STATUS (result, apr_socket_recv)
           
           /*  Allocate a buffer and read message content into it            */
           fragment = (char*) amq_malloc (FRAGMENT_HEADER_SIZE + body_size);
           if (!fragment)
               AMQ_ASSERT (Not enough memory)
           size = body_size;
           result = apr_socket_recv (context->socket,
               fragment + FRAGMENT_HEADER_SIZE, &size);
           AMQ_ASSERT_STATUS (result, apr_socket_receive)
           
           /*  Send event to FSM                                             */
           result = channel_fsm_receive_message (channel,
               &(frame.fields.handle_notify), message_header_buffer_ptr,
               header_size, fragment, body_size);
           AMQ_ASSERT_STATUS (result, handle_fsm_receive_message)

           break;
       case amq_stdc_handle_index_type:
           result =connection_get_handle (
               context, frame.fields.handle_index.handle_id, &handle);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = handle_fsm_index (handle,
               frame.fields.handle_index.message_nbr,
               frame.fields.handle_index.message_list_size,
               frame.fields.handle_index.message_list);
           AMQ_ASSERT_STATUS (result, handle_fsm_index)
           break;
       case amq_stdc_handle_prepare_type:
           assert (0);
           break;
       case amq_stdc_handle_ready_type:
           assert (0);
           break;
       case amq_stdc_handle_reply_type:
           result =connection_get_handle (
               context, frame.fields.handle_reply.handle_id, &handle);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = handle_fsm_reply (handle,
                frame.fields.handle_reply.confirm_tag,
                frame.fields.handle_reply.reply_code,
                frame.fields.handle_reply.reply_text_size,
                frame.fields.handle_reply.reply_text);
           AMQ_ASSERT_STATUS (result, handle_fsm_reply)
           break;
       case amq_stdc_handle_close_type:
           result = connection_get_handle (context,
               frame.fields.handle_reply.handle_id, &handle);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = handle_fsm_close (handle);
           AMQ_ASSERT_STATUS (result, handle_fsm_close)
           break;
       default:
           AMQ_ASSERT (Unexpected frame type received from server)
       }

       if (command_buffer_ptr && command_buffer_ptr != (char*) command_buffer)
           amq_free ((void*) command_buffer_ptr);
       if (message_header_buffer_ptr &&
             message_header_buffer_ptr != (char*) message_header_buffer)
           amq_free ((void*) message_header_buffer_ptr);

       /*  Conection requested to stop                                       */
       if (context->stop == 1)
           break;
   }

   return NULL;
}

/*  -------------------------------------------------------------------------
    Function: s_sender_thread

    Synopsis:
    Thread body for sender. Sends chunks of data to server.
    
    Arguments:
        thread              thread handle
        data                corresponding connection object handle
    -------------------------------------------------------------------------*/

static void *s_sender_thread (
    apr_thread_t  *thread,
    void          *data
    )
{
   apr_status_t
       result;
   connection_fsm_context_t
       *context = (connection_fsm_context_t*) data;
   lock_t
       lock;
   chunk_list_item_t
       *chunk;
   apr_size_t
       size;
   char
       ping_data [COMMAND_SIZE_MAX_SIZE +
           AMQ_STDC_CONNECTION_PING_CONSTANT_SIZE];
   qbyte
       ping_data_size;

    /*  Prepare single instance of PING command to be used repeatedly       */
    ping_data_size = amq_stdc_encode_connection_ping (ping_data,
        COMMAND_SIZE_MAX_SIZE + AMQ_STDC_CONNECTION_PING_CONSTANT_SIZE, 0);
    if (!ping_data_size)
        AMQ_ASSERT (Framing error)

    while (1) {

       result = connection_fsm_get_chunk (context, &lock);
       AMQ_ASSERT_STATUS (result, connection_fsm_get_chunk)
       result = wait_for_lock (lock, (void**) &chunk);
       AMQ_ASSERT_STATUS (result, wait_for_lock)

       /*  Conection requested to stop                                       */
       if (context->stop == 1)
           break;

       /*  Is PING to be sent ? If so, do it.                                */
       if (chunk->send_ping) {
           size = ping_data_size;
           result = apr_socket_send (context->socket, ping_data, &size);
           AMQ_ASSERT_STATUS (result, apr_socket_send)
       }

       /*  Is chunk to be sent ? If so, send it.                             */
       if (chunk->data) {
           size = chunk->size;
           result = apr_socket_send (context->socket, chunk->data, &size);
           AMQ_ASSERT_STATUS (result, apr_socket_send)
       }

       /*  Is there a lock waiting while chunk is sent ?                     */
       /*  If so, release it now.                                            */
       if (chunk->lock_id) {
           result = release_lock (context->global, chunk->lock_id, NULL);
           AMQ_ASSERT_STATUS (result, release_lock)
       }

       /*  Deallocate chunk data as well as chunk descriptor                 */
       amq_free ((void*) (chunk->data));
       amq_free ((void*) chunk);
    }

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
    connection_fsm_context_t  *context,
    const char                *server
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
    AMQ_ASSERT_STATUS (result, apr_parse_addr_port)
    result = apr_sockaddr_info_get (&sockaddr, addr, AF_UNSPEC, port,
        APR_IPV4_ADDR_OK, context->pool);
    AMQ_ASSERT_STATUS (result, apr_socket_info_get)
    result = apr_socket_create ( &(context->socket), APR_INET, SOCK_STREAM,
        APR_PROTO_TCP, context->pool);
    AMQ_ASSERT_STATUS (result, apr_socket_create)
    result = apr_socket_connect (context->socket, sockaddr);
    AMQ_ASSERT_STATUS (result, apr_socket_connect)

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
    connection_fsm_t  context,
    dbyte             channel_id,
    channel_fsm_t     *channel
    )
{
    apr_status_t
        result;
    channel_list_item_t
        *item;

    result = connection_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);

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

    result = connection_fsm_sync_end (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_end);

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
    connection_fsm_t  context,
    dbyte             handle_id,
    handle_fsm_t      *handle
    )
{
    apr_status_t
        result;
    channel_list_item_t
        *item;

    result = connection_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);

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

    result = connection_fsm_sync_end (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_end);

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: connection_get_channel_from_handle

    Synopsis:
    Converts handle id into channel handle belongs to.
    Used by command dispatcher to redirect HANDLE NOTIFY commands to
    apropriate channel object.

    Arguments:
        ctx                connection handle
        handle_id          id to map
        channel            out parameter; mapped channel
    -------------------------------------------------------------------------*/

apr_status_t connection_get_channel_from_handle (
    connection_fsm_t  context,
    dbyte             handle_id,
    channel_fsm_t      *channel
    )
{
    apr_status_t
        result;
    channel_list_item_t
        *item;
    handle_fsm_t
        *handle;

    result = connection_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);

    item = context->channels;
    while (1) {
        if (!item) {
            if (channel) *channel = NULL;
            break;
        }
        channel_get_handle (item->channel, handle_id, handle);
        if (*handle)
        {
            if (channel) *channel = item->channel;
            break;
        }
        item = item->next;
    }

    result = connection_fsm_sync_end (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_end);

    return APR_SUCCESS;
}

/*---------------------------------------------------------------------------
 *  State machine actions (for documentation see amq_stdc_fsms.xml)
 *---------------------------------------------------------------------------*/

inline static apr_status_t do_init (
    connection_fsm_context_t  *context,
    global_fsm_t              global,
    dbyte                     connection_id,
    const char                *server,
    const char                *host,
    const char                *client_name,
    amq_stdc_table_t          options,
    byte                      async,
    lock_t                    *lock
    )
{
    apr_status_t
        result;
    apr_threadattr_t
        *threadattr_receiver;
    apr_threadattr_t
        *threadattr_sender;
    char
        *init_chunk;

    /*  Store info that will be needed later                                 */
    context->global = global;
    context->id = connection_id;
    context->async = async;
    context->host_size = strlen (host);
    context->host = apr_palloc (context->pool, context->host_size);
    if (context->host == NULL)
        AMQ_ASSERT (Not enough memory)
    memcpy ((void*) context->host, (void*) host, context->host_size);
    context->client_name_size = strlen (client_name);
    context->client_name = apr_palloc (context->pool,
        context->client_name_size);
    if (context->client_name == NULL)
        AMQ_ASSERT (Not enough memory)
    memcpy ((void*) context->client_name, (void*) client_name,
        context->client_name_size);
    if (options) {
        result = amq_stdc_table_create (amq_stdc_table_size (options),
            amq_stdc_table_data (options), &(context->options));
        AMQ_ASSERT_STATUS (result, amq_stdc_table_create);
    }

    /*  Open socket                                                          */
    result = s_open_socket (context, server);
    AMQ_ASSERT_STATUS (result, open_socket);

    /*  Run receiver thread                                                  */
    result = apr_threadattr_create (&threadattr_receiver, context->pool);
    AMQ_ASSERT_STATUS (result, apr_threadattr_create)
    result = apr_thread_create (&(context->receiver), threadattr_receiver,
        s_receiver_thread, (void*) context, context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_create)

    /*  Run sender thread                                                    */
    result = apr_threadattr_create (&threadattr_sender, context->pool);
    AMQ_ASSERT_STATUS (result, apr_threadattr_create)
    result = apr_thread_create (&(context->sender), threadattr_sender,
        s_sender_thread, (void*) context, context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_create)

    /*  Register that we will be waiting for connection open completion      */
    result = register_lock (context->global, context->id, 0, 0,
        &(context->open_tag), lock) ;

    /*  Send initiation bytes                                                */
    init_chunk = (char*) amq_malloc (sizeof(byte) + sizeof (byte));
    if (!init_chunk)
        AMQ_ASSERT (Not enough memory)
    *((byte*) init_chunk) = PROTOCOL_ID;
    *((byte*) (init_chunk + sizeof (byte))) = PROTOCOL_VERSION;
    result = connection_fsm_send_chunk (context, init_chunk,
        sizeof (byte) + sizeof (byte), NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_get_chunk (
    connection_fsm_context_t  *context,    
    lock_t                    *lock
    )
{
    apr_status_t
        result;
    chunk_list_item_t
        *chunk;

    if (context->first_chunk) {

        /*  Theres a chunk waiting in the queue. Remove it from the queue    */
        /*  and pass it to the sender thread.                                */
        chunk = context->first_chunk;
        if (!chunk->next)
            context->last_chunk = NULL;
        context->first_chunk = chunk->next;
        chunk->send_ping = context->send_ping;
        chunk->next = NULL;
        result = register_dummy_lock (context->global, (void*) chunk, lock);
        AMQ_ASSERT_STATUS (result, register_dummy_lock)
        context->send_ping = 0;
        context->sender_tag = 0;
    }
    else if (context->send_ping) {

        /*  Chunk queue is empty but PING has to be send. Create empty       */
        /*  chunk structure with ping flag set, pass it to sender thread.    */
        chunk = (chunk_list_item_t*) amq_malloc (sizeof (chunk_list_item_t));
        if (!chunk)
            AMQ_ASSERT (Not enough memory)
        chunk->send_ping = 1;
        chunk->data = NULL;
        chunk->size = 0;
        chunk->next = NULL;
        result = register_dummy_lock (context->global, (void*) chunk, lock);
        AMQ_ASSERT_STATUS (result, register_dummy_lock)
        context->send_ping = 0;
        context->sender_tag = 0;
    }
    else {

        /*  Nothing to be done. Sender thread has to wait till something     */
        /*  happens (chunk to be sent arrives, ping has to be send, thread   */
        /*  has to be terminated).                                           */ 
        result = register_lock (context->global, context->id, 0, 0,
            &(context->sender_tag), lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_send_chunk (
    connection_fsm_context_t  *context,
    char                      *data,
    qbyte                     size,    
    lock_t                    *lock
    )
{
    apr_status_t
        result;
    chunk_list_item_t
        *chunk;

    /*  Create chunk descriptor                                              */
    chunk = (chunk_list_item_t*) amq_malloc (sizeof (chunk_list_item_t));
    if (!chunk)
        AMQ_ASSERT (No enough memory)
    chunk->send_ping = 0;
    chunk->data = data;
    chunk->size = size;
    chunk->lock_id = 0;
    chunk->next = NULL;

    /*  If required, create lock that will be released when data are         */
    /*  actually sent                                                        */
    if (lock) {
        result = register_lock (context->global, context->id, 0, 0,
            &(chunk->lock_id), lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    if (!context->sender_tag) {
        /*  If sender thread isn't waiting, add chunk to the list            */
        context->last_chunk = chunk;
        if (!context->first_chunk)
            context->first_chunk = chunk;
    }
    else {
        /* If seder thread is waiting, release it                            */
        result = release_lock (context->global, context->sender_tag,
            (void*) chunk);
        context->sender_tag = 0;
        AMQ_ASSERT_STATUS (result, release_lock)
    }

    return APR_SUCCESS;
}

inline static apr_status_t do_challenge (
    connection_fsm_context_t  *context,
    byte                      version
    )
{
    apr_status_t
        result;
    char
        *chunk;
    qbyte
        chunk_size;

    if (version != PROTOCOL_VERSION)
        AMQ_ASSERT (Protocol version proposed by server unsupported by client)

    /*  Send CONNECTION RESPONSE                                             */    
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_RESPONSE_CONSTANT_SIZE + 5 + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_response (chunk, chunk_size,
        5, "plain", 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context, chunk, chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_tune (
    connection_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    dbyte
        confirm_tag;
    amq_stdc_table_t
        options;
    char
        *chunk;
    qbyte
        chunk_size;

    /*  Create option table                                                  */
    result = amq_stdc_table_create (0, NULL, &options);
    AMQ_ASSERT_STATUS (result, amq_stdc_table_create)
    /*  TODO: should be adjusted dyncamically  */
    result = amq_stdc_table_add_integer (options, "FRAME_MAX", 1024);
    AMQ_ASSERT_STATUS (result, amq_stdc_table_add_integer)

    /*  Send CONNECTION TUNE                                                 */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_TUNE_CONSTANT_SIZE +
        amq_stdc_table_size (options) + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_tune (chunk, chunk_size,
        amq_stdc_table_size (options), amq_stdc_table_data (options),
        0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context, chunk, chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    /*  Destroy the options table                                            */
    result = amq_stdc_table_destroy (options);
    AMQ_ASSERT_STATUS (result, amq_stdc_table_destroy)

    /*  If asynchronous, don't require reply for CONNECTION OPEN             */
    confirm_tag = context->async ? 0 : context->open_tag;

    /*  Send CONNECTION OPEN                                                 */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_OPEN_CONSTANT_SIZE + context->host_size +
        context->client_name_size + amq_stdc_table_size (context->options);
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_open (chunk, chunk_size,
        confirm_tag, context->host_size, context->host,
        context->client_name_size, context->client_name,
        amq_stdc_table_size (context->options),
        amq_stdc_table_data (context->options));
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context, chunk, chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    /*  If client opted for asynchronous open, client thread should be       */
    /*  released at this point (otherwise it is released when connection     */
    /*  open confirmation is received)                                       */
    /*  (Lock may be released even before CONNECTION OPEN is physically sent */
    /*  by sender thread, but that doesn't matter as there cna be only       */
    /*  one such situation per connection.)                                  */
    if (context->async)
        release_lock (context->global, context->open_tag, (void*) context);
    return APR_SUCCESS;
}

inline static apr_status_t do_create_channel (
    connection_fsm_context_t  *context,
    byte                      transacted,
    byte                      restartable,
    amq_stdc_table_t          options,
    const char                *out_of_band,
    byte                      async,
    lock_t                    *lock
    )
{
    apr_status_t
        result;
    channel_fsm_t
        channel;
    channel_list_item_t
        *item;
    dbyte
        channel_id;

    /*  Assign new channel id                                                */
    result = global_fsm_assign_new_channel_id (context->global, &channel_id);
    AMQ_ASSERT_STATUS (result, global_fsm_assign_new_channel_id)

    /*  Create channel object                                                */
    result = channel_fsm_create (&channel);
    AMQ_ASSERT_STATUS (result, channel_fsm_create)

    /*  Insert it into the list of existing channels                         */
    item = (channel_list_item_t*)
        amq_malloc (sizeof (channel_list_item_t));
    if (!item)
        AMQ_ASSERT (Not enough memory)
    item->channel = channel;
    item->channel_id = channel_id;
    item->next = context->channels;
    context->channels = item;

    /*  Start it                                                             */
    result = channel_fsm_init (channel, context->global, context,
        context->id, channel_id, transacted, restartable,
        options, out_of_band, async, lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_init)
    return APR_SUCCESS;    
}

inline static apr_status_t do_remove_channel (
    connection_fsm_context_t  *context,
    channel_fsm_t             channel
    )
{
    channel_list_item_t
        *item;
    channel_list_item_t
        **prev;

    item = context->channels;
    prev = &(context->channels);
    while (1) {
        if (!item)
            AMQ_ASSERT (Channel specified does not exist)
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
    connection_fsm_context_t  *context,
    dbyte                     confirm_tag,
    dbyte                     reply_code,
    dbyte                     reply_text_size,
    const char                *reply_text
    )
{
    apr_status_t
        result;

    result = release_lock (context->global, confirm_tag, (void*) context);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    connection_fsm_context_t  *context,
    lock_t                    *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;

    /*  TODO: Close handles, rollback transactions, close channels           */

    /*  Release all threads waiting within the connection                    */
    result = register_lock (context->global, context->id, 0, 0,
        &(context->shutdown_tag), lock);
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send CONNECTION CLOSE                                                */    
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_close (chunk, chunk_size,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context, chunk, chunk_size, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;    
}

inline static apr_status_t do_duplicate_terminate (
    connection_fsm_context_t  *context,
    lock_t                    *lock
    )
{
    /*  This must be an explicit error; if not defined, it would fall        */
    /*  through default state and send CONNECTION CLOSE once more            */
    AMQ_ASSERT (Connection is already being terminated)

    return APR_SUCCESS;    
}

inline static apr_status_t do_client_requested_close (
    connection_fsm_context_t  *context
    )
{
    apr_status_t
        result;

    /*  Stop connection threads                                              */
    context->stop = 1;
    /*  Release sender thread so that it can shut down                       */
    if (context->sender_tag) {
        result = release_lock (context->global, context->sender_tag, NULL);
        AMQ_ASSERT_STATUS (result, release_lock)
    }
  
    /*  Resume with error all waiting threads except one requesting          */
    /*  connection termination                                               */
    result = release_all_locks (context->global, context->id, 0, 0,
        context->shutdown_tag, AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    /*  Resume client thread waiting for connection termination              */
    result = release_lock (context->global, context->shutdown_tag, NULL);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;         
}

inline static apr_status_t do_server_requested_close (
    connection_fsm_context_t  *context
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;
    lock_t
        lock;

    /*  Server request connection shutdown                                   */
    /*  TODO: Close handles, rollback transactions, close channels           */

    /*  Send CONNECTION CLOSE                                                */    
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_CLOSE_CONSTANT_SIZE + 0;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_close (chunk, chunk_size,
        200, 0, "");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = connection_fsm_send_chunk (context, chunk, chunk_size, &lock);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    /*  Wait till CONNECTION CLOSE is physically sent; we cannot shutdown    */
    /*  sender thread till this is done. Receiver thread is blocked by this  */
    /*  wait, but there shoudn't be any data from server after CONNECTION    */
    /*  CLOSE anyway.                                                        */
    result = wait_for_lock (lock, NULL);
    AMQ_ASSERT_STATUS (result, wait_for_lock)

    /*  Shutdown connection threads.                                         */
    context->stop = 1;

    /*  Release sender thread so that it can shutdown                        */
    if (context->sender_tag) {
        result = release_lock (context->global, context->sender_tag, NULL);
        AMQ_ASSERT_STATUS (result, release_lock)
    }

    /*  Resume with error all waiting threads                                */
    result = release_all_locks (context->global, context->id, 0, 0, 0,
        AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    return APR_SUCCESS;         
}
