/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#include "prelude.h"                    /*  Universal include file           */
#include "sfldate.h"                    /*  Date and time functions          */
#include "sflfile.h"                    /*  File access functions            */
#include "sfllist.h"                    /*  List access functions            */
#include "sflmem.h"                     /*  Memory allocation functions      */
#include "sflstr.h"                     /*  String access functions          */
#include "sflunic.h"                    /*  Unicode function                 */
#include "sfluxml.h"                    /*  Unicode XML definitions          */
#include "sflusymb.h"
#include "sfluhttp.h"                   /*  Meta-UCODE encoding/decoding      */
#include "sfluxmll.h"                   /*  Include prototype data           */

#define BUFFER_SIZE     16384

typedef struct _UXML_BUFFER             /*  API to load XML files/string     */
{
    int
            state;                      /*  Most of time : BUF_READY         */
    int
            pageln,                     /*  length of current page loaded    */
            cur;                        /*  index in page to last read UCODE  */
    UCODE
           *page;                       /*  page currently loaded            */
    UFILE
            *fd;
    char
            *fname;                     /*  file name if from file (for errors) */
    UCODE
            cur_line[BUFFER_SIZE+1];    /*  last line returned so far        */
    int
            line_nbr,                   /*  number of \n encountered         */
            char_nbr,                   /*  index for cur_line               */
	        virtual_char_nbr;           /*  Virtual position we are at       */
    int 
            tab_width_left;             /*  Count of spaces left to return   */
    byte
        cmap [256];                     /*  Character classification tables  */
} UXML_BUFFER;

static UCODE
        last_error[BUFFER_SIZE+1];

/*  Function prototypes                                                      */

static int       uxml_save_file_item        (UFILE *xmlfile, UXML_ITEM *item, int generation,
                                             byte format);
static void      uxml_save_string_item      (UCODE *uxml_string, UXML_ITEM *item);
static int       uxml_item_size             (UXML_ITEM *item);
static void      uxml_save_string_item_utf8 (char *uxml_string, UXML_ITEM *item);
static int       uxml_item_size_utf8        (UXML_ITEM *item);
static void      init_charmaps              (byte *cmap);
static void      build_charmap              (byte *cmap, byte flag, char *chars);

static char     *make_long_filename         (const char *path, const char *filename);
static void      dispose_uxml_buffer        (UXML_BUFFER *buf);
static UXML_BUFFER *init_uxml_buffer_from_file      (const char * name);
static UXML_BUFFER *init_uxml_buffer_from_string    (const UCODE * xmlstring);
static int       buf_seek_next             (UXML_BUFFER * buf, UCODE c, Bool *non_white_skipped);
static int       buf_load_page             (UXML_BUFFER * buf);
static int       uxml_load_item             (UXML_ITEM *item, UXML_BUFFER *buf);
static int       buf_seek_next_nonwhite    (UXML_BUFFER *buf);
static UCODE     *buf_get_name              (UXML_BUFFER *buf);
static UCODE     *buf_get_string            (UXML_BUFFER *buf);
static UCODE     *buf_get_item_value        (UXML_BUFFER *buf);
static UCODE     *buf_get_white_char        (UXML_BUFFER *buf);
static UCODE     *buf_get_until_gt          (UXML_BUFFER *buf);
static int       uxml_load_attribute        (UXML_ITEM *item, UXML_BUFFER *buf);
static int       uxml_load_item_value       (UXML_ITEM *item, UXML_BUFFER *buf, UCODE *blanks);
static void      set_error                 (UXML_BUFFER *buf, UCODE *format, ...);

/*  Character classification tables and macros                               */

static UCODE
        *errors[10] = { L"unknown error",            /* 0 */
                        L"'?' expected",             /* 1 */
                        L"char expected",            /* 2 */
                        L"'-->' expected",           /* 3 */
                        L"'>' expected",             /* 4 */
                        L"tag name expected",        /* 5 */
                        L"'<' expected",             /* 6 */
                        L"Unexpected character: "    /* 7 */

                        };


#define CMAP_NAME        1              /*  Normal name character           */
#define CMAP_NAME_OPEN   2              /*  Valid character to start name   */
#define CMAP_QUOTE       4              /*  Possible string delimiters       */
#define CMAP_PRINTABLE   8              /*  Valid characters in literal      */
#define CMAP_DECIMAL    16              /*  Decimal digits                   */
#define CMAP_HEX        32              /*  Hexadecimal digits               */
#define CMAP_SPACE      64              /*  White space                      */

                                        /*  Macros for character mapping:    */
#define is_name(ch)      (ch > 255 || (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_NAME)))
#define is_name_open(ch) (ch > 255 || (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_NAME_OPEN)))
#define is_quote(ch)     (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_QUOTE))
#define is_printable(ch) (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_PRINTABLE))
#define is_decimal(ch)   (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_DECIMAL))
#define is_hex(ch)       (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_HEX))
#define is_space(ch)     (ch < 255 && (buf-> cmap [(byte) (ch)] & CMAP_SPACE))


                                        /*  Macros for UXML_BUFFER */
#define BUF_READY               (0)     /*  Possible states */
#define BUF_FILEERROR           (1)
#define BUF_UNEXPECTED_ERROR    (2)
#define BUF_DISPOSED            (3)
#define BUF_END                 (4)
#define MAX_BUF_SIZE          (64*1024) /*  page length when read from file */
#define UXML_END_ITEM            (-1)

#define METACHAR_SIZE           10

/*  Note this set of macros is used for reading through a buffer
 *  character by character, keeping track of lines, and when to load 
 *  more data from a file (if loading from a file).  The code is mostly 
 *  straight forward but for backwards compatibility, we need to expand 
 *  tabs properly based on their position on the line.  
 *
 *  We do this by returning spaces for the right number of "fake" reads 
 *  for the tab character, and then go back to advancing through the 
 *  buffer.  When we encounter a tab character we set tab_width_left
 *  to the number of space characters required, and decrement it each
 *  time we read out a space.  We are done when tab_width_left = 0, and 
 *  we are asked for the _next_ character.  We always return a space
 *  when the current character is a tab.
 *
 *  In order to track the supposed location along the line, we use
 *  virtual_char_nbr to track how far along the line we notionally are.
 *  char_nbr tracks the "real" position along the line.
 */

#define CHECK_BUF_STATE                                                       \
    if (buf-> state != BUF_READY)                                             \
      {                                                                       \
        res = (buf-> state == BUF_END)                                        \
            ? UXML_LOADERROR                                                   \
            : UXML_FILEERROR;                                                  \
      }

#define CHECK_BUF_STATE_AND_BREAK_ON_ERROR(_ERROR_IDX_)                       \
    CHECK_BUF_STATE;                                                          \
    if (res != UXML_NOERROR)                                                   \
      {                                                                       \
        set_error (buf, errors[_ERROR_IDX_]);                                 \
        break;                                                                \
      }

#define buf_page_fault(_BUF_)                                                 \
    ((_BUF_)-> cur == (_BUF_)-> pageln)



