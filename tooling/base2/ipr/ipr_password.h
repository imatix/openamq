/*---------------------------------------------------------------------------
    ipr_password.h - ipr_password component

    Password tables are held as dictionaries.  Supports passwords in
    either basic or digest format as defined by RFC 2617. ipr_password
    items do not use links.  This class is threadsafe and can be used
    from concurrent threads.
    Generated from ipr_password.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_PASSWORD_SAFE
#   define INCLUDE_IPR_PASSWORD_SAFE
#   define INCLUDE_IPR_PASSWORD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_PASSWORD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_password_t ipr_password_t;

#define IPR_PASSWORD_ALIVE              0xFABB
#define IPR_PASSWORD_DEAD               0xDEAD

#define IPR_PASSWORD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_password\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_PASSWORD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_password at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_PASSWORD_ALIVE, self->object_tag);\
        ipr_password_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#define IPR_PASSWD_BASIC        1       //  Basic encoding
#define IPR_PASSWD_DIGEST       2       //  Digest encoding
#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "ipr_password_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_PASSWORD_UNSAFE
#   define INCLUDE_IPR_PASSWORD_UNSAFE
#   define INCLUDE_IPR_PASSWORD_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_password_animating;

//  Structure of the ipr_password object

struct _ipr_password_t {
#if (defined (BASE_THREADSAFE))
    icl_mutex_t *
        mutex;
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_password_table_t
    *table_head;                    //  Hash table container
ipr_password_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
volatile ipr_password_list_t *list_head;
volatile ipr_password_t *list_prev;
volatile ipr_password_t *list_next;
char
    *hash;                          //  Password hash
int
    type;                           //  IPR_PASSWD_ value
//  Guard data
size_t
    hits,                           //  Total hits
    hit_quota,                      //  Hits remaining, or 0
    misses,                         //  Total misses
    miss_quota;                     //  Misses remaining, or 0
apr_time_t
    expiry,                         //  Expiration time, or 0
    hit_time,                       //  Time of last hit
    miss_time;                      //  Time of last miss
icl_shortstr_t
    hit_from,                       //  Last good IP address
    miss_from;                      //  Last bad IP address
Bool
    expired;                        //  Run out of luck...
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_password_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_password_new(table,key,hash)  ipr_password_new_ (__FILE__, __LINE__, table, key, hash)
ipr_password_t *
    ipr_password_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_password_table_t * table,           //  Table to insert into
char * key,                             //  Hash key
char * hash                             //  Not documented
);

int
    ipr_password_update (
ipr_password_t * self,                  //  Reference to object
char * hash                             //  Not documented
);

ipr_password_t *
    ipr_password_assume (
ipr_password_table_t * table,           //  Not documented
char * key,                             //  Not documented
char * hash                             //  Not documented
);

int
    ipr_password_guard_set (
ipr_password_t * self,                  //  Reference to object
size_t hit_quota,                       //  Not documented
size_t miss_quota,                      //  Not documented
size_t time_to_live                     //  Not documented
);

int
    ipr_password_guard_get (
ipr_password_t * self,                  //  Reference to object
char * options                          //  name=value ...
);

int
    ipr_password_guard_put (
ipr_password_t * self,                  //  Reference to object
FILE * file                             //  Not documented
);

int
    ipr_password_hit (
ipr_password_t * self,                  //  Reference to object
char * address                          //  Not documented
);

int
    ipr_password_miss (
ipr_password_t * self,                  //  Reference to object
char * address                          //  Not documented
);

void
    ipr_password_selftest (
void);

void
    ipr_password_remove_from_all_containers (
ipr_password_t * self                   //  The itemThe item
);

#define ipr_password_show(item,opcode,trace_file)  ipr_password_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_password_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_password_terminate (
void);

#define ipr_password_destroy(self)      ipr_password_destroy_ (self, __FILE__, __LINE__)
void
    ipr_password_destroy_ (
ipr_password_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_password_lock (
ipr_password_t * self                   //  Reference to object
);

int
    ipr_password_unlock (
ipr_password_t * self                   //  Reference to object
);

void
    ipr_password_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_password_new_in_scope(self_p,_scope,table,key,hash)  ipr_password_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, hash)
void
    ipr_password_new_in_scope_ (
ipr_password_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_password_table_t * table,           //  Table to insert into
char * key,                             //  Hash key
char * hash                             //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_PASSWORD_INLINE)
#   define INCLUDE_IPR_PASSWORD_INLINE
#   define INCLUDE_IPR_PASSWORD_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "ipr_password_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_PASSWORD_ROOT
#   endif
# endif
#endif
