/*===========================================================================*
 *                                                                           *
 *  tstsock.c - Test socket agen                                             *
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

QID
    sockq;
QUEUE
    *myqueue;
sock_t
    connect_handle;

void display_events ()
{
    EVENT
        *event;
    struct_smtsock_read_reply
        *read_reply;
    struct_smtsock_readh_reply
        *readh_reply;
    struct_smtsock_connect_ok
        *connect_ok;
    struct_smtsock_reply
        *reply;
    struct_smtsock_error
        *error;
    char
        *buffer;

    while (myqueue-> cur_events)
      {
        event = event_accept (myqueue, NULL);
        coprintf ("Received event: %s", event-> name);
        if (streq (event-> name, SMTSOCK_READ_OK)
        ||  streq (event-> name, SMTSOCK_READ_CLOSED)
        ||  streq (event-> name, SMTSOCK_READ_TIMEOUT))
          {
            get_smtsock_read_reply (event-> body, & read_reply);

            buffer = mem_alloc (read_reply-> size + 1);
            memcpy (buffer, read_reply-> data, read_reply-> size);
            buffer [read_reply-> size] = 0;
            coprintf ("    Tag: %i Size: %u Data = %s",
                      read_reply-> tag,
                      read_reply-> size,
                      buffer);

            mem_free (buffer);
            free_smtsock_read_reply (& read_reply);
          }
        else
        if (streq (event-> name, SMTSOCK_READH_OK)
        ||  streq (event-> name, SMTSOCK_READH_CLOSED)
        ||  streq (event-> name, SMTSOCK_READH_TIMEOUT))
          {
            get_smtsock_readh_reply (event-> body, & readh_reply);

            coprintf ("    Tag: %i Size: %u Data = %s",
                      readh_reply-> tag,
                      readh_reply-> size,
                      readh_reply-> data);

            free_smtsock_readh_reply (& readh_reply);
          }
        else
        if (streq (event-> name, SMTSOCK_CONNECT_OK))
          {
            get_smtsock_connect_ok (event-> body, & connect_ok);

            coprintf ("    Tag: %i Socket: %u",
                      connect_ok-> tag,
                      connect_ok-> socket);
            connect_handle = connect_ok-> socket;
            free_smtsock_connect_ok (& connect_ok);
          }
        else
        if (streq (event-> name, SMTSOCK_OK)
        ||  streq (event-> name, SMTSOCK_CLOSED)
        ||  streq (event-> name, SMTSOCK_TIMEOUT))
          {
            get_smtsock_reply (event-> body, & reply);

            coprintf ("    Tag: %i",
                      reply-> tag);

            free_smtsock_reply (& reply);
          }
        else
        if (streq (event-> name, SMTSOCK_ERROR))
          {
            get_smtsock_error (event-> body, & error);

            coprintf ("    Tag: %i Message: %s",
                      error-> tag,
                      error-> message);

            free_smtsock_error (& error);
          }

        event_destroy (event);
      }
}


void
step_and_accept_messages ()
{
    Bool
        cont = TRUE;

    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();

        display_events ();
      }
}


int
main (int argc, char *argv [])
{
    THREAD
        *thread;
    int
        master_handle,
        slave_handle;

    smt_init ();                        /*  Initialise SMT kernel            */
    myqueue = queue_create (NULL, 0);
    smtsock_init ();
    console_set_mode (CONSOLE_PLAIN);   /*  So we can 'diff' outputs.        */

    if ((thread = thread_lookup (SMT_SOCKET, "")) != NULL)
        sockq = thread-> queue-> qid;
    else
        return (-1);

    step_and_accept_messages();

    
    coprintf ("Test failed connect.");
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();

    coprintf ("Test closing passive socket.");
    master_handle = passive_TCP ("5000", 5);
/*      lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0, */
/*                           0, master_handle, 0); */
/*      step_and_accept_messages(); */
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 1, 0);
    step_and_accept_messages();

    coprintf ("Test successful connect.");
    master_handle = passive_TCP ("5000", 5);
    lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 0);
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();
    slave_handle = accept_socket (master_handle);

    coprintf ("Test sending & receiving data.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 1, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 2, "Up", TRUE, 0);

    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, slave_handle, 4, "Down", TRUE, 0);
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, slave_handle, 255, 1, 0);
    step_and_accept_messages();

    coprintf ("Test timeout.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        2, connect_handle, 255, 255, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         1, connect_handle, 5, "Start", TRUE, 0);
    step_and_accept_messages();

    coprintf ("Test flush with no requests.");
    lsend_smtsock_flush (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         connect_handle, TRUE);
    step_and_accept_messages();

    coprintf ("Test flush all requests.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 255, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 5, "Start", TRUE, 0);
    lsend_smtsock_flush (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         connect_handle, TRUE);
    step_and_accept_messages();

    coprintf ("Test flush only read requests.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 255, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 5, "Start", TRUE, 0);
    lsend_smtsock_flush (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         connect_handle, FALSE);
    step_and_accept_messages();

    coprintf ("Test write then close.");
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 4, "Data", TRUE, 0);
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, connect_handle, 1, 0);
    step_and_accept_messages();

    coprintf ("Test close on broken pipe.");
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, slave_handle, 1, 0);
    step_and_accept_messages();

    coprintf ("Test close on closed socket.");
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, slave_handle, 1, 0);
    step_and_accept_messages();

    coprintf ("Test read on closed socket.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, slave_handle, 255, 1, 0);
    step_and_accept_messages();

    coprintf ("Test write to invalid socket.");
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, 1024, 4, "Data", TRUE, 0);
    step_and_accept_messages();

    coprintf ("Test write following close.");
    lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 0);
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();
    slave_handle = accept_socket (master_handle);
    step_and_accept_messages();
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, connect_handle, 1, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 4, "Data", TRUE, 0);
    step_and_accept_messages();

    coprintf ("Test read on broken pipe.");
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, slave_handle, 255, 1, 0);
    step_and_accept_messages();

    coprintf ("Test read following close.");
    lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 0);
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();
    slave_handle = accept_socket (master_handle);
    step_and_accept_messages();
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, connect_handle, 1, 0);
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 1, 0);
    step_and_accept_messages();

    coprintf ("Test read and write on closed socket.");
    lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 0);
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();
    slave_handle = accept_socket (master_handle);
    step_and_accept_messages();
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 1, 0);
    lsend_smtsock_write (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         2, connect_handle, 4, "Data", TRUE, 0);
    close_socket (connect_handle);
    step_and_accept_messages();

    coprintf ("Test read then close.");
    lsend_smtsock_input (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         0, master_handle, 0);
    lsend_smtsock_connect (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                           0, "TCP", "127.0.0.1", "5000", 0, 0, 0);
    step_and_accept_messages();
    slave_handle = accept_socket (master_handle);
    step_and_accept_messages();
    lsend_smtsock_read (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                        1, connect_handle, 255, 1, 0);
    lsend_smtsock_close (& sockq, & myqueue-> qid, NULL, NULL, NULL, 0,
                         5, connect_handle, 1, 0);
    step_and_accept_messages();

    smt_shutdown ();
    smt_exec_full ();
    display_events ();

    queue_destroy (myqueue);
    smt_term ();                        /*  Shut-down SMT kernel             */

    printf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());
    mem_assert ();
    return (0);
}


