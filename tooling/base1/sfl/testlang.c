/*===========================================================================*
 *                                                                           *
 *  testlang.c - Test language conversions                                   *
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
    static char
        buffer [1024];
    static char
        text [100];

    puts (timestamp_string (buffer, "Doy=%y Year=%yy Full=%yyyy"));
    puts (timestamp_string (buffer, "Month=%mm, %mmm, %mmmm, %MMM, %MMMM"));
    puts (timestamp_string (buffer, "Day=%dd, %ddd, %dddd, %DDD, %DDDD"));
    puts (timestamp_string (buffer, "Dow=%w Week=%ww Quarter=%q"));
    puts (timestamp_string (buffer, "Time=%h:%m:%s:%c"));
    puts (timestamp_string (buffer, "Literal %%"));
    puts (timestamp_string (buffer, "%"));

    printf ("rm:%s\n", certify_the_number (text, 100, 4347L, "rm", 850 ));
    printf ("en-gb:%s\n", certify_the_number (text, 40, 212086798L, "en-gb", 850 ));
    printf ("fr-be:%s\n", certify_the_number (text, 100, 212086798L, "fr-be", 850 ));
    printf ("fr:%s\n", certify_the_number (text, 100, 212086798L, "fr", 850 ));
    printf ("nl:%s\n", certify_the_number (text, 100, 212086798L, "nl", 850 ));
    printf ("de:%s\n", certify_the_number (text, 100, 212030798L, "de", 850 ));
    printf ("no:%s\n", certify_the_number (text, 100, 212040798L, "no", 850 ));
    printf ("is:%s\n", certify_the_number (text, 100, 212086798L, "is", 850 ));
    printf ("sv:%s\n", certify_the_number (text, 100, 212086798L, "sv", 850 ));
    printf ("da:%s\n", certify_the_number (text, 100, 212086798L, "da", 850 ));
    printf ("pt:%s\n", certify_the_number (text, 100, 212086798L, "pt", 850 ));
    printf ("it:%s\n", certify_the_number (text, 100, 212086798L, "it", 850 ));
    printf ("es:%s\n", certify_the_number (text, 100, 212086798L, "es", 850 ));

    return 0;
}
