/*---------------------------------------------------------------------------
    icl_demo_local.c - icl_demo_local component

    Generated from icl_demo_local.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_local.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_local_t

//  Shorthands for class methods

#define self_new            icl_demo_local_new
#define self_annihilate     icl_demo_local_annihilate
#define self_selftest       icl_demo_local_selftest
#define self_initialise     icl_demo_local_initialise
#define self_terminate      icl_demo_local_terminate
#define self_show           icl_demo_local_show
#define self_destroy        icl_demo_local_destroy
#define self_alloc          icl_demo_local_alloc
#define self_free           icl_demo_local_free
#define self_show_animation  icl_demo_local_show_animation
#define self_new_in_scope   icl_demo_local_new_in_scope

static void
    icl_demo_local_annihilate (
icl_demo_local_t * ( * self_p )         //  Reference to object reference
);

static void
    icl_demo_local_initialise (
void);

static void
    icl_demo_local_terminate (
void);

#define icl_demo_local_alloc()          icl_demo_local_alloc_ (__FILE__, __LINE__)
static icl_demo_local_t *
    icl_demo_local_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_local_free (
icl_demo_local_t * self                 //  Object reference
);

Bool
    icl_demo_local_animating = TRUE;    //  Animation enabled by default
static Bool
    s_icl_demo_local_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_icl_demo_local_mutex   = NULL;
#endif
#if (defined (BASE_THREADSAFE))
static apr_threadkey_t
    *threadkey;                         //  To hold thread-local cache header
#else
static icl_demo_local_t
        *s_cache = NULL;
#endif

/*  -------------------------------------------------------------------------
    icl_demo_local_new

    Type: Component method
    Creates and initialises a new icl_demo_local_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_local_t *
    icl_demo_local_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_demo_local_t *
        self = NULL;                    //  Object reference

if (!s_icl_demo_local_active)
    self_initialise ();
self = icl_demo_local_alloc_ (file, line);
if (self) {
    self->object_tag   = ICL_DEMO_LOCAL_ALIVE;

self->name    = icl_mem_strdup (name);
self->address = icl_mem_strdup (address);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_local_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_local_annihilate (
    icl_demo_local_t * ( * self_p )     //  Reference to object reference
)
{

    icl_demo_local_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_DEMO_LOCAL_ASSERT_SANE (self);

icl_mem_free (self->name);
icl_mem_free (self->address);

}
/*  -------------------------------------------------------------------------
    icl_demo_local_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_local_selftest (
void)
{
icl_demo_local_t *
    item;

self_initialise ();
item = icl_demo_local_new ("John", "Main street");
icl_demo_local_destroy (&item);
item = icl_demo_local_new ("Janet", "Side avenue");
icl_demo_local_destroy (&item);
icl_demo_local_destroy (&item);
self_terminate ();
}
/*  -------------------------------------------------------------------------
    icl_demo_local_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_local_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_icl_demo_local_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_icl_demo_local_mutex)
            s_icl_demo_local_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_icl_demo_local_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_icl_demo_local_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_create (&threadkey, NULL, icl_global_pool));
#endif

            s_icl_demo_local_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_icl_demo_local_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    icl_demo_local_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_local_terminate (
void)
{

if (s_icl_demo_local_active) {
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_delete (threadkey));
#endif

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_icl_demo_local_mutex);
#endif
        s_icl_demo_local_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    icl_demo_local_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_local_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_local_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_local file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_local_destroy

    Type: Component method
    Destroys a icl_demo_local_t object. Takes the address of a
    icl_demo_local_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_local_destroy_ (
    icl_demo_local_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_local_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_local_annihilate (self_p);
    icl_demo_local_free ((icl_demo_local_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_local_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_local_t *
    icl_demo_local_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    icl_demo_local_t
        **s_cache;
    void
        *s_cache_p;
#endif

    icl_demo_local_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
    s_cache = s_cache_p;
    if (!s_cache) {
        s_cache = malloc (sizeof (icl_demo_local_t **));
        *s_cache = NULL;
        icl_apr_assert (apr_threadkey_private_set (s_cache, threadkey));
    }
    else
        self = *s_cache;

    if (self) {
        *s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = (icl_demo_local_t *) malloc (sizeof (icl_demo_local_t));
#else
    if (s_cache) {
        self = s_cache;
        s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = (icl_demo_local_t *) malloc (sizeof (icl_demo_local_t));
#endif
    if (self)
        memset (self, 0, sizeof (icl_demo_local_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_local_free

    Type: Component method
    Freess a icl_demo_local_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_local_free (
    icl_demo_local_t * self             //  Object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_demo_local_t
        **s_cache;
    void
        *s_cache_p;
#endif


if (self) {

        memset (&self->object_tag, 0, sizeof (icl_demo_local_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_local_t));
        self->object_tag = ICL_DEMO_LOCAL_DEAD;
#if (defined (BASE_THREADSAFE))
        icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
        s_cache = s_cache_p;
        self->local_next = *s_cache;
        *s_cache = self;
#else
        self->local_next = s_cache;
        s_cache = self;
#endif
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_local_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_local_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_local_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_local_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_local_new_in_scope_ (
    icl_demo_local_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_local_new_ (file,line,name,address);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_local_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_local_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_local_component     = "icl_demo_local ";
char *EMBED__icl_demo_local_version       = "1.0 ";
char *EMBED__icl_demo_local_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_local_filename      = "icl_demo_local.icl ";
char *EMBED__icl_demo_local_builddate     = "2011/03/01 ";
char *EMBED__icl_demo_local_version_end   = "VeRsIoNeNd:ipc";

