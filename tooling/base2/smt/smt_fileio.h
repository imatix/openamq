/*---------------------------------------------------------------------------
    smt_fileio.h - smt_fileio component

    The smt_fileio class is used to create socket objects for use with the
    smt_fileio_request class.
    Generated from smt_fileio.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_FILEIO_SAFE
#   define INCLUDE_SMT_FILEIO_SAFE
#   define INCLUDE_SMT_FILEIO_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_FILEIO_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_fileio_t smt_fileio_t;

#define SMT_FILEIO_ALIVE                0xFABB
#define SMT_FILEIO_DEAD                 0xDEAD

#define SMT_FILEIO_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_fileio\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_FILEIO_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_fileio at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_FILEIO_ALIVE, self->object_tag);\
        smt_fileio_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#include "base_apr.h"

//  Types                                                                      

typedef enum {
    SMT_FILE_READ   = 1,
    SMT_FILE_WRITE  = 2,
    SMT_FILE_APPEND = 3
} smt_file_mode_e;
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_fileio_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_FILEIO_UNSAFE
#   define INCLUDE_SMT_FILEIO_UNSAFE
#   define INCLUDE_SMT_FILEIO_ACTIVE

#   include "smt_thread.h"
#   include "smt_fileio_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_fileio_animating;

//  Structure of the smt_fileio object

struct _smt_fileio_t {
dbyte
    object_tag;                     //  Object validity marker
apr_pool_t
    *pool;
apr_file_t
    *handle;
size_t
    io_size;
Bool
    eof;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
smt_fileio_t *
    smt_fileio_open (
smt_thread_t * thread,                  //  Thread to deliver reply to
const char * path,                      //  The optional path???
const char * name,                      //  The name of the file to open
smt_file_mode_e mode,                   //  The mode (read/write/append)
Bool binary,                            //  Open file for binary data?
smt_event_t event                       //  Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking
);

smt_fileio_t *
    smt_fileio_temp (
smt_thread_t * thread,                  //  Thread to deliver reply to
const char * path,                      //  The optional path???
Bool binary,                            //  Open file for binary data?
smt_event_t event                       //  Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking
);

Bool
    smt_fileio_eof (
smt_fileio_t * self                     //  The fileio item
);

void
    smt_fileio_selftest (
void);

void
    smt_fileio_terminate (
void);

#define smt_fileio_show(item,opcode,trace_file)  smt_fileio_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_fileio_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_fileio_destroy(self)        smt_fileio_destroy_ (self, __FILE__, __LINE__)
void
    smt_fileio_destroy_ (
smt_fileio_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    smt_fileio_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_FILEIO_INLINE)
#   define INCLUDE_SMT_FILEIO_INLINE
#   define INCLUDE_SMT_FILEIO_ACTIVE

#   include "smt_thread.h"
#   include "smt_fileio_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_FILEIO_ACTIVE
#   if defined (ICL_IMPORT_SMT_FILEIO_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_FILEIO_ROOT
#   endif
# endif
#endif
