/*---------------------------------------------------------------------------
    ipr_str.h - ipr_str component

    Provides various string manipulation functions.
    Generated from ipr_str.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_STR_SAFE
#   define INCLUDE_IPR_STR_SAFE
#   define INCLUDE_IPR_STR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_STR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_STR_UNSAFE
#   define INCLUDE_IPR_STR_UNSAFE
#   define INCLUDE_IPR_STR_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_str_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
char *
    ipr_str_skip (
char * string                           //  Input string
);

char *
    ipr_str_wipe (
char * string,                          //  Input string
char ch                                 //  Character to use
);

char *
    ipr_str_lower (
char * string                           //  Input string
);

char *
    ipr_str_upper (
char * string                           //  Input string
);

char *
    ipr_str_subch (
char * string,                          //  Input string
char from,                              //  Character to look for
char to                                 //  Character to replace with
);

char *
    ipr_str_crop (
char * string                           //  Input string
);

char *
    ipr_str_block (
char * string,                          //  Input string
char content,                           //  Character to repeat
size_t count                            //  Number of repetitions
);

char *
    ipr_str_format (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    ipr_str_format_v (
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

char *
    ipr_str_filter (
char * string                           //  Input string
);

int
    ipr_str_match (
char * string1,                         //  First string
char * string2                          //  Second string
);

Bool
    ipr_str_prefixed (
char * string,                          //  Input string
char * prefix                           //  Prefix string
);

char *
    ipr_str_defix (
char * string,                          //  Input string
char * prefix                           //  Prefix to remove
);

char *
    ipr_str_clean (
char * string,                          //  Input string
char from,                              //  Char to look for
char to                                 //  Char to replace with
);

Bool
    ipr_str_lexeq (
char * string1,                         //  Input string 1
char * string2                          //  Input string 2
);

int
    ipr_str_lexcmp (
char * string1,                         //  Input string 1
char * string2                          //  Input string 2
);

char *
    ipr_str_soundex (
char * string,                          //  Input string
char * encoding                         //  Resulting soundex
);

Bool
    ipr_str_matches (
char * string,                          //  Input string
char * pattern                          //  Input pattern
);

Bool
    ipr_str_lex_matches (
char * string,                          //  Input string
char * pattern                          //  Input pattern
);

char *
    ipr_str_find (
char * haystack,                        //  String to search in
char * needle                           //  String to search for
);

char *
    ipr_str_random (
char * string,                          //  String to fill
char * pattern                          //  Pattern
);

Bool
    ipr_str_numeric (
char * string                           //  Input string 1
);

char *
    ipr_str_md5 (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    ipr_str_md5_v (
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

void
    ipr_str_selftest (
void);

void
    ipr_str_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_STR_INLINE)
#   define INCLUDE_IPR_STR_INLINE
#   define INCLUDE_IPR_STR_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_STR_ROOT
#   endif
# endif
#endif
