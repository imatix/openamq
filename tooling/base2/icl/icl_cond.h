/*---------------------------------------------------------------------------
    icl_cond.h - icl_cond component

    Defines wrappers to manage condition variable objects.
    Generated from icl_cond.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_COND_SAFE
#   define INCLUDE_ICL_COND_SAFE
#   define INCLUDE_ICL_COND_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_COND_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_cond_t icl_cond_t;

#define ICL_COND_ALIVE                  0xFABB
#define ICL_COND_DEAD                   0xDEAD

#define ICL_COND_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_cond\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_COND_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_cond at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_COND_ALIVE, self->object_tag);\
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
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_COND_UNSAFE
#   define INCLUDE_ICL_COND_UNSAFE
#   define INCLUDE_ICL_COND_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_cond_animating;

//  Structure of the icl_cond object

struct _icl_cond_t {
dbyte
    object_tag;                     //  Object validity marker
#if (defined (BASE_THREADSAFE))
    apr_thread_cond_t
        *cond;
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
static inline int
    icl_cond_signal (
icl_cond_t * self                       //  Reference to object
);

static inline int
    icl_cond_broadcast (
icl_cond_t * self                       //  Reference to object
);

static inline int
    icl_cond_wait (
icl_cond_t * self,                      //  Reference to object
icl_mutex_t * mutex_p                   //  The mutex pointer
);

static inline int
    icl_cond_timed_wait (
icl_cond_t * self,                      //  Reference to object
icl_mutex_t * mutex_p,                  //  The mutex pointer
int64_t usecs                           //  Timeout for wait
);

void
    icl_cond_selftest (
void);

void
    icl_cond_terminate (
void);

#define icl_cond_new()                  icl_cond_new_ (__FILE__, __LINE__)
icl_cond_t *
    icl_cond_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_cond_show(item,opcode,trace_file)  icl_cond_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_cond_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_cond_destroy(self)          icl_cond_destroy_ (self, __FILE__, __LINE__)
void
    icl_cond_destroy_ (
icl_cond_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_cond_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_cond_new_in_scope(self_p,_scope)  icl_cond_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_cond_new_in_scope_ (
icl_cond_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_COND_INLINE)
#   define INCLUDE_ICL_COND_INLINE
#   define INCLUDE_ICL_COND_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_cond_signal

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_signal (
    icl_cond_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_signal (self->cond));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_broadcast

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_broadcast (
    icl_cond_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_broadcast (self->cond));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_wait

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_wait (
    icl_cond_t * self,                  //  Reference to object
    icl_mutex_t * mutex_p               //  The mutex pointer
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    icl_apr_assert (apr_thread_cond_wait (self->cond, mutex_p->mutex));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_timed_wait

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    Returns -1 on timeout; zero otherwise.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_timed_wait (
    icl_cond_t * self,                  //  Reference to object
    icl_mutex_t * mutex_p,              //  The mutex pointer
    int64_t usecs                       //  Timeout for wait
)
{
#if (defined (BASE_THREADSAFE))
    apr_status_t
        status;
#endif
    int
        rc = 0;                         //  Return codeReturn code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    status = apr_thread_cond_timedwait (self->cond, mutex_p->mutex, usecs);
    if (APR_STATUS_IS_TIMEUP (status)) {
        status = APR_SUCCESS;
        rc = -1;
    }
    icl_apr_assert (status);
#endif


    return (rc);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_COND_ROOT
#   endif
# endif
#endif
