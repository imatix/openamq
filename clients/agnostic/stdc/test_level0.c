/*---------------------------------------------------------------------------
 *  test_level0.c - APR test client for Level 0 AMQ Client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amqp_level0.h"                /*  Level 0 API definitions          */

#include <stdio.h>                      /*  C standard I/O                   */
#include <stdlib.h>                     /*  C standard library               */
#include <stdarg.h>                     /*  For va_* functions               */

/*- Definitions ------------------------------------------------------------
*/

#define MAX_FRAME_SIZE 32000            /*  Max supported frame size         */
#define BUFFER_SIZE    32768            /*  Network buffer size              */
#define EXIT_SUCCESS   0                /*  Code returned from main when     */
                                        /*  there's no error                 */
#define EXIT_FAILURE   1                /*  Code returned from main in       */           
                                        /*  case of error                    */

/*  Scenario types client may use
*/
typedef enum
{
    clienttype_undefined,               /*  Invalid type                     */
    clienttype_producer,                /*  Sends messages to server         */
    clienttype_consumer,                /*  Receives messages from server    */
    clienttype_query                    /*  Queries server for messages      */
} clienttype_t;

/*  States client can be in
*/
typedef enum
{
    state_initial,
    state_waiting_for_connection_challenge,
    state_waiting_for_connection_tune,
    state_waiting_for_connection_open_confirmation,
    state_waiting_for_channel_open_confirmation,
    state_waiting_for_handle_open_confirmation,
    state_waiting_for_consume_confirmation,
    state_waiting_for_send_confirmation,
    state_waiting_for_message_or_acknowledge_confirmation,
    state_waiting_for_index,
    state_waiting_for_browse_confirmation
} state_t;

/*  Client object
*/
typedef struct
{
    clienttype_t
        clienttype;                     /*  Type of client                   */
    state_t
        state;                          /*  Current state  of client         */
    apr_socket_t
        *sck;                           /*  Socket                           */
    const char
        *server;                        /*  Server to connect to             */
    const char
        *host;                          /*  Virtual host to connect to       */
    const char
        *destination;                   /*  Destination to connect to        */
    const char 
        *client_name;                   /*  Name of the client, used also as */
                                        /*  prefix for message identifiers   */
    apr_uint16_t
        tag;                            /*  Current confirm tag              */
    int
        till_acknowledge;               /*  Number of messages to be         */
                                        /*  received till acknowledge is to  */
                                        /*  be sent                          */
    long
        last_message_number;            /*  Messages are numbered so that    */
                                        /*  each can have unique identifier  */
    int 
        stop;                           /*  Callback functions set this      */
                                        /*  variable to 1, when message      */
                                        /*  dispatch loop is to be           */
                                        /*  terminated                       */
    long
        messages;                       /*  Number of messages to send,      */
                                        /*  0 means infinite                 */
    char
        *message_buffer;                /*  Stores message data to be sent   */
    apr_size_t
        message_size;                   /*  Size of message data             */
    apr_byte_t
        persistent;                     /*  1 for persistent messages to be  */
                                        /*  sent, 0 for nonpersistent        */
    long
        commit_count;                   /*  Transaction commited every       */
                                        /*  N-th message                     */
    long
        rollback_count;                 /*  Transaction rolled back every    */
                                        /*  N-th message                     */
    int
        interval;                       /*  Interval (in milliseconds)       */
                                        /*  between individual messages sent */
    apr_uint16_t
        prefetch;                       /*  Number of messages that server   */
                                        /*  prefetches                       */
    
    char
        *query_result;                  /*  Stores result of query while     */
                                        /*  individual returned messages are */
                                        /*  being browsed                    */
    int
        query_result_pos;               /*  Position in query_result, where  */
                                        /*  parsing is going on              */
    int
        browse_begin;                   /*  When interval is specified in    */
                                        /*  query result, holds left bound   */
    int
        browse_end;                     /*  When interval is specified in    */
                                        /*  query result, holds right bound  */
    int
        last_query_result;              /*  When move_to_next_query_result   */
                                        /*  is called, this variable stores  */
                                        /*  the result                       */
} client_t;    

/*- Function prototypes ----------------------------------------------------
*/

static int s_trace (
   const char *format,
   ...
   );

static void s_print_usage (void);

static void s_main_terminate (
    client_t *client
    );

static void s_move_to_next_query_result (
    client_t *client
    );