/*  Update our record of the current line. 
 *  NOTE: We record the characters as they come in, and notice a new
 *  line if the _previous_ character we recorded was a '\n'.  char_nbr
 *  records the number of characters in our line; virtual_char_nbr
 *  records our offset from the start of the line including the offset
 *  gained by expanding tabs.
 */
#define buf_update_cur_line(_BUF_)                                            \
  {                                                                           \
    if ((_BUF_)-> state == BUF_READY )                                        \
      {                                                                       \
        ASSERT ((_BUF_)-> char_nbr > 0);                                      \
        if ((_BUF_)-> cur_line[(_BUF_)-> char_nbr-1] == (UCODE)'\n'           \
        ||  (_BUF_)-> cur_line[(_BUF_)-> char_nbr-1] == (UCODE)'\r')          \
          {                                                                   \
            (_BUF_)-> char_nbr = 0;                                           \
	    (_BUF_)-> virtual_char_nbr = -1;                                   \
            (_BUF_)-> line_nbr++;                                             \
          }                                                                   \
                                                                              \
        (_BUF_)-> cur_line[(_BUF_)-> char_nbr] = (_BUF_)-> page[(_BUF_)-> cur];\
        (_BUF_)-> char_nbr ++;                                                \
        (_BUF_)-> virtual_char_nbr ++;                                        \
        if ((_BUF_)-> char_nbr >= BUFFER_SIZE)                                \
            (_BUF_)-> char_nbr = 1;                                           \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        (_BUF_)-> cur_line[0] = 0;                                            \
        (_BUF_)-> line_nbr    = -1;                                           \
        (_BUF_)-> char_nbr    = 0;                                            \
	(_BUF_)-> virtual_char_nbr = 0;                                       \
      }                                                                       \
  }

/*  Check if we have started a tab character -- to get the count right, we
 *  must be called _after_ the position is updated, so we get correct offset.
 */
#define buf_check_for_tab_start(_BUF_)                                        \
    if (((_BUF_)-> page[(_BUF_)-> cur]) == (UCODE)'\t')                              \
	(_BUF_)-> tab_width_left = 8 - ((_BUF_)-> virtual_char_nbr & 7);      

#define buf_next(_BUF_)                                                       \
  {                                                                           \
    if ((_BUF_)-> tab_width_left)                                             \
      {                                                                       \
	(_BUF_)-> tab_width_left --;                                          \
	(_BUF_)-> virtual_char_nbr ++;                                        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _BUF_-> cur++;                                                        \
        if (buf_page_fault (_BUF_))                                           \
            buf_load_page (_BUF_);                                            \
        buf_update_cur_line (_BUF_);                                          \
        buf_check_for_tab_start (_BUF_);                                      \
        if ((_BUF_)-> tab_width_left)                                         \
    	    (_BUF_)-> tab_width_left --;                                      \
     }                                                                        \
  }

#define buf_get_next(_BUF_, _RES_)                                            \
  {                                                                           \
    if ((_BUF_)-> tab_width_left)                                             \
      {                                                                       \
	(_BUF_)-> tab_width_left --;                                          \
	(_BUF_)-> virtual_char_nbr ++;                                        \
	_RES_ = ' ';                                                          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _BUF_-> cur++;                                                        \
        if ((buf_page_fault(_BUF_)) && (buf_load_page(_BUF_) != BUF_READY))   \
            _RES_ = 0;                                                        \
        else                                                                  \
            _RES_ = (_BUF_)-> page[(_BUF_)-> cur];                            \
        buf_update_cur_line (_BUF_);                                          \
        buf_check_for_tab_start (_BUF_);                                      \
        if ((_BUF_)-> tab_width_left)                                         \
          {                                                                   \
	    (_BUF_)-> tab_width_left --;                                      \
	    _RES_ = (UCODE)' ';                                                      \
          }                                                                   \
      }                                                                       \
  }

/*  If the current character is a tab, we return a space; otherwise as is    */
#define buf_get_current(_BUF_)                                                \
    (((_BUF_)-> page[(_BUF_)-> cur]) == (UCODE)'\t'                                  \
     ? (UCODE)' ' : ((_BUF_)-> page[(_BUF_)-> cur]))

#ifdef _DEBUG_
    #define my_assert(_COND_)                                       \
        if (! (_COND_)) {                                           \
            printf ("\nmy_ASSERT (%d)\n", LineNr);                  \
            ASSERT (_COND_);                                        \
        }

    void uinvariant (UXML_BUFFER * buf, size_t LineNr)
    {
        my_assert (buf);

        if (buf-> state != BUF_READY)
          {
            my_assert (buf-> page != NULL);
            my_assert (buf-> pageln > 0);
            my_assert (buf-> cur < buf-> pageln);
          }
    }

    #define UXML_BUF_INVARIANT(_BUF_)    uinvariant (_BUF_, __LINE__)
#else
    #define UXML_BUF_INVARIANT(_BUF_)
#endif

/*  ---------------------------------------------------------------------[<]-
    Function: uxml_save_file

    Synopsis: Saves an XML tree to the specified file.  Returns UXML_NOERROR
    or UXML_FILEERROR.
    ---------------------------------------------------------------------[>]-*/

int
uxml_save_file (
    UXML_ITEM   *item,
    const char *filename,
    byte        unicode_format)
{
    UFILE
        *xmlfile;                       /*  XML output stream                */
    int
        count;                          /*  How many symbols did we save?    */

    ASSERT (item);
    ASSERT (filename);
//    init_charmaps ();                   /*  Initialise character maps        */

    if ((xmlfile = unicode_open (filename, 'w')) == NULL)
        return UXML_FILEERROR;           /*  No permission to write file      */

    unicode_set_format (xmlfile, unicode_format, BYTE_ORDER);

    /*  Write XML file header                                                */
    unicode_write (L"<?xml version=\"1.0\"?>\r\n", 23, xmlfile);

    /*  Output XML root                                                      */
    count = uxml_save_file_item (xmlfile, item, 0, unicode_format);

    /*  Output a final carriage return  */
    unicode_write (L"\r\n", 2, xmlfile);

    unicode_close (xmlfile);
    return UXML_NOERROR;
}


/*  -------------------------------------------------------------------------
 *  init_charmaps
 *
 *  Initialise character map bit tables.  These are used to speed-up
 *  token recognition and collection.
 */

