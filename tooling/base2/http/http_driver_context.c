/*---------------------------------------------------------------------------
    http_driver_context.c - http_driver_context component

    This class holds properties for driver control.  These properties are
    provided to and modified by the various driver modules.  The object is
    for local use by driver modules.
    Generated from http_driver_context.icl by icl_gen using GSL/4.
    
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
#include "http_driver_context.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              http_driver_context_t

//  Shorthands for class methods

#define self_new            http_driver_context_new
#define self_annihilate     http_driver_context_annihilate
#define self_free           http_driver_context_free
#define self_init           http_driver_context_init
#define self_reply_error    http_driver_context_reply_error
#define self_reply_success  http_driver_context_reply_success
#define self_reply_void     http_driver_context_reply_void
#define self_reply          http_driver_context_reply
#define self_abort          http_driver_context_abort
#define self_xml_parse      http_driver_context_xml_parse
#define self_selftest       http_driver_context_selftest
#define self_terminate      http_driver_context_terminate
#define self_show           http_driver_context_show
#define self_destroy        http_driver_context_destroy
#define self_alloc          http_driver_context_alloc
#define self_link           http_driver_context_link
#define self_unlink         http_driver_context_unlink
#define self_cache_initialise  http_driver_context_cache_initialise
#define self_cache_purge    http_driver_context_cache_purge
#define self_cache_terminate  http_driver_context_cache_terminate
#define self_show_animation  http_driver_context_show_animation
#define self_new_in_scope   http_driver_context_new_in_scope

#define http_driver_context_annihilate(self)  http_driver_context_annihilate_ (self, __FILE__, __LINE__)
static void
    http_driver_context_annihilate_ (
http_driver_context_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_driver_context_free(self)  http_driver_context_free_ (self, __FILE__, __LINE__)
static void
    http_driver_context_free_ (
http_driver_context_t * self,           //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_driver_context_alloc()     http_driver_context_alloc_ (__FILE__, __LINE__)
static http_driver_context_t *
    http_driver_context_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_driver_context_cache_initialise (
void);

static void
    http_driver_context_cache_purge (
void);

static void
    http_driver_context_cache_terminate (
void);

Bool
    http_driver_context_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    http_driver_context_new

    Type: Component method
    Creates and initialises a new http_driver_context_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_driver_context_t *
    http_driver_context_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    smt_thread_t * thread,              //  Not documented
    http_server_t * server,             //  Not documented
    http_request_t * request,           //  Not documented
    http_response_t * response          //  Not documented
)
{
    http_driver_context_t *
        self = NULL;                    //  Object reference

    self = http_driver_context_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_DRIVER_CONTEXT_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_driver_context_show_);
#endif

//
assert (request);
assert (response);
self->thread = smt_thread_link (thread);
self->request  = http_request_link (request);
self->response = http_response_link (response);
self->alert_log = smt_log_link (server->alert_log);
self->debug_log = smt_log_link (server->debug_log);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_context_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_context_annihilate_ (
    http_driver_context_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    int
        history_last;
#endif

    http_driver_context_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = self->links;
#endif

    HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);

smt_thread_unlink (&self->thread);

}
/*  -------------------------------------------------------------------------
    http_driver_context_free

    Type: Component method
    Freess a http_driver_context_t object.
    -------------------------------------------------------------------------
 */

