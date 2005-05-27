
#include "base.h"
#include "base_apr.h"
#include "sfl.h"
#include "amq_stdc_client.h"
#include "gtw.h"

int main (
    int         argc,
    const char  *const argv[],
    const char  *const env[]
    )
{
    int
        counter;
    int
        length;
    JAMQ_tsNCharcb
        *fields;
    int
        retcode;
    void
        *gmm = NULL;
    JAMQ_tsBufcb
        *buf = NULL;
    JAMQ_tsNCharcb
        data;
    char
        *queue_name;
    char
        queue_name_buffer [256];
    apr_status_t
        result;
    apr_pool_t
        *pool;
    char
        *grm_config_filename;
    XML_ITEM
        *grm_config = NULL;
    XML_ITEM
        *item;
    XML_ATTR
        *attr1;
    const char
        *server;
    const char
        *port;
    const char
        *vhost;
    amq_stdc_connection_t
        connection;
    amq_stdc_channel_t
        channel;
    dbyte
        handle_id;

    /*  Allocate array to store fields retrieved from command line          */
    fields = (JAMQ_tsNCharcb*) malloc (sizeof (JAMQ_tsNCharcb) * (argc - 1));
    if (!fields) {
        printf ("Not enough memory.\n");
        exit (1);
    }

    for (counter = 1; counter != argc; counter++) {
        length = 0;
        while (argv [counter] [length] != ' ' && argv [counter] [length] != 0)
            length++;
        fields [counter - 1].iDataLen = length;
        fields [counter - 1].pData = (char*) argv [counter];
    }

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

    /*  Initialise AMQ client API, open connection, channel and handle       */
    result = amq_stdc_init ();
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_init failed\n");
        exit (1);
    }
    result = amq_stdc_open_connection (server, atoi (port), vhost,
        "msgen", amq_stdc_heartbeats_off, amq_stdc_heartbeats_off,
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
        exit (1);
    }

    /*  Open buffer and GMM object, parse command line                       */
    if (!JAMQ_gmm_open (&gmm, &retcode)) {
        printf ("JAMQ_gmm_open failed (%ld)\n", (long) retcode);
        exit (1);
    }

    if (!JAMQ_m_get_buffer (&buf, 1024, &retcode)) {
        printf ("JAMQ_m_get_buffer failed (%ld)\n", (long) retcode);
        exit (1);
    }
    if (!JAMQ_gmm_start_msg (gmm, buf, &retcode)) {
        printf ("JAMQ_gmm_start_msg failed (%ld)\n", (long) retcode);
        exit (1);
    }
    if (!JAMQ_gmm_from_cmdline (gmm, argc - 1, fields, buf, &retcode)) {
        printf ("JAMQ_gmm_from_cmdline failed (%ld)\n", (long) retcode);
        exit (1);
    }

    /*  Get queue name, send message                                         */
    if (!JAMQ_gmm_get_data (gmm, 0, 0, &data, &retcode)) {
        if (retcode == JAMQ_GMM_DATA_UNV)
            queue_name = "pubsub";
        else {
            printf ("JAMQ_gmm_from_cmdline failed (%ld)\n", (long) retcode);
            exit (1);
        }
    }
    else {
        queue_name = queue_name_buffer;
        memcpy ((void*) queue_name_buffer, (void*) data.pData, data.iDataLen);
        queue_name_buffer [data.iDataLen] = 0;
    }

    result = amq_stdc_send_message (channel, handle_id, 0, 0, 0, queue_name,
        0, 5, 0, "", "", "", 0, NULL, 0);
    if (result != APR_SUCCESS) {
        printf ("amq_stdc_send_message failed\n");
        exit (1);
    }

    /*  Close buffer, close GMM object                                       */
    if (!JAMQ_m_put_buffer (&buf, &retcode)) {
        printf ("JAMQ_m_put_buffer failed (%ld)\n", (long) retcode);
        exit (1);
    }

    if (!JAMQ_gmm_close (&gmm, &retcode)) {
        printf ("JAMQ_gmm_close failed (%ld)\n", (long) retcode);
        exit (1);
    }

    /*  TODO: Close channel & connection & handle                            */

    xml_free (grm_config);
    free ((void*) fields);

    return 0;
}
