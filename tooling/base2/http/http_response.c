/*---------------------------------------------------------------------------
    http_response.c - http_response component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "http_response.h"              //  Definitions for our class

//  Shorthand for class type

#define self_t              http_response_t

//  Shorthands for class methods

#define self_new            http_response_new
#define self_annihilate     http_response_annihilate
#define self_set_header     http_response_set_header
#define self_set_from_file  http_response_set_from_file
#define self_set_from_bucket  http_response_set_from_bucket
#define self_set_from_xml_str  http_response_set_from_xml_str
#define self_set_no_cache   http_response_set_no_cache
#define self_set_content_type  http_response_set_content_type
#define self_set_redirect   http_response_set_redirect
#define self_set_error      http_response_set_error
#define self_format_reply   http_response_format_reply
#define self_selftest       http_response_selftest
#define self_terminate      http_response_terminate
#define self_show           http_response_show
#define self_destroy        http_response_destroy
#define self_alloc          http_response_alloc
#define self_free           http_response_free
#define self_link           http_response_link
#define self_unlink         http_response_unlink
#define self_cache_initialise  http_response_cache_initialise
#define self_cache_purge    http_response_cache_purge
#define self_cache_terminate  http_response_cache_terminate
#define self_show_animation  http_response_show_animation
#define self_new_in_scope   http_response_new_in_scope

#define http_response_annihilate(self)  http_response_annihilate_ (self, __FILE__, __LINE__)
static void
    http_response_annihilate_ (
http_response_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_response_alloc()           http_response_alloc_ (__FILE__, __LINE__)
static http_response_t *
    http_response_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_response_free(self)        http_response_free_ (self, __FILE__, __LINE__)
static void
    http_response_free_ (
http_response_t * self,                 //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    http_response_cache_initialise (
void);

static void
    http_response_cache_purge (
void);

static void
    http_response_cache_terminate (
void);

Bool
    http_response_animating = TRUE;     //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void
    s_format_root_uri (http_response_t *self);
static void
    s_format_error_page (http_response_t *self);
/*  -------------------------------------------------------------------------
    http_response_new

    Type: Component method
    Creates and initialises a new http_response_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_response_t *
    http_response_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server,             //  Parent server
    http_request_t * request            //  Request context
)
{
    http_response_t *
        self = NULL;                    //  Object reference

    self = http_response_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_RESPONSE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_response_show_);
#endif

//
self->server = http_server_link (server);
self->request = http_request_link (request);
self->reply_code = http_request_parse (self->request);
self->headers = ipr_dict_table_new ();
self->bucket_list = ipr_bucket_list_new ();
s_format_root_uri (self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_response_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_response_annihilate_ (
    http_response_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    int
        history_last;
#endif

    http_response_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = self->links;
#endif

    HTTP_RESPONSE_ASSERT_SANE (self);

http_server_unlink (&self->server);
http_request_unlink (&self->request);
ipr_bucket_list_destroy (&self->bucket_list);
ipr_dict_table_destroy (&self->headers);
icl_mem_strfree (&self->filename);

}
/*  -------------------------------------------------------------------------
    http_response_set_header

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the value of a specified header in the HTTP response. The header value
    is a printf format, followed by insertion arguments.
    -------------------------------------------------------------------------
 */

