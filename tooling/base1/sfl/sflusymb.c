/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#include "prelude.h"                    /*  Universal header file            */
#include "sflunic.h"                    /*  Uniocode functions               */
#include "sflstr.h"                     /*  String functions                 */
#include "sflconv.h"                    /*  Conversion functions             */
#include "sfllist.h"                    /*  Linked-list functions            */
#include "sflmem.h"                     /*  Memory allocation functions      */
#include "sflprint.h"                   /*  snprintf functions               */
#include "sflusymb.h"                   /*  Prototypes for functions         */


static int usym_sort_by_name (const void *sym1, const void *sym2);


/*  ---------------------------------------------------------------------[<]-
    Function: usym_create_table_

    Synopsis: Creates a new symbol table.  Use the sym_create_table macro
    to call this function!  Returns a SYMTAB pointer which you must use in
    all future references to the symbol table.  The symbol table is
    maintained in memory until the program ends or you use sym_delete_table()
    to delete it.  Returns null if there was not enough memory to create the
    symbol table.
    ---------------------------------------------------------------------[>]-*/

USYMTAB *
usym_create_table_ (
    const char *filename,               /*  Name of source file making call  */
    size_t      lineno)                 /*  Line number in calling source    */
{
    USYMTAB
        *table;                         /*  Pointer to created table         */
    int
        hash_index;                     /*  Index into hash bucket           */

    /*  Allocate by going directly to mem_alloc_ function                    */
    table = mem_alloc_ (NULL, sizeof (USYMTAB), filename, lineno);
    if (table)
      {
        table-> symbols = NULL;         /*  No symbols attached yet          */
        table-> size    = 0;
        for (hash_index = 0; hash_index < USYM_HASH_SIZE; hash_index++)
            table-> hash [hash_index] = NULL;
      }
    return (table);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_delete_table

    Synopsis: Deletes the given symbol table.  First frees any memory space
    used by the table and attached symbols, including the user data block
    if that is not null.  If the table argument is NULL, does nothing.
    ---------------------------------------------------------------------[>]-*/

void
usym_delete_table (
    USYMTAB *table)                      /*  Symbol table to delete           */
{
    USYMBOL
        *symbol,                        /*  Pointer to symbol                */
        *next = NULL;                   /*    and to next symbol in list     */

    if (!table)
        return;                         /*  Do nothing if argument is null   */

    for (symbol = table-> symbols; symbol; symbol = next)
      {
        next = symbol-> next;           /*  Keep track of next in list       */
        mem_free (symbol-> value);      /*  Free value if used               */
        mem_free (symbol);              /*  Finally free symbol and name     */
      }
    mem_free (table);                   /*  Now free the table               */
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_empty_table

    Synopsis: Empties the given symbol table, by deleting all symbols.  You
    can then add new symbols.  If the table argument is NULL, does nothing.
    ---------------------------------------------------------------------[>]-*/

void
usym_empty_table (
    USYMTAB *table)                      /*  Symbol table to empty            */
{
    USYMBOL
        *symbol,                        /*  Pointer to symbol                */
        *next = NULL;                   /*    and to next symbol in list     */
    int
        hash_index;                     /*  Index into hash bucket           */

    if (!table)
        return;                         /*  Do nothing if argument is null   */

    for (symbol = table-> symbols; symbol; symbol = next)
      {
        next = symbol-> next;           /*  Keep track of next in list       */
        mem_free (symbol-> value);      /*  Free value if used               */
        mem_free (symbol);              /*  Finally free symbol and name     */
      }
    table-> symbols = NULL;             /*  No symbols attached yet          */
    table-> size    = 0;
    for (hash_index = 0; hash_index < USYM_HASH_SIZE; hash_index++)
        table-> hash [hash_index] = NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_merge_tables

    Synopsis: Imports the contents of one symbol table into another.  Will
    overwrite symbols with the same name.  Returns the number of symbols
    imported.  If there is a lack of available memory, will stop importing,
    and return negative the number of symbols imported so far.  
    ---------------------------------------------------------------------[>]-*/

int
usym_merge_tables (
    USYMTAB *table,                      /*  Symbol table to import into      */
    const USYMTAB *import)               /*  Symbol table to import from      */
{
    USYMBOL
        *symbol;                        /*  Next symbol in table             */
    int
        count = 0;

    ASSERT (table);
    ASSERT (import);

    for (symbol = import-> symbols; symbol; symbol = symbol-> next)
      {
        if (usym_assume_symbol (table, symbol-> name, symbol-> value) == NULL)
        {
          return (-(count));            /*  Failed => negative value returned*/
        }

        count++;
      }
      
    return (count);                     /*  Success => postive value returned*/
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_lookup_symbol

    Synopsis: Searches for a symbol, by name, in the specified symbol table.
    Returns a pointer to the symbol if found, or NULL if not found.  If more
    than one symbol with the same name exists, finds the latest entry.
    ---------------------------------------------------------------------[>]-*/

USYMBOL *
usym_lookup_symbol (
    const USYMTAB *table,                /*  Symbol table to search           */
    const UCODE   *name)                 /*  Symbol name to search for        */
{
    USYMBOL
        *symbol;                        /*  Search through hash bucket list  */

    ASSERT (table);

    for (symbol = table-> hash [usym_hash (name)];
         symbol;
         symbol = symbol-> h_next)
      {
        if (wcscmp (symbol-> name, name) == 0)
            return (symbol);
      }
    return (NULL);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_create_symbol_

    Synopsis: Creates a new symbol in the specified table.  Use the
    sym_create_symbol macro to call this function!  Returns a SYMBOL
    pointer to the created symbol, or NULL if there was not enough memory to
    create the symbol.  Initialises the symbol name and value to the values
    supplied.  Sets symbol data to NULL.  You can set this yourself if you
    need to, after calling this function.  Use mem_alloc() or mem_strdup()
    to assign values to the data block, otherwise you may cause problems when
    you delete the symbol or symbol table, since these functions free these
    fields.  You can create several symbols with the same name; the
    last-defined is always placed before older instances and will be found
    first by sym_lookup_symbol().

    Examples:
    USYMTAB
        *symbol_table;
    USYMBOL
        *new_symbol;

    symbol_table = usym_create_table ();
    ASSERT (symbol_table);
    new_symbol = usym_create_symbol (symbol_table, L"This name", L"This value");
    if (new_symbol)
      {
        new_symbol-> data = mem_alloc (sizeof (my_block));
        memcpy (new_symbol-> data, my_block);
      }
    ---------------------------------------------------------------------[>]-*/

USYMBOL *
usym_create_symbol_ (
    USYMTAB *table,                     /*  Symbol table to insert into      */
    const UCODE *name,                  /*  Name of symbol to create         */
    const UCODE *value,                 /*  Value of symbol to create        */
    const char *filename,               /*  Name of source file making call  */
    size_t      lineno)                 /*  Line number in calling source    */
{
    USYMBOL
        *symbol;                        /*  Allocated symbol                 */
    int
        hash;                           /*  Hash bucket no. for symbol       */

    ASSERT (table);

    symbol = mem_alloc_ (NULL, sizeof (*symbol) + ((wcslen (name) + 1) * UCODE_SIZE),
                         filename, lineno);
    if (symbol)
      {
        /*  Set the symbol pointers and fields                               */
        hash = usym_hash (name);
        symbol-> next   = table-> symbols;
        symbol-> prev   = NULL;
        symbol-> h_next = table-> hash [hash];
        symbol-> h_prev = NULL;
        symbol-> name   = (UCODE *)((byte*)symbol + sizeof (*symbol));
        symbol-> value  = mem_ustrdup (value);
        symbol-> data   = NULL;
        symbol-> hash   = (short)hash;
        wcscpy (symbol-> name, name);

        if (table-> symbols)
            table-> symbols-> prev = symbol;
        table-> symbols = symbol;

        if (table-> hash [hash])
            table-> hash [hash]-> h_prev = symbol;
        table-> hash [hash] = symbol;
        table-> size++;
      }
    return (symbol);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_assume_symbol_

    Synopsis: Searches for a symbol, by name, in the specified symbol table.
    If the symbol does not exist, creates the symbol as specified.  Returns
    a SYMBOL pointer to the existing or new symbol, or NULL if a new symbol
    could not be created.  The lookup and creation follow the same rules as
    sym_lookup_symbol() and sym_create_symbol().  The symbol's value is set
    to the supplied value in all cases.
    Do not call this function directly; pass through the sym_assume_symbol
    macro.
    ---------------------------------------------------------------------[>]-*/

USYMBOL *
usym_assume_symbol_ (
    USYMTAB *table,                      /*  Symbol table to search           */
    const UCODE *name,                   /*  Name of symbol to find/create    */
    const UCODE *value,                  /*  Value of symbol to create        */
    const char *filename,               /*  Name of source file making call  */
    size_t      lineno)                 /*  Line number in calling source    */
{
    USYMBOL
        *symbol;                        /*  Allocated or found symbol        */

    ASSERT (table);

    symbol = usym_lookup_symbol (table, name);
    if (symbol)
      {
        /*  Update the symbol value, if it has changed                       */
        if (symbol-> value && wcscmp (symbol-> value, value) != 0)
            usym_set_value (symbol, value);
      }
    else
        symbol = usym_create_symbol_ (table, name, value, filename, lineno);

    return (symbol);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_delete_symbol

    Synopsis: Removes the specified symbol from the symbol table and looks
    through the table for another with the same name.  Returns a pointer to
    the next symbol, or NULL if no further symbols were found with the same
    name.  Deallocates the symbol value, if not NULL.  NOTE: The SYMBOL
    must be part of the symbol table that is being modified.
    ---------------------------------------------------------------------[>]-*/

USYMBOL *
usym_delete_symbol (
    USYMTAB *table,                     /*  Symbol table to search           */
    USYMBOL *symbol)                    /*  Symbol to delete                 */
{
    USYMBOL
        *next;                          /*  Next symbol with same name       */

    ASSERT (table);
    ASSERT (symbol);

    /*  Find a symbol with the same name, or NULL if none found              */
    next = symbol;
    for (next = symbol-> h_next; next; next = next-> h_next)
        if (wcscmp (next-> name, symbol-> name) == 0)
            break;

    /*  Fix up the pointers and remove the original symbol                   */
    if (symbol-> prev)
        symbol-> prev-> next = symbol-> next;
    else
        table-> symbols = symbol-> next;

    if (symbol-> next)
        symbol-> next-> prev = symbol-> prev;

    if (symbol-> h_prev)
        symbol-> h_prev-> h_next = symbol-> h_next;
    else
        table-> hash [symbol-> hash] = symbol-> h_next;

    if (symbol-> h_next)
        symbol-> h_next-> h_prev = symbol-> h_prev;

    table-> size--;
    mem_free (symbol-> value);
    mem_free (symbol);

    return (next);
}

/*  ---------------------------------------------------------------------[<]-
    Function: usym_exec_all

    Synopsis: Traverses the symbol table, executing the specified function
    for every symbol in the table.  The function receives one or more
    arguments: the first argument is a SYMBOL pointer to the symbol, and
    following arguments as supplied by the caller.  Continues so long as the
    function returns TRUE; halts when every symbol has been processed, or
    when the function returns FALSE.  Returns the number of symbols processed,
    if all symbols were processed; or negative the number of symbols processed
    if processing stopped early due to the function returning FALSE or other
    errors.  The symbols are processed in reverse creation order; the newest
    symbol is processed first.

    Examples:
    static Bool
    dump_symbol (USYMBOL *symbol, ...)
    {
        wprintf (L"%s = %s\n", symbol-> name, symbol-> value);
        return (TRUE);
    }

    USYMTAB
        *table;

    table = usym_create_table ();
    usym_create_symbol (table, L"Symbol 1", L"value 1");
    usym_create_symbol (table, L"Symbol 2", L"value 2");
    usym_exec_all (table, dump_symbol);
    usym_delete_table (table);
    ---------------------------------------------------------------------[>]-*/

int
usym_exec_all (
    const USYMTAB *table,                /*  Symbol table to process          */
    usymfunc the_function, ...           /*  Function to call                 */
)
{
    USYMBOL
        *symbol;                        /*  Next symbol in table             */
    va_list
        argptr;                         /*  Argument list pointer            */
    int
        count = 0;                      /*  Number of symbols processed ok   */
    Bool
        alldone = TRUE;                 /*  Assume all symbols will be done  */
        
    ASSERT (table);

    va_start (argptr, the_function);    /*  Start variable args processing   */
    for (symbol = table-> symbols; symbol; symbol = symbol-> next)
      {
        if ((*the_function) (symbol, argptr))
            count++;
        else
          {
            alldone = FALSE;
            break;
	  }  
      }
    va_end (argptr);                    /*  End variable args processing     */

    if (alldone)
        return (count);                 /*  All symbols processed -> positive*/
    else
        return (-(count));              /*  Stopped early -> negative count  */
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_hash

    Synopsis: Computes the hash value for a null-delimited string.  The
    algorithm used is a simple checksum of the characters in the
    string.  The hash is within the range 0 .. USYM_HASH_SIZE - 1.
    ---------------------------------------------------------------------[>]-*/

int
usym_hash (
    const UCODE *name)
{
    int
        hash;                           /*  Computed hash value              */

    for (hash = 0; *name; name++)
        hash += *name;

    return (hash & (USYM_HASH_SIZE - 1));
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_get_name

    Synopsis: Returns the name of a particular symbol.  This can be used
    with the result of usym_lookup_symbol(), when you don't know the name
    of the symbol that you have got.
    ---------------------------------------------------------------------[>]-*/

const UCODE *
usym_get_name (const USYMBOL *symbol)
{
    ASSERT (symbol);
    return (symbol-> name);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_get_value

    Synopsis: Returns value for specified symbol, if defined in table, or
    a default value otherwise.  You can use this in situations where a symbol
    does not need to exist, and where frequent look-up by name is required.
    The symbol table must exist and be populated beforehand as appropriate.
    Returns a pointer to the value; you should never write to this string
    since it may exist as a string constant, not writable memory.

    Examples:
    value = usym_get_value (env, L"PATH", NULL);
    if (!value)
        puts ("PATH not defined");
    ---------------------------------------------------------------------[>]-*/

UCODE *
usym_get_value (
    const USYMTAB *table,               /*  Symbol table to process          */
    const UCODE   *name,                /*  Name of symbol to look for       */
    const UCODE   *default_value)       /*  Value to return if not defined   */
{
    USYMBOL
        *symbol;                        /*  Search through symbol table      */

    ASSERT (table);
    symbol = usym_lookup_symbol (table, name);
    if (symbol)
        return (symbol-> value);
    else
        return ((UCODE *) default_value);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_get_number

    Synopsis: Returns value for specified symbol, as a long value.  If the
    symbol is not defined in the table, returns a default value.

    Examples:
    value = usym_get_number (env, L"MAX_USERS", 10);
    ---------------------------------------------------------------------[>]-*/

long
usym_get_number (
    const USYMTAB *table,               /*  Symbol table to process          */
    const UCODE   *name,                /*  Name of symbol to look for       */
    const long default_value)           /*  Value to return if not defined   */
{
    UCODE
        *value;

    value = usym_get_value (table, name, NULL);
    return (value? wcstol (value, NULL, 10): default_value);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_get_boolean

    Synopsis: Returns value for specified symbol, as TRUE or FALSE.  If the
    symbol is not defined in the table, returns a default value.

    Examples:
    value = usym_get_boolean (env, L"CONNECTS_ENABLED", TRUE);
    ---------------------------------------------------------------------[>]-*/

Bool
usym_get_boolean (
    const USYMTAB *table,               /*  Symbol table to process          */
    const UCODE   *name,                /*  Name of symbol to look for       */
    const Bool    default_value)        /*  Value to return if not defined   */
{
    UCODE
        *value;

    value = usym_get_value (table, name, NULL);
    return (value? (conv_ustr_bool (value) != 0): default_value);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_set_value

    Synopsis: Assigns a new value for the symbol; this frees any previously
    assigned value and duplicates the supplied value, which must be a null
    terminated string.  If you want to assign binary values, you can use the
    symbol's data block.  If the value is NULL, any existing value is freed
    and the symbol value pointer is set to NULL.
    ---------------------------------------------------------------------[>]-*/

void
usym_set_value (
    USYMBOL *symbol,                    /*  Symbol to change                 */
    const UCODE *value)                 /*  New value to assign              */
{
    ASSERT (symbol);

    if (symbol-> value)
        mem_free (symbol-> value);      /*  Free existing value if any       */
    symbol-> value = mem_ustrdup (value);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usym_sort_table

    Synopsis: Sorts a symbol table using the qsort library function.  To
    access the symbol table, use the next and prev symbol pointers.  If
    the sort_function is NULL, sorts on the symbol name.

    Examples:
    int compare (const void *sym1, const void *sym2)
    {
        return (wcscmp (*(USYMBOL **) sym1)-> value,
                        *(USYMBOL **) sym2)-> value));
    }
    USYMTAB
        *table;
    USYMBOL
        *symbol;

    table = usym_create_table ();
    usym_create_symbol (table, L"Symbol 1", L"A");
    usym_create_symbol (table, L"Symbol 2", L"D");
    usym_create_symbol (table, L"Symbol 4", L"B");
    usym_create_symbol (table, L"Symbol 3", L"C");
    usym_sort_table (table, compare);
    for (symbol = symtab-> symbols; symbol; symbol = symbol-> next)
        wprintf (L"Symbol %s = %s\n", symbol-> name, symbol-> value);
    usym_delete_table (table);
    ---------------------------------------------------------------------[>]-*/

void
usym_sort_table (USYMTAB *table, usymsort sort_function)
{
    USYMBOL
        *symbol,
        **array;
    int
        index;

    ASSERT (table);

    if (table-> size == 0)
        return;

    array = mem_alloc (table-> size * sizeof (USYMBOL *));
    if (array == NULL)
        return;                         /*  Not enough memory                */

    /*  Build sorting array                                                  */
    for (symbol = table-> symbols, index = 0;
         symbol && index < table-> size;
         symbol = symbol-> next, index++)
        array [index] = symbol;

    if (sort_function == NULL)
        sort_function = usym_sort_by_name;

    qsort ((void *) array, table-> size, sizeof (USYMBOL *), sort_function);

    /*  Re-order symbol table links                                          */
    table-> symbols = array [0];
    for (index = 0; index < table-> size; index++)
      {
        symbol = array [index];
        symbol-> prev = (index > 0)? array [index -1]: NULL;
        symbol-> next = (index < table-> size - 1)? array [index + 1]: NULL;
      }
    mem_free (array);
}


static int
usym_sort_by_name (const void *sym1, const void *sym2)
{
    int
        compare;
    UCODE
        *name1,
        *name2;

    /*  In our sort order, ':' comes before everything else, so that symbols
     *  sort correctly like this:
     *  name
     *  name:one
     *  name:two
     *  name1
     *  name1:one ... etc
     */
    name1 = (*(USYMBOL **) sym1)-> name;
    name2 = (*(USYMBOL **) sym2)-> name;
    
    ustrconvch (name1, (UCODE)':', 1);
    ustrconvch (name2, (UCODE)':', 1);
    compare = wcscmp (name1, name2);
    ustrconvch (name1, 1, (UCODE)':');
    ustrconvch (name2, 1, (UCODE)':');
    return (compare);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usymb2ustrt_

    Synopsis: Exports the symbol table as an array of strings of the format
    "name=value".  Returns a pointer to the array.  The array is allocated
    dynamically.  The array ends with a NULL string.  To free the table,
    call strtfree().  If there was not enough memory to allocate the table,
    returns NULL.  See also symb2env().
    Do not call this function directly: pass through the symb2strt macro.
    ---------------------------------------------------------------------[>]-*/

UCODE **
usymb2ustrt_ (
    const USYMTAB *symtab,               /*  Symbol table to export           */
    const char   *filename,             /*  Name of source file making call  */
    size_t        lineno)               /*  Line number in calling source    */
{
    USYMBOL
        *symbol;                        /*  Pointer to symbol                */
    UCODE
        **strings,                      /*  Returned string array            */
        *name_and_value;                /*  Name=value string                */
    int
        string_nbr;                     /*  Index into symbol_array          */
    int
        n_and_v_len;

    if (!symtab)
        return (NULL);                  /*  Return NULL if argument is null  */

    /*  Allocate the array of pointers with one slot for the final NULL      */
    strings = mem_alloc_ (NULL, sizeof (UCODE *) * (symtab-> size + 1),
                          filename, lineno);
    if (strings)
      {
        string_nbr = 0;
        for (symbol = symtab-> symbols; symbol; symbol = symbol-> next)
          {
            /*  Allocate space for "name=value" plus final null char         */
	        n_and_v_len = wcslen (symbol-> name) + wcslen (symbol-> value) + 2;
            n_and_v_len *= UCODE_SIZE;

            name_and_value = mem_alloc_ (NULL, n_and_v_len, filename, lineno);
            if (name_and_value)         /*  Fail-safe if no memory left      */
                swprintf (name_and_value, L"%s=%s", 
			                  symbol-> name, symbol-> value);
            strings [string_nbr++] = name_and_value;
          }
        strings [string_nbr] = NULL;    /*  Store final null pointer         */
      }
    return (strings);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustrt2usymb_

    Synopsis: Converts a table of strings into a symbol table.  The input
    table consists of an array of null-terminated strings, terminated in a
    null pointer.  Ignores any strings that don't look like: "name=value".
    If the table contains multiple strings with the same name, the last
    instance is stored in the symbol table.  Note that if you omit the last
    null pointer in the input table, you will probably get an addressing
    error.  Returns NULL if there was insufficient memory to allocate the
    symbol table, or if the input argument was null.
    Do not call this function directly: pass through the strt2symb macro.
    ---------------------------------------------------------------------[>]-*/

USYMTAB *
ustrt2usymb_ (
    UCODE      **table,                  /*  String table to convert          */
    const char *filename,               /*  Name of source file making call  */
    size_t      lineno)                 /*  Line number in calling source    */
{
    USYMTAB
        *symtab;                        /*  Allocated symbol table           */
    UCODE
        *equals;                        /*  Position of '=' in string        */
    int
        string_nbr;                     /*  Index into string table          */

    if (!table)
        return (NULL);                  /*  Return NULL if argument is null  */

    symtab = usym_create_table_ (filename, lineno);
    if (symtab)
      {
        for (string_nbr = 0; table [string_nbr]; string_nbr++)
          {
            equals = wcschr (table [string_nbr], (UCODE)'=');
            if (equals)
              {
                *equals = 0;            /*  Cut into two strings             */
                usym_assume_symbol (symtab, table [string_nbr], equals + 1);
                *equals = (UCODE)'=';   /*  Restore previous state           */
              }
          }
      }
    return (symtab);
}


/*  ---------------------------------------------------------------------[<]-
    Function: usymb2descr_

    Synopsis: Exports the symbol table as a table of strings in a DESCR
    block.  Each string has the format "name=value".  The block ends with
    a null string.  Returns a pointer to the descriptor.  The descriptor is
    allocated dynamically; to free it, use mem_free().  If there was not
    enough memory to allocate the descriptor, returns NULL.
    Do not call this function directly: pass through the symb2descr macro.
    ---------------------------------------------------------------------[>]-*/

DESCR *
usymb2descr_ (
    const USYMTAB *symtab,               /*  Symbol table to export           */
    const char   *filename,             /*  Name of source file making call  */
    size_t        lineno)               /*  Line number in calling source    */
{
    UCODE
        **strings;                      /*  Formatted string array           */
    DESCR
        *descr;                         /*  Formatted descriptor             */

    if (!symtab)
        return (NULL);                  /*  Return NULL if argument is null  */

    /*  Convert symbol table to strings                                      */
    strings = usymb2ustrt_ (symtab, filename, lineno);
    descr   = ustrt2descr (strings);     /*  And build into descriptor        */
    ustrtfree (strings);
    return (descr);
}


/*  ---------------------------------------------------------------------[<]-
    Function: descr2symb_

    Synopsis: Converts a DESCR block into a symbol table.  The descriptor
    consists of a block of null-terminated strings, terminated in a double
    null byte.  Ignores any strings that don't look like: "name=value".
    If the block contains multiple strings with the same name, the last
    instance is stored in the symbol table.  Returns NULL if there was not
    enough memory to allocate the symbol table, or if the input argument was
    null.
    Do not call this function directly: pass through the descr2symb macro.
    ---------------------------------------------------------------------[>]-*/

USYMTAB *
udescr2symb_ (
    const DESCR  *descr,                /*  Descriptor to convert            */
    const char   *filename,             /*  Name of source file making call  */
    size_t        lineno)               /*  Line number in calling source    */
{
    USYMTAB
        *symtab;                        /*  Allocated symbol table           */
    UCODE
        **strings;                      /*  Formatted string array           */

    if (!descr)
        return (NULL);                  /*  Return NULL if argument is null  */

    if (!descr-> data)              
        return (usym_create_table ());   /*  Create new symtab if no data     */
        
    strings = descr2ustrt (descr);      /*  Convert descriptor to strings    */
    symtab  = ustrt2usymb_ (strings, filename, lineno);
    ustrtfree (strings);
    return (symtab);
}


/*  ---------------------------------------------------------------------[<]-
    Function: utok_subst

    Synopsis: Performs symbol substitution into the specified string.
    Returns a newly-allocated string containing the result, or NULL if there
    was not enough memory.  The symbol table holds the set of symbols that
    may be inserted.  Undefined symbols are inserted as an empty value.
    Symbols are specified in the string using this syntax: $(name), where
    'name' is case-sensitive.  This version does not allow embedded symbols.
    ---------------------------------------------------------------------[>]-*/

UCODE *
utok_subst (const UCODE *string, USYMTAB *symbols)
{
    UCODE
        *sym_start,
        *sym_end,
        *sym_name,                      /*  Symbol name to look for          */
        *copied_to,                     /*  Up to where we copied so far     */
        *cur_result,                    /*  Current result buffer            */
        *new_result;                    /*  After adding next symbol         */
    int
        sym_length;                     /*  Length of symbol name            */
    USYMBOL
        *symbol;                        /*  Symbol in symbol table           */

    ASSERT (string);
    ASSERT (symbols);

    /*  Parse from left to right, looking for $(...) patterns                */
    cur_result = mem_ustrdup (L"");      /*  Result buffer is empty           */
    sym_start  = wcschr (string, (UCODE)'$');
    copied_to  = (UCODE *) string;
    while (sym_start)
      {
        sym_end = wcschr (++sym_start, (UCODE)')');
        if (*sym_start == (UCODE)'('
        &&   sym_end
        &&  *sym_end   == (UCODE)')')
          {
            /*  First, copy string up to sym_start                           */
            size_t cur_len = wcslen (cur_result);
            ASSERT ((sym_start - copied_to) >= 0);

            new_result = unicode_alloc (cur_len + (sym_start - copied_to));
            if (new_result == NULL)
              {
                mem_free (cur_result);
                return (NULL);
              }

            wcsncpy (new_result, cur_result, (cur_len + 1));
            wcsncat (new_result, copied_to, sym_start - 1 - copied_to);
            mem_free (cur_result);
            cur_result = new_result;
            copied_to  = sym_end + 1;

            /*  Now translate and insert symbol                              */
            sym_length = sym_end - sym_start - 1;
            sym_name   = unicode_alloc (sym_length + 1);
            if (sym_name == NULL)
              {
                mem_free (cur_result);
                return (NULL);
              }

            memcpy (sym_name, sym_start + 1, sym_length * UCODE_SIZE);
            sym_name [sym_length] = 0;
            symbol = usym_lookup_symbol (symbols, sym_name);
            mem_free (sym_name);
            if (symbol)
              {
                xustrcpy_debug ();
                new_result = xustrcpy (NULL, cur_result, symbol-> value, NULL);
                mem_free (cur_result);
                cur_result = new_result;
              }
          }
        else
            sym_end = sym_start + 1;

        sym_start = wcschr (sym_end, (UCODE)'$');
      }
    /*  Copy anything remaining in the string                                */
    xustrcpy_debug ();
    new_result = xustrcpy (NULL, cur_result, copied_to, NULL);
    mem_free (cur_result);
    return (new_result);
}
