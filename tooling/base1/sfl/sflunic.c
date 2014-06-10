/*===========================================================================*
 *                                                                           *
 *  sflunic.c - Unicode functions                                            *
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

#include "prelude.h"
#include "sflfile.h"
#include "sflmem.h"
#include "sflunic.h"

/*- Macro definitions -------------------------------------------------------*/

#define  REPLACEMENT_CHAR      0x0000FFFDUL
#define  UCS2_MAXIMUM          0x0000FFFFUL

#if defined (INTERNAL_UCS4)
#    define  CODE_MAXIMUM      0x7FFFFFFFUL
#else
#    define  CODE_MAXIMUM      0x0010FFFFUL
#endif

#define HALF_SHIFT             10
#define HALF_BASE              0x0010000UL
#define HALF_MASK              0x3FFUL
#define REPLACE_HIGH_START     0xD800UL
#define REPLACE_HIGH_END       0xDBFFUL
#define REPLACE_LOW_START      0xDC00UL
#define REPLACE_LOW_END        0xDFFFUL


/*- Program variables -------------------------------------------------------*/

static qbyte offset_from_UTF8 [6] ={
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL};

static byte bytes_from_UTF8 [256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};

static byte first_byte_mark [7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

/*  Globals                                                                  */
char *xustrcpy_file = "";               /*  Source file calling xustrcpy     */
word  xustrcpy_line = 0;                /*  Source line for call             */

/*- Macro definition --------------------------------------------------------*/
#define UTF16_TO_UCODE                          \
        while (code < end)                      \
         {                                      \
            *code = 0;                          \
            value = fgetc (stream-> file);      \
            if (value == EOF)                   \
                break;                          \
            *code = ((UCODE)value) << 8;        \
            value = fgetc (stream-> file);      \
            if (value == EOF)                   \
                break;                          \
            *code += value;                     \
            code++;                             \
            feedback++;                         \
          }

#define UTF8_TO_UCODE_VAR                                                    \
    UCODE *target;                   /* pointer to the value               */\
    qbyte  value;                    /* Unicode value                      */\
    byte   nb_bytes;                 /* Number of UTF8 bytes               */\
    byte  *source;                   /* pointer to current byte            */\
    byte   utf8_bytes [6];           /* UTF8 bytes                         */\
    int    tmpval;

#define UTF8_TO_UCODE(file, ucode)                                           \
    target      = *ucode;                                                    \
    value       = 0;                                                         \
    tmpval      = fgetc (file);                                              \
    if (tmpval == EOF)                                                       \
        break;                                                               \
    *utf8_bytes = (byte)tmpval;                                              \
    nb_bytes    = bytes_from_UTF8 [*utf8_bytes];                             \
    if (nb_bytes > 0)                                                        \
        fread (&utf8_bytes [1], 1, nb_bytes, file);                          \
    source = utf8_bytes;                                                     \
    switch(nb_bytes)                                                         \
      {                                                                      \
        case 5: value += *source++; value <<= 6;                             \
        case 4: value += *source++; value <<= 6;                             \
        case 3: value += *source++; value <<= 6;                             \
        case 2: value += *source++; value <<= 6;                             \
        case 1: value += *source++; value <<= 6;                             \
        case 0: value += *source++;                                          \
      }                                                                      \
    value -= offset_from_UTF8 [nb_bytes];                                    \
    if (value <= UCS2_MAXIMUM)                                               \
        *target++ = (UCODE)value;                                            \
    else                                                                     \
      {                                                                      \
        if (value > CODE_MAXIMUM)                                            \
			*target++ = REPLACEMENT_CHAR;                                    \
		else                                                                 \
          {                                                                  \
            value -= HALF_BASE;                                              \
            *target++ = (UCODE)((value >> HALF_SHIFT) + REPLACE_HIGH_START); \
            *target++ = (UCODE)((value &  HALF_MASK)  + REPLACE_LOW_START);  \
          }                                                                  \
      }


/*- Local function declarations ---------------------------------------------*/

static void get_file_format    (FILE *file, byte *format, int *byte_order);
static void seek_after_header  (UFILE *file);
static long read_buffer_utf8   (UCODE *buffer, long maxsize, UFILE *stream);
static long read_buffer_utf16  (UCODE *buffer, long maxsize, UFILE *stream);
static long read_buffer_ascii  (UCODE *buffer, long maxsize, UFILE *stream);
static long write_buffer_utf8  (UCODE *buffer, long size,    UFILE *stream);
static long write_buffer_utf16 (UCODE *buffer, long size,    UFILE *stream);
static long write_buffer_ascii (UCODE *buffer, long size,    UFILE *stream);


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_open

    Synopsis: Open a text file (unicode or ascii). If you open a file in writing
    mode, you must use the 'unicode_set_format' function to set format of file
    before writing data. The default format is ASCII.
    ---------------------------------------------------------------------[>]-*/

UFILE *
unicode_open (const char *file_name, const char mode)
{
    UFILE
        *feedback = NULL;
    FILE
        *file = NULL;
    byte
        format = UNICODE_FORMAT_ASCII;
    int
        byte_order = LITTLE_ENDIAN;
    long
        file_size;

    if (mode == 'r')
      {
        file = fopen (file_name, FOPEN_READ_BINARY);
        if (file)
            get_file_format (file, &format, &byte_order);
      }
    else
    if (mode == 'w')
        file = fopen (file_name, FOPEN_WRITE_BINARY);
    else
    if (mode == 'a'
    &&  safe_to_extend (file_name))
      {
        file_size = get_file_size (file_name);
        file = fopen (file_name, FOPEN_APPEND_BINARY);
        if (file && file_size == 0)
            get_file_format (file, &format, &byte_order);
      }

    if (file)
      {
        feedback = mem_alloc (sizeof (UFILE));
        if (feedback)
          {
            feedback-> file       = file;
            feedback-> format     = format;
            feedback-> byte_order = byte_order;
            seek_after_header (feedback);
          }
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_close

    Synopsis: Close unicode file and free all allocated resource.
    ---------------------------------------------------------------------[>]-*/

int
unicode_close (UFILE *stream)
{
    int
        feedback = 0;

    ASSERT (stream);

    if (stream-> file)
        fclose (stream-> file);
    memset (stream, 0, sizeof (UFILE));
    mem_free (stream);

    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_write

    Synopsis: Write a unicode buffer to a file.
    ---------------------------------------------------------------------[>]-*/

long
unicode_write (UCODE *buffer, long size, UFILE *stream)
{
    long
        feedback = 0;

    switch (stream-> format)
      {
        case UNICODE_FORMAT_UTF8:
            feedback = write_buffer_utf8  (buffer, size, stream);
            break;
        case UNICODE_FORMAT_UTF16:
            feedback = write_buffer_utf16 (buffer, size, stream);
            break;
        case UNICODE_FORMAT_UTF32:
            break;
        default:
            feedback = write_buffer_ascii (buffer, size, stream);
            break;
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_read

    Synopsis: Read a unicode string from a file. max_size content the maximum
    size to read. The buffer must be allocated by 'unicode_alloc' and freed by
    'mem_free'.
    ---------------------------------------------------------------------[>]-*/

long
unicode_read (UCODE *buffer, long max_size, UFILE *stream)
{
    long
        feedback = 0;

    switch (stream-> format)
      {
        case UNICODE_FORMAT_UTF8:
            feedback = read_buffer_utf8  (buffer, max_size, stream);
            break;
        case UNICODE_FORMAT_UTF16:
            feedback = read_buffer_utf16 (buffer, max_size, stream);
            break;
        case UNICODE_FORMAT_UTF32:
            break;
        default:
            feedback = read_buffer_ascii (buffer, max_size, stream);
            break;
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_getc

    Synopsis: Get a Unicode character from a file
    ---------------------------------------------------------------------[>]-*/

UCODE
unicode_getc (UFILE *stream)
{
    UCODE
        *code,
        feedback = 0;
    UTF8_TO_UCODE_VAR;

    switch (stream-> format)
      {
        case UNICODE_FORMAT_UTF8:
            code = &feedback;
            UTF8_TO_UCODE (stream->file, &code);
            break;
        case UNICODE_FORMAT_UTF16:
            fread (utf8_bytes, 1, 2, stream-> file);
            if (stream-> byte_order == BYTE_ORDER)
                feedback = *((UCODE *)utf8_bytes);
            else
            if (stream-> byte_order == LITTLE_ENDIAN)
              {
                feedback  =  (UCODE)utf8_bytes [0];
                feedback += ((UCODE)utf8_bytes [1]) << 8;
              }
            else
              {
                feedback  = ((UCODE)utf8_bytes [0]) << 8;
                feedback +=  (UCODE)utf8_bytes [0];
              }
            break;
        case UNICODE_FORMAT_UTF32:
            break;
        default:
            break;
      }

    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_get_format

    Synopsis: Get the encode format of unicode file.
    ---------------------------------------------------------------------[>]-*/

byte
unicode_get_format (UFILE *stream)
{
    ASSERT (stream);
    return (stream-> format);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_set_format

    Synopsis: Set the format and byte order of the file. If the file is open,
    Save the format signature in the beginning of the file.
    ---------------------------------------------------------------------[>]-*/

void
unicode_set_format (UFILE *stream, byte format, int byte_order)
{
    byte
        signature [4];
    int
        size;
    ASSERT (stream);

    if (stream-> format == format)
        return;

    stream-> format     = format;
    stream-> byte_order = byte_order;

    if (stream-> file)
      {
        fseek (stream-> file, 0L, SEEK_SET);
        switch (stream-> format)
          {
            case UNICODE_FORMAT_UTF8:
                signature [0] = 0xEF;
                signature [1] = 0xBB;
                signature [2] = 0xBF;
                size = 3;
                break;
            case UNICODE_FORMAT_UTF16:
                if (stream-> byte_order == LITTLE_ENDIAN)
                  {
                    signature [0] = 0xFF;
                    signature [1] = 0xFE;
                  }
                else
                  {
                    signature [0] = 0xFE;
                    signature [1] = 0xFF;
                  }
                size = 2;
                break;
            case UNICODE_FORMAT_UTF32:
                if (stream-> byte_order == LITTLE_ENDIAN)
                  {
                    signature [0] = 0xFF;
                    signature [1] = 0xFE;
                    signature [2] = 0x00;
                    signature [3] = 0x00;
                  }
                else
                  {
                    signature [0] = 0x00;
                    signature [1] = 0x00;
                    signature [2] = 0xFE;
                    signature [3] = 0xFF;
                  }
                size = 4;
                break;
            default:
                break;
          }
        if (size)
            fwrite (signature, 1, size, stream-> file);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_get_file_format

    Synopsis: Return the text file format.
    ---------------------------------------------------------------------[>]-*/

byte
unicode_get_file_format (const char *file_name)
{
    byte
        feedback = 0;
    int
        order;
    FILE
        *file;

    file = fopen (file_name, FOPEN_READ_BINARY);
    if (file)
      {
        get_file_format (file, &feedback, &order);
        fclose (file);
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_get_byteorder

    Synopsis: Return the byte order (LITTLE_ENDIAN or BIG_ENDIAN)
    ---------------------------------------------------------------------[>]-*/

int
unicode_get_byteorder (UFILE *stream)
{
    ASSERT (stream);
    return (stream-> byte_order);
}


/*  ---------------------------------------------------------------------[<]-
    Function: unicode_convert

    Synopsis: Convert a file (ascii or unicode) to another encode format.
    ---------------------------------------------------------------------[>]-*/

Bool
unicode_convert (char *source_file, char *target_file, byte target_format)
{
#   define BUFFER_SIZE 16382
    UFILE
        *source = NULL,
        *target = NULL;
    UCODE
        *buffer;
    Bool
        feedback = FALSE;
    long
        size;

    ASSERT (source_file);
    ASSERT (target_file);

    source = unicode_open (source_file, 'r');
    if (source)
      {
        target = unicode_open (target_file, 'w');
        if (target)
          {
            buffer = unicode_alloc (BUFFER_SIZE);
            if (buffer)
              {
                feedback = TRUE;
                unicode_set_format (target, target_format, BYTE_ORDER);

                do {
                    size = unicode_read (buffer, BUFFER_SIZE, source);
                    if (size > 0)
                        unicode_write (buffer, size, target);
                } while (size > 0);
                mem_free (buffer);
              }
            unicode_close (target);
          }
        unicode_close (source);
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: mem_ustrdup

    Synopsis: Duplicate a unicode string.
    ---------------------------------------------------------------------[>]-*/

UCODE *
mem_ustrdup (const UCODE *source)
{
    UCODE
        *feedback = NULL;
    long
        size;

    if (source)
      {
        size = wcslen (source);
        feedback = unicode_alloc (size + 1);
        if (feedback)
            memcpy (feedback, source, (size + 1) * UCODE_SIZE);
      }
    return (feedback);
}

/*  ---------------------------------------------------------------------[<]-
    Function: lexucmp

    Synopsis: Performs an unsigned comparison of two unicode strings without
    regard to the case of any letters in the strings.
    Returns a value that is
    <TABLE>
        <_0     if string1 is less than string2
        ==_0    if string1 is equal to string2
        >_0     if string1 is greater than string2
    </TABLE>
    ---------------------------------------------------------------------[>]-*/

int
lexucmp (
    const UCODE *string1,
    const UCODE *string2)
{
    int cmp;

    ASSERT (string1);
    ASSERT (string2);

    do
      {
        cmp = (int) towlower (*string1) - (int) towlower (*string2);
      }
    while (*string1++ && *string2++ && cmp == 0);
    return (cmp);
}


/*  ---------------------------------------------------------------------[<]-
    Function: lexuncmp

    Synopsis: Performs an unsigned comparison of two strings without regard
    to the case of specified number of letters in the strings.
    Returns a value that is
    <TABLE>
        <_0     if string1 is less than string2
        ==_0    if string1 is equal to string2
        >_0     if string1 is greater than string2
    </TABLE>
    ---------------------------------------------------------------------[>]-*/

int
lexuncmp (
    const UCODE *string1,
    const UCODE *string2,
    const int   count)
{
    int
        cmp;
    UCODE
        *end;

    ASSERT (string1);
    ASSERT (string2);

    end = (UCODE *)string1 + count;
    do
      {
        cmp = (int) towlower (*string1) - (int) towlower (*string2);
      }
    while (*string1++ && *string2++ && cmp == 0 && string1 < end);
    return (cmp);
}


/*  ---------------------------------------------------------------------[<]-
    Function: lexuwcmp

    Synopsis: Compares two strings ignoring case, and allowing wildcards
    in the second string (the pattern).  Two special characters are
    recognised in the pattern: '?' matches any character in the string,
    and '*' matches the remainder of the string.
    Returns a value that is:
    <TABLE>
        <_0     if string1 is less than pattern
        ==_0    if string1 is equal to pattern
        >_0     if string1 is greater than pattern
    </TABLE>
    ---------------------------------------------------------------------[>]-*/

int
lexuwcmp (
    const UCODE *string1,
    const UCODE *pattern)
{
    int cmp = 0;

    ASSERT (string1);
    ASSERT (pattern);

    do
      {
        if (*pattern != (UCODE)'?' && *pattern != (UCODE)'*')
            cmp = (int) towlower (*string1) - (int) towlower (*pattern);
      }
    while (*string1++ && *pattern++ && cmp == 0 && *pattern != (UCODE)'*');
    return (cmp);
}

/*  ---------------------------------------------------------------------[<]-
    Function: usearchreplace

    Synopsis: A case insensitive search and replace. Searches for all
    occurances of a string, and replaces it with another string.
    Returns a pointer
    to head of the buffer.
    ---------------------------------------------------------------------[>]-*/

UCODE *
usearchreplace (
    UCODE *strbuf,
    UCODE *strtofnd,
    UCODE *strtoins)
{
    UCODE 
        *offset,
        *strbase;

    ASSERT (strbuf);
    ASSERT (strtofnd);
    ASSERT (strtoins);

    offset = strbase = (UCODE *)NULL;

    while (*strbuf)
      {
        offset = ustricstr (!offset ? strbuf : strbase, strtofnd);
        if (offset)
          {
            strbase = (offset + wcslen (strtoins));
            wcscpy (offset, (offset + wcslen (strtofnd)));
            memmove (offset + wcslen (strtoins),
                     offset, (wcslen (offset) + 1) * UCODE_SIZE);
            memcpy (offset, strtoins, wcslen (strtoins) * UCODE_SIZE);
         }

      else
           break;
     }

   return (strbuf);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustricstr

    Synopsis:
    A case insensitive strstr.  Returns a pointer to head of the str1.
    Submitted by Scott Beasley <jscottb@infoave.com>
    ---------------------------------------------------------------------[>]-*/

UCODE *
ustricstr (
    const UCODE *str1,
    const UCODE *str2)
{
    UCODE
        *strtmp = (UCODE *)str1;
    size_t
        length_2;
    int
        iret = 1;

    ASSERT (str1);
    ASSERT (str2);

    length_2 = wcslen (str2);
    while (*strtmp)
      {
        if (wcslen (strtmp) >= length_2)
           iret = lexuncmp (strtmp, str2, length_2);
       else
           break;

       if (!iret)
           break;

       strtmp++;
     }

   return !iret ? strtmp : (UCODE *)NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustrlwc

    Synopsis: Converts all alphabetic characters in string to lowercase,
    stopping at the final null character.  Returns string.  If string is
    null, returns null.  We do not call this function strlwr because that
    is already provided by some systems (but not by ANSI C).
    ---------------------------------------------------------------------[>]-*/

UCODE *
ustrlwc (
    UCODE *string)
{
    UCODE *scan;

    if (string)
      {
        scan = string;
        while (*scan)
          {
            *scan = (UCODE) towlower (*scan);
            scan++;
          }
      }
    return (string);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ucode2ascii

    Synopsis: Convert a unicode string to a ascii string.
    The return value must be freed by 'mem_free'.
    ---------------------------------------------------------------------[>]-*/

char *
ucode2ascii (const UCODE *string)
{
    char
        *feedback = NULL;
    register char
        *value;
    register UCODE
        *end,
        *uvalue;
    long
        length;

    ASSERT (string);

    length = wcslen (string);
    if (length > 0)
      {
        feedback = mem_alloc (length + 1);
        if (feedback)
          {
            end    = (UCODE *)string + length;
            uvalue = (UCODE *)string;
            value  = feedback;
            while (uvalue < end)
                wctomb (value++, *uvalue++);
            *value = '\0';
          }
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ucode2ascii_ex

    Synopsis: Convert a unicode string to an ASCII string.
    The return value must be freed by 'mem_free'.
    Use the specified code page.
    ---------------------------------------------------------------------[>]-*/

char *
ucode2ascii_ex (const UCODE *string, qbyte codepage)
{
#if defined (__WINDOWS__)
    char
        *res = NULL;
    UCODE
        *feedback = NULL;
    size_t
        length;

    ASSERT (string);
    /* first call computes size                                              */
    length = WideCharToMultiByte (codepage, 0, string, -1, NULL, 0, NULL,
                                   NULL );
    if (length > 0)
      {
        /* for some codepage, the length is wrong :-/                        */
        if (codepage == 50220 || codepage == 50222)
            length = (size_t)((double)length * 1.5);

        res = mem_alloc (length + 1);
        if (res)
            /* second call actually converts the Unicode string              */
            length = WideCharToMultiByte (codepage, 0, string, -1, res,
                                          length + 1, NULL, NULL);

        if (length == 0)
          {
            mem_free (res);
            res = NULL;
          }
        else
            res [length] = 0;
      }
    return (res);
#else
    return (ucode2ascii (string));
#endif
}


/*  ---------------------------------------------------------------------[<]-
    Function: ascii2ucode

    Synopsis: Convert a ascii string to a unicode string.
    The return value must be freed by 'mem_free'.
    ---------------------------------------------------------------------[>]-*/

UCODE *
ascii2ucode (const char *string)
{
    UCODE
        *feedback = NULL;
    register UCODE
        *uvalue;
    register char
        *end,
        *value;
    long
        length;

    ASSERT (string);

    length = strlen (string);
    if (length > 0)
      {
        feedback = unicode_alloc (length + 1);
        if (feedback)
          {
            end    = (char *)string + length;
            value = (char *)string;
            uvalue  = feedback;
            while (value < end)
                mbtowc (uvalue++, value++, 1);
            *uvalue = 0;
          }
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ascii2ucode_ex 

    Synopsis: Convert a ascii string to a unicode string.
    The return value must be freed by 'mem_free'.
    Use the specified code page. If the code page is a multi-byte code page
    (like japanese) the ascii buffer is a multi-byte ascii buffer.
    ---------------------------------------------------------------------[>]-*/

UCODE *
ascii2ucode_ex (const char  *string, qbyte codepage)
{
#if defined (__WINDOWS__)
    UCODE
        *feedback = NULL;
    long
        length;

    ASSERT (string);

    length = strlen (string);
    if (length > 0)
      {
        feedback = unicode_alloc (length + 1);
        if (feedback)
            MultiByteToWideChar (codepage, 0, string, -1, feedback, length + 1);
      }
    return (feedback);
#else
    return (ascii2ucode (string));
#endif
}

/*  ---------------------------------------------------------------------[<]-
    Function: unicode_fprintf

    Synopsis: unicode version of fprintf using unicode format specified.
    ---------------------------------------------------------------------[>]-*/

int
unicode_fprintf (UFILE *file, UCODE *format, ...)
{
    int
        feedback;
    UCODE
        buffer [2048];
    va_list
        argptr;

    va_start (argptr, format);          /*  Start variable arguments list    */
    feedback = vswprintf (buffer, format, argptr);
    va_end   (argptr);                  /*  End variable arguments list      */
    unicode_write (buffer, feedback, file);

    return (feedback);
}

/*################## INTERNAL FUNCTIONS #####################################*/

/*  -------------------------------------------------------------------------
    Function: get_file_format

    Synopsis: Get the header of the file and check codage signature.
    -------------------------------------------------------------------------*/

static void
get_file_format (FILE *file, byte *format, int *byte_order)
{
    byte
        header [5];
    long
        position;

    ASSERT (file);
    ASSERT (format);
    ASSERT (byte_order);

    *format     = UNICODE_FORMAT_ASCII;
    *byte_order = BYTE_ORDER;

    /* Get current position of the file                                      */
    position = ftell (file);

    /* Go to the beginning of the file                                       */
    if (position != 0L)
        fseek (file, 0, SEEK_SET);

    /* Read the header                                                       */
    memset (header, 0, sizeof (header));
    fread (header, 1, 4, file);

    if (header [0] == 0xEF
    &&  header [1] == 0xBB
    &&  header [2] == 0xBF)
        *format = UNICODE_FORMAT_UTF8;
    else
    if (header [0] == 0x00
    &&  header [1] == 0x00
    &&  header [2] == 0xFE
    &&  header [3] == 0xFF)
      {
        *format = UNICODE_FORMAT_UTF32;
        *byte_order = BIG_ENDIAN;
      }
    else
    if (header [0] == 0x00
    &&  header [1] == 0x00
    &&  header [2] == 0xFF
    &&  header [3] == 0xFE)
      {
        *format = UNICODE_FORMAT_UTF32;
        *byte_order = LITTLE_ENDIAN;
      }
    else
    if (header [0] == 0xFE
    &&  header [1] == 0xFF)
      {
        *format = UNICODE_FORMAT_UTF16;
        *byte_order = BIG_ENDIAN;
      }
    else
    if (header [0] == 0xFF
    &&  header [1] == 0xFE)
      {
        *format = UNICODE_FORMAT_UTF16;
        *byte_order = LITTLE_ENDIAN;
      }

    fseek (file, position, SEEK_SET);
}



/*  -------------------------------------------------------------------------
    Function: seek_after_header

    Synopsis: Seek the file to the position after the header signature.
    -------------------------------------------------------------------------*/

static void
seek_after_header (UFILE *file)
{
    ASSERT (file);

    switch (file-> format)
      {
        case UNICODE_FORMAT_UTF8:  fseek (file-> file, 3L, SEEK_SET); break;
        case UNICODE_FORMAT_UTF16: fseek (file-> file, 2L, SEEK_SET); break;
        case UNICODE_FORMAT_UTF32: fseek (file-> file, 4L, SEEK_SET); break;
        default:                                                      break;
      }
}


/*  -------------------------------------------------------------------------
    Function: read_buffer_utf8

    Synopsis: Read a buffer of data encoded in UTF8 format.
    -------------------------------------------------------------------------*/

static long
read_buffer_utf8  (UCODE *buffer, long maxsize, UFILE *stream)
{
    long
        feedback = 0;
    UCODE
        *end,
        *code;
    UTF8_TO_UCODE_VAR;

    end  = buffer + maxsize;
    code = buffer;

    while (code < end)
      {
        UTF8_TO_UCODE (stream-> file, &code);
        code++;
        feedback++;
      }
    return (feedback);
}


/*  -------------------------------------------------------------------------
    Function: read_buffer_utf16

    Synopsis: Read a buffer of data encoded in UTF16 format.
    -------------------------------------------------------------------------*/

static long
read_buffer_utf16  (UCODE *buffer, long maxsize, UFILE *stream)
{
    long
        feedback = 0;
    UCODE
        *code,
        *end;
    int
        value;

/* For unicode value in 16 bits format                                       */
#if !defined (INTERNAL_UCS4)
#  if (BYTE_ORDER == LITTLE_ENDIAN)
    if (stream-> byte_order == LITTLE_ENDIAN)
        feedback = fread (buffer, 2, maxsize, stream-> file);
    else
      {
        end  = buffer + maxsize;
        code = buffer;
        UTF16_TO_UCODE;
      }
#  else
    if (stream-> byte_order == BIG_ENDIAN)
        feedback = fread (buffer, 2, maxsize, stream-> file);
    else
      {
        end  = buffer + maxsize;
        code = buffer;
        UTF16_TO_UCODE;
      }
#  endif
#else
/* For unicode value in 16 bits format                                       */
    end  = buffer + maxsize;
    code = buffer;
#  if (BYTE_ORDER == LITTLE_ENDIAN)
    if (stream-> byte_order == LITTLE_ENDIAN)
      {
        while (code < end)
          {
            *code = 0;
            if (fread (*code, 2, 1, stream-> file) == 0)
                break;
            code++;
            feedback++;
          }
      }
    else
      {
        UTF16_TO_UCODE;
      }
#  else
    if (stream-> byte_order == BIG_ENDIAN)
      {
        while (code < end)
          {
            *code = 0;
            if (fread (*code, 2, 1, stream-> file) == 0)
                break;
            code++;
            feedback++;
          }
      }
    else
      {
        UTF16_TO_UCODE;
      }
#  endif
#endif
    return (feedback);
}


/*  -------------------------------------------------------------------------
    Function: read_buffer_ascii

    Synopsis: Read a buffer of data encoded in ascii format.
    -------------------------------------------------------------------------*/

static long
read_buffer_ascii (UCODE *buffer, long maxsize, UFILE *stream)
{
    long
        feedback = 0;
    int
        value;
    UCODE
        *code,
        *end;

    code = buffer;
    end  = buffer + maxsize;

    while (code < end)
      {
        value = fgetc (stream-> file);
        if (value == EOF)
            break;
        mbtowc (code, (const char *)&value, 1);
        code++;
        feedback++;
      }

    return (feedback);
}


/*  -------------------------------------------------------------------------
    Function: write_buffer_utf8

    Synopsis: Write a buffer of data encoded in UTF8 format.
    -------------------------------------------------------------------------*/

static long
write_buffer_utf8  (UCODE *buffer, long size, UFILE *stream)
{
    long
        feedback = 0;
    UCODE
        *end,
        *code;
    dbyte
        byte_to_write;
    register const qbyte
        byte_mask = 0xBF,
        byte_mark = 0x80;
    register qbyte
        value;
    qbyte
        value2;
    byte
        *target,
        byte_buffer [7];

    end  = buffer + size;
    code = buffer;

    while (code < end)
      {
        byte_to_write = 0;
        value = *code++;
        if (value >= REPLACE_HIGH_START
        &&  value <= REPLACE_HIGH_END)
          {
            value2 = *code;
			if (value2 >= REPLACE_LOW_START
            &&  value2 <= REPLACE_LOW_END)
              {
				value = ((value  - REPLACE_HIGH_START) << HALF_SHIFT)
					   + (value2 - REPLACE_LOW_START) + HALF_BASE;
				++code;
			  }
		  }
		if (value < 0x80)
            byte_to_write = 1;
		else if (value < 0x800)
            byte_to_write = 2;
        else if (value < 0x10000)
            byte_to_write = 3;
        else if (value < 0x200000)
            byte_to_write = 4;
        else if (value < 0x4000000)
            byte_to_write = 5;
        else if (value <= CODE_MAXIMUM)
            byte_to_write = 6;
        else
          {
            byte_to_write = 2;
			value = REPLACEMENT_CHAR;
		  }

		target = &byte_buffer [byte_to_write];
		switch (byte_to_write)
          {
            case 6: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 5: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 4: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 3: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 2: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 1: *--target = (byte)( value | first_byte_mark [byte_to_write]);
          }
        fwrite (byte_buffer, 1, byte_to_write, stream-> file);
        feedback++;
      }
    return (feedback);
}


/*  -------------------------------------------------------------------------
    Function: write_buffer_utf16

    Synopsis: Write a buffer of data encoded in UTF16 format.
    -------------------------------------------------------------------------*/

static long
write_buffer_utf16  (UCODE *buffer, long size, UFILE *stream)
{
    long
        feedback = 0;
    UCODE
        *end,
        *code;
    int
        low,
        high;
    byte
        byte_buffer [2];
#if defined (INTERNAL_UCS4)
    dbyte
        value;
#endif

    if (stream-> byte_order == BYTE_ORDER)
      {
/* For unicode value in 16 bits format                                       */
#if !defined (INTERNAL_UCS4)
        feedback = fwrite (buffer, 2, size, stream-> file);
#else
        end  = buffer + size;
        code = buffer;

        while (code < end)
          {
            value = (dbyte)*code++;
            fwrite (&value, 2, 1, stream-> file);
            feedback++;
          }
#endif
      }
    else
      {
        end  = buffer + size;
        code = buffer;
        if (stream-> byte_order == LITTLE_ENDIAN)
          {
            low  = 0;
            high = 1;
          }
        else
          {
            low  = 1;
            high = 0;
          }
        while (code < end)
          {
            byte_buffer [low]  = (byte) (*code & 0x00FF);
            byte_buffer [high] = (byte)((*code & 0xFF00) >> 8);
            fwrite (byte_buffer, 1, 2, stream-> file);
            code++;
          }
      }
    return (feedback);
}


/*  -------------------------------------------------------------------------
    Function: write_buffer_ascii

    Synopsis: Write a buffer of data encoded in ascii format.
    -------------------------------------------------------------------------*/

static long
write_buffer_ascii (UCODE *buffer, long size, UFILE *stream)
{
    long
        feedback = 0;
    UCODE
        *end,
        *code;
    char
        value [6];

    code = buffer;
    end  = buffer + size;

    while (code < end)
      {
         if (wctomb (value, *code++) != -1)
           {
             fwrite (value, 1, 1, stream-> file);
             feedback++;
           }
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ucode2utf8_size

    Synopsis: Calculate the size of buffer to store in UTF8 format.
    ---------------------------------------------------------------------[>]-*/

long
ucode2utf8_size (const UCODE *string)
{
    long
        feedback = 0;
    UCODE
        *code;
    dbyte
        byte_to_write;
    register const qbyte
        byte_mask = 0xBF,
        byte_mark = 0x80;
    register qbyte
        value;
    qbyte
        value2;

    ASSERT (string);

    code = (UCODE *)string;

    while (*code)
      {
        byte_to_write = 0;
        value = *code++;
        if (value >= REPLACE_HIGH_START
        &&  value <= REPLACE_HIGH_END)
          {
            value2 = *code;
			if (value2 >= REPLACE_LOW_START
            &&  value2 <= REPLACE_LOW_END)
              {
				value = ((value  - REPLACE_HIGH_START) << HALF_SHIFT)
					   + (value2 - REPLACE_LOW_START) + HALF_BASE;
				++code;
			  }
		  }
		if (value < 0x80)
            byte_to_write = 1;
		else if (value < 0x800)
            byte_to_write = 2;
        else if (value < 0x10000)
            byte_to_write = 3;
        else if (value < 0x200000)
            byte_to_write = 4;
        else if (value < 0x4000000)
            byte_to_write = 5;
        else if (value <= CODE_MAXIMUM)
            byte_to_write = 6;
        else
            byte_to_write = 2;
        feedback += byte_to_write;
      }

    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ucode2utf8

    Synopsis: Convert a unicode buffer to a ascii buffer in UTF8 format.
              The returned string must be free by mem_free.
              Return NULL if error.
    ---------------------------------------------------------------------[>]-*/

char *
ucode2utf8 (const UCODE *string)
{
    byte
        *feedback = NULL;
    long
        size;
    UCODE
        *code;
    dbyte
        byte_to_write;
    register const qbyte
        byte_mask = 0xBF,
        byte_mark = 0x80;
    register qbyte
        value;
    qbyte
        value2;
    byte
        *target,
        *byte_buffer;

    size = ucode2utf8_size (string);
    if (size <= 0)
        return (NULL);

    feedback = mem_alloc (size + 1);
    if (feedback == NULL)
        return (NULL);
    
    code = (UCODE *)string;
    byte_buffer = feedback;
    feedback [size] = 0;

    while (*code)
      {
        byte_to_write = 0;
        value = *code++;
        if (value >= REPLACE_HIGH_START
        &&  value <= REPLACE_HIGH_END)
          {
            value2 = *code;
			if (value2 >= REPLACE_LOW_START
            &&  value2 <= REPLACE_LOW_END)
              {
				value = ((value  - REPLACE_HIGH_START) << HALF_SHIFT)
					   + (value2 - REPLACE_LOW_START) + HALF_BASE;
				++code;
			  }
		  }
		if (value < 0x80)
            byte_to_write = 1;
		else if (value < 0x800)
            byte_to_write = 2;
        else if (value < 0x10000)
            byte_to_write = 3;
        else if (value < 0x200000)
            byte_to_write = 4;
        else if (value < 0x4000000)
            byte_to_write = 5;
        else if (value <= CODE_MAXIMUM)
            byte_to_write = 6;
        else
          {
            byte_to_write = 2;
			value = REPLACEMENT_CHAR;
		  }

		target = &byte_buffer [byte_to_write];
		switch (byte_to_write)
          {
            case 6: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 5: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 4: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 3: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 2: *--target = (byte)((value | byte_mark) & byte_mask); value >>= 6;
			case 1: *--target = (byte)( value | first_byte_mark [byte_to_write]);
          }
        byte_buffer += byte_to_write;
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ucoutf82ucode_sizede2utf8_size

    Synopsis: Calculate the size of buffer to store unicode value.
    ---------------------------------------------------------------------[>]-*/

long
utf82ucode_size (const char  *string)
{
    long
        feedback = 0;
    byte
        nb_bytes;                       /* Number of UTF8 bytes               */
    register byte
        *value;

    value = (byte *)string;

    while (*value)
      {
        nb_bytes = bytes_from_UTF8 [*value] + 1;
        feedback += nb_bytes;
        value    += nb_bytes;
      }
    
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: utf82ucode

    Synopsis: Convert a ascii buffer in UTF8 format to a unicode buffer;
              The returned string must be free by mem_free.
              Return NULL if error.
    ---------------------------------------------------------------------[>]-*/

UCODE *
utf82ucode (const char  *string)
{
    UCODE
        *feedback = NULL;
    long
        size;
    UCODE 
        *target;                        /* pointer to the value               */
    qbyte
        value;                          /* Unicode value                      */
    byte
        nb_bytes,                       /* Number of UTF8 bytes               */
       *source;                         /* pointer to current byte            */

    size = utf82ucode_size (string);

    feedback = unicode_alloc (size + 1);
    if (feedback)
      {

        target  = feedback;
        value   = 0;
        source  = (byte *)string;
        while (*source)
          {
            nb_bytes = bytes_from_UTF8 [*source];
            value    = 0;
            switch(nb_bytes)
              {
                case 5: value += *source++; value <<= 6;
                case 4: value += *source++; value <<= 6;
                case 3: value += *source++; value <<= 6;
                case 2: value += *source++; value <<= 6;
                case 1: value += *source++; value <<= 6;
                case 0: value += *source++;
              }
            value -= offset_from_UTF8 [nb_bytes];
            if (value <= UCS2_MAXIMUM)
                *target++ = (UCODE)value;
            else
              {
                if (value > CODE_MAXIMUM)
			        *target++ = REPLACEMENT_CHAR;
		        else
                  {
                    value -= HALF_BASE;
                    *target++ = (UCODE)((value >> HALF_SHIFT) + REPLACE_HIGH_START); 
                    *target++ = (UCODE)((value &  HALF_MASK)  + REPLACE_LOW_START);  
                  }
              }
          }
         *target = 0;
      }
    return (feedback);
}

/*  ---------------------------------------------------------------------[<]-
    Function: conv_ustr_bool

    Synopsis: Converts a string to a Bool.  Accepts T/Y/1 as TRUE, F/N/0
    as FALSE, ignoring case.  Looks only at the first letter of the string.
    Returns 1 for TRUE, 0 for FALSE, -1 if the string was not valid.
    ---------------------------------------------------------------------[>]-*/

int
conv_ustr_bool (
    const UCODE *string)
{
    UCODE
        ch;

    ch = towlower (string [0]);
    if (ch == (UCODE)'y' || ch == (UCODE)'t' || ch == (UCODE)'1')
        return (1);
    else
    if (ch == (UCODE)'n' || ch == (UCODE)'f' || ch == (UCODE)'0')
        return (0);
    else
      {
        return (-1);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustrconvch

    Synopsis: Converts all instances of one character in a string to some
    other character.  Returns string.  Does nothing if the string is NULL.
    ---------------------------------------------------------------------[>]-*/

UCODE *
ustrconvch (
    UCODE *string,
    UCODE from,
    UCODE to)
{
    UCODE *scan;

    if (string)
      {
        scan = string;
        while (*scan)
          {
            if (*scan == from)
               *scan = to;
            scan++;
          }
      }
    return (string);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustrt2descr

    Synopsis: Converts a table of strings into a single block of memory.
    The input table consists of an array of null-terminated strings,
    terminated in a null pointer.  Returns the address of a DESCR block
    defined as: "typedef struct {size_t size; byte *data} DESCR;".
    Allocates the descriptor block using the mem_alloc() function; you must
    free it using mem_free() when you are finished with it. The strings are
    packed into the descriptor data field, each terminated by a null byte.
    The final string is terminated by two nulls.  The total size of the
    descriptor is descr-> size + sizeof (DESCR).  Note that if you omit the
    last null pointer in the input table, you will probably get an addressing
    error.  Returns NULL if there was insufficient memory to allocate the
    descriptor block.
    ---------------------------------------------------------------------[>]-*/

DESCR *
ustrt2descr (
    UCODE **table)
{
    DESCR
        *descr;                         /*  Allocated descriptor             */
    UCODE
        *descr_ptr;                     /*  Pointer into block               */
    size_t
        descr_size;                     /*  Size of table                    */
    int
        string_nbr;                     /*  Index into string table          */

    ASSERT (table);

    /*  Calculate the size of the descriptor                                 */
    descr_size = 1;                     /*  Allow for final null byte        */
    for (string_nbr = 0; table [string_nbr]; string_nbr++)
        descr_size += wcslen (table [string_nbr]) + 1;

    /*  Allocate a descriptor and fill it with the strings                   */
    descr = mem_alloc ((descr_size * UCODE_SIZE) + sizeof (DESCR));
    if (descr)
      {
        descr-> size = descr_size;
        descr-> data = (byte *) descr + sizeof (DESCR);
        descr_ptr    = (UCODE *) descr-> data;

        for (string_nbr = 0; table [string_nbr]; string_nbr++)
          {
            size_t descr_len = wcslen (table [string_nbr]) + 1;
            wcsncpy (descr_ptr, table [string_nbr], descr_len);
            descr_ptr += descr_len;
          }
        *descr_ptr = (UCODE)'\0';            /*  Add a null string                */
      }
    return (descr);
}


/*  ---------------------------------------------------------------------[<]-
    Function: descr2ustrt

    Synopsis: Takes a descriptor prepared by strt2descr() and returns an
    array of strings pointers, terminated in a null pointer.  The array is
    allocated using the mem_alloc() function.  Each string is individually
    allocated.  Thus, to free the string table you must call mem_free() for
    each entry in the table, except the last one, and then for the table.
    You can also call strtfree() to destroy the table in a single operation.
    Returns NULL if there was insufficient memory to allocate the table of
    strings.
    ---------------------------------------------------------------------[>]-*/

UCODE **
descr2ustrt (
    const DESCR *descr)
{
    UCODE
        **table;
    int
        string_count,
        string_nbr;                     /*  Index into string table          */
    UCODE
        *descr_ptr;                     /*  Pointer into block               */

    ASSERT (descr);

    /*  Count the number of strings in the table                             */
    descr_ptr = (UCODE *) descr-> data;
    string_count = 0;
    while (*descr_ptr)                  /*  Loop until we hit null string    */
      {
        string_count++;
        descr_ptr += wcslen (descr_ptr) + 1;
      }

    /*  Allocate a table and fill it with the strings                        */
    table = mem_alloc ((string_count + 1) * sizeof (UCODE *));
    if (table)
      {
        descr_ptr = (UCODE *) descr-> data;
        for (string_nbr = 0; string_nbr < string_count; string_nbr++)
          {
            table [string_nbr] = mem_ustrdup (descr_ptr);
            descr_ptr += wcslen (descr_ptr) + 1;
          }
        table [string_count] = NULL;    /*  Store final null pointer         */
      }
    return (table);
}


/*  ---------------------------------------------------------------------[<]-
    Function: ustrtfree

    Synopsis: Releases a table of strings as created by descr2strt() or a
    similar function.  If the argument is null, does nothing.
    ---------------------------------------------------------------------[>]-*/

void
ustrtfree (
    UCODE **table)
{
    int
        string_nbr;                     /*  Index into string array          */

    if (table)
      {
        for (string_nbr = 0; table [string_nbr]; string_nbr++)
            mem_free (table [string_nbr]);
        mem_free (table);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: xustrcat

    Synopsis: Concatenates multiple strings into a single result.  Eg.
    xstrcat (buffer, "A", "B", NULL) stores "AB" in buffer.  Returns dest.
    Append the string to any existing contents of dest.
    From DDJ Nov 1992 p. 155, with adaptions.
    ---------------------------------------------------------------------[>]-*/

UCODE *
xustrcat (
    UCODE *dest,
    const UCODE *src, ...)
{
    UCODE
        *feedback = dest;
    va_list
        va;

    ASSERT (dest);
    while (*dest)                       /*  Find end of dest string          */
        dest++;

    va_start (va, src);
    while (src)
      {
        while (*src)
            *dest++ = *src++;
        src = va_arg (va, UCODE *);
      }
    *dest = (UCODE)'\0';                       /*  Append a null character          */
    va_end (va);
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: xustrcpy

    Synopsis: Concatenates multiple strings into a single result.  Eg.
    xstrcpy (buffer, "A", "B", NULL) stores "AB" in buffer.  Returns dest.
    Any existing contents of dest are cleared.  If the dest buffer is NULL,
    allocates a new buffer with the required length and returns that.  The
    buffer is allocated using mem_alloc(), and should eventually be freed
    using mem_free() or mem_strfree().  Returns NULL if there was too little
    memory to allocate the new string.  We can't define macros with variable
    argument lists, so we pass the file and line number through two globals,
    xstrcpy_file and xstrcpy_line, which are reset to empty values after
    each call to xstrcpy.
    ---------------------------------------------------------------------[>]-*/

UCODE *
xustrcpy (
    UCODE *dest,
    const UCODE *src, ...)
{
    const UCODE
        *src_ptr;
    va_list
        va;
    size_t
        dest_size;                      /*  Size of concatenated strings     */

    /*  Allocate new buffer if necessary                                     */
    if (dest == NULL)
      {
        va_start (va, src);             /*  Start variable args processing   */
        src_ptr   = src;
        dest_size = 1;                  /*  Allow for trailing null char     */
        while (src_ptr)
          {
            dest_size += wcslen (src_ptr);
            src_ptr = va_arg (va, UCODE *);
          }
        va_end (va);                    /*  End variable args processing     */

        /*  Allocate by going directly to mem_alloc_ function                */
        dest = mem_alloc_ (NULL, dest_size * UCODE_SIZE,
                           xustrcpy_file, xustrcpy_line);
        xustrcpy_file = "";
        xustrcpy_line = 0;
        if (dest == NULL)
            return (NULL);              /*  Not enough memory                */
      }

    /*  Now copy strings into destination buffer                             */
    va_start (va, src);                 /*  Start variable args processing   */
    src_ptr  = src;
    dest [0] = '\0';
    while (src_ptr)
      {
        wcscat (dest, src_ptr);
        src_ptr = va_arg (va, UCODE *);
      }
    va_end (va);                        /*  End variable args processing     */
    return (dest);
}
