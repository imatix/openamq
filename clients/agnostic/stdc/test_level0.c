/*---------------------------------------------------------------------------
 *  test_level0.c - code example for Level 0 AMQ Client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

/*  PH: code review
    2005/03/04
    - indentation is generally OK, but there are many other problems
    - no clear program structure
        - I expect to see the main at the start and be able to READ the code
        - take a look at amqpcli_serial.c
    - use prototypes for internal functions
    - comment functions
    - do NOT use global variables to pass data around, this is terrible
    - do nt use shrt nms nls U xpct ppl 2 wkr rlly hrd to ndrstd yr cd!
    - consider using a tool like Libero to structure this program
        - especially since you are using states
    - gotos are not allowed unless there is no alternative (and there is here)
    - do not include system headers in your programs

    Score:
        - general syntax:   6/10
        - program structure:    5/10
        - variable names:    7/10
        - conformity with RFC: 4/10
        - readability: 5/10

    You should be aiming to get a 9/10 or 10/10 for every score item.

    I recommend you:
        - reread the RFC
        - study some iMatix code

 */

#include "amqp_level0.h"

/*  PH: Do not include system files like this - put these into
    a standard project file or use sfl.h  */
#include <stdio.h>
#include <stdlib.h>

/*  PH: what is this structure?  no comments, names like 'x', 'b'            */
typedef struct
{
    void *hint;
    apr_byte_t type;
    union
    {
        struct
        {
            int x;
        } a;
        struct
        {
            char *y;
        } b;
    } fields;
} frame_t;

typedef enum
{
    apptype_undefined,
    apptype_producer,
    apptype_consumer,
    apptype_query
} apptype_t;

/*  PH: global variables?  use 'static' and indent neatly   */

apptype_t apptype;
apr_socket_t *sck;
apr_uint16_t tag;
const char *server;
const char *host;
const char *destination;
const char *prefix;
long messages;
long message_number;
int interval;
int prefetch;
int till_acknowledge;
int message_size;
char *message_buffer;
apr_byte_t persistent;
long commit_count;
long rollback_count;
char *query_result;
int query_result_pos = 0;
int browse_begin;
int browse_end;
apr_uint32_t last_query_result;
int stop;

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
    /*  waiting for acknowledge confirmations as well  */
    state_waiting_for_message,
    state_waiting_for_index,
    state_waiting_for_browse_confirmation,
    state_idle
} state_t;

/*  PH: what is this?  */
state_t state;

/*  PH: functions must be commented, there is a standard for this  */
apr_status_t handle_close_cb (
    const void *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    fprintf (stderr, "Server closed the handle.\n%ld : %s\n",
        (long) reply_code, reply_text);
/*  PH: abort, then return?  */
    abort ();
    return APR_SUCCESS;
}

apr_status_t channel_close_cb (
    const void *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    fprintf (stderr, "Server closed the channel.\n%ld : %s\n",
        (long) reply_code, reply_text);
    abort();
    return APR_SUCCESS;
}

apr_status_t connection_close_cb (
    const void *hint,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    fprintf (stderr, "Server closed the connection.\n%ld : %s\n",
        (long) reply_code, reply_text);
    abort ();
    return APR_SUCCESS;
}

/*  PH: no arguments - this is not standard ANSI C
    also, do not pass results via global variables, we are not writing
    COBOL.  This function does something quite specific (I would have to
    guess because it's not explained) but what?  What arguments?  What
    return values?  Impossible to know without reading everything, and
    that's no good...

  */
