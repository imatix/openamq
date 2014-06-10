/*---------------------------------------------------------------------------
    demo_client_connection.h - demo_client_connection component

A demo_client_connection object establishes and manages a single
connection to a demo server. The connection can hold
many sessions - each is a demo_client_session object. To use this
API, first create a connection to the server, then create one or
more sessions and do work on the sessions. Each session represents
a serial stream of work. In simple applications, a single session
is usually sufficient. In multithreaded applications, each thread
will want its own session.
    Generated from demo_client_connection.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_CLIENT_CONNECTION_SAFE
#   define INCLUDE_DEMO_CLIENT_CONNECTION_SAFE
#   define INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_CLIENT_CONNECTION_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_client_connection_t demo_client_connection_t;

#define DEMO_CLIENT_CONNECTION_ALIVE    0xFABB
#define DEMO_CLIENT_CONNECTION_DEAD     0xDEAD

#define DEMO_CLIENT_CONNECTION_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_client_connection\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_CLIENT_CONNECTION_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_client_connection at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_CLIENT_CONNECTION_ALIVE, self->object_tag);\
        demo_client_connection_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "asl.h"
#   include "demo_constants.h"
#   include "demo_client_session.h"
#   include "demo_content_basic.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CLIENT_CONNECTION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_CLIENT_CONNECTION_UNSAFE
#   define INCLUDE_DEMO_CLIENT_CONNECTION_UNSAFE
#   define INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE

#   include "asl.h"
#   include "demo_constants.h"
#   include "demo_client_session.h"
#   include "demo_content_basic.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    demo_client_connection_animating;

//  Structure of the demo_client_connection object

struct _demo_client_connection_t {
dbyte
    object_tag;                     //  Object validity marker
smt_thread_t
    *thread;                        //  Thread for this connection
smt_method_queue_t
    *method_queue;                  //  Replies from protocol agent
dbyte
    channel_nbr;                    //  Top channel opened so far
Bool
    silent;                         //  Don't print errors
Bool
    alive;                          //  Connection is alive?
Bool
    interrupt;                      //  Application was interrupted?
Bool
    shutdown;                       //  We were shutdown
int
    trace,                          //  Trace level 0-3
    timeout;                        //  Connection timeout, msecs
char
    *error_text;                    //  Last error cause
Bool
    direct;                         //  Use Direct Protocol
icl_shortstr_t
    host;                           //  Host we connected to
byte
    version_major;                  //  protocol major version
byte
    version_minor;                  //  protocol major version
icl_longstr_t *
    server_properties;              //  server properties
icl_longstr_t *
    mechanisms;                     //  available security mechanisms
icl_longstr_t *
    locales;                        //  available message locales
icl_longstr_t *
    challenge;                      //  security challenge data
dbyte
    channel_max;                    //  proposed maximum channels
qbyte
    frame_max;                      //  proposed maximum frame size
dbyte
    heartbeat;                      //  desired heartbeat delay
icl_shortstr_t
    known_hosts;                    //  list of known hosts
dbyte
    reply_code;                     //  reply code from server
icl_shortstr_t
    reply_text;                     //  localised reply text
dbyte
    class_id;                       //  failing method class
dbyte
    method_id;                      //  failing method ID
icl_shortstr_t
    server_host;                    //  Reported by server
icl_shortstr_t
    server_port;                    //  Just the port number
icl_shortstr_t
    server_product;
icl_shortstr_t
    server_version;
icl_shortstr_t
    server_platform;
icl_shortstr_t
    server_copyright;
icl_shortstr_t
    server_information;
icl_shortstr_t
    server_instance;
icl_shortstr_t
    server_identifier;
icl_shortstr_t
    id;                             //  Server-assigned ID
Bool
    server_direct;                  //  Server proposes Direct Mode?
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define demo_client_connection_new(host,virtual_host,auth_data,instance,trace,timeout)  demo_client_connection_new_ (__FILE__, __LINE__, host, virtual_host, auth_data, instance, trace, timeout)
demo_client_connection_t *
    demo_client_connection_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * host,                            //  Host to connect to
char * virtual_host,                    //  Virtual host
icl_longstr_t * auth_data,              //  Authentication data
char* instance,                         //  Client instance name
int trace,                              //  Trace level, 0 - 3
int timeout                             //  Connection timeout, msecs
);

icl_longstr_t *
    demo_client_connection_auth_plain (
char * login,                           //  User login name
char * password                         //  User password
);

int
    demo_client_connection_wait (
demo_client_connection_t * self,        //  Reference to object
int timeout                             //  Timeout, in milliseconds
);

int
    demo_client_connection_set_silent (
demo_client_connection_t * self,        //  Reference to object
Bool silent                             //  Not documented
);

int
    demo_client_connection_set_timeout (
demo_client_connection_t * self,        //  Reference to object
Bool timeout                            //  Not documented
);

Bool
    demo_client_connection_get_silent (
demo_client_connection_t * self         //  Object reference
);

Bool
    demo_client_connection_get_alive (
demo_client_connection_t * self         //  Object reference
);

char *
    demo_client_connection_get_error_text (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_version_major (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_version_minor (
demo_client_connection_t * self         //  Object reference
);

icl_longstr_t *
    demo_client_connection_get_server_properties (
demo_client_connection_t * self         //  Object reference
);

icl_longstr_t *
    demo_client_connection_get_mechanisms (
demo_client_connection_t * self         //  Object reference
);

icl_longstr_t *
    demo_client_connection_get_locales (
demo_client_connection_t * self         //  Object reference
);

icl_longstr_t *
    demo_client_connection_get_challenge (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_channel_max (
demo_client_connection_t * self         //  Object reference
);

qbyte
    demo_client_connection_get_frame_max (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_heartbeat (
demo_client_connection_t * self         //  Object reference
);

char *
    demo_client_connection_get_known_hosts (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_reply_code (
demo_client_connection_t * self         //  Object reference
);

char *
    demo_client_connection_get_reply_text (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_class_id (
demo_client_connection_t * self         //  Object reference
);

int
    demo_client_connection_get_method_id (
demo_client_connection_t * self         //  Object reference
);

void
    demo_client_connection_selftest (
void);

#define demo_client_connection_show(item,opcode,trace_file)  demo_client_connection_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_client_connection_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_client_connection_destroy(self)  demo_client_connection_destroy_ (self, __FILE__, __LINE__)
void
    demo_client_connection_destroy_ (
demo_client_connection_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    demo_client_connection_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define demo_client_connection_new_in_scope(self_p,_scope,host,virtual_host,auth_data,instance,trace,timeout)  demo_client_connection_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, host, virtual_host, auth_data, instance, trace, timeout)
void
    demo_client_connection_new_in_scope_ (
demo_client_connection_t * * self_p,    //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * host,                            //  Host to connect to
char * virtual_host,                    //  Virtual host
icl_longstr_t * auth_data,              //  Authentication data
char* instance,                         //  Client instance name
int trace,                              //  Trace level, 0 - 3
int timeout                             //  Connection timeout, msecs
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CLIENT_CONNECTION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_CLIENT_CONNECTION_INLINE)
#   define INCLUDE_DEMO_CLIENT_CONNECTION_INLINE
#   define INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE

#   include "asl.h"
#   include "demo_constants.h"
#   include "demo_client_session.h"
#   include "demo_content_basic.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CLIENT_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CLIENT_CONNECTION_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_CLIENT_CONNECTION_ROOT
#   endif
# endif
#endif
