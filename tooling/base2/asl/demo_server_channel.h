/*---------------------------------------------------------------------------
    demo_server_channel.h - demo_server_channel component

    This class implements the ASL demo server channel class.
    Generated from demo_server_channel.icl by smt_object_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_SERVER_CHANNEL_SAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_SAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_SERVER_CHANNEL_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_server_channel_t demo_server_channel_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
#    define DEMO_SERVER_CHANNEL_HISTORY_LENGTH 32
#endif

#define DEMO_SERVER_CHANNEL_ALIVE       0xFABB
#define DEMO_SERVER_CHANNEL_DEAD        0xDEAD

#define DEMO_SERVER_CHANNEL_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_server_channel\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_SERVER_CHANNEL_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_server_channel at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_SERVER_CHANNEL_ALIVE, self->object_tag);\
        demo_server_channel_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "demo_server_channel_table.h"
#   include "asl.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_server_channel_agent.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_SERVER_CHANNEL_UNSAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_UNSAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE

#   include "demo_server_channel_table.h"
#   include "asl.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_server_channel_agent.h"
//  Global variables

extern Bool
    demo_server_channel_animating;

//  Structure of the demo_server_channel object

struct _demo_server_channel_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [DEMO_SERVER_CHANNEL_HISTORY_LENGTH];
    int
        history_line  [DEMO_SERVER_CHANNEL_HISTORY_LENGTH];
    char
        *history_type [DEMO_SERVER_CHANNEL_HISTORY_LENGTH];
    int
        history_links [DEMO_SERVER_CHANNEL_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;
demo_server_channel_table_t
    *table_head;                    //  Hash table container
demo_server_channel_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
int
    key;                            //  Item's original key
Bool
    active;                         //  If TRUE, channel is active
demo_server_connection_t
    *connection;                    //  Parent connection
dbyte
    number;                         //  Channel number
icl_shortstr_t
    id;                             //  Unique channel id (per broker)
volatile qbyte
    credit;                         //  Current channel credit
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    demo_server_channel_consume (
demo_server_channel_t * self,           //  Reference to object
demo_server_method_t * method           //  Consume method
);

int
    demo_server_channel_cancel (
demo_server_channel_t * self,           //  Reference to object
demo_server_method_t * method           //  Cancel method
);

#define demo_server_channel_new(table,connection,number)  demo_server_channel_new_ (__FILE__, __LINE__, table, connection, number)
demo_server_channel_t *
    demo_server_channel_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
demo_server_channel_table_t * table,    //  Table to insert into
demo_server_connection_t * connection,   //  Parent connection
dbyte number                            //  Channel number
);

int
    demo_server_channel_spend (
demo_server_channel_t * self            //  Reference to object
);

int
    demo_server_channel_earn (
demo_server_channel_t * self            //  Reference to object
);

int
    demo_server_channel_error (
demo_server_channel_t * self,           //  Reference to object
dbyte reply_code,                       //  Error code
char * reply_text,                      //  Error text
dbyte faulting_class_id,                //  Faulting class id
dbyte faulting_method_id                //  Faulting method id
);

void
    demo_server_channel_selftest (
void);

void
    demo_server_channel_remove_from_all_containers (
demo_server_channel_t * self            //  The item
);

#define demo_server_channel_show(item,opcode,trace_file)  demo_server_channel_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_server_channel_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_server_channel_destroy(self)  demo_server_channel_destroy_ (self, __FILE__, __LINE__)
int
    demo_server_channel_destroy_ (
demo_server_channel_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_server_channel_unlink(self)  demo_server_channel_unlink_ (self, __FILE__, __LINE__)
void
    demo_server_channel_unlink_ (
demo_server_channel_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    demo_server_channel_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define demo_server_channel_free(self)  demo_server_channel_free_ (self, __FILE__, __LINE__)
void
    demo_server_channel_free_ (
demo_server_channel_t * self,           //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    demo_server_channel_terminate (
void);

#define demo_server_channel_link(self)  demo_server_channel_link_ (self, __FILE__, __LINE__)
demo_server_channel_t *
    demo_server_channel_link_ (
demo_server_channel_t * self,           //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_server_channel_new_in_scope(self_p,_scope,table,connection,number)  demo_server_channel_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, connection, number)
void
    demo_server_channel_new_in_scope_ (
demo_server_channel_t * * self_p,       //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
demo_server_channel_table_t * table,    //  Table to insert into
demo_server_connection_t * connection,   //  Parent connection
dbyte number                            //  Channel number
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_SERVER_CHANNEL_INLINE)
#   define INCLUDE_DEMO_SERVER_CHANNEL_INLINE
#   define INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE

#   include "demo_server_channel_table.h"
#   include "asl.h"
#   include "demo_server_method.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "demo_server_channel_agent.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_SERVER_CHANNEL_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_SERVER_CHANNEL_ROOT
#   endif
# endif
#endif
