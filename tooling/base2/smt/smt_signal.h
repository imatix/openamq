/*---------------------------------------------------------------------------
    smt_signal.h - smt_signal component

    The smt_signal class is used to manage signal handlers in an SMT
    application.
    Generated from smt_signal.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_SIGNAL_SAFE
#   define INCLUDE_SMT_SIGNAL_SAFE
#   define INCLUDE_SMT_SIGNAL_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_SIGNAL_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_signal_t smt_signal_t;

#define SMT_SIGNAL_ALIVE                0xFABB
#define SMT_SIGNAL_DEAD                 0xDEAD

#define SMT_SIGNAL_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_signal\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_SIGNAL_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_signal at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_SIGNAL_ALIVE, self->object_tag);\
        smt_signal_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_signal_table.h"
#   include "smt_signal_by_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SIGNAL_ACTIVE
#   if defined (ICL_IMPORT_SMT_SIGNAL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_SIGNAL_UNSAFE
#   define INCLUDE_SMT_SIGNAL_UNSAFE
#   define INCLUDE_SMT_SIGNAL_ACTIVE

#   include "smt_thread.h"
#   include "smt_signal_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_signal_animating;

//  Structure of the smt_signal object

struct _smt_signal_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_signal_table_t
    *table_head;                    //  Hash table container
smt_signal_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
int
    key;                            //  Item's original key
volatile smt_signal_by_thread_t *by_thread_head;
volatile smt_signal_t *by_thread_prev;
volatile smt_signal_t *by_thread_next;
smt_event_t
    event;                          //  The event to deliver on signal
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_signal_by_thread.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    smt_signal_initialise (
void);

int
    smt_signal_register (
int the_signal,                         //  The signal to register
smt_thread_t * thread,                  //  The thread to register the signal with
smt_event_t event                       //  The event to be sent on signal
);

void
    smt_signal_register_shut_down (
int the_signal                          //  The signal to register
);

void
    smt_signal_deregister (
int the_signal                          //  The signal to deregister
);

void
    smt_signal_selftest (
void);

void
    smt_signal_remove_from_all_containers (
smt_signal_t * self                     //  The itemThe item
);

#define smt_signal_show(item,opcode,trace_file)  smt_signal_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_signal_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_signal_destroy(self)        smt_signal_destroy_ (self, __FILE__, __LINE__)
void
    smt_signal_destroy_ (
smt_signal_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    smt_signal_read_lock (
smt_signal_t * self                     //  Reference to object
);

int
    smt_signal_write_lock (
smt_signal_t * self                     //  Reference to object
);

int
    smt_signal_unlock (
smt_signal_t * self                     //  Reference to object
);

void
    smt_signal_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SIGNAL_ACTIVE
#   if defined (ICL_IMPORT_SMT_SIGNAL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_SIGNAL_INLINE)
#   define INCLUDE_SMT_SIGNAL_INLINE
#   define INCLUDE_SMT_SIGNAL_ACTIVE

#   include "smt_thread.h"
#   include "smt_signal_table.h"
#   include "smt_signal_by_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SIGNAL_ACTIVE
#   if defined (ICL_IMPORT_SMT_SIGNAL_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_SIGNAL_ROOT
#   endif
# endif
#endif
