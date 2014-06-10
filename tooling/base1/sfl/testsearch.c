/*===========================================================================*
 *                                                                           *
 *  testsearch.c - Test free-text search functions                           *
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
 *===========================================================================*/

#include "sfl.h"

static SYMTAB
    *index_table;

/*  Function to put an index record into the index symbol table
    Destroys the bitstring after saving it.
 */
int index_put (char *field, char *value, char type, BITS *bits, Bool create)
{
    char
        *bitstring;

    bitstring = bits_save (bits);
    coprintf ("Save: %s/%s/%c", field, value, type);
    sym_assume_symbol (index_table, strprintf ("%s\001%s\001%c", field, value, type), bitstring);
    bits_destroy (bits);
    mem_free (bitstring);
    return (0);
}


/*  Function to get an index record from the index symbol table
    Loads and returns a freshly created bitstring, or NULL if not found.
 */
BITS *index_get (char *field, char *value, char type)
{
    char
        *bitstring;

    coprintf ("Load: %s/%s/%c", field, value, type);
    bitstring = sym_get_value (index_table, strprintf ("%s\001%s\001%c", field, value, type), NULL);
    return (bitstring? bits_load (bitstring): NULL);
}

Bool sym_dump (SYMBOL *symbol, ...)
{
    BITS
        *bits;
    long
        itemid;
        
    bits = bits_load (symbol-> value);
    coprintf ("%s", symbol-> name);
    itemid = bits_search_set (bits, 0, FALSE);
    while (itemid >= 0) {
        coprintf ("  - item %ld", itemid);
        itemid = bits_search_set (bits, itemid, FALSE);
    }
    bits_destroy (bits);
    return (TRUE);
}

int main (int argc, char *argv [])
{
    LIST
        *icrit_list;
    SEARCH
        *search;                        /*  Search information               */
    long
        count,                          /*  Return from function calls       */
        itemid;

    /*  Step 1: build search index  */
    /*      We use a symbol table to hold the index - normally this would
            be a database table of some kind.  Let's empty the damn thing...
     */
    index_table = sym_create_table ();

    /*      Now, index a bunch of records.  This builds an icrit list with
            each criteria nicely prepared in an ICRIT structure.
     */
    coprintf ("Indexing test data...");
    icrit_list = search_init_icrit ();
    search_build_icrit (icrit_list, 1, "name",    "John Roger Hammock");
    search_build_icrit (icrit_list, 1, "address", "Blueberry hill 22, 1000 Carlockston, uk");
    search_build_icrit (icrit_list, 2, "name",    "Jenny Jèz Slaughter");
    search_build_icrit (icrit_list, 2, "address", "12 Carmichael Street, Intrapment ZXP109");
    search_build_icrit (icrit_list, 3, "name",    "Bill Masters");
    search_build_icrit (icrit_list, 3, "address", "14 Stuart Mains, Bridgetown");
    search_build_icrit (icrit_list, 4, "name",    "Jenny Masters");
    search_build_icrit (icrit_list, 4, "address", "21 Random Hill, Bridgetown");

    /*      We add this information to the index.  The search_merge_icrit ()
            function calls our index_get/_put functions in order to read/write
            index records.
     */
    coprintf ("Saving index records...");
    count = search_merge_icrit (icrit_list, index_get, index_put);
    search_free_icrit (&icrit_list);
    coprintf ("  -- %ld records saved", count);
    
    /*      OK, now we've built the index.  We dump it just for fun...
    sym_exec_all (index_table, sym_dump);
     */

    /*  Step 2: try some searches   */
    coprintf ("Testing search");
    search = search_open (FALSE, index_get);
    search_ordered (search, "name", "jenny", FALSE);
    search_ordered (search, "address", "hill bridge", FALSE);

    /*      Let's see what records we found...
     */
    coprintf ("Found %ld records", search_count (search));
    itemid = search_first (search);
    while (itemid > 0) {
        coprintf ("  - item %ld", itemid);
        itemid = search_next (search);
    }

    /*      Fun's over, close up
     */
    search_close   (search);

    /*  Step 3: clean-up  */
    sym_delete_table (index_table);
    mem_assert ();
    return (0);
}

