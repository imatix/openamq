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

typedef struct tag_chunk_list_item_t
{
    void
        *data;                         /*  Data to send (single chunk)       */
    qbyte
        data_size;                     /*  Number of bytes to send           */
    content_chunk_t
        *chunks;                       /*  Data to send (chunk list)         */
    qbyte
        chunks_size;                   /*  Number of bytes from chunks list  */
                                       /*  to send                           */
    dbyte
        lock_id;                       /*  Id of lock waiting while chunk    */
                                       /*  is sent; if 0, no lock is waiting */
    byte
        deallocate;                    /*  If 1, sender thread deallocates   */
                                       /*  chunk (data member) after send    */ 
    byte
        stop;                          /*  If 1, sender thread will terminate*/
                                       /*  ater processing this chunk        */
    struct tag_chunk_list_item_t
        *next;                         /*  Next chunk list to send           */
} chunk_list_item_t;

typedef struct tag_channel_list_item_t
{
    channel_fsm_t
        channel;
    dbyte
        channel_id;
    struct tag_channel_list_item_t
        *next;
} channel_list_item_t;

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
    dbyte
        options_size;               /*  Size of options table                */
    char
        *options;                   /*  Options table to be used             */
    dbyte
        open_tag;                   /*  Lock id that is used to wait while   */
                                    /*  connection is opened                 */
    chunk_list_item_t
        *first_chunk;               /*  First chunk of chunk linked list     */
    chunk_list_item_t
        *last_chunk;                /*  Last chunk of chunk linked list      */
    byte
        send_ping;                  /*  If 1 PING is to be sent to server    */
    apr_thread_mutex_t
        *sender_sync;               /*  Used by sender thread to wait for    */
                                    /*  new data to send                     */
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
    apr_status_t
        result;

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
    context->stop = 0;
    context->options = NULL;
    context->options_size = 0;

    result = apr_thread_mutex_create (&(context->sender_sync),
        APR_THREAD_MUTEX_UNNESTED, context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_mutex_create);

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

    /*  TODO: do deallocation properly                                       */
    /*  deallocate chunk list for example...                                 */

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
   content_chunk_t
       *content_chunk;
   qbyte
       command_size;
   qbyte
       header_size;
   qbyte
       body_size;
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
           result = channel_fsm_reply (channel, 0,
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
           result = channel_fsm_created (channel,
               frame.fields.handle_created.handle_id,
               frame.fields.handle_created.dest_name_size,
               frame.fields.handle_created.dest_name);
           AMQ_ASSERT_STATUS (result, channel_fsm_created)
           break;
       case amq_stdc_handle_notify_type:
           result =connection_get_channel_from_handle (
               context, frame.fields.handle_notify.handle_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_channel_from_handle)

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
           
           /*  Send fragment header                                          */
           result = channel_fsm_receive_fragment (channel,
               &(frame.fields.handle_notify), message_header_buffer_ptr,
               header_size);
           AMQ_ASSERT_STATUS (result, channel_fsm_receive_fragment)
           
           /*  Send fragment content to FSM                                  */
           while (body_size) {
               size = CONTENT_CHUNK_SIZE < body_size ? CONTENT_CHUNK_SIZE :
                   body_size;
               content_chunk = (content_chunk_t*) amq_malloc (
                   sizeof (content_chunk_t));
               if (!content_chunk)
                   AMQ_ASSERT (Not enough memory)
               result = apr_socket_recv (context->socket, content_chunk->data,
                   &size);
               AMQ_ASSERT_STATUS (result, apr_socket_receive)
               content_chunk->size = size;
               content_chunk->prev = NULL;
               content_chunk->next = NULL;
               body_size -= size;

               result = channel_fsm_receive_content_chunk (channel,
                   content_chunk,
                   !body_size && !frame.fields.handle_notify.partial);
               AMQ_ASSERT_STATUS (result, channel_fsm_receive_content_chunk)
           }

           break;
       case amq_stdc_handle_index_type:
           result =connection_get_channel_from_handle (
               context, frame.fields.handle_index.handle_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = channel_fsm_index (channel,
               frame.fields.handle_index.handle_id,
               frame.fields.handle_index.message_nbr,
               frame.fields.handle_index.message_list_size,
               frame.fields.handle_index.message_list);
           AMQ_ASSERT_STATUS (result, channel_fsm_index)
           break;
       case amq_stdc_handle_prepare_type:
           assert (0);
           break;
       case amq_stdc_handle_ready_type:
           assert (0);
           break;
       case amq_stdc_handle_reply_type:
           result =connection_get_channel_from_handle (
               context, frame.fields.handle_notify.handle_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_channel_from_handle)
           result = channel_fsm_reply (channel,
                frame.fields.handle_reply.handle_id,
                frame.fields.handle_reply.confirm_tag,
                frame.fields.handle_reply.reply_code,
                frame.fields.handle_reply.reply_text_size,
                frame.fields.handle_reply.reply_text);
           AMQ_ASSERT_STATUS (result, channel_fsm_reply)
           break;
       case amq_stdc_handle_close_type:
           result = connection_get_channel_from_handle (context,
               frame.fields.handle_reply.handle_id, &channel);
           AMQ_ASSERT_STATUS (result, connection_get_handle)
           result = channel_fsm_handle_closed (channel,
               frame.fields.handle_reply.handle_id);
           AMQ_ASSERT_STATUS (result, channel_fsm_close_handle)
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
   chunk_list_item_t
       *chunk;
   apr_size_t
       size;
   qbyte
       to_send;
   char
       ping_data [COMMAND_SIZE_MAX_SIZE +
           AMQ_STDC_CONNECTION_PING_CONSTANT_SIZE];
   qbyte
       ping_data_size;
   content_chunk_t
       *content_chunk;

    /*  Prepare single instance of PING command to be used repeatedly       */
    /*  TODO: PING is not send yet                                          */
    ping_data_size = amq_stdc_encode_connection_ping (ping_data,
        COMMAND_SIZE_MAX_SIZE + AMQ_STDC_CONNECTION_PING_CONSTANT_SIZE, 0);
    if (!ping_data_size)
        AMQ_ASSERT (Framing error)

    while (1) {     

        /*  If there are no chunks in list, wait for new chunk arriving      */
        result = apr_thread_mutex_lock (context->sender_sync);
        AMQ_ASSERT_STATUS (result, apr_thread_mutex_lock);
        result = apr_thread_mutex_unlock (context->sender_sync);
        AMQ_ASSERT_STATUS (result, apr_thread_mutex_unlock);

        /*  Get ownership of one chunk, remove it from chunk list            */
        /*  Do it in synchronised manner so that it won't interfere with     */
        /*  adding the chunks, if there are no more chunks available make    */
        /*  sender sync unpassable.                                          */
        result = connection_fsm_sync_begin_s (context);
        AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);
        chunk = context->first_chunk;
        context->first_chunk = chunk->next;
        if (context->last_chunk == chunk)
            context->last_chunk = NULL;
        if (!context->first_chunk) {
            result = apr_thread_mutex_lock (context->sender_sync);
            AMQ_ASSERT_STATUS (result, apr_thread_mutex_lock);
        }
        result = connection_fsm_sync_end_s (context);
        AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);

        /*  Is chunk to be sent ? If so, do it.                              */
        if (chunk->data) {
            size = chunk->data_size;
            result = apr_socket_send (context->socket, chunk->data, &size);
            AMQ_ASSERT_STATUS (result, apr_socket_send)
            if (chunk->chunks) {
                to_send = chunk->chunks_size;
                content_chunk = chunk->chunks;
                while (1) {
                    size = content_chunk->size < to_send ?
                        content_chunk->size : to_send;
                    result = apr_socket_send (context->socket,
                        (void*) content_chunk->data, &size);
                    AMQ_ASSERT_STATUS (result, apr_socket_send)
                    to_send -= size;
                    if (!to_send)
                        break;
                    content_chunk = content_chunk->next;
                    assert (content_chunk);
                }
            }
        }

        /*  Is there a lock waiting ? If so, release it now.                 */
        if (chunk->lock_id) {
            result = release_lock (context->global, chunk->lock_id, NULL);
            AMQ_ASSERT_STATUS (result, release_lock)
        }

        /*  Deallocate chunk data if requested                               */
        if (chunk->deallocate) {
            if (chunk->data)
                amq_free ((void*) (chunk->data));
            content_chunk = chunk->chunks;
            while (content_chunk) {
                chunk->chunks = content_chunk->next;
                amq_free ((void*) content_chunk);
                content_chunk = chunk->chunks;
            }
        }

        /*  Terminate the sender thread if requested                         */
        if (chunk->stop) {
            amq_free ((void*) chunk);
            break;
        }

        /*  Deallocate chunk descriptor                                      */
        amq_free ((void*) chunk);
        chunk = NULL; 
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
    const char                *server,
    dbyte                     port
    )
{
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer[APRMAXHOSTLEN + 32];      /*  Buffer to build hostname in      */
    char
        *addr = NULL;                   /*  Contains hostname                */
    char
        *scope_id = NULL;               /*  Contains scope id                */
    apr_sockaddr_t
        *sockaddr = NULL;               /*  APR socket address object        */

    sprintf (buffer, "%s:%ld", server, (long) port);
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

    result = connection_fsm_sync_begin (context);
    AMQ_ASSERT_STATUS (result, connection_fsm_sync_begin);

    item = context->channels;
    while (1) {
        if (!item) {
            if (channel) *channel = NULL;
            break;
        }
        if (channel_has_handle (item->channel, handle_id))
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
    dbyte                     port,
    const char                *host,
    const char                *client_name,
    dbyte                     options_size,
    const char                *options,
    byte                      async,
    amq_stdc_lock_t           *lock
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
    qbyte
        host_size = strlen (host);
    qbyte
        client_name_size = strlen (client_name);

    if (host_size > 255)
        AMQ_ASSERT (Host size longer than 255 characters)
    if (client_name_size > 255)
        AMQ_ASSERT (Client name longer than 255 characters)

    /*  Store info that will be needed later                                 */
    context->global = global;
    context->id = connection_id;
    context->async = async;
    context->host_size = (byte) host_size;
    context->host = apr_palloc (context->pool, host_size);
    if (context->host == NULL)
        AMQ_ASSERT (Not enough memory)
    memcpy ((void*) context->host, (void*) host, host_size);
    context->client_name_size = (byte) client_name_size;
    context->client_name = apr_palloc (context->pool, client_name_size);
    if (context->client_name == NULL)
        AMQ_ASSERT (Not enough memory)
    memcpy ((void*) context->client_name, (void*) client_name,
        client_name_size);
    if (options && options_size) {
        context->options_size = options_size;
        context->options = apr_palloc (context->pool, options_size);
        if (!context->options)
            AMQ_ASSERT (Not enough memory)
        memcpy ((void*) context->options, (void*) options, options_size);
    }

    /*  Open socket                                                          */
    result = s_open_socket (context, server, port);
    AMQ_ASSERT_STATUS (result, open_socket);

    /*  Run receiver thread                                                  */
    result = apr_threadattr_create (&threadattr_receiver, context->pool);
    AMQ_ASSERT_STATUS (result, apr_threadattr_create)
    result = apr_thread_create (&(context->receiver), threadattr_receiver,
        (apr_thread_start_t) s_receiver_thread, (void*) context,
        context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_create)  

    /*  Run sender thread                                                    */
    result = apr_threadattr_create (&threadattr_sender, context->pool);
    AMQ_ASSERT_STATUS (result, apr_threadattr_create)
    result = apr_thread_create (&(context->sender), threadattr_sender,
        (apr_thread_start_t) s_sender_thread, (void*) context,
        context->pool);
    AMQ_ASSERT_STATUS (result, apr_thread_create)

    /*  Register that we will be waiting for connection open completion      */
    result = register_lock (context->global, context->id, 0,
        &(context->open_tag), lock) ;
    AMQ_ASSERT_STATUS (result, register_lock)

    /*  Send initiation bytes                                                */
    init_chunk = (char*) amq_malloc (sizeof(byte) + sizeof (byte));
    if (!init_chunk)
        AMQ_ASSERT (Not enough memory)
    *((byte*) init_chunk) = PROTOCOL_ID;
    *((byte*) (init_chunk + sizeof (byte))) = PROTOCOL_VERSION;
    result = do_send_chunk (context, init_chunk,
        sizeof (byte) + sizeof (byte), NULL, 0, 1, 0, NULL);
    AMQ_ASSERT_STATUS (result, connection_fsm_send_chunk);

    return APR_SUCCESS;
}

inline static apr_status_t do_send_chunk (
    connection_fsm_context_t  *context,
    char                      *data,
    qbyte                     data_size,
    content_chunk_t           *chunks,
    qbyte                     chunks_size,
    byte                      deallocate, 
    byte                      stop,   
    amq_stdc_lock_t           *lock
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
    chunk->data = data;
    chunk->data_size = data_size;
    chunk->chunks = chunks;
    chunk->chunks_size = chunks_size;
    chunk->deallocate = deallocate;
    chunk->lock_id = 0;
    chunk->next = NULL;
    chunk->stop = stop;

    /*  If required, create lock that will be released when data are         */
    /*  actually sent                                                        */
    if (lock) {
        result = register_lock (context->global, context->id, 0,
            &(chunk->lock_id), lock);
        AMQ_ASSERT_STATUS (result, register_lock)
    }

    /* If there are chunks in list sender_sync must be passable              */
    if (!context->first_chunk) {
        result = apr_thread_mutex_unlock (context->sender_sync);
        AMQ_ASSERT_STATUS (result, apr_thread_mutex_unlock)
    }

    /*  Add chunk to the list                                                */
    if (context->last_chunk)
        context->last_chunk->next = chunk;
    context->last_chunk = chunk;
    if (!context->first_chunk)
        context->first_chunk = chunk;

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
        AMQ_STDC_CONNECTION_RESPONSE_CONSTANT_SIZE + 5 + 0 + 31;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_response (chunk, chunk_size,
        5, "plain", 31,
        "\x005LOGINS\x000\x005guest\x008PASSWORDS\x000\x005guest");
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = do_send_chunk (context, chunk, chunk_size, NULL, 0,
        1, 0, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk);

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
    result = amq_stdc_table_add_integer (options, "FRAME_MAX", 0x8000);
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
    result = do_send_chunk (context, chunk, chunk_size, NULL, 0,
        1, 0, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk);

    /*  Destroy the options table                                            */
    result = amq_stdc_table_destroy (options);
    AMQ_ASSERT_STATUS (result, amq_stdc_table_destroy)

    /*  If asynchronous, don't require reply for CONNECTION OPEN             */
    confirm_tag = context->async ? 0 : context->open_tag;

    /*  Send CONNECTION OPEN                                                 */
    chunk_size = COMMAND_SIZE_MAX_SIZE +
        AMQ_STDC_CONNECTION_OPEN_CONSTANT_SIZE + context->host_size +
        context->client_name_size + context->options_size;
    chunk = (char*) amq_malloc (chunk_size);
    if (!chunk)
        AMQ_ASSERT (Not enough memory)
    chunk_size = amq_stdc_encode_connection_open (chunk, chunk_size,
        confirm_tag, context->host_size, context->host,
        context->client_name_size, context->client_name,
        context->options_size,
        context->options);
    if (!chunk_size)
        AMQ_ASSERT (Framing error)
    result = do_send_chunk (context, chunk, chunk_size, NULL, 0,
        1, 0, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk);

    /*  If client opted for asynchronous open, client thread should be       */
    /*  released at this point (otherwise it is released when connection     */
    /*  open confirmation is received)                                       */
    /*  (Lock may be released even before CONNECTION OPEN is physically sent */
    /*  by sender thread, but that doesn't matter as there can be only       */
    /*  one such situation per connection.)                                  */
    if (context->async)
        release_lock (context->global, context->open_tag, NULL);
    return APR_SUCCESS;
}

inline static apr_status_t do_create_channel (
    connection_fsm_context_t  *context,
    byte                      transacted,
    byte                      restartable,
    dbyte                     options_size,
    const char                *options,
    const char                *out_of_band,
    byte                      async,
    channel_fsm_t             *out,
    amq_stdc_lock_t           *lock
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
        options_size, options, out_of_band, async, lock);
    AMQ_ASSERT_STATUS (result, channel_fsm_init)

    if (out) *out = channel;

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

    result = release_lock (context->global, confirm_tag, NULL);
    AMQ_ASSERT_STATUS (result, release_lock)
    return APR_SUCCESS;
}