void next_query_result ()
{
    int pos;

    if (browse_begin != browse_end) {
        last_query_result = browse_begin++;
        return;
    }

    if (query_result [query_result_pos] == 0) {
        free (query_result);
        last_query_result = 0;
        return;
    }

    pos = query_result_pos;
    browse_begin = 0;
    while (query_result [pos] >='0' && query_result [pos] <='9')
        browse_begin = browse_begin * 10 + query_result [pos++] - '0';

    if (query_result [pos] == '-') {
        pos++;
        browse_end = 0;
            while (query_result [pos] >='0' && query_result [pos] <='9')
                browse_end = browse_end * 10 + query_result [pos++] - '0';
    }
/*  PH: statements are not correctly formatted.  do not write oneliners */
    else browse_end = browse_begin + 1;

    while (query_result [pos] == ' ') pos++;

    query_result_pos = pos;

    last_query_result = browse_begin++;
}

apr_status_t browse_next (apr_uint16_t handle_id)
{
    apr_status_t result;
    char buffer [32768];

    next_query_result ();
    if (last_query_result) {
        result = amqp_handle_browse (sck, buffer, 32767,
            handle_id, tag++, last_query_result);
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_handle_browse failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );

            return result;
        }
        state = state_waiting_for_browse_confirmation;
    }
/*  PH: again, 'else' comes on a line of its own  */
    else state = state_idle;

    return APR_SUCCESS;
}

