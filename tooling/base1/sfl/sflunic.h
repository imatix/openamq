/*===========================================================================*
 *                                                                           *
 *  sflunic.h - Unicode functions                                            *
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
/*  ----------------------------------------------------------------<Prolog>-
    Synopsis:   Provides functions to read and write text files with unicode
                support and unicode string function.
 ------------------------------------------------------------------</Prolog>-*/

#ifndef SFLUNICODE_INCLUDED             /*  Allow multiple inclusions        */
#define SFLUNICODE_INCLUDED

/*- Definitions -------------------------------------------------------------*/

/* Get compiler byte order                                                   */

#ifndef BYTE_ORDER
#define LITTLE_ENDIAN   1234            /* least-significant first (vax, pc) */
#define BIG_ENDIAN      4321            /* most-significant first (IBM, net) */
#define PDP_ENDIAN      3412            /* LSB first in word, MSW first in
                                           long (pdp)                        */
#if defined (__MSDOS__) || defined (__OS2__) || defined (vax) || \
    defined (ns32000) || defined (sun386)  || defined (__QNX__) || \
    defined (MIPSEL)  || defined (_MIPSEL) || \
    defined (BIT_ZERO_ON_RIGHT) || defined (__alpha__) || defined(__alpha)
#   define BYTE_ORDER      LITTLE_ENDIAN
#elif defined(sel) || defined(pyr) || defined(mc68000) || defined(sparc) || \
    defined(is68k) || defined(tahoe) || defined(ibm032) || defined(ibm370) || \
    defined(MIPSEB) || defined(_MIPSEB) || defined(_IBMR2) || defined(DGUX) ||\
    defined(apollo) || defined(__convex__) || defined(_CRAY) || \
    defined(__hppa) || defined(__hp9000) || \
    defined(__hp9000s300) || defined(__hp9000s700) || \
    defined (BIT_ZERO_ON_LEFT) || defined(m68k)
#   define BYTE_ORDER      BIG_ENDIAN
#endif

#endif /* BYTE_ORDER */
#if !defined(BYTE_ORDER) || \
    (BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN && \
    BYTE_ORDER != PDP_ENDIAN)
        /* you must determine what the correct bit order is for
         * your compiler - the next line is an intentional error
         * which will force your compiles to bomb until you fix
         * the above macros.
         */
#  error "Undefined or invalid BYTE_ORDER";
#endif

/* Remove comment of this define if you want store unicode in 32 bits        */
/*#define INTERNAL_UCS4              *//*  Store unicode value in 32 bits    */

/*  System-specific definitions                                              */
/*  If our compiler can tell us how big the unicode definitions are, we      */
/*  use those for compatibility.  Otherwise we use the compiled in           */
/*  definition.                                                              */

#if defined (SFL_UNICODE_TYPE)
typedef SFL_UNICODE_TYPE  UCODE;
#define UCODE_SIZE        sizeof(SFL_UNICODE_TYPE)

#else
#    if defined (INTERNAL_UCS4)
     /*  For 32 bits unicode character size                                 */

#         if (defined (__IS_32BIT__))
typedef unsigned long   UCODE;
#         else
typedef unsigned int    UCODE;
#         endif

#         define UCODE_SIZE            4

/* For 16 bits unicode character size                                        */
#    else

typedef unsigned short  UCODE;
#         define UCODE_SIZE            2

#    endif
#endif

#define UNICODE_FORMAT_ASCII  0
#define UNICODE_FORMAT_UTF8   1
#define UNICODE_FORMAT_UTF16  2
#define UNICODE_FORMAT_UTF32  3


#define unicode_alloc(size) (UCODE *)mem_alloc ((size) * UCODE_SIZE)

/*  Macro to prepare call to xstrcpy                                         */
#if (defined (DEBUG))
#  define xustrcpy_debug() { xustrcpy_file = __FILE__; xustrcpy_line = __LINE__;}
#else
#  define xustrcpy_debug()
#endif

extern char *xustrcpy_file;
extern word  xustrcpy_line;

/* Unicode File handle                                                       */

typedef struct {
    FILE * file;                  /* File handler                            */
    byte   format;                /* Format of file (UTF8, UTF16, UTF32)     */
    int    byte_order;            /* Byte order (big endian or little endian)*/
} UFILE;


/*- Function prototypes -----------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Unicode file functions                                                    */

UFILE *unicode_open            (const char *file_name, const char mode);
int    unicode_close           (UFILE *stream);
long   unicode_write           (UCODE *buffer, long size,     UFILE *stream);
long   unicode_read            (UCODE *buffer, long max_size, UFILE *stream);
UCODE  unicode_getc            (UFILE *stream);
int    unicode_fprintf         (UFILE *file, UCODE *format, ...);
byte   unicode_get_format      (UFILE *stream);
int    unicode_get_byteorder   (UFILE *stream);
void   unicode_set_format      (UFILE *stream, byte format, int byte_order);
byte   unicode_get_file_format (const char *file_name);
Bool   unicode_convert         (char *source_file, char *target_file,
                                byte target_format);

/* Unicode string function                                                   */

UCODE *mem_ustrdup             (const UCODE *source);
int    lexucmp                 (const UCODE *string1, const UCODE *string2);
int    lexuncmp                (const UCODE *string1, const UCODE *string2,
                                const int    count);
int    lexuwcmp                (const UCODE *string1, const UCODE *string2);
UCODE *usearchreplace          (UCODE *strbuf, UCODE *strtofnd, UCODE *strtoins);
UCODE *ustricstr               (const UCODE *str1, const UCODE *str2);
UCODE *ustrlwc                 (UCODE *string);
char  *ucode2ascii             (const UCODE *string);
char  *ucode2ascii_ex          (const UCODE *string, qbyte codepage);
UCODE *ascii2ucode             (const char  *string);
UCODE *ascii2ucode_ex          (const char  *string, qbyte codepage);
long   ucode2utf8_size         (const UCODE *string);
char  *ucode2utf8              (const UCODE *string);
long   utf82ucode_size         (const char  *string);
UCODE *utf82ucode              (const char  *string);

int    conv_ustr_bool          (const UCODE *string);
UCODE *ustrconvch              (UCODE *string, UCODE from, UCODE to);
DESCR *ustrt2descr             (UCODE **table);
UCODE**descr2ustrt             (const DESCR *descr);
void   ustrtfree               (UCODE **table);
UCODE *xustrcat                (UCODE *dest, const UCODE *src, ...);
UCODE *xustrcpy                (UCODE *dest, const UCODE *src, ...);

#ifdef __cplusplus
}
#endif

#endif
