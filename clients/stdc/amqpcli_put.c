/*  -------------------------------------------------------------------------
 *
 *  amqpcli_put - AMQP/Fast prototype put client
 *
 *  Copyright (c) 1991-2004 iMatix Corporation
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_sclient.h"
#include "amq_classes.h"

#define CLIENT_VERSION "0.7"

/* ESIPUT  LocalDir   LocalName  ESItranDir  Route  Timeout  ControlRec  DeleteInd EncryptKEY*/

int
main (int argc, char *argv [])
{
    int
        rc = 0;
    char
        *localdir,
        *localname,
        *esitrandir,
        *route,
        *timeout,
        *controlrec,
        *deleteind,
        *encryptkey,
        *fullname,
        *clientname   = "amqpcli_put client",
        *login        = "user",
        *password     = "pass",
        *hostname     = "localhost",
        *virtual_path = "/test";
    FILE
        *file;
    amq_sclient_t
        *amq_client;
    dbyte
        handle;
    amq_message_t
        *message;
    amq_bucket_t
        *fragment;
    int
        fileerror;

    /*  Set up logging                                                       */
    console_send     (NULL, TRUE);
    console_capture  ("amqpcli_put.log", 'w');
    console_set_mode (CONSOLE_DATETIME);

    coprintf ("AMQP/Fast prototype client v" CLIENT_VERSION);
    
    if (argc < 5) {
        coprintf ("Usage: amqpcli_put LocalDir LocalName ESItranDir Route Timeout ControlRec DeleteInd EncryptKEY");
        return (0);
    }
    
    localdir   = argv [1];
    localname  = argv [2];
    esitrandir = argv [3];
    route      = argv [4];
    timeout    = argc > 5 ? argv [5] : NULL;
    controlrec = argc > 6 ? argv [6] : NULL;
    deleteind  = argc > 7 ? argv [7] : "n";
    encryptkey = argc > 8 ? argv [8] : NULL;
    
    fullname = file_where ('s', localdir, localname, NULL);
    file = file_open (fullname, 'r');
    if (! file) {
        coprintf ("E: Local file '%s' not found.", fullname);
        rc = -1;
    }
    if (! rc) {
        amq_client = amq_sclient_new (clientname, login, password);
        if (amq_client
        &&  amq_sclient_connect (amq_client, hostname, virtual_path)) {
    
            handle = amq_sclient_producer (amq_client, route);
            
            message  = amq_message_new ();
            fragment = amq_bucket_new ();
            while ((!feof   (file))
               &&  (!ferror (file))) {
                amq_bucket_load (fragment, file, AMQ_BUCKET_SIZE);
                amq_message_record (message, fragment, (Bool) !feof (file));
            }
            amq_bucket_destroy (&fragment);
            
            fileerror = ferror (file);
            file_close (file);
            if (fileerror) {
                coprintf ("E: Error loading local file '%s': %s", 
                          fullname, strerror (fileerror));
                amq_message_destroy (&message);
            }
            else {
                amq_sclient_msg_send (amq_client, handle, message);
                if (lexcmp (deleteind, "y") == 0)
                    file_delete (fullname);
            }                
            amq_sclient_close (amq_client, 0);
        }
        else {
            coprintf ("Error connecting to amq server.");
            rc = -1;
        }
        amq_sclient_destroy (&amq_client);
    }
    
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());
    icl_mem_assert ();
    return (rc);
}
