/*---------------------------------------------------------------------------
    smt_method.h - smt_method component

    The smt_method class is used to send and receive 'replies' (queued events
    with result code and data) for smt threads.
    Generated from smt_method.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_METHOD_SAFE
#   define INCLUDE_SMT_METHOD_SAFE
#   define INCLUDE_SMT_METHOD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_METHOD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_method_t smt_method_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
#    define SMT_METHOD_HISTORY_LENGTH 32
#endif

#define SMT_METHOD_ALIVE                0xFABB
#define SMT_METHOD_DEAD                 0xDEAD

#define SMT_METHOD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_method\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_METHOD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_method at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_METHOD_ALIVE, self->object_tag);\
        smt_method_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
typedef int (smt_method_destructor_fn) (void *data);
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_method_queue.h"
#   include "smt_method_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_METHOD_ACTIVE
#   if defined (ICL_IMPORT_SMT_METHOD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_METHOD_UNSAFE
#   define INCLUDE_SMT_METHOD_UNSAFE
#   define INCLUDE_SMT_METHOD_ACTIVE

#   include "smt_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_method_animating;

//  Structure of the smt_method object

struct _smt_method_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [SMT_METHOD_HISTORY_LENGTH];
    int
        history_line  [SMT_METHOD_HISTORY_LENGTH];
    char
        *history_type [SMT_METHOD_HISTORY_LENGTH];
    int
        history_links [SMT_METHOD_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile smt_method_queue_t
    *queue_head;
volatile smt_method_t
    *queue_next;
volatile smt_method_list_t *list_head;
volatile smt_method_t *list_prev;
volatile smt_method_t *list_next;
smt_event_t 
    event;                          //  The event to deliver
smt_priority_t
    priority;                       //  The priority for the method
int
    result;                         //  The result
void
    *data;                          //  Opaque data
smt_method_destructor_fn
    *destructor;                    //  Data destructor function
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_method_queue.h"
#   include "smt_method_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    smt_method_send (
smt_method_queue_t * queue,             //  Queue to send reply to
smt_event_t event,                      //  Internal event number
smt_priority_t priority,                //  The priority for the event
int result,                             //  Result to return
void * data,                            //  Data pointer
smt_method_destructor_fn * destructor   //  Data destructor callbak
);

void
    smt_method_selftest (
void);

#define smt_method_new()                smt_method_new_ (__FILE__, __LINE__)
smt_method_t *
    smt_method_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_method_remove_from_all_containers (
smt_method_t * self                     //  The itemThe item
);

#define smt_method_show(item,opcode,trace_file)  smt_method_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_method_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    smt_method_terminate (
void);

#define smt_method_destroy(self)        smt_method_destroy_ (self, __FILE__, __LINE__)
void
    smt_method_destroy_ (
smt_method_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define smt_method_link(self)           smt_method_link_ (self, __FILE__, __LINE__)
smt_method_t *
    smt_method_link_ (
smt_method_t * self,                    //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_method_unlink(self)         smt_method_unlink_ (self, __FILE__, __LINE__)
void
    smt_method_unlink_ (
smt_method_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_method_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_method_new_in_scope(self_p,_scope)  smt_method_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    smt_method_new_in_scope_ (
smt_method_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_METHOD_ACTIVE
#   if defined (ICL_IMPORT_SMT_METHOD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_METHOD_INLINE)
#   define INCLUDE_SMT_METHOD_INLINE
#   define INCLUDE_SMT_METHOD_ACTIVE

#   include "smt_thread.h"
#   include "smt_method_queue.h"
#   include "smt_method_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_METHOD_ACTIVE
#   if defined (ICL_IMPORT_SMT_METHOD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_METHOD_ROOT
#   endif
# endif
#endif
