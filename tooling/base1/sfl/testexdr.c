/*===========================================================================*
 *                                                                           *
 *  testexdr.c - Test EXDR conversions                                       *
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

void dump (byte *buffer, int buffer_size);

int main (int argc, char *argv [])
{
    static byte
            buffer [1024];
    static char
            string_value [1024];
    char    *string = string_value;
    int     buffer_size;
    Bool    Bool_value;
    byte    byte_value;
    dbyte   dbyte_value,
            size_value;
    qbyte   qbyte_value;

    puts ("--- bwq ---");
    buffer_size = exdr_write (buffer, "bwq", (byte) 0x12,
                             (dbyte) 0x12, (qbyte) 0x12L);
    dump (buffer, buffer_size);
    exdr_read (buffer, "bwq", &byte_value, &dbyte_value, &qbyte_value);
    printf ("%02x %04x %08lx\n", byte_value, dbyte_value, (long) qbyte_value);

    puts ("--- cdl ---");
    buffer_size = exdr_write (buffer, "cdl", (byte) 0x12,
                             (dbyte) 0x1234, (qbyte) 0x1234L);
    dump (buffer, buffer_size);
    exdr_read (buffer, "cdl", &byte_value, &dbyte_value, &qbyte_value);
    printf ("%02x %04x %08lx\n", byte_value, dbyte_value, (long) qbyte_value);

    puts ("--- bwq ---");
    buffer_size = exdr_write (buffer, "bwq", (byte) 0x12,
                             (dbyte) 0x1234, (qbyte) 0x123456L);
    dump (buffer, buffer_size);
    exdr_read (buffer, "bwq", &byte_value, &dbyte_value, &qbyte_value);
    printf ("%02x %04x %08lx\n", byte_value, dbyte_value, (long) qbyte_value);

    puts ("--- cdl ---");
    buffer_size = exdr_write (buffer, "cdl", (byte) 0x12,
                             (dbyte) 0x1234, (qbyte) 0x12345678L);
    dump (buffer, buffer_size);
    exdr_read (buffer, "cdl", &byte_value, &dbyte_value, &qbyte_value);
    printf ("%02x %04x %08lx\n", byte_value, dbyte_value, (long) qbyte_value);

    puts ("--- Bs ---");
    buffer_size = exdr_write (buffer, "Bs", TRUE, "AZaz");
    dump (buffer, buffer_size);
    exdr_read (buffer, "Bs", &Bool_value, &string);
    printf ("%d %s\n", Bool_value, string_value);

    puts ("--- mMc ---");
    buffer_size = exdr_write (buffer, "mMc", (dbyte) 4, "AZaz", (byte) 0xff);
    dump (buffer, buffer_size);
    exdr_read (buffer, "mMc", &dbyte_value, &string, &byte_value);
    printf ("%d %c%c%c%c %x\n", dbyte_value, string_value [0],
            string_value [1], string_value [2], string_value [3], byte_value);

    puts ("--- dmMq ---");
    dbyte_value = 0x1234;
    qbyte_value = 0x12345678L;
    buffer_size = exdr_write (buffer, "dmMq", dbyte_value,
                              (dbyte) strlen ("AZaz"), "AZaz", qbyte_value);
    dump (buffer, buffer_size);
    exdr_read (buffer, "dmMq", &dbyte_value,
               &size_value, &string, &qbyte_value);
    printf ("%04x %d %c%c%c%c %08lx\n", dbyte_value, size_value,
             string_value [0], string_value [1],
             string_value [2], string_value [3], (long) qbyte_value);

    return (EXIT_SUCCESS);
}


void dump (byte *buffer, int buffer_size)
{
    int
        byte_nbr;

    for (byte_nbr = 0; byte_nbr < buffer_size; byte_nbr++)
        printf ("%0x ", buffer [byte_nbr]);
    printf ("\n");
}
