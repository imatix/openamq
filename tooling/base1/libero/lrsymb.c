/*===========================================================================*
 *                                                                           *
 *  lrsymb.c - Symbol table management functions                             *
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

/*---------------------------------------------------------------------------

Symbol Table Management

Introduction

   The file lrsymb.c contains code for creating and managing symbol tables.
The functions are designed to be as general as possible (to support a wide
variety of applications), but at the same time fast.  The symbol table data
structure is based on a combined linked list & hash table representation.

   The file lrsymb.h contains definitions for the various structures and
external functions used in the lrsymb.c.

Structure

   Both the linked-list and hash-table representations have a guaranteed
ordering.  In the linked-list, new symbols are pushed on to the head of
the list.  In the hash table, each bucket just contains a pointer
to a linked-list of symbols.  When a new symbol is created, it is pushed
onto the front of this list.

   The reason that both data structures are used is to make the algorithm
faster.  Each representation has its stengths and weaknesses.  For instance,
if you wanted to lookup a symbol table entry for a given name
using the hash table you could find it immediately, whereas with the linked-
list, you would need to traverse most of the table to find the symbol.

Functions

lr_sym_tab *sym_create_table (void);

    - initialize a new symbol table
    - returns a pointer to the symbol table header


void sym_delete_table (lr_sym_tab *tabp);

    - deletes the given table, and frees all space used by it

    lr_sym_tab * tabp => symbol table pointer that was obtained from
                     sym_create_table()


symbol *sym_lookup (lr_sym_tab *tabp, char *s);

        - find a symbol in the symbol table
        - NULL is returned if a symbol is not found

        lr_sym_tab * tabp => symbol table pointer that was obtained from
                         sym_create_table ()
        char * s => the name to lookup in the symbol table


symbol *sym_create (lr_sym_tab *tabp, char *s, int type);

        - create a new symbol with the given name and type
        - returns a pointer to the new symbol table entry

        lr_sym_tab * tabp => symbol table pointer that was obtained from
                         sym_create_table ()
        char * s => the name to attach to the new symbol
        int type => the type to attach to the new symbol

symbol *sym_intern (lr_sym_tab *tabp, char *s, int type);

        - lookup the symbol in the symbol table.  If the symbol is not
          found, create a new symbol with the given name and type
        - the lookup follows the same scoping rules as sym_lookup()
        - the create (if necessary) follows the same rules as sym_create()
        - returns a pointer to either a symbol table entry with the same
          name (if one is found), or a new symbol table entry

        lr_sym_tab * tabp => symbol table pointer that was obtained from
                         sym_create_table ()
        char * s => the name look for, and (if a symbol is not found)
                    to attach to the new symbol
        int type => the type to attach to the new symbol (if one is
                    created)

symbol *sym_delete (lr_sym_tab *tabp, symbol *sp);

        - remove the given symbol from the symbol table, and search
          through the table for another symbol with the same name
        - return a pointer to the found symbol, or NULL if another symbol
          is not found

        lr_sym_tab * tabp => symbol table pointer that was obtained from
                         sym_create_table ()
        symbol * sp => the symbol to delete


void sym_set_svalue (symbol *sp, char *string_value);

        - sets the given symbol value to svalue; deallocates any previous
          value.


void sym_exec_all (lr_sym_tab *tabp, sym_func fn,...);

        - traverse the symbol table, performing the specified function
          for every symbol

        lr_sym_tab * tabp => symbol table pointer that was obtained from
                         sym_create_table ()
        sym_func fn => the function to call.  The first argument passed
                       this function will be the symbol pointer.  The
                       remaining arguments are supplied by arg
        void * arg => the second (and further) arguments that will be passed
                      to the given function


-End-------------------------------------------------------------------------*/

#include "prelude.h"                    /*  Public definitions               */
#include "lrpriv.h"                     /*  Private definitions              */

#include "lrsymb.h"                     /*  symbol table definitions         */

static int     compute_hash (const char *name);
static lr_symbol *lookup_entry (const lr_sym_tab *, const char *, const int);
static lr_symbol *create_entry (lr_sym_tab *, const char *, const int, const int);

lr_sym_tab *
lr_sym_create_table (void)
{
    lr_sym_tab *tabp;
    int i;

    tabp = (lr_sym_tab *) Check (malloc (sizeof (*tabp)));
    tabp-> syms = (lr_symbol *) 0;
    for (i = 0; i < SYM_HASH_SIZE; i++)
        tabp-> hash [i] = (lr_symbol *) 0;

    tabp-> size = 0;                    /*  No symbols attached yet          */
    return (tabp);
}

void
lr_sym_delete_table (lr_sym_tab *tabp)
{
    lr_symbol *sp, *next;

    ASSERT (tabp != NULL);
    next = 0;
    for (sp = tabp-> syms; sp; sp = next)
      {
        next = sp-> next;
        ASSERT (sp != NULL);
        if (sp-> svalue)
            free (sp-> svalue);
        free (sp);
      }
    free (tabp);
}


/*
 * compute the hash value of an identifier
 */
