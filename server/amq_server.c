/*===========================================================================
 *
 *  OpenAMQ server main program
 *
 *  Copyright (c) 2007 iMatix Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  For information on alternative licensing for OEMs, please contact
 *  iMatix Corporation.
 *
 *===========================================================================*/

//  Name and description of server for public consumption

#define SERVER_NAME     "OpenAMQ"

#include "amq_server_main.inc"

int
main (int argc, char *argv [])
{
    FOREVER {
        s_start (argc, argv);
        if (s_run_server () == 0)
            break;                      //  Rerun server if wanted
    }
    return (EXIT_SUCCESS);
}

