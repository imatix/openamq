/*---------------------------------------------------------------------------
 *  test_level1.c - Test client for Level 1 AMQ Client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_client.h"

typedef enum
{
    clienttype_undefined,               /*  Invalid type                     */
    clienttype_producer,                /*  Sends messages to server         */
    clienttype_consumer,                /*  Receives messages from server    */
    clienttype_query                    /*  Queries server for messages      */
} clienttype_t;

typedef struct
{
    clienttype_t
        clienttype;                     /*  Type of client                   */
    const char
        *server;                        /*  Server to connect to             */
    const char
        *host;                          /*  Virtual host to connect to       */
    const char
        *destination;                   /*  Destination to connect to        */
    const char 
        *client_name;                   /*  Name of the client, used also as */
                                        /*  prefix for message identifiers   */
    int
        till_acknowledge;               /*  Number of messages to be         */
                                        /*  received till acknowledge is to  */
                                        /*  be sent                          */
    long
        last_message_number;            /*  Messages are numbered so that    */
                                        /*  each can have unique identifier  */
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
        *query_result;                  /*  Resultset returned by query      */
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
        last_query_result;              /*  When s_move_to_next_query_result */
                                        /*  is called, this variable stores  */
                                        /*  the result                       */
} client_t;

/*  -------------------------------------------------------------------------
    Function: s_move_to_next_query_result

    Synopsis:
    Parses next query result from existing query result string
    (one returned by HANDLE INDEX) and stores it into last_query_result.

    Arguments:
        client_t *client  Client object
    -------------------------------------------------------------------------*/

apr_uint32_t s_move_to_next_query_result (
    client_t *client
    )
{
    int
        pos;

    /*  If we are inside a range specifier, return next value
    */
    if (client->browse_begin < client->browse_end) {
        client->last_query_result = client->browse_begin++;
        return client->last_query_result;
    }
    else
    /*  If we are at the end of query result string,
        there are no more results
    */
    if (client->query_result [client->query_result_pos] == '\0') {
        free (client->query_result);
        client->last_query_result = -1;
        return client->last_query_result;
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
        return client->last_query_result;
    }
}
 