static int
compute_hash (const char *name)
{
    int hashvalue;

    for (hashvalue = 0; *name != '\0'; hashvalue += *name++);
    return (hashvalue & (SYM_HASH_SIZE - 1));
}

/*
 * Return pointer to existing named symbol, if found;
 * otherwise return NULL
 */
static lr_symbol *
lookup_entry (
    const lr_sym_tab *tabp,
    const char    *s,
    const int     hashvalue
)
{
    lr_symbol *sp;

    ASSERT (tabp != NULL);
    for (sp = tabp-> hash [hashvalue]; sp; sp = sp-> h_next)
        if (streq (sp-> name, s))
            return (sp);

    return (NULL);
}

lr_symbol *
lr_sym_lookup (const lr_sym_tab *tabp, const char *s)
{
    ASSERT (tabp != NULL);
    return (lookup_entry (tabp, s, compute_hash (s)));
}

/*
 * create a symbol table entry
 */
static lr_symbol *
create_entry (
    lr_sym_tab    *tabp,
    const char *s,
    const int  type,
    const int  hashvalue
)
{
    lr_symbol *sp;

    ASSERT (tabp != NULL);
    sp = (lr_symbol *) Check (malloc (sizeof (*sp) + strlen (s) + 1));
    sp-> name = (char *) sp + sizeof (*sp);
    strcpy (sp-> name, s);

    sp-> type = (byte) type;            /*  Set symbol type                  */
    sp-> flags = 0;                     /*  Reset all flags                  */
    sp-> hash_value = (byte) hashvalue;
    sp-> signature = SYM_SIGNATURE;

    /* confuse the hell out of everybody by setting lots of pointers */
    sp-> next   = tabp-> syms;
    sp-> h_next = tabp-> hash [hashvalue];
    sp-> prev   = NULL;
    sp-> h_prev = NULL;
    sp-> ivalue = 0;
    sp-> svalue = NULL;

    if (tabp-> syms)
        tabp-> syms-> prev = sp;

    if (tabp-> hash [hashvalue])
        tabp-> hash [hashvalue]-> h_prev = sp;

    tabp-> syms = sp;
    tabp-> hash [hashvalue] = sp;
    tabp-> size++;

    return (sp);
}

lr_symbol *
lr_sym_create (
    lr_sym_tab    *tabp,
    const char *s,
    const int  type
)
{
    return (create_entry (tabp, s, type, compute_hash (s)));
}

/*
 * Like lookup(), but add symbol with given type if not found
 */
lr_symbol *
lr_sym_intern (
    lr_sym_tab    *tabp,
    const char *s,
    const int  type
)
{
    lr_symbol *sp;
    int hashvalue;

    ASSERT (tabp != NULL);
    hashvalue = compute_hash (s);
    sp = lookup_entry (tabp, s, hashvalue);
    if (sp == NULL)
        return (create_entry (tabp, s, type, hashvalue));

    return (sp);
}

/*
 * Remove the given identifier from the symbol table.
 */
lr_symbol *
lr_sym_delete (lr_sym_tab *tabp, lr_symbol *sp)
{
    lr_symbol *orig;

    ASSERT (tabp != NULL);
    ASSERT (sp != NULL);
    ASSERT_SYM (sp);

    /*  find a symbol with the same name, or NULL if none found              */
    orig = sp;
    for (sp = sp-> h_next; sp; sp = sp-> h_next)
        if (streq (sp-> name, orig-> name))
            break;

    /*  fix up the pointers and remove the original symbol                   */
    if (orig-> prev)
        orig-> prev-> next = orig-> next;
    else
        tabp-> syms = orig-> next;

    if (orig-> h_prev)
        orig-> h_prev-> h_next = orig-> h_next;
    else
        tabp-> hash [orig-> hash_value] = orig-> h_next;

    if (orig-> next)
        orig-> next-> prev = orig-> prev;

    if (orig-> h_next)
        orig-> h_next-> h_prev = orig-> h_prev;

    tabp-> size--;

    ASSERT (orig != NULL);
    if (orig-> svalue)
        free (orig-> svalue);

    free (orig);
    return (sp);
}

void
lr_sym_values (lr_symbol *sp, const long ivalue, const char *svalue)
{
    ASSERT (sp != NULL);
    ASSERT_SYM (sp);

    if (sp-> svalue)
        free (sp-> svalue);
    sp-> svalue = StrDup ((char *) svalue);
    sp-> ivalue = ivalue;
}


/*
 * Invoke user function as (*fn)(lr_symbol *, arg)
 * for every symbol in the symbol table
 * Stop on zero return from (*fn)
 */
void
lr_sym_exec_all (
    const lr_sym_tab  *tabp,
    const lr_sym_func the_function, ...
)
{
    lr_symbol *sp;
    va_list argptr;                     /*  Argument list pointer            */

    ASSERT (tabp != NULL);
    va_start (argptr, the_function);    /*  Start variable args processing   */
    for (sp = tabp-> syms; sp; sp = sp-> next)
        if (!(*the_function)(sp, argptr))
            break;
    va_end (argptr);                    /*  End variable args processing     */
}
