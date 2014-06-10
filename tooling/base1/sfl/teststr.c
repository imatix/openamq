/*===========================================================================*
 *                                                                           *
 *  teststr.c - Test string manipulation functions                           *
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

#define NAME_TABLE_SIZE  18

typedef struct {
    char *string;                       /* Test string                       */
    char *pattern;                      /* Pattern to check                  */
    Bool ignore_case;                   /* Case sensitive test               */
    Bool result;                        /* Normal result of test             */
} PATTERN_TEST;

static char *surname [NAME_TABLE_SIZE] = {
    "Allricht",
    "Eberhard",
    "Engebrethson",
    "Heimbach",
    "Hanselmann",
    "Henzelmann",
    "Hildebrand",
    "Kavanagh",
    "Lind, Van",
    "Lukaschowsky",
    "McDonnell",
    "McGee",
    "Opnian",
    "Oppenheimer",
    "Swhgler",
    "Riedemanas",
    "Zita",
    "Zitzmeinn"
};

static char *soundex_value [NAME_TABLE_SIZE] = {
    "A462",
    "E166",
    "E521",
    "H512",
    "H524",
    "H524",
    "H431",
    "K152",
    "L530",
    "L222",
    "M235",
    "M200",
    "O155",
    "O155",
    "S460",
    "R355",
    "Z300",
    "Z325"
};

static PATTERN_TEST pattern_table [] = {
    {"filename.ext",      "*.ext",              TRUE,  TRUE},
    {"filename.ext",      "f?*.*",              TRUE,  TRUE},
    {"filename.ext.ext2", "*.ext2",             TRUE,  TRUE},
    {"filename.ext.ext2", "*.ex",               TRUE,  FALSE},
    {"f0A.doc",           "[a-z][0-9]*.doc",    TRUE,  TRUE},
    {"F3A.doc",           "[a-z][a-z0-9]*.doc", TRUE,  TRUE},
    {"f0A.doc",           "[a-z][0-9]*.do",     TRUE,  FALSE},
    {"filename.ext",      "*.ext",              FALSE, TRUE},
    {"filename.ext",      "f?*.*",              FALSE, TRUE},
    {"filename.ext.ext2", "*.ext2",             FALSE, TRUE},
    {"filename.ext.ext2", "*.ex",               FALSE, FALSE},
    {"f1A.doc",           "[a-z][0-9]*.doc",    FALSE, TRUE},
    {"f2A.doc",           "[a-z][0-9]*.do",     FALSE, FALSE},
    {"filename.ext",      "*.ext",              FALSE, TRUE},
    {"filename.ext",      "f?*.*",              FALSE, TRUE},
    {"filename.ext.ext2", "*.exT2",             FALSE, FALSE},
    {"filename.ext.ext2", "*.ex",               FALSE, FALSE},
    {"F8A.doc",           "[a-z][0-9]*.doc",    FALSE, FALSE},
    {"f5A.doc",           "[a-z][0-9]*.do",     FALSE, FALSE},
    {"xf8A.doc",          "x[a-z][0-9]*.doc",   FALSE, TRUE},
    {NULL,                NULL,                 FALSE, FALSE}
};

int main (int argc, char *argv [])
{
    char
        *alloc,
        dest [128],
        *table [10] = { "One", "Two", "Three", "Four", "Five",
                       "Six", "Seven", "Eight", "Nine", NULL },
        **new_table;
    DESCR
        *descr;
    int
        index,
        string;
    Bool
        result;

    puts ("Testing xstrcpy():");
    xstrcpy (dest, "This ", "Is ", "A ", "String", NULL);
    puts (dest);

    puts ("Testing xstrcpy():");
    alloc = xstrcpy (NULL, "This ", "Is ", "A ", "String", NULL);
    puts (alloc);

    puts ("Testing xstrcat():");
    xstrcat (dest, "1", "2", "3", NULL);
    puts (dest);

    puts ("Testing strt2descr():");
    descr     = strt2descr (table);
    printf ("Descriptor size=%ld\n", (long) descr-> size);

    new_table = descr2strt (descr);
    printf ("Contents of table: ");
    for (string = 0; new_table [string]; string++)
        printf ("[%s] ", new_table [string]);
    puts ("");

    printf ("Testing soundex():\n");
    for (index = 0; index < NAME_TABLE_SIZE; index++)
      {
        alloc = soundex (surname [index]);
        printf ("%20s -> %5s %s\n", 
                surname [index],
                alloc,
                streq (alloc, soundex_value [index])? "OK": "FAIL");
      }

    printf ("Testing match_pattern():\n\n");
    
    printf ("%-20s %-20s %-10s = %-10s => %s\n",
            "String", "Pattern", "Check case", "Result", "Test");
    printf ("---------------------------------------------------------------" \
            "----------\n");
    index = 0;
    while (pattern_table [index].string)
      {
        result = match_pattern (pattern_table [index].string,
                                pattern_table [index].pattern,
                                pattern_table [index].ignore_case);
        printf ("%-20s %-20s %-10s = %-10s => %s\n",
                pattern_table [index].string,
                pattern_table [index].pattern,
                pattern_table [index].ignore_case? "No": "Yes",
                result? "Ok": "Fail",
                (result == pattern_table [index].result)? "Ok": "Fail");
        index++;
      }
    return (EXIT_SUCCESS);
}
