/*---------------------------------------------------------------------------
    smt_socket_request.h - smt_socket_request component

    The smt_socket_request class contains all the methods for doing activity
    on a connected socket.  The socket may be connected by smt_socket_connect
    or a slave socket created when by remote connection to a passively opened
    socket.

    You do not typically need to manipulate smt_socket_request objects; once
    created they do the work without further intervention by the application.
    Generated from smt_socket_request.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_SMT_SOCKET_REQUEST_SAFE
#   define INCLUDE_SMT_SOCKET_REQUEST_SAFE
#   define INCLUDE_SMT_SOCKET_REQUEST_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_SMT_SOCKET_REQUEST_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _smt_socket_request_t smt_socket_request_t;

#define SMT_SOCKET_REQUEST_ALIVE        0xFABB
#define SMT_SOCKET_REQUEST_DEAD         0xDEAD

#define SMT_SOCKET_REQUEST_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL smt_socket_request\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != SMT_SOCKET_REQUEST_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "smt_socket_request at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, SMT_SOCKET_REQUEST_ALIVE, self->object_tag);\
        smt_socket_request_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
typedef enum {
    SOCK_NULL_REQUEST,
    SOCK_ACCEPT,
    SOCK_READ,
    SOCK_CONNECT,
    SOCK_WRITE,
    SOCK_CLOSE,
    SOCK_MONITOR
} smt_socket_request_type_t;

#include "base_apr.h"
#   ifdef __cplusplus
}
#   endif

#   include "ipr_bucket.h"
#   include "icl_shortstr.h"
#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_timer_request.h"
#   include "smt_socket_request_by_thread.h"
#   include "smt_socket_request_by_socket.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_SMT_SOCKET_REQUEST_UNSAFE
#   define INCLUDE_SMT_SOCKET_REQUEST_UNSAFE
#   define INCLUDE_SMT_SOCKET_REQUEST_ACTIVE

#   include "ipr_bucket.h"
#   include "icl_shortstr.h"
#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_timer_request.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    smt_socket_request_animating;

//  Structure of the smt_socket_request object

struct _smt_socket_request_t {
smt_socket_request_t
    *local_next;
dbyte
    object_tag;                     //  Object validity marker
volatile smt_socket_request_by_thread_t *by_thread_head;
volatile smt_socket_request_t *by_thread_prev;
volatile smt_socket_request_t *by_thread_next;
volatile smt_socket_request_by_socket_t *by_socket_head;
volatile smt_socket_request_t *by_socket_prev;
volatile smt_socket_request_t *by_socket_next;
smt_thread_t         *thread;       //  Thread to reply to                 
smt_os_thread_t      *os_thread;    //  OS thread holding pollset          
smt_socket_request_type_t
                     type;          //  Type of request                    
smt_event_t          input_event;   //  Event to send on completion        
smt_event_t          output_event;  //  Event to send on completion        
smt_socket_t         *socket;       //  Socket for request                 
smt_socket_t         **socket_p;    //  Pointer for accept/close           
byte                 *buffer;       //  i/o buffer                         
size_t               max_size;      //  Maximum size of buffer             
size_t               min_size;      //  Minimum data to process
size_t               cur_size;      //  Minimum data to process
qbyte                msecs;         //  Timeout for this request
smt_timer_request_t  *timer_request;//  Corresponding timer                
ipr_bucket_t         *bucket;       //  Bucket used as buffer
ipr_mother_t         *mother;       //  Mother object for no-copy read
ipr_bucket_t         **slave_p;     //  Pointer to slave bucket to allocate
};

#   ifdef __cplusplus
}
#   endif
#   include "smt_socket_request_by_thread.h"
#   include "smt_socket_request_by_socket.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    smt_socket_request_initialise (
void);

#define smt_socket_request_new(thread,type,socket,msecs,input_event,output_event)  smt_socket_request_new_ (thread, type, socket, msecs, input_event, output_event, __FILE__, __LINE__)
smt_socket_request_t *
    smt_socket_request_new_ (
smt_thread_t * thread,                  //  Not documented
smt_socket_request_type_t type,         //  Not documented
smt_socket_t * socket,                  //  Not documented
qbyte msecs,                            //  Not documented
smt_event_t input_event,                //  Not documented
smt_event_t output_event,               //  Not documented
char * file,                            //  Source file for callSource file for call
size_t line                             //  Line number for callLine number for call
);

void
    smt_socket_request_timer (
smt_socket_request_t * self             //  The request
);

smt_socket_request_t *
    smt_socket_request_accept (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Passively opened socket to accept connection
qbyte msecs,                            //  Timeout for connection
smt_event_t event,                      //  Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking
smt_socket_t ** connect                 //  Pointer to result of passive connection
);

#define smt_socket_request_destroy(self)  smt_socket_request_destroy_ (self, __FILE__, __LINE__)
void
    smt_socket_request_destroy_ (
smt_socket_request_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    smt_socket_request_return (
smt_socket_request_t * ( * self_p ),    //  Not documented
int result,                             //  Not documented
int error                               //  Not documented
);

smt_socket_request_t *
    smt_socket_request_monitor (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to monitor
qbyte msecs,                            //  Timeout
smt_event_t input_event,                //  Event to deliver on socket ready for input
smt_event_t output_event                //  Event to deliver on socket ready for output
);

smt_socket_request_t *
    smt_socket_request_read (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to read data from
qbyte msecs,                            //  Timeout for socket to be ready
size_t min_size,                        //  Minimum data size to read
size_t max_size,                        //  Maximum data size to read
byte * buffer,                          //  Buffer to store data read
smt_event_t event                       //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
);

#define smt_socket_request_read_bucket(thread,socket,msecs,min_size,max_size,bucket_p,event)  smt_socket_request_read_bucket_ (thread, socket, msecs, min_size, max_size, bucket_p, event, __FILE__, __LINE__)
smt_socket_request_t *
    smt_socket_request_read_bucket_ (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to read data from
qbyte msecs,                            //  Timeout for socket to be ready
size_t min_size,                        //  Minimum data size to read
size_t max_size,                        //  Maximum data size to read
ipr_bucket_t ** bucket_p,               //  Pointer to created bucket
smt_event_t event,                      //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_socket_request_read_mother(thread,socket,msecs,size,mother,slave_p,event)  smt_socket_request_read_mother_ (thread, socket, msecs, size, mother, slave_p, event, __FILE__, __LINE__)
smt_socket_request_t *
    smt_socket_request_read_mother_ (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to read data from
qbyte msecs,                            //  Timeout for socket to be ready
size_t size,                            //  Data size to return
ipr_mother_t * ( * mother_p ),          //  Pointer to mother
ipr_bucket_t ** slave_p,                //  Pointer to slave bucket to create
smt_event_t event,                      //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

smt_socket_request_t *
    smt_socket_request_write (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to write data to
qbyte msecs,                            //  Timeout for socket to be ready
size_t size,                            //  Data size to write
byte * buffer,                          //  Buffer holding data to write
smt_event_t event                       //  Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking
);

smt_socket_request_t *
    smt_socket_request_write_bucket (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t * socket,                  //  Connected socket to write data to
qbyte msecs,                            //  Timeout for socket to be ready
ipr_bucket_t * bucket,                  //  Data in a bucket
smt_event_t event                       //  Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking
);

smt_socket_request_t *
    smt_socket_request_close (
smt_thread_t * thread,                  //  Thread to deliver reply to
smt_socket_t ** socket,                 //  Connected socket to close
qbyte msecs,                            //  Timeout for socket to be ready
smt_event_t event                       //  Event to deliver on close complete; SMT_NULL_EVENT for simple thread-blocking
);

int
    smt_socket_request_poll (
smt_os_thread_t * os_thread,            //  Not documented
Bool force                              //  Force poll of all sockets?
);

int
    smt_socket_request_wait (
smt_os_thread_t * os_thread             //  Not documented
);

void
    smt_socket_request_trace (
Bool enabled                            //  Not documented
);

void
    smt_socket_request_selftest (
void);

smt_socket_request_t *
    smt_socket_request_write_data (
smt_socket_request_t * self             //  The request
);

void
    smt_socket_request_remove_from_all_containers (
smt_socket_request_t * self             //  The itemThe item
);

#define smt_socket_request_show(item,opcode,trace_file)  smt_socket_request_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    smt_socket_request_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    smt_socket_request_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define smt_socket_request_new_in_scope(self_p,_scope,thread,type,socket,msecs,input_event,output_event)  smt_socket_request_new_in_scope_ (self_p, _scope, thread, type, socket, msecs, input_event, output_event, __FILE__, __LINE__)
void
    smt_socket_request_new_in_scope_ (
smt_socket_request_t * * self_p,        //  Not documented
icl_scope_t * _scope,                   //  Not documented
smt_thread_t * thread,                  //  Not documented
smt_socket_request_type_t type,         //  Not documented
smt_socket_t * socket,                  //  Not documented
qbyte msecs,                            //  Not documented
smt_event_t input_event,                //  Not documented
smt_event_t output_event,               //  Not documented
char * file,                            //  Source file for callSource file for call
size_t line                             //  Line number for callLine number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_SMT_SOCKET_REQUEST_INLINE)
#   define INCLUDE_SMT_SOCKET_REQUEST_INLINE
#   define INCLUDE_SMT_SOCKET_REQUEST_ACTIVE

#   include "ipr_bucket.h"
#   include "icl_shortstr.h"
#   include "smt_os_thread.h"
#   include "smt_thread.h"
#   include "smt_socket.h"
#   include "smt_timer_request.h"
#   include "smt_socket_request_by_thread.h"
#   include "smt_socket_request_by_socket.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_SMT_SOCKET_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_SMT_SOCKET_REQUEST_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_SMT_SOCKET_REQUEST_ROOT
#   endif
# endif
#endif
