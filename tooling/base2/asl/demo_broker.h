/*---------------------------------------------------------------------------
    demo_broker.h - demo_broker component

    Generated from demo_broker.icl by smt_object_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_BROKER_SAFE
#   define INCLUDE_DEMO_BROKER_SAFE
#   define INCLUDE_DEMO_BROKER_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_BROKER_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_broker_t demo_broker_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
#    define DEMO_BROKER_HISTORY_LENGTH 32
#endif

#define DEMO_BROKER_ALIVE               0xFABB
#define DEMO_BROKER_DEAD                0xDEAD

#define DEMO_BROKER_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_broker\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_BROKER_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_broker at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_BROKER_ALIVE, self->object_tag);\
        demo_broker_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "demo_server_agent.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_broker_agent.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_BROKER_ACTIVE
#   if defined (ICL_IMPORT_DEMO_BROKER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_BROKER_UNSAFE
#   define INCLUDE_DEMO_BROKER_UNSAFE
#   define INCLUDE_DEMO_BROKER_ACTIVE

#   include "demo_server_agent.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_broker_agent.h"
//  Global variables

extern Bool
    demo_broker_animating;
extern demo_broker_t
    *demo_broker;                       //  Single broker, self

//  Structure of the demo_broker object

struct _demo_broker_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_BROKER)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [DEMO_BROKER_HISTORY_LENGTH];
    int
        history_line  [DEMO_BROKER_HISTORY_LENGTH];
    char
        *history_type [DEMO_BROKER_HISTORY_LENGTH];
    int
        history_links [DEMO_BROKER_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;
demo_server_connection_table_t
    *connections;                   //  Connection table
demo_server_connection_list_t
    *connection_list;               //  Connection list
smt_thread_t
    *agent_thread;                  //  demo_server_server_agent thread
apr_time_t
    started;                        //  Time started
icl_shortstr_t
    host;                           //  Server network address
icl_shortstr_t
    name,                           //  Server name
    identifier;                     //  Unique identifier
ipr_meter_t
    *imeter,                        //  Incoming messages meter
    *ometer;                        //  Outgoing messages meter
int
    monitor_timer;                  //  Monitor timer
FILE
    *stats;                         //  Statistics output
smt_log_t
    *alert_log,                     //  Alert log file
    *daily_log,                     //  Daily log file
    *debug_log;                     //  Debug log file
Bool
    cycled;                         //  Log files just cycled?
qbyte
    direct_in,                      //  Direct messages received
    direct_out;                     //  Direct messages sent out
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define demo_broker_new()               demo_broker_new_ (__FILE__, __LINE__)
demo_broker_t *
    demo_broker_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_broker_free(self)          demo_broker_free_ (self, __FILE__, __LINE__)
void
    demo_broker_free_ (
demo_broker_t * self,                   //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

int
    demo_broker_start (
demo_broker_t * self                    //  Reference to object
);

int
    demo_broker_shutdown (
demo_broker_t * self                    //  Reference to object
);

void
    demo_broker_selftest (
void);

#define demo_broker_destroy(self)       demo_broker_destroy_ (self, __FILE__, __LINE__)
int
    demo_broker_destroy_ (
demo_broker_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_broker_unlink(self)        demo_broker_unlink_ (self, __FILE__, __LINE__)
void
    demo_broker_unlink_ (
demo_broker_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    demo_broker_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

void
    demo_broker_terminate (
void);

#define demo_broker_show(item,opcode,trace_file)  demo_broker_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_broker_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_broker_link(self)          demo_broker_link_ (self, __FILE__, __LINE__)
demo_broker_t *
    demo_broker_link_ (
demo_broker_t * self,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_broker_new_in_scope(self_p,_scope)  demo_broker_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    demo_broker_new_in_scope_ (
demo_broker_t * * self_p,               //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_BROKER_ACTIVE
#   if defined (ICL_IMPORT_DEMO_BROKER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_BROKER_INLINE)
#   define INCLUDE_DEMO_BROKER_INLINE
#   define INCLUDE_DEMO_BROKER_ACTIVE

#   include "demo_server_agent.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_broker_agent.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_BROKER_ACTIVE
#   if defined (ICL_IMPORT_DEMO_BROKER_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_BROKER_ROOT
#   endif
# endif
#endif
