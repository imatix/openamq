/*===========================================================================

    OpenAMQ server main program

    Copyright (c) 1991-2005 iMatix Corporation
 *===========================================================================*/

//  Name and description of server for public consumption

#define SERVER_NAME     "OpenAMQ"
#define SERVER_TEXT     "*** Test server - for internal use only ***"

#include "amq_server_main.inc"

int
main (int argc, char *argv [])
{
    if (s_start (argc, argv, "server.cfg", "custom.cfg"))
        exit (EXIT_FAILURE);

    if (s_run_server ())
        return (EXIT_FAILURE);
    else
        return (EXIT_SUCCESS);
}

