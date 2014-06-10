/*---------------------------------------------------------------------------
    icl_stats_examine.c - iCL statistics examiner.

    Examines and dumps statistics from a running process.
    The statistics are held in a shared memory block.

    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes

#define ICL_STATS_SHARE_DEFAULT_NAME "icl__stats"

#define BYTE_ALIGNMENT               4

int main (int argc, char *argv [])
{
    char
        share_name [128];
    apr_shm_t
        *s_share = NULL;
    apr_status_t
        status;
    char
        *start;
    size_t
        name_length;

    icl_console_mode (ICL_CONSOLE_QUIET, TRUE);//  So no init messages
    icl_system_initialise (argc, argv);

    icl_console_redirect (stdout);      //  So 'watch -d' works
    icl_console_mode (ICL_CONSOLE_QUIET, FALSE);
    icl_console_mode (ICL_CONSOLE_TIME,  FALSE);

    if (argc > 1) {
        strncpy (share_name, g_argv [1], 120);
        share_name [120] = 0;
    }
    else
        share_name [0] = 0;

    strcat (share_name, "_stats");

    status = apr_shm_attach (&s_share, share_name, icl_global_pool);
    if (status != APR_SUCCESS)
        icl_apr_error_print (status);
    else {
        icl_console_print ("<?xml?>");
        start = apr_shm_baseaddr_get (s_share);
        while (*start) {
            name_length = strlen (start) + 1;//  Including null terminator
            name_length = ((name_length + BYTE_ALIGNMENT - 1) / BYTE_ALIGNMENT) * BYTE_ALIGNMENT;
            icl_console_print ("<stat name = \"%s\" value = \"%u\"/>", start, * (qbyte *) (start + name_length));
            start += name_length + 4;
        }
    }
    icl_system_terminate ();
    return 0;
}

