
#include "base.h"
#include "base_apr.h"
#include "gtw.h"

inline apr_status_t xxx (unsigned char *p, apr_size_t sz)
{
    return APR_SUCCESS;
}

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
    apr_status_t
        result;
    JAMQ_tsApicb
        *context = NULL;
    apr_uuid_t
        client_uuid;
    char
        client_name_buffer [10 + APR_UUID_FORMATTED_LENGTH + 1];
    JAMQ_tsNCharcb
        client_name;

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

    /*  Create unique client name                                            */
    memcpy ((void*) client_name_buffer, (void*) "grm-msgen-", 10);
    apr_uuid_get (&client_uuid);
    apr_uuid_format (client_name_buffer + 10, &client_uuid);

    /*  Connect to AMQ server                                                */
    client_name.iDataLen = strlen (client_name_buffer);
    client_name.pData = client_name_buffer;
    if (!JAMQ_apiu_open (&context, &client_name, &retcode)) {
        printf ("Connection to server cannot be opened. (%ld)\n",
            (long) retcode);
        exit (1);
    }
    context->argc = argc;
    context->argv = (char**) argv;
    context->sName.iDataLen = strlen (argv [0]);
    context->sName.pData = (char*) argv [0];

    /*  Parse command line to write buffer                                   */
    if (!JAMQ_gmm_from_cmdline (context->pGMMWriteHndl, argc - 1, fields,
          &(context->sWriteBuf), &retcode)) {
        printf ("JAMQ_gmm_from_cmdline failed (%ld)\n", (long) retcode);
        exit (1);
    }

    /*  Send it                                                              */
    if (!JAMQ_apiu_flush_broadcast (context, &(context->sWriteBuf), &retcode)) {
        printf ("JAMQ_apiu_flush_broadcast failed (%ld)\n", (long) retcode);
        exit (1);
    }

    /*  Close connection to server                                           */
    if (!JAMQ_apiu_close (&context, &retcode)) {
        printf ("JAMQ_apiu_close failed (%ld)\n", (long) retcode);
        exit (1);
    }
    
    free ((void*) fields);

    return 0;
}
