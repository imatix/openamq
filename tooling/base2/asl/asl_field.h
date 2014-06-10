/*---------------------------------------------------------------------------
    asl_field.h - asl_field component

    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at a per-field level.
    Generated from asl_field.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ASL_FIELD_SAFE
#   define INCLUDE_ASL_FIELD_SAFE
#   define INCLUDE_ASL_FIELD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ASL_FIELD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _asl_field_t asl_field_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
#    define ASL_FIELD_HISTORY_LENGTH 32
#endif

#define ASL_FIELD_ALIVE                 0xFABB
#define ASL_FIELD_DEAD                  0xDEAD

#define ASL_FIELD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL asl_field\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ASL_FIELD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "asl_field at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ASL_FIELD_ALIVE, self->object_tag);\
        asl_field_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#define ASL_FIELD_TYPE_STRING       'S'
#define ASL_FIELD_TYPE_INTEGER      'I'
#define ASL_FIELD_TYPE_DECIMAL      'D'
#define ASL_FIELD_TYPE_TIME         'T'
#define ASL_FIELD_TYPE_FIELD_TABLE  'F'  // not implemented so far
#define ASL_FIELD_TYPE_VOID         'V'
#define ASL_FIELD_NAME_MAX          128
#   ifdef __cplusplus
}
#   endif

#   include "asl.h"
#   include "asl_field_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ASL_FIELD_UNSAFE
#   define INCLUDE_ASL_FIELD_UNSAFE
#   define INCLUDE_ASL_FIELD_ACTIVE

#   include "asl.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    asl_field_animating;

//  Structure of the asl_field object

struct _asl_field_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [ASL_FIELD_HISTORY_LENGTH];
    int
        history_line  [ASL_FIELD_HISTORY_LENGTH];
    char
        *history_type [ASL_FIELD_HISTORY_LENGTH];
    int
        history_links [ASL_FIELD_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile asl_field_list_t *list_head;
volatile asl_field_t *list_prev;
volatile asl_field_t *list_next;
icl_shortstr_t
    name;                           //  Field name                         
byte
    type;                           //  Field type                         
byte
    decimals;                       //  Decimals, if necessary             
icl_longstr_t
    *string;                        //  Value as string                    
int64_t
    integer;                        //  Value as integer, decimal, time
};

#   ifdef __cplusplus
}
#   endif
#   include "asl_field_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define asl_field_new()                 asl_field_new_ (__FILE__, __LINE__)
asl_field_t *
    asl_field_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_new_string(list,name,value)  asl_field_new_string_ (__FILE__, __LINE__, list, name, value)
void
    asl_field_new_string_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
char * value                            //  Field string value
);

#define asl_field_new_longstr(list,name,value)  asl_field_new_longstr_ (__FILE__, __LINE__, list, name, value)
void
    asl_field_new_longstr_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
icl_longstr_t * value                   //  Field string value
);

#define asl_field_new_integer(list,name,value)  asl_field_new_integer_ (__FILE__, __LINE__, list, name, value)
void
    asl_field_new_integer_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
int value                               //  Field integer value
);

#define asl_field_new_decimal(list,name,value,decimals)  asl_field_new_decimal_ (__FILE__, __LINE__, list, name, value, decimals)
void
    asl_field_new_decimal_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
long value,                             //  Field decimal value
int decimals                            //  Number of decimal places
);

#define asl_field_new_time(list,name,value)  asl_field_new_time_ (__FILE__, __LINE__, list, name, value)
void
    asl_field_new_time_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
time_t value                            //  Field time value
);

#define asl_field_new_field_table(list,name,value)  asl_field_new_field_table_ (__FILE__, __LINE__, list, name, value)
void
    asl_field_new_field_table_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
asl_field_list_t * value                //  The value
);

#define asl_field_new_void(list,name)   asl_field_new_void_ (__FILE__, __LINE__, list, name)
void
    asl_field_new_void_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
asl_field_list_t * list,                //  Parent field list
char * name                             //  Field name
);

void
    asl_field_assume (
asl_field_list_t * list,                //  Parent field list
char * name,                            //  Field name
char * value                            //  Field string value
);

byte *
    asl_field_parse (
asl_field_list_t * list,                //  Parent field list
byte * input,                           //  Field definition structure
byte * limit                            //  End of field-table string
);

char *
    asl_field_string (
asl_field_t * self                      //  Reference to object
);

long
    asl_field_integer (
asl_field_t * self                      //  Reference to object
);

asl_field_list_t*
    asl_field_field_table (
asl_field_t * self                      //  Reference to object
);

int
    asl_field_flatten (
asl_field_t * self,                     //  Reference to object
icl_longstr_t * string                  //  String to write to
);

int
    asl_field_flat_size (
asl_field_t * self                      //  Reference to object
);

int
    asl_field_print (
asl_field_t * self                      //  Reference to object
);

void
    asl_field_selftest (
void);

void
    asl_field_remove_from_all_containers (
asl_field_t * self                      //  The item
);

#define asl_field_show(item,opcode,trace_file)  asl_field_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    asl_field_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    asl_field_terminate (
void);

#define asl_field_destroy(self)         asl_field_destroy_ (self, __FILE__, __LINE__)
void
    asl_field_destroy_ (
asl_field_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define asl_field_link(self)            asl_field_link_ (self, __FILE__, __LINE__)
asl_field_t *
    asl_field_link_ (
asl_field_t * self,                     //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_unlink(self)          asl_field_unlink_ (self, __FILE__, __LINE__)
void
    asl_field_unlink_ (
asl_field_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    asl_field_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define asl_field_new_in_scope(self_p,_scope)  asl_field_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    asl_field_new_in_scope_ (
asl_field_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ASL_FIELD_INLINE)
#   define INCLUDE_ASL_FIELD_INLINE
#   define INCLUDE_ASL_FIELD_ACTIVE

#   include "asl.h"
#   include "asl_field_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ASL_FIELD_ACTIVE
#   if defined (ICL_IMPORT_ASL_FIELD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ASL_FIELD_ROOT
#   endif
# endif
#endif
