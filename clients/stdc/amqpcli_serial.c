/*  -------------------------------------------------------------------------
 *
 *  amqpcli_serial - AMQP/Fast prototype serial client
 *
 *  Copyright (c) 1991-2004 iMatix Corporation
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_sclient.h"

#define CLIENT_VERSION "0.7"

int
main (int argc, char *argv [])
{
    char
        *hostname,
        *clientname;
    int
        consumer,
        producer;
    amq_sclient_t
        *amq_client;
    dbyte
        in_handle = 0,
        out_handle = 0;
    amq_message_t
        *message;

    console_send     (NULL, TRUE);
    console_capture  ("amqpcli_serial.log", 'w');
    console_set_mode (CONSOLE_DATETIME);

    coprintf ("AMQP/Fast prototype client v" CLIENT_VERSION);
    hostname   = argc > 1? argv [1]: "localhost";
    clientname = argc > 2? argv [2]: "test client 1";
    producer   = argc > 3? atoi (argv [3]): 1;
    consumer   = argc > 4? atoi (argv [4]): 1;

    coprintf ("Connecting to %s using identity '%s'...", hostname, clientname);
    amq_client = amq_sclient_new (clientname, "user", "pass");
    if (amq_client
    &&  amq_sclient_connect (amq_client, hostname, "/test")) {
        if (producer) {
            out_handle = amq_sclient_producer (amq_client, "test-small");
            message = amq_message_new ();
            amq_message_testfill (message, 100);
            amq_sclient_msg_send (amq_client, out_handle, message);
        }
        if (consumer) {
            in_handle = amq_sclient_consumer (amq_client, "test-small", 1, FALSE);
            while (1) {
                message = amq_sclient_msg_read (amq_client, 5000);
                if (!message)
                    break;
                coprintf ("Message number %d arrived", amq_client->msg_number);
                if (producer)
                    amq_sclient_msg_send (amq_client, out_handle, message);
                amq_sclient_msg_ack (amq_client);
            }
        }
        amq_sclient_close (amq_client, 0);
    }
    amq_sclient_destroy (&amq_client);
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());
    icl_mem_assert ();
    return (0);
}

