/*---------------------------------------------------------------------------
    http_response.h - http_response component

    This class holds properties and methods for representing an HTTP
    response and turning it into HTML to send back to the client.  The
    response is the primary object exchanged between all components,
    and it holds references to all other key objects.
    Generated from http_response.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_RESPONSE_SAFE
#   define INCLUDE_HTTP_RESPONSE_SAFE
#   define INCLUDE_HTTP_RESPONSE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_RESPONSE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_response_t http_response_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
#    define HTTP_RESPONSE_HISTORY_LENGTH 32
#endif

#define HTTP_RESPONSE_ALIVE             0xFABB
#define HTTP_RESPONSE_DEAD              0xDEAD

#define HTTP_RESPONSE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_response\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_RESPONSE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_response at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_RESPONSE_ALIVE, self->object_tag);\
        http_response_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_RESPONSE_ACTIVE
#   if defined (ICL_IMPORT_HTTP_RESPONSE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_RESPONSE_UNSAFE
#   define INCLUDE_HTTP_RESPONSE_UNSAFE
#   define INCLUDE_HTTP_RESPONSE_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    http_response_animating;

//  Structure of the http_response object

struct _http_response_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [HTTP_RESPONSE_HISTORY_LENGTH];
    int
        history_line  [HTTP_RESPONSE_HISTORY_LENGTH];
    char
        *history_type [HTTP_RESPONSE_HISTORY_LENGTH];
    int
        history_links [HTTP_RESPONSE_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
//  todo replace server->content_types with mime mapper
//  No more need to peek into server object
http_server_t
    *server;                        //  Parent server object
http_request_t
    *request;                       //  Tied request object
uint
    reply_code;                     //  Reply code, if not ok
icl_shortstr_t
    reply_text;                     //  Reason for error, if any
ipr_dict_table_t
    *headers;                       //  HTTP response header fields
ipr_bucket_list_t
    *bucket_list;                   //  Bucket list to return
char
    *filename;                      //  Filename to return, or
icl_shortstr_t
    content_type;                   //  Content type for response
size_t
    content_length;                 //  Content length for response
icl_shortstr_t
    hostname,                       //  Host name to use
    root_uri;                       //  URI to root path
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_response_new(server,request)  http_response_new_ (__FILE__, __LINE__, server, request)
http_response_t *
    http_response_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server,                 //  Parent server
http_request_t * request                //  Request context
);

int
    http_response_set_header (
http_response_t * self,                 //  Reference to object
char * header_name,                     //  Name of header, case insensitive
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    http_response_set_header_v (
http_response_t * self,                 //  Reference to object
char * header_name,                     //  Name of header, case insensitive
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

int
    http_response_set_from_file (
http_response_t * self,                 //  Reference to object
char * filename                         //  File to send
);

int
    http_response_set_from_bucket (
http_response_t * self,                 //  Reference to object
ipr_bucket_t * bucket,                  //  Bucket to return
char * content_type                     //  Content type
);

int
    http_response_set_from_xml_str (
http_response_t * self,                 //  Reference to object
char * xml_string                       //  XML to return
);

int
    http_response_set_no_cache (
http_response_t * self                  //  Reference to object
);

int
    http_response_set_content_type (
http_response_t * self,                 //  Reference to object
char * content_type                     //  Content type
);

int
    http_response_set_redirect (
http_response_t * self,                 //  Reference to object
char * new_uri                          //  New URI location
);

int
    http_response_set_error (
http_response_t * self,                 //  Reference to object
uint reply_code,                        //  Reply code
char * reply_text,                      //  Reply text
...                                     //  Variable arguments
);

int
    http_response_set_error_v (
http_response_t * self,                 //  Reference to object
uint reply_code,                        //  Reply code
char * reply_text,                      //  Reply text
va_list args                            //  Variable arguments
);

int
    http_response_format_reply (
http_response_t * self                  //  Reference to object
);

void
    http_response_selftest (
void);

void
    http_response_terminate (
void);

#define http_response_show(item,opcode,trace_file)  http_response_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_response_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_response_destroy(self)     http_response_destroy_ (self, __FILE__, __LINE__)
void
    http_response_destroy_ (
http_response_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define http_response_link(self)        http_response_link_ (self, __FILE__, __LINE__)
http_response_t *
    http_response_link_ (
http_response_t * self,                 //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_response_unlink(self)      http_response_unlink_ (self, __FILE__, __LINE__)
void
    http_response_unlink_ (
http_response_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_response_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_response_new_in_scope(self_p,_scope,server,request)  http_response_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, server, request)
void
    http_response_new_in_scope_ (
http_response_t * * self_p,             //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server,                 //  Parent server
http_request_t * request                //  Request context
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_RESPONSE_ACTIVE
#   if defined (ICL_IMPORT_HTTP_RESPONSE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_RESPONSE_INLINE)
#   define INCLUDE_HTTP_RESPONSE_INLINE
#   define INCLUDE_HTTP_RESPONSE_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_RESPONSE_ACTIVE
#   if defined (ICL_IMPORT_HTTP_RESPONSE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_RESPONSE_ROOT
#   endif
# endif
#endif