inline static apr_status_t do_terminate (
    connection_fsm_context_t  *context,
    amq_stdc_lock_t           *lock
    )
{
    apr_status_t
        result;
    qbyte
        chunk_size;
    char
        *chunk;

    /*  TODO: Close handles, rollback transactions, close channels           */

    /*  Register lock to wait for server close confirmation                  */
    result = register_lock (context->global, context->id, 0,
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
    result = do_send_chunk (context, chunk, chunk_size, NULL, 0,
        1, 0, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk);

    return APR_SUCCESS;    
}

inline static apr_status_t do_duplicate_terminate (
    connection_fsm_context_t  *context,
    amq_stdc_lock_t           *lock
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

    /*  Stop receiver thread                                                 */
    context->stop = 1;

    /*  Stop sender thread                                                   */
    result = do_send_chunk (context, NULL, 0, NULL, 0, 0, 1, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk)
  
    /*  Resume with error all waiting threads except one requesting          */
    /*  connection termination                                               */
    result = release_all_locks (context->global, context->id, 0,
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
    result = do_send_chunk (context, chunk, chunk_size, NULL, 0,
        1, 0, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk);

    /*  Stop receiver thread                                                 */
    context->stop = 1;

    /*  Stop sender thread                                                   */
    result = do_send_chunk (context, NULL, 0, NULL, 0, 0, 1, NULL);
    AMQ_ASSERT_STATUS (result, do_send_chunk)

    /*  Resume with error all waiting threads                                */
    result = release_all_locks (context->global, context->id, 0, 0,
        AMQ_OBJECT_CLOSED);
    AMQ_ASSERT_STATUS (result, release_all_locks)

    return APR_SUCCESS;         
}
