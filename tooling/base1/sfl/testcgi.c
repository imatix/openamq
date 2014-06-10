/*===========================================================================*
 *                                                                           *
 *  testcgi.c - Test CGI functions                                           *
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

#include "sfl.h"                        /*  SFL functions                    */

static Bool dump_symbol (SYMBOL *symbol);
static char *time_str   (void);

int
main (int argc, char *argv [])
{
    SYMTAB
        *symbols;
    int
        index;
    char
        *curdir;
    size_t
        content_length;

    printf ("Content-Type: text/html\n\n");
    printf ("<HTML><HEAD><TITLE>CGI Test Program</TITLE></HEAD>\n");
    printf ("<BODY>\n");
    printf ("<H1>CGI Test Program</H1>\n");

    /*  Print argument variables, if any                                     */
    if (argc > 1)
      {
        printf ("<H2>Arguments To Testcgi</H2>\n");
        printf ("<PRE>\n");
        for (index = 1; index < argc; index++)
            printf ("<B>Argument %d </B>: %s\n", index, argv [index]);
        printf ("</PRE>\n");
      }

    /*  Print POST data variables coming from stdin, if present              */
    content_length = (size_t) env_get_number ("HTTP_CONTENT_LENGTH", 0);
    if (content_length)
      {
        symbols = sym_create_table ();
        if (symbols)
          {
            if (cgi_parse_file_vars (symbols, stdin, "", content_length) > 0)
              {
                printf ("<H2>POST Data Variables</H2>\n");
                printf ("<PRE>\n");
                sym_exec_all (symbols, dump_symbol);
                printf ("</PRE>\n");
              }
            sym_delete_table (symbols);
          }
      }

    /*  Print URL query variables coming from QUERY_STRING                   */
    symbols = sym_create_table ();
    if (symbols)
      {
        if (cgi_parse_query_vars (symbols,
            env_get_string ("QUERY_STRING", ""), "") > 0)
          {
            printf ("<H2>QUERY_STRING Variables</H2>\n");
            printf ("<PRE>\n");
            sym_exec_all (symbols, dump_symbol);
            printf ("</PRE>\n");
          }
        sym_delete_table (symbols);
      }

    /*  Print environment variables                                          */
    symbols = env2symb ();
    if (symbols)
      {
        printf ("<H2>Environment Variables</H2>\n");
        printf ("<PRE>\n");
        sym_exec_all (symbols, dump_symbol);
        sym_delete_table (symbols);
        printf ("</PRE>\n");
      }

    curdir = get_curdir ();
    printf ("<H2>Miscellaneous Information</H2>\n");
    printf ("<P>Working directory: %s\n", curdir);
    printf ("<P>Current date and time: %s\n", time_str ());
    printf ("</BODY></HTML>\n");

    mem_free (curdir);
    mem_assert ();
    return (EXIT_SUCCESS);
}

/*  -------------------------------------------------------------------------
 *  This function is invoked by sym_exec_all() to process each item in
 *  the symbol table.  It always returns TRUE to keep sym_exec_all() going.
 */

static Bool
dump_symbol (SYMBOL *symbol)
{
    printf ("<B>%-20s</B> = %s\n", symbol-> name, symbol-> value);
    return (TRUE);
}


/*  -------------------------------------------------------------------------
 *  time_str
 *
 *  Returns the current date and time formatted as: "yy/mm/dd hh:mm:ss".
 *  The formatted time is in a static string that each call overwrites.
 */

static char *
time_str (void)
{
    static char
        formatted_time [18];
    time_t
        time_secs;
    struct tm
        *time_struct;

    time_secs   = time (NULL);
    time_struct = localtime (&time_secs);

    snprintf (formatted_time, sizeof (formatted_time), 
 	                      "%2d/%02d/%02d %2d:%02d:%02d",
                              time_struct-> tm_year,
                              time_struct-> tm_mon + 1,
                              time_struct-> tm_mday,
                              time_struct-> tm_hour,
                              time_struct-> tm_min,
                              time_struct-> tm_sec);
    return (formatted_time);
}
