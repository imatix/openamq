/*===========================================================================*
 *                                                                           *
 *  echocli.c - Echo protocol client                                         *
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

#include "smtpriv.h"                    /*  SMT definitions                  */

static sock_t handle;

void handle_signal (int the_signal)
{
    close (handle);
    exit (1);
}

int main (int argc, char *argv [])
{
#   define BUFFER_SIZE  512
    char buffer [BUFFER_SIZE + 1];
    int  count, length;
    char *socket_number;

    signal (SIGINT,  handle_signal);
    signal (SIGSEGV, handle_signal);
    signal (SIGTERM, handle_signal);

    sock_init ();
    if (argc > 1)
        socket_number = argv [1];
    else
        socket_number = "5001";

    handle = connect_TCP ("", socket_number);
    if (handle == INVALID_SOCKET)
      {
        printf ("Echocli error : %s\n", sockmsg ());
        exit (EXIT_FAILURE);
      }
    printf ("=>");
    fflush (stdout);
    while (fgets (buffer, BUFFER_SIZE, stdin))
      {
        buffer [BUFFER_SIZE] = '\0';
        length = strlen (buffer);
        write_TCP (handle, buffer, length);
        printf ("--");
        fflush (stdout);
        while (length > 0)
          {
            count = read_TCP (handle, buffer, BUFFER_SIZE);
            if (count == 0)
                break;
            if (count == -1)
              {
                if (sockerrno == EAGAIN || sockerrno == EWOULDBLOCK)
                    continue;
                printf ("Echocli error : %s\n", sockmsg ());
                return (EXIT_FAILURE);
              }
            buffer [count] = '\0';
            printf ("%s", buffer);
            length -= count;
          }
        printf ("=>");
        fflush (stdout);
      }
    close_socket (handle);
    sock_term ();
    return (EXIT_SUCCESS);
}
