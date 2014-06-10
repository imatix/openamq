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
#include "sflunic.h"                    /*  Unicode functions                */
#include "sflusymb.h"                   /*  Unicode Symbol functions         */
#include "sflsymb.h"                    /*  Symbol functions                 */
#include "sflmem.h"                     /*  Memory functions                 */
#include "sflhttp.h"                    /*  non unicode HTTP                 */
#include "sflstr.h"                     /*  String functions                 */
#include "sflfind.h"                    /*  Find text functions              */
#include "sflfile.h"                    /*  Files functions                  */

#include "sfluhttp.h"                   /*  Prototypes for functions         */

/*- Local function declaration ----------------------------------------------*/

static int  decode_uhex             (const UCODE   **input, size_t outmax);
static void multipart_decode_header (char *header, USYMTAB *table, qbyte codepage);

/*  ---------------------------------------------------------------------[<]-
    Function: http_encode_umeta

    Synopsis: Translates special characters into HTML/SGML metacharacters.
    The input buffer is not modified; you supply an output buffer and specify
    the maximum size of this buffer.  The input buffer must end in a null.
    Aftern calling, the input pointer is set to the character after the last
    encoded character.  Returns the final size of the translated data
    excluding the final null byte.  If the resulting output data would be
    too long, translations stops.  If html is TRUE then the metacharacters
    amp, lt, gt and quot are not translated - this allows you to encode
    markup characters within HTML; otherwise they are translated and the
    output doesn't look like HTML.
    ---------------------------------------------------------------------[>]-*/

size_t
http_encode_umeta (
    UCODE    *output,
    UCODE    **input,
    size_t  outmax,
    Bool    html)
{
    size_t
        space_left,                     /*  Space left in destination        */
        length;
    UCODE
        *dest;                          /*  Pointer to result string         */

    ASSERT (input);
    ASSERT (*input);
    ASSERT (output);

    if (outmax == 0)                    /*  Special case for zero space      */
        return (0);

    space_left = outmax - 1;            /*  Allow for final null byte        */
    dest = output;
    while (**input && space_left > 0)
      {
        length = encode_meta_uchar (dest, **input, space_left, html, FALSE);
        if (length)
          {
            space_left -= length;
            dest += length;
            (*input) ++;
          }
        else
            break;
      }
    *dest = 0;
    return ((size_t) (dest - output));
}


/*  ---------------------------------------------------------------------[<]-
    Function: http_encode_umeta_ascii

    Synopsis: Translates special characters into HTML/SGML metacharacters.
    The input buffer is not modified; you supply an output buffer and specify
    the maximum size of this buffer.  The input buffer must end in a null.
    Aftern calling, the input pointer is set to the character after the last
    encoded character.  Returns the final size of the translated data
    excluding the final null byte.  If the resulting output data would be
    too long, translations stops.  If html is TRUE then the metacharacters
    amp, lt, gt and quot are not translated - this allows you to encode
    markup characters within HTML; otherwise they are translated and the
    output doesn't look like HTML.
    This version translate only ascii character value, Don't encode value
    greater than 255.
    ---------------------------------------------------------------------[>]-*/

size_t
http_encode_umeta_ascii (
    UCODE    *output,
    UCODE    **input,
    size_t  outmax,
    Bool    html)
{
    size_t
        space_left,                     /*  Space left in destination        */
        length;
    UCODE
        *dest;                          /*  Pointer to result string         */

    ASSERT (input);
    ASSERT (*input);
    ASSERT (output);

    if (outmax == 0)                    /*  Special case for zero space      */
        return (0);

    space_left = outmax - 1;            /*  Allow for final null byte        */
    dest = output;
    while (**input && space_left > 0)
      {
        length = encode_meta_uchar (dest, **input, space_left, html, TRUE);
        if (length)
          {
            space_left -= length;
            dest += length;
            (*input) ++;
          }
        else
            break;
      }
    *dest = 0;
    return ((size_t) (dest - output));
}

/*  ---------------------------------------------------------------------[<]-
    Function: encode_meta_uchar

    Synopsis: Translates one character into HTML/SGML metacharacters.  You
    supply an output buffer and specify the maximum size of this buffer.
    Returns the final size of the translated data excluding the final null
    byte.  If the resulting data is too long, translation does not occur
    and the returned value is zero.  If html is TRUE then the metacharacters
    cr, amp, lt, gt and quot are not translated - this allows you to encode
    accented characters within HTML; otherwise they are translated and the
    output doesn't look like HTML.
    ---------------------------------------------------------------------[>]-*/

