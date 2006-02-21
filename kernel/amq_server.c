/*===========================================================================

    OpenAMQ server main program

    Copyright (c) 1991-2005 iMatix Corporation
 *===========================================================================*/

//  Name and description of server for public consumption

#define SERVER_NAME     "OpenAMQ"

#include "amq_server_main.inc"

int
main (int argc, char *argv [])
{
    FOREVER {
        s_start (argc, argv, "server.cfg", "custom.cfg");
        if (s_run_server () == 0)
            break;                      //  Rerun server if wanted
    }
    return (EXIT_SUCCESS);
}

