/*---------------------------------------------------------------------------
    http_request.c - http_request component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "http_request.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              http_request_t

//  Shorthands for class methods

#define self_new            http_request_new
#define self_annihilate     http_request_annihilate
#define self_set_address    http_request_set_address
#define self_parse          http_request_parse
#define self_content_parse  http_request_content_parse
#define self_content_pending  http_request_content_pending
#define self_content_append  http_request_content_append
#define self_get_header     http_request_get_header
#define self_selftest       http_request_selftest
#define self_terminate      http_request_terminate
#define self_show           http_request_show
#define self_destroy        http_request_destroy
#define self_alloc          http_request_alloc
#define self_free           http_request_free
#define self_link           http_request_link
#define self_unlink         http_request_unlink
#define self_cache_initialise  http_request_cache_initialise
#define self_cache_purge    http_request_cache_purge
#define self_cache_terminate  http_request_cache_terminate
#define self_show_animation  http_request_show_animation
#define self_new_in_scope   http_request_new_in_scope

#define http_request_annihilate(self)   http_request_annihilate_ (self, __FILE__, __LINE__)
static void
    http_request_annihilate_ (
http_request_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_request_alloc()            http_request_alloc_ (__FILE__, __LINE__)
static http_request_t *
    http_request_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_request_free(self)         http_request_free_ (self, __FILE__, __LINE__)
static void
    http_request_free_ (
http_request_t * self,                  //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    http_request_cache_initialise (
void);

static void
    http_request_cache_purge (
void);

static void
    http_request_cache_terminate (
void);

Bool
    http_request_animating = TRUE;      //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static int
    s_parse_request (http_request_t *self, char *data);
static int
    s_parse_headers (http_request_t *self, char *data);
//  These are the methods we allow
static struct {
    char *name;
    uint ident;
} s_valid_methods [] = {
    { "GET",    HTTP_METHOD_GET    },
    { "HEAD",   HTTP_METHOD_HEAD   },
    { "POST",   HTTP_METHOD_POST   },
    { "PUT",    HTTP_METHOD_PUT    },
    { "DELETE", HTTP_METHOD_DELETE },
    { "MOVE",   HTTP_METHOD_MOVE   },
    { "COPY",   HTTP_METHOD_COPY   },
    {  NULL,    0                  }    //  End of table
};

//  These are the versions we allow
static struct {
    char *name;
    uint ident;
} s_valid_versions [] = {
    { "1.0",    HTTP_VERSION_1_0   },
    { "1.1",    HTTP_VERSION_1_1   },
    {  NULL,    0                  }    //  End of table
};
/*  -------------------------------------------------------------------------
    http_request_new

    Type: Component method
    Creates and initialises a new http_request_t object, returns a
    reference to the created object.
    Creates a new HTTP request object.  The caller must provide the
    current socket, and a bucket holding the HTTP request header.
    -------------------------------------------------------------------------
 */

http_request_t *
    http_request_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server,             //  Parent server
    smt_socket_t * socket,              //  Client connection
    ipr_bucket_t * bucket               //  Bucket holding header
)
{
    http_request_t *
        self = NULL;                    //  Object reference

    self = http_request_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_REQUEST_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_request_show_);
#endif

//
self->server  = http_server_link (server);
self->socket  = socket;
self->header  = ipr_bucket_link (bucket);
self->headers = ipr_dict_table_new ();
self->args    = ipr_dict_table_new ();
self->version = HTTP_VERSION_1_0;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_request_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_request_annihilate_ (
    http_request_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    int
        history_last;
#endif

    http_request_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_REQUEST_HISTORY_LENGTH] = self->links;
#endif

    HTTP_REQUEST_ASSERT_SANE (self);

//  Unlink shared objects
ipr_bucket_unlink (&self->header);
http_server_unlink (&self->server);

//  External resources
if (self->content_file)
    ipr_file_delete (self->content_file);

//  Free strings
icl_mem_free (self->address);
icl_mem_free (self->request_line);
icl_mem_free (self->request_method);
icl_mem_free (self->request_uriargs);
icl_mem_free (self->request_path);
icl_mem_free (self->request_arguments);
icl_mem_free (self->request_fragment);
icl_mem_free (self->request_version);
icl_mem_free (self->path);
icl_mem_free (self->pathroot);
icl_mem_free (self->pathinfo);
icl_mem_free (self->content_file);

