/*===========================================================================*
 *                                                                           *
 *  testmime.c - Test MIME functions                                         *
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

const unsigned char *TEST_FRENCH = (const unsigned char *) \
                    "ceci est un chaîne avec ceci: éçà@ù," \
                    " et cela encore & î, voilà c'est la fin";

int main (int argc, char *argv [])
{
    char
        *feedback,
        read_buffer    [LINE_MAX+1];
    byte
        decoded_buffer [LINE_MAX+1],
        encoded_buffer [LINE_MAX+1];
    size_t
        return_size;
    FILE
        *file;

    memset (read_buffer,    0, LINE_MAX+1);
    memset (encoded_buffer, 0, LINE_MAX+1);
    memset (decoded_buffer, 0, LINE_MAX+1);

    if (argc > 1)
      {
        file = file_open (argv[1], 'r');
        if (file != NULL)
          {
            while (file_read (file, read_buffer) == TRUE)
              {
                printf ("Read    : %s\n", read_buffer);

                /*  Encode the buffer                                        */
                return_size = encode_base64 ((byte *) read_buffer, 
                                             encoded_buffer,
                                             strlen (read_buffer));
                printf ("Encoded : %s\n", encoded_buffer);

                /*  Decode the buffer                                        */
                decode_base64 (encoded_buffer, decoded_buffer, return_size);
                printf ("Decoded : %s\n\n", decoded_buffer);

                /*  Test if error                                            */
                if (strcmp (read_buffer, (char *) decoded_buffer) != 0)
                  {
                    printf ("Error : decoded is not same as read\n");
                    break;
                  }
                /*  Reset the buffers                                        */
                memset (decoded_buffer, 0, LINE_MAX+1);
                memset (encoded_buffer, 0, LINE_MAX+1);
              }
            file_close (file);
          }
      }
    printf ("Test encode quoted string\n");
    printf ("encoding this string: %s\n", TEST_FRENCH);
    feedback = encode_quoted_string (NULL, 0, TEST_FRENCH, "ISO-8859-1");
    if (feedback)
      {
        printf ("Result: %s\n", feedback);
        mem_free (feedback);
      }
    feedback = encode_mimeb_string (NULL, 0, TEST_FRENCH, "iso-2220-jp");
    if (feedback)
      {
        printf ("Result: %s\n", feedback);
        mem_free (feedback);
      }
    return (EXIT_SUCCESS);
}
