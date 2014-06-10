/*---------------------------------------------------------------------------
    demo_lease.h - demo_lease component

    Implements the Direct Protocol lease for the demo server.
    Creates and resolves leases for sinks (exchanges) and
    feeds (queues).
    Generated from demo_lease.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_LEASE_SAFE
#   define INCLUDE_DEMO_LEASE_SAFE
#   define INCLUDE_DEMO_LEASE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_LEASE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_lease_t demo_lease_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_LEASE)
#    define DEMO_LEASE_HISTORY_LENGTH 32
#endif

#define DEMO_LEASE_ALIVE                0xFABB
#define DEMO_LEASE_DEAD                 0xDEAD

#define DEMO_LEASE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_lease\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_LEASE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_lease at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_LEASE_ALIVE, self->object_tag);\
        demo_lease_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "demo_server_classes.h"
#   include "demo_lease_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_LEASE_ACTIVE
#   if defined (ICL_IMPORT_DEMO_LEASE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_LEASE_UNSAFE
#   define INCLUDE_DEMO_LEASE_UNSAFE
#   define INCLUDE_DEMO_LEASE_ACTIVE

#   include "demo_server_classes.h"
#   include "demo_lease_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    demo_lease_animating;

//  Structure of the demo_lease object

struct _demo_lease_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_LEASE)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [DEMO_LEASE_HISTORY_LENGTH];
    int
        history_line  [DEMO_LEASE_HISTORY_LENGTH];
    char
        *history_type [DEMO_LEASE_HISTORY_LENGTH];
    int
        history_links [DEMO_LEASE_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
demo_lease_table_t
    *table_head;                    //  Hash table container
demo_lease_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
demo_server_connection_t
    *connection;                    //  Parent connection
demo_server_channel_t
    *channel;                       //  Parent channel
smt_thread_t
    *thread;                        //  Connection thread, if lease used
icl_shortstr_t
    name;                           //  Field name
byte
    type;                           //  Field type
demo_exchange_t
    *sink;                          //  Sink we are using
demo_queue_t
    *feed;                          //  Feed for the lease
volatile qbyte
    pending;                        //  Number of pending outgoing messages
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define demo_lease_new(name,type,channel)  demo_lease_new_ (__FILE__, __LINE__, name, type, channel)
demo_lease_t *
    demo_lease_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * name,                            //  Sink or feed name
int type,                               //  DP_SINK or DP_FEED
demo_server_channel_t * channel         //  Parent channel
);

demo_lease_t *
    demo_lease_search (
char * name                             //  Exchange name
);

int
    demo_lease_acquire (
demo_lease_t * self,                    //  Reference to object
smt_thread_t * thread,                  //  Not documented
smt_socket_t * socket                   //  Not documented
);

int
    demo_lease_sink (
demo_lease_t * self,                    //  Reference to object
demo_content_basic_t * content,         //  Not documented
byte options                            //  Publish options octet
);

void
    demo_lease_selftest (
void);

void
    demo_lease_remove_from_all_containers (
demo_lease_t * self                     //  The item
);

#define demo_lease_show(item,opcode,trace_file)  demo_lease_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_lease_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_lease_destroy(self)        demo_lease_destroy_ (self, __FILE__, __LINE__)
void
    demo_lease_destroy_ (
demo_lease_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define demo_lease_link(self)           demo_lease_link_ (self, __FILE__, __LINE__)
demo_lease_t *
    demo_lease_link_ (
demo_lease_t * self,                    //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_lease_unlink(self)         demo_lease_unlink_ (self, __FILE__, __LINE__)
void
    demo_lease_unlink_ (
demo_lease_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    demo_lease_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define demo_lease_new_in_scope(self_p,_scope,name,type,channel)  demo_lease_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, name, type, channel)
void
    demo_lease_new_in_scope_ (
demo_lease_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * name,                            //  Sink or feed name
int type,                               //  DP_SINK or DP_FEED
demo_server_channel_t * channel         //  Parent channel
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_LEASE_ACTIVE
#   if defined (ICL_IMPORT_DEMO_LEASE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_LEASE_INLINE)
#   define INCLUDE_DEMO_LEASE_INLINE
#   define INCLUDE_DEMO_LEASE_ACTIVE

#   include "demo_server_classes.h"
#   include "demo_lease_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_LEASE_ACTIVE
#   if defined (ICL_IMPORT_DEMO_LEASE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_LEASE_ROOT
#   endif
# endif
#endif
