/*===========================================================================*
 *                                                                           *
 *  testfort.c - Test fortune cookies                                        *
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
    int
        rc;
    char
        *textfile,
        *fortunefile,
        *fortune;

    if (argc < 2)
      {
        puts ("syntax: testfort [-b textfile] fortunefile");
        return (EXIT_SUCCESS);
      }
    if (streq (argv [1], "-b"))
      {
        if (argc < 4)
          {
            puts ("syntax: testfort [-b textfile] fortunefile");
            return (EXIT_SUCCESS);
          }
        textfile    = argv [2];
        fortunefile = argv [3];
        printf ("Compressing %s to %s...\n", textfile, fortunefile);
        rc = fortune_build (textfile, fortunefile, TRUE);
        if (rc)
            printf ("Failed: %s\n", strerror (errno));
        else
            printf ("Okay\n");
      }
    else
      {
        fortunefile = argv [1];
        fortune = fortune_read (fortunefile);
        if (fortune)
            printf ("%s", fortune);
        else
            printf ("Failed: %s\n", strerror (errno));
      }
    return (EXIT_SUCCESS);
}

