/*---------------------------------------------------------------------------
    icl_apr.h - icl_apr component

    Provides iCL wrappers for some APR functions.
    Generated from icl_apr.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_APR_SAFE
#   define INCLUDE_ICL_APR_SAFE
#   define INCLUDE_ICL_APR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_APR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#include "base_apr.h"

//  Macros                                                                     

#if (defined (BASE_THREADSAFE))
#define icl_os_thread_t apr_os_thread_t
#else
#define icl_os_thread_t qbyte
#endif

//  Global variables                                                           

extern apr_pool_t *
    icl_global_pool;
extern apr_allocator_t *
    icl_global_allocator;

#if (defined (BASE_THREADSAFE))
extern apr_thread_mutex_t *
    icl_global_mutex;
extern apr_thread_mutex_t *
    icl_global_allocator_mutex;
#endif
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_APR_UNSAFE
#   define INCLUDE_ICL_APR_UNSAFE
#   define INCLUDE_ICL_APR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_apr_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_apr_assert(status)          icl_apr_assert_ (status, __FILE__, __LINE__)
static inline void
    icl_apr_assert_ (
apr_status_t status,                    //  The status code to print
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_apr_error_print(status)     icl_apr_error_print_ (status, __FILE__, __LINE__)
void
    icl_apr_error_print_ (
apr_status_t status,                    //  The status code to print
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline qbyte
    icl_atomic_inc32 (
volatile qbyte * mem                    //  The memory address
);

static inline Bool
    icl_atomic_dec32 (
volatile qbyte * mem                    //  The memory address
);

static inline qbyte
    icl_atomic_get32 (
volatile qbyte * mem                    //  The memory address
);

static inline void
    icl_atomic_set32 (
volatile qbyte * mem,                   //  The memory address
qbyte val                               //  The value
);

static inline qbyte
    icl_atomic_add32 (
volatile qbyte * mem,                   //  The memory address
qbyte add_val                           //  The value to add
);

static inline void
    icl_atomic_sub32 (
volatile qbyte * mem,                   //  The memory address
qbyte val                               //  The value to subtract
);

static inline qbyte
    icl_atomic_cas32 (
volatile qbyte * mem,                   //  The memory address
qbyte new_val,                          //  The new value
qbyte cmp_val                           //  The value to compare with
);

static inline volatile void *
    icl_atomic_casptr (
volatile void ** mem,                   //  The memory address
void * new_val,                         //  The new value
void * cmp_val                          //  The value to compare with
);

static inline qbyte
    icl_atomic_swap (
volatile qbyte * mem,                   //  The memory address
qbyte new_val                           //  The new value
);

void
    icl_selftest (
void);

void
    icl_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_APR_INLINE)
#   define INCLUDE_ICL_APR_INLINE
#   define INCLUDE_ICL_APR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_apr_assert

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_apr_assert_ (
    apr_status_t status,                //  The status code to print
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (status != APR_SUCCESS) {
    icl_apr_error_print_ (status, file, line);
    abort ();
}
}
/*  -------------------------------------------------------------------------
    icl_atomic_inc32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_inc32 (
    volatile qbyte * mem                //  The memory address
)
{
    qbyte
        val;                            //  The old value

#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_inc32 (mem);
#else
    val = (*mem)++;
#endif

    return (val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_dec32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline Bool
    icl_atomic_dec32 (
    volatile qbyte * mem                //  The memory address
)
{
    Bool
        non_zero;                       //  True if value becomes non-zero

#if (defined (BASE_THREADSAFE))
    non_zero = (apr_atomic_dec32 (mem) != 0);
#else
    non_zero = (--(*mem) != 0);
#endif

    return (non_zero);
}
/*  -------------------------------------------------------------------------
    icl_atomic_get32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_get32 (
    volatile qbyte * mem                //  The memory address
)
{
    qbyte
        val;                            //  The value

#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_read32 (mem);
#else
    val = *mem;
#endif

    return (val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_set32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_atomic_set32 (
    volatile qbyte * mem,               //  The memory address
    qbyte val                           //  The value
)
{

#if (defined (BASE_THREADSAFE))
    apr_atomic_set32 (mem, val);
#else
    *mem = val;
#endif
}
/*  -------------------------------------------------------------------------
    icl_atomic_add32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_add32 (
    volatile qbyte * mem,               //  The memory address
    qbyte add_val                       //  The value to add
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_add32 (mem, add_val);
#else
    old_val = *mem;
    *mem += add_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_sub32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_atomic_sub32 (
    volatile qbyte * mem,               //  The memory address
    qbyte val                           //  The value to subtract
)
{

#if (defined (BASE_THREADSAFE))
    apr_atomic_sub32 (mem, val);
#else
    *mem -= val;
#endif
}
/*  -------------------------------------------------------------------------
    icl_atomic_cas32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_cas32 (
    volatile qbyte * mem,               //  The memory address
    qbyte new_val,                      //  The new value
    qbyte cmp_val                       //  The value to compare with
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_cas32 (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_casptr

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline volatile void *
    icl_atomic_casptr (
    volatile void ** mem,               //  The memory address
    void * new_val,                     //  The new value
    void * cmp_val                      //  The value to compare with
)
{
    volatile void *
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_casptr (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_swap

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_swap (
    volatile qbyte * mem,               //  The memory address
    qbyte new_val                       //  The new value
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_xchg32 (mem, new_val);
#else
    old_val = *mem;
    *mem = new_val;
#endif

    return (old_val);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_APR_ROOT
#   endif
# endif
#endif