size_t
encode_meta_uchar (
    UCODE  *output,
    UCODE   code,
    size_t outmax,
    Bool   html,
    Bool   only_ascii)
{
    static UCODE
        *meta [256];                    /*  Metacharacter translation table  */
    static Bool
        first_time = TRUE;              /*  First time flag                  */
    size_t
        length;                         /*  Length of translation            */
    UCODE
        *meta_char,                     /*  Pointer through metachar string  */
        buffer [10];                    /*  Buffer for conversions           */
    char
        ascii_char;                     /*  ASCII value of unicode code      */

    /*  Initialise translation table first time through                      */
    if (first_time)
      {
        first_time = FALSE;
        memset (meta, 0, sizeof (meta));

#if (defined (__UNIX__) || defined (__WINDOWS__))
        /*  UNIX and Windows generally use ISO-8859-1 (Latin-1)              */
        meta [0x91] = L"lsquo";
        meta [0x92] = L"rsquo";
        meta [0xA1] = L"iexcl";
        meta [0xA2] = L"cent";
        meta [0xA3] = L"pound";
        meta [0xA4] = L"curren";
        meta [0xA5] = L"yen";
        meta [0xA6] = L"brvbar";
        meta [0xA7] = L"sect";
        meta [0xA8] = L"uml";
        meta [0xA9] = L"copy";
        meta [0xAA] = L"ordf";
        meta [0xAB] = L"laquo";
        meta [0xAC] = L"not";
        meta [0xAD] = L"shy";
        meta [0xAE] = L"reg";
        meta [0xAF] = L"macr";
        meta [0xB0] = L"deg";
        meta [0xB1] = L"plusmn";
        meta [0xB2] = L"sup2";
        meta [0xB3] = L"sup3";
        meta [0xB4] = L"acute";
        meta [0xB5] = L"micro";
        meta [0xB6] = L"para";
        meta [0xB7] = L"middot";
        meta [0xB8] = L"cedil";
        meta [0xB9] = L"sup1";
        meta [0xBA] = L"ordm";
        meta [0xBB] = L"raquo";
        meta [0xBC] = L"frac14";
        meta [0xBD] = L"frac12";
        meta [0xBE] = L"frac34";
        meta [0xBF] = L"iquest";
        meta [0xC0] = L"Agrave";
        meta [0xC1] = L"Aacute";
        meta [0xC2] = L"Acirc";
        meta [0xC3] = L"Atilde";
        meta [0xC4] = L"Auml";
        meta [0xC5] = L"Aring";
        meta [0xC6] = L"AElig";
        meta [0xC7] = L"Ccedil";
        meta [0xC8] = L"Egrave";
        meta [0xC9] = L"Eacute";
        meta [0xCA] = L"Ecirc";
        meta [0xCB] = L"Euml";
        meta [0xCC] = L"Igrave";
        meta [0xCD] = L"Iacute";
        meta [0xCE] = L"Icirc";
        meta [0xCF] = L"Iuml";
        meta [0xD0] = L"ETH";
        meta [0xD1] = L"Ntilde";
        meta [0xD2] = L"Ograve";
        meta [0xD3] = L"Oacute";
        meta [0xD4] = L"Ocirc";
        meta [0xD5] = L"Otilde";
        meta [0xD6] = L"Ouml";
        meta [0xD7] = L"times";
        meta [0xD8] = L"Oslash";
        meta [0xD9] = L"Ugrave";
        meta [0xDA] = L"Uacute";
        meta [0xDB] = L"Ucirc";
        meta [0xDC] = L"Uuml";
        meta [0xDD] = L"Yacute";
        meta [0xDE] = L"THORN";
        meta [0xDF] = L"szlig";
        meta [0xE0] = L"agrave";
        meta [0xE1] = L"aacute";
        meta [0xE2] = L"acirc";
        meta [0xE3] = L"atilde";
        meta [0xE4] = L"auml";
        meta [0xE5] = L"aring";
        meta [0xE6] = L"aelig";
        meta [0xE7] = L"ccedil";
        meta [0xE8] = L"egrave";
        meta [0xE9] = L"eacute";
        meta [0xEA] = L"ecirc";
        meta [0xEB] = L"euml";
        meta [0xEC] = L"igrave";
        meta [0xED] = L"iacute";
        meta [0xEE] = L"icirc";
        meta [0xEF] = L"iuml";
        meta [0xF0] = L"eth";
        meta [0xF1] = L"ntilde";
        meta [0xF2] = L"ograve";
        meta [0xF3] = L"oacute";
        meta [0xF4] = L"ocirc";
        meta [0xF5] = L"otilde";
        meta [0xF6] = L"ouml";
        meta [0xF7] = L"divide";
        meta [0xF8] = L"oslash";
        meta [0xF9] = L"ugrave";
        meta [0xFA] = L"uacute";
        meta [0xFB] = L"ucirc";
        meta [0xFC] = L"uuml";
        meta [0xFD] = L"yacute";
        meta [0xFE] = L"thorn";
        meta [0xFF] = L"yuml";

#elif (defined (__MSDOS__))
        /*  DOS generally uses the PC-1 alphabet                             */
        meta [0x80] = L"Ccedil";
        meta [0x81] = L"uuml";
        meta [0x82] = L"eacute";
        meta [0x83] = L"acirc";
        meta [0x84] = L"auml";
        meta [0x85] = L"agrave";
        meta [0x86] = L"aring";
        meta [0x87] = L"ccedil";
        meta [0x88] = L"ecirc";
        meta [0x89] = L"euml";
        meta [0x8B] = L"iuml";
        meta [0x8C] = L"icirc";
        meta [0x8D] = L"igrave";
        meta [0x8E] = L"Auml";
        meta [0x8F] = L"Aring";
        meta [0x90] = L"Eacute";
        meta [0x91] = L"aelig";
        meta [0x92] = L"AElig";
        meta [0x93] = L"ocirc";
        meta [0x94] = L"ouml";
        meta [0x95] = L"ograve";
        meta [0x96] = L"ucirc";
        meta [0x97] = L"ugrave";
        meta [0x98] = L"yuml";
        meta [0x99] = L"Ouml";
        meta [0x9A] = L"Uuml";
        meta [0x9B] = L"oslash";
        meta [0x9C] = L"pound";
        meta [0x9D] = L"Oslash";
        meta [0x9E] = L"times";
        meta [0xA0] = L"aacute";
        meta [0xA1] = L"iacute";
        meta [0xA2] = L"otilde";
        meta [0xA3] = L"uacute";
        meta [0xA4] = L"ntilde";
        meta [0xA5] = L"Ntilde";
        meta [0xA6] = L"ordf";
        meta [0xA7] = L"ordm";
        meta [0xA8] = L"iquest";
        meta [0xA9] = L"reg";
        meta [0xAA] = L"not";
        meta [0xAB] = L"frac14";
        meta [0xAC] = L"frac12";
        meta [0xAD] = L"iexcl";
        meta [0xAE] = L"laquo";
        meta [0xAF] = L"raquo";
        meta [0xB0] = L"shy";
        meta [0xB5] = L"Aacute";
        meta [0xB6] = L"Acirc";
        meta [0xB7] = L"Agrave";
        meta [0xB8] = L"copy";
        meta [0xC6] = L"atilde";
        meta [0xC7] = L"Atilde";
        meta [0xCA] = L"egrave";
        meta [0xCF] = L"curren";
        meta [0xD1] = L"ETH";
        meta [0xD2] = L"Ecirc";
        meta [0xD3] = L"Euml";
        meta [0xD4] = L"Egrave";
        meta [0xD6] = L"Iacute";
        meta [0xD7] = L"Icirc";
        meta [0xD8] = L"Iuml";
        meta [0xDD] = L"brvbar";
        meta [0xDE] = L"Igrave";
        meta [0xE0] = L"Oacute";
        meta [0xE1] = L"szlig";
        meta [0xE2] = L"Ocirc";
        meta [0xE3] = L"Ograve";
        meta [0xE5] = L"Otilde";
        meta [0xE6] = L"THORN";
        meta [0xE9] = L"Uacute";
        meta [0xEA] = L"Ucirc";
        meta [0xEB] = L"Ugrave";
        meta [0xEC] = L"yacute";
        meta [0xED] = L"Yacute";
        meta [0xEE] = L"macr";
        meta [0xEF] = L"acute";
        meta [0xF1] = L"plusmn";
        meta [0xF3] = L"frac34";
        meta [0xF4] = L"para";
        meta [0xF5] = L"sect";
        meta [0xF6] = L"divide";
        meta [0xF8] = L"deg";
        meta [0xFA] = L"middot";
        meta [0xFB] = L"sup1";
        meta [0xFC] = L"sup3";
        meta [0xFD] = L"sup2";
#endif
      }
    if (code > 127)
      {
        if (only_ascii)
          {
            output [0] = code;
            return (1);
          }
        swprintf (buffer, L"&#%d;", code);
        length = wcslen (buffer);
        if (length < outmax)
          {
            wcsncpy (output, buffer, length);
            return length;
          }
      }
    wctomb (&ascii_char, code);
    meta_char = meta [(int) ascii_char & 255];
    if (meta_char == 0)
      {
        if (html)
          {
            output [0] = code;
            return (1);
          }
        else
          {
            switch (ascii_char)
              {
                case '&':
                    meta_char = L"amp";
                    break;
                case '<':
                    meta_char = L"lt";
                    break;
                case '>':
                    meta_char = L"gt";
                    break;
                case '"':
                    meta_char = L"quot";
                    break;
                default:
                    output [0] = code;
                    return (1);
              }
          }
      }
    swprintf (buffer, L"&%s;", meta_char);
    length = wcslen (buffer);
    if (length < outmax)
      {
        wcsncpy (output, buffer, length);
        return length;
      }
    else
      {
        output [0] = 0;
        return (0);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: http_decode_umeta

    Synopsis: Translates special characters from HTML/SGML
    metacharacters.  The input buffer is not modified; you supply an
    output buffer and specify the maximum size of this buffer.  The input
    buffer must end in a null.  The two buffers may be the same, since
    decoding reduces the size of the data.  Returns the final size of the
    translated data excluding the final null byte.  If the resulting data
    is too long, it is brutally truncated.  Invalid meta-characters are
    left as-is.  Normally the input pointer points to the final null;
    however if the input string runs out in the middle of a
    meta-character, the input pointer is left pointing at the start of
    that meta-character (the '&').  If the two buffers are the same, it
    may be the case that this character has been overwritten with the
    terminating null.
    ---------------------------------------------------------------------[>]-*/

size_t
http_decode_umeta (
    UCODE  *output,
    UCODE **input,
    size_t outmax)
{
    UCODE
        *in,
        *dest,                          /*  Pointer to result string         */
        *end,                           /*  Next character after meta-char   */
        code;                           /*  Decoded metachar string          */
    size_t
        space_left;                     /*  Space left in destination        */

    ASSERT (input);
    ASSERT (*input);
    ASSERT (output);

    if (outmax == 0)                    /*  Special case for zero space      */
        return (0);

    space_left = outmax - 1;            /*  Allow for final null byte        */
    dest = output;
    in = *input;
    for (; *in; in++)
      {
        if (*in == (UCODE)'&')
          {
            end = wcschr (in, (UCODE)';');
            if (end)
              {
                code = decode_meta_ucharn (in + 1, end - in - 1);
                if (code)
                    in = end;       /*  Skip past decoded metachar       */
                else
                    code = *in;     /*  Ignore the &, no valid metachar  */
              }
            else                        /*  The string ends before the ';'   */
            if (wcslen (in) > 10)
                code = *in;         /*  Ignore the &, no valid metachar  */
            else
                break;
          }
        else
            code = *in;

        if (space_left > 0)
          {
            *dest++ = code;
            space_left --;
          }
      }
    *dest = (UCODE)'\0';
    return ((size_t) (dest - output));
}


/*  ---------------------------------------------------------------------[<]-
    Function: decode_meta_ucharn

    Synopsis: Decodes a single meta-character (the piece from the character
    after the '&' up to but not including the ';'.  If the meta-character
    is valid, returns the character; otherwise returns zero.  Decodes both
    named and numeric meta-characters.  Use the macro decode_meta_char if
    the input has a terminating zero.
    ---------------------------------------------------------------------[>]-*/

UCODE
decode_meta_ucharn (const UCODE *input, size_t length)
{
#define META_COUNT 102

    static UCODE
        *meta [META_COUNT];             /*  Metacharacter translation tables */
    static byte
        code [META_COUNT];
    static Bool
        first_time = TRUE;              /*  First time flag                  */
    int
        char_index,                     /*  Index into translation table     */
        min,                            /*  Pointers for binary chop         */
        max,
        cmp,
        num;
    UCODE
        feedback;

    /*  Initialise translation table first time through                      */
    if (first_time)
      {
        first_time = FALSE;

        /*  The meta-characters must be in alphabetical order so we can use  */
        /*  a binary chop.                                                   */
#if (defined (__UNIX__) || defined (__WINDOWS__))
        /*  UNIX and Windows generally use ISO-8859-1 (Latin-1)              */
        code [  0] = 0xC6;  meta [  0] = L"AElig";
        code [  1] = 0xC1;  meta [  1] = L"Aacute";
        code [  2] = 0xC2;  meta [  2] = L"Acirc";
        code [  3] = 0xC0;  meta [  3] = L"Agrave";
        code [  4] = 0xC5;  meta [  4] = L"Aring";
        code [  5] = 0xC3;  meta [  5] = L"Atilde";
        code [  6] = 0xC4;  meta [  6] = L"Auml";
        code [  7] = 0xC7;  meta [  7] = L"Ccedil";
        code [  8] = 0xD0;  meta [  8] = L"ETH";
        code [  9] = 0xC9;  meta [  9] = L"Eacute";
        code [ 10] = 0xCA;  meta [ 10] = L"Ecirc";
        code [ 11] = 0xC8;  meta [ 11] = L"Egrave";
        code [ 12] = 0xCB;  meta [ 12] = L"Euml";
        code [ 13] = 0xCD;  meta [ 13] = L"Iacute";
        code [ 14] = 0xCE;  meta [ 14] = L"Icirc";
        code [ 15] = 0xCC;  meta [ 15] = L"Igrave";
        code [ 16] = 0xCF;  meta [ 16] = L"Iuml";
        code [ 17] = 0xD1;  meta [ 17] = L"Ntilde";
        code [ 18] = 0xD3;  meta [ 18] = L"Oacute";
        code [ 19] = 0xD4;  meta [ 19] = L"Ocirc";
        code [ 20] = 0xD2;  meta [ 20] = L"Ograve";
        code [ 21] = 0xD8;  meta [ 21] = L"Oslash";
        code [ 22] = 0xD5;  meta [ 22] = L"Otilde";
        code [ 23] = 0xD6;  meta [ 23] = L"Ouml";
        code [ 24] = 0xDE;  meta [ 24] = L"THORN";
        code [ 25] = 0xDA;  meta [ 25] = L"Uacute";
        code [ 26] = 0xDB;  meta [ 26] = L"Ucirc";
        code [ 27] = 0xD9;  meta [ 27] = L"Ugrave";
        code [ 28] = 0xDC;  meta [ 28] = L"Uuml";
        code [ 29] = 0xDD;  meta [ 29] = L"Yacute";
        code [ 30] = 0xE1;  meta [ 30] = L"aacute";
        code [ 31] = 0xE2;  meta [ 31] = L"acirc";
        code [ 32] = 0xB4;  meta [ 32] = L"acute";
        code [ 33] = 0xE6;  meta [ 33] = L"aelig";
        code [ 34] = 0xE0;  meta [ 34] = L"agrave";
        code [ 35] = '&';   meta [ 35] = L"amp";
        code [ 36] = 0xE5;  meta [ 36] = L"aring";
        code [ 37] = 0xE3;  meta [ 37] = L"atilde";
        code [ 38] = 0xE4;  meta [ 38] = L"auml";
        code [ 39] = 0xA6;  meta [ 39] = L"brvbar";
        code [ 40] = 0xE7;  meta [ 40] = L"ccedil";
        code [ 41] = 0xB8;  meta [ 41] = L"cedil";
        code [ 42] = 0xA2;  meta [ 42] = L"cent";
        code [ 43] = 0xA9;  meta [ 43] = L"copy";
        code [ 44] = 0xA4;  meta [ 44] = L"curren";
        code [ 45] = 0xB0;  meta [ 45] = L"deg";
        code [ 46] = 0xF7;  meta [ 46] = L"divide";
        code [ 47] = 0xE9;  meta [ 47] = L"eacute";
        code [ 48] = 0xEA;  meta [ 48] = L"ecirc";
        code [ 49] = 0xE8;  meta [ 49] = L"egrave";
        code [ 50] = 0xF0;  meta [ 50] = L"eth";
        code [ 51] = 0xEB;  meta [ 51] = L"euml";
        code [ 52] = 0xBD;  meta [ 52] = L"frac12";
        code [ 53] = 0xBC;  meta [ 53] = L"frac14";
        code [ 54] = 0xBE;  meta [ 54] = L"frac34";
        code [ 55] = '>';   meta [ 55] = L"gt";
        code [ 56] = 0xED;  meta [ 56] = L"iacute";
        code [ 57] = 0xEE;  meta [ 57] = L"icirc";
        code [ 58] = 0xA1;  meta [ 58] = L"iexcl";
        code [ 59] = 0xEC;  meta [ 59] = L"igrave";
        code [ 60] = 0xBF;  meta [ 60] = L"iquest";
        code [ 61] = 0xEF;  meta [ 61] = L"iuml";
        code [ 62] = 0xAB;  meta [ 62] = L"laquo";
        code [ 63] = 0x91;  meta [ 63] = L"lsquo";
        code [ 64] = '<';   meta [ 64] = L"lt";
        code [ 65] = 0xAF;  meta [ 65] = L"macr";
        code [ 66] = 0xB5;  meta [ 66] = L"micro";
        code [ 67] = 0xB7;  meta [ 67] = L"middot";
        code [ 68] = ' ';   meta [ 68] = L"nbsp";
        code [ 69] = 0xAC;  meta [ 69] = L"not";
        code [ 70] = 0xF1;  meta [ 70] = L"ntilde";
        code [ 71] = 0xF3;  meta [ 71] = L"oacute";
        code [ 72] = 0xF4;  meta [ 72] = L"ocirc";
        code [ 73] = 0xF2;  meta [ 73] = L"ograve";
        code [ 74] = 0xAA;  meta [ 74] = L"ordf";
        code [ 75] = 0xBA;  meta [ 75] = L"ordm";
        code [ 76] = 0xF8;  meta [ 76] = L"oslash";
        code [ 77] = 0xF5;  meta [ 77] = L"otilde";
        code [ 78] = 0xF6;  meta [ 78] = L"ouml";
        code [ 79] = 0xB6;  meta [ 79] = L"para";
        code [ 80] = 0xB1;  meta [ 80] = L"plusmn";
        code [ 81] = 0xA3;  meta [ 81] = L"pound";
        code [ 82] = '"';   meta [ 82] = L"quot";
        code [ 83] = 0xBB;  meta [ 83] = L"raquo";
        code [ 84] = 0xAE;  meta [ 84] = L"reg";
        code [ 85] = 0x92;  meta [ 85] = L"rsquo";
        code [ 86] = 0xA7;  meta [ 86] = L"sect";
        code [ 87] = 0xAD;  meta [ 87] = L"shy";
        code [ 88] = 0xB9;  meta [ 88] = L"sup1";
        code [ 89] = 0xB2;  meta [ 89] = L"sup2";
        code [ 90] = 0xB3;  meta [ 90] = L"sup3";
        code [ 91] = 0xDF;  meta [ 91] = L"szlig";
        code [ 92] = 0xFE;  meta [ 92] = L"thorn";
        code [ 93] = 0xD7;  meta [ 93] = L"times";
        code [ 94] = 0xFA;  meta [ 94] = L"uacute";
        code [ 95] = 0xFB;  meta [ 95] = L"ucirc";
        code [ 96] = 0xF9;  meta [ 96] = L"ugrave";
        code [ 97] = 0xA8;  meta [ 97] = L"uml";
        code [ 98] = 0xFC;  meta [ 98] = L"uuml";
        code [ 99] = 0xFD;  meta [ 99] = L"yacute";
        code [100] = 0xA5;  meta [100] = L"yen";
        code [101] = 0xFF;  meta [101] = L"yuml";
#elif (defined (__MSDOS__))
        code [  0] = 0x92;  meta [  0] = L"AElig";
        code [  1] = 0xB5;  meta [  1] = L"Aacute";
        code [  2] = 0xB6;  meta [  2] = L"Acirc";
        code [  3] = 0xB7;  meta [  3] = L"Agrave";
        code [  4] = 0x8F;  meta [  4] = L"Aring";
        code [  5] = 0xC7;  meta [  5] = L"Atilde";
        code [  6] = 0x8E;  meta [  6] = L"Auml";
        code [  7] = 0x80;  meta [  7] = L"Ccedil";
        code [  8] = 0xD1;  meta [  8] = L"ETH";
        code [  9] = 0x90;  meta [  9] = L"Eacute";
        code [ 10] = 0xD2;  meta [ 10] = L"Ecirc";
        code [ 11] = 0xD4;  meta [ 11] = L"Egrave";
        code [ 12] = 0xD3;  meta [ 12] = L"Euml";
        code [ 13] = 0xD6;  meta [ 13] = L"Iacute";
        code [ 14] = 0xD7;  meta [ 14] = L"Icirc";
        code [ 15] = 0xDE;  meta [ 15] = L"Igrave";
        code [ 16] = 0xD8;  meta [ 16] = L"Iuml";
        code [ 17] = 0xA5;  meta [ 17] = L"Ntilde";
        code [ 18] = 0xE0;  meta [ 18] = L"Oacute";
        code [ 19] = 0xE2;  meta [ 19] = L"Ocirc";
        code [ 20] = 0xE3;  meta [ 20] = L"Ograve";
        code [ 21] = 0x9D;  meta [ 21] = L"Oslash";
        code [ 22] = 0xE5;  meta [ 22] = L"Otilde";
        code [ 23] = 0x99;  meta [ 23] = L"Ouml";
        code [ 24] = 0xE6;  meta [ 24] = L"THORN";
        code [ 25] = 0xE9;  meta [ 25] = L"Uacute";
        code [ 26] = 0xEA;  meta [ 26] = L"Ucirc";
        code [ 27] = 0xEB;  meta [ 27] = L"Ugrave";
        code [ 28] = 0x9A;  meta [ 28] = L"Uuml";
        code [ 29] = 0xED;  meta [ 29] = L"Yacute";
        code [ 30] = 0xA0;  meta [ 30] = L"aacute";
        code [ 31] = 0x83;  meta [ 31] = L"acirc";
        code [ 32] = 0xEF;  meta [ 32] = L"acute";
        code [ 33] = 0x91;  meta [ 33] = L"aelig";
        code [ 34] = 0x85;  meta [ 34] = L"agrave";
        code [ 35] = '&';   meta [ 35] = L"amp";
        code [ 36] = 0x86;  meta [ 36] = L"aring";
        code [ 37] = 0xC6;  meta [ 37] = L"atilde";
        code [ 38] = 0x84;  meta [ 38] = L"auml";
        code [ 39] = 0xDD;  meta [ 39] = L"brvbar";
        code [ 40] = 0x87;  meta [ 40] = L"ccedil";
        code [ 41] = 0xB8;  meta [ 41] = L"cedil";
        code [ 42] = 0xA2;  meta [ 42] = L"cent";
        code [ 43] = 0xB8;  meta [ 43] = L"copy";
        code [ 44] = 0xCF;  meta [ 44] = L"curren";
        code [ 45] = 0xF8;  meta [ 45] = L"deg";
        code [ 46] = 0xF6;  meta [ 46] = L"divide";
        code [ 47] = 0x82;  meta [ 47] = L"eacute";
        code [ 48] = 0x88;  meta [ 48] = L"ecirc";
        code [ 49] = 0xCA;  meta [ 49] = L"egrave";
        code [ 50] = 0xF0;  meta [ 50] = L"eth";
        code [ 51] = 0x89;  meta [ 51] = L"euml";
        code [ 52] = 0xAC;  meta [ 52] = L"frac12";
        code [ 53] = 0xAB;  meta [ 53] = L"frac14";
        code [ 54] = 0xF3;  meta [ 54] = L"frac34";
        code [ 55] = '>';   meta [ 55] = L"gt";
        code [ 56] = 0xA1;  meta [ 56] = L"iacute";
        code [ 57] = 0x8C;  meta [ 57] = L"icirc";
        code [ 58] = 0xAD;  meta [ 58] = L"iexcl";
        code [ 59] = 0x8D;  meta [ 59] = L"igrave";
        code [ 60] = 0xA8;  meta [ 60] = L"iquest";
        code [ 61] = 0x8B;  meta [ 61] = L"iuml";
        code [ 62] = 0xAE;  meta [ 62] = L"laquo";
        code [ 63] = 0x91;  meta [ 63] = L"lsquo";
        code [ 64] = '<';   meta [ 64] = L"lt";
        code [ 65] = 0xEE;  meta [ 65] = L"macr";
        code [ 66] = 0xB5;  meta [ 66] = L"micro";
        code [ 67] = 0xFA;  meta [ 67] = L"middot";
        code [ 68] = ' ';   meta [ 68] = L"nbsp";
        code [ 69] = 0xAA;  meta [ 69] = L"not";
        code [ 70] = 0xA4;  meta [ 70] = L"ntilde";
        code [ 71] = 0xF3;  meta [ 71] = L"oacute";
        code [ 72] = 0x93;  meta [ 72] = L"ocirc";
        code [ 73] = 0x95;  meta [ 73] = L"ograve";
        code [ 74] = 0xA6;  meta [ 74] = L"ordf";
        code [ 75] = 0xA7;  meta [ 75] = L"ordm";
        code [ 76] = 0x9B;  meta [ 76] = L"oslash";
        code [ 77] = 0xA2;  meta [ 77] = L"otilde";
        code [ 78] = 0x94;  meta [ 78] = L"ouml";
        code [ 79] = 0xF4;  meta [ 79] = L"para";
        code [ 80] = 0xF1;  meta [ 80] = L"plusmn";
        code [ 81] = 0x9C;  meta [ 81] = L"pound";
        code [ 82] = '"';   meta [ 82] = L"quot";
        code [ 83] = 0xAF;  meta [ 83] = L"raquo";
        code [ 84] = 0xA9;  meta [ 84] = L"reg";
        code [ 85] = 0x92;  meta [ 85] = L"rsquo";
        code [ 86] = 0xF5;  meta [ 86] = L"sect";
        code [ 87] = 0xB0;  meta [ 87] = L"shy";
        code [ 88] = 0xFB;  meta [ 88] = L"sup1";
        code [ 89] = 0xFD;  meta [ 89] = L"sup2";
        code [ 90] = 0xFC;  meta [ 90] = L"sup3";
        code [ 91] = 0xE1;  meta [ 91] = L"szlig";
        code [ 92] = 0xFE;  meta [ 92] = L"thorn";
        code [ 93] = 0x9E;  meta [ 93] = L"times";
        code [ 94] = 0xA3;  meta [ 94] = L"uacute";
        code [ 95] = 0x96;  meta [ 95] = L"ucirc";
        code [ 96] = 0x97;  meta [ 96] = L"ugrave";
        code [ 97] = 0xA8;  meta [ 97] = L"uml";
        code [ 98] = 0x81;  meta [ 98] = L"uuml";
        code [ 99] = 0xEC;  meta [ 99] = L"yacute";
        code [100] = 0xA5;  meta [100] = L"yen";
        code [101] = 0x98;  meta [101] = L"yuml";
#endif
      }

    if (*input == (UCODE)'#')    /*  Numeric translation  */
      {
        input++;
        num = 0;
        if (*input == (UCODE)'x')  /*  Hex  */
          {
            input++;
            num = decode_uhex (&input, 0);
            input++;
          }
        else
            FOREVER
              {
                if ((*input >= (UCODE)'0') && (*input <= (UCODE)'9'))
                    num = (num * 10) + *input - (UCODE)'0';
                else
                    break;

                input++;
              }

        if (*input != (UCODE)';')
            num = 0;

        return num;
      }
    else  /*  Lookup meta-character  */
      {
        min = 0;
        max = META_COUNT;
        while (max > min)
          {
            char_index = (max + min) / 2;
            cmp = wcsncmp (input, meta [char_index], length);
            if (cmp == 0)
              {
                mbtowc (&feedback, &code [char_index], 1);
                return feedback;
              }
            if (cmp > 0)
                min = char_index + 1;
            else
                max = char_index;
          }

        return 0;
      }
}

int    
http_multipart_umem  (const byte *buffer, const long buffer_size,
                      USYMTAB ** form_field, USYMTAB **files,
                      qbyte codepage)
{
    static char
        separator [80 + 1];
    static int
        sep_size;
    char
        *p_end,
        *char_filename,
        *sym_name,
        *tmp_name = NULL,
        *p_head,
        *p_data,
        *p_next;
    USYMTAB
        *header_tab,
        *head;
    USYMBOL
        *symbol;
    int
        nb_files = 0;
    UPLOADFILE
        *fupload = NULL;
    UCODE
        *value,
        *utmp_filename;

    if (form_field  == NULL
    ||  files       == NULL
    ||  buffer      == NULL
    ||  buffer_size == 0)
        return (0);

    header_tab = usym_create_table ();
    if (header_tab == NULL)
        return (0);

    if (*form_field)
        usym_delete_table (*form_field);
    if (*files)
        usym_delete_table (*files);

    head       = *form_field = usym_create_table ();
    *files     =               usym_create_table ();

    memset (separator, 0, sizeof (separator));
    separator [0] = 0x0D;
    separator [1] = 0x0A;

    p_next = strchr ((const char *)buffer, '\n');
    if (p_next)
      {
        *p_next++ = '\0';
        strcpy (&separator [2], (const char *)buffer);
      }
    strconvch (&separator [2] , '\r', '\0');
    strconvch (&separator [2] , '\n', '\0');
    sep_size  = strlen (separator);
    p_end = (char *)buffer + buffer_size;

    while (p_next && p_next < p_end)
      {
        usym_empty_table (header_tab);
        p_head = p_next;
        p_data = (char *) memfind ((byte *) p_head, p_end - p_head,
                          (byte *) "\r\n\r\n", 4, FALSE);
        if (p_data)
          {
            *p_data = '\0';
            p_data += 4;
          }
        if (p_head)
            multipart_decode_header (p_head, header_tab, codepage);

        p_next = (char *) memfind ((byte *) p_data, p_end - p_data,
                          (byte *) separator, sep_size, FALSE);
        if (p_next != NULL)
          {
            /*  The next attribute-value pair was found                      */
            *p_next = '\0';

            /* If file name                                                  */
            symbol = usym_lookup_symbol (header_tab, L"filename");
            if (symbol && p_next - p_data > 0)
              {
                char_filename = ucode2ascii (symbol-> value);
                if (char_filename)
                  {
                    tmp_name = strip_file_path (char_filename);
                    utmp_filename = ascii2ucode (tmp_name);
                    if (utmp_filename)
                      {
                        symbol = usym_assume_symbol (*files, utmp_filename, L"");
                        if (symbol)
                          {
                            sym_name = ucode2ascii (symbol-> name);
                            if (sym_name)
                              {
                                fupload = alloc_upload (sym_name, NULL, 
			                                            p_next - p_data, (byte *)p_data);
                                if (fupload)
                                  {
                                    symbol-> data = fupload;
                                    nb_files++;
                                  }
                                mem_free (sym_name);
                              }
                          }
                         p_data = tmp_name;
                         if ( (symbol = usym_lookup_symbol (header_tab, L"name")) != NULL )
                              usym_assume_symbol (head, ustrlwc (symbol-> value), utmp_filename);
                         mem_free (utmp_filename);
                       }
                    mem_free (char_filename);
                  }
              }
            else
            /* If form                                                       */
            if ( (symbol = usym_lookup_symbol (header_tab, L"name")) != NULL )
              {
                value = ascii2ucode_ex (p_data, codepage);
                if (value)
                  {                
                    usym_assume_symbol (head, ustrlwc (symbol-> value), value);
                    mem_free (value);
                  }
              } 
            else
              {
                /* we assume this is a body message                          */
                value = ascii2ucode_ex (p_data, codepage);
                if (value)
                  {                
                    usym_assume_symbol (head, L"bodymessage", value);
                    mem_free (value);
                  }
              }

            p_next += sep_size;

            /*  Found end of file marker                                     */
            if (*p_next == '-' && *(p_next + 1) == '-')
                break;
            else
                while (*p_next == '\r' || *p_next == '\n')
                    p_next++;
          }
      }

    usym_delete_table (header_tab);

    return (nb_files);
}

/*  -------------------------------------------------------------------------
    Function: multipart_decode_header

    Synopsis: Decode mime header of a part.
    -------------------------------------------------------------------------*/

static void
multipart_decode_header (char *header, USYMTAB *table, qbyte codepage)
{
    char
        *p,
        *variable,
        *value = "";
    UCODE
        *uvariable,
        *uvalue;

    ASSERT (header);
    ASSERT (table);

    while ( *header == ' ' )
        header++;

    p        = header;
    variable = header;
    while (*p)
      {
        if ((*p == ':' && *(p + 1) == ' ')
        ||   *p == '=')
          {
            *p++ = '\0';
            if (*p == ' ' || *p == '"')
                p++;
            value = p;
          }
        else
        if (*p == ';' || *p == '\r'||  *p == '\n')
          {
            if (*(p - 1) == '"')
                *(p - 1) = '\0';
            else
                *p = '\0';
            http_unescape (value, NULL);
            uvalue = ascii2ucode_ex (value, codepage);
            if (uvalue)
              {
                uvariable = ascii2ucode_ex (variable, codepage);
                if (uvariable)
                  {
                     usym_assume_symbol (table, uvariable, uvalue);
                     mem_free (uvariable);
                  }
                mem_free (uvalue);
              }
            p++;
            while (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')
                p++;
            variable = p;
          }
        p++;
      }
    if (p != header && *(p - 1) == '"')
        *(p - 1) = '\0';
    http_unescape (value, NULL);
    uvalue = ascii2ucode_ex (value, codepage);
    if (uvalue)
      {
         uvariable = ascii2ucode_ex (variable, codepage);
         if (uvariable)
           {
             usym_assume_symbol (table, uvariable, uvalue);
             mem_free (uvariable);
           }
         mem_free (uvalue);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: http_unescape

    Synopsis: Removes HTTP escaping from a string.  See http_escape() for
    details of the escaping algorithm.  If the result string is NULL,
    modifies the source string in place, else fills-in the result string.
    Returns the resulting string.  End-of-line sequences (%0A%0D) are
    stored as a single new-line character, i.e. carriage-returns (%0D) are
    not stored.
    ---------------------------------------------------------------------[>]-*/

UCODE *
http_ucode_unescape (
    UCODE *string,
    UCODE *result)
{
    UCODE
        *target;                        /*  Where we store the result        */

    ASSERT (string);
    if (!result)                        /*  If result string is null,        */
        result = string;                /*    modify in place                */
    target = result;

    while (*string)
      {
        if (*string == (UCODE)'%'              /*  Unescape %xx sequence            */
        &&   string [1] && string [2])
          {
            string++;
            *target = decode_uhex ((const UCODE **) &string, 2);
            if (*target != (UCODE)'\r')
                target++;               /*  We do not store CRs              */
          }
        else
          {
            if (*string == (UCODE)'+')         /*  Spaces are escaped as '+'        */
                *target++ = (UCODE)' ';
            else
                *target++ = *string;    /*  Otherwise just copy              */

            string++;
          }
      }
    *target = (UCODE)'\0';                     /*  Terminate target string          */
    return (result);
}


/*  -------------------------------------------------------------------------
    Function: decode_uhex

    Synopsis: Decodes a unicode hexadecimal string.  Stops after outmax characters
    or when an invalid hex character is reached.  Sets the input pointer
    to the first unprocessed character.  Returns the result.
    -------------------------------------------------------------------------*/

static int
decode_uhex (
    const UCODE   **input,
    size_t         outmax)
{
    static char
        hex_to_bin [128] = {
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,    /*   0..9     */
           -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*   A..F     */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*   a..f     */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };  /*            */
    int
        nextch;
    size_t
        index,
        result;

    ASSERT (input);
    ASSERT (*input);

    index  = 0;
    result = 0;
    while (outmax == 0 || index < outmax)
      {
        nextch = (*input) [index] & 127;
        if (nextch && hex_to_bin [nextch] != -1)
          {
            result = result * 16 + hex_to_bin [nextch];
            index++;
          }
        else
            break;
      }
    (*input) += index;
    return (result);
}


