/*---------------------------------------------------------------------------
    icl_demo_rwlock.h - icl_demo_rwlock component

    Tests and demonstrates the iCL class framework.  This class creates
    a file object that you can open, close, and work with.
    Generated from icl_demo_rwlock.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_DEMO_RWLOCK_SAFE
#   define INCLUDE_ICL_DEMO_RWLOCK_SAFE
#   define INCLUDE_ICL_DEMO_RWLOCK_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_DEMO_RWLOCK_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_demo_rwlock_t icl_demo_rwlock_t;

#define ICL_DEMO_RWLOCK_ALIVE           0xFABB
#define ICL_DEMO_RWLOCK_DEAD            0xDEAD

#define ICL_DEMO_RWLOCK_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_demo_rwlock\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_DEMO_RWLOCK_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_demo_rwlock at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_DEMO_RWLOCK_ALIVE, self->object_tag);\
        icl_demo_rwlock_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_DEMO_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_DEMO_RWLOCK_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_DEMO_RWLOCK_UNSAFE
#   define INCLUDE_ICL_DEMO_RWLOCK_UNSAFE
#   define INCLUDE_ICL_DEMO_RWLOCK_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_demo_rwlock_animating;

//  Structure of the icl_demo_rwlock object

struct _icl_demo_rwlock_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
FILE *handle;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_demo_rwlock_new(filename)   icl_demo_rwlock_new_ (__FILE__, __LINE__, filename)
icl_demo_rwlock_t *
    icl_demo_rwlock_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename                         //  Name of file to open
);

int
    icl_demo_rwlock_write (
icl_demo_rwlock_t * self,               //  Reference to object
byte * data,                            //  Pointer to data to write
size_t size                             //  Amount of data, in octets
);

int
    icl_demo_rwlock_read (
icl_demo_rwlock_t * self,               //  Reference to object
byte * data,                            //  Pointer to receive data
size_t size                             //  Maximum amount to read, in octets
);

int
    icl_demo_rwlock_reset (
icl_demo_rwlock_t * self                //  Reference to object
);

void
    icl_demo_rwlock_selftest (
void);

void
    icl_demo_rwlock_terminate (
void);

#define icl_demo_rwlock_show(item,opcode,trace_file)  icl_demo_rwlock_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_demo_rwlock_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_demo_rwlock_destroy(self)   icl_demo_rwlock_destroy_ (self, __FILE__, __LINE__)
void
    icl_demo_rwlock_destroy_ (
icl_demo_rwlock_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    icl_demo_rwlock_read_lock (
icl_demo_rwlock_t * self                //  Reference to object
);

int
    icl_demo_rwlock_write_lock (
icl_demo_rwlock_t * self                //  Reference to object
);

int
    icl_demo_rwlock_unlock (
icl_demo_rwlock_t * self                //  Reference to object
);

void
    icl_demo_rwlock_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_demo_rwlock_new_in_scope(self_p,_scope,filename)  icl_demo_rwlock_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, filename)
void
    icl_demo_rwlock_new_in_scope_ (
icl_demo_rwlock_t * * self_p,           //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename                         //  Name of file to open
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_DEMO_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_DEMO_RWLOCK_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_DEMO_RWLOCK_INLINE)
#   define INCLUDE_ICL_DEMO_RWLOCK_INLINE
#   define INCLUDE_ICL_DEMO_RWLOCK_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_DEMO_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_DEMO_RWLOCK_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_DEMO_RWLOCK_ROOT
#   endif
# endif
#endif
