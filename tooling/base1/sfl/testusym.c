/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#include "sfl.h"

#if (defined (DOES_UNICODE_PRINTF))
static Bool
dump_symbol (USYMBOL *symbol, ...)
{
    wprintf (L"%s = %s\n", symbol-> name, symbol-> value);
    return (TRUE);
}
#endif


int
compare (const void *symb1, const void* symb2)
{
#if (defined (DOES_UNICODE))
    long
        val1,
        val2;

    val1 = wcstol ((*(USYMBOL **) symb1)-> value, NULL, 10);
    val2 = wcstol ((*(USYMBOL **) symb2)-> value, NULL, 10);
    if (val1 < val2)
        return (-1);
    else
    if (val1 > val2)
        return (1);
#endif

    return (0);
}

int main (int argc, char *argv [])
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRITNF))
    USYMTAB *
        table;

    table = usym_create_table ();
    usym_create_symbol (table, L"a Symbol 1", L"1");
    usym_create_symbol (table, L"d Symbol 2", L"2");
    usym_create_symbol (table, L"e Symbol 4", L"4");
    usym_create_symbol (table, L"b Symbol 3", L"3");
    usym_exec_all (table, dump_symbol);

    puts ("\nSorted by symbol value:");
    usym_sort_table (table, compare);
    usym_exec_all (table, dump_symbol);

    puts ("\nSorted by symbol name:");
    usym_sort_table (table, NULL);
    usym_exec_all (table, dump_symbol);

    usym_delete_table (table);

    mem_assert ();
#endif
    return (EXIT_SUCCESS);
}
