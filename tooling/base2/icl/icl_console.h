/*---------------------------------------------------------------------------
    icl_console.h - icl_console component

    Defines the console interface for iCL.
    Generated from icl_console.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_CONSOLE_SAFE
#   define INCLUDE_ICL_CONSOLE_SAFE
#   define INCLUDE_ICL_CONSOLE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_CONSOLE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#include "base_apr.h"

//- Constants ---------------------------------------------------------------

typedef enum {
    ICL_CONSOLE_DATE,
    ICL_CONSOLE_TIME,
    ICL_CONSOLE_MILLI,
    ICL_CONSOLE_MICRO,
    ICL_CONSOLE_THREAD,
    ICL_CONSOLE_FLUSH,
    ICL_CONSOLE_QUIET
} icl_console_option_t;

//  Shortcuts

#define _icp        icl_console_print
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_CONSOLE_UNSAFE
#   define INCLUDE_ICL_CONSOLE_UNSAFE
#   define INCLUDE_ICL_CONSOLE_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_console_mode (
icl_console_option_t attr,              //  The option to set or clear
Bool value                              //  The value for the option
);

void
    icl_console_redirect (
FILE * file                             //  The stream to redirect the console to
);

void
    icl_console_print (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_print_v (
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_out (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_out_v (
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_print_thread_time (
icl_os_thread_t os_thread,              //  The thread info to print
apr_time_t time,                        //  The date/time to print
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_print_thread_time_v (
icl_os_thread_t os_thread,              //  The thread info to print
apr_time_t time,                        //  The date/time to print
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_selftest (
void);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_CONSOLE_INLINE)
#   define INCLUDE_ICL_CONSOLE_INLINE
#   define INCLUDE_ICL_CONSOLE_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_CONSOLE_ROOT
#   endif
# endif
#endif