static apr_status_t s_browse_next (
    client_t     *client,
    apr_uint16_t handle_id
    );

static apr_status_t s_send_message(
    client_t     *client,
    apr_uint16_t confirm_tag
    );

static apr_status_t s_handle_close_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_channel_close_cb (
    const void         *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_connection_close_cb (
    const void         *hint,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_handle_index_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_size_t   message_list_size,
    const char*        message_list
    );

static apr_status_t s_handle_reply_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_channel_reply_cb (
    const void         *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_connection_reply_cb (
    const void         *hint,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    );

static apr_status_t s_connection_challenge_cb (
    const void       *hint,
    const apr_byte_t version,
    const char       *mechanisms,
    const apr_size_t challenges_size,
    const char       *challenges
    );

static apr_status_t s_connection_tune_cb (
    const void         *hint,
    const apr_size_t   options_size,
    const char         *options,
    const apr_size_t   dictionary_size,
    const char         *dictionary
    );

static apr_status_t s_handle_notify_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_byte_t   partial,
    const apr_byte_t   out_of_band,
    const apr_byte_t   recovery,
    const apr_byte_t   delivered,
    const apr_byte_t   redelivered,
    const apr_byte_t   streaming,
    const char         *dest_name,
    const apr_uint32_t body_size,
    const apr_byte_t   persistent,
    const apr_byte_t   priority,
    const apr_uint32_t expiration,
    const char         *mime_type,
    const char         *encoding,
    const char         *identifier,
    const apr_size_t   headers_size,
    const char         *headers,
    const char         *data
    );

/*- Main program ------------------------------------------------------------  
  Synopsis:

  Main program creates clients (client_t object), each of them representing
  individual socket and connection to AMQ server.
  There's single client for now, but more of them may be created if needed.

  Each client may run different scenario type (clienttype member) : it may
  be a producer (sending messages to server), a consumer (receiving
  messages from server or a query (querying server for messages).
  More scenarios may be added in future.

  Each client is controlled by a state machine, where state is stored in 
  'state' member of client object and events are in fact callbacks from AMQ 
  level 0 API.  As state machines for individual scenarios are quite similar, 
  there is no separate state machine for each of them.  Differences are 
  handled as if-else statements within event handlers (level 0 callback 
  functions).
  ---------------------------------------------------------------------------
*/

int main (
    int        argc,
    const char *const argv[],
    const char *const env[]) 
{
    apr_status_t
        result;                         /*  Stores return values             */
    apr_pool_t
        *pool = NULL;                   /*  Memory pool for APR functions    */
    char
        *addr = NULL;                   /*  Contains hostname                */
    char
        *scope_id = NULL;               /*  Contains scope id                */
    apr_port_t
        port;                           /*  APR port object                  */
    apr_sockaddr_t
        *sockaddr = NULL;               /*  APR socket address object        */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */
    int 
        arg_pos;                        /*  Loop control variable            */
    int
        buff_pos;                       /*  Loop control variable            */
    amqp_callbacks_t
        callbacks;                      /*  Table of AMQP level0 callbacks   */
    client_t
        client;                         /*  Main client object               */
    
    /*  Initialize client object according to command line parameters
    */
    client.clienttype          = clienttype_undefined;
    client.state               = state_initial;
    client.server              = "127.0.0.1";
    client.host                = NULL;
    client.destination         = NULL;
    client.client_name         = "client";
    client.messages            = 0;
    client.interval            = 500;
    client.prefetch            = 1;
    client.message_size        = 2;
    client.persistent          = 0;
    client.commit_count        = 0;
    client.rollback_count      = 0;    
    client.last_message_number = 0;
    client.stop                = 0;
    for (arg_pos=1; arg_pos!=argc; arg_pos++) {
        if (strcmp (argv[arg_pos], "producer") == 0)
            client.clienttype = clienttype_producer;
        if (strcmp (argv[arg_pos], "consumer") == 0)
            client.clienttype = clienttype_consumer;
        if (strcmp (argv[arg_pos], "query") == 0)
            client.clienttype = clienttype_query;
        if (strcmp (argv[arg_pos], "-s") == 0 && ++arg_pos != argc)
            client.server = argv [arg_pos];
        if (strcmp (argv[arg_pos], "-h") == 0 && ++arg_pos != argc)
            client.host = argv [arg_pos];
        if (strcmp (argv[arg_pos], "-d") == 0 && ++arg_pos != argc)
            client.destination = argv [arg_pos];
        if (strcmp (argv[arg_pos], "-m") == 0 && ++arg_pos != argc)
            client.client_name = argv [arg_pos];
        if (strcmp (argv[arg_pos], "-n") == 0 && ++arg_pos != argc)
            client.messages = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-i") == 0 && ++arg_pos != argc)
            client.interval = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-p") == 0 && ++arg_pos != argc)
            client.prefetch = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-l") == 0 && ++arg_pos != argc)
            client.message_size = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-c") == 0 && ++arg_pos != argc)
            client.commit_count = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-r") == 0 && ++arg_pos != argc)
            client.rollback_count = atoi (argv [arg_pos]);
        if (strcmp (argv[arg_pos], "-x") == 0)
            client.persistent = 1;
    }
    client.till_acknowledge = client.prefetch;

    /*  Make sure that mandatory parameters were supplied
    */
    if (client.clienttype == clienttype_undefined ||
            client.host == NULL ||
            client.destination == NULL) {
        s_print_usage ();
        return EXIT_FAILURE;
    }

    /*  Create message  to be sent
    */
    client.message_buffer = malloc (client.message_size);
    if (!client.message_buffer) {
        s_trace ("Not enough memory for message body.");
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }
    for (buff_pos = 0; buff_pos != client.message_size; buff_pos++) 
        client.message_buffer [buff_pos] = buff_pos % 256;

    s_trace ("Connecting to server.\n");

    /*  Initialize APR, create and open a socket, initialize AMQ
    */

    result = apr_app_initialize (&argc, &argv, &env);
    if(result != APR_SUCCESS) {
        s_trace ("apr_app_initialize failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = apr_pool_initialize();
    if(result != APR_SUCCESS) {
        s_trace ("apr_pool_initialize failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = apr_pool_create(&pool, NULL);
    if (result != APR_SUCCESS) {
        s_trace ("apr_pool_create failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = apr_socket_create ( &(client.sck), APR_INET, SOCK_STREAM,
        APR_PROTO_TCP, pool);
    if (result != APR_SUCCESS) {
        s_trace ("apr_socket_create failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    sprintf (buffer, "%s:7654", client.server);
    result = apr_parse_addr_port (&addr, &scope_id, &port, buffer, pool);
    if (result != APR_SUCCESS) {
        s_trace ("apr_parse_addr_port failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = apr_sockaddr_info_get(&sockaddr, addr, AF_UNSPEC, port,
        APR_IPV4_ADDR_OK, pool);
    if (result != APR_SUCCESS) {
        s_trace ("apr_sockaddr_info_get failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = apr_socket_connect (client.sck, sockaddr);
    if (result != APR_SUCCESS) {
        s_trace ("apr_socket_connect failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    result = amqp_init (client.sck);
    if (result != APR_SUCCESS) {
        s_trace ("amqp_init failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        s_main_terminate (&client);
        return EXIT_FAILURE;
    }

    client.state = state_waiting_for_connection_challenge;

    /*  Fill in the callbacks table
    */
    memset ((void*) &callbacks, 0, sizeof (amqp_callbacks_t));
    callbacks.connection_challenge = s_connection_challenge_cb;
    callbacks.connection_tune      = s_connection_tune_cb;
    callbacks.handle_notify        = s_handle_notify_cb;
    callbacks.connection_reply     = s_connection_reply_cb;
    callbacks.channel_reply        = s_channel_reply_cb;
    callbacks.handle_reply         = s_handle_reply_cb;
    callbacks.handle_index         = s_handle_index_cb;
    callbacks.connection_close     = s_connection_close_cb;
    callbacks.channel_close        = s_channel_close_cb;
    callbacks.handle_close         = s_handle_close_cb;

    /*  Dispatcher loop
    */
    while (1) {
        result = amqp_recv (client.sck, buffer, BUFFER_SIZE,
            &callbacks, (void*) &client);
        if (result != APR_SUCCESS && result != APR_TIMEUP) {
            s_trace ("amqp_recv failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            s_main_terminate (&client);
            return EXIT_FAILURE;
        }

        /*  Dispatcher loop ends when some command handler sets client.stop
            variable to value of 1
        */
        if (client.stop)
            break;
    }

    s_main_terminate (&client);
    return EXIT_SUCCESS;
}

/*- Internal functions ----------------------------------------------------- */

/*  -------------------------------------------------------------------------
    Function: s_s_trace

    Synopsis:
    For now same as fprint to stderr, may be modified in case of need

    Arguments:
        const char *format  Format string, see printf documentation
        ...                 Variable arguments, same as for printf
    -------------------------------------------------------------------------*/

int s_trace (
   const char *format,
   ...
   )
{
    va_list
        args;
    int 
        result;
    char
        date_str [APR_CTIME_LEN+1];

    apr_ctime (date_str, apr_time_now ());
    fprintf (stderr, "%s: ", date_str);
    va_start (args, format);
    result = vfprintf (stderr, format, args);
    va_end (args);
    return result;
}

/*  -------------------------------------------------------------------------
    Function: s_print_usage

    Synopsis:
    Prints out usage notes.
    -------------------------------------------------------------------------*/

void s_print_usage (void)
{
    printf (
        "Usage: test_level0 MODE OPTIONS ...\n"
        "\n"
        "  MODE: producer\n"
        "    -s <server name/ip address, default='127.0.0.1'>\n"
        "    -h <virtual host name>\n"
        "    -d <destination>\n"
        "    -m <client identifier, default='client'>\n"
        "    -n <number of messages, 0 means infinite, default=0>\n"
        "    -i <interval between individual messages in ms, default=500>\n"
        "    -l <length of message content in bytes, default=2>\n"
        "    -x (sends persistent messages)\n"
        "    -c <number of messages while commit is issued>\n"
        "    -r <number of messages while rollback is issued>\n"
        "  Note : When neither 'c' or 'r' parameter is set\n"
        "         client works in nontransacted mode.\n"
        "\n"
        "  MODE: consumer\n"
        "    -s <server name/ip address, default=127.0.0.1>\n"
        "    -h <virtual host name>\n"
        "    -d <destination>\n"
        "    -p <number of prefetched messages, default=1>\n"
        "\n"
        "  MODE: query (queries for all messages and browses them one by one)\n"
        "    -s <server name/ip address, default=127.0.0.1>\n"
        "    -h <virtual host name>\n"
        "    -d <destination>\n"
        "\n"
    );
}

/*  -------------------------------------------------------------------------
    Function: s_main_terminate

    Synopsis:
    Termination handler for main function.

    Arguments:
            client_t *client  Client object
    -------------------------------------------------------------------------*/

void s_main_terminate (
    client_t *client
    )
{
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */

    s_trace ("Terminating client.\n");

    /*  Terminates AMQ, closes the socket, terminates APR
    */

    if (client->sck) {
        result = amqp_term (client->sck);
        if (result != APR_SUCCESS)
            s_trace ("apr_term failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        
        result = apr_socket_close (client->sck);
        if (result != APR_SUCCESS)
            s_trace ("apr_socket_close failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));

        apr_terminate();
    }
}

/*  -------------------------------------------------------------------------
    Function: s_handle_close_cb

    Synopsis:
    Callback function called when HANDLE CLOSE command arrives.

    Arguments:              
        See amqp_level0.h for documentation
    -------------------------------------------------------------------------*/

apr_status_t s_handle_close_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    s_trace ("Server closed the handle.\n%ld : %s\n",
        (long) reply_code, reply_text);
    /*  Behaviour not implemented yet
    */
    abort ();
    /*  Returns some value to prevent compiler from producing warning
    */
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: s_channel_close_cb

    Synopsis:
    Callback function called when CHANNEL CLOSE command arrives.              

    Arguments:              
        See amqp_level0.h for documentation
    -------------------------------------------------------------------------*/

apr_status_t s_channel_close_cb (
    const void         *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    s_trace ("Server closed the channel.\n%ld : %s\n",
        (long) reply_code, reply_text);
    /*  Behaviour not implemented yet
    */
    abort();
    /*  Returns some value to prevent compiler from producing warning
    */
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: s_connection_close_cb

    Synopsis:
    Callback function called when CONNECTION CLOSE command arrives.              

    Arguments:              
        See amqp_level0.h for documentation                  
    -------------------------------------------------------------------------*/

apr_status_t s_connection_close_cb (
    const void         *hint,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    s_trace ("Server closed the connection.\n%ld : %s\n",
        (long) reply_code, reply_text);
    /*  Behaviour not implemented yet
    */
    abort ();
    /*  Returns some value to prevent compiler from producing warning
    */
    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: s_move_to_next_query_result

    Synopsis:
    Parses next query result from existing query result string
    (one returned by HANDLE INDEX) and stores it into last_query_result.

    Arguments:
        client_t *client  Client object
    -------------------------------------------------------------------------*/

void s_move_to_next_query_result (
    client_t *client
    )
{
    int
        pos;

    /*  If we are inside a range specifier, return next value
    */
    if (client->browse_begin < client->browse_end) {
        client->last_query_result = client->browse_begin++;
    }
    else
    /*  If we are at the end of query result string,
        there are no more results
    */
    if (client->query_result [client->query_result_pos] == '\0') {
        free (client->query_result);
        client->last_query_result = -1;
    }
    else {
        /*  Parse next value from query result string
        */
        pos = client->query_result_pos;
        client->browse_begin = 0;
        while (isdigit (client->query_result [pos]))
            client->browse_begin = client->browse_begin * 10 +
                client->query_result [pos++] - '0';

        /*  If we have a range specifier, parse the end value
        */
        if (client->query_result [pos] == '-') {
            pos++;
            client->browse_end = 0;
                while (isdigit (client->query_result [pos]))
                    client->browse_end = client->browse_end * 10 +
                        client->query_result [pos++] - '0';
        }
        /* It's just a single value, left and round bound should be the same
        */
        else {
            client->browse_end = client->browse_begin;
        }

        /*  Ignore whitespace
        */
        while (isspace (client->query_result [pos]))
            pos++;

        client->query_result_pos = pos;
        client->last_query_result = client->browse_begin++;
    }
}

/*  -------------------------------------------------------------------------
    Function: s_browse_next

    Synopsis:
    Browses next message returned from query.

    Arguments:
        client_t *client        Client object
        apr_uint16_t handle_id  Handle identifier
    -------------------------------------------------------------------------*/

apr_status_t s_browse_next (
    client_t     *client,
    apr_uint16_t handle_id
    )
{
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */

    s_move_to_next_query_result (client);
    /*  If we have more messages to browse, fetch next from server
    */
    if (client->last_query_result != -1) {
        result = amqp_handle_browse (client->sck, buffer, BUFFER_SIZE,
            handle_id, client->tag++, client->last_query_result);
        if (result != APR_SUCCESS) {
            s_trace ("amqp_handle_browse failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));

            return result;
        }
        client->state = state_waiting_for_browse_confirmation;
    }
    else {
        client->stop = 1;
    }

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: s_send_message

    Synopsis:
    Sends message to the server.

    Arguments:
        client_t     *client      Client object
        apr_uint16_t confirm_tag  Confirmation tag to use
    -------------------------------------------------------------------------*/

apr_status_t s_send_message(
    client_t     *client,
    apr_uint16_t confirm_tag
    )
{
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */
    char
        identifier [BUFFER_SIZE];       /*  Buffer to construct message      */
                                        /*  identifier in                    */

    if (client->interval)
        apr_sleep (client->interval * 1000);

    /*  Send the message
    */
    sprintf (identifier, "%s-%ld", client->client_name,
        client->last_message_number);

    result = amqp_handle_send (client->sck, buffer, BUFFER_SIZE, 1,
        confirm_tag, 0, 0, 0, 0, "", client->message_size, client->persistent,
        0, 0, "", "", identifier, 0, "", client->message_buffer);
    if (result != APR_SUCCESS) {
        s_trace ("amqp_handle_send failed: err=%d (%s)\n",
            (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
        return result;
    }

    s_trace ("Message %s sent. (%ld bytes)\n", identifier,
        (long) client->message_size);

    /*  Rollback transaction if needed
    */
    if (client->rollback_count &&
        ((client->last_message_number + 1) % client->rollback_count) == 0) {

        result = amqp_channel_rollback (client->sck, buffer, BUFFER_SIZE, 
            1, 0, 0, "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_channel_rollback failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        s_trace ("Rolled back.\n");
    }
    /*  Commit transaction if needed
    */
    else 
    if (client->commit_count &&
        ((client->last_message_number + 1) % client->commit_count) == 0) {

        result = amqp_channel_commit (client->sck, buffer, BUFFER_SIZE,
            1, 0, 0, "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_channel_commit failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        s_trace ("Commited.\n");
    }

    client->last_message_number++;
    client->state = state_waiting_for_send_confirmation;

    return APR_SUCCESS;
}

/*  -------------------------------------------------------------------------
    Function: s_handle_index_cb

    Synopsis:
    Callback function called when HANDLE INDEX command arrives.              

    Arguments:              
        See amqp_level0.h for documentation
    -------------------------------------------------------------------------*/

apr_status_t s_handle_index_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_size_t   message_list_size,
    const char         *message_list
    )
{
    client_t 
        *client = (client_t*) hint;     /*  Client object                    */

    if (client->state == state_waiting_for_index) {
        s_trace ("Result of query: %s\n", message_list);

        /*  Fill query result into client
        */
        client->query_result = malloc (strlen (message_list) + 1);
        strcpy (client->query_result , message_list);
        client->query_result_pos = 0;

        /*  Browse first message from result
            subsequent messages will be browsed on confirmation of this
            HANDLE BROWSE
        */
        s_browse_next (client, handle_id);

        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_handle_reply_cb

    Synopsis:
    Callback function called when HANDLE REPLY command arrives.              

    Arguments:              
        See amqp_level0.h for documentation               
    -------------------------------------------------------------------------*/

apr_status_t s_handle_reply_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    client_t
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */

    if (client->state == state_waiting_for_handle_open_confirmation) {

        s_trace ("Handle opened.\n");

        if (client->clienttype == clienttype_consumer) {
            result = amqp_handle_consume (client->sck, buffer, BUFFER_SIZE,
                1, 4, client->prefetch, 0, 0, "", "", 0, "", "");
            if (result != APR_SUCCESS) {
                s_trace ("amqp_handle_consume failed: err=%d (%s)\n",
                    (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
                return result;
            }
            client->state = state_waiting_for_consume_confirmation;
        }

        if (client->clienttype == clienttype_producer) {
            return s_send_message (client, (apr_uint16_t) (confirm_tag + 1));
        }

        if (client->clienttype == clienttype_query) {
            s_trace ("Issuing query.\n");
            result = amqp_handle_query (client->sck, buffer, BUFFER_SIZE,
                1, 0, "", 0, "", "");
            if (result != APR_SUCCESS) {
                s_trace ("amqp_handle_query failed: err=%d (%s)\n",
                    (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
                return result;
            }
            client->state = state_waiting_for_index;
        }
        return APR_SUCCESS;
    }
    if (client->state == state_waiting_for_consume_confirmation) {
        client->tag = confirm_tag;
        client->state = state_waiting_for_message_or_acknowledge_confirmation;
        return APR_SUCCESS;
    }
    if (client->state == state_waiting_for_send_confirmation) {
        if (client->messages) {
            /*  Decrement number of messages still to be sent
                If there are no more messages to be sent, stop the client    
            */
            client->messages--;
            if (client->messages == 0)
                client->stop = 1;
        }
        if (!client->stop)
            s_send_message (client, (apr_uint16_t) (confirm_tag + 1));
        return APR_SUCCESS;
    }
    if (client->state == state_waiting_for_browse_confirmation) {
        if (reply_code != 200) {
            s_trace ("Browsing message number %d failed with error %d, %s\n",
                client->last_query_result, reply_code, reply_text);
            s_browse_next (client, handle_id);
        }
        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_channel_reply_cb

    Synopsis:
    Callback function called when CHANNEL REPLY command arrives.              

    Arguments:              
        See amqp_level0.h for documentation
    -------------------------------------------------------------------------*/

apr_status_t s_channel_reply_cb (
    const void         *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    client_t
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];

    if (client->state == state_waiting_for_channel_open_confirmation) {
        s_trace ("Channel opened.\n");

        result = amqp_handle_open (client->sck, buffer, BUFFER_SIZE,
            1, 1, 1, 3, 1, 1, 0, 0, client->destination, "", "", 0, "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_handle_open failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        client->state = state_waiting_for_handle_open_confirmation;
        return APR_SUCCESS;
    }
    if (client->state == state_waiting_for_message_or_acknowledge_confirmation) {
        /*  Consumer : reply to channel ack
        */
        s_trace ("Message acknowledged.\n");
        return APR_SUCCESS;
    }
    
    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_connection_reply_cb

    Synopsis:
    Callback function called when CONNECTION REPLY command arrives.              

    Arguments:              
        See amqp_level0.h for documentation
    -------------------------------------------------------------------------*/

apr_status_t s_connection_reply_cb (
    const void         *hint,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char         *reply_text
    )
{
    client_t
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];

    if (client->state == state_waiting_for_connection_open_confirmation) {
        result = amqp_channel_open (client->sck, buffer, BUFFER_SIZE, 1, 2,
	    /*  Transaction mode
            */
            (apr_byte_t) ((client->commit_count || client->rollback_count) ? 1 : 0),
            0, 0, "", "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_channel_open failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        client->state = state_waiting_for_channel_open_confirmation;
        s_trace ("Connected to server.\n");
        return APR_SUCCESS;
    }

    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_connection_challenge_cb

    Synopsis:
    Callback function called when CONNECTION CHALLENGE command arrives.              

    Arguments:              
        See amqp_level0.h for documentation                  
    -------------------------------------------------------------------------*/

apr_status_t s_connection_challenge_cb (
    const void       *hint,
    const apr_byte_t version,
    const char       *mechanisms,
    const apr_size_t challenges_size,
    const char       *challenges
    )
{
    client_t 
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];
    
    if (client->state == state_waiting_for_connection_challenge) {
        /*  Reply to amq protocol initiation
        */
        result = amqp_connection_response (client->sck, buffer,
            BUFFER_SIZE, "plain", 0, "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_connection_response failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        client->state = state_waiting_for_connection_tune;
        return APR_SUCCESS;
    }

    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_connection_tune_cb

    Synopsis:
    Callback function called when CONNECTION TUNE command arrives.              

    Arguments:              
        See amqp_level0.h for documentation                
    -------------------------------------------------------------------------*/

apr_status_t s_connection_tune_cb (
    const void        *hint,
    const apr_size_t   options_size,
    const char        *options,
    const apr_size_t   dictionary_size,
    const char        *dictionary
    )
{
    client_t
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];
    
    if (client->state == state_waiting_for_connection_tune) {
        /*  PH: removed sending connection tune back to server, not needed  */
        result = amqp_connection_open (client->sck, buffer, BUFFER_SIZE, 1,
            client->host, client->client_name, 0, "");
        if (result != APR_SUCCESS) {
            s_trace ("amqp_connection_open failed: err=%d (%s)\n",
                (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
            return result;
        }
        client->state = state_waiting_for_connection_open_confirmation;
        return APR_SUCCESS;
    }

    return AMQ_FRAME_CORRUPTED;
}

/*  -------------------------------------------------------------------------
    Function: s_handle_notify_cb

    Synopsis:
    Callback function called when HANDLE NOTIFY command arrives.              

    Arguments:              
        See amqp_level0.h for documentation                 
    -------------------------------------------------------------------------*/

apr_status_t s_handle_notify_cb (
    const void         *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_byte_t   partial,
    const apr_byte_t   out_of_band,
    const apr_byte_t   recovery,
    const apr_byte_t   delivered,
    const apr_byte_t   redelivered,
    const apr_byte_t   streaming,
    const char         *dest_name,
    const apr_uint32_t body_size,
    const apr_byte_t   persistent,
    const apr_byte_t   priority,
    const apr_uint32_t expiration,
    const char         *mime_type,
    const char         *encoding,
    const char         *identifier,
    const apr_size_t   headers_size,
    const char         *headers,
    const char         *data
    )
{
    client_t
        *client = (client_t*) hint;     /*  Client object                    */
    apr_status_t
        result;                         /*  Stores return values             */
    char
        buffer [BUFFER_SIZE];           /*  Multipurpose buffer              */

    if (client->state == state_waiting_for_browse_confirmation) {
        s_trace ("Message %s browsed successfully.\n", identifier);
        s_browse_next (client, handle_id);
        return APR_SUCCESS;
    }
    if (client->state == state_waiting_for_message_or_acknowledge_confirmation) {
    
        s_trace ("Message %s received. (%ld bytes)\n",
            identifier, (long) body_size);

        client->tag++;

        /* If N messages where already received, acknowledge them
        */
        client->till_acknowledge--;
        if (!client->till_acknowledge)
        {
            result = amqp_channel_ack (client->sck, buffer, BUFFER_SIZE,
                1, client->tag, message_nbr);
            if (result != APR_SUCCESS) {
                s_trace ("amqp_channel_ack failed: err=%d (%s)\n",
                    (long) result, amqp_strerror (result, buffer, BUFFER_SIZE));
                return result;
            }
            client->till_acknowledge = client->prefetch;
        }
        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

