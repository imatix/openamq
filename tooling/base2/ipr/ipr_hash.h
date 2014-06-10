/*---------------------------------------------------------------------------
    ipr_hash.h - ipr_hash component

Implements a loose-reference hash with no locking, for use in lock-free agents.
The hash is indexed by a string key, and holds a void pointer reference.  Hash
items are reference counted, so use unlink and destroy appropriately.
    Generated from ipr_hash.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_HASH_SAFE
#   define INCLUDE_IPR_HASH_SAFE
#   define INCLUDE_IPR_HASH_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_HASH_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_hash_t ipr_hash_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
#    define IPR_HASH_HISTORY_LENGTH 32
#endif

#define IPR_HASH_ALIVE                  0xFABB
#define IPR_HASH_DEAD                   0xDEAD

#define IPR_HASH_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_hash\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_HASH_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_hash at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_HASH_ALIVE, self->object_tag);\
        ipr_hash_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_HASH_UNSAFE
#   define INCLUDE_IPR_HASH_UNSAFE
#   define INCLUDE_IPR_HASH_ACTIVE

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_hash_animating;

//  Structure of the ipr_hash object

struct _ipr_hash_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_HASH_HISTORY_LENGTH];
    int
        history_line  [IPR_HASH_HISTORY_LENGTH];
    char
        *history_type [IPR_HASH_HISTORY_LENGTH];
    int
        history_links [IPR_HASH_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_hash_table_t
    *table_head;                    //  Hash table container
ipr_hash_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
void *data;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_hash_new(table,key,data)    ipr_hash_new_ (__FILE__, __LINE__, table, key, data)
ipr_hash_t *
    ipr_hash_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_hash_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Not documented
);

void
    ipr_hash_insert (
ipr_hash_table_t * table,               //  Not documented
char * key,                             //  Not documented
void * data                             //  Not documented
);

void *
    ipr_hash_lookup (
ipr_hash_table_t * table,               //  Not documented
char * key                              //  Not documented
);

void
    ipr_hash_delete (
ipr_hash_table_t * table,               //  Not documented
char * key                              //  Not documented
);

void
    ipr_hash_selftest (
void);

void
    ipr_hash_remove_from_all_containers (
ipr_hash_t * self                       //  The item
);

#define ipr_hash_show(item,opcode,trace_file)  ipr_hash_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_hash_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_hash_terminate (
void);

#define ipr_hash_destroy(self)          ipr_hash_destroy_ (self, __FILE__, __LINE__)
void
    ipr_hash_destroy_ (
ipr_hash_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_hash_link(self)             ipr_hash_link_ (self, __FILE__, __LINE__)
ipr_hash_t *
    ipr_hash_link_ (
ipr_hash_t * self,                      //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_hash_unlink(self)           ipr_hash_unlink_ (self, __FILE__, __LINE__)
void
    ipr_hash_unlink_ (
ipr_hash_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_hash_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_hash_new_in_scope(self_p,_scope,table,key,data)  ipr_hash_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, data)
void
    ipr_hash_new_in_scope_ (
ipr_hash_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_hash_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_HASH_INLINE)
#   define INCLUDE_IPR_HASH_INLINE
#   define INCLUDE_IPR_HASH_ACTIVE

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_HASH_ROOT
#   endif
# endif
#endif
