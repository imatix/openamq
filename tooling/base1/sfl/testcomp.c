/*===========================================================================*
 *                                                                           *
 *  testcomp.c - Test compression functions                                  *
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

#define BLOCK_SIZE      8192
#define RLE_BLOCK_SIZE  512
#define OUT_FILE        "testcomp.out"
#define LZ_SIGNATURE    0x12345670L
#define RLE_SIGNATURE   0x12345671L

local compress_file_lz  (FILE *input, FILE *output);
local expand_file_lz    (FILE *input, FILE *output);
local compress_file_rle (FILE *input, FILE *output);
local expand_file_rle   (FILE *input, FILE *output);

static byte
    in_block  [BLOCK_SIZE + 1],
    out_block [BLOCK_SIZE + 1];

int main (int argc, char *argv [])
{
    FILE
        *input,
        *output;
    qbyte
        signature;

    if (argc != 2 && argc != 3)
      {
        puts ("Usage: testcomp filename [rle]");
        return (EXIT_FAILURE);
      }
    input = fopen (argv [1], FOPEN_READ_BINARY);
    if (input == NULL)
      {
        printf ("Cannot open %s for input: %s\n", argv [1], strerror (errno));
        return (EXIT_FAILURE);
      }
    output = fopen (OUT_FILE, FOPEN_WRITE_BINARY);
    if (output == NULL)
      {
        printf ("Cannot open %s for output: %s\n", OUT_FILE, strerror (errno));
        return (EXIT_FAILURE);
      }
    /*  Look at signature to see if already compressed                       */
    ASSERT (fread (&signature, sizeof (signature), 1, input));
    if (signature == LZ_SIGNATURE)
        expand_file_lz (input, output);
    else
    if (signature == RLE_SIGNATURE)
        expand_file_rle (input, output);
    else
    if (argc == 3 && streq (argv [2], "rle"))
        compress_file_rle (input, output);
    else
        compress_file_lz (input, output);

    fclose (input);
    fclose (output);
    return (EXIT_SUCCESS);
}

local
compress_file_lz (FILE *input, FILE *output)
{
    word
        in_size,
        out_size;
    qbyte
        signature = LZ_SIGNATURE;

    fseek (input, 0, SEEK_SET);         /*  Start from beginning of file     */
    ASSERT (fwrite (&signature, sizeof (signature), 1, output));
    while ((in_size = fread (in_block, 1, BLOCK_SIZE, input)) > 0)
      {
        out_size = compress_block (in_block, out_block, in_size);
        ASSERT (fwrite (&out_size, 1, sizeof (out_size), output));
        ASSERT (fwrite (out_block, 1, out_size, output));
        printf ("lz compress: in=%d out=%d\n", in_size, out_size);
      }
}

local
expand_file_lz (FILE *input, FILE *output)
{
    word
        in_size,
        out_size;

    while ((fread (&in_size, 1, sizeof (in_size), input)) > 0)
      {
        ASSERT (fread (in_block, 1, in_size, input));
        out_size = expand_block (in_block, out_block, in_size);
        ASSERT (fwrite (out_block, 1, out_size, output));
        printf ("lz expand: in=%d out=%d\n", in_size, out_size);
      }
}

local
compress_file_rle (FILE *input, FILE *output)
{
    word
        in_size,
        out_size;
    qbyte
        signature = RLE_SIGNATURE;

    fseek (input, 0, SEEK_SET);         /*  Start from beginning of file     */
    ASSERT (fwrite (&signature, sizeof (signature), 1, output));
    while ((in_size = fread (in_block, 1, BLOCK_SIZE, input)) > 0)
      {
        out_size = compress_bits (in_block, out_block, in_size);
        ASSERT (fwrite (&out_size, 1, sizeof (out_size), output));
        ASSERT (fwrite (out_block, 1, out_size, output));
        printf ("rle compress: in=%d out=%d\n", in_size, out_size);
      }
}

local
expand_file_rle (FILE *input, FILE *output)
{
    word
        in_size,
        out_size;

    while ((fread (&in_size, 1, sizeof (in_size), input)) > 0)
      {
        ASSERT (fread (in_block, 1, in_size, input));
        out_size = expand_bits (in_block, out_block, in_size);
        ASSERT (fwrite (out_block, 1, out_size, output));
        printf ("rle expand: in=%d out=%d\n", in_size, out_size);
      }
}
