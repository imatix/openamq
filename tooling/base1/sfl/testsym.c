/*===========================================================================*
 *                                                                           *
 *  testsym.c - Test symbol table functions                                  *
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

static Bool
dump_symbol (SYMBOL *symbol)
{
    printf ("%s = %s\n", symbol-> name, symbol-> value);
    return (TRUE);
}


int
compare (const void *symb1, const void* symb2)
{
    int
        val1,
        val2;

    val1 = atoi ((*(SYMBOL **) symb1)-> value);
    val2 = atoi ((*(SYMBOL **) symb2)-> value);
    if (val1 < val2)
        return (-1);
    else
    if (val1 > val2)
        return (1);

    return (0);
}

int main (int argc, char *argv [])
{
    SYMTAB *
        table;

    table = sym_create_table ();
    sym_create_symbol (table, "a Symbol 1", "1");
    sym_create_symbol (table, "d Symbol 2", "2");
    sym_create_symbol (table, "e Symbol 4", "4");
    sym_create_symbol (table, "b Symbol 3", "3");
    sym_exec_all (table, dump_symbol);

    puts ("\nSorted by symbol value:");
    sym_sort_table (table, compare);
    sym_exec_all (table, dump_symbol);

    puts ("\nSorted by symbol name:");
    sym_sort_table (table, NULL);
    sym_exec_all (table, dump_symbol);

    sym_delete_table (table);

    return (EXIT_SUCCESS);
}
