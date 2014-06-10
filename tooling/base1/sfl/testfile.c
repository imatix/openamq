/*===========================================================================*
 *                                                                           *
 *  testfile.c - Test file access functions                                  *
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

#define TEST_FILENAME "testfile.tmp"
#define BUFFER_SIZE   10240

int main (int argc, char *argv [])
{
    FILE
        *tempfile;
    char
        *tempname;
    Bool
        opened,
        closed;
    char
        buffer [BUFFER_SIZE + 1];
    /* Test of file_stable function                                          */
    printf ("Test of file_stable\n");
    tempfile = fopen (TEST_FILENAME, "wb");
    if (tempfile)
      {
        memset (buffer, 'X', BUFFER_SIZE);
        ASSERT (fwrite (buffer, 1, BUFFER_SIZE, tempfile));
        fflush  (tempfile);
        fclose (tempfile);
        tempfile = fopen (TEST_FILENAME, "ab");
        ASSERT (fwrite (buffer, 1, BUFFER_SIZE, tempfile));

        opened =  file_stable (TEST_FILENAME);
        fclose (tempfile);
        closed =  file_stable (TEST_FILENAME);
        if (opened == FALSE && closed == TRUE)
            printf ("    Ok\n");
        else
            printf ("    Error: open = %d, close = %d\n",
                    opened, closed);

        file_delete (TEST_FILENAME);
      }

        /* Test of temporary file                                            */
    printf ("\nTest of temp file\n");
    tempfile = ftmp_open (&tempname);
    printf ("File is %p, %s\n", tempfile, tempname);
    ftmp_close (tempfile);

    return (EXIT_SUCCESS);
}
