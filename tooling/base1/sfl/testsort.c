#include "sfl.h"

void test_format (char *data, char *format)
{
    char
        *key;

    key = build_sort_key (data, format);
    if (key) {
        coprintf (key);
        mem_free (key);
    }
    else
        coprintf ("Illegal format: %s", format);
}

void main (void)
{
        
    test_format ("One,veg,,tomatoes,rice", "f,:1");
    test_format ("Fruit,veg,,tw,o", "f,:3-5");
    test_format ("Fruit,veg,,tomatoes,three,four,five", "f,:4-");
    test_format ("Fruit,veg,,tomatoes,rice", "f:1");

               /* 1234567890123456    */
    test_format ("1000000000000000", "c:1");
    test_format ("0123040000000000", "c:2-4,6");
    test_format ("0000000012345678", "c:9-");
    test_format ("0000000000000000", "c:99");

    test_format ("<data one=\"one\" two=\"two\" three = \"three\" />", "x:one");
    test_format ("<data one=\"one\" two=\"two\" three = \"three\" />", "x:two,three");
    test_format ("<data one=\"one\" two=\"two\" three = \"three\" />", "x:none");

    mem_assert ();
}
