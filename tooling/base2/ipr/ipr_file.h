/*---------------------------------------------------------------------------
    ipr_file.h - ipr_file component

Provides a general set of file and filename manipulation functions that
return information about a file specified by name. The maximum length of
a filename is set at 255 characters, to fit in an icl_shortstr_t. All
filenames should be defined using this type.
    Generated from ipr_file.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_FILE_SAFE
#   define INCLUDE_IPR_FILE_SAFE
#   define INCLUDE_IPR_FILE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_FILE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_FILE_UNSAFE
#   define INCLUDE_IPR_FILE_UNSAFE
#   define INCLUDE_IPR_FILE_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_file_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
Bool
    ipr_file_exists (
char * filename                         //  File to examine
);

int64_t
    ipr_file_size (
char * filename                         //  File to measure
);

apr_time_t
    ipr_file_time (
char * filename                         //  File to check
);

size_t
    ipr_file_lines (
char * filename                         //  File to check
);

apr_ino_t
    ipr_file_inode (
char * filename                         //  File to check
);

ipr_bucket_t *
    ipr_file_slurp (
char * filename                         //  File to read
);

int
    ipr_file_where (
char * filename,                        //  File to locate
char * path,                            //  Path to use
char * fullname                         //  Resulting name
);

int
    ipr_file_delete (
char * filename                         //  File to delete
);

int
    ipr_file_rename (
char * newname,                         //  Rename file to this
char * oldname                          //  Actual name of file
);

void
    ipr_file_truncate (
char * filename                         //  File to truncate
);

char *
    ipr_file_strip_path (
char * filename                         //  Filename to process
);

char *
    ipr_file_set_path (
char * filename,                        //  Filename to process
char * path                             //  Desired path
);

char *
    ipr_file_set_extension (
char * filename,                        //  Filename to process
char * extension                        //  Extension to add
);

char *
    ipr_file_move (
char * directory,                       //  Desired directory path
char * filename                         //  Filename to process
);

Bool
    ipr_file_is_readable (
char * filename                         //  File to examine
);

Bool
    ipr_file_is_writeable (
char * filename                         //  File to examine
);

Bool
    ipr_file_is_directory (
char * filename                         //  File to examine
);

ipr_finfo_t *
    ipr_file_get_info (
char * filename                         //  File to examine
);

char *
    ipr_file_digest (
char * filename                         //  File to digest
);

char *
    ipr_file_tmpfile (
void);

char *
    ipr_file_etag (
char * filename,                        //  File to digest
char * type                             //  ETag type: fixed, float
);

void
    ipr_file_selftest (
void);

void
    ipr_file_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_FILE_INLINE)
#   define INCLUDE_IPR_FILE_INLINE
#   define INCLUDE_IPR_FILE_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_FILE_ROOT
#   endif
# endif
#endif
