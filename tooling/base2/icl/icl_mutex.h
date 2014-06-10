/*---------------------------------------------------------------------------
    icl_mutex.h - icl_mutex component

    Defines wrappers to manage mutex objects.
    Generated from icl_mutex.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_MUTEX_SAFE
#   define INCLUDE_ICL_MUTEX_SAFE
#   define INCLUDE_ICL_MUTEX_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_MUTEX_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_mutex_t icl_mutex_t;

#define ICL_MUTEX_ALIVE                 0xFABB
#define ICL_MUTEX_DEAD                  0xDEAD

#define ICL_MUTEX_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_mutex\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_MUTEX_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_mutex at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_MUTEX_ALIVE, self->object_tag);\
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
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_MUTEX_UNSAFE
#   define INCLUDE_ICL_MUTEX_UNSAFE
#   define INCLUDE_ICL_MUTEX_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_mutex_animating;

//  Structure of the icl_mutex object

struct _icl_mutex_t {
dbyte
    object_tag;                     //  Object validity marker
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_t
        *mutex;
    apr_pool_t
        *pool;
#else
    qbyte
        filler;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline void
    icl_mutex_lock (
icl_mutex_t * self                      //  Reference to object
);

static inline void
    icl_mutex_unlock (
icl_mutex_t * self                      //  Reference to object
);

void
    icl_mutex_selftest (
void);

void
    icl_mutex_terminate (
void);

#define icl_mutex_new()                 icl_mutex_new_ (__FILE__, __LINE__)
icl_mutex_t *
    icl_mutex_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mutex_show(item,opcode,trace_file)  icl_mutex_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_mutex_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_mutex_destroy(self)         icl_mutex_destroy_ (self, __FILE__, __LINE__)
void
    icl_mutex_destroy_ (
icl_mutex_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_mutex_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_mutex_new_in_scope(self_p,_scope)  icl_mutex_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_mutex_new_in_scope_ (
icl_mutex_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_MUTEX_INLINE)
#   define INCLUDE_ICL_MUTEX_INLINE
#   define INCLUDE_ICL_MUTEX_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_mutex_lock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_mutex_lock (
    icl_mutex_t * self                  //  Reference to object
)
{

    ICL_MUTEX_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_lock (self->mutex));
#endif
}
/*  -------------------------------------------------------------------------
    icl_mutex_unlock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_mutex_unlock (
    icl_mutex_t * self                  //  Reference to object
)
{

    ICL_MUTEX_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_unlock (self->mutex));
#endif
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_MUTEX_ROOT
#   endif
# endif
#endif
