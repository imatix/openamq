/*---------------------------------------------------------------------------
    smt_fileio.c - smt_fileio component

    The smt_fileio class is used to create socket objects for use with the
    smt_fileio_request class.
    Generated from smt_fileio.icl by icl_gen using GSL/4.
    
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
#include "smt_fileio.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_fileio_t

//  Shorthands for class methods

#define self_new            smt_fileio_new
#define self_annihilate     smt_fileio_annihilate
#define self_open           smt_fileio_open
#define self_temp           smt_fileio_temp
#define self_eof            smt_fileio_eof
#define self_selftest       smt_fileio_selftest
#define self_terminate      smt_fileio_terminate
#define self_show           smt_fileio_show
#define self_destroy        smt_fileio_destroy
#define self_alloc          smt_fileio_alloc
#define self_free           smt_fileio_free
#define self_cache_initialise  smt_fileio_cache_initialise
#define self_cache_purge    smt_fileio_cache_purge
#define self_cache_terminate  smt_fileio_cache_terminate
#define self_show_animation  smt_fileio_show_animation

#define smt_fileio_new()                smt_fileio_new_ (__FILE__, __LINE__)
static smt_fileio_t *
    smt_fileio_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_fileio_annihilate (
smt_fileio_t * ( * self_p )             //  Reference to object reference
);

#define smt_fileio_alloc()              smt_fileio_alloc_ (__FILE__, __LINE__)
static smt_fileio_t *
    smt_fileio_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_fileio_free (
smt_fileio_t * self                     //  Object reference
);

static void
    smt_fileio_cache_initialise (
void);

static void
    smt_fileio_cache_purge (
void);

static void
    smt_fileio_cache_terminate (
void);

Bool
    smt_fileio_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_fileio_new

    Type: Component method
    Creates and initialises a new smt_fileio_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

static smt_fileio_t *
    smt_fileio_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_fileio_t *
        self = NULL;                    //  Object reference

    self = smt_fileio_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_FILEIO_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_fileio_show_);
#endif

self->handle  = NULL;
self->io_size = 0;
self->eof     = FALSE;
icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
apr_pool_tag (self->pool, "smt_fileio_new");
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_fileio_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_fileio_annihilate (
    smt_fileio_t * ( * self_p )         //  Reference to object reference
)
{

    smt_fileio_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

SMT_FILEIO_ASSERT_SANE (self);

apr_file_close (self->handle);
apr_pool_destroy (self->pool);

}
/*  -------------------------------------------------------------------------
    smt_fileio_open

    Type: Component method
    -------------------------------------------------------------------------
 */

smt_fileio_t *
    smt_fileio_open (
    smt_thread_t * thread,              //  Thread to deliver reply to
    const char * path,                  //  The optional path???
    const char * name,                  //  The name of the file to open
    smt_file_mode_e mode,               //  The mode (read/write/append)
    Bool binary,                        //  Open file for binary data?
    smt_event_t event                   //  Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking
)
{
    apr_status_t
        status;
    apr_int32_t
#if (defined (BASE_THREADSAFE))
        flag = APR_XTHREAD;
#else
        flag = 0;
#endif
    smt_fileio_t *
        self;                           //  The fileio item

self = smt_fileio_new ();
switch (mode) {
    case SMT_FILE_READ:   flag = APR_READ;                break;
    case SMT_FILE_WRITE:  flag = APR_WRITE | APR_CREATE;  break;
    case SMT_FILE_APPEND: flag = APR_WRITE | APR_APPEND;  break;
}
if (binary)
    flag |= APR_BINARY;

status = apr_file_open (&self->handle, name, flag,
                        APR_OS_DEFAULT, self->pool);
if (status == APR_SUCCESS)
    thread->result = SMT_OK;
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);
else {
    smt_fileio_destroy (&self);
    thread->result = SMT_FILEIO_ERROR;
    thread->error  = status;
    thread->catcher (thread);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_fileio_temp

    Type: Component method
    -------------------------------------------------------------------------
 */

smt_fileio_t *
    smt_fileio_temp (
    smt_thread_t * thread,              //  Thread to deliver reply to
    const char * path,                  //  The optional path???
    Bool binary,                        //  Open file for binary data?
    smt_event_t event                   //  Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking
)
{
    apr_status_t
        status;
    apr_int32_t
#if (defined (BASE_THREADSAFE))
        flag = APR_XTHREAD | APR_CREATE | APR_READ | APR_WRITE | APR_EXCL | APR_DELONCLOSE;
#else
        flag = APR_CREATE | APR_READ | APR_WRITE | APR_EXCL | APR_DELONCLOSE;
#endif
    icl_shortstr_t
        name_template;
    smt_fileio_t *
        self;                           //  The fileio item

icl_shortstr_cpy (name_template, (char *) path);
icl_shortstr_cat (name_template, "/tmp_XXXXXX");
self = smt_fileio_new ();
if (binary)
    flag |= APR_BINARY;

status = apr_file_mktemp (&self->handle, name_template, flag, self->pool);
if (status == APR_SUCCESS)
    thread->result = SMT_OK;
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);
else {
    smt_fileio_destroy (&self);
    thread->result = SMT_FILEIO_ERROR;
    thread->error  = status;
    thread->catcher (thread);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_fileio_eof

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    smt_fileio_eof (
    smt_fileio_t * self                 //  The fileio item
)
{
    Bool
        rc;                             //  The result code

rc = self->eof;

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_fileio_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_fileio_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_fileio_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_fileio_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_fileio_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_fileio_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_fileio_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_fileio file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_fileio_destroy

    Type: Component method
    Destroys a smt_fileio_t object. Takes the address of a
    smt_fileio_t reference (a pointer to a pointer) and nullifies the
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
    smt_fileio_destroy_ (
    smt_fileio_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_fileio_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    smt_fileio_annihilate (self_p);
    smt_fileio_free ((smt_fileio_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_fileio_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_fileio_t *
    smt_fileio_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_fileio_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_fileio_cache_initialise ();

self = (smt_fileio_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_fileio_free

    Type: Component method
    Freess a smt_fileio_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_fileio_free (
    smt_fileio_t * self                 //  Object reference
)
{


if (self) {

    self->object_tag = SMT_FILEIO_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_fileio_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_fileio_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_fileio_t));
icl_system_register (smt_fileio_cache_purge, smt_fileio_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_fileio_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_fileio_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_fileio_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_fileio_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_fileio_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_fileio_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_fileio_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__smt_fileio_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_fileio_component         = "smt_fileio ";
char *EMBED__smt_fileio_version           = "1.0 ";
char *EMBED__smt_fileio_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_fileio_filename          = "smt_fileio.icl ";
char *EMBED__smt_fileio_builddate         = "2011/03/01 ";
char *EMBED__smt_fileio_version_end       = "VeRsIoNeNd:ipc";

