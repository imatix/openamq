/*  -------------------------------------------------------------------------
 *
 *  amqpcli_get - AMQP/Fast prototype get client
 *
 *  Copyright (c) 1991-2004 iMatix Corporation
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_sclient.h"

#define CLIENT_VERSION "0.7"

/* ESIGET ESItranDir  LocalName  Route  Timeout  Timer  CommandFile EncryptKEY  */
int
main (int argc, char *argv [])
{
    int
        rc = 0;
    char
        *esitrandir,
        *localname,
        *route,
        *timeout,
        *timer,
        *commandfile,
        *encryptkey,
        *fullname,
        *clientname   = "amqpcli_get client",
        *login        = "user",
        *password     = "pass",
        *hostname     = "localhost",
        *virtual_path = "/test";
    qbyte
        msecs;
    FILE
        *file;
    amq_sclient_t
        *amq_client;
    dbyte
        handle;
    Bool
        partial;
    amq_message_t
        *message;
    amq_bucket_t
        *fragment;
    int
        fileerror;
    char
        *proc_argv [2];
    PROCESS
        proc_id;

    /*  Set up logging                                                       */
    console_send     (NULL, TRUE);
    console_capture  ("amqpcli_get.log", 'w');
    console_set_mode (CONSOLE_DATETIME);

    coprintf ("AMQP/Fast prototype client v" CLIENT_VERSION);
    
    if (argc < 4) {
        coprintf ("Usage: amqpcli_get ESItranDir LocalName Route Timeout Timer CommandFile EncryptKEY");
        return (0);
    }
    
    esitrandir  = argv [1];
    localname   = argv [2];
    route       = argv [3];
    timeout     = argc > 4 ? argv [4] : "0";
    timer       = argc > 5 ? argv [5] : NULL;
    commandfile = argc > 6 ? argv [6] : NULL;
    encryptkey  = argc > 7 ? argv [7] : NULL;
    
    amq_client = amq_sclient_new (clientname, login, password);
    if (amq_client) {
        if (amq_sclient_connect (amq_client, hostname, virtual_path)) {
            msecs = (qbyte) atol (timeout) * 1000;
            handle = amq_sclient_consumer (amq_client, route, 1, FALSE);
            message = amq_sclient_msg_read (amq_client, msecs);
            if (! message) {
                coprintf ("E: Error reading message from server");
                rc = -1;
            }
            else {
                fullname = file_where ('s', ".", localname, NULL);
                file = file_open (fullname, 'w');
                if (! file) {
                    coprintf ("E: Error opening '%s': %s", fullname, strerror (errno));
                    rc = -1;
                }
                if (! rc) {
                    fragment = amq_bucket_new (AMQ_BUCKET_MAX_SIZE);
                    do {
                        partial = amq_message_replay (message, fragment, AMQ_BUCKET_MAX_SIZE);
                        amq_bucket_save (fragment, file);
                    } while (partial
                            && (!ferror (file)));
                    amq_bucket_destroy (&fragment);
                    
                    fileerror = ferror (file);
                    file_close (file);
                    if (!fileerror) {
                        if (commandfile) {
                            proc_argv [0] = fullname;
                            proc_argv [1] = NULL;
                            proc_id = process_create (commandfile, proc_argv, 
                                                    NULL, NULL, NULL, NULL, NULL,
                                                    TRUE);   /*  Wait for process  */
                            if (proc_id == 0)  {        /*  Couldn't create process  */
                                coprintf ("E: Error running command: '%s %s'",
                                        commandfile, fullname);
                                rc = -1;
                            }
                        }
                    }
                    else {
                        coprintf ("E: Error saving '%s': %s", 
                                    fullname, strerror (fileerror));
                        rc = -1;
                    }
                }
                if (! rc)
                    amq_sclient_msg_ack   (amq_client);
                else
                    amq_sclient_msg_unget (amq_client);
            }
            amq_sclient_close (amq_client, 0);
        }
        else {
            coprintf ("Error connecting to amq server.");
            rc = -1;
        }
        amq_sclient_destroy (&amq_client);
    }
    else {
        coprintf ("Error creating client object.");
        rc = -1;
    }
    
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());
    icl_mem_assert ();
    return (rc);
}
