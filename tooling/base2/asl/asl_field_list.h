/*---------------------------------------------------------------------------
    asl_field_list.h - asl_field_list component

    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at the field-table level.
    Each field table is represented as a list of fields.  This class
    provides methods to transform field lists to and from aslP binary
    formats, and lookup and operate on field lists.
    Generated from asl_field_list.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ASL_FIELD_LIST_SAFE
#   define INCLUDE_ASL_FIELD_LIST_SAFE
#   define INCLUDE_ASL_FIELD_LIST_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ASL_FIELD_LIST_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _asl_field_list_t asl_field_list_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
#    define ASL_FIELD_LIST_HISTORY_LENGTH 32
#endif

#define ASL_FIELD_LIST_ALIVE            0xFABB
#define ASL_FIELD_LIST_DEAD             0xDEAD

#define ASL_FIELD_LIST_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL asl_field_list\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ASL_FIELD_LIST_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "asl_field_list at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ASL_FIELD_LIST_ALIVE, self->object_tag);\
        asl_field_list_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "asl_field.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_LIST_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_LIST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ASL_FIELD_LIST_UNSAFE
#   define INCLUDE_ASL_FIELD_LIST_UNSAFE
#   define INCLUDE_ASL_FIELD_LIST_ACTIVE

#   include "asl_field.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    asl_field_list_animating;

//  Structure of the asl_field_list object

struct _asl_field_list_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [ASL_FIELD_LIST_HISTORY_LENGTH];
    int
        history_line  [ASL_FIELD_LIST_HISTORY_LENGTH];
    char
        *history_type [ASL_FIELD_LIST_HISTORY_LENGTH];
    int
        history_links [ASL_FIELD_LIST_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
asl_field_t
    asl_field;                      //  Self starts with child object

};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define asl_field_list_new(field_table)  asl_field_list_new_ (__FILE__, __LINE__, field_table)
asl_field_list_t *
    asl_field_list_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
icl_longstr_t * field_table             //  Field table
);

int
    asl_field_list_parse (
asl_field_list_t * self,                //  Reference to object
icl_longstr_t * field_table             //  Field table
);

icl_longstr_t *
    asl_field_list_flatten (
asl_field_list_t * self                 //  Reference to object
);

asl_field_t *
    asl_field_list_search (
asl_field_list_t * self,                //  Reference to object
char * name                             //  Field name
);

int
    asl_field_list_cpy (
asl_field_list_t * self,                //  Reference to objectReference to object
char * dest,                            //  Destination shortstr
char * name                             //  Field name
);

char *
    asl_field_list_string (
asl_field_list_t * self,                //  Reference to object
char * name                             //  Field name
);

long
    asl_field_list_integer (
asl_field_list_t * self,                //  Reference to object
char * name                             //  Field name
);

int
    asl_field_list_print (
asl_field_list_t * self                 //  Reference to object
);

int
    asl_field_list_dump (
asl_field_list_t * self,                //  Reference to object
char * dest                             //  Not documented
);

int
    asl_field_list_to_tree (
asl_field_list_t * self,                //  Reference to object
ipr_tree_t * tree                       //  Not documented
);

icl_longstr_t *
    asl_field_list_build (
char * start,                           //  First field name
...                                     //  Variable arguments
);

icl_longstr_t *
    asl_field_list_build_v (
char * start,                           //  First field name
va_list argptr                          //  Variable arguments
);

void
    asl_field_list_selftest (
void);

int
    asl_field_list_isempty (
asl_field_list_t * self                 //  Reference to object
);

int
    asl_field_list_push (
asl_field_list_t * self,                //  Reference to object
asl_field_t * item                      //  Not documented
);

asl_field_t *
    asl_field_list_pop (
asl_field_list_t * self                 //  The list
);

int
    asl_field_list_queue (
asl_field_list_t * self,                //  Reference to object
asl_field_t * item                      //  Not documented
);

int
    asl_field_list_relink_before (
asl_field_t * item,                     //  Not documented
asl_field_t * where                     //  Not documented
);

int
    asl_field_list_remove (
asl_field_t * item                      //  Not documented
);

#define asl_field_list_first(self)      asl_field_list_first_ (self, __FILE__, __LINE__)
asl_field_t *
    asl_field_list_first_ (
asl_field_list_t * self,                //  The list
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_list_last(self)       asl_field_list_last_ (self, __FILE__, __LINE__)
asl_field_t *
    asl_field_list_last_ (
asl_field_list_t * self,                //  The list
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_list_next(item)       asl_field_list_next_ (item, __FILE__, __LINE__)
asl_field_t *
    asl_field_list_next_ (
asl_field_t * ( * item_p ),             //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_list_prev(item)       asl_field_list_prev_ (item, __FILE__, __LINE__)
asl_field_t *
    asl_field_list_prev_ (
asl_field_t * ( * item_p ),             //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    asl_field_list_terminate (
void);

#define asl_field_list_show(item,opcode,trace_file)  asl_field_list_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    asl_field_list_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define asl_field_list_destroy(self)    asl_field_list_destroy_ (self, __FILE__, __LINE__)
void
    asl_field_list_destroy_ (
asl_field_list_t * ( * self_p ),        //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define asl_field_list_link(self)       asl_field_list_link_ (self, __FILE__, __LINE__)
asl_field_list_t *
    asl_field_list_link_ (
asl_field_list_t * self,                //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_list_unlink(self)     asl_field_list_unlink_ (self, __FILE__, __LINE__)
void
    asl_field_list_unlink_ (
asl_field_list_t * ( * self_p ),        //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    asl_field_list_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define asl_field_list_new_in_scope(self_p,_scope,field_table)  asl_field_list_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, field_table)
void
    asl_field_list_new_in_scope_ (
asl_field_list_t * * self_p,            //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
icl_longstr_t * field_table             //  Field table
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_LIST_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_LIST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ASL_FIELD_LIST_INLINE)
#   define INCLUDE_ASL_FIELD_LIST_INLINE
#   define INCLUDE_ASL_FIELD_LIST_ACTIVE

#   include "asl_field.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_LIST_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_LIST_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ASL_FIELD_LIST_ROOT
#   endif
# endif
#endif
