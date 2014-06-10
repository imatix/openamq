/*---------------------------------------------------------------------------
    ipr_demo_portal.h - ipr_demo_portal component

    Generated from ipr_demo_portal.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DEMO_PORTAL_SAFE
#   define INCLUDE_IPR_DEMO_PORTAL_SAFE
#   define INCLUDE_IPR_DEMO_PORTAL_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DEMO_PORTAL_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_demo_portal_t ipr_demo_portal_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL)
#    define IPR_DEMO_PORTAL_HISTORY_LENGTH 32
#endif

#define IPR_DEMO_PORTAL_ALIVE           0xFABB
#define IPR_DEMO_PORTAL_DEAD            0xDEAD

#define IPR_DEMO_PORTAL_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_demo_portal\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DEMO_PORTAL_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_demo_portal at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DEMO_PORTAL_ALIVE, self->object_tag);\
        ipr_demo_portal_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#ifndef IPR_UNTYPED_LINK
#define IPR_UNTYPED_LINK
    typedef void * (untyped_link_fn) (void *object);
    typedef void (untyped_unlink_fn) (void *object_p);
#endif
#   ifdef __cplusplus
}
#   endif

#   include "ipr_bucket.h"
#   include "ipr_looseref.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DEMO_PORTAL_UNSAFE
#   define INCLUDE_IPR_DEMO_PORTAL_UNSAFE
#   define INCLUDE_IPR_DEMO_PORTAL_ACTIVE

#   include "ipr_bucket.h"
#   include "ipr_looseref.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_demo_portal_animating;

//  Structure of the ipr_demo_portal object

struct _ipr_demo_portal_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_DEMO_PORTAL_HISTORY_LENGTH];
    int
        history_line  [IPR_DEMO_PORTAL_HISTORY_LENGTH];
    char
        *history_type [IPR_DEMO_PORTAL_HISTORY_LENGTH];
    int
        history_links [IPR_DEMO_PORTAL_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
Bool
    bound,                          //  Has portal been bound?
    stopped;                        //  Has portal been stopped?

//  Server-side state glue
void
    *server_object;                 //  If null, is a factory
ipr_looseref_t
    *server_looseref;               //  Where server holds portal reference
//  Server virtual methods
void (*server_unlink)
    (void *server);
void (*server_unbind)
    (void *server, ipr_demo_portal_t *portal);

//  Client-side state glue
void
    *client_object;                 //  Client object reference
ipr_looseref_t
    *client_looseref;               //  Where client holds portal reference
void (*client_unbind)
    (void *client, ipr_demo_portal_t *portal);
icl_shortstr_t
    name;
ipr_demo_portal_t *(*server_new)
    (void *_server, char * name);
int (*request_hello)
    (void *_server, ipr_demo_portal_t *portal);
int (*request_print)
    (void *_server, ipr_demo_portal_t *portal, char * string);
int (*response_print)
    (void *_client, ipr_demo_portal_t *portal, char * string);
int (*response_mogrify)
    (void *_client, ipr_demo_portal_t *portal, char * string, ipr_bucket_t * bucket, Bool option);
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_demo_portal_new(server_object,server_link,server_unlink,name)  ipr_demo_portal_new_ (__FILE__, __LINE__, server_object, server_link, server_unlink, name)
ipr_demo_portal_t *
    ipr_demo_portal_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
void * server_object,                   //  Not documented
untyped_link_fn * server_link,          //  Not documented
untyped_unlink_fn * server_unlink,      //  Not documented
char * name                             //  Not documented
);

ipr_demo_portal_t *
    ipr_demo_portal_factory (
ipr_demo_portal_t * self,               //  Portal factory
char * name                             //  Not documented
);

int
    ipr_demo_portal_request_hello (
ipr_demo_portal_t * self                //  Not documented
);

int
    ipr_demo_portal_request_print (
ipr_demo_portal_t * self,               //  Not documented
char * string                           //  Not documented
);

int
    ipr_demo_portal_response_print (
ipr_demo_portal_t * self,               //  Not documented
char * string                           //  Not documented
);

int
    ipr_demo_portal_response_mogrify (
ipr_demo_portal_t * self,               //  Not documented
char * string,                          //  Not documented
ipr_bucket_t * bucket,                  //  Not documented
Bool option                             //  Not documented
);

int
    ipr_demo_portal_ready (
ipr_demo_portal_t * self                //  Reference to object
);

void
    ipr_demo_portal_selftest (
void);

void
    ipr_demo_portal_terminate (
void);

#define ipr_demo_portal_show(item,opcode,trace_file)  ipr_demo_portal_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_demo_portal_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_demo_portal_destroy(self)   ipr_demo_portal_destroy_ (self, __FILE__, __LINE__)
void
    ipr_demo_portal_destroy_ (
ipr_demo_portal_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_demo_portal_read_lock (
ipr_demo_portal_t * self                //  Reference to object
);

int
    ipr_demo_portal_write_lock (
ipr_demo_portal_t * self                //  Reference to object
);

int
    ipr_demo_portal_unlock (
ipr_demo_portal_t * self                //  Reference to object
);

#define ipr_demo_portal_link(self)      ipr_demo_portal_link_ (self, __FILE__, __LINE__)
ipr_demo_portal_t *
    ipr_demo_portal_link_ (
ipr_demo_portal_t * self,               //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_demo_portal_unlink(self)    ipr_demo_portal_unlink_ (self, __FILE__, __LINE__)
void
    ipr_demo_portal_unlink_ (
ipr_demo_portal_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_demo_portal_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_demo_portal_new_in_scope(self_p,_scope,server_object,server_link,server_unlink,name)  ipr_demo_portal_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, server_object, server_link, server_unlink, name)
void
    ipr_demo_portal_new_in_scope_ (
ipr_demo_portal_t * * self_p,           //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
void * server_object,                   //  Not documented
untyped_link_fn * server_link,          //  Not documented
untyped_unlink_fn * server_unlink,      //  Not documented
char * name                             //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DEMO_PORTAL_INLINE)
#   define INCLUDE_IPR_DEMO_PORTAL_INLINE
#   define INCLUDE_IPR_DEMO_PORTAL_ACTIVE

#   include "ipr_bucket.h"
#   include "ipr_looseref.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DEMO_PORTAL_ROOT
#   endif
# endif
#endif
