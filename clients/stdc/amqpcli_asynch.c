/*  -------------------------------------------------------------------------
 *
 *  amqpcli_asynch - AMQP asynchronous client demonstration
 *
 *  Copyright (c) 1991-2004 iMatix Corporation
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_aclient.h"

#define CLIENT_VERSION "0.7"

static int
    consumer,
    producer;

/*  Number of full trip messages                                             */
static dbyte
    test_size   = 5,
    window_size = 5;

/*  Prototypes for our callback functions                                    */
static void s_connected      (amq_aclient_connected_t      *args);
static void s_handle_created (amq_aclient_handle_created_t *args);
static void s_handle_notify  (amq_aclient_handle_notify_t  *args);

int
main (int argc, char *argv [])
{
    char
        *hostname,
        *clientname;
    amq_aclient_t
        *amq_client;

    console_send     (NULL, TRUE);
    console_capture  ("amqpcli.log", 'w');
    console_set_mode (CONSOLE_DATETIME);

    hostname   = argc > 1? argv [1]: "localhost";
    clientname = argc > 2? argv [2]: "test client 1";
    producer   = argc > 3? atoi (argv [3]): 1;
    consumer   = argc > 4? atoi (argv [4]): 1;

    coprintf ("AMQP asynchronous client v" CLIENT_VERSION);
    coprintf ("Connecting to %s using identity '%s'...", hostname, clientname);

    amq_client = amq_aclient_new (clientname, "test-login", "test-password");
    amq_aclient_register (amq_client, AMQ_ACLIENT_CONNECTED,      s_connected);
    amq_aclient_register (amq_client, AMQ_ACLIENT_HANDLE_CREATED, s_handle_created);
    amq_aclient_register (amq_client, AMQ_ACLIENT_HANDLE_NOTIFY,  s_handle_notify);
    amq_aclient_connect  (amq_client, hostname, "/test");

    amq_aclient_destroy (&amq_client);
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());
    icl_mem_assert ();
    return (0);
}

static void
s_connected (amq_aclient_connected_t *args)
{
    dbyte
        channel_id;
    dbyte
        smallq_id;
    amq_message_t
        *message;
    uint
        index;

    channel_id = amq_aclient_channel_open (args->client, FALSE, FALSE);

#if 0
    dbyte
        tempq_id;

    /*  Open temporary queue                                                 */
    tempq_id = amq_aclient_handle_open (args->client, channel_id, TRUE, NULL);
#endif

    /*  Open pre-configured test queues                                      */
    smallq_id = amq_aclient_handle_open (args->client, channel_id, FALSE, "test-small");

#if 0
    dbyte
        largeq_id;
    largeq_id = amq_aclient_handle_open (args->client, channel_id, FALSE, "test-large");
#endif

    /*  Register as consumer for both queues                                 */
    if (consumer) {
        amq_aclient_handle_consume (
            args->client,               /*  Ourselves                        */
            smallq_id,                  /*  Small queue handle               */
            window_size,                /*  Our window size                  */
            TRUE,                       /*  No local delivery                */
            FALSE,                      /*  Auto-ack at server side          */
            NULL,                       /*  Destination name                 */
            NULL);                      /*  Subscription name                */
#if 0
        amq_aclient_handle_consume (args->client, largeq_id, 2, TRUE, NULL, NULL);
#endif
    }
    if (producer) {
        /*  Send some small messages to the small queue                      */
        for (index = 0; index < test_size; index++) {
            message = amq_message_new ();
            amq_message_testfill       (message, 100);
            amq_message_set_persistent (message, TRUE);
            amq_aclient_handle_send (args->client, smallq_id, message, NULL);
        }
#if 0
        /*  Send some large messages to the large queue                      */
        for (index = 0; index < 3; index++) {
            message = amq_message_new ();
            amq_message_testfill (message, 100000);
            amq_aclient_handle_send (args->client, largeq_id, message, NULL);
        }
#endif
    }
}

/*  Callback functions for callbacktest                                          */

static void
s_handle_created (amq_aclient_handle_created_t *args)
{
    coprintf ("Temporary queue created, name='%s'", args->dest_name);
}

static void
s_handle_notify (amq_aclient_handle_notify_t *args)
{
    static uint
        received = 0;

    amq_aclient_channel_ack (args->client, 1, args->message_nbr);
    amq_message_destroy (&args->message);
    received++;
    if (received == test_size) {
        coprintf ("%ld messages received and acknowledged individually", test_size);
        amq_aclient_connection_close (args->client);
    }
    else
    if (received == 1)
        coprintf ("Received first message...");
}
