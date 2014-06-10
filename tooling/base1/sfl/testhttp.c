/*===========================================================================*
 *                                                                           *
 *  testhttp.c - Test http functions                                         *
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


void url_struct_print (URL *url)
{
    if (url) {
        if (url-> scheme)
            printf ("scheme=<%s>\n", url-> scheme);
        if (url-> authority)
            printf ("authority=<%s>\n", url-> authority);
        if (url-> path)
            printf ("path=<%s>\n", url-> path);
        if (url-> query)
            printf ("query=<%s>\n", url-> query);
        if (url-> fragment)
            printf ("fragment=<%s>\n", url-> fragment);
        if (url-> http) {
            if (url-> http-> host)
                printf ("http.host=<%s>\n", url-> http-> host);
            if (url-> http-> port)
                printf ("http.port=<%d>\n", url-> http-> port);
        }
    }
}


int main (int argc, char *argv[])
{
    const char
        *url_strings[] = {
            "http://www.example.com/",
            "http://www.example.com/woot",
            "http://www.example.com/woot#bar",
            "http://www.example.com:8080/woot",
            "http://www.example.com:8080/woot?foo=bar&baz=quux",
            "http://www.example.com:8080/woot#quuux",
            "http://www.example.com:8080/woot?bar#foo",
            "http://company.com:3333/long/path/to/resource.cgi?var1=val1&var2=val2#ref1",
            "http:/kickme",
            ":///////",
            NULL };
    const char
        **url_string;
    char
        *parsed_url_string;
    URL
        *url;

    /*  Loop through all test strings, dumping each URL we get back          */
    for (url_string = url_strings; *url_string != NULL; url_string++) {
        printf ("---%s\n", *url_string);
        url = url_from_string (*url_string);
        url_struct_print (url);
        /*  Convert it back to a string and print the result                 */
        if (url) {
            parsed_url_string = url_to_string (url);
            if (parsed_url_string) {
                printf (">>>%s\n", parsed_url_string);
                mem_free (parsed_url_string);
            }
        }
        url_free (url);
    }

    mem_assert ();
    exit (0);
}

