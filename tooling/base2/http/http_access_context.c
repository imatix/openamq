/*---------------------------------------------------------------------------
    http_access_context.c - http_access_context component

    This class holds properties for access control.  These properties are
    provided to and modified by the various access modules.  The object is
    for local use by access modules.
    Generated from http_access_context.icl by icl_gen using GSL/4.
    
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
#include "http_access_context.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              http_access_context_t

//  Shorthands for class methods

#define self_new            http_access_context_new
#define self_annihilate     http_access_context_annihilate
#define self_free           http_access_context_free
#define self_set_thread     http_access_context_set_thread
#define self_set_address    http_access_context_set_address
#define self_set_request    http_access_context_set_request
#define self_set_response   http_access_context_set_response
#define self_set_auth       http_access_context_set_auth
#define self_set_fail       http_access_context_set_fail
#define self_format         http_access_context_format
#define self_selftest       http_access_context_selftest
#define self_terminate      http_access_context_terminate
#define self_show           http_access_context_show
#define self_destroy        http_access_context_destroy
#define self_alloc          http_access_context_alloc
#define self_cache_initialise  http_access_context_cache_initialise
#define self_cache_purge    http_access_context_cache_purge
#define self_cache_terminate  http_access_context_cache_terminate
#define self_show_animation  http_access_context_show_animation
#define self_new_in_scope   http_access_context_new_in_scope

static void
    http_access_context_annihilate (
http_access_context_t * ( * self_p )    //  Reference to object reference
);

static void
    http_access_context_free (
http_access_context_t * self            //  Object reference
);

#define http_access_context_alloc()     http_access_context_alloc_ (__FILE__, __LINE__)
static http_access_context_t *
    http_access_context_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_access_context_cache_initialise (
void);

static void
    http_access_context_cache_purge (
void);

static void
    http_access_context_cache_terminate (
void);

Bool
    http_access_context_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    http_access_context_new

    Type: Component method
    Creates and initialises a new http_access_context_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_access_context_t *
    http_access_context_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server              //  Not documented
)
{
    http_access_context_t *
        self = NULL;                    //  Object reference

    self = http_access_context_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_ACCESS_CONTEXT_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, http_access_context_show_);
#endif

//
self->basic  = ipr_password_table_link (server->basic);
self->digest = ipr_password_table_link (server->digest);
self->access_log = smt_log_link (server->access_log);
self->alert_log  = smt_log_link (server->alert_log);
self->debug_log  = smt_log_link (server->debug_log);
self->blacklist  = ipr_dict_table_link (server->blacklist);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_access_context_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_context_annihilate (
    http_access_context_t * ( * self_p )  //  Reference to object reference
)
{

    http_access_context_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

smt_thread_unlink (&self->thread);

}
/*  -------------------------------------------------------------------------
    http_access_context_free

    Type: Component method
    Freess a http_access_context_t object.
    -------------------------------------------------------------------------
 */

