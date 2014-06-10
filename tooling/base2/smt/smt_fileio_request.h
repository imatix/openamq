/*---------------------------------------------------------------------------
    smt_fileio_request.h - smt_fileio_request component

    The smt_fileio_request class contains all the methods for doing activity
    on an open file.
    Generated from smt_fileio_request.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_FILEIO_REQUEST_SAFE
#   define INCLUDE_SMT_FILEIO_REQUEST_SAFE
#   define INCLUDE_SMT_FILEIO_REQUEST_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_FILEIO_REQUEST_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#include "base_apr.h"
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_fileio.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_FILEIO_REQUEST_UNSAFE
#   define INCLUDE_SMT_FILEIO_REQUEST_UNSAFE
#   define INCLUDE_SMT_FILEIO_REQUEST_ACTIVE

#   include "smt_thread.h"
#   include "smt_fileio.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_fileio_request_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    smt_fileio_request_read (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_fileio_t * file,                    //  Open file to read data from
size_t min_size,                        //  Minimum data size to read
size_t max_size,                        //  Maximum data size to read
byte * buffer,                          //  Buffer to store data read
smt_event_t event                       //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
);

int
    smt_fileio_request_write (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_fileio_t * file,                    //  Open file to read data from
size_t size,                            //  Data size to write
byte * buffer,                          //  Buffer to store data read
smt_event_t event                       //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
);

int
    smt_fileio_request_close (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_fileio_t * file,                    //  Open file to read data from
smt_event_t event                       //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
);

int
    smt_fileio_request_seek (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_fileio_t * file,                    //  Open file to read data from
size_t offset,                          //  Position to seek to
smt_event_t event                       //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
);

void
    smt_fileio_request_selftest (
void);

void
    smt_fileio_request_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_FILEIO_REQUEST_INLINE)
#   define INCLUDE_SMT_FILEIO_REQUEST_INLINE
#   define INCLUDE_SMT_FILEIO_REQUEST_ACTIVE

#   include "smt_thread.h"
#   include "smt_fileio.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_REQUEST_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_FILEIO_REQUEST_ROOT
#   endif
# endif
#endif
