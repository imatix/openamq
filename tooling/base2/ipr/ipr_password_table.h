/*---------------------------------------------------------------------------
    ipr_password_table.h - ipr_password_table component

    This class implements a hash container for ipr_password with the ability
    to load password files in various formats. One table can hold passwords
    of any type (Basic or Digest), the type is verified when the password is
    validated.  See http://tools.ietf.org/html/rfc2617. The class is
    threadsafe and may be used from concurrent threads.
    Generated from ipr_password_table.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_PASSWORD_TABLE_SAFE
#   define INCLUDE_IPR_PASSWORD_TABLE_SAFE
#   define INCLUDE_IPR_PASSWORD_TABLE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_PASSWORD_TABLE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_password_table_t ipr_password_table_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_PASSWORD_TABLE)
#    define IPR_PASSWORD_TABLE_HISTORY_LENGTH 32
#endif

#define IPR_PASSWORD_TABLE_ALIVE        0xFABB
#define IPR_PASSWORD_TABLE_DEAD         0xDEAD

#define IPR_PASSWORD_TABLE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_password_table\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_PASSWORD_TABLE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_password_table at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_PASSWORD_TABLE_ALIVE, self->object_tag);\
        ipr_password_table_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#define IPR_PASSWORD_OK             0
#define IPR_PASSWORD_CHALLENGE      1   //  No credentials provided
#define IPR_PASSWORD_NOTFOUND       2   //  No such user[:realm]
#define IPR_PASSWORD_EXPIRED        3   //  Password expired
#define IPR_PASSWORD_INCORRECT      4   //  Credentials are wrong
#define IPR_PASSWORD_BADTYPE        5   //  Wrong mechanism
#define IPR_PASSWORD_SYNTAX         6   //  Credential syntax bogus
#define IPR_PASSWORD_HOSTILE        7   //  Credentials look hostile
#define IPR_PASSWORD_TRYAGAIN       8   //  Client should try again
#   ifdef __cplusplus
}
#   endif

#   include "ipr_password.h"
#   include "ipr_nonce.h"
#   include "ipr_str.h"
#   include "ipr_file.h"
#   include "ipr_dict.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_PASSWORD_TABLE_UNSAFE
#   define INCLUDE_IPR_PASSWORD_TABLE_UNSAFE
#   define INCLUDE_IPR_PASSWORD_TABLE_ACTIVE

#   include "ipr_password.h"
#   include "ipr_nonce.h"
#   include "ipr_str.h"
#   include "ipr_file.h"
#   include "ipr_dict.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_password_table_animating;
#define IPR_PASSWORD_TABLE_INITIAL_SIZE  65535
#define IPR_PASSWORD_TABLE_LOAD_FACTOR  75
#define IPR_PASSWORD_TABLE_GROWTH_FACTOR 200

typedef void (ipr_password_table_callback_fn) (ipr_password_t *item, void *argument);

//  Structure of the ipr_password_table object

struct _ipr_password_table_t {
#if (defined (BASE_THREADSAFE))
    icl_mutex_t *
        mutex;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_PASSWORD_TABLE)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_PASSWORD_TABLE_HISTORY_LENGTH];
    int
        history_line  [IPR_PASSWORD_TABLE_HISTORY_LENGTH];
    char
        *history_type [IPR_PASSWORD_TABLE_HISTORY_LENGTH];
    int
        history_links [IPR_PASSWORD_TABLE_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
size_t
    nbr_items;
size_t
    max_items;
ipr_password_t
    **table_items;
ipr_password_list_t
    *list;                          //  List of all password items
char
    *file_name;                     //  If data was loaded from file
int64_t
    file_size;                      //  Size of loaded file
apr_time_t
    file_time;                      //  Time of loaded file
Bool
    debug,                          //  Yes to get debug output
    dirty;                          //  Guard data changed
uint
    result;                         //  IPR_PASSWORD_something
icl_shortstr_t
    username,                       //  User name if known
    realm;                          //  Realm, if known
size_t
    hit_quota,                      //  Guard properties
    miss_quota,
    time_to_live;
ipr_nonce_table_t
    *nonces;                        //  Table of nonces, if needed
size_t
    nonce_ttl,                      //  Time to live for new nonces
    nonce_quota;                    //  Hit quota for new nonces
char
    *nonce_test;                    //  Force nonce value for tests
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_password_table_new(filename)  ipr_password_table_new_ (__FILE__, __LINE__, filename)
ipr_password_table_t *
    ipr_password_table_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename                         //  Name of associated file
);

int
    ipr_password_table_insert (
ipr_password_table_t * self,            //  Reference to object
char * key,                             //  Hash key
ipr_password_t * item                   //  Item to insert
);

int
    ipr_password_table_remove (
ipr_password_t * item                   //  Item to remove
);

ipr_password_t *
    ipr_password_table_search_digest (
ipr_password_table_t * self,            //  Not documented
char * username,                        //  Not documented
char * realm                            //  Not documented
);

int
    ipr_password_table_load (
ipr_password_table_t * self,            //  Reference to object
char * filename                         //  Name of associated file
);

int
    ipr_password_table_save (
ipr_password_table_t * self             //  Reference to object
);

int
    ipr_password_table_sync (
ipr_password_table_t * self             //  Reference to object
);

int
    ipr_password_table_guard_set (
ipr_password_table_t * self,            //  Reference to object
size_t hit_quota,                       //  Not documented
size_t miss_quota,                      //  Not documented
size_t time_to_live                     //  Not documented
);

int
    ipr_password_table_guard_load (
ipr_password_table_t * self             //  Reference to object
);

int
    ipr_password_table_guard_save (
ipr_password_table_t * self             //  Reference to object
);

int
    ipr_password_table_guard_sync (
ipr_password_table_t * self             //  Reference to object
);

char *
    ipr_password_table_basic (
ipr_password_table_t * self,            //  Reference to self
char * address,                         //  IP address of caller
char * realm,                           //  Authentication realm
char * response                         //  Authentication response
);

char *
    ipr_password_table_digest (
ipr_password_table_t * self,            //  Reference to self
char * address,                         //  IP address of caller
char * realm,                           //  Authentication realm
char * response,                        //  Authentication response
char * method,                          //  HTTP request method
char * uri                              //  HTTP request URI
);

int
    ipr_password_table_set_nonce_ttl (
ipr_password_table_t * self,            //  Reference to object
size_t ttl                              //  Time to live, seconds
);

int
    ipr_password_table_set_nonce_quota (
ipr_password_table_t * self,            //  Reference to object
size_t quota                            //  Lifespan, in hits
);

int
    ipr_password_table_set_nonce_test (
ipr_password_table_t * self,            //  Reference to object
char * value                            //  Test nonce value
);

int
    ipr_password_table_set_debug (
ipr_password_table_t * self,            //  Reference to object
Bool value                              //  Enable debug?
);

void
    ipr_password_table_selftest (
void);

ipr_password_t *
    ipr_password_table_search (
ipr_password_table_t * self,            //  Table to search
char * key                              //  Hash key
);

void
    ipr_password_table_apply (
ipr_password_table_t * self,            //  Table to iterate
ipr_password_table_callback_fn * callback,   //  Not documented
void * argument                         //  Arbitrary argument pointer
);

void
    ipr_password_table_terminate (
void);

#define ipr_password_table_show(item,opcode,trace_file)  ipr_password_table_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_password_table_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_password_table_destroy(self)  ipr_password_table_destroy_ (self, __FILE__, __LINE__)
void
    ipr_password_table_destroy_ (
ipr_password_table_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_password_table_lock (
ipr_password_table_t * self             //  Reference to object
);

int
    ipr_password_table_unlock (
ipr_password_table_t * self             //  Reference to object
);

#define ipr_password_table_link(self)   ipr_password_table_link_ (self, __FILE__, __LINE__)
ipr_password_table_t *
    ipr_password_table_link_ (
ipr_password_table_t * self,            //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_password_table_unlink(self)  ipr_password_table_unlink_ (self, __FILE__, __LINE__)
void
    ipr_password_table_unlink_ (
ipr_password_table_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_password_table_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_password_table_new_in_scope(self_p,_scope,filename)  ipr_password_table_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, filename)
void
    ipr_password_table_new_in_scope_ (
ipr_password_table_t * * self_p,        //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename                         //  Name of associated file
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_PASSWORD_TABLE_INLINE)
#   define INCLUDE_IPR_PASSWORD_TABLE_INLINE
#   define INCLUDE_IPR_PASSWORD_TABLE_ACTIVE

#   include "ipr_password.h"
#   include "ipr_nonce.h"
#   include "ipr_str.h"
#   include "ipr_file.h"
#   include "ipr_dict.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_TABLE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_PASSWORD_TABLE_ROOT
#   endif
# endif
#endif
