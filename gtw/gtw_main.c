
#include "base.h"
#include "base_apr.h"
#include "amq_stdc_client.h"
#include "gtw.h"
#include "sfl.h"

int application_init (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    );

int application_uninit (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    );

#define RESPONSE_QUEUE_POSTFIX ".response"
#define RESPONSE_QUEUE_POSTFIX_SIZE 9

int main (
    int         argc,
    const char  *const argv[],
    const char  *const env[]
    ) 
{
    JAMQ_tsApicb
        *context = NULL;
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
    JAMQ_tsNCharcb
        client_name;
    JAMQ_fMessageHandlerRoutine
        message_handler = NULL;
    int
        retcode;
    apr_status_t
        result;
    JAMQ_tsUnrefSymInfo
        symbol;
    char
        response_queue_name_buffer [256 + RESPONSE_QUEUE_POSTFIX_SIZE];
    JAMQ_tsNCharcb
        response_queue_name;
    JAMQ_tsNCharcb
        service_name;
    byte
        stop = 0;
    JAMQ_tsNCharcb
        queue_name;
    JAMQ_tsNCharcb
        command;

    if (argc != 3) {
        printf ("Command line syntax: gtw_main <instance name> "
            "<config file name>\n");
        exit (1);
    }

    /* Initialise APR                                                        */
    result = apr_app_initialize (&argc, &argv, &env);
    if (result != APR_SUCCESS) {
        printf ("apr_app_initialise failed\n");
        exit (1);
    }

    /*  Load configuration file                                              */        
    if (xml_load (&config, "", argv [2]) != XML_NOERROR) {
        printf ("Application configuration file cannot be opened.\n");
        exit (1);
    }

    /*  Connect to AMQ server                                                */
    client_name.iDataLen = strlen (argv [1]);
    client_name.pData = (char*) argv [1];
    if (!JAMQ_apiu_open (&context, &client_name, &retcode)) {
        printf ("Connection to server cannot be opened. (%ld)\n",
            (long) retcode);
        exit (1);
    }
    context->argc = argc;
    context->argv = (char**) argv;
    context->sName.iDataLen = strlen (argv [0]);
    context->sName.pData = (char*) argv [0];

    /*  Call application to register its functions                           */
    if (application_init (context, &retcode) != OK) {
        printf ("Application initialization failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Call application startup                                             */
    item = xml_find_item (config, "/application-config/startup");
    if (!item) {
        printf ("Startup tag missing in configuration file.\n");
        exit (1);
    }
    attr1 = xml_attr (item, "function-name");
    if (!item) {
        printf ("Function-name attribute of startup tag missing in "
            "configuration file.\n");
        exit (1);
    }
    symbol.pUnreferencedSymbol = xml_attr_value (attr1);
    if (JAMQ_apiu_find_unref_sym (context, &symbol, &retcode) != OK) {
        printf ("Application hadn't registered open callback function. (%ld)\n",
            (long) retcode);
        exit (1);
    }
    if (((JAMQ_fOpenRoutine) symbol.pUnreferencedRoutine) (context,
          &retcode) != OK) {
        printf ("Application start-up failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Get message handler                                                  */
    item = xml_find_item (config, "/application-config/message-handler");
    if (item) {
        attr1 = xml_attr (item, "function-name");
        if (!attr1) {
            printf ("Function-name attribute of message-handler tag missing"
                "in configuration file.\n");
            exit (1);
        }
        symbol.pUnreferencedSymbol = xml_attr_value (attr1);
        if (JAMQ_apiu_find_unref_sym (context, &symbol, &retcode) != OK) {
            printf ("Application hadn't registered message handler callback "
                "function. (%ld)\n", (long) retcode);
            exit (1);
        }
        message_handler =
            (JAMQ_fMessageHandlerRoutine) symbol.pUnreferencedRoutine;    
    }

    /*  TODO: Register timers                                                */

    /*  Consume from administrative queue                                    */
    if (JAMQ_apiu_consume (context, &client_name, 1, &retcode) != OK) {
        printf ("Cannot consume from administrative queue. (%ld)\n",
            (long) retcode);
        exit (1);
    }

    /*  Consume from response queue                                          */
    memcpy ((void*) response_queue_name_buffer, (void*) client_name.pData,
        client_name.iDataLen);
    strcpy (response_queue_name_buffer + client_name.iDataLen,
        RESPONSE_QUEUE_POSTFIX);
    response_queue_name.iDataLen = client_name.iDataLen +
        RESPONSE_QUEUE_POSTFIX_SIZE;
    response_queue_name.pData = response_queue_name_buffer;
    if (JAMQ_apiu_consume (context, &response_queue_name, 1, &retcode) != OK) {
        printf ("Cannot consume from response queue. (%ld)\n",
            (long) retcode);
        exit (1);
    }

    /*  Consume from queues corresponding to individual services             */
    /*  Fill in map of services and corresponding callback functions         */
    item = xml_find_item (config, "/application-config/services");
    if (!item) {
        printf ("Services tag missing in application configuration file.\n");
        exit (1);
    }
    child_item = xml_first_child (item);
    while (child_item) {
        attr1 = xml_attr (child_item, "forward-mode");
        if (!attr1) {
            printf ("Forward-mode attribute missing in service tag in "
                "application configuration file.");
            exit (1);
        }
        if (strcmp (xml_attr_value (attr1), "load-balance") == 0) {
            attr1 = xml_attr (child_item, "name");
            if (!attr1) {
                printf ("Name attribute missing in service tag in "
                    "application configuration file.\n");
            }
            service_name.iDataLen = strlen (xml_attr_value (attr1));
            service_name.pData = xml_attr_value (attr1);
            if (JAMQ_apiu_consume (context, &service_name, 1, &retcode) != OK) {
                printf ("Cannot consume from service queue. (%ld)\n",
                    (long) retcode);
                exit (1);
            }
            item2 = xml_find_item (config,
                "/application-config/application-callbacks");
            if (!item2) {
                printf ("Application-callbacks tag missing in application "
                    "configuration file.\n");
                exit (1);
            }
            item2 = xml_first_child (item2);
            while (item2) {
                attr2 = xml_attr (item2, "logical-name");
                if (!attr2) {
                    printf ("Logical-name attribute missing in callback tag "
                        "in application configuration file.\n");
                    exit (1);
                }
                if (strcmp (xml_attr_value (attr1),
                      xml_attr_value (attr2)) == 0) {
                    attr2 = xml_attr (item2, "function-name"); 
                    if (!attr2) {
                        printf ("Logical-name attribute missing in callback "
                            "tag in application configuration file.\n");
                        exit (1);
                    }
                    if (!JAMQ_apiu_register_service (context,
                          xml_attr_value (attr1), xml_attr_value (attr2),
                          &retcode)) {
                        printf ("Cannot register service handler. (%ld)",
                            (long) retcode);
                        exit (1);
                    }
                    break;
                }
                item2 = xml_next_sibling (item2);
            }
        }
        else {
            /*  TODO: Pub-sub services here...                               */
        }
        child_item = xml_next_sibling (child_item);
    }

    /*  Event loop                                                           */
    while (!stop) {

        /*  Receive one message                                              */;
        if (!JAMQ_apiu_receive_message (context, &queue_name, &retcode)) {
            printf ("Failed while receiving message. (%ld)\n", (long) retcode);
            exit (1);
        }

        if (queue_name.iDataLen == strlen (argv [1]) && memcmp ((void*) 
            queue_name.pData, (void*) argv[1], queue_name.iDataLen) == 0) {

            /*  Message came from administrative queue                       */
            if (!JAMQ_gmm_get_data (context->pGMMReadHndl, 0, 1, &command,
                  &retcode)) {
                printf ("Invalid command arrived.");
                exit (1);
            }
            if (command.iDataLen == 8 && memcmp ((void*) command.pData,
                  (void*) "shutdown", 8) == 0) {
                stop = 1;
            }
            else if (command.iDataLen == 8 && memcmp ((void*) command.pData,
                  (void*) "is_ready", 8) == 0) {
                /* TODO: reply somehow                                       */
            }
            else {
                printf ("Invalid command arrived.\n");
                exit (1);
            }
        }
        else {

            /*  Standard message                                             */
            if (message_handler) {
                if (!message_handler (context, context->pApplHndl->pApplsHndl,
                      NULL, &retcode)) {

                    /*  Message handler failed. What now ?                   */
                    /*  TODO: some implementation                            */
                }
            }
            else {

                /*  There's no message handler defined, so we take       */
                /*  default action: Dispatch message to apropriate       */
                /*  callback function.                                   */
                if (!JAMQ_apiu_handle_message (context, &retcode)) {
                    printf ("Message handling failed. (%ld)\n" ,
                        (long) retcode);
                    exit (1);
                }
            }
        }
    }

    /*  Call application shutdown                                            */
    item = xml_find_item (config, "/application-config/shutdown");
    if (!item) {
        printf ("Shutdown tag missing in configuration file.\n");
        exit (1);
    }
    attr1 = xml_attr (item, "function-name");
    if (!item) {
        printf ("Function-name attribute of shutdown tag missing in "
            "configuration file.\n");
        exit (1);
    }
    symbol.pUnreferencedSymbol = xml_attr_value (attr1);
    if (JAMQ_apiu_find_unref_sym (context, &symbol, &retcode) != OK) {
        printf ("Application hadn't registered close callback function. "
            "(%ld)\n", (long) retcode);
        exit (1);
    }
    if (((JAMQ_fCloseRoutine) symbol.pUnreferencedRoutine) (context,
          &retcode) != OK) {
        printf ("Application shutdown failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Call apllication to unregister its functions                         */
    if (application_uninit (context, &retcode) != OK) {
        printf ("Application uninitialization failed with error code – %ld\n",
            (long) retcode);
        exit (1);
    }

    /*  Close connection to AMQ server                                       */
    if (!JAMQ_apiu_close (&context, &retcode)) {
        printf ("Connection to server cannot be closed. (%ld)\n",
            (long) retcode);
        exit (1);
    } 

    /*  Deallocate xml tree read from config file                            */
    xml_free (config);

    /*  Deinitialise APR                                                     */
    apr_terminate();

    return 0;
}
