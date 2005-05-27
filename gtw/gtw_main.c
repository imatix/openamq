
#include "base.h"
#include "base_apr.h"
#include "amq_stdc_client.h"
#include "gtw.h"
#include "sfl.h"

typedef int (*JAMQ_fOpenRoutine) (JAMQ_tsApicb*, int*);
typedef int (*JAMQ_fCloseRoutine) (JAMQ_tsApicb*, int*);
typedef int (*JAMQ_fMessageHandlerRoutine) (JAMQ_tsApicb*, void*,
    JAMQ_tsApiDvcb*, int *);

int application_init (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    );

int application_uninit (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    );

int main (
    int         argc,
    const char  *const argv[],
    const char  *const env[]
    ) 
{
    gtw_context_t
        context;
    JAMQ_tsHashParams
        hparams;
    XML_ITEM
        *grm_config = NULL;
    XML_ITEM
        *config = NULL;
    XML_ITEM
        *item;
    XML_ITEM
        *item2;
    XML_ITEM
        *child_item;
    XML_ATTR
        *attr1;
    XML_ATTR
        *attr2;
    void
        *fx;
    JAMQ_tsNCharcb
        key;
    JAMQ_fMessageHandlerRoutine
        message_handler = NULL;
    int
        retcode;
    apr_status_t
        result;
    amq_stdc_connection_t
        connection;
    amq_stdc_channel_t
        channel;
    dbyte
        handle_id;
    amq_stdc_message_desc_t
        *message_desc;
    amq_stdc_message_t
        message;
    const char
        *server;
    const char
        *port;
    const char
        *vhost;
    char
        *grm_config_filename;
    apr_pool_t
        *pool;

    /* Initialise APR                                                        */
    result = apr_app_initialize (&argc, &argv, &env);
    if (result != APR_SUCCESS) {
        printf ("apr_app_initialise failed\n");
        exit (1);
    }
    result = apr_pool_create (&pool, NULL);
    if (result != APR_SUCCESS) {
        printf ("apr_pool_create failed\n");
        exit (1);
    }

    if (argc != 3) {
        printf ("Command line syntax: gtw_main <instance name> "
            "<config file name>\n");
        exit (1);
    }

    /*  Load and parse grm-config file                                       */
    result = apr_env_get (&grm_config_filename, "GRM_AMQ_CONFIG", pool);
    if (result == APR_ENOENT)
        grm_config_filename = "grm-config.xml";
    else if (result != APR_SUCCESS) {
        printf ("apr_env_get failed\n");
        exit (1);
    }

    if (xml_load (&grm_config, "", grm_config_filename) != XML_NOERROR) {
        printf ("GRM configuration file cannot be opened.\n");
        exit (1);
    }
    item = xml_find_item (grm_config, "/grm-config");
    if (!item) {
        printf ("grm-config tag missing in GRM configuration file.\n");
        exit (1);
    }
    attr1 = xml_attr (item, "server");
    if (!attr1) {
        printf ("Server name missing in GRM configuration file.\n");
        exit (1);
    }
    server = xml_attr_value (attr1);
    attr1 = xml_attr (item, "port");
    if (!attr1) {
        printf ("Port number missing in GRM configuration file.\n");
        exit (1);
    }
    port = xml_attr_value (attr1);
    attr1 = xml_attr (item, "virtual-host");
    if (!attr1) {
        printf ("Virtual host name missing in GRM configuration file.\n");
        exit (1);
    }
    vhost = xml_attr_value (attr1);
    
    if (xml_load (&config, "", argv [2]) != XML_NOERROR) {
        printf ("Application configuration file cannot be opened.\n");
        exit (1);
    }

    /*  TODO: Validate config file... XML schema ?                           */

    /*  Initialisation of context                                            */
    context.appctx.argc = argc;
    context.appctx.argv = (char**) argv;
    context.appctx.sName.pData = (char*) argv [0];
    context.appctx.sName.iDataLen = strlen (argv [0]);
    context.appctx.sStateHndl.iOkToRun = 0;
    context.appctx.sStateHndl.iPermissionLevel = 1;
    context.appctx.pApplHndl = &(context.appctx_internal);
    context.appctx.pGMMReadHndl = NULL;
    context.appctx.pGMMWriteHndl = NULL;
    context.appctx.pTmHndl = NULL;
    context.appctx.pApiDvHndlList = NULL;
    context.appctx.sReadBuf.pData = NULL;
    context.appctx.sReadBuf.iCurrentLen = 0;
    context.appctx.sReadBuf.iPhysicalLen = 0;
    context.appctx.sWriteBuf.pData = NULL;
    context.appctx.sWriteBuf.iCurrentLen = 0;
    context.appctx.sWriteBuf.iPhysicalLen = 0;
    context.appctx.sBuildBuf.pData = NULL;
    context.appctx.sBuildBuf.iCurrentLen = 0;
    context.appctx.sBuildBuf.iPhysicalLen = 0;
    context.appctx.sOutputBuf.pData = NULL;
    context.appctx.sOutputBuf.iCurrentLen = 0;
    context.appctx.sOutputBuf.iPhysicalLen = 0;
    context.appctx.sLogBuf = NULL;
    context.appctx.pLogHndl = NULL; /* TODO: Should be something...          */
    context.appctx.pDvHndl = NULL;
    context.functions = NULL;
    context.services = NULL;
    context.appctx_internal.sName.pData = "";
    context.appctx_internal.sName.iDataLen = 0;
    context.appctx_internal.pApplsHndl = NULL;  /*  Application context      */


    if (JAMQ_gmm_open (&(context.appctx.pGMMReadHndl), &retcode) != OK) {
        printf ("GMM read object cannot be opened. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (JAMQ_gmm_open (&(context.appctx.pGMMWriteHndl), &retcode) != OK) {
        printf ("GMM write object cannot be opened. (%ld)\n", (long) retcode);
        exit (1);
    }
#if 0
    if (JAMQ_m_get_buffer (&(context.appctx.sReadBuf), 1024 , &retcode) != OK) {
        printf ("Buffer for reading cannot be opened. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (JAMQ_m_get_buffer (&(context.appctx.sWriteBuf), 1024 , &retcode) != OK) {
        printf ("Buffer for writing cannot be opened. (%ld)\n", (long) retcode);
        exit (1);
    }
#endif
    hparams.ihash_TableSize            = 199;
    hparams.shash_RoutineName.pData    = LTW_HASH_TOREK_ROUTINE_NAME;
    hparams.shash_RoutineName.iDataLen = LTW_HASH_TOREK_ROUTINE_NAME_LEN;
    hparams.p_hash_mem_Hndl            = JAMQ_OS_NO_MEM_MGR;
    if (!JAMQ_hash_open (&(context.functions), &hparams, &retcode)) {
        printf ("Function name hash table cannot be opened (%ld)\n",
            (long) retcode);
        exit (EXIT_FAILURE);
    }
    if (!JAMQ_hash_open (&(context.services), &hparams, &retcode)) {
        printf ("Services hash table cannot be opened (%ld)\n",
            (long) retcode);
        exit (EXIT_FAILURE);
    }

    /*  Call application to register its functions                           */
    if (application_init (&(context.appctx), &retcode) != OK) {
        printf ("Application initialization failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Call application startup                                             */
    item = xml_find_item (config, "/application-config/startup");
    attr1 = xml_attr (item, "function-name");
    key.pData = xml_attr_value (attr1);
    key.iDataLen = strlen (xml_attr_value (attr1));
    if (JAMQ_hash_find (context.functions, &key, &fx , &retcode) != OK) {
        printf ("Open callback cannot be found. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (((JAMQ_fOpenRoutine) fx) (&(context.appctx), &retcode) != OK) {
        printf ("Application start-up failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Get message handler                                                  */
    item = xml_find_item (config, "/application-config/message-handler");
    if (item) {
        attr1 = xml_attr (item, "function-name");
        key.pData = xml_attr_value (attr1);
        key.iDataLen = strlen (xml_attr_value (attr1));
        if (JAMQ_hash_find (context.functions, &key, &fx , &retcode) != OK) {
            printf ("Message handler cannot be found. (%ld)\n", (long) retcode);
            
        }
        message_handler = (JAMQ_fMessageHandlerRoutine) fx;    
    }

    /*  Register timers                                                      */
    /*  TODO  */

    /*  Initialise AMQ client API, open connection, channel and handle       */
    result = amq_stdc_init ();
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_init failed\n");
        exit (1);
    }
    result = amq_stdc_open_connection (server, atoi (port), vhost,
        argv[1], amq_stdc_heartbeats_off, amq_stdc_heartbeats_off,
        0, NULL, 0, &connection);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_connection failed\n");
        exit (1);
    }
    result = amq_stdc_open_channel (connection, 0, 0, NULL, "", 0,
        &channel);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_channel_failed\n");
        exit (1);
    }
    result = amq_stdc_open_handle (channel, amq_stdc_service_type_queue,
        1, 1, 0, 0, "", "", "", NULL, 0, NULL, &handle_id);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_open_handle failed\n");
        return EXIT_FAILURE;
    }

    /*  Register for consuming messages from administrative queue            */
    /*  Marked as unreliable, so administrative commands don't need to       */
    /*  be acknowledged (?)                                                  */
    result = amq_stdc_consume (channel, handle_id, 1,
        0, 1, argv [1], "", "", "", 0);
    if (result != APR_SUCCESS) {
        printf ("Not able to consume from administrative queue.\n");
        exit (1);
    }

    /*  Register for queues corresponding to individual services             */
    /*  Fill in map of services and corresponding callback functions         */
    item = xml_find_item (config, "/application-config/services");
    child_item = xml_first_child (item);
    while (child_item) {
        attr1 = xml_attr (child_item, "forward-mode");
        if (strcmp (xml_attr_value (attr1), "load-balance") == 0) {
            attr1 = xml_attr (child_item, "name");
            result = amq_stdc_consume (channel, handle_id, 1,
                0, 0, xml_attr_value (attr1), "", "", "", 0);
            if (result != APR_SUCCESS) {
                printf ("Not able to consume from service queue.\n");
                exit (1);
            }
            item2 = xml_find_item (config,
                "/application-config/application-callbacks");
            item2 = xml_first_child (item2);
            while (item2) {
                attr2 = xml_attr (item2, "logical-name");
                if (strcmp (xml_attr_value (attr1),
                      xml_attr_value (attr2)) == 0) {
                    attr2 = xml_attr (item2, "function-name"); 
                    key.pData = xml_attr_value (attr2);
                    key.iDataLen = strlen (xml_attr_value (attr2));
                    if (JAMQ_hash_find (context.functions, &key, &fx ,
                          &retcode) != OK) {
                        printf ("Callback function %s cannot be found. (%ld)\n",
                            xml_attr_value (attr2), (long) retcode);
                        exit (1);
                    }
                    key.pData = xml_attr_value (attr1);
                    key.iDataLen = strlen (xml_attr_value (attr1));
                    if (!JAMQ_hash_add (context.services, &key, fx,
                          &retcode)) {
                        printf ("Service can't be added to service hash table");
                        exit (1);
                    }
                    break;
                }
                item2 = xml_next_sibling (item2);
            }
        }
        child_item = xml_next_sibling (child_item);
    }

    /*  Event loop begins                                                    */
    while (1) {

        result = amq_stdc_get_message (channel, 0, &message_desc, &message);
        if (result != APR_SUCCESS) {
            printf ("amq_stdc_receive_message failed\n");
            exit (1);
        }

        /*  Semi-active waiting here because of timers                       */
        /*  TODO: Add timeout to amq_stdc_get_message function (?)           */
        if (!message_desc) {
            apr_sleep (10000);
            continue;
        }

        /*  If message came from administrative queue, begin shutdown        */
        /*  This behaviour may be refined in the future (?)                  */
        if (strcmp (message_desc->dest_name, argv [1]) == 0) {
            result = amq_stdc_close_message (message, 0);
            if (result != APR_SUCCESS) {
                printf ("amq_stdc_close_message failed\n");
                exit (1);
            }
            break;
        }

        /*  Otherwise this is a standard message                             */
        /*  So forward it to message handler                                 */
        if (message_handler) {
            if (message_handler (&(context.appctx),
                  context.appctx.pApplHndl->pApplsHndl, NULL, &retcode) != OK) {
                /* Nothing here yet, should be specified...                  */
            }
            continue;
        }

        /*  There's no message handler. Message should be forwarded to       */
        /*  appropriate callback function                                    */
        
/*
        if (JAMQ_hash_find (context.functions, &key, &fx , &retcode) != OK) {
            printf ("Open callback cannot be found. (%ld)\n", (long) retcode);
            exit (1);
        }
*/

        
    }
















    /*  Close handle, channel, connection, deinitialise AMQ client API       */
    result = amq_stdc_close_handle (channel, handle_id);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_handle failed\n");
        return EXIT_FAILURE;
    }
    result = amq_stdc_close_channel (channel);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_channel failed\n");
        exit (1);
    }
    result = amq_stdc_close_connection (connection);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_close_connection failed\n");
        exit (1);
    }
    result = amq_stdc_term ();
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_term failed\n");
        exit (1);
    }

    /*  Call application shutdown                                            */
    item = xml_find_item (config, "/application-config/shutdown");
    attr1 = xml_attr (item, "function-name");
    key.pData = xml_attr_value (attr1);
    key.iDataLen = strlen (xml_attr_value (attr1));
    if (JAMQ_hash_find (context.functions, &key, &fx , &retcode) != OK) {
        printf ("Close callback cannot be found. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (((JAMQ_fCloseRoutine) fx) (&(context.appctx), &retcode) != OK) {
        printf ("Application shutdown failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Call apllication to unregister its functions                         */
    if (application_uninit (&(context.appctx), &retcode) != OK) {
        printf ("Application uninitialization failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }   

    /*  Deinitialisation of context                                          */
    if (!JAMQ_hash_close (&(context.services), &retcode)) {
        printf ("Services hash table cannot be closed (%ld)\\n",
            (long) retcode);
        exit (EXIT_FAILURE);
    }
    if (!JAMQ_hash_close (&(context.functions), &retcode)) {
        printf ("Function name hash table cannot be closed (%ld)\\n",
            (long) retcode);
        exit (EXIT_FAILURE);
    }
#if 0
    if (JAMQ_m_put_buffer (&(context.appctx.sWriteBuf), &retcode) != OK) {
        printf ("Buffer for writing cannot be closed. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (JAMQ_m_put_buffer (&(context.appctx.sReadBuf), &retcode) != OK) {
        printf ("Buffer for reading cannot be closed. (%ld)\n", (long) retcode);
        exit (1);
    }
#endif
    if (JAMQ_gmm_close (&(context.appctx.pGMMWriteHndl), &retcode) != OK) {
        printf ("GMM write object cannot be closed. (%ld)\n", (long) retcode);
        exit (1);
    }
    if (JAMQ_gmm_close (&(context.appctx.pGMMReadHndl), &retcode) != OK) {
        printf ("GMM read object cannot be closed. (%ld)\n", (long) retcode);
        exit (1);
    }

    /*  Deallocate xml tree read from config file                            */
    xml_free (config);


    /*  Uninitialise APR                                                     */
    /*  TODO: should be moved to amq_stdc_client                             */
    apr_pool_destroy (pool);
    apr_terminate();

    return 0;
}