//  Destroy own objects
ipr_bucket_destroy (&self->content);
ipr_dict_table_destroy (&self->headers);
ipr_dict_table_destroy (&self->args);

}
/*  -------------------------------------------------------------------------
    http_request_set_address

    Type: Component method
    Accepts a http_request_t reference and returns zero in case of success,
    1 in case of errors.
    Stores the client IP address in the request, for use by other classes.
    Accepts an IP address string in dotted numeric format, "127.0.0.1".
    -------------------------------------------------------------------------
 */

int
    http_request_set_address (
    http_request_t * self,              //  Reference to object
    char * address                      //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_REQUEST_ASSERT_SANE (self);
if (!self->zombie) {

//
icl_mem_strfree (&self->address);
self->address = icl_mem_strdup (address);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_request_parse

    Type: Component method
    Accepts a http_request_t reference and returns zero in case of success,
    1 in case of errors.
    Parse the HTTP request header and set the request context accordingly.
    Return zero if ok, non-zero if the request was invalid. After this call,
    the return code is HTTP_REPLY_OK if the request was valid, and an HTTP
    error reply code if not.
    -------------------------------------------------------------------------
 */

int
    http_request_parse (
    http_request_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_REQUEST_ASSERT_SANE (self);
if (!self->zombie) {

//
rc = s_parse_request (self, (char *) (self->header->data));
if (rc == HTTP_REPLY_OK)
    rc = s_parse_headers (self, (char *) (self->header->data));
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_request_content_parse

    Type: Component method
    Accepts a http_request_t reference and returns zero in case of success,
    1 in case of errors.
    Parse the content headers and content body, if any was already read.
    Sets the request->content_length to the total expected, and content_read
    to the actual read so far.  If content_length > content_read, the caller
    should read again and call the content_append() method to store the
    additional content.  If the content fits into the configured bucket max,
    holds as a single bucket in request->content, else writes as a file in
    request->content_file. Returns number of octets still expected from the
    client socket.
    -------------------------------------------------------------------------
 */

int
    http_request_content_parse (
    http_request_t * self               //  Reference to object
)
{
byte
    *content_start;
    int
        rc = 0;                         //  Return code

HTTP_REQUEST_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Store content length and type
self->content_length = atoi (self_get_header (self, "content-length"));
self->content_type = self_get_header (self, "content-type");

//  Calculate size of header block
content_start = ipr_bucket_find (self->header, 0, (byte *) "\r\n\r\n", 4);
assert (content_start);
content_start += 4;
self->header_length = content_start - self->header->data;

//  Calculate how much content we already have in the header bucket
self->content_read = self->header->cur_size - self->header_length;
if (self->content_length > (size_t) http_config_bucket_max (http_config)) {
    //  Generate random name for pipe file and write what we have
    self->content_file = icl_mem_alloc (ICL_SHORTSTR_MAX + 1);
    ipr_str_random (self->content_file, "POST_9x10.in");
    self->content_pipe = fopen (self->content_file, "w");
    assert (self->content_pipe);
    if (self->content_read)
        assert (fwrite (content_start, self->content_read, 1, self->content_pipe) == 1);
}
else
if (self->content_length > 0) {
    //  Allocate a bucket with room for a binary zero
    //  This makes it safer for imperfect code that parses the bucket
    self->content = ipr_bucket_new (self->content_length + 1);
    ipr_bucket_fill (self->content, content_start, self->content_read);
    self->content->data [self->content->cur_size] = 0;
}
rc = self_content_pending (self);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_request_content_pending

    Type: Component method
    Accepts a http_request_t reference and returns zero in case of success,
    1 in case of errors.
    Reports how many octets still need to be read from the post body in order
    to complete the content as specified by the content-length header.
    -------------------------------------------------------------------------
 */

int
    http_request_content_pending (
    http_request_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_REQUEST_ASSERT_SANE (self);
if (!self->zombie) {

//
rc = (int) (self->content_length - self->content_read);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_request_content_append

    Type: Component method
    Accepts a http_request_t reference and returns zero in case of success,
    1 in case of errors.
    Appends a bucket of data to the request content.
    -------------------------------------------------------------------------
 */

int
    http_request_content_append (
    http_request_t * self,              //  Reference to object
    ipr_bucket_t * bucket               //  Bucket holding content
)
{
    int
        rc = 0;                         //  Return code

HTTP_REQUEST_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (self->content || self->content_pipe);
if (self->content) {
    ipr_bucket_cat (self->content, bucket->data, bucket->cur_size);
    self->content->data [self->content->cur_size] = 0;
    self->content_read += bucket->cur_size;
}
else {
    assert (fwrite (bucket->data, bucket->cur_size, 1, self->content_pipe) == 1);
    self->content_read += bucket->cur_size;
    if (self->content_read == self->content_length)
        fclose (self->content_pipe);
}
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_request_get_header

    Type: Component method
    Gets the value of a specified header from the HTTP request.
    -------------------------------------------------------------------------
 */

char *
    http_request_get_header (
    http_request_t * self,              //  Reference to request object
    char * header_name                  //  Name of header, case insensitive
)
{
    char *
        header_value;                   //  Returned value, or ""

//
header_value = ipr_dict_table_headers_search (self->headers, header_name);

    return (header_value);
}
/*  -------------------------------------------------------------------------
    http_request_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_request_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_request_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_request_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_request_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_request_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_request_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_request zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    if (self->history_last > HTTP_REQUEST_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_REQUEST_HISTORY_LENGTH;
        self->history_last %= HTTP_REQUEST_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_REQUEST_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_request>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_request_destroy

    Type: Component method
    Destroys a http_request_t object. Takes the address of a
    http_request_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    -------------------------------------------------------------------------
 */

void
    http_request_destroy_ (
    http_request_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_request_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        http_request_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("http_request", "E: missing link on http_request object");
        http_request_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_request_free_ ((http_request_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_request_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_request_t *
    http_request_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_request_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_request_cache_initialise ();

self = (http_request_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_request_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_request_free

    Type: Component method
    Freess a http_request_t object.
    -------------------------------------------------------------------------
 */

static void
    http_request_free_ (
    http_request_t * self,              //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_REQUEST_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (http_request_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_request_t));
        self->object_tag = HTTP_REQUEST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_request_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_request_t *
    http_request_link_ (
    http_request_t * self,              //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    int
        history_last;
#endif

    if (self) {
        HTTP_REQUEST_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_REQUEST_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_request_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_request_unlink_ (
    http_request_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
    int
        history_last;
#endif

    http_request_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        HTTP_REQUEST_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("http_request", "E: missing link on http_request object");
            http_request_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_REQUEST)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_REQUEST_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_REQUEST_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            http_request_annihilate_ (self_p, file, line);
        http_request_free_ ((http_request_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_request_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_request_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_request_t));
icl_system_register (http_request_cache_purge, http_request_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_request_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_request_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_request_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_request_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_request_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_request_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_request_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_request_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_request_new_in_scope_ (
    http_request_t * * self_p,          //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server,             //  Parent server
    smt_socket_t * socket,              //  Client connection
    ipr_bucket_t * bucket               //  Bucket holding header
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_request_new_ (file,line,server,socket,bucket);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_request_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Parse the Request-Line according to RFC2616 section 5.1. format:
//      Method SP Request-URI SP HTTP-Version CRLF
//  Returns 0 if OK, -1 if there was an error, and sets self->result.
//
static int
s_parse_request (http_request_t *self, char *data)
{
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    uint
        index;                          //  Index into method table
    int
        rc = HTTP_REPLY_OK;             //  Return code

    regexp = ipr_regexp_new (
        //  We want to match these items from the request line:
        // $1 = request line
        // |$2 = method
        // ||     $3 = uriargs
        // ||     |$4 = pathname
        // ||     ||              $5 = arguments
        // ||     ||              |              $6 = fragment
        // ||     ||              |              |
        // ||     ||              |              |
        // ||     ||              |              |
        // ||     ||              |              |                $7 = version
        // ||     ||              |              |                |
         "^((`w+) (([^?#`s]*)(?:`?([^#`s]*))?(?:#(`s*))?)(?: HTTP/([0-9]`.[0-9]))?)\r\n");
    if (ipr_regexp_match (
        regexp,
        data,
        &self->request_line,
        &self->request_method,
        &self->request_uriargs,
        &self->request_path,
        &self->request_arguments,
        &self->request_fragment,
        &self->request_version) == 7)
    {
        char
            *path;                      //  Working copy of path

        ipr_str_upper (self->request_method);
        if (http_config_verbose (http_config)) {
            icl_console_print ("I: HTTP request -----------------------------------------");
            icl_console_print ("I: %s %s HTTP/%s",
                self->request_method, self->request_uriargs, self->request_version);
        }
        //  Clean-up path to remove things like '..'
        path = icl_mem_strdup (self->request_path);
        ipr_http_unescape_uri (path);
        ipr_str_subch (path, '\\', '/');
        self->path = ipr_path_resolve (path);
        icl_mem_free (path);            //  Finished with working copy

        //  Load arguments string into request->args
        ipr_dict_table_uri_load (self->args, self->request_arguments);

        //  Check that requested method is valid
        for (index = 0; s_valid_methods [index].name; index++) {
            if (streq (self->request_method, s_valid_methods [index].name)) {
                self->method = s_valid_methods [index].ident;
                break;
            }
        }
        //  Check that requested version is valid
        for (index = 0; s_valid_versions [index].name; index++) {
            if (streq (self->request_version, s_valid_versions [index].name)) {
                self->version = s_valid_versions [index].ident;
                break;
            }
        }
        if (!self->method)
            rc = HTTP_REPLY_BADREQUEST;
        else
        if (!self->version)
            rc = HTTP_REPLY_VERSIONUNSUP;
    }
    else {
        if (http_config_verbose (http_config)) {
            icl_console_print ("I: 400 - BADREQUEST: URI does not parse");
            icl_console_print ("I: %s", data);
        }
        rc = HTTP_REPLY_BADREQUEST;
    }
    ipr_regexp_destroy (&regexp);
    return (rc);
}

//  Parse HTTP headers and collect interesting header values into
//  request context.
//  Returns 0 if OK, -1 if there was an error, and sets self->result.
//
static int
s_parse_headers (http_request_t *self, char *data)
{
    int
        rc = HTTP_REPLY_OK;             //  Return code
    char
        *host;                          //  Host: header value

    //  Create headers table and insert default value
    ipr_dict_assume (self->headers, "content-length", "0");
    ipr_dict_table_headers_load (self->headers, strchr (data, '\n') + 1,
        http_config_verbose (http_config));

    //  Load and parse special headers
    //  --------------------------------------------------------------------

    //  RFC 2616 section 14.23 requires Host: header for HTTP/1.1 requests
    host = self_get_header (self, "host");
    if (*host) {
        //  Don't allow ., .., or / characters
        if (streq (host, ".") || streq (host, "..") || strchr (host, '/'))
            rc = HTTP_REPLY_BADREQUEST;
    }
    else
    if (self->version == HTTP_VERSION_1_1) {
        if (http_config_verbose (http_config))
            icl_console_print ("I: 400 - BADREQUEST: Missing Host: header");
        rc = HTTP_REPLY_BADREQUEST;
    }
    //  Set the persistent connection flag
    if (self->version == HTTP_VERSION_1_1)
        //  Use "connection: close" to determine persistent flag
        self->persistent = !ipr_dict_table_matches (self->headers, "connection", "(?i)^close");
    else
        //  Use "connection: keep-alive" to determine persistent flag
        self->persistent = ipr_dict_table_matches (self->headers, "connection", "(?i)^keep-alive");

    return (rc);
}

//  Embed the version information in the resulting binary

char *EMBED__http_request_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__http_request_component       = "http_request ";
char *EMBED__http_request_version         = "1.0 ";
char *EMBED__http_request_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_request_filename        = "http_request.icl ";
char *EMBED__http_request_builddate       = "2011/03/01 ";
char *EMBED__http_request_version_end     = "VeRsIoNeNd:ipc";

