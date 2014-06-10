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

/*- Definitions -------------------------------------------------------------*/
#define TEST_BUFFER "this is a test buffer\naaaaaaaaaaaaaaaaaaaaa\n" \
                    "zzzzzzzzzzzzzzzzzzzzz\nâµ$éçà@ïüâùñ\n"

#define TEST_XML    L"<main attr = \"value1\" data = \"data1\" ><item>itemval" \
                    L"<sub_item>sub_item_val</sub_item>itemval2</item></main>"

/*- Functions declaration ---------------------------------------------------*/

void   print_format     (char *file_name);
void   test_write2file  (char *file_name, UCODE *buffer, long size,
                         byte format, int byte_order);
UCODE *test_readfile    (char *file_name, long size);
void   test_all         (char *file_name, UCODE *buffer, long size,
                         byte format, int byte_order);
void   test_unicode_xml (void);


/*########################### M A I N #######################################*/

int
main (int argc, char *argv [])
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    UCODE
        *buffer;
    long
        size;
    /* Convert ascii buffer to unicode                                       */
    buffer = ascii2ucode (TEST_BUFFER);
    if (buffer)
      {
        size   = wcslen (buffer);

        test_all ("test_ascii.txt",        buffer, size,
                     UNICODE_FORMAT_ASCII, BYTE_ORDER);
        test_all ("test_unicode_16le.txt", buffer, size,
                     UNICODE_FORMAT_UTF16, LITTLE_ENDIAN);
        test_all ("test_unicode_16be.txt", buffer, size,
                     UNICODE_FORMAT_UTF16, BIG_ENDIAN);
        test_all ("test_unicode_utf8.txt", buffer, size,
                     UNICODE_FORMAT_UTF8,  BYTE_ORDER);
         mem_free (buffer);
      }

    test_unicode_xml ();

    mem_assert ();
#endif
    return (0);
}

/*################### L O C A L  F U N C T I O N S ##########################*/

void 
print_format (char *file_name)
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    printf ("Format of file %s is %X\n",
            file_name,
            unicode_get_file_format (file_name));
#endif
}

void
test_write2file (char *filename, UCODE *buffer, long size, byte format,
                 int byte_order)
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    UFILE
        *stream;

    stream = unicode_open (filename, 'w');
    if (stream)
      {
        printf ("Writing file %s\n", filename);
        unicode_set_format (stream, format, byte_order);
        unicode_write      (buffer, size, stream);
        unicode_close      (stream);
      }
#endif
}


UCODE *
test_readfile (char *filename, long size)
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    UCODE
         *buffer = NULL;
    UFILE
         *stream;
    long
         read_size;

    stream = unicode_open (filename, 'r');
    if (stream)
      {
        buffer = unicode_alloc (size + 2);
        if (buffer)
          {
            printf ("Reading file %s\n", filename);
            read_size = unicode_read      (buffer, size, stream);
            if (read_size > 0)
                buffer [read_size] = 0;
            unicode_close      (stream);
          }
      }
    
    return (buffer);
#else
    return (NULL);
#endif
}

void
test_all (char *file_name, UCODE *buffer, long size, byte format, int byte_order)
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    UCODE
        *read_buffer;

    test_write2file (file_name, buffer, size, format, byte_order);
    print_format (file_name);
    read_buffer = test_readfile (file_name, size);
    if (read_buffer)
      {
        if (wcscmp (read_buffer, buffer) == 0)
            printf ("    Test OK\n");
        else
            wprintf (L"    Error %s is not %s\n", read_buffer, buffer);
        mem_free (read_buffer);
      }
    else
        printf ("    Error in read file\n");
#endif
}



void 
test_unicode_xml ()
{
#if (defined (DOES_UNICODE) && defined (DOES_UNICODE_PRINTF))
    UXML_ITEM
        *root = NULL,
        *item,
        *child,
        *value;
    UXML_ATTR
        *attr;
    UCODE
        *buffer;

    printf ("Test of XML\n");
    printf ("===========\n");
    if (uxml_load_string (&root, TEST_XML, FALSE) == UXML_NOERROR)
      {
        item = uxml_first_child (root);
        wprintf (L"Main item name is %s\n", uxml_item_name (item));
        FORUATTRIBUTES (attr, item)
          {
            wprintf (L"Attribute %s = %s\n",
                     uxml_attr_name  (attr),
                     uxml_attr_value (attr));
          }
        FORUCHILDREN (child, item)
          {
            wprintf (L"item name = %s\n", uxml_item_name (child));
            FORUVALUES (value, child)
              {
                wprintf (L"    have value %s\n", uxml_item_value (value));
              }
          }
        buffer = uxml_save_string (item);
        if (buffer)
          {
            wprintf (L"Result xml = %s\n", buffer);
            mem_free (buffer);
          }

        uxml_save_file (item, "test.xml", UNICODE_FORMAT_UTF8);
        uxml_free (root);
        root = NULL;

      }
#endif
}

