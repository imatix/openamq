/*---------------------------------------------------------------------------
 *  test_level1.c - Test client for Level 1 AMQ Client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "base.h"                       /*  Base definitions                 */
#include "base_apr.h"                   /*  APR definitions                  */
#include "amqp_level1.h"                /*  Level 1 API definitions          */


/*- Definitions ------------------------------------------------------------ */

/*  Scenario types client may use                                            */
typedef enum
{
    clienttype_undefined,               /*  Invalid type                     */
    clienttype_producer,                /*  Sends messages to server         */
    clienttype_consumer,                /*  Receives messages from server    */
    clienttype_query                    /*  Queries server for messages      */
} clienttype_t;

/*  Client object                                                            */
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
} client_t; 

/*- Main program ----------------------------------------------------------- */

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
    amqp_connection_t
        connection;                     /*  Connection object                */
    amqp_channel_t
        channel;                        /*  Channel object                   */
    amqp_handle_t
        handle;                         /*  Handle object                    */
    amqp_query_t
        query;                          /*  Query object                     */
    apr_uint32_t
        message_nbr;                    /*  Individual query result          */
    char
        identifier [100];               /*  Buffer to construct message      */
                                        /*  identifier                       */
    amqp_frame_t
        *frame;                         /*  Frame to hold received message   */

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

    /*  Make sure that mandatory parameters were supplied                    */
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

    /*  Create message to be sent                                            */
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

    result = amqp_init1 ();
    if (result != APR_SUCCESS) {
        printf ("amqp_init1 failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_open_connection (
        client.server,
        client.host,
        client.client_name,
        amqp_heartbeats_off,
        amqp_heartbeats_off,
        0,
        0,
        &connection
    );
    if (result != APR_SUCCESS) {
        printf ("amqp_open_connection failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_open_channel (
        connection,
        /*  Transacted                                                       */
        (apr_byte_t) ((client.clienttype == clienttype_consumer ||
            client.commit_count || client.rollback_count) ? 1 : 0),
        0,
        0,
        &channel
    );
    if (result != APR_SUCCESS) {
        printf ("amqp_open_channel_failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_open_handle (
        channel,
        amqp_service_type_queue,
        client.clienttype == clienttype_producer ? 1: 0,
        client.clienttype == clienttype_consumer ? 1: 0,
        client.clienttype == clienttype_query ? 1: 0,
        0,
        "",
        "",
        "",
        0,
        NULL,
        &handle
    );
    if (result != APR_SUCCESS) {
        printf ("amqp_open_handle failed\n");
        return EXIT_FAILURE;
    }

    if (client.clienttype == clienttype_producer) {
        while (1) {
            
            sprintf (identifier, "%s-%ld", client.client_name,
                client.last_message_number);

            result = amqp_send_message (
                handle,
                0,
                0,
                0,
                client.destination,
                client.persistent,
                0,
                0,
                "",
                "",
                identifier,
                client.message_size,
                client.message_buffer,
                0
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_send_message failed\n");
                return EXIT_FAILURE;
            }
            printf ("Message %s sent.\n", identifier);

            /*  Rollback transaction if needed                               */
            if (client.rollback_count && ((client.last_message_number + 1) %
                  client.rollback_count) == 0) {
                result = amqp_rollback (channel, 0);
                if (result != APR_SUCCESS) {
                    printf ("amqp_rollback failed\n");
                    return EXIT_FAILURE;
                }
            }
            /*  Commit transaction if needed                                 */
            else 
            if (client.commit_count && ((client.last_message_number + 1) %
                  client.commit_count) == 0) {
                result = amqp_commit (channel, 0);
                if (result != APR_SUCCESS) {
                    printf ("amqp_commit failed\n");
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

    if (client.clienttype == clienttype_consumer) {
        result = amqp_consume (
            handle,
            client.prefetch,
            0,
            0,
            client.destination,
            "",
            "",
            0
            );
       if (result != APR_SUCCESS) {
            printf ("amqp_consume failed\n");
            return EXIT_FAILURE;
       }

        while ((!client.messages) || client.messages--) {
            result = amqp_receive_message (
                handle,
                &frame
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_receive_message failed\n");
                return EXIT_FAILURE;
            }
            
            printf ("Message %s received.\n",
                frame->fields.handle_notify.identifier);

            if (--client.till_acknowledge == 0) {
                result = amqp_acknowledge (
                    channel,
                    frame->fields.handle_notify.message_nbr,
                    0
                    );
                if (result != APR_SUCCESS) {
                    printf ("amqp_acknowledge failed\n");
                    return EXIT_FAILURE;
                }

                result = amqp_commit (
                    channel,
                    0
                    );
                if (result != APR_SUCCESS) {
                    printf ("amqp_commit failed\n");
                    return EXIT_FAILURE;
                }

                client.till_acknowledge = client.prefetch;
            }
            
            result = amqp_destroy_message (
                frame
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_destroy_message failed\n");
                return EXIT_FAILURE;
            } 
        }
    }

    if (client.clienttype == clienttype_query) {
        result = amqp_query (
            handle,
            0,
            client.destination,
            "",
            1,
            &query
            );
        if (result != APR_SUCCESS) {
            printf ("amqp_query failed\n");
            return EXIT_FAILURE;
        }

        while (1) {
            result = amqp_get_query_result (
                query,
                &message_nbr
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_get_query_result failed\n");
                return EXIT_FAILURE;
            }

            result = amqp_browse (
                handle,
                message_nbr,
                0,
                &frame
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_browse failed\n");
                return EXIT_FAILURE;
            }
            
            printf ("Message %s browsed.\n",
                frame->fields.handle_notify.identifier);

            result = amqp_destroy_message (
                frame
                );
            if (result != APR_SUCCESS) {
                printf ("amqp_destroy_message failed\n");
                return EXIT_FAILURE;
            } 
        }

        result = amqp_close_query (
            query
            );
        if (result != APR_SUCCESS) {
            printf ("amqp_close_query failed\n");
            return EXIT_FAILURE;
        }
    }

    result = amqp_close_handle (handle, 0);
    if (result != APR_SUCCESS) {
        printf ("amqp_close_handle failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_close_channel (channel, 0);
    if (result != APR_SUCCESS) {
        printf ("amqp_close_channel failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_close_connection (connection, 1);
    if (result != APR_SUCCESS) {
        printf ("amqp_close_connection failed\n");
        return EXIT_FAILURE;
    }

    result = amqp_term1 ();
    if (result != APR_SUCCESS) {
        printf ("amqp_term1 failed\n");
        return EXIT_FAILURE;
    }
    
    apr_terminate();
    return 0; 
}
