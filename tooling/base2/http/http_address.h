/*---------------------------------------------------------------------------
    http_address.h - http_address component

    Stores data about a known IP address, used for DoS detection and other
    administration.  The address holds a looseref list of http_agent threads.
    Generated from http_address.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_ADDRESS_SAFE
#   define INCLUDE_HTTP_ADDRESS_SAFE
#   define INCLUDE_HTTP_ADDRESS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_ADDRESS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_address_t http_address_t;

#define HTTP_ADDRESS_ALIVE              0xFABB
#define HTTP_ADDRESS_DEAD               0xDEAD

#define HTTP_ADDRESS_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_address\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_ADDRESS_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_address at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_ADDRESS_ALIVE, self->object_tag);\
        http_address_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "http.h"
#   include "http_address_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ADDRESS_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ADDRESS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_ADDRESS_UNSAFE
#   define INCLUDE_HTTP_ADDRESS_UNSAFE
#   define INCLUDE_HTTP_ADDRESS_ACTIVE

#   include "http.h"
#   include "http_address_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    http_address_animating;

//  Structure of the http_address object

struct _http_address_t {
#if (defined (BASE_THREADSAFE))
    icl_mutex_t *
        mutex;
#endif
    dbyte
        object_tag;                     //  Object validity marker
http_address_table_t
    *table_head;                    //  Hash table container
http_address_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
ipr_looseref_list_t
    *thread_list;                   //  List of threads active on address
size_t
    thread_quota;                   //  Remaining quota for threads
/*
- number of hits
- first date/time
- last date/time
- threads open
- list of threads
- thread quota
- throttle quota
    b/s
*/
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_address_new(table,key)     http_address_new_ (__FILE__, __LINE__, table, key)
http_address_t *
    http_address_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_address_table_t * table,           //  Table to insert into
char * key                              //  Hash key
);

int
    http_address_thread_open (
http_address_table_t * table,           //  Not documented
http_access_context_t * context         //  Not documented
);

int
    http_address_thread_close (
http_address_table_t * table,           //  Not documented
http_access_context_t * context         //  Not documented
);

void
    http_address_selftest (
void);

void
    http_address_remove_from_all_containers (
http_address_t * self                   //  The item
);

#define http_address_show(item,opcode,trace_file)  http_address_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_address_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    http_address_terminate (
void);

#define http_address_destroy(self)      http_address_destroy_ (self, __FILE__, __LINE__)
void
    http_address_destroy_ (
http_address_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    http_address_lock (
http_address_t * self                   //  Reference to object
);

int
    http_address_unlock (
http_address_t * self                   //  Reference to object
);

void
    http_address_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_address_new_in_scope(self_p,_scope,table,key)  http_address_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key)
void
    http_address_new_in_scope_ (
http_address_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_address_table_t * table,           //  Table to insert into
char * key                              //  Hash key
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ADDRESS_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ADDRESS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_ADDRESS_INLINE)
#   define INCLUDE_HTTP_ADDRESS_INLINE
#   define INCLUDE_HTTP_ADDRESS_ACTIVE

#   include "http.h"
#   include "http_address_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ADDRESS_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ADDRESS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_ADDRESS_ROOT
#   endif
# endif
#endif