int main(
    int        argc,
    const char *const argv[],
    const char *const env[]) 
{
    apr_status_t
        result;                         /*  Stores return values             */
    int 
        arg_pos;                        /*  Loop control variable            */
    int
        buff_pos;                       /*  Loop control variable            */
    client_t
        client;                         /*  Main client object               */
    amq_stdc_connection_t
        connection;                     /*  Connection object                */
    amq_stdc_channel_t
        channel;                        /*  Channel object                   */
    amq_stdc_handle_t
        handle;                         /*  Handle object                    */
    apr_uint32_t
        message_nbr;                    /*  Individual query result          */
    char
        identifier [100];               /*  Buffer to construct message      */
                                        /*  identifier                       */
    amqp_frame_t
        *frame;                         /*  Frame to hold received message   */
    apr_byte_t
        transacted;                     /*  Whether channel has to be        */
                                        /*  transacted                       */

    client.clienttype          = clienttype_undefined;
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
        printf (
            "Usage: test_level1 MODE OPTIONS ...\n"
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
        return EXIT_FAILURE;
    }

    /*  Create message  to be sent
    */
    client.message_buffer = malloc (client.message_size);
    if (!client.message_buffer) {
        printf ("Not enough memory for message body.");
        return EXIT_FAILURE;
    }
    for (buff_pos = 0; buff_pos != client.message_size; buff_pos++) 
        client.message_buffer [buff_pos] = buff_pos % 256;

    result = apr_app_initialize (&argc, &argv, &env);
    if (result != APR_SUCCESS) {
        printf ("apr_app_initialise failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_init ();
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_init failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_open_connection (client.server, client.host,
        client.client_name, amq_stdc_heartbeats_off, amq_stdc_heartbeats_off,
        0, 0, &connection);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_connection failed\n");
        return EXIT_FAILURE;
    }
    transacted = (apr_byte_t) ((client.clienttype == clienttype_consumer ||
            client.commit_count || client.rollback_count) ? 1 : 0);
    result = amq_stdc_open_channel (connection, transacted, 0, 0, &channel);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_channel_failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_open_handle (channel, amq_stdc_service_type_queue,
        client.clienttype == clienttype_producer ? 1: 0,
        client.clienttype == clienttype_consumer ? 1: 0,
        client.clienttype == clienttype_query ? 1: 0,
        0, "", "", "", 0, NULL, &handle);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_handle failed\n");
        return EXIT_FAILURE;
    }

    /*  Mode : PRODUCER                                                      */
    if (client.clienttype == clienttype_producer) {
        while (1) {
            
            sprintf (identifier, "%s-%ld", client.client_name,
                client.last_message_number);

            result = amq_stdc_send_message (handle, 0, 0, 0,
                client.destination, client.persistent, 0, 0, "", "",
                identifier, client.message_size, client.message_buffer, 0);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_send_message failed\n");
                return EXIT_FAILURE;
            }

            /*  Rollback transaction if needed                               */
            if (client.rollback_count && ((client.last_message_number + 1) %
                  client.rollback_count) == 0) {
                result = amq_stdc_rollback (channel, 0);
                if (result != APR_SUCCESS) {
                    printf ("amq_stdc_rollback failed\n");
                    return EXIT_FAILURE;
                }
            }
            /*  Commit transaction if needed                                 */
            else 
            if (client.commit_count && ((client.last_message_number + 1) %
                  client.commit_count) == 0) {
                result = amq_stdc_commit (channel, 0);
                if (result != APR_SUCCESS) {
                    printf ("amq_stdc_commit failed\n");
                    return EXIT_FAILURE;
                }
            }

            client.last_message_number++;

            if (client.messages)
                if(--client.messages == 0)
                    break;

            if (client.interval)
                apr_sleep (client.interval * 1000);
        }
    }

    /*  Mode : CONSUMER                                                      */
    if (client.clienttype == clienttype_consumer) {
        result = amq_stdc_consume (handle, client.prefetch, 0, 0,
            client.destination, "", "", 0);
        if (result != APR_SUCCESS) {
            printf ("amq_stdc_consume failed\n");
            return EXIT_FAILURE;
        }

        while ((!client.messages) || client.messages--) {
            result = amq_stdc_get_message (handle, &frame);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_receive_message failed\n");
                return EXIT_FAILURE;
            }
            
            printf ("Message %s received.\n",
                frame->fields.handle_notify.identifier);

            if (--client.till_acknowledge == 0) {
                result = amq_stdc_acknowledge (channel,
                    frame->fields.handle_notify.message_nbr, 0);
                if (result != APR_SUCCESS) {
                    printf ("amq_stdc_acknowledge failed\n");
                    return EXIT_FAILURE;
                }

                result = amq_stdc_commit (channel, 0);
                if (result != APR_SUCCESS) {
                    printf ("amq_stdc_commit failed\n");
                    return EXIT_FAILURE;
                }

                client.till_acknowledge = client.prefetch;
            }
            
            result = amq_stdc_destroy_message (frame);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_destroy_message failed\n");
                return EXIT_FAILURE;
            } 
        }
    }

    /*  Mode : QUERY                                                         */
    if (client.clienttype == clienttype_query) {
        result = amq_stdc_query (handle, 0, client.destination, "", 1,
            &(client.query_result));
        if (result != APR_SUCCESS) {
            printf ("amq_stdc_query failed\n");
            return EXIT_FAILURE;
        }

        /*  Move current position to the beginning of the query              */
        client.query_result_pos = 0;
        client.browse_begin = 0;
        client.browse_end = 0;
        client.last_query_result = 0;

        while (1) {
            message_nbr = s_move_to_next_query_result (&client);
            if (message_nbr == -1)
                break;

            result = amq_stdc_browse (handle, message_nbr, 0, &frame);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_browse failed\n");
                return EXIT_FAILURE;
            }
            
            printf ("Message %s browsed.\n",
                frame->fields.handle_notify.identifier);

            result = amq_stdc_destroy_message (frame);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_destroy_message failed\n");
                return EXIT_FAILURE;
            } 
        }

        result = amq_stdc_destroy_query (client.query_result);
        if (result != APR_SUCCESS) {
            printf ("amq_stdc_destroy_query failed\n");
            return EXIT_FAILURE;
        }
    }

    result = amq_stdc_close_handle (handle);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_handle failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_close_channel (channel);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_channel failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_close_connection (connection);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_connection failed\n");
        return EXIT_FAILURE;
    }

    result = amq_stdc_term ();
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_term failed\n");
        return EXIT_FAILURE;
    }
    
    apr_terminate();
    return 0; 
}