static void
    http_driver_context_free_ (
    http_driver_context_t * self,       //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = self->links;
#endif

http_request_unlink  (&self->request);
http_response_unlink (&self->response);
smt_log_unlink (&self->alert_log);
smt_log_unlink (&self->debug_log);

ipr_xml_unlink (&self->xml_item);
ipr_xml_unlink (&self->xml_root);
ipr_xml_unlink (&self->xml_tree);
        memset (&self->object_tag, 0, sizeof (http_driver_context_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_driver_context_t));
        self->object_tag = HTTP_DRIVER_CONTEXT_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_driver_context_init

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_init (
    http_driver_context_t * self,       //  Reference to object
    http_request_t * request,           //  Not documented
    http_response_t * response          //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

//
self->replied = FALSE;
http_request_unlink  (&self->request);
http_response_unlink (&self->response);
self->request  = http_request_link (request);
self->response = http_response_link (response);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_reply_error

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sends a reply event back to the http_agent with an error code and text.
    Sets the context->failed property to TRUE.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_reply_error (
    http_driver_context_t * self,       //  Reference to object
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
    _rc = http_driver_context_reply_error_v (self, reply_code, reply_text, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    http_driver_context_reply_error_v

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sends a reply event back to the http_agent with an error code and text.
    Sets the context->failed property to TRUE.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_reply_error_v (
    http_driver_context_t * self,       //  Reference to object
    uint reply_code,                    //  Reply code
    char * reply_text,                  //  Reply text
va_list args                            //  Variable arguments
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (self->response);
self->response->reply_code = reply_code;
if (reply_text)
    apr_vsnprintf (self->response->reply_text, ICL_SHORTSTR_MAX, reply_text, args);
self->failed = TRUE;
self_reply (self);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_reply_success

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sends a reply event back to the http_agent with a 2xx or 3xx reply code.
    Note that we only format reply text bodies for 4xx and 5xx codes.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_reply_success (
    http_driver_context_t * self,       //  Reference to object
    uint reply_code                     //  Reply code
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (self->response);
self->response->reply_code = reply_code;
self_reply (self);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_reply_void

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the context->replied status but does not actually send a reply
    method back to the http thread, which will wait.  This lets us do a
    long poll, and reply at some later stage to the same context.  To
    use this, objects should grab a link to the context so it and the http
    thread will hang around.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_reply_void (
    http_driver_context_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (!self->replied);
self->replied = TRUE;
self->pending = TRUE;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_reply

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sends a reply event back to the http_agent.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_reply (
    http_driver_context_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

assert (!self->replied);
http_agent_driver_reply (self->thread);
self->replied = TRUE;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_abort

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Sends an abort event back to the http_agent.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_abort (
    http_driver_context_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

assert (!self->replied);
http_agent_driver_abort (self->thread);
self->replied = TRUE;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_xml_parse

    Type: Component method
    Accepts a http_driver_context_t reference and returns zero in case of success,
    1 in case of errors.
    Attempts to parse the request content as XML, returns 0 if this was
    successful, else -1.  Applications should check the content-type before
    doing this.  Sets the context->xml_tree, xml_root and xml_item.  If the
    content could not be parsed, signals an error reply to the http_agent.
    If the content was empty, creates a tree with an empty root and empty
    item, if specified.
    -------------------------------------------------------------------------
 */

int
    http_driver_context_xml_parse (
    http_driver_context_t * self,       //  Reference to object
    char * root,                        //  Default root, or null
    char * item                         //  Default item, or null
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
if (!self->zombie) {

//
ipr_xml_unlink (&self->xml_item);
ipr_xml_unlink (&self->xml_root);
ipr_xml_unlink (&self->xml_tree);

if (self->request->content_length) {
    if (self->request->content) {
        if (ipr_xml_load_bucket (&self->xml_tree, self->request->content, FALSE) == 0) {
            self->xml_root = ipr_xml_first_child (self->xml_tree);
            if (self->xml_root)
                self->xml_item = ipr_xml_first_child (self->xml_root);

            //  Check document root and first item
            if (root
            && (self->xml_root == NULL
            ||  strneq (ipr_xml_name (self->xml_root), root)))
                self_reply_error (self, HTTP_REPLY_BADREQUEST,
                    "document root must be '<%s>'", root);
            else
            if (item
            && (self->xml_item == NULL
            ||  strneq (ipr_xml_name (self->xml_item), item)))
                self_reply_error (self, HTTP_REPLY_BADREQUEST,
                    "first element must be '<%s>'", item);
        }
        else {
            self_reply_error (self, HTTP_REPLY_BADREQUEST,
                "XML content is not correctly formed");
            ipr_xml_destroy (&self->xml_tree);
        }
    }
    else
        //  If the content can't fit into a bucket, it's too large
        //  to fit into memory and would probably kill the server...
        self_reply_error (self, HTTP_REPLY_TOOLARGE,
            "document is too large (limit is %), rejected", IPR_BUCKET_MAX_SIZE);
}
else {
    //  If we don't have any content, create a default empty tree
    self->xml_tree = ipr_xml_new (NULL, "root", NULL);
    if (root) {
        self->xml_root = ipr_xml_new (self->xml_tree, root, NULL);
        if (item)
            self->xml_item = ipr_xml_new (self->xml_root, item, NULL);
    }
}
if (self->failed)
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_context_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_context_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_driver_context_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_context_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_driver_context_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_context_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_driver_context_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_driver_context zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    if (self->history_last > HTTP_DRIVER_CONTEXT_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH;
        self->history_last %= HTTP_DRIVER_CONTEXT_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_driver_context>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_driver_context_destroy

    Type: Component method
    Destroys a http_driver_context_t object. Takes the address of a
    http_driver_context_t reference (a pointer to a pointer) and nullifies the
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
    http_driver_context_destroy_ (
    http_driver_context_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_driver_context_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        http_driver_context_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("http_driver_context", "E: missing link on http_driver_context object");
        http_driver_context_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_driver_context_free_ ((http_driver_context_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_driver_context_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_driver_context_t *
    http_driver_context_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_driver_context_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_driver_context_cache_initialise ();

self = (http_driver_context_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_driver_context_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_context_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_driver_context_t *
    http_driver_context_link_ (
    http_driver_context_t * self,       //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    int
        history_last;
#endif

    if (self) {
        HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_context_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_driver_context_unlink_ (
    http_driver_context_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
    int
        history_last;
#endif

    http_driver_context_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        HTTP_DRIVER_CONTEXT_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("http_driver_context", "E: missing link on http_driver_context object");
            http_driver_context_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_CONTEXT)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_DRIVER_CONTEXT_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            http_driver_context_annihilate_ (self_p, file, line);
        http_driver_context_free_ ((http_driver_context_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_driver_context_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_driver_context_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_driver_context_t));
icl_system_register (http_driver_context_cache_purge, http_driver_context_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_driver_context_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_context_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_driver_context_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_context_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_driver_context_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_driver_context_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_driver_context_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_driver_context_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_context_new_in_scope_ (
    http_driver_context_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    smt_thread_t * thread,              //  Not documented
    http_server_t * server,             //  Not documented
    http_request_t * request,           //  Not documented
    http_response_t * response          //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_driver_context_new_ (file,line,thread,server,request,response);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_driver_context_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_driver_context_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__http_driver_context_component  = "http_driver_context ";
char *EMBED__http_driver_context_version  = "1.0 ";
char *EMBED__http_driver_context_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_driver_context_filename  = "http_driver_context.icl ";
char *EMBED__http_driver_context_builddate  = "2011/03/01 ";
char *EMBED__http_driver_context_version_end  = "VeRsIoNeNd:ipc";

