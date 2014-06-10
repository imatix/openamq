/*---------------------------------------------------------------------------
    demo_content_basic.h - demo_content_basic component

    Generated from demo_content_basic.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_CONTENT_BASIC_SAFE
#   define INCLUDE_DEMO_CONTENT_BASIC_SAFE
#   define INCLUDE_DEMO_CONTENT_BASIC_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_CONTENT_BASIC_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _demo_content_basic_t demo_content_basic_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CONTENT_BASIC)
#    define DEMO_CONTENT_BASIC_HISTORY_LENGTH 32
#endif

#define DEMO_CONTENT_BASIC_ALIVE        0xFABB
#define DEMO_CONTENT_BASIC_DEAD         0xDEAD

#define DEMO_CONTENT_BASIC_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL demo_content_basic\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != DEMO_CONTENT_BASIC_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "demo_content_basic at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, DEMO_CONTENT_BASIC_ALIVE, self->object_tag);\
        demo_content_basic_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CONTENT_BASIC_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONTENT_BASIC_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_CONTENT_BASIC_UNSAFE
#   define INCLUDE_DEMO_CONTENT_BASIC_UNSAFE
#   define INCLUDE_DEMO_CONTENT_BASIC_ACTIVE

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    demo_content_basic_animating;

//  Structure of the demo_content_basic object

struct _demo_content_basic_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CONTENT_BASIC)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [DEMO_CONTENT_BASIC_HISTORY_LENGTH];
    int
        history_line  [DEMO_CONTENT_BASIC_HISTORY_LENGTH];
    char
        *history_type [DEMO_CONTENT_BASIC_HISTORY_LENGTH];
    int
        history_links [DEMO_CONTENT_BASIC_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
dbyte
    class_id,                       //  Content class
    weight;                         //  Content weight
ipr_bucket_list_t
    *bucket_list;                   //  List of message buckets
int64_t
    body_size,                      //  Size of content body data
    body_expect;                    //  Expected size of body data
byte
    *body_data;                     //  Body set by application
icl_mem_free_fn
    *free_fn;                       //  Function to free appl_body
icl_shortstr_t
    exchange,                       //  Exchange specified by publish
    routing_key,                    //  Routing key specified by publish
    consumer_tag,                   //  Consumer tag specified by consume
    producer_id;                    //  Connection identifier of producer
int64_t
    delivery_tag;                   //  Delivery tag from queue

//  Used internally by servers processing the content
Bool
    immediate,                      //  Immediate delivery wanted
    redelivered,                    //  Content has been redelivered
    returned;                       //  Content was returned

//  Content properties
icl_shortstr_t
    content_type;                   //  MIME content type
icl_shortstr_t
    content_encoding;               //  MIME content encoding
icl_longstr_t *
    headers;                        //  Message header field table
icl_shortstr_t
    reply_to;                       //  The destination to reply to
icl_shortstr_t
    message_id;                     //  The application message identifier
icl_shortstr_t
    correlation_id;                 //  The application correlation identifier
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define demo_content_basic_new()        demo_content_basic_new_ (__FILE__, __LINE__)
demo_content_basic_t *
    demo_content_basic_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    demo_content_basic_record_header (
demo_content_basic_t * self,            //  Not documented
ipr_bucket_t * bucket                   //  Bucket containing header
);

ipr_bucket_t *
    demo_content_basic_replay_header (
demo_content_basic_t * self             //  Reference to content
);

int
    demo_content_basic_record_body (
demo_content_basic_t * self,            //  Reference to object
ipr_bucket_t * bucket                   //  Bucket of raw data
);

int
    demo_content_basic_set_reader (
demo_content_basic_t * self,            //  Reference to object
asl_reader_t * reader,                  //  Reader to initialise
size_t frame_max                        //  Maximum frame size
);

ipr_bucket_t *
    demo_content_basic_replay_body (
demo_content_basic_t * self,            //  Reference to content
asl_reader_t * reader                   //  Reader to use
);

int
    demo_content_basic_set_body (
demo_content_basic_t * self,            //  Reference to object
void * data,                            //  Data for message content
size_t size,                            //  Length of message content
icl_mem_free_fn * free_fn               //  Function to free the memory
);

int
    demo_content_basic_get_body (
demo_content_basic_t * self,            //  Reference to object
byte * buffer,                          //  Buffer for message content
size_t maxsize                          //  Maximum size of buffer
);

int
    demo_content_basic_save_body (
demo_content_basic_t * self,            //  Reference to object
char * filename                         //  File to save to
);

int
    demo_content_basic_set_routing_key (
demo_content_basic_t * self,            //  Reference to object
char * exchange,                        //  Root exchange
char * routing_key,                     //  Routing key
char * producer_id                      //  Producer connection id
);

dbyte
    demo_content_basic_get_class_id (
demo_content_basic_t * self             //  Object reference
);

dbyte
    demo_content_basic_get_weight (
demo_content_basic_t * self             //  Object reference
);

int64_t
    demo_content_basic_get_body_size (
demo_content_basic_t * self             //  Object reference
);

char *
    demo_content_basic_get_exchange (
demo_content_basic_t * self             //  Object reference
);

char *
    demo_content_basic_get_routing_key (
demo_content_basic_t * self             //  Object reference
);

char *
    demo_content_basic_get_producer_id (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_content_type (
demo_content_basic_t * self,            //  Reference to object
char * content_type,                    //  MIME content type
...                                     //  Variable arguments
);

int
    demo_content_basic_set_content_type_v (
demo_content_basic_t * self,            //  Reference to object
char * content_type,                    //  MIME content type
va_list args                            //  Variable arguments
);

char *
    demo_content_basic_get_content_type (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_content_encoding (
demo_content_basic_t * self,            //  Reference to object
char * content_encoding,                //  MIME content encoding
...                                     //  Variable arguments
);

int
    demo_content_basic_set_content_encoding_v (
demo_content_basic_t * self,            //  Reference to object
char * content_encoding,                //  MIME content encoding
va_list args                            //  Variable arguments
);

char *
    demo_content_basic_get_content_encoding (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_headers (
demo_content_basic_t * self,            //  Reference to object
icl_longstr_t * headers                 //  Message header field table
);

icl_longstr_t *
    demo_content_basic_get_headers (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_reply_to (
demo_content_basic_t * self,            //  Reference to object
char * reply_to,                        //  The destination to reply to
...                                     //  Variable arguments
);

int
    demo_content_basic_set_reply_to_v (
demo_content_basic_t * self,            //  Reference to object
char * reply_to,                        //  The destination to reply to
va_list args                            //  Variable arguments
);

char *
    demo_content_basic_get_reply_to (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_message_id (
demo_content_basic_t * self,            //  Reference to object
char * message_id,                      //  The application message identifier
...                                     //  Variable arguments
);

int
    demo_content_basic_set_message_id_v (
demo_content_basic_t * self,            //  Reference to object
char * message_id,                      //  The application message identifier
va_list args                            //  Variable arguments
);

char *
    demo_content_basic_get_message_id (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_correlation_id (
demo_content_basic_t * self,            //  Reference to object
char * correlation_id,                  //  The application correlation identifier
...                                     //  Variable arguments
);

int
    demo_content_basic_set_correlation_id_v (
demo_content_basic_t * self,            //  Reference to object
char * correlation_id,                  //  The application correlation identifier
va_list args                            //  Variable arguments
);

char *
    demo_content_basic_get_correlation_id (
demo_content_basic_t * self             //  Object reference
);

int
    demo_content_basic_set_headers_field (
demo_content_basic_t * self,            //  Reference to object
char * name,                            //  Field name
char * value,                           //  Field value
...                                     //  Variable arguments
);

int
    demo_content_basic_set_headers_field_v (
demo_content_basic_t * self,            //  Reference to object
char * name,                            //  Field name
char * value,                           //  Field value
va_list args                            //  Variable arguments
);

int
    demo_content_basic_wire_put (
demo_content_basic_t * self,            //  Reference to object
ipr_bucket_t * bucket,                  //  Bucket to fill
byte options                            //  Options octet
);

size_t
    demo_content_basic_wire_size (
demo_content_basic_t * self             //  Not documented
);

demo_content_basic_t *
    demo_content_basic_wire_get (
ipr_bucket_t * bucket,                  //  Not documented
size_t * offset,                        //  Starting, ending offset in bucket
byte * options_p                        //  Options octet address
);

int
    demo_content_basic_wire_pending (
ipr_bucket_t * bucket                   //  Not documented
);

int
    demo_content_basic_chrono_set (
demo_content_basic_t * self             //  Reference to object
);

int
    demo_content_basic_chrono_add (
demo_content_basic_t * self             //  Reference to object
);

int
    demo_content_basic_chrono_get (
demo_content_basic_t * self,            //  Reference to object
char * deltas,                          //  Holds deltas
int floor                               //  Only if final delta GE this
);

void
    demo_content_basic_selftest (
void);

qbyte
    demo_content_basic_count (
void);

void
    demo_content_basic_terminate (
void);

#define demo_content_basic_show(item,opcode,trace_file)  demo_content_basic_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    demo_content_basic_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_content_basic_destroy(self)  demo_content_basic_destroy_ (self, __FILE__, __LINE__)
void
    demo_content_basic_destroy_ (
demo_content_basic_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define demo_content_basic_link(self)   demo_content_basic_link_ (self, __FILE__, __LINE__)
demo_content_basic_t *
    demo_content_basic_link_ (
demo_content_basic_t * self,            //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_content_basic_unlink(self)  demo_content_basic_unlink_ (self, __FILE__, __LINE__)
void
    demo_content_basic_unlink_ (
demo_content_basic_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    demo_content_basic_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define demo_content_basic_new_in_scope(self_p,_scope)  demo_content_basic_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    demo_content_basic_new_in_scope_ (
demo_content_basic_t * * self_p,        //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CONTENT_BASIC_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONTENT_BASIC_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_CONTENT_BASIC_INLINE)
#   define INCLUDE_DEMO_CONTENT_BASIC_INLINE
#   define INCLUDE_DEMO_CONTENT_BASIC_ACTIVE

#   include "asl.h"
#   include "asl_field.h"
#   include "demo_constants.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CONTENT_BASIC_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONTENT_BASIC_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_CONTENT_BASIC_ROOT
#   endif
# endif
#endif
