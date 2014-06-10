/*===========================================================================*
 *                                                                           *
 *  testproc.c - Test process control functions                              *
 *                                                                           *
 *  Copyright (c) 1991-2009 iMatix Corporation                               *
 *                                                                           *
 *  ------------------ GPL Licensed Source Code ------------------           *
 *  iMatix makes this software available under the GNU General               *
 *  Public License (GPL) license for open source projects.  For              *
 *  details of the GPL license please see www.gnu.org or read the            *
 *  file license.gpl provided in this package.                               *
 *                                                                           *
 *  This program is free software; you can redistribute it and/or            *
 *  modify it under the terms of the GNU General Public License as           *
 *  published by the Free Software Foundation; either version 2 of           *
 *  the License, or (at your option) any later version.                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public                *
 *  License along with this program in the file 'license.gpl'; if            *
 *  not, write to the Free Software Foundation, Inc., 59 Temple              *
 *  Place - Suite 330, Boston, MA 02111-1307, USA.                           *
 *                                                                           *
 *  You can also license this software under iMatix's General Terms          *
 *  of Business (GTB) for commercial projects.  If you have not              *
 *  explicitly licensed this software under the iMatix GTB you may           *
 *  only use it under the terms of the GNU General Public License.           *
 *                                                                           *
 *  For more information, send an email to info@imatix.com.                  *
 *  --------------------------------------------------------------           *
 *===========================================================================*/

#include "sfl.h"

int main (int argc, char *argv [])
{
    PROCESS
        process;                        /*  Created process id               */
    PROCESS_DATA
        procinfo = PROCESS_DATA_INIT;
    static char
        buffer [1024 + 1];
    int
        read_size;
    char
        *std_in,
        *std_out,
        *std_err;

    /*  Copy stdin to stdout        */
    if (argc == 2
    &&  streq (argv [1], "-test"))
      {
        coprintf ("PASSED IN: %d", stdin);
        coprintf ("HERE");
        read_size = fread (buffer, 1, 1024, stdin);
        coprintf ("read size: %d", read_size);
        buffer [read_size] = '\0';
        printf ("%s", buffer);
        return (0);
      }
    std_in  = "testproc.c";
    std_out = "testproc.out";
    std_err = "testproc.err";

#if 0
    /*  Remove old stdout file if any                                        */
    if (file_exists (std_out))
        file_delete (std_out);
#endif

    procinfo.filename  = "testproc -test";
    procinfo.workdir   = ".";
    if (process_setinfo (&procinfo, std_in, std_out, TRUE, std_err, FALSE) == 0)
      {
        process = process_create_full (&procinfo);
        process_close_io (&procinfo);
        if (process == NULL_PROCESS)
            trace ("Can't start process: %s", strerror (procinfo.error));
      }
    else
      {
        trace ("Can't redirect i/o: %s", strerror (errno));
        process = NULL_PROCESS;
      }

    return (0);
}
