/*===========================================================================*
 *                                                                           *
 *  linend.c - end-of-line convertor                                         *
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
    static char
        fullname [1024],
        backup [1024];

    if (directory)
        xstrcpy (fullname, directory, "/", filename, NULL);
    else
        strcpy (fullname, file_where ('r', ".", filename, NULL));
    if (!file_exists (fullname))
      {
        printf ("`%s' does not exist", fullname);
        exit (1);
      }
    printf ("Converting %s...\n", fullname);

    /*  Check if file contains suspicious binary characters                  */
    if (file_is_text (fullname) != 1)
      {
        printf ("%s is a binary file - skipping\n", fullname);
        return;
      }

    strcpy (backup, fullname);
    fixed_extension (backup, fullname, "bak");
    file_delete (backup);
    file_rename (fullname, backup);
    if (file_copy_text (fullname, backup) != -1)
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
        puts ("linend - part of the iMatix RealiBase Gaggle");
        puts ("Set the right End Of Line character (CRLF or LF)");
        puts ("for text file on the current operating system\n");
        puts ("syntax: linend [/?] [filename_pattern]");
        puts (" /?       - show help on command");
        puts (" filename - file to expand. the match of pattern is not case");
        puts ("            sensitive.\n");
        puts ("Pattern syntax: the pattern can have this characters");
        puts ("  '?' matches any character in the file name");
        puts ("  '*' matches the remainder of the string.");
        puts ("you can use a range of character in pattern between");
        puts (" bracket character like '[aeioy]' or '[a-z0-9]'.");
        exit (0);
    }
    filename = argv [1];
    if ( strchr (filename, '*') 
    ||   strchr (filename, '?')
    ||  (strchr (filename, '[') && strchr (filename, ']'))
       )
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
                if (match_pattern (dir.file_name, filename, TRUE))
                    convert (directory, dir.file_name);
          }
        while (read_dir (&dir));
        close_dir (&dir);
      }
    else
        convert (NULL, filename);
    exit (0);
}

