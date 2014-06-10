/*---------------------------------------------------------------------------
    icl_shortstr.h - icl_shortstr component

    We define the icl_shortstr_t type in order to provide a simple way of
    storing and serialising short strings.  A "short string" is defined
    as one that can be serialised as a single-octet length indicator
    plus 0-255 octets of data.  The icl_shortstr_t type is held in memory
    as C-compatible null-terminated char arrays of 255+1 bytes.  There
    are no constructor/destructor methods for icl_shortstr_t variables since
    this would create incompatabilities with normal C strings.  See the
    selftest method for examples of using icl_shortstr_t.  When writing a
    icl_shortstr_t you should always use the icl_shortstr_cpy method to
    avoid the risk of overflows.
    Generated from icl_shortstr.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_SHORTSTR_SAFE
#   define INCLUDE_ICL_SHORTSTR_SAFE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_SHORTSTR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#define ICL_SHORTSTR_MAX    255
typedef char icl_shortstr_t [ICL_SHORTSTR_MAX + 1];
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_SHORTSTR_UNSAFE
#   define INCLUDE_ICL_SHORTSTR_UNSAFE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_shortstr_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
size_t
    icl_shortstr_cpy (
char * dest,                            //  String to copy into
char * src                              //  String to copy from
);

size_t
    icl_shortstr_cat (
char * dest,                            //  String to copy into
char * src                              //  String to copy from
);

size_t
    icl_shortstr_ncpy (
char * dest,                            //  String to copy into
char * src,                             //  String to copy from
size_t size                             //  Length of source string
);

size_t
    icl_shortstr_ncat (
char * dest,                            //  String to copy into
char * src,                             //  String to copy from
size_t size                             //  Length of source string
);

char *
    icl_shortstr_fmt (
char * dest,                            //  String to format into
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    icl_shortstr_fmt_v (
char * dest,                            //  String to format into
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

size_t
    icl_shortstr_read (
char * string,                          //  Short string to fill
FILE * file                             //  File to read from
);

void
    icl_shortstr_selftest (
void);

void
    icl_shortstr_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_SHORTSTR_INLINE)
#   define INCLUDE_ICL_SHORTSTR_INLINE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_SHORTSTR_ROOT
#   endif
# endif
#endif
