/*---------------------------------------------------------------------------
    http_request.h - http_request component

    This class holds properties and methods for parsing and representing
    an incoming HTTP request.
    Generated from http_request.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_HTTP_REQUEST_SAFE
#   define INCLUDE_HTTP_REQUEST_SAFE
#   define INCLUDE_HTTP_REQUEST_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_REQUEST_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_request_t http_request_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
#    define HTTP_REQUEST_HISTORY_LENGTH 32
#endif

#define HTTP_REQUEST_ALIVE              0xFABB
#define HTTP_REQUEST_DEAD               0xDEAD

#define HTTP_REQUEST_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL http_request\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != HTTP_REQUEST_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "http_request at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_REQUEST_ALIVE, self->object_tag);\
        http_request_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_HTTP_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_HTTP_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_REQUEST_UNSAFE
#   define INCLUDE_HTTP_REQUEST_UNSAFE
#   define INCLUDE_HTTP_REQUEST_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    http_request_animating;
//  HTTP methods identifiers, non-zero
#define HTTP_METHOD_GET                 1
#define HTTP_METHOD_HEAD                2
#define HTTP_METHOD_POST                3
#define HTTP_METHOD_PUT                 4
#define HTTP_METHOD_DELETE              5
#define HTTP_METHOD_MOVE                6
#define HTTP_METHOD_COPY                7

//  HTTP version identifiers, non-zero
#define HTTP_VERSION_1_0                1
#define HTTP_VERSION_1_1                2

//  HTTP reply codes

#define HTTP_REPLY_OK                   200
#define HTTP_REPLY_CREATED              201
#define HTTP_REPLY_ACCEPTED             202
#define HTTP_REPLY_NOCONTENT            204
#define HTTP_REPLY_PARTIAL              206
#define HTTP_REPLY_MOVED                301
#define HTTP_REPLY_FOUND                302
#define HTTP_REPLY_METHOD               303
#define HTTP_REPLY_NOTMODIFIED          304
#define HTTP_REPLY_BADREQUEST           400
#define HTTP_REPLY_UNAUTHORIZED         401
#define HTTP_REPLY_PAYMENT              402
#define HTTP_REPLY_FORBIDDEN            403
#define HTTP_REPLY_NOTFOUND             404
#define HTTP_REPLY_PRECONDITION         412
#define HTTP_REPLY_TOOLARGE             413
#define HTTP_REPLY_INTERNALERROR        500
#define HTTP_REPLY_NOTIMPLEMENTED       501
#define HTTP_REPLY_OVERLOADED           503
#define HTTP_REPLY_VERSIONUNSUP         505

//  Structure of the http_request object

struct _http_request_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [HTTP_REQUEST_HISTORY_LENGTH];
    int
        history_line  [HTTP_REQUEST_HISTORY_LENGTH];
    char
        *history_type [HTTP_REQUEST_HISTORY_LENGTH];
    int
        history_links [HTTP_REQUEST_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
http_server_t
    *server;                        //  Parent server object
smt_socket_t
    *socket;                        //  Incoming client socket
ipr_bucket_t
    *header,                        //  Bucket holding header
    *content;                       //  Bucket holding content if any
char
    *address,                       //  IP address of client
    *request_line,                  //  Full request line
    *request_method,                //  Method text from request line
    *request_uriargs,               //  Full URI from request line
    *request_path,                  //  URI path from request line
    *request_arguments,             //  URI arguments from request line
    *request_fragment,              //  URI fragment from request line
    *request_version;               //  HTTP version from request line
char
    *path,                          //  Parsed URI path and name
    *pathroot,                      //  Part of path mapped by plugin
    *pathinfo;                      //  Part of path following pathroot
uint
    method,                         //  Method identifier
    version;                        //  Version identifier
size_t
    header_length,                  //  Size of header including eoln
    content_length,                 //  Parsed Content-Length any
    content_read;                   //  Amount read so far
char
    *content_type,                  //  Content-Type header, if any
    *content_file;                  //  Content pipe name, if any
FILE
    *content_pipe;                  //  Content pipe file, if any
ipr_dict_table_t
    *headers,                       //  HTTP request header fields
    *args;                          //  Arguments, parsed as table
Bool
    encrypted,                      //  HTTPS protocol?
    persistent;                     //  Do we keep the connection alive?
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define http_request_new(server,socket,bucket)  http_request_new_ (__FILE__, __LINE__, server, socket, bucket)
http_request_t *
    http_request_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server,                 //  Parent server
smt_socket_t * socket,                  //  Client connection
ipr_bucket_t * bucket                   //  Bucket holding header
);

int
    http_request_set_address (
http_request_t * self,                  //  Reference to object
char * address                          //  Not documented
);

int
    http_request_parse (
http_request_t * self                   //  Reference to object
);

int
    http_request_content_parse (
http_request_t * self                   //  Reference to object
);

int
    http_request_content_pending (
http_request_t * self                   //  Reference to object
);

int
    http_request_content_append (
http_request_t * self,                  //  Reference to object
ipr_bucket_t * bucket                   //  Bucket holding content
);

char *
    http_request_get_header (
http_request_t * self,                  //  Reference to request object
char * header_name                      //  Name of header, case insensitive
);

void
    http_request_selftest (
void);

void
    http_request_terminate (
void);

#define http_request_show(item,opcode,trace_file)  http_request_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_request_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_request_destroy(self)      http_request_destroy_ (self, __FILE__, __LINE__)
void
    http_request_destroy_ (
http_request_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define http_request_link(self)         http_request_link_ (self, __FILE__, __LINE__)
http_request_t *
    http_request_link_ (
http_request_t * self,                  //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_request_unlink(self)       http_request_unlink_ (self, __FILE__, __LINE__)
void
    http_request_unlink_ (
http_request_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_request_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_request_new_in_scope(self_p,_scope,server,socket,bucket)  http_request_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, server, socket, bucket)
void
    http_request_new_in_scope_ (
http_request_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
http_server_t * server,                 //  Parent server
smt_socket_t * socket,                  //  Client connection
ipr_bucket_t * bucket                   //  Bucket holding header
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_HTTP_REQUEST_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_REQUEST_INLINE)
#   define INCLUDE_HTTP_REQUEST_INLINE
#   define INCLUDE_HTTP_REQUEST_ACTIVE

#   include "http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_REQUEST_ACTIVE
#   if defined (ICL_IMPORT_HTTP_REQUEST_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_REQUEST_ROOT
#   endif
# endif
#endif