int
    http_response_set_header (
    http_response_t * self,             //  Reference to object
    char * header_name,                 //  Name of header, case insensitive
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    int
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = http_response_set_header_v (self, header_name, format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    http_response_set_header_v

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the value of a specified header in the HTTP response. The header value
    is a printf format, followed by insertion arguments.
    -------------------------------------------------------------------------
 */

int
    http_response_set_header_v (
    http_response_t * self,             //  Reference to object
    char * header_name,                 //  Name of header, case insensitive
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
char
    *string;                        //  Holds formatted value
int
    str_size = 1024,
    fmt_size;
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Format a string with the value
string = icl_mem_alloc (str_size + 1);
fmt_size = apr_vsnprintf (string, str_size, format, args);
if (fmt_size >= str_size) {
    //  If formatted value overflowed string, resize and repeat
    icl_mem_free (string);
    str_size = fmt_size;
    string = icl_mem_alloc (str_size + 1);
    fmt_size = apr_vsnprintf (string, str_size, format, args);
}
ipr_dict_assume (self->headers, header_name, string);
icl_mem_free (string);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_from_file

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response to send the file headers back to the browser.  Sets the
    content length, modified date, ETag and content type from the file info.
    For GET methods, prepares to return the file content.
    -------------------------------------------------------------------------
 */

int
    http_response_set_from_file (
    http_response_t * self,             //  Reference to object
    char * filename                     //  File to send
)
{
ipr_finfo_t
    *finfo;
icl_shortstr_t
    mime_date;
ipr_dict_t
    *content_type;
char
    *etag = NULL,                   //  Calculated ETag, if any
    *extension;                     //  File extension, if any
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Set response to return file content iff method is GET
icl_mem_strfree (&self->filename);
if (self->request->method == HTTP_METHOD_GET)
    self->filename = icl_mem_strdup (filename);

//  Calculate Content-Length: and Last-Modified: headers
finfo = ipr_file_get_info (filename);
self->content_length = (size_t) finfo->size;
ipr_time_mime (finfo->mtime, mime_date);
self_set_header (self, "last-modified", mime_date);

//  Calculate ETag: header
etag = ipr_file_etag (filename, http_config_etag (http_config));
self_set_header (self, "etag", etag);
icl_mem_free (etag);

//  Calculate Content type from file extension
extension = strchr (filename, '.');
if (!extension)
    extension = "default";
content_type = ipr_dict_table_search (self->server->content_types, extension);
if (content_type)
    icl_shortstr_cpy (self->content_type, content_type->value);
else
    icl_shortstr_cpy (self->content_type,  "*/*");

ipr_finfo_destroy (&finfo);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_from_bucket

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response to send a bucket back to the browser. Updates the content
    length from the bucket and the content type if not null. If you send back
    multiple buckets, the last content type you specify will be used.
    -------------------------------------------------------------------------
 */

int
    http_response_set_from_bucket (
    http_response_t * self,             //  Reference to object
    ipr_bucket_t * bucket,              //  Bucket to return
    char * content_type                 //  Content type
)
{
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
ipr_bucket_list_queue (self->bucket_list, bucket);
self->bucket_list->cur_size += bucket->cur_size;
self->content_length = (size_t) self->bucket_list->cur_size;
if (content_type)
    icl_shortstr_cpy (self->content_type, content_type);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_from_xml_str

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response to send an XML string back to the browser.  To make it
    easier to code, the string uses [ and ] instead of lt and gt.  Sets the
    content length and content type to text/xml.  Automatically prefixes the
    XML string with an ?xml tag.
    -------------------------------------------------------------------------
 */

int
    http_response_set_from_xml_str (
    http_response_t * self,             //  Reference to object
    char * xml_string                   //  XML to return
)
{
ipr_bucket_t
    *bucket;                        //  Serialized XML
char
    *xml_tag = "[?xml version=\"1.0\"?]";
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
bucket = ipr_bucket_new (strlen (xml_tag) + strlen (xml_string) + 1);
ipr_bucket_cat (bucket, xml_tag, strlen (xml_tag));
ipr_bucket_cat (bucket, xml_string, strlen (xml_string) + 1);
ipr_str_subch ((char *) bucket->data, '[', '<');
ipr_str_subch ((char *) bucket->data, ']', '>');
http_response_set_from_bucket (self, bucket, "text/xml");
ipr_bucket_unlink (&bucket);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_no_cache

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response so that the browser does not cache the URI.
    -------------------------------------------------------------------------
 */

int
    http_response_set_no_cache (
    http_response_t * self              //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
self_set_header (self, "cache-control", "no-cache");
self_set_header (self, "expires", "0");
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_content_type

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response content type.
    -------------------------------------------------------------------------
 */

int
    http_response_set_content_type (
    http_response_t * self,             //  Reference to object
    char * content_type                 //  Content type
)
{
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
icl_shortstr_cpy (self->content_type, content_type);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_redirect

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Redirects the response to the specified URI.  The URI can be a relative
    URI in which case the browser is redirected to the same server and port,
    or an absolute URI on the same or other server.
    -------------------------------------------------------------------------
 */

int
    http_response_set_redirect (
    http_response_t * self,             //  Reference to object
    char * new_uri                      //  New URI location
)
{
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Set Location: header into response headers table
//  If we have an absolute URI with proto name, use as-is
if (ipr_regexp_eq ("^[a-zA-Z0-9`.`-`+]+:", new_uri))
    self_set_header (self, "location", new_uri);
else
    self_set_header (self, "location", "%s%s", self->root_uri, new_uri);

self_set_error (self, HTTP_REPLY_FOUND, NULL);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_set_error

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response reply code.
    -------------------------------------------------------------------------
 */

int
    http_response_set_error (
    http_response_t * self,             //  Reference to object
    uint reply_code,                    //  Reply code
    char * reply_text,                  //  Reply text
...                                     //  Variable arguments
)
{
    int
        _rc;
    va_list
        args;
    va_start (args, reply_text);
    _rc = http_response_set_error_v (self, reply_code, reply_text, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    http_response_set_error_v

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the response reply code.
    -------------------------------------------------------------------------
 */

int
    http_response_set_error_v (
    http_response_t * self,             //  Reference to object
    uint reply_code,                    //  Reply code
    char * reply_text,                  //  Reply text
va_list args                            //  Variable arguments
)
{
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
self->reply_code = reply_code;
if (reply_text)
    apr_vsnprintf (self->reply_text, ICL_SHORTSTR_MAX, reply_text, args);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_format_reply

    Type: Component method
    Accepts a http_response_t reference and returns zero in case of success,
    1 in case of errors.
    Formats a response header and (in case of errors) body and places
    these at the start of the response bucket list, before any buckets
    the list might contain.
    -------------------------------------------------------------------------
 */

int
    http_response_format_reply (
    http_response_t * self              //  Reference to object
)
{
icl_shortstr_t
    mime_date;                      //  Holds formatted date/time
ipr_bucket_t
    *bucket;                        //  Holds formatted header
icl_longstr_t
    *prefix;                        //  Holds formatted first line
    int
        rc = 0;                         //  Return code

HTTP_RESPONSE_ASSERT_SANE (self);
if (!self->zombie) {

//
//  If we have an error, switch off keep-alive and format an error
//  message as a bucket of dynamic response data.
if (self->reply_code >= 400) {
    self->request->persistent = FALSE;
    s_format_error_page (self);
}
//  Define standard header fields
ipr_time_mime (apr_time_now (), mime_date);
self_set_header (self, "server", http_config_server_name (http_config));
self_set_header (self, "date", mime_date);
if (strused (self->content_type))
    self_set_header (self, "content-type", self->content_type);
self_set_header (self, "content-length", "%u", self->content_length);

//  Set connection header
if (self->request->persistent) {
    if (self->request->version == HTTP_VERSION_1_0)
        self_set_header (self, "connection", "Keep-Alive");
}
else
    if (self->request->version == HTTP_VERSION_1_1)
        self_set_header (self, "connection", "Close");

//  Format HTTP header block from header fields
prefix = icl_longstr_new (NULL, 1024);
icl_longstr_fmt (prefix, "HTTP/%s %d %s",
    self->request->version == HTTP_VERSION_1_0? "1.0": "1.1",
    self->reply_code, self->reply_text);

if (http_config_verbose (http_config)) {
    icl_console_print ("I: HTTP response ----------------------------------------");
    icl_console_print ("I: %s", (char *) prefix->data);
}
icl_longstr_cat (prefix, "\r\n");
bucket = ipr_dict_table_headers_save (
    self->headers, prefix, http_config_verbose (http_config));
ipr_bucket_list_push (self->bucket_list, bucket);
ipr_bucket_unlink (&bucket);
icl_longstr_destroy (&prefix);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_response_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_response_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_response_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_response_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_response_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_response_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_response_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_response zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    if (self->history_last > HTTP_RESPONSE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_RESPONSE_HISTORY_LENGTH;
        self->history_last %= HTTP_RESPONSE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_RESPONSE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_response>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_response_destroy

    Type: Component method
    Destroys a http_response_t object. Takes the address of a
    http_response_t reference (a pointer to a pointer) and nullifies the
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
    http_response_destroy_ (
    http_response_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_response_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        http_response_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("http_response", "E: missing link on http_response object");
        http_response_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_response_free_ ((http_response_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_response_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_response_t *
    http_response_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_response_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_response_cache_initialise ();

self = (http_response_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_response_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_response_free

    Type: Component method
    Freess a http_response_t object.
    -------------------------------------------------------------------------
 */

static void
    http_response_free_ (
    http_response_t * self,             //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (http_response_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_response_t));
        self->object_tag = HTTP_RESPONSE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_response_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_response_t *
    http_response_link_ (
    http_response_t * self,             //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    int
        history_last;
#endif

    if (self) {
        HTTP_RESPONSE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_response_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_response_unlink_ (
    http_response_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
    int
        history_last;
#endif

    http_response_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        HTTP_RESPONSE_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("http_response", "E: missing link on http_response object");
            http_response_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_RESPONSE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_RESPONSE_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            http_response_annihilate_ (self_p, file, line);
        http_response_free_ ((http_response_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_response_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_response_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_response_t));
icl_system_register (http_response_cache_purge, http_response_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_response_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_response_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_response_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_response_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_response_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_response_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_response_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_response_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_response_new_in_scope_ (
    http_response_t * * self_p,         //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server,             //  Parent server
    http_request_t * request            //  Request context
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_response_new_ (file,line,server,request);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_response_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Formats the external URI to the root path /.  This URI is used
//  in any response that needs to refer the client application back
//  to the same service.  Also formats the response->hostname.
//
static void
    s_format_root_uri (http_response_t *self)
{
    char
        *public_port;                   //  Current client http port

    //  Format proto://
    icl_shortstr_cpy (self->root_uri, self->request->encrypted? "https://": "http://");

    //  Format proto://hostname
    if (*http_request_get_header (self->request, "host"))
        icl_shortstr_cpy (self->hostname, http_request_get_header (self->request, "host"));
    else
        ipr_net_get_hostname (self->hostname);
    icl_shortstr_cat (self->root_uri, self->hostname);

    //  Format proto://hostname:port if necessary
    if (strchr (self->hostname, ':') == NULL) {
        public_port = http_config_public_port (http_config);
        if (*public_port && strneq (public_port, "80")) {
            icl_shortstr_cat (self->root_uri, ":");
            icl_shortstr_cat (self->root_uri, public_port);
        }
    }
    icl_shortstr_cat (self->root_uri, "/");
}

static void
s_format_error_page (http_response_t *self)
{
    ipr_bucket_t
        *bucket;                        //  Bucket we will format
    char
        *text = "Unknown error";
    icl_shortstr_t
        error_filename;
    FILE
        *error_file;                    //  Handle to error_nnn.html file

    switch (self->reply_code) {
        case HTTP_REPLY_BADREQUEST:
            text = "Badly formed HTTP request: ";
            break;
        case HTTP_REPLY_UNAUTHORIZED:
            text = "Not authorized: ";
            break;
        case HTTP_REPLY_PAYMENT:
            text = "Payment is required: ";
            break;
        case HTTP_REPLY_FORBIDDEN:
            text = "Not authorised: ";
            break;
        case HTTP_REPLY_NOTFOUND:
            text = "Resource not found: ";
            break;
        case HTTP_REPLY_PRECONDITION:
            text = "Precondition failed: ";
            break;
        case HTTP_REPLY_TOOLARGE:
            text = "Request is too large: ";
            break;
        case HTTP_REPLY_INTERNALERROR:
            text = "Internal error: ";
            break;
        case HTTP_REPLY_NOTIMPLEMENTED:
            text = "Not implemented: ";
            break;
        case HTTP_REPLY_OVERLOADED:
            text = "Server overloaded, try again later: ";
            break;
        case HTTP_REPLY_VERSIONUNSUP:
            text = "Unsupported browser version: ";
            break;
    }
    //  Allocated working buffer, fairly arbitrary sizes
    bucket = ipr_bucket_new (65000);

    //  Load contents of error_nnn.html file, if present
    icl_shortstr_fmt (error_filename, "error_%d.html", self->reply_code);
    error_file = fopen (error_filename, "r");
    if (error_file) {
        ipr_bucket_load (bucket, error_file);
        bucket->data [bucket->cur_size++] = 0;
        fclose (error_file);
        http_response_set_from_bucket (self, bucket, "text/html");
    }
    else {
        ipr_bucket_cat (bucket, text, strlen (text));
        if (strused (self->reply_text))
            ipr_bucket_cat (bucket, self->reply_text, strlen (self->reply_text));
        else
            ipr_bucket_cat (bucket, "failed", strlen ("failed"));
        http_response_set_from_bucket (self, bucket, "text/plain");
    }
    //  Add bucket to list of buckets returned to user
    http_response_set_no_cache (self);
    ipr_bucket_unlink (&bucket);
}

//  Embed the version information in the resulting binary

char *EMBED__http_response_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__http_response_component      = "http_response ";
char *EMBED__http_response_version        = "1.0 ";
char *EMBED__http_response_copyright      = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_response_filename       = "http_response.icl ";
char *EMBED__http_response_builddate      = "2011/03/01 ";
char *EMBED__http_response_version_end    = "VeRsIoNeNd:ipc";

