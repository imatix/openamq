/*  -------------------------------------------------------------------------
 *
 *  amqpcli_file - AMQP file transfer demonstration
 *
 *  Copyright (c) 1991-2004 iMatix Corporation
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_aclient.h"

#define CLIENT_VERSION "0.1"

/*  Callback functions                                                       */

static void
    s_connected (amq_aclient_connected_t *args);
static void
    s_file_monitor (amq_aclient_monitor_t *args);
static void
    s_handle_notify (amq_aclient_handle_notify_t *args);
static void
    s_channel_close (amq_aclient_channel_close_t *args);
static void
    s_handle_close (amq_aclient_handle_close_t *args);

/*  Module-global variables                                                  */

static ipr_shortstr_t
    inbox;
static ipr_shortstr_t
    outbox;
static ipr_shortstr_t
    dest_name;
static dbyte
    channel_id;
static dbyte
    handle_id;
static uint
    infilenum;
    
/*  The main function                                                        */
    
int
main (int argc, char *argv [])
{
    char
        *hostname,
        *clientname;
    amq_aclient_t
        *client;

    console_send     (NULL, TRUE);
    console_capture  ("amqpcli.log", 'w');
    console_set_mode (CONSOLE_DATETIME);

    hostname   = argc > 1? argv [1]: "localhost";
    clientname = argc > 2? argv [2]: "test client 1";
    ipr_shortstr_cpy (inbox,  argc > 3? argv [3]: "inbox");
    ipr_shortstr_cpy (outbox, argc > 4? argv [4]: "outbox");

    coprintf ("AMQP asynchronous client v" CLIENT_VERSION);
    coprintf ("Connecting to %s using identity '%s'...", hostname, clientname);

    client = amq_aclient_new (clientname, "guest", "guest");
    amq_aclient_register (client, AMQ_ACLIENT_CONNECTED, s_connected);
    amq_aclient_connect  (client, hostname, "/test");  /*  Runs the agent  */

    amq_aclient_destroy (&client);
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());
    icl_mem_assert ();
    return (0);
}

static void
s_connected (amq_aclient_connected_t *args)
{
    channel_id = amq_aclient_channel_open (args->client, FALSE, FALSE);
    handle_id  = amq_aclient_handle_open  (args->client, channel_id);

    /*  Register server-driven close feedbacks                               */
    amq_aclient_register (args->client, AMQ_ACLIENT_HANDLE_CLOSE,  s_handle_close);
    amq_aclient_register (args->client, AMQ_ACLIENT_CHANNEL_CLOSE, s_channel_close);
    
    /*  Register as consumer if inbox is defined                             */
    if (inbox [0]) {
        amq_aclient_handle_consume (
            args->client,
            handle_id,
            AMQP_SERVICE_QUEUE,
            1,                          /*  Prefetch                         */
            TRUE,                       /*  No local delivery                */
            FALSE,                      /*  Auto-ack at server side          */
            FALSE,                      /*  Dynamic consumer                 */
            FALSE,                      /*  Exclusive consumer               */
            dest_name);                 /*  Destination name                 */
        amq_aclient_register (args->client, AMQ_ACLIENT_HANDLE_NOTIFY, s_handle_notify);
    }
    /*  Register monitor function if outbox is defined                       */
    if (outbox[0])
        amq_aclient_register (args->client, AMQ_ACLIENT_MONITOR, s_file_monitor);
}

static void
s_file_monitor (amq_aclient_monitor_t *args)
{
    LIST
        *dir_list;
    FILEINFO
        *fileinfo;
    ipr_shortstr_t
        fullname;
    FILE
        *file;
    int
        fileerror;
    amq_bucket_t
        *fragment;
    amq_message_t
        *message;

    dir_list = load_dir_list (outbox, "t");
    if (dir_list) {
        fileinfo = (FILEINFO *) dir_list->next;
        fileerror = 0;
        while (fileinfo != (FILEINFO *) dir_list
           && (!fileerror)) {
            /*  Load up and send the file... */
            ipr_shortstr_cpy (fullname, file_where ('s', 
                                                    outbox, 
                                                    fileinfo->dir.file_name, 
                                                    NULL));
            if (fullname && file_stable (fullname)) {
                file = file_open (fullname, 'r');
                /*  If we can't read the file, close the connection.             */
                if (!file) {
                    coprintf ("E: can't read '%s': %s", 
                            fullname, strerror (errno));
                    amq_aclient_connection_close (args->client);
                    break;
                }
                coprintf ("I: transferring file '%s', %ld bytes", 
                        fullname, get_file_size (fullname));
                fragment = amq_bucket_new  (AMQ_BUCKET_MAX_SIZE);
                message  = amq_message_new ();
                amq_message_set_persistent (message, TRUE);
                while ((!feof   (file))
                  &&  (!ferror (file))) {
                    amq_bucket_load (fragment, file, AMQ_BUCKET_MAX_SIZE);
                    amq_message_record (message, fragment, (Bool) !feof (file));
                }
                fileerror = ferror (file);
                if (fileerror) {
                    coprintf ("E: Error loading '%s': %s", 
                            fullname, strerror (fileerror));
                    amq_aclient_connection_close (args->client);
                    amq_message_destroy (&message);
                }
                else
                    amq_aclient_handle_send (
                        args->client,
                        handle_id,
                        AMQP_SERVICE_QUEUE,
                        message,
                        dest_name,
                        FALSE);         /*  Assert immediate delivery       */
                file_delete (fullname);
            }
            fileinfo = fileinfo->next;
        }
        free_dir_list (dir_list);
    }
}

static void
s_handle_notify (amq_aclient_handle_notify_t *args)
{
    ipr_shortstr_t
        fullname;
    FILE
        *file;
    int
        fileerror;
    amq_bucket_t
        *fragment;
    Bool
        partial;
        
    ipr_shortstr_cpy (fullname, file_where ('s', 
                                            inbox, 
                                            strprintf ("%ui", ++infilenum),
                                            NULL));
    file = file_open (fullname, 'w');
    /*  If we can't read the file, close the connection.             */
    if (!file) {
        coprintf ("E: can't write '%s': %s", 
                  fullname, strerror (errno));
        amq_aclient_connection_close (args->client);
    }
    else {
        coprintf ("I: receiving file '%s', %ld bytes", 
                  fullname, args->message->body_size);
        fragment = amq_bucket_new (AMQ_BUCKET_MAX_SIZE);
        do {
            partial = amq_message_replay (args->message, fragment, AMQ_BUCKET_MAX_SIZE);
            amq_bucket_save (fragment, file);
        } while (partial
             && (!ferror (file)));
        fileerror = ferror (file);
        if (fileerror) {
            coprintf ("E: Error saving '%s': %s", 
                      fullname, strerror (fileerror));
            amq_aclient_connection_close (args->client);
        }
        else
            amq_aclient_channel_ack (args->client, channel_id, args->message_nbr);
            
        amq_bucket_destroy  (&fragment);
    }
    amq_message_destroy (&args->message);
}

static void
s_channel_close (amq_aclient_channel_close_t *args)
{
    amq_aclient_connection_close (args->client);
}

static void
s_handle_close (amq_aclient_handle_close_t *args)
{
    amq_aclient_connection_close (args->client);
}