static void
init_charmaps (byte *cmap)
{
    memset (cmap, 0, 256);    /*  Clear all bitmaps                */

    /*  Name     ::= (Letter | '_' | ':') (NameChar)*                        */
    /*  NameChar ::= Letter | Digit | MiscName                               */

    /*  Map fixed character sets to various bitmaps                          */
    build_charmap (cmap, CMAP_NAME, "abcdefghijklmnopqrstuvwxyz");
    build_charmap (cmap, CMAP_NAME, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    build_charmap (cmap, CMAP_NAME, "0123456789");
    build_charmap (cmap, CMAP_NAME, "_:.-");

    build_charmap (cmap, CMAP_NAME_OPEN, "abcdefghijklmnopqrstuvwxyz");
    build_charmap (cmap, CMAP_NAME_OPEN, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    build_charmap (cmap, CMAP_NAME_OPEN, "_:");

    build_charmap (cmap, CMAP_QUOTE, "\"'");

    /*  Printable characters.  ???                                           */
    build_charmap (cmap, CMAP_PRINTABLE, "abcdefghijklmnopqrstuvwxyz");
    build_charmap (cmap, CMAP_PRINTABLE, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    build_charmap (cmap, CMAP_PRINTABLE, "0123456789");
    build_charmap (cmap, CMAP_PRINTABLE, "!@#$%^&*()-_=+[]{}\\|;:'\"<>,./?`~ ");

    build_charmap (cmap, CMAP_DECIMAL, "0123456789");

    build_charmap (cmap, CMAP_HEX, "abcdefghijklmnopqrstuvwxyz");
    build_charmap (cmap, CMAP_HEX, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    build_charmap (cmap, CMAP_HEX, "0123456789");

    build_charmap (cmap, CMAP_SPACE, " \t\r\n");
}


/*  -------------------------------------------------------------------------
 *  build_charmap
 *
 *  Encode character string and flag into character map table.  Flag should
 *  be a 1-bit value from 1 to 128 (character map is 8 bits wide).
 */

static void
build_charmap (byte *cmap, byte flag, char *string)
{
    for (; *string; string++)
        cmap [(byte) *string] |= flag;
}


static int
uxml_save_file_item (UFILE *xmlfile, UXML_ITEM *item, int generation, byte format)
{
    int
        count = 1;                      /*  Count 1 for current item         */
    UXML_ITEM
        *child,
        *sibling;
    UXML_ATTR
        *attr;
    UCODE
        *token,
        *item_name,
        *attr_name,
        *ptr;
    Bool
        pretty;

    /*  First output item name and attributes                                */
    token = unicode_alloc (BUFFER_SIZE + 1);
    item_name  = uxml_item_name (item);
    if (item_name)
      {
        unicode_fprintf (xmlfile, L"<%s", item_name);
        FORUATTRIBUTES (attr, item)
          {
            attr_name  = uxml_attr_name  (attr);
            ptr        = uxml_attr_value (attr);
            if (format == UNICODE_FORMAT_ASCII)
                http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
            else
                http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
            unicode_fprintf (xmlfile, L"\r\n%*s%s = \"%s", (generation + 1) * 4, L"",
                                              attr_name, token);
            while (*ptr)
              {
                if (format == UNICODE_FORMAT_ASCII)
                    http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
                else
                    http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
                unicode_fprintf (xmlfile, L"\r\n%s", token);
              }
            unicode_fprintf (xmlfile, L"\"");
          }

        /*  If value or children exist, use long form, otherwise short form  */
        if ((child = uxml_first_child (item)))
          {
            unicode_fprintf (xmlfile, L">");

            pretty = TRUE;
            for (sibling = child ;
                 sibling != NULL ;
                 sibling = uxml_next_sibling (sibling))
                if (! uxml_item_name (sibling))
                    pretty = FALSE;
                else
                    break;

            for (; child != NULL; child  = uxml_next_sibling (child))
              {
                if (pretty)
                    unicode_fprintf (xmlfile, L"\r\n%*s", (generation + 1) * 4, "");

                count += uxml_save_file_item (xmlfile, child, generation + 1, format);
                if (uxml_item_name (child))
                  {
                    pretty = TRUE;
                    for (sibling = uxml_next_sibling (child) ;
                         sibling != NULL ;
                         sibling = uxml_next_sibling (sibling))
                        if (! uxml_item_name (sibling))
                            pretty = FALSE;
                        else
                            break;
                  }
              }

            if (pretty)
                unicode_fprintf (xmlfile, L"\r\n%*s", generation * 4, "");

            unicode_fprintf (xmlfile, L"</%s>", item_name);
          }
        else
            unicode_fprintf (xmlfile, L"/>");
      }
    else            /*  Not name => this is a value node  */
      {
        ptr = uxml_item_value (item);
        if (ptr)
            while (*ptr)
              {
                if (format == UNICODE_FORMAT_ASCII)
                    http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
                else
                    http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
                unicode_fprintf (xmlfile, L"%s", token);
              }
      }
    mem_free (token);
    return (count);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_save_string

    Synopsis: Saves an XML tree to a string.  Returns a freshly-allocated
    string containing the XML tree, or NULL if there was insufficient memory
    to allocate a new string.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_save_string (
    UXML_ITEM *item)
{
    size_t
        uxml_size;
    UCODE
        *uxml_string;

    ASSERT (item);
//    init_charmaps ();                   /*  Initialise character maps        */
    uxml_size = uxml_item_size (item);
    uxml_string = unicode_alloc (uxml_size + 1000);
    if (uxml_string)
        uxml_save_string_item (uxml_string, item);
    return (uxml_string);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_save_utf8_string

    Synopsis: Saves an XML tree to a string.  Returns a freshly-allocated
    string containing the XML tree, or NULL if there was insufficient memory
    to allocate a new string. the xml is utf8 encoded
    ---------------------------------------------------------------------[>]-*/

char *
uxml_save_utf8_string (UXML_ITEM   *item)
{
    size_t
        xml_size;
    char
        *xml_string;

    ASSERT (item);
//    init_charmaps ();                   /*  Initialise character maps        */
    xml_size = uxml_item_size_utf8 (item);
    xml_string = mem_alloc (xml_size + 1000);
    if (xml_string)
        uxml_save_string_item_utf8 (xml_string, item);
    return (xml_string);
}

/*  Return string size of XML item and all its children                      */

static int
uxml_item_size (UXML_ITEM *item)
{
    int
        item_size;
    UXML_ITEM
        *child = NULL;
    UXML_ATTR
        *attr;
    UCODE
        *token,
        *item_name,
        *attr_name,
        *ptr;

    item_size  = 0;
    token = unicode_alloc (BUFFER_SIZE + 1);

    item_name = uxml_item_name (item);
    if (item_name)
      {
        item_size++;
        item_size += wcslen (item_name);
        FORUATTRIBUTES (attr, item)
          {
            attr_name  = uxml_attr_name  (attr);
            ptr        = uxml_attr_value (attr);
            item_size += 3 + wcslen (attr_name);
            while (*ptr)
              {
                http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
                item_size += wcslen (token);
              }
            item_size++;
          }

        /*  If value or children exist, use long form, otherwise short form  */
        if ((child = uxml_first_child (item)))
          {
            item_size++;
            for (child  = uxml_first_child (item);
                 child != NULL;
                 child  = uxml_next_sibling (child))
              {
                item_size += uxml_item_size (child);
              }
            item_size += wcslen (item_name) + 3;
          }
        else
            item_size += 2;
      }
    else            /*  No name => this is a value node                      */
      {
        ptr = uxml_item_value (item);
        while (*ptr)
          {
            http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
            item_size += wcslen (token);
          }
      }
    mem_free (token);
    return (item_size);
}


static void
uxml_save_string_item (UCODE *xml_string, UXML_ITEM *item)
{
    UXML_ITEM
        *child = NULL;
    UXML_ATTR
        *attr;
    UCODE
        *token,
        *item_name,
        *attr_name,
        *ptr;

    /*  First output item name and attributes                                */
    token = unicode_alloc (BUFFER_SIZE + 1);
    xml_string [0] = 0;
    item_name = uxml_item_name (item);
    if (item_name)
      {
        wcscat (xml_string, L"<");
        wcscat (xml_string, item_name);
        FORUATTRIBUTES (attr, item)
          {
            attr_name  = uxml_attr_name  (attr);
            ptr        = uxml_attr_value (attr);
            wcscat (xml_string, L" ");
            wcscat (xml_string, attr_name);
            wcscat (xml_string, L"=\"");
            while (*ptr)
              {
                http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
                wcscat (xml_string, token);
              }
            wcscat (xml_string, L"\"");
          }

        /*  If value or children exist, use long form, otherwise short form  */
        if ((child = uxml_first_child (item)))
          {
            wcscat (xml_string, L">");
            for (child  = uxml_first_child (item);
                 child != NULL;
                 child  = uxml_next_sibling (child))
              {
                xml_string += wcslen (xml_string);
                uxml_save_string_item (xml_string, child);
              }
            wcscat (xml_string, L"</");
            wcscat (xml_string, item_name);
            wcscat (xml_string, L">");
          }
        else
            wcscat (xml_string, L"/>");
      }
    else            /*  No name => this is a value node                      */
      {
        ptr = uxml_item_value (item);
        while (*ptr)
          {
            xml_string += wcslen (xml_string);
            http_encode_umeta (token, &ptr, BUFFER_SIZE, FALSE);
            wcscat (xml_string, token);
          }
      }
    mem_free (token);
}


/*  Return string size of XML item and all its children                      */

static int
uxml_item_size_utf8 (UXML_ITEM *item)
{
    int
        item_size;
    UXML_ITEM
        *child = NULL;
    UXML_ATTR
        *attr;
    UCODE
        *token,
        *item_name,
        *attr_name,
        *ptr;

    item_size  = 0;
    token = unicode_alloc (BUFFER_SIZE + 1);

    item_name = uxml_item_name (item);
    if (item_name)
      {
        item_size++;
        item_size += ucode2utf8_size (item_name);
        FORUATTRIBUTES (attr, item)
          {
            attr_name  = uxml_attr_name  (attr);
            ptr        = uxml_attr_value (attr);
            item_size += 3 + ucode2utf8_size (attr_name);
            while (*ptr)
              {
                http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
                item_size += ucode2utf8_size (token);
              }
            item_size++;
          }

        /*  If value or children exist, use long form, otherwise short form  */
        if ((child = uxml_first_child (item)))
          {
            item_size++;
            for (child  = uxml_first_child (item);
                 child != NULL;
                 child  = uxml_next_sibling (child))
              {
                item_size += uxml_item_size_utf8 (child);
              }
            item_size += ucode2utf8_size (item_name) + 3;
          }
        else
            item_size += 2;
      }
    else            /*  No name => this is a value node                      */
      {
        ptr = uxml_item_value (item);
        while (*ptr)
          {
            http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
            item_size += ucode2utf8_size (token);
          }
      }
    mem_free (token);
    return (item_size);
}


static void
uxml_save_string_item_utf8 (char *xml_string, UXML_ITEM *item)
{
    UXML_ITEM
        *child = NULL;
    UXML_ATTR
        *attr;
    UCODE
        *token,
        *item_name,
        *attr_name,
        *ptr;
    char
        *buffer;
#define SAVE_UTF8(string) {buffer = ucode2utf8 (string); \
                           if (buffer){ strcat (xml_string, buffer); mem_free (buffer);}}
    /*  First output item name and attributes                                */
    token = unicode_alloc (BUFFER_SIZE + 1);
    xml_string [0] = 0;
    item_name = uxml_item_name (item);
    if (item_name)
      {
        strcat (xml_string, "<");
        SAVE_UTF8 (item_name);

        FORUATTRIBUTES (attr, item)
          {
            attr_name  = uxml_attr_name  (attr);
            ptr        = uxml_attr_value (attr);
            strcat (xml_string, " ");            
            SAVE_UTF8 (attr_name);
            strcat (xml_string, "=\"");
            while (*ptr)
              {
                http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
                SAVE_UTF8 (token);
              }
            strcat (xml_string, "\"");
          }

        /*  If value or children exist, use long form, otherwise short form  */
        if ((child = uxml_first_child (item)))
          {
            strcat (xml_string, ">");
            for (child  = uxml_first_child (item);
                 child != NULL;
                 child  = uxml_next_sibling (child))
              {
                xml_string += strlen (xml_string);
                uxml_save_string_item_utf8 (xml_string, child);
              }
            strcat (xml_string, "</");
            SAVE_UTF8 (item_name);
            strcat (xml_string, ">");
          }
        else
            strcat (xml_string, "/>");
      }
    else            /*  No name => this is a value node                      */
      {
        ptr = uxml_item_value (item);
        while (*ptr)
          {
            xml_string += strlen (xml_string);
            http_encode_umeta_ascii (token, &ptr, BUFFER_SIZE, FALSE);
            SAVE_UTF8 (token);
          }
      }
    mem_free (token);
}

/*  ---------------------------------------------------------------------[<]-
    Function: uxml_error

    Synopsis: Returns the last error message generated by uxml_load.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_error (void)
{
    return (last_error);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_seems_to_be

    Synopsis: Reads the first line of a file.  Returns UXML_NOERROR if file
    is found and has a valid XML header, UXML_FILEERROR if the file could not
    be opened and UXML_LOADERROR if the first line is not an XML header.
    ---------------------------------------------------------------------[>]-*/

int
uxml_seems_to_be (const char *path,
                  const char *filename)
{
    UXML_BUFFER
            *buf = 0;
    int
            res = UXML_NOERROR;
    UCODE
            c;
    char
            *name;
    Bool
            non_white_skipped;

    name = make_long_filename (path, filename);
    if (!name)
        res = UXML_FILEERROR;

    if (res == UXML_NOERROR)
      {
        buf = init_uxml_buffer_from_file (name);
        mem_free (name);
        if (!buf)
            res = UXML_FILEERROR;
      }

    if (res == UXML_NOERROR)
      {
        buf_seek_next (buf, (UCODE)'<', &non_white_skipped);
        if (non_white_skipped)
            res = UXML_LOADERROR;        /*  unexpected UCODE were found       */
        else if (buf-> state != BUF_READY)
            res = UXML_LOADERROR;
      }

    if (res == UXML_NOERROR)
      {
        buf_get_next(buf, c);
        if (c != '?')
            res = UXML_LOADERROR;
      }


    if (res == UXML_NOERROR)
      {
        buf_next(buf);
        do
          {
            res = buf_seek_next (buf, (UCODE)'?', NULL);
            CHECK_BUF_STATE_AND_BREAK_ON_ERROR(1);          /* ? expected */
            buf_get_next(buf, c);
            CHECK_BUF_STATE_AND_BREAK_ON_ERROR(2);          /* UCODE expected */
            buf_next(buf)
          } while (c != (UCODE)'>');
      }

    if (buf)
        dispose_uxml_buffer (buf);

    return res;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_load_file

    Synopsis: Loads the contents of an XML file into a given XML tree,
    or a new XML tree if none was specified.  The XML data is not checked
    against a DTD.  Returns one of UXML_NOERROR, UXML_FILEERROR,
    UXML_LOADERROR.  If the parameter `allow_extended' is TRUE then the file
    may contain more than one XML item.  The following attributes are
    defined in the root item of the XML tree.
    <TABLE>
    filename        Name of the XML input file
    filetime        Modification time of the file, "HHMMSSCC"
    filedate        Modification date of input file, "YYYYMMDD"
    </TABLE>
    Looks for the XML file on the specified path symbol, or in the current
    directory if the path argument is null.  Adds the extension ".xml"
    to the file name if there is none already included.
    ---------------------------------------------------------------------[>]-*/

int
uxml_load_file (UXML_ITEM   **item,
               const char  *path,
               const char  *filename,
               Bool         allow_extended)
{
    UXML_BUFFER
           *buf = NULL;
    int
            res = UXML_NOERROR;
    Bool
            non_white_skipped;
    char
           *name;
    UCODE
            *uname,
            buffer [60];

    ASSERT (filename);

    name = make_long_filename (path, filename);
    if (!name)
      {
        set_error (buf, L"Could not find XML file: %s", filename);
        res = UXML_FILEERROR;
      }

    if (res == UXML_NOERROR)
      {
        buf = init_uxml_buffer_from_file (name);
        if (!buf)
          {
            set_error (buf, L"Could not open XML file: %s", filename);
            res = UXML_FILEERROR;
          }
        else
          buf-> fname = strdupl (filename);
      }
    if ((!*item) && (res == UXML_NOERROR))
      {
        *item = uxml_new (NULL, L"root", L"");
        ASSERT (*item);
        /*  SM : should this init be done even when root item is NOT created ? */
      }

    if ((res == UXML_NOERROR) && (!allow_extended))
      {
        uname = ascii2ucode (name);
        if (uname)
          {
            uxml_put_attr (*item, L"filename", uname);
            mem_free (uname);
          }
        swprintf (buffer, L"%ld", timer_to_date (get_file_time (name)));
        uxml_put_attr (*item, L"filedate", buffer);
        swprintf (buffer, L"%ld", timer_to_time (get_file_time (name)));
        uxml_put_attr (*item, L"filetime", buffer);
      }

    while (res == UXML_NOERROR)
      {
        buf_seek_next (buf, (UCODE)'<', &non_white_skipped);
        if (non_white_skipped)
          {
            res = UXML_LOADERROR;        /*  unexpected UCODE were found       */
            break;
          }
        if (buf-> state == BUF_END)     /*  we processed the entire file     */
            break;
        CHECK_BUF_STATE_AND_BREAK_ON_ERROR(7);
        if (!allow_extended && (uxml_first_child(*item) != NULL))
          {
            res = UXML_LOADERROR;        /* root has children                 */
            break;
          }
        buf_next(buf);
        res = uxml_load_item (*item, buf); /* we try to load it               */
        if (res == UXML_END_ITEM)
          {
            set_error (buf, L"Unexpected character: /");
            res = UXML_LOADERROR;
          }
      }

    if (buf)
        dispose_uxml_buffer (buf);
    if (name)
        mem_free (name);

    return res;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_load_string

    Synopsis: Loads an XML string into a new XML tree.  The XML data is not
    checked against a DTD.  See uxml_load() for details.
    ---------------------------------------------------------------------[>]-*/

int
uxml_load_string (UXML_ITEM   **item,
                 const UCODE *xmlstring,
                 Bool  allow_extended)
{
    int
            res = UXML_NOERROR;
    UXML_BUFFER
           *buf;
    Bool
            non_white_skipped;

    ASSERT (xmlstring);


    buf = init_uxml_buffer_from_string (xmlstring);
    if (!buf)
        res = UXML_FILEERROR;

    if ((!*item) && (res == UXML_NOERROR))
      {
        *item = uxml_new (NULL, L"root", L"");
        ASSERT (*item);
      }
    while (res == UXML_NOERROR)
      {
        buf_seek_next (buf, (UCODE)'<', &non_white_skipped);
        if (non_white_skipped)
          {
            res = UXML_LOADERROR;        /*  unexpected UCODE were found       */
            break;
          }
        if (buf-> state == BUF_END)     /*  we processed the entire file     */
            break;
        CHECK_BUF_STATE_AND_BREAK_ON_ERROR(7);
        if (!allow_extended && (uxml_first_child(*item) != NULL))
          {
            res = UXML_LOADERROR;        /* root has children                 */
            break;
          }
        buf_next(buf);
        res = uxml_load_item (*item, buf); /* we try to load it               */
        if (res == UXML_END_ITEM)
          {
            set_error (buf, L"Unexpected character: /");
            res = UXML_LOADERROR;
          }
      }

    if (buf)
        dispose_uxml_buffer (buf);

    return res;
}

static char *
make_long_filename (const char *path,
                    const char *filename)
{
    char *
        fullpath = NULL;

    fullpath = file_where ('r', path, filename, "xml");
    if (fullpath)
        return mem_strdup (fullpath);
    else
	return NULL;
}

static UXML_BUFFER *
init_uxml_buffer_from_file (const char * name)
{
    UFILE
            *file;
    UXML_BUFFER
            *res = NULL;

    ASSERT (name);
    file = unicode_open(name, 'r');
    if (file)
      {
        res = (UXML_BUFFER *) mem_alloc (sizeof(UXML_BUFFER));
        if (res)
          {
            res-> fd = file;
            res-> cur = 0;
            res-> pageln = 0;
            res-> page = NULL;
            res-> state = BUF_READY;
            res-> cur_line[0] = (UCODE)'\n';
            res-> line_nbr = 0;
            res-> char_nbr = 1;
	        res-> virtual_char_nbr = 0;
            res-> fname = NULL;
	        res-> tab_width_left = 0;
            init_charmaps (res->cmap);

            buf_load_page(res);
            UXML_BUF_INVARIANT(res);
          }
      }
    return res;
}

static UXML_BUFFER *
init_uxml_buffer_from_string (const UCODE *uxml_string)
{
    UXML_BUFFER
            *res = NULL;

    ASSERT (uxml_string);

    res = (UXML_BUFFER *) mem_alloc (sizeof(UXML_BUFFER));
    if (res)
      {
        res-> fd = NULL;
        res-> page = mem_ustrdup(uxml_string);
        res-> pageln = wcslen(uxml_string);
        res-> cur = 0;
        res-> state = BUF_READY;
        res-> cur_line[0] = (UCODE)'\n';
        res-> line_nbr = 0;
        res-> char_nbr = 1;
        res-> virtual_char_nbr = 0;
        res-> fname = NULL;
        res-> tab_width_left = 0;
        init_charmaps (res-> cmap);

        UXML_BUF_INVARIANT (res);
      }

    return res;
}



static void
dispose_uxml_buffer (UXML_BUFFER * buf)
{
    ASSERT (buf);

    if (buf-> fd)
      {
        unicode_close (buf-> fd);
        buf-> fd = NULL;
      }
    if (buf-> page)
        mem_free (buf-> page);
    buf-> page = NULL;

    strfree (&buf-> fname);

    mem_free (buf);
}

/*  ----------------------------------------------------------------------
 *  make buf to point on the next occurence of UCODE c in buffer
 *  returns UXML_NOERROR on success;
 *          UXML_LOADERROR if UCODE c was not found
 *          UXML_FILEERROR
 *  if NonWhiteSkipped, on exit, *NonWhiteSkipped is TRUE if at least one
 *  non white character has been lost because of seek.
 *  ---------------------------------------------------------------------- */
static int
buf_seek_next (UXML_BUFFER *buf,
               UCODE        wanted,
               Bool       *non_white_skipped)
{
    UCODE
            ch    = (UCODE)'\0';

    ASSERT (buf);
    UXML_BUF_INVARIANT(buf);

    if (buf-> state != BUF_READY)
	return buf-> state;

    if (non_white_skipped)
        *non_white_skipped = FALSE;

    while ((ch = buf_get_current (buf)) && ch != wanted)
      {
        if (non_white_skipped
        && !(*non_white_skipped)
        && ! is_space (buf_get_current (buf)))
          {
            set_error (buf, L"Unexpected character: %c", buf_get_current (buf));
            *non_white_skipped = TRUE;
          }

	buf_next (buf);
	if (buf-> state != BUF_READY)
	    break;
      }

    if (buf-> state != BUF_READY)
	return buf-> state;

    if (ch == wanted)
	return BUF_READY;

    /*  In theory this code is unreachable, but just in case... */
    buf-> state = BUF_UNEXPECTED_ERROR;
    set_error (buf, L"Something strange happened in buf_seek_next");
    return buf->state;
}

static int
buf_load_page (UXML_BUFFER * buf)
{
    ASSERT (buf);

    if (buf-> state != BUF_READY)
        return buf-> state;

    if (buf-> fd)
      {
        if (feof(buf-> fd-> file))
            buf-> state = BUF_END;
        else
          {
            if (! buf-> page)
              {
                /* it's the first load. We allocate a memory page */
                if (!(buf-> page = (UCODE*) unicode_alloc (MAX_BUF_SIZE)))
                  {
                    buf-> state = BUF_UNEXPECTED_ERROR;
                    return BUF_UNEXPECTED_ERROR;
                  }
              }
            buf-> pageln = unicode_read(buf-> page, MAX_BUF_SIZE, buf-> fd);

            if (ferror(buf-> fd-> file))
                buf-> state = BUF_FILEERROR;
            else
              {
                ASSERT (!feof(buf-> fd-> file) ? buf-> pageln == MAX_BUF_SIZE : TRUE);
                buf-> cur = 0;
              }
          }
      }
    else
         buf-> state = BUF_END;

    buf_update_cur_line(buf);                     /* XXX: Is this correct?   */

    return buf-> state;
}

/*-----------------28/07/00 2:25--------------------
 * loads an xml item from buffer and attachs it to Item.
 * assumes that buffer currently points to '<'
 * returns UXML_NOERROR on success
 *         UXML_FILEERROR on io error
 *         UXML_LOADERROR on xml syntax error
 * on success, buffer is currently pointing to the next non white UCODE after the end of the previously loaded item
 * --------------------------------------------------*/
static int
uxml_load_item (UXML_ITEM  *item, UXML_BUFFER *buf)
{
    UCODE
            c = 0,
           *comment,
           *blanks,
           *item_name     = NULL,
           *end_item_name = NULL;
    unsigned int
            size;
    Bool
            found;
    UXML_ITEM
           *new_item;
    int
            res = UXML_NOERROR;

    ASSERT (item);
    ASSERT (buf);
    UXML_BUF_INVARIANT(buf);

    buf_seek_next_nonwhite(buf);
    CHECK_BUF_STATE;
    if (res == UXML_NOERROR)
      {
        c = buf_get_current(buf);
        if (c == (UCODE)'/')
          {
            buf_next(buf);
            return UXML_END_ITEM;
          }
        if (c == (UCODE)'!')
          {
            /*  This is either a comment or a DOCTYPE declaration.           */
            buf_get_next (buf, c);
            if (c == (UCODE)'-') {
                buf_get_next (buf, c);
                if (c != (UCODE)'-') {
                    set_error (buf, L"'--' expected");
                    return UXML_LOADERROR;
                }

                /*  It is a comment. Skip everything until the next occurence 
                 *  of '-->'
                 */
                found = FALSE;
                while (!found)
                  {
                    buf_next(buf);
                    CHECK_BUF_STATE_AND_BREAK_ON_ERROR(3); /* '-->' expected */
                    comment = buf_get_until_gt(buf);
                    CHECK_BUF_STATE_AND_BREAK_ON_ERROR(3);
                    ASSERT (comment);
                    size = wcslen(comment);
                    found = ((size >= 2)
                        && (comment[size-1] == (UCODE)'-')
                        && (comment[size-2] == (UCODE)'-')) ? TRUE : FALSE;
                    mem_free (comment);
                  }
                if (found)
                    buf_next(buf);
            }
            else if (c == (UCODE)'D') {
                item_name = buf_get_name (buf);
                if (!item_name || !wcscmp (item_name, L"OCTYPE")) {
                    set_error (buf, L"'--' or DOCTYPE declaration expected");
                    return UXML_LOADERROR;
                }
                mem_free (item_name);

                /*  It is a DOCTYPE declaration. Skip everything until the
                 *  next '>'. 
                 */
                buf_seek_next (buf, (UCODE)'>', NULL);
                CHECK_BUF_STATE;
                if (res != UXML_NOERROR) {
                    set_error (buf, L"'>' expected");
                    return res;
                }
                buf_next (buf);
            }
            else {
                set_error (buf, L"'--' or DOCTYPE declaration expected");
                return UXML_LOADERROR;
            }
            return res;
          }
        if (c == (UCODE)'?')
          {
            /*  processing instruction. We skip it. syntax is: <? blahblah ?>*/
            /*  --> we search next '?' until next UCODE after '?' is '>'      */
            buf_next(buf);
            do
              {
                res = buf_seek_next (buf, (UCODE)'?', NULL);
                CHECK_BUF_STATE_AND_BREAK_ON_ERROR(1);  /* ? expected */
                buf_get_next(buf, c);
                CHECK_BUF_STATE_AND_BREAK_ON_ERROR(4);  /* > expected */
                buf_next(buf)
              } while (c != (UCODE)'>');

            return res;
          }

        if (! is_name_open(c))
          {
            set_error (buf, L"item name should begin with an alphanumeric char");
            return UXML_LOADERROR;
          }
      }

    /*  here, we're sure we are trying to actually load a new item           */
    ASSERT (is_name_open(c));
    ASSERT (res == UXML_NOERROR);

    item_name = buf_get_name (buf);
    if (! item_name)
        return UXML_LOADERROR;

    /*  we create an item and we'll set the name later                       */
    new_item = uxml_create_no_dup(item_name, NULL);
    if (!new_item)
        return UXML_FILEERROR;           /*  memory exhausted ?               */

    /*  so far, we've create the item, with its name.                        */
    /*  !!! if an error occurs, we'll have to free the item !!!              */
    /*  We now manage its attributes                                         */
    /*  buf is currently pointing to the next UCODE after the item name       */
    while (res == UXML_NOERROR)
      {
        buf_seek_next_nonwhite(buf);
        CHECK_BUF_STATE_AND_BREAK_ON_ERROR(7);

        c = buf_get_current(buf);
        ASSERT (c);
        if ((c == (UCODE)'/') || (c == (UCODE)'>'))
            break;                      /*  we're done with item attributes  */

        /*  we have an attribute. We try to load it.                         */
        res = uxml_load_attribute (new_item, buf);
        if (res != UXML_NOERROR)
            break;
      }

    if (res == UXML_NOERROR)
      {
        ASSERT (c == buf_get_current(buf));
        ASSERT ((c == (UCODE)'/') || (c == (UCODE)'>'));
        if (c == (UCODE)'/')
          {
            /*  CASE 1. : <item attributes... />                             */
            /*  the item has no child item nor value. We check for "/>"      */
            buf_next(buf);
            buf_seek_next_nonwhite(buf);
            CHECK_BUF_STATE;
            if (res == UXML_NOERROR)
              {
                if (buf_get_current(buf) == (UCODE)'>')
                    buf_next(buf)
                else
                  {
                    set_error (buf, L"Unexpected token at end of item: %c", buf_get_current(buf));
                    res = UXML_LOADERROR;
                  }
              }
          }
        else
          {
            /*  CASE 2. : <item attributes...> child and/or values </item>   */
            /*  we now look for closing item tag, ie </item>                 */
            buf_next(buf);
            FOREVER
              {
                /*  we handle children items/values while closing tag is not found */
                blanks = buf_get_white_char(buf);
                CHECK_BUF_STATE_AND_BREAK_ON_ERROR(7);
                c = buf_get_current(buf);
                if (c == (UCODE)'<')
                  {
                    mem_free (blanks);  /* blanks are unsignificant          */
                    buf_next(buf);
                    CHECK_BUF_STATE_AND_BREAK_ON_ERROR(5);
                    res = uxml_load_item (new_item, buf);
                    if (res != UXML_NOERROR)
                      {
                        if (res == UXML_END_ITEM)
                          {
                            /*  not an error. end tag has been found.        */
                            end_item_name = buf_get_name (buf);

			    if (wcscmp (item_name, end_item_name) == 0)
			      {
				res = UXML_NOERROR;

			        /* Skip optional white space at end of name */
			        /* Then check for '>" at end of end tag.    */
                                if (buf_seek_next_nonwhite(buf) == BUF_READY)
			          {
				    if (buf_get_current(buf) == (UCODE)'>')
				      {
                                        buf_next(buf);
				      }
				    else
				      {
				        res = UXML_LOADERROR;
				        set_error (buf, L"Expected '>' to close end tag, got '%c'", buf_get_current(buf));
				      }
			          }
			        else
			          {
				    res = UXML_LOADERROR;
                                    set_error (buf, L"Expected '>' to close end tag, ran out of data");
			          }
			      }
			    else
			      {
                                res = UXML_LOADERROR;
                                set_error (buf, L"Incorrect closing tag name: %s (expected %s)", end_item_name, item_name);
                              }
                            mem_free (end_item_name);
                          }
                        break;
                      }
                  }
                else
                  {
                    /* we have found an item value                           */
                    res = uxml_load_item_value (new_item, buf, blanks);
                    mem_free (blanks);
                  }
              }
          }
      }

    if (res == UXML_NOERROR)
        uxml_attach_child (item, new_item);
    else
      {
        /*  we mem_free allocated items                                      */
        if (new_item)
            uxml_free (new_item);
      }

    return res;
}

/*  XXX: This routine _MUST_ use the macros to walk through the buffer
 *  in order for our tab expansion to work.  It has been hacked to do
 *  so, but this probably means at least some of the code is now
 *  redundant, so this needs to be optimised.
 */
static int
buf_seek_next_nonwhite (UXML_BUFFER * buf)
{
    UCODE
        ch = (UCODE)'\0';

    ASSERT (buf);
    UXML_BUF_INVARIANT(buf);

    if (buf-> state != BUF_READY)
	return buf-> state;

    while ((ch = buf_get_current (buf)) && is_space (ch))
      {
	buf_next (buf);
	if (buf-> state != BUF_READY)
	    break;
      }

    if (buf-> state != BUF_READY)
	return buf-> state;

    if (! is_space (ch))
	return BUF_READY;

    /*  In theory this code is unreachable, but just in case... */
    buf-> state = BUF_UNEXPECTED_ERROR;
    set_error (buf, L"Something strange happened in buf_seek_next_nonwhite");
    return buf->state;
}

#define STRING_BUF_SIZE             (64)
#define APPEND(_c_)                                                           \
  {                                                                           \
    ASSERT (res_size <= res_alloc_size);                                      \
    if (res_size >= res_alloc_size)                                           \
      {                                                                       \
        /* res is, or could become, full (or NULL) --> we increase its size*/ \
        if (!res_alloc_size)                                                  \
          {                                                                   \
            res_alloc_size = STRING_BUF_SIZE;                                 \
            res = (UCODE*) unicode_alloc (res_alloc_size);                         \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            res_alloc_size += STRING_BUF_SIZE;                                \
            res = (UCODE*) mem_realloc (res, res_alloc_size * UCODE_SIZE);    \
          }                                                                   \
        if (!res)                                                             \
            return NULL;     /* reallocation failed. NULL is returned */      \
      }                                                                       \
    res[res_size++] = (_c_);                                                  \
  }

#define BUF_GET_UNTIL(buf)                                                    \
    UCODE * res = NULL;                                                        \
    unsigned int res_alloc_size = 0;                                          \
    unsigned int res_size = 0;                                                \
    UCODE metachar[METACHAR_SIZE];                                                         \
    int metasize;                                                             \
    int idx;                                                                  \
    UCODE c;                                                                   \
                                                                              \
    /* if meta char must be escaped, & can not be a terminator, */            \
    /* otherwise mechanism can be wrong (?) */                                \
    ASSERT (_ESCAPEMETA_ ? !TERMINATOR('&') : TRUE);                          \
    ASSERT (buf);                                                             \
    c = buf_get_current(buf);                                                 \
    FOREVER                                                                   \
      {                                                                       \
        if (_ESCAPEMETA_ && (c == (UCODE)'&'))                                \
          {                                                                   \
            /* perhaps a metachar. We handle the case and continue. */        \
            metasize=0;                                                       \
            buf_get_next(buf, c);                                             \
            while ((metasize < METACHAR_SIZE) && c && (iswalnum(c) || c == (UCODE)'#'))    \
              {                                                               \
                metachar[metasize++] = c;                                     \
                buf_get_next(buf, c);                                         \
              }                                                               \
            if (c == (UCODE)';')                                              \
              {                                                               \
                metachar [metasize] = (UCODE)';';                                      \
                c = decode_meta_ucharn (metachar, metasize);                  \
                if (c)                                                        \
                    APPEND (c)                                                \
                else                                                          \
                  {                                                           \
                    APPEND ((UCODE)'&');                                      \
                    for (idx=0; idx<metasize; idx++) {                        \
                        APPEND (metachar[idx]);                               \
                    }                                                         \
                    APPEND ((UCODE)';');                                      \
                  }                                                           \
                buf_get_next(buf, c);                                         \
              }                                                               \
            else                                                              \
              {                                                               \
                APPEND ((UCODE)'&');                                                 \
                for (idx=0; idx<metasize; idx++)                              \
                    APPEND (metachar [idx])                                    \
              }                                                               \
            continue;                                                         \
          }                                                                   \
                                                                              \
        if (TERMINATOR(c))                                                    \
          {                                                                   \
            /* we've found what we search and exit the loop */                \
            APPEND (0);                                                       \
            break;                                                            \
          }                                                                   \
                                                                              \
        if (buf-> state != BUF_READY)                                         \
          {                                                                   \
            /* an error occured or end of the file was reached. */            \
	    /* We exit the loop */                                            \
            ASSERT (!c);                                                      \
            if (res)                                                          \
              {                                                               \
                mem_free (res);                                               \
                res = NULL;                                                   \
                res_size = 0;                                                 \
              }                                                               \
            break;                                                            \
          }                                                                   \
                                                                              \
        /* we add char and read next */                                       \
        APPEND (c);                                                           \
        buf_get_next(buf, c);                                                 \
      }                                                                       \
                                                                              \
    ASSERT ((res != NULL) ? (wcslen(res)==(res_size-1)) : (res_size == 0));   \
                                                                              \
    return res;                                                               \


static UCODE *
buf_get_name(UXML_BUFFER * buf)
{
    #define _ESCAPEMETA_                    (FALSE)
    #define TERMINATOR(_C_)                 (! is_name (_C_))
    BUF_GET_UNTIL(buf);
    #undef _ESCAPEMETA_
    #undef TERMINATOR
}

static UCODE *
buf_get_string(UXML_BUFFER * buf)
{
    #define _ESCAPEMETA_                    (TRUE)
    #define TERMINATOR(_C_)                 (_C_ == (UCODE)'"')
    BUF_GET_UNTIL(buf)
    #undef _ESCAPEMETA_
    #undef TERMINATOR
}

static UCODE *
buf_get_item_value(UXML_BUFFER * buf)
{
    #define _ESCAPEMETA_                    (TRUE)
    #define TERMINATOR(_C_)                 (_C_ == (UCODE)'<')
    BUF_GET_UNTIL(buf)
    #undef _ESCAPEMETA_
    #undef TERMINATOR
}

static UCODE *
buf_get_white_char(UXML_BUFFER * buf)
{
    #define _ESCAPEMETA_                    (FALSE)
    #define TERMINATOR(_C_)                 (! wcschr(L" \t\r\n", c))
    BUF_GET_UNTIL(buf)
    #undef _ESCAPEMETA_
    #undef TERMINATOR
}

static UCODE *
buf_get_until_gt(UXML_BUFFER * buf)
{
    #define _ESCAPEMETA_                    (FALSE)
    #define TERMINATOR(_C_)                 (_C_ == (UCODE)'>')
    BUF_GET_UNTIL(buf)
    #undef _ESCAPEMETA_
    #undef TERMINATOR
}

static int
uxml_load_attribute (UXML_ITEM * item, UXML_BUFFER * buf)
{
    UCODE * attr_name;
    UCODE * attr_value;
    UCODE c;

    ASSERT (buf);
    UXML_BUF_INVARIANT(buf);
    if (! is_name_open (buf_get_current(buf)))
      {
        set_error (buf, L"Unrecognized token : %c", buf_get_current(buf));
        return UXML_LOADERROR;
      }

    attr_name = buf_get_name(buf);

    if (!attr_name)
        return UXML_LOADERROR;
    ASSERT (wcslen(attr_name) > 0);

    /*  we expect equal sign. buf points to a non alphanumeric UCODE          */
    buf_seek_next_nonwhite (buf);
    c = buf_get_current(buf);
    if (c != '=')
      {
        mem_free (attr_name);
        set_error (buf, L"Unexpected token loading attribute : %c", 
		        buf_get_current(buf));
        return UXML_LOADERROR;
      }

    /*  now, we expect a double-quoted string                                */
    buf_next(buf);
    buf_seek_next_nonwhite (buf);
    c = buf_get_current(buf);
    if (c != (UCODE)'\"')
      {
        set_error (buf, L"Invalid literal opening quote: %c", c);
        mem_free (attr_name);
        return UXML_LOADERROR;
      }

    buf_next(buf);
    attr_value = buf_get_string(buf);
    if (!attr_value)
      {
        set_error (buf, L"File end inside literal.");
        mem_free (attr_name);
        return UXML_LOADERROR;
      }

    /*  we've found attribute name and value                                 */
    ASSERT (attr_name);
    ASSERT (attr_value);
    uxml_put_attr_no_dup (item, attr_name, attr_value);

    buf_next(buf);

    return UXML_NOERROR;
}

static int
uxml_load_item_value (UXML_ITEM   *item,
                     UXML_BUFFER *buf,
                     UCODE       *blanks)
{
    UCODE * value;
    UCODE * str = NULL;

    ASSERT (buf);
    ASSERT (blanks);

    value = buf_get_item_value(buf);
    if (!value)
        return UXML_LOADERROR;

    ASSERT (value);
    str = (UCODE*) unicode_alloc (wcslen(blanks) +  wcslen(value) + 2);
    wcscpy (str, blanks);
    wcscat (str, value);

    /*  an item value has been found --> we create a new xml item and attach it to item */
    uxml_new_no_dup (item, NULL, str);
    mem_free (value);

    return UXML_NOERROR;
}

static void
set_error (UXML_BUFFER *buf,
           UCODE       *format, ...)
{
    UCODE
        *mess_ptr;
    int
        offset;
    va_list
        argptr;

    mess_ptr = last_error;
    mess_ptr[0] = 0;
    if (buf && (buf-> line_nbr >= 0))
      {
        ASSERT (buf-> char_nbr >= 0);
        buf-> cur_line [buf-> char_nbr] = 0;
        if (buf-> fname)
          {
            offset = swprintf (mess_ptr, L"%s ", buf-> fname);
            mess_ptr += offset;
          }
        offset = swprintf (mess_ptr, L"%d: %s\n",
                          buf-> line_nbr,
                          buf-> cur_line);
        mess_ptr += offset;
      }

    va_start (argptr, format);          /*  Start variable arguments list    */
    offset = vswprintf (mess_ptr, format, argptr);
    va_end   (argptr);                  /*  End variable arguments list      */
    mess_ptr += offset;
}





