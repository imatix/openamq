/*---------------------------------------------------------------------------
    icl_trace.h - icl_trace component

    Class to enable trace of program behaviour.
    To trace any program operation, call the trace method during operation.
    The trace is replayed via the dump method.
    Generated from icl_trace.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_TRACE_SAFE
#   define INCLUDE_ICL_TRACE_SAFE
#   define INCLUDE_ICL_TRACE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_TRACE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_trace_t icl_trace_t;

#define ICL_TRACE_ALIVE                 0xFABB
#define ICL_TRACE_DEAD                  0xDEAD

#define ICL_TRACE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_trace\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_TRACE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_trace at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_TRACE_ALIVE, self->object_tag);\
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
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_TRACE_UNSAFE
#   define INCLUDE_ICL_TRACE_UNSAFE
#   define INCLUDE_ICL_TRACE_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_trace_animating;
//  Macros

#define ICL_TRACE_SIZE 256

//  Types

typedef void (icl_trace_dump_fn) (
    icl_os_thread_t thread,
    apr_time_t      time,
    qbyte           info);

//  Structure to hold an individual trace item

typedef struct {
    icl_trace_dump_fn
        *dump;                          //  The function to interpret trace
#if (defined (BASE_THREADSAFE))
    apr_os_thread_t
        thread;                         //  The OS thread
#endif
    apr_time_t
        time;                           //  The time of the trace
    qbyte
        info;                           //  The trace information
} icl_trace_item_t;

//  These variables are accessed only from inline functions, so are
//  extern but also named s_ to mark their private nature.

extern icl_trace_t
    *s_icl_trace;

//  Structure of the icl_trace object

struct _icl_trace_t {
dbyte
    object_tag;                     //  Object validity marker
icl_trace_item_t
    *array;                         //  Array of trace items
uint
    size,                           //  Total size of trace
    idx;                            //  Current index in trace
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_trace_new(size)             icl_trace_new_ (__FILE__, __LINE__, size)
icl_trace_t *
    icl_trace_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
uint size                               //  Size for this trace
);

static inline void
    icl_trace_record (
icl_trace_t * self,                     //  The trace structure
icl_trace_dump_fn * dump,               //  The function to dump the trace
qbyte info                              //  Thd ID of the event, place or action
);

void
    icl_trace_dump (
icl_trace_t * self                      //  The trace structure
);

void
    icl_trace_terminate (
void);

void
    icl_trace_selftest (
void);

#define icl_trace_show(item,opcode,trace_file)  icl_trace_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_trace_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_trace_destroy(self)         icl_trace_destroy_ (self, __FILE__, __LINE__)
void
    icl_trace_destroy_ (
icl_trace_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_trace_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_trace_new_in_scope(self_p,_scope,size)  icl_trace_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, size)
void
    icl_trace_new_in_scope_ (
icl_trace_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
uint size                               //  Size for this trace
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_TRACE_INLINE)
#   define INCLUDE_ICL_TRACE_INLINE
#   define INCLUDE_ICL_TRACE_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_trace_record

    Type: Component method
    Call this method to register an event.
    
    On playback, the dump callback will be called with the
    thread and time of the trace call along with the info.
    -------------------------------------------------------------------------
 */

static inline void
    icl_trace_record (
    icl_trace_t * self,                 //  The trace structure
    icl_trace_dump_fn * dump,           //  The function to dump the trace
    qbyte info                          //  Thd ID of the event, place or action
)
{
uint
    idx;

    if (self == NULL) {
        if (!s_icl_trace)
            s_icl_trace = icl_trace_new (ICL_TRACE_SIZE);

        self = s_icl_trace;
    }

    idx = icl_atomic_inc32 ((qbyte*) &self->idx) % self->size;
    self->array[idx].dump = dump;
#if (defined (BASE_THREADSAFE))
    self->array[idx].thread      = apr_os_thread_current ();
#endif
    self->array[idx].time        = apr_time_now ();
    self->array[idx].info        = info;
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_TRACE_ROOT
#   endif
# endif
#endif
