/*---------------------------------------------------------------------------
    ipr_log.h - ipr_log component

    Generated from ipr_log.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_LOG_SAFE
#   define INCLUDE_IPR_LOG_SAFE
#   define INCLUDE_IPR_LOG_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_LOG_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Define portable syslog priority constants
//  We use the values defined for syslog when implemented
#define IPR_LOG_SYSERROR    3
#define IPR_LOG_SYSWARNING  4
#define IPR_LOG_SYSINFO     6
#define IPR_LOG_SYSDEBUG    7
#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_LOG_UNSAFE
#   define INCLUDE_IPR_LOG_UNSAFE
#   define INCLUDE_IPR_LOG_ACTIVE

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_log_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    ipr_log_cycle (
char * filename,                        //  File to examine
char * directory,                       //  Archive directory, or NULL
char * command                          //  Command to run
);

int
    ipr_log_sysopen (
char * identifier                       //  Log identifier
);

void
    ipr_log_sysprint (
int priority,                           //  Log priority
char * string                           //  String to log
);

void
    ipr_log_sysclose (
void);

void
    ipr_log_selftest (
void);

void
    ipr_log_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_LOG_INLINE)
#   define INCLUDE_IPR_LOG_INLINE
#   define INCLUDE_IPR_LOG_ACTIVE

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_LOG_ROOT
#   endif
# endif
#endif
