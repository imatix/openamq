/*---------------------------------------------------------------------------
    demo_server_connection.h - demo_server_connection component

    This class implements the ASL connection class for the demo server.
    Generated from demo_server_connection.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_SERVER_CONNECTION_SAFE
#   define INCLUDE_DEMO_SERVER_CONNECTION_SAFE
#   define INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_SERVER_CONNECTION_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_server_connection_t demo_server_connection_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
#    define DEMO_SERVER_CONNECTION_HISTORY_LENGTH 32
#endif

#define DEMO_SERVER_CONNECTION_ALIVE    0xFABB
#define DEMO_SERVER_CONNECTION_DEAD     0xDEAD

#define DEMO_SERVER_CONNECTION_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_server_connection\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_SERVER_CONNECTION_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_server_connection at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_SERVER_CONNECTION_ALIVE, self->object_tag);\
        demo_server_connection_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "demo_server_connection_table.h"
#   include "demo_server_connection_list.h"
#   include "asl.h"
#   include "demo_server_channel.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CONNECTION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_SERVER_CONNECTION_UNSAFE
#   define INCLUDE_DEMO_SERVER_CONNECTION_UNSAFE
#   define INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE

#   include "demo_server_connection_table.h"
#   include "asl.h"
#   include "demo_server_channel.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    demo_server_connection_animating;

//  Structure of the demo_server_connection object

struct _demo_server_connection_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [DEMO_SERVER_CONNECTION_HISTORY_LENGTH];
    int
        history_line  [DEMO_SERVER_CONNECTION_HISTORY_LENGTH];
    char
        *history_type [DEMO_SERVER_CONNECTION_HISTORY_LENGTH];
    int
        history_links [DEMO_SERVER_CONNECTION_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
demo_server_connection_table_t
    *table_head;                    //  Hash table container
demo_server_connection_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
volatile demo_server_connection_list_t *list_head;
volatile demo_server_connection_t *list_prev;
volatile demo_server_connection_t *list_next;
//  References to parent objects
smt_thread_t
    *thread;                        //  Parent thread

//  Object properties
apr_time_t
    started;                        //  Time started
icl_shortstr_t
    id;                             //  Connection identifier
demo_server_channel_table_t
    *channels;                      //  Active channels
dbyte
    channel_max,                    //  Negotiated channel limit
    heartbeat;                      //  Connection heartbeat
qbyte
    frame_max;                      //  Negotiated maximum frame size
Bool
    exception_raised;               //  Indicates exception raised
dbyte
    reply_code;                     //  Exception error code
icl_shortstr_t
    reply_text;                     //  Exception error text
dbyte
    faulting_method_id,             //  Class id of faulting method
    faulting_class_id;              //  Method id of faulting method
Bool
    nowarning,                      //  Suppress disconnect warnings
    exception,                      //  Indicates connection exception
    authorised;                     //  Connection authorised?
int
    trace;                          //  Last known trace value

icl_shortstr_t
    client_address;                 //  IP address:port of client
icl_shortstr_t
    client_product;                 //  Reported by client
icl_shortstr_t
    client_version;                 //  Client version
icl_shortstr_t  
    client_platform;                //  Client OS/language
icl_shortstr_t
    client_copyright;               //  Client copyright
icl_shortstr_t
    client_information;             //  Client information
icl_shortstr_t
    client_instance;                //  Client instance name

//  If the client connection comes from a server, it will also supply
//  us with information that lets make a call-back connection to it
icl_shortstr_t
    client_proxy_name;              //  Server name
icl_shortstr_t
    client_proxy_host;              //  Server address and port

//  Statistics
int64_t
    traffic_in,                     //  Traffic in, in octets
    traffic_out,                    //  Traffic out, in octets
    contents_in,                    //  Contents in
    contents_out;                   //  Contents out
};

#   ifdef __cplusplus
}
#   endif
#   include "demo_server_connection_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    demo_server_connection_start_ok (
demo_server_connection_t * self,        //  Reference to object
demo_server_connection_start_ok_t * method  //  Not documented
);

#define demo_server_connection_new(table,thread)  demo_server_connection_new_ (__FILE__, __LINE__, table, thread)
demo_server_connection_t *
    demo_server_connection_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
demo_server_connection_table_t * table,   //  Table to insert into
smt_thread_t * thread                   //  Parent thread
);

int
    demo_server_connection_set_address (
demo_server_connection_t * self,        //  Reference to object
char * address                          //  Client address
);

int
    demo_server_connection_set_trace (
demo_server_connection_t * self,        //  Reference to object
int trace                               //  Desired trace level
);

int
    demo_server_connection_kill (
demo_server_connection_t * self         //  Reference to object
);

int
    demo_server_connection_ready (
demo_server_connection_t * self         //  Reference to object
);

int
    demo_server_connection_raise_exception (
demo_server_connection_t * self,        //  Reference to object
dbyte reply_code,                       //  Error code
char * reply_text,                      //  Error text
dbyte faulting_method_id,               //  Faulting method id
dbyte faulting_class_id                 //  Faulting class id
);

int
    demo_server_connection_error (
demo_server_connection_t * self,        //  Reference to object
dbyte reply_code,                       //  Error code
char * reply_text,                      //  Error text
dbyte faulting_method_id,               //  Faulting method id
dbyte faulting_class_id                 //  Faulting class id
);

int
    demo_server_connection_exception_raised (
demo_server_connection_t * self         //  Reference to object
);

int
    demo_server_connection_tune_ok (
demo_server_connection_t * self,        //  Reference to object
demo_server_connection_tune_ok_t * method  //  Not documented
);

int
    demo_server_connection_open (
demo_server_connection_t * self,        //  Reference to object
demo_server_connection_open_t * method  //  Not documented
);

void
    demo_server_connection_selftest (
void);

qbyte
    demo_server_connection_count (
void);

void
    demo_server_connection_remove_from_all_containers (
demo_server_connection_t * self         //  The itemThe item
);

#define demo_server_connection_show(item,opcode,trace_file)  demo_server_connection_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_server_connection_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_server_connection_destroy(self)  demo_server_connection_destroy_ (self, __FILE__, __LINE__)
void
    demo_server_connection_destroy_ (
demo_server_connection_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    demo_server_connection_read_lock (
demo_server_connection_t * self         //  Reference to object
);

int
    demo_server_connection_write_lock (
demo_server_connection_t * self         //  Reference to object
);

int
    demo_server_connection_unlock (
demo_server_connection_t * self         //  Reference to object
);

#define demo_server_connection_link(self)  demo_server_connection_link_ (self, __FILE__, __LINE__)
demo_server_connection_t *
    demo_server_connection_link_ (
demo_server_connection_t * self,        //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_server_connection_unlink(self)  demo_server_connection_unlink_ (self, __FILE__, __LINE__)
void
    demo_server_connection_unlink_ (
demo_server_connection_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    demo_server_connection_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define demo_server_connection_new_in_scope(self_p,_scope,table,thread)  demo_server_connection_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, thread)
void
    demo_server_connection_new_in_scope_ (
demo_server_connection_t * * self_p,    //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
demo_server_connection_table_t * table,   //  Table to insert into
smt_thread_t * thread                   //  Parent thread
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CONNECTION_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_SERVER_CONNECTION_INLINE)
#   define INCLUDE_DEMO_SERVER_CONNECTION_INLINE
#   define INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE

#   include "demo_server_connection_table.h"
#   include "demo_server_connection_list.h"
#   include "asl.h"
#   include "demo_server_channel.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CONNECTION_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CONNECTION_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_SERVER_CONNECTION_ROOT
#   endif
# endif
#endif
