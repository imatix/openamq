/*---------------------------------------------------------------------------
    smt_demo_object.h - smt_demo_object component

This object is a demonstration of SMT objects.
    Generated from smt_demo_object.icl by smt_object_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_DEMO_OBJECT_SAFE
#   define INCLUDE_SMT_DEMO_OBJECT_SAFE
#   define INCLUDE_SMT_DEMO_OBJECT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_DEMO_OBJECT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_demo_object_t smt_demo_object_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_OBJECT)
#    define SMT_DEMO_OBJECT_HISTORY_LENGTH 32
#endif

#define SMT_DEMO_OBJECT_ALIVE           0xFABB
#define SMT_DEMO_OBJECT_DEAD            0xDEAD

#define SMT_DEMO_OBJECT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_demo_object\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_DEMO_OBJECT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_demo_object at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_DEMO_OBJECT_ALIVE, self->object_tag);\
        smt_demo_object_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "smt_demo_object_agent.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_DEMO_OBJECT_ACTIVE
#   if defined (ICL_IMPORT_SMT_DEMO_OBJECT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_DEMO_OBJECT_UNSAFE
#   define INCLUDE_SMT_DEMO_OBJECT_UNSAFE
#   define INCLUDE_SMT_DEMO_OBJECT_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "smt_demo_object_agent.h"
//  Global variables

extern Bool
    smt_demo_object_animating;

//  Structure of the smt_demo_object object

struct _smt_demo_object_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_OBJECT)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [SMT_DEMO_OBJECT_HISTORY_LENGTH];
    int
        history_line  [SMT_DEMO_OBJECT_HISTORY_LENGTH];
    char
        *history_type [SMT_DEMO_OBJECT_HISTORY_LENGTH];
    int
        history_links [SMT_DEMO_OBJECT_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;
char
    *string;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define smt_demo_object_new(string)     smt_demo_object_new_ (__FILE__, __LINE__, string)
smt_demo_object_t *
    smt_demo_object_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * string                           //  Not documented
);

int
    smt_demo_object_print (
smt_demo_object_t * self,               //  Reference to object
char * prefix                           //  Not documented
);

void
    smt_demo_object_selftest (
void);

#define smt_demo_object_destroy(self)   smt_demo_object_destroy_ (self, __FILE__, __LINE__)
int
    smt_demo_object_destroy_ (
smt_demo_object_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_demo_object_unlink(self)    smt_demo_object_unlink_ (self, __FILE__, __LINE__)
void
    smt_demo_object_unlink_ (
smt_demo_object_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_demo_object_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_demo_object_free(self)      smt_demo_object_free_ (self, __FILE__, __LINE__)
void
    smt_demo_object_free_ (
smt_demo_object_t * self,               //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    smt_demo_object_terminate (
void);

#define smt_demo_object_show(item,opcode,trace_file)  smt_demo_object_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_demo_object_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_demo_object_link(self)      smt_demo_object_link_ (self, __FILE__, __LINE__)
smt_demo_object_t *
    smt_demo_object_link_ (
smt_demo_object_t * self,               //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_demo_object_new_in_scope(self_p,_scope,string)  smt_demo_object_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, string)
void
    smt_demo_object_new_in_scope_ (
smt_demo_object_t * * self_p,           //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * string                           //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_DEMO_OBJECT_ACTIVE
#   if defined (ICL_IMPORT_SMT_DEMO_OBJECT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_DEMO_OBJECT_INLINE)
#   define INCLUDE_SMT_DEMO_OBJECT_INLINE
#   define INCLUDE_SMT_DEMO_OBJECT_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "smt_demo_object_agent.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_DEMO_OBJECT_ACTIVE
#   if defined (ICL_IMPORT_SMT_DEMO_OBJECT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_DEMO_OBJECT_ROOT
#   endif
# endif
#endif
