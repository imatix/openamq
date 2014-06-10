/*---------------------------------------------------------------------------
    smt_position.h - smt_position component

    The smt_position class is used to build a stack of 'positions' in an smt
    dialog.  A position consists of a 'place' (identifies thread/event/action)
    and a 'module' (action number within a given 'place').
    Generated from smt_position.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_POSITION_SAFE
#   define INCLUDE_SMT_POSITION_SAFE
#   define INCLUDE_SMT_POSITION_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_POSITION_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_position_t smt_position_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
#    define SMT_POSITION_HISTORY_LENGTH 32
#endif

#define SMT_POSITION_ALIVE              0xFABB
#define SMT_POSITION_DEAD               0xDEAD

#define SMT_POSITION_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_position\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_POSITION_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_position at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_POSITION_ALIVE, self->object_tag);\
        smt_position_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_position_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_POSITION_ACTIVE
#   if defined (ICL_IMPORT_SMT_POSITION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_POSITION_UNSAFE
#   define INCLUDE_SMT_POSITION_UNSAFE
#   define INCLUDE_SMT_POSITION_ACTIVE

#   include "smt_thread.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_position_animating;

//  Structure of the smt_position object

struct _smt_position_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [SMT_POSITION_HISTORY_LENGTH];
    int
        history_line  [SMT_POSITION_HISTORY_LENGTH];
    char
        *history_type [SMT_POSITION_HISTORY_LENGTH];
    int
        history_links [SMT_POSITION_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile smt_position_list_t *list_head;
volatile smt_position_t *list_prev;
volatile smt_position_t *list_next;
smt_place_t   place;
smt_module_t  module;
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_position_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    smt_position_selftest (
void);

#define smt_position_new()              smt_position_new_ (__FILE__, __LINE__)
smt_position_t *
    smt_position_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_position_remove_from_all_containers (
smt_position_t * self                   //  The item
);

#define smt_position_show(item,opcode,trace_file)  smt_position_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_position_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    smt_position_terminate (
void);

#define smt_position_destroy(self)      smt_position_destroy_ (self, __FILE__, __LINE__)
void
    smt_position_destroy_ (
smt_position_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define smt_position_link(self)         smt_position_link_ (self, __FILE__, __LINE__)
smt_position_t *
    smt_position_link_ (
smt_position_t * self,                  //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_position_unlink(self)       smt_position_unlink_ (self, __FILE__, __LINE__)
void
    smt_position_unlink_ (
smt_position_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    smt_position_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_position_new_in_scope(self_p,_scope)  smt_position_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    smt_position_new_in_scope_ (
smt_position_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_POSITION_ACTIVE
#   if defined (ICL_IMPORT_SMT_POSITION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_POSITION_INLINE)
#   define INCLUDE_SMT_POSITION_INLINE
#   define INCLUDE_SMT_POSITION_ACTIVE

#   include "smt_thread.h"
#   include "smt_position_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_POSITION_ACTIVE
#   if defined (ICL_IMPORT_SMT_POSITION_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_POSITION_ROOT
#   endif
# endif
#endif
