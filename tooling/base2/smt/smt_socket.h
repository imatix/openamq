/*---------------------------------------------------------------------------
    smt_socket.h - smt_socket component

    The smt_socket class is used to create socket objects for use with the
    smt_socket_request class.
    Generated from smt_socket.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_SOCKET_SAFE
#   define INCLUDE_SMT_SOCKET_SAFE
#   define INCLUDE_SMT_SOCKET_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_SOCKET_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_socket_t smt_socket_t;

#define SMT_SOCKET_ALIVE                0xFABB
#define SMT_SOCKET_DEAD                 0xDEAD

#define SMT_SOCKET_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_socket\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_SOCKET_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_socket at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_SOCKET_ALIVE, self->object_tag);\
        smt_socket_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#include "base_apr.h"

#define SMT_SOCKET_VECTOR_MAX        16
#   ifdef __cplusplus
}
#   endif

#   include "smt_thread.h"
#   include "smt_socket_request.h"
#   include "smt_timer_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_SOCKET_UNSAFE
#   define INCLUDE_SMT_SOCKET_UNSAFE
#   define INCLUDE_SMT_SOCKET_ACTIVE

#   include "smt_thread.h"
#   include "smt_socket_request.h"
#   include "smt_timer_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_socket_animating;

//  Structure of the smt_socket object

struct _smt_socket_t {
dbyte
    object_tag;                     //  Object validity marker
size_t       io_size;
smt_socket_request_by_socket_t
             *request_list;
apr_pollfd_t pollfd;
Bool         trace;                 //  Trace this socket?
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define smt_socket_new()                smt_socket_new_ (__FILE__, __LINE__)
smt_socket_t *
    smt_socket_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    smt_socket_connect (
smt_socket_t * self,                    //  Reference to object
smt_thread_t * thread,                  //  Thread to deliver reply to
qbyte msecs,                            //  Timeout for connection
const char * host,                      //  Host to connect to
const char * service,                   //  Service to connect to
smt_event_t event                       //  Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking
);

int
    smt_socket_passive (
smt_socket_t * self,                    //  Reference to object
smt_thread_t * thread,                  //  Thread to deliver reply to
const char * service,                   //  Service to listen on
const char * ipaddress,                 //  IP address to bind to
int queue_length                        //  Length of connect queue
);

int
    smt_socket_set_nodelay (
smt_socket_t * self,                    //  Reference to object
int value                               //  Option value
);

int
    smt_socket_set_rcvbuf (
smt_socket_t * self,                    //  Reference to object
int new_size,                           //  New buffer size
int * old_size                          //  Old buffer size
);

int
    smt_socket_set_sndbuf (
smt_socket_t * self,                    //  Reference to object
int new_size,                           //  New buffer size
int * old_size                          //  Old buffer size
);

char *
    smt_socket_peeraddr (
smt_socket_t * self                     //  Not documented
);

int
    smt_socket_trace (
smt_socket_t * self,                    //  Reference to object
Bool enabled                            //  Not documented
);

void
    smt_socket_selftest (
void);

void
    smt_socket_terminate (
void);

#define smt_socket_show(item,opcode,trace_file)  smt_socket_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_socket_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_socket_destroy(self)        smt_socket_destroy_ (self, __FILE__, __LINE__)
void
    smt_socket_destroy_ (
smt_socket_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    smt_socket_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_socket_new_in_scope(self_p,_scope)  smt_socket_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    smt_socket_new_in_scope_ (
smt_socket_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_SOCKET_INLINE)
#   define INCLUDE_SMT_SOCKET_INLINE
#   define INCLUDE_SMT_SOCKET_ACTIVE

#   include "smt_thread.h"
#   include "smt_socket_request.h"
#   include "smt_timer_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_SOCKET_ROOT
#   endif
# endif
#endif
