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
    if (s_start (argc, argv, "server.cfg", "custom.cfg"))
        exit (EXIT_FAILURE);

    while (s_run_server ());
    return (EXIT_SUCCESS);
}

