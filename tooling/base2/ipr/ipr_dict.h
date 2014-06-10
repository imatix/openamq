/*---------------------------------------------------------------------------
    ipr_dict.h - ipr_dict component

Implements a symbol-table dictionary.  The dictionary holds string
values indexed by string keys.  The string values are duplicated.
All access to the dictionary is unlocked, and dictionary items do
not use links, so the dictionary should not be used from multiple
threads in parallel.
    Generated from ipr_dict.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DICT_SAFE
#   define INCLUDE_IPR_DICT_SAFE
#   define INCLUDE_IPR_DICT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DICT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_dict_t ipr_dict_t;

#define IPR_DICT_ALIVE                  0xFABB
#define IPR_DICT_DEAD                   0xDEAD

#define IPR_DICT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_dict\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DICT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_dict at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DICT_ALIVE, self->object_tag);\
        ipr_dict_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "ipr_dict_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DICT_UNSAFE
#   define INCLUDE_IPR_DICT_UNSAFE
#   define INCLUDE_IPR_DICT_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_dict_animating;

//  Structure of the ipr_dict object

struct _ipr_dict_t {
dbyte
    object_tag;                     //  Object validity marker
ipr_dict_table_t
    *table_head;                    //  Hash table container
ipr_dict_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
volatile ipr_dict_list_t *list_head;
volatile ipr_dict_t *list_prev;
volatile ipr_dict_t *list_next;
char
    *value;                         //  Text value for item
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_dict_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_dict_new(table,key,value)   ipr_dict_new_ (__FILE__, __LINE__, table, key, value)
ipr_dict_t *
    ipr_dict_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_dict_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
char * value                            //  Not documented
);

int
    ipr_dict_update (
ipr_dict_t * self,                      //  Reference to object
char * value                            //  Not documented
);

Bool
    ipr_dict_assume (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * value                            //  Symbol value
);

void
    ipr_dict_assume_fmt (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    ipr_dict_assume_fmt_v (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

void
    ipr_dict_selftest (
void);

void
    ipr_dict_remove_from_all_containers (
ipr_dict_t * self                       //  The itemThe item
);

#define ipr_dict_show(item,opcode,trace_file)  ipr_dict_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_dict_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_dict_terminate (
void);

#define ipr_dict_destroy(self)          ipr_dict_destroy_ (self, __FILE__, __LINE__)
void
    ipr_dict_destroy_ (
ipr_dict_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_dict_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_dict_new_in_scope(self_p,_scope,table,key,value)  ipr_dict_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, value)
void
    ipr_dict_new_in_scope_ (
ipr_dict_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_dict_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
char * value                            //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DICT_INLINE)
#   define INCLUDE_IPR_DICT_INLINE
#   define INCLUDE_IPR_DICT_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "ipr_dict_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DICT_ROOT
#   endif
# endif
#endif
