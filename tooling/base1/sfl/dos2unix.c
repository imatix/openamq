/*===========================================================================*
 *                                                                           *
 *  dos2unix.c - DOS end-of-line convertor                                   *
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

void convert (char *directory, char *filename)
{
#   define BUFMAX  32000
    static char
        fullname [1024],
        bufin  [BUFMAX],
        bufout [BUFMAX],
        backup [1024],
        ch;
    int
        insize,
        outsize,
        bufptr;
    FILE
        *input,
        *output;

    xstrcpy (fullname, directory, "/", filename, NULL);
    if (!file_exists (fullname)) {
        printf ("`%s' does not exist", fullname);
        exit (1);
    }
    printf ("Converting %s...\n", fullname);

    /*  Check if file contains suspicious binary characters                  */
    input = fopen (fullname, "rb");
    insize = fread (bufin, 1, 256, input);
    for (bufptr = 0; bufptr < insize; bufptr++) {
        ch = bufin [bufptr];
        if (ch < ' ' && !isspace (ch) && ch != 0x1A) {
            printf ("[0x%X at %d] ", bufin [bufptr], bufptr);
            printf ("%s is a binary file - skipping\n", fullname);
            return;
        }
    }
    fclose (input);

    strcpy (backup, fullname);
    fixed_extension (backup, fullname, "bak");
    file_delete (backup);
    file_rename (fullname, backup);

    input  = fopen (backup,   "rb");
    output = fopen (fullname, "wb");
    if (input == NULL || output == NULL) {
        printf ("Error: %s\n", strerror (errno));
        exit (1);
    }
    while ((insize = fread (bufin, 1, BUFMAX, input)) > 0) {
        outsize = 0;
        for (bufptr = 0; bufptr < insize; bufptr++)
            if (bufin [bufptr] != '\r'
            &&  bufin [bufptr] != 0x1A)
                bufout [outsize++] = bufin [bufptr];
        ASSERT (fwrite (bufout, 1, outsize, output));
    }
    fclose (input);
    fclose (output);
    file_delete (backup);
}

int main (int argc, char *argv[])
{
    DIRST
        dir;
    char
        *directory,
        *filename,
        *slash;

    if (argc == 1 || streq (argv [1], "/?")) {
        puts ("dos2unix - part of the iMatix RealiBase Gaggle");
        puts ("Strip CR characters from text file.\n");
        puts ("syntax: dos2unix [/?] [filename]");
        puts (" /?       - show help on command");
        puts (" filename - file to expand");
        exit (0);
    }
    filename = argv [1];
    if (strchr (filename, '*') || strchr (filename, '?'))
      {
        directory = mem_strdup (filename);
        slash = strrchr (directory, '\\');
        if (!slash)
            slash = strrchr (directory, '/');
        if (!slash)
            directory = ".";
        else
          {
            *slash = '\0';
            filename = slash + 1;
          }
        if (open_dir (&dir, directory)) do
          {
            if ((dir.file_attrs & ATTR_HIDDEN) == 0
            &&  (dir.file_attrs & ATTR_SUBDIR) == 0)
                if (file_matches (dir.file_name, filename))
                    convert (directory, dir.file_name);
          }
        while (read_dir (&dir));
        close_dir (&dir);
      }
    else
        convert (".", filename);
    exit (0);
}

