/*===========================================================================*
 *                                                                           *
 *  testbits.c - Test bitmap functions                                       *
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

#define NB_NUMBER 10

static long number_table [NB_NUMBER] = {
    123, 102789, 1256208, 5, 568, 45312, 9853415, 756321, 452123, 1285548 };

static long number_table2 [NB_NUMBER] = {
    123, 102789, 1256208, 6, 5689, 45312, 9853415, 7568321, 452123, 185548 };

int main (int argc, char *argv [])
{
    BITS
        *bits2,
        *bits,
        *result;
    FILE
        *bitfile;                       /*  Where we store the bits          */
    long
        index;
    char
        *buffer;
    long
        size;

    printf ("Saving a bitstring to testbits.out\n");
    bits = bits_create ();
    for (index = 0; index < NB_NUMBER; index++)
        bits_set (bits, number_table [index]);

    bitfile = fopen ("testbits.out", "w");
    bits_fput (bitfile, bits);
    fclose (bitfile);

    printf ("    have %ld bit set\n", bits_set_count (bits));

    bits_destroy (bits);


    printf ("Loading testbits.out and saving to testbits2.out\n");
    bitfile = fopen ("testbits.out", "r");
    bits = bits_fget (bitfile);
    fclose (bitfile);


    buffer = bits_save (bits);
    bitfile = fopen ("testbits2.out", "w");
    ASSERT (fwrite (buffer, 1, strlen (buffer), bitfile));
    fclose (bitfile);

    bits2 = bits_load (buffer);
    bitfile = fopen ("testbits3.out", "w");
    bits_fput (bitfile, bits2);
    fclose (bitfile);
    bits_destroy (bits2);

    mem_free (buffer);


    printf ("Comparing saved bitstring\n");
    for (index = 0; index < NB_NUMBER; index++)
      {
         printf ("%8ld -> %s\n",
                 number_table [index],
                 bits_test (bits, number_table [index])? "Ok": "FAIL");
      }
    printf ("List of bit set:\n");
    size = 0;
    while ((size = bits_search_set (bits, size, FALSE)) != -1)
        printf ("%ld\n", size);

    printf ("AND operation\n");
    bits2 = bits_create ();
    for (index = 0; index < NB_NUMBER; index++)
        bits_set (bits2, number_table2 [index]);

    result = bits_and (bits, bits2);
    size = 0;
    while ((size = bits_search_set (result, size, FALSE)) != -1)
        printf ("%ld\n", size);

    printf ("OR operation\n");
    bits_destroy (result);
    result = bits_or (bits, bits2);
    size = 0;
    while ((size = bits_search_set (result, size, FALSE)) != -1)
        printf ("%ld\n", size);

    printf ("In reverse \n");
    size = BIT_MAXBITS - 1;
    while ((size = bits_search_set (result, size, TRUE)) != -1)
        printf ("%ld\n", size);

    printf ("XOR operation\n");
    bits_destroy (result);
    result = bits_xor (bits, bits2);
    size = 0;
    while ((size = bits_search_set (result, size, FALSE)) != -1)
        printf ("%ld\n", size);

    printf ("Invert operation (first 10)\n");
    bits_destroy (result);
    result = bits_invert (bits);
    for (index = 0, size = 0; index < 10; index++)
      {
        size = bits_search_set (result, size, FALSE);
        printf ("%ld\n", size);
      }

    printf ("    have %ld bit set\n", bits_set_count (result));

    bits_destroy (result);
    bits_destroy (bits2);
    bits_destroy (bits);

    mem_assert ();
    return (EXIT_SUCCESS);
}
