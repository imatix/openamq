/*---------------------------------------------------------------------------
    ipr_demo_portal_client.h - ipr_demo_portal_client component

    Generated from ipr_demo_portal_client.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DEMO_PORTAL_CLIENT_SAFE
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_SAFE
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DEMO_PORTAL_CLIENT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_demo_portal_client_t ipr_demo_portal_client_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
#    define IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH 32
#endif

#define IPR_DEMO_PORTAL_CLIENT_ALIVE    0xFABB
#define IPR_DEMO_PORTAL_CLIENT_DEAD     0xDEAD

#define IPR_DEMO_PORTAL_CLIENT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_demo_portal_client\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DEMO_PORTAL_CLIENT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_demo_portal_client at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DEMO_PORTAL_CLIENT_ALIVE, self->object_tag);\
        ipr_demo_portal_client_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_demo_portal_server.h"
#   include "ipr_demo_portal.h"
#   include "ipr_looseref.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_CLIENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DEMO_PORTAL_CLIENT_UNSAFE
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_UNSAFE
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE

#   include "ipr_demo_portal_server.h"
#   include "ipr_demo_portal.h"
#   include "ipr_looseref.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_demo_portal_client_animating;

//  Structure of the ipr_demo_portal_client object

struct _ipr_demo_portal_client_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH];
    int
        history_line  [IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH];
    char
        *history_type [IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH];
    int
        history_links [IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_looseref_list_t
    *ipr_demo_portal_list,          //  List of all own portals
    *ipr_demo_portal_factory_list;  //  List of all own portal factories
Bool
    ipr_demo_portal_stopped;        //  Is this portal stopped?
ipr_demo_portal_t
    *portal;                        //  Client's portal
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    ipr_demo_portal_client_selftest (
void);

#define ipr_demo_portal_client_new()    ipr_demo_portal_client_new_ (__FILE__, __LINE__)
ipr_demo_portal_client_t *
    ipr_demo_portal_client_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    ipr_demo_portal_client__ipr_demo_portal_bind (
ipr_demo_portal_client_t * self,        //  Reference to object
ipr_demo_portal_t * ipr_demo_portal     //  Portal reference
);

void
    ipr_demo_portal_client_terminate (
void);

#define ipr_demo_portal_client_show(item,opcode,trace_file)  ipr_demo_portal_client_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_demo_portal_client_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_demo_portal_client_destroy(self)  ipr_demo_portal_client_destroy_ (self, __FILE__, __LINE__)
void
    ipr_demo_portal_client_destroy_ (
ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_demo_portal_client_link(self)  ipr_demo_portal_client_link_ (self, __FILE__, __LINE__)
ipr_demo_portal_client_t *
    ipr_demo_portal_client_link_ (
ipr_demo_portal_client_t * self,        //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_demo_portal_client_unlink(self)  ipr_demo_portal_client_unlink_ (self, __FILE__, __LINE__)
void
    ipr_demo_portal_client_unlink_ (
ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_demo_portal_client_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_demo_portal_client_new_in_scope(self_p,_scope)  ipr_demo_portal_client_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_demo_portal_client_new_in_scope_ (
ipr_demo_portal_client_t * * self_p,    //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_CLIENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DEMO_PORTAL_CLIENT_INLINE)
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_INLINE
#   define INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE

#   include "ipr_demo_portal_server.h"
#   include "ipr_demo_portal.h"
#   include "ipr_looseref.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DEMO_PORTAL_CLIENT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DEMO_PORTAL_CLIENT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DEMO_PORTAL_CLIENT_ROOT
#   endif
# endif
#endif