apr_status_t send_message(apr_uint16_t confirm_tag)
{
    apr_status_t result;
    char buffer [32768];
    char identifier [2000];

    /* wait for N milliseconds seconds */
    if (interval)
        apr_sleep (interval * 1000);

    sprintf (identifier, "%s%ld", prefix, message_number);

    result = amqp_handle_send (sck, buffer, 32767, 1, confirm_tag, 0, 0, 0, 0, "",
        message_size, persistent, 0, 0, "", "", identifier, 0, "", message_buffer);
    if (result != APR_SUCCESS) {
/*  PH: please use coprintf for output  */
        fprintf (stderr, "amqp_handle_send failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }

    fprintf (stderr, "Message %s sent. (%ld bytes)\n", identifier, (long) message_size);

    if (rollback_count && (message_number + 1) % rollback_count == 0) {
        result = amqp_channel_rollback (sck, buffer, 32767, 1, 0, 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_channel_rollback failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        fprintf (stderr, "Rollbacked.\n");
    }
    else if (commit_count && (message_number + 1) % commit_count == 0) {
        result = amqp_channel_commit (sck, buffer, 32767, 1, 0, 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_channel_commit failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        fprintf (stderr, "Commited.\n");
    }

    message_number++;

    state = state_waiting_for_send_confirmation;

    return APR_SUCCESS;
}

apr_status_t handle_index_cb (
    const void *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_size_t message_list_size,
    const char* message_list
    )
{
    if (state == state_waiting_for_index) {
        fprintf (stderr, "Result of query: %s\n", message_list);

        query_result = malloc (strlen (message_list) + 1);
        strcpy (query_result , message_list);
        query_result_pos = 0;

        browse_next (handle_id);

        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

apr_status_t handle_reply_cb (
    const void *hint,
    const apr_uint16_t handle_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (state == state_waiting_for_handle_open_confirmation) {
        /* receiver : reply to handle open */

        fprintf (stderr, "Handle opened.\n");

        if (apptype == apptype_consumer) {
            result = amqp_handle_consume (sck, buffer, 32767, 1, 4,
                (apr_uint16_t) prefetch, 0, 0, "", "", 0, "", "");
            if (result != APR_SUCCESS) {
                fprintf (stderr, "amqp_handle_consume failed.\n%ld : %s\n",
                    (long) result, amqp_strerror (result, buffer, 32767) );
                return result;
            }
            state = state_waiting_for_consume_confirmation;
        }

        if (apptype == apptype_producer) {
            return send_message ( (apr_uint16_t) (confirm_tag + 1) );
        }

        if (apptype == apptype_query) {
            fprintf (stderr, "Issuing query.\n");
            result = amqp_handle_query (sck, buffer, 32768, 1, 0, "", 0, "", "");
            if (result != APR_SUCCESS) {
                fprintf (stderr, "amqp_handle_query failed.\n%ld : %s\n",
                    (long) result, amqp_strerror (result, buffer, 32767) );
                return result;
            }
            state = state_waiting_for_index;
        }

        return APR_SUCCESS;
    }
    if (state == state_waiting_for_consume_confirmation) {
        /* receiver : reply to handle consume */
        tag = confirm_tag;
        state = state_waiting_for_message;
        return APR_SUCCESS;
    }
    if (state == state_waiting_for_send_confirmation) {
        /* sender : reply to handle send */
        if (messages)
        {
            messages--;
            if (messages == 0) stop = 1;
        }
        if (!stop) send_message ( (apr_uint16_t) (confirm_tag + 1) );
        return APR_SUCCESS;
    }
    if (state == state_waiting_for_browse_confirmation) {
        if (reply_code != 200) {
            fprintf (stderr, "Browsing message number %ld failed with error %ld, %s\n",
                (long) last_query_result, (long) reply_code, reply_text);
            browse_next (handle_id);
        }
        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

apr_status_t channel_reply_cb (
    const void *hint,
    const apr_uint16_t channel_id,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (state == state_waiting_for_channel_open_confirmation) {
        /*  reply to channel open  */
        fprintf (stderr, "Channel opened.\n");

        result = amqp_handle_open (sck, buffer, 32767, 1, 1, 1, 3, 1, 1, 0, 0,
            destination, "", "", 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_handle_open failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        state = state_waiting_for_handle_open_confirmation;
        return APR_SUCCESS;
    }
    if (state == state_waiting_for_message) {
        /*  receiver : reply to channel ack  */
        fprintf (stderr, "Message acknowledged.\n");
        state = state_waiting_for_message;
        return APR_SUCCESS;
    }
    
    return AMQ_FRAME_CORRUPTED;
}

apr_status_t connection_reply_cb (
    const void *hint,
    const apr_uint16_t confirm_tag,
    const apr_uint16_t reply_code,
    const char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (state == state_waiting_for_connection_open_confirmation) {
        /* reply to connection open */
        result = amqp_channel_open (sck, buffer, 32767, 1, 2,
            (apr_byte_t) ( (commit_count || rollback_count) ? 1 : 0),
            0, 0, "", "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_channel_open failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        state = state_waiting_for_channel_open_confirmation;
        fprintf (stderr, "Connected to server.\n");
        return APR_SUCCESS;
    }

    return AMQ_FRAME_CORRUPTED;
}
    
apr_status_t connection_challenge_cb (
    const void *hint,
    const apr_byte_t version,
    const char *mechanisms,
    const apr_size_t challenges_size,
    const char *challenges
    )
{
    apr_status_t result;
    char buffer [32768];
    
    if (state == state_waiting_for_connection_challenge) {
        /*  reply to amq protocol initiation  */
        result = amqp_connection_response (sck, buffer, 32767, "plain", 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_connection_response failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        state = state_waiting_for_connection_tune;
        return APR_SUCCESS;
    }
    
    return AMQ_FRAME_CORRUPTED;
}

apr_status_t connection_tune_cb (
    const void *hint,
    const apr_uint32_t frame_max,
    const apr_uint16_t channel_max,
    const apr_uint16_t handle_max,
    const apr_uint16_t heartbeat,
    const apr_size_t options_size,
    const char *options
    )
{
    apr_status_t result;
    char buffer [32768];
    char data [2];
    data [0] = 1;
    data [1] = 2;

    if (state = state_waiting_for_connection_tune) {
        /*  reply to connection response  */
        result = amqp_connection_tune (sck, buffer, 32767, 32000, 255, 255, 0, 
            options_size, options);
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_connection_tune failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        result = amqp_connection_open (sck, buffer, 32767, 1, host,
            prefix, 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_connection_open failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        state = state_waiting_for_connection_open_confirmation;
        return APR_SUCCESS;
    }

    return AMQ_FRAME_CORRUPTED;
}

apr_status_t handle_notify_cb (
    const void *hint,
    const apr_uint16_t handle_id,
    const apr_uint32_t message_nbr,
    const apr_byte_t partial,
    const apr_byte_t out_of_band,
    const apr_byte_t recovery,
    const apr_byte_t delivered,
    const apr_byte_t redelivered,
    const apr_byte_t streaming,
    const char *dest_name,
    const apr_uint32_t body_size,
    const apr_byte_t persistent,
    const apr_byte_t priority,
    const apr_uint32_t expiration,
    const char *mime_type,
    const char *encoding,
    const char *identifier,
    const apr_size_t headers_size,
    const char *headers,
    const char *data
    )
{
    apr_status_t result;
    char buffer [32768];

    if (state == state_waiting_for_browse_confirmation) {
        fprintf (stderr, "Message %s browsed successfully.\n", identifier);
        browse_next (handle_id);
        return APR_SUCCESS;
    }
    if (state == state_waiting_for_message) {
    
        fprintf (stderr, "Message %s received. (%ld bytes)\n", identifier, (long) body_size);

        tag++;

        till_acknowledge--;
        if (!till_acknowledge)
        {
            result = amqp_channel_ack (sck, buffer, 32767, 1, tag, message_nbr);
            if (result != APR_SUCCESS) {
                fprintf (stderr, "amqp_channel_ack failed.\n%ld : %s\n",
                    (long) result, amqp_strerror (result, buffer, 32767) );
                return result;
            }
            till_acknowledge = prefetch;
        }
        return APR_SUCCESS;
    }
    return AMQ_FRAME_CORRUPTED;
}

int main (int argc, const char *const argv[], const char *const env[]) 
{
    apr_status_t result;
    apr_pool_t *pool = NULL;
    char *addr = NULL;
    char *scope_id = NULL;
    apr_port_t port;
    apr_sockaddr_t *sockaddr = NULL;
    char buffer [2000];
    int i;
    amqp_callbacks_t callbacks;

    memset ( (void*) &callbacks, 0, sizeof (amqp_callbacks_t) );

    apptype = apptype_undefined;
    server = "127.0.0.1";
    host = NULL;
    destination = NULL;
    prefix = "client";
    messages = 0;
    interval = 500;
    prefetch = 1;
    message_size = 2;
    message_number = 0;
    persistent = 0;
    commit_count = 0;
    rollback_count = 0;

    state = state_initial;
    stop = 0;

    for (i=1; i!=argc; i++) {
        if (strcmp (argv[i], "producer") == 0) apptype = apptype_producer;
        if (strcmp (argv[i], "consumer") == 0) apptype = apptype_consumer;
        if (strcmp (argv[i], "query") == 0) apptype = apptype_query;
        if (strncmp (argv[i], "-s", 2) == 0) server = argv [i] + 2;
        if (strncmp (argv[i], "-h", 2) == 0) host = argv [i] + 2;
        if (strncmp (argv[i], "-d", 2) == 0) destination = argv [i] + 2;
        if (strncmp (argv[i], "-m", 2) == 0) prefix = argv [i] + 2;
        if (strncmp (argv[i], "-n", 2) == 0) messages = atoi (argv [i] + 2);
        if (strncmp (argv[i], "-i", 2) == 0) interval = atoi (argv [i] + 2);
        if (strncmp (argv[i], "-p", 2) == 0) prefetch = atoi (argv [i] + 2);
        if (strncmp (argv[i], "-l", 2) == 0) message_size = atoi (argv [i] + 2);
        if (strncmp (argv[i], "-x", 2) == 0) persistent = 1;
        if (strncmp (argv[i], "-c", 2) == 0) commit_count = atoi (argv [i] + 2);
        if (strncmp (argv[i], "-r", 2) == 0) rollback_count = atoi (argv [i] + 2);
    }

    if (apptype == apptype_undefined) goto badparams;
    if (host == NULL) goto badparams;
    if (destination == NULL) goto badparams;
    till_acknowledge = prefetch;

    message_buffer = malloc (message_size);
    if (!message_buffer) {
        fprintf (stderr, "Not enough memory for message body.");
        goto err;
    }

    for (i = 0; i != message_size; i++) 
        message_buffer [i] = i % 256;

    fprintf (stderr, "Connecting to server.\n");

    result = apr_app_initialize (&argc, &argv, &env);
    if(result != APR_SUCCESS) {
        fprintf (stderr, "apr_app_initialize failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_pool_initialize();
    if(result != APR_SUCCESS) {
        fprintf (stderr, "apr_pool_initialize failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_pool_create(&pool, NULL);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_pool_create failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_socket_create (&sck, APR_INET, SOCK_STREAM, APR_PROTO_TCP, pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_socket_create failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    sprintf (buffer, "%s:7654", server);
    result = apr_parse_addr_port (&addr, &scope_id, &port, buffer, pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_parse_addr_port failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_sockaddr_info_get(&sockaddr, addr, AF_UNSPEC, port, APR_IPV4_ADDR_OK, pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_sockaddr_info_get failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_socket_connect(sck, sockaddr);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_socket_connect failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = amqp_init(sck);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_init failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }
    state = state_waiting_for_connection_challenge;

    callbacks.connection_challenge = connection_challenge_cb;
    callbacks.connection_tune = connection_tune_cb;
    callbacks.handle_notify = handle_notify_cb;
    callbacks.connection_reply = connection_reply_cb;
    callbacks.channel_reply = channel_reply_cb;
    callbacks.handle_reply = handle_reply_cb;
    callbacks.handle_index = handle_index_cb;
    callbacks.connection_close = connection_close_cb;
    callbacks.channel_close = channel_close_cb;
    callbacks.handle_close = handle_close_cb;

    while (1)
    {
        result=amqp_recv (sck, buffer, 2000, &callbacks, NULL);
        if (result != APR_SUCCESS && result != APR_TIMEUP) {
            fprintf (stderr, "amqp_recv failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 2000) );
            goto err;
        }

        if (stop) break;
    }

err:

    fprintf (stderr, "Terminating client.\n");

    result=amqp_term (sck);
    if (result != APR_SUCCESS)
        fprintf (stderr, "apr_term failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        
    result = apr_socket_close (sck);
    if (result != APR_SUCCESS)
        fprintf (stderr, "apr_socket_close failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );

    apr_terminate();

    return -1;

badparams:
    fprintf (stderr,
        "\n"
        "  agnostic producer\n"
        "    -s<server name/ip address, default='127.0.0.1'>\n"
        "    -h<virtual host name>\n"
        "    -d<destination>\n"
        "    -m<client identifier, default='client'>\n"
        "    -n<number of messages, 0 means infinite, default=0>\n"
        "    -i<interval between individual messages in ms, default=500>\n"
        "    -l<length of message content in bytes, default=2>\n"
        "    -x (sends persistent messages)\n"
        "    -c<number of messages while commit is issued>\n"
        "    -r<number of messages while rollback is issued>\n"
        "\n"
        "  agnostic consumer\n"
        "    -s<server name/ip address, default=127.0.0.1>\n"
        "    -h<virtual host name>\n"
        "    -d<destination>\n"
        "    -p<number of prefetched messages, default=1>\n"
        "\n"
        "  agnostic query (queries for all messages and browses them one by one)\n"
        "    -s<server name/ip address, default=127.0.0.1>\n"
        "    -h<virtual host name>\n"
        "    -d<destination>\n"
        "\n"
        "  Note : When neither 'c' or 'r' parameter is set\n"
        "         client works in nontransacted mode.\n");
    return -1;
}

