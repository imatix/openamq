/*---------------------------------------------------------------------------
    ipr_token.h - ipr_token component

Provides string tokenisation functions.  Use this to split a string
into a list of words (delimited by spaces) so that each word can be
manipulated separately.
    Generated from ipr_token.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_TOKEN_SAFE
#   define INCLUDE_IPR_TOKEN_SAFE
#   define INCLUDE_IPR_TOKEN_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_TOKEN_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_token_t ipr_token_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
#    define IPR_TOKEN_HISTORY_LENGTH 32
#endif

#define IPR_TOKEN_ALIVE                 0xFABB
#define IPR_TOKEN_DEAD                  0xDEAD

#define IPR_TOKEN_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_token\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_TOKEN_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_token at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_TOKEN_ALIVE, self->object_tag);\
        ipr_token_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "ipr_token_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_TOKEN_UNSAFE
#   define INCLUDE_IPR_TOKEN_UNSAFE
#   define INCLUDE_IPR_TOKEN_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_token_animating;

//  Structure of the ipr_token object

struct _ipr_token_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_TOKEN_HISTORY_LENGTH];
    int
        history_line  [IPR_TOKEN_HISTORY_LENGTH];
    char
        *history_type [IPR_TOKEN_HISTORY_LENGTH];
    int
        history_links [IPR_TOKEN_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile ipr_token_list_t *list_head;
volatile ipr_token_t *list_prev;
volatile ipr_token_t *list_next;
icl_shortstr_t
    value;                          //  Item value
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_token_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
ipr_token_list_t *
    ipr_token_split (
char * string                           //  String to parse
);

ipr_token_list_t *
    ipr_token_split_rich (
char * string,                          //  String to parse
char * delims                           //  Delimiters
);

void
    ipr_token_selftest (
void);

#define ipr_token_new()                 ipr_token_new_ (__FILE__, __LINE__)
ipr_token_t *
    ipr_token_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_token_remove_from_all_containers (
ipr_token_t * self                      //  The item
);

#define ipr_token_show(item,opcode,trace_file)  ipr_token_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_token_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_token_terminate (
void);

#define ipr_token_destroy(self)         ipr_token_destroy_ (self, __FILE__, __LINE__)
void
    ipr_token_destroy_ (
ipr_token_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_token_link(self)            ipr_token_link_ (self, __FILE__, __LINE__)
ipr_token_t *
    ipr_token_link_ (
ipr_token_t * self,                     //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_token_unlink(self)          ipr_token_unlink_ (self, __FILE__, __LINE__)
void
    ipr_token_unlink_ (
ipr_token_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_token_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_token_new_in_scope(self_p,_scope)  ipr_token_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_token_new_in_scope_ (
ipr_token_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_TOKEN_INLINE)
#   define INCLUDE_IPR_TOKEN_INLINE
#   define INCLUDE_IPR_TOKEN_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "ipr_token_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_TOKEN_ROOT
#   endif
# endif
#endif