static void
    http_access_context_free (
    http_access_context_t * self        //  Object reference
)
{


if (self) {

icl_mem_strfree (&self->address);
http_request_unlink  (&self->request);
http_response_unlink (&self->response);
ipr_password_table_unlink (&self->basic);
ipr_password_table_unlink (&self->digest);
smt_log_unlink  (&self->access_log);
smt_log_unlink  (&self->alert_log);
smt_log_unlink  (&self->debug_log);
icl_mem_strfree (&self->fail_user);
icl_mem_strfree (&self->fail_realm);
icl_mem_strfree (&self->auth_user);
icl_mem_strfree (&self->auth_group);
ipr_process_destroy (&self->process);
ipr_dict_table_unlink (&self->blacklist);
        memset (&self->object_tag, 0, sizeof (http_access_context_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_access_context_t));
        self->object_tag = HTTP_ACCESS_CONTEXT_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_access_context_set_thread

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_thread (
    http_access_context_t * self,       //  Reference to object
    smt_thread_t * thread               //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
smt_thread_unlink (&self->thread);
self->thread = smt_thread_link (thread);


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_set_address

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_address (
    http_access_context_t * self,       //  Reference to object
    char * address                      //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
icl_mem_strfree (&self->address);
self->address = icl_mem_strdup (address);


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_set_request

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_request (
    http_access_context_t * self,       //  Reference to object
    http_request_t * request            //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
http_request_unlink (&self->request);
self->request = http_request_link (request);


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_set_response

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_response (
    http_access_context_t * self,       //  Reference to object
    http_response_t * response          //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
http_response_unlink (&self->response);
self->response = http_response_link (response);


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_set_auth

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    Stores the user name and group after successful authentication.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_auth (
    http_access_context_t * self,       //  Reference to object
    ipr_password_table_t * table,       //  Not documented
    char * group                        //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
icl_mem_strfree (&self->fail_user);
icl_mem_strfree (&self->fail_realm);
icl_mem_strfree (&self->auth_user);
icl_mem_strfree (&self->auth_group);
self->auth_user  = icl_mem_strdup (table->username);
self->auth_group = icl_mem_strdup (group);
self->fail_code  = table->result;


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_set_fail

    Type: Component method
    Accepts a http_access_context_t reference and returns zero in case of success,
    1 in case of errors.
    Stores the user name which tried but failed to authenticated, and its
    realm.
    -------------------------------------------------------------------------
 */

int
    http_access_context_set_fail (
    http_access_context_t * self,       //  Reference to object
    ipr_password_table_t * table        //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_CONTEXT_ASSERT_SANE (self);

//
icl_mem_strfree (&self->fail_user);
icl_mem_strfree (&self->fail_realm);
icl_mem_strfree (&self->auth_user);
icl_mem_strfree (&self->auth_group);
self->fail_user  = icl_mem_strdup (table->username);
self->fail_realm = icl_mem_strdup (table->realm);
self->fail_code  = table->result;


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_context_format

    Type: Component method
    Formats a line of logging information, inserting logging symbols.  Returns
    freshly allocated line with formatted text. The template symbools are:
    * $agent - User-Agent header value
    * $arguments - requested URI arguments
    * $datetime - date/time in NCSA format
    * $day - day as two digits
    * $file - filename to which request was translated
    * $from - client address, as dotted number
    * $hh - hour, using 24-hour clock
    * $method - HTTP method
    * $mm - minutes as two digits
    * $mon - month as two digits
    * $path - requested URI path
    * $query - query string, if any
    * $recd - request size, in bytes
    * $referer - Referer header
    * $request - complete request line
    * $sent - response size, in bytes
    * $ss - seconds as two digits
    * $status - response code, 3 digits
    * $user - user name, if authenticated, else -
    * $year - year as four digits
    * $yy - year as two digits
    * $VAR - environment variable VAR
    -------------------------------------------------------------------------
 */

char *
    http_access_context_format (
    http_access_context_t * self,       //  Not documented
    char * format                       //  Not documented
)
{
static char
    *months = "Jan0Feb0Mar0Apr0May0Jun0Jul0Aug0Sep0Oct0Nov0Dec";
ipr_dict_table_t
    *table;
apr_time_t
    apr_time;
apr_time_exp_t
    time_exp;
icl_shortstr_t
    time_str;
    char *
        formatted = NULL;               //  Not documented

//
table = ipr_dict_table_new ();
//  Import environment
ipr_dict_table_import (table, environ, TRUE);

apr_time = apr_time_now ();
apr_time_exp_lt (&time_exp, apr_time);

//  Define all variables
ipr_dict_assume     (table, "agent",        http_request_get_header (self->request, "user-agent"));
ipr_dict_assume     (table, "arguments",    self->request->request_arguments);
ipr_dict_assume     (table, "datetime",     ipr_http_time_str (time_str));
ipr_dict_assume_fmt (table, "day", "%02d",  time_exp.tm_mday);
ipr_dict_assume     (table, "file",         self->response->filename? self->response->filename: "-");
ipr_dict_assume     (table, "from",         self->request->address);
ipr_dict_assume_fmt (table, "hh", "%02d",   time_exp.tm_hour);
ipr_dict_assume     (table, "method",       self->request->request_method);
ipr_dict_assume_fmt (table, "mm", "%02d",   time_exp.tm_mon + 1);
ipr_dict_assume     (table, "mon",          months + time_exp.tm_mon * 4);
ipr_dict_assume     (table, "path",         self->request->request_path);
ipr_dict_assume_fmt (table, "recd", "%ld",  self->request->content_length);
ipr_dict_assume     (table, "referer",      http_request_get_header (self->request, "referer"));
ipr_dict_assume     (table, "request",      self->request->request_line);
ipr_dict_assume_fmt (table, "sent", "%ld",  self->response->content_length);
ipr_dict_assume_fmt (table, "ss", "%02d",   time_exp.tm_sec);
ipr_dict_assume_fmt (table, "status", "%d", self->response->reply_code);
ipr_dict_assume     (table, "user",         self->auth_user? self->auth_user: "-");
ipr_dict_assume_fmt (table, "year", "%04d", time_exp.tm_year + 1900);
ipr_dict_assume_fmt (table, "yy", "%02d",   time_exp.tm_year);

formatted = ipr_dict_table_template (table, format);
ipr_dict_table_destroy (&table);

    return (formatted);
}
/*  -------------------------------------------------------------------------
    http_access_context_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_context_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_access_context_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_context_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_access_context_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_context_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
http_access_context_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <http_access_context file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    http_access_context_destroy

    Type: Component method
    Destroys a http_access_context_t object. Takes the address of a
    http_access_context_t reference (a pointer to a pointer) and nullifies the
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
    http_access_context_destroy_ (
    http_access_context_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_access_context_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    http_access_context_annihilate (self_p);
    http_access_context_free ((http_access_context_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_access_context_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_access_context_t *
    http_access_context_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_access_context_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_access_context_cache_initialise ();

self = (http_access_context_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_access_context_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_access_context_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_access_context_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_access_context_t));
icl_system_register (http_access_context_cache_purge, http_access_context_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_access_context_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_context_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_access_context_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_context_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_access_context_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_access_context_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_access_context_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_access_context_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_context_new_in_scope_ (
    http_access_context_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_server_t * server              //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_access_context_new_ (file,line,server);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_access_context_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_access_context_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__http_access_context_component  = "http_access_context ";
char *EMBED__http_access_context_version  = "1.0 ";
char *EMBED__http_access_context_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_access_context_filename  = "http_access_context.icl ";
char *EMBED__http_access_context_builddate  = "2011/03/01 ";
char *EMBED__http_access_context_version_end  = "VeRsIoNeNd:ipc";

