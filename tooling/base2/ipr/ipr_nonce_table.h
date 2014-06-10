/*---------------------------------------------------------------------------
    ipr_nonce_table.h - ipr_nonce_table component

    This class implements the hash table container for ipr_nonce
    Generated from ipr_nonce_table.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_NONCE_TABLE_SAFE
#   define INCLUDE_IPR_NONCE_TABLE_SAFE
#   define INCLUDE_IPR_NONCE_TABLE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_NONCE_TABLE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_nonce_table_t ipr_nonce_table_t;

#define IPR_NONCE_TABLE_ALIVE           0xFABB
#define IPR_NONCE_TABLE_DEAD            0xDEAD

#define IPR_NONCE_TABLE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_nonce_table\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_NONCE_TABLE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_nonce_table at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_NONCE_TABLE_ALIVE, self->object_tag);\
        ipr_nonce_table_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_nonce.h"
#   include "icl_shortstr.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_NONCE_TABLE_UNSAFE
#   define INCLUDE_IPR_NONCE_TABLE_UNSAFE
#   define INCLUDE_IPR_NONCE_TABLE_ACTIVE

#   include "icl_shortstr.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_nonce_table_animating;
#define IPR_NONCE_TABLE_INITIAL_SIZE    255
#define IPR_NONCE_TABLE_LOAD_FACTOR     75
#define IPR_NONCE_TABLE_GROWTH_FACTOR 200

typedef void (ipr_nonce_table_callback_fn) (ipr_nonce_t *item, void *argument);

//  Structure of the ipr_nonce_table object

struct _ipr_nonce_table_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
size_t
    nbr_items;
size_t
    max_items;
ipr_nonce_t
    **table_items;
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_nonce.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_nonce_table_new()           ipr_nonce_table_new_ (__FILE__, __LINE__)
ipr_nonce_table_t *
    ipr_nonce_table_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_nonce_table_selftest (
void);

int
    ipr_nonce_table_insert (
ipr_nonce_table_t * self,               //  Reference to object
char * key,                             //  Hash key
ipr_nonce_t * item                      //  Item to insert
);

int
    ipr_nonce_table_remove (
ipr_nonce_t * item                      //  Item to remove
);

ipr_nonce_t *
    ipr_nonce_table_search (
ipr_nonce_table_t * self,               //  Table to search
char * key                              //  Hash key
);

void
    ipr_nonce_table_apply (
ipr_nonce_table_t * self,               //  Table to iterate
ipr_nonce_table_callback_fn * callback,   //  Not documented
void * argument                         //  Arbitrary argument pointer
);

void
    ipr_nonce_table_terminate (
void);

#define ipr_nonce_table_show(item,opcode,trace_file)  ipr_nonce_table_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_nonce_table_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_nonce_table_destroy(self)   ipr_nonce_table_destroy_ (self, __FILE__, __LINE__)
void
    ipr_nonce_table_destroy_ (
ipr_nonce_table_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_nonce_table_read_lock (
ipr_nonce_table_t * self                //  Reference to object
);

int
    ipr_nonce_table_write_lock (
ipr_nonce_table_t * self                //  Reference to object
);

int
    ipr_nonce_table_unlock (
ipr_nonce_table_t * self                //  Reference to object
);

void
    ipr_nonce_table_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_nonce_table_new_in_scope(self_p,_scope)  ipr_nonce_table_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_nonce_table_new_in_scope_ (
ipr_nonce_table_t * * self_p,           //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_NONCE_TABLE_INLINE)
#   define INCLUDE_IPR_NONCE_TABLE_INLINE
#   define INCLUDE_IPR_NONCE_TABLE_ACTIVE

#   include "ipr_nonce.h"
#   include "icl_shortstr.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_TABLE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_NONCE_TABLE_ROOT
#   endif
# endif
#endif
