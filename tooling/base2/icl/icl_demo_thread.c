/*---------------------------------------------------------------------------
    icl_demo_thread.c - icl_demo_thread component

    Tests and demonstrates the iCL class framework.  This class creates
    a file object that you can open, close, and work with.
    Generated from icl_demo_thread.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_thread.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_thread_t

//  Shorthands for class methods

#define self_new            icl_demo_thread_new
#define self_annihilate     icl_demo_thread_annihilate
#define self_selftest       icl_demo_thread_selftest
#define self_start          icl_demo_thread_start
#define self_join           icl_demo_thread_join
#define self_free           icl_demo_thread_free
#define self_terminate      icl_demo_thread_terminate
#define self_show           icl_demo_thread_show
#define self_destroy        icl_demo_thread_destroy
#define self_alloc          icl_demo_thread_alloc
#define self_link           icl_demo_thread_link
#define self_unlink         icl_demo_thread_unlink
#define self_show_animation  icl_demo_thread_show_animation
#define self_new_in_scope   icl_demo_thread_new_in_scope

#define icl_demo_thread_annihilate(self)  icl_demo_thread_annihilate_ (self, __FILE__, __LINE__)
static void
    icl_demo_thread_annihilate_ (
icl_demo_thread_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_demo_thread_free(self)      icl_demo_thread_free_ (self, __FILE__, __LINE__)
static void
    icl_demo_thread_free_ (
icl_demo_thread_t * self,               //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_demo_thread_alloc()         icl_demo_thread_alloc_ (__FILE__, __LINE__)
static icl_demo_thread_t *
    icl_demo_thread_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

Bool
    icl_demo_thread_animating = TRUE;   //  Animation enabled by default

void * APR_THREAD_FUNC s_test (apr_thread_t *apr_thread, void * data);
/*  -------------------------------------------------------------------------
    icl_demo_thread_new

    Type: Component method
    Creates and initialises a new icl_demo_thread_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_thread_t *
    icl_demo_thread_new_ (
    char * file,                        //  Source file for callSource file for call
    size_t line,                        //  Line number for callLine number for call
    apr_thread_start_t startup,         //  Function to start the new thread
    char * text                         //  Not documented
)
{
    icl_demo_thread_t *
        self = NULL;                    //  Object reference

    self = icl_demo_thread_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_THREAD_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_thread_show_);
#endif

    self->file          = file;
    self->line          = line;
    self->started       = FALSE;
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "icl_demo_thread_new");
#if (defined (BASE_THREADSAFE))
    self->threadattr = NULL;
    icl_apr_assert (apr_threadattr_create (&self->threadattr, self->pool));
#endif

    self->startup = startup;
    self->text    = icl_mem_strdup (text);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_thread_annihilate_ (
    icl_demo_thread_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    int
        history_last;
#endif

    icl_demo_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = file;
    self->history_line  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = line;
    self->history_type  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = self->links;
#endif

    ICL_DEMO_THREAD_ASSERT_SANE (self);

icl_mem_free (self->text);

}
/*  -------------------------------------------------------------------------
    icl_demo_thread_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_selftest (
void)
{
icl_demo_thread_t
    *thread_1,
    *thread_2,
    *thread_3,
    *thread_4,
    *thread_5,
    *thread_6,
    *thread_7,
    *thread_8;

icl_console_mode (ICL_CONSOLE_THREAD, TRUE);

thread_1 = icl_demo_thread_new (s_test, "1");
thread_2 = icl_demo_thread_new (s_test, "2");
thread_3 = icl_demo_thread_new (s_test, "3");
thread_4 = icl_demo_thread_new (s_test, "4");
thread_5 = icl_demo_thread_new (s_test, "5");
thread_6 = icl_demo_thread_new (s_test, "6");
thread_7 = icl_demo_thread_new (s_test, "7");
thread_8 = icl_demo_thread_new (s_test, "8");
icl_demo_thread_start (thread_8);
icl_demo_thread_start (thread_7);
icl_demo_thread_start (thread_6);
icl_demo_thread_start (thread_5);
icl_demo_thread_start (thread_4);
icl_demo_thread_start (thread_3);
icl_demo_thread_start (thread_2);
icl_demo_thread_start (thread_1);

icl_demo_thread_join    (thread_1);
icl_demo_thread_destroy (&thread_1);

icl_demo_thread_join    (thread_2);
icl_demo_thread_destroy (&thread_2);

icl_demo_thread_join    (thread_3);
icl_demo_thread_destroy (&thread_3);

icl_demo_thread_join    (thread_4);
icl_demo_thread_destroy (&thread_4);

icl_demo_thread_join    (thread_5);
icl_demo_thread_destroy (&thread_5);

icl_demo_thread_join    (thread_6);
icl_demo_thread_destroy (&thread_6);

icl_demo_thread_join    (thread_7);
icl_demo_thread_destroy (&thread_7);

icl_demo_thread_join    (thread_8);
icl_demo_thread_destroy (&thread_8);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_start

    Type: Component method
    Accepts a icl_demo_thread_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_thread_start (
    icl_demo_thread_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_THREAD_ASSERT_SANE (self);
if (!self->zombie) {

    self->started = TRUE;
#if (defined (BASE_THREADSAFE))
    icl_demo_thread_link (self);        //  OS thread needs its own link
    icl_apr_assert (apr_thread_create (&self->thread, self->threadattr, self->startup, self, self->pool));
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_join

    Type: Component method
    Does a 'join' to the thread which waits until the thread terminates.
    
    Don't use 'function' template since it needs to be called with a zombie
    object.
    -------------------------------------------------------------------------
 */

int
    icl_demo_thread_join (
    icl_demo_thread_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

    ICL_DEMO_THREAD_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    apr_thread_join (&rc, self->thread);
#endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_free

    Type: Component method
    Freess a icl_demo_thread_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_thread_free_ (
    icl_demo_thread_t * self,           //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = "free";
        self->history_links [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = self->links;
#endif

apr_pool_destroy (self->pool);
        memset (&self->object_tag, 0, sizeof (icl_demo_thread_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_thread_t));
        self->object_tag = ICL_DEMO_THREAD_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_thread_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    icl_demo_thread_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <icl_demo_thread zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    if (self->history_last > ICL_DEMO_THREAD_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % ICL_DEMO_THREAD_HISTORY_LENGTH;
        self->history_last %= ICL_DEMO_THREAD_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % ICL_DEMO_THREAD_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </icl_demo_thread>\n");
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_thread_destroy

    Type: Component method
    Destroys a icl_demo_thread_t object. Takes the address of a
    icl_demo_thread_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_thread_destroy_ (
    icl_demo_thread_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        icl_demo_thread_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("icl_demo_thread", "E: missing link on icl_demo_thread object");
        icl_demo_thread_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        icl_demo_thread_free_ ((icl_demo_thread_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_thread_t *
    icl_demo_thread_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_thread_t *
        self = NULL;                    //  Object reference

self = (icl_demo_thread_t *) icl_mem_alloc_ (sizeof (icl_demo_thread_t), file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_thread_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

icl_demo_thread_t *
    icl_demo_thread_link_ (
    icl_demo_thread_t * self,           //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    int
        history_last;
#endif

    if (self) {
        ICL_DEMO_THREAD_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = "link";
        self->history_links [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_unlink_ (
    icl_demo_thread_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
    int
        history_last;
#endif

    icl_demo_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        ICL_DEMO_THREAD_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("icl_demo_thread", "E: missing link on icl_demo_thread object");
            icl_demo_thread_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_THREAD)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % ICL_DEMO_THREAD_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            icl_demo_thread_annihilate_ (self_p, file, line);
        icl_demo_thread_free_ ((icl_demo_thread_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_thread_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_thread_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_thread_new_in_scope_ (
    icl_demo_thread_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for callSource file for call
    size_t line,                        //  Line number for callLine number for call
    apr_thread_start_t startup,         //  Function to start the new thread
    char * text                         //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_thread_new_ (file,line,startup,text);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_thread_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
void * APR_THREAD_FUNC
s_test (apr_thread_t *apr_thread, void *data)
{
    icl_demo_thread_t
        *self = data;
    icl_demo_cache_t
        *demo_cache;
    int
        i;

    icl_console_print (self->text);
    for (i = 0; i < 100000; i++) {
        demo_cache = icl_demo_cache_new (NULL,0);
        icl_demo_cache_destroy (&demo_cache);
    }
    icl_console_print (self->text);
    icl_demo_thread_destroy (&self);
    return NULL;
}

//  Embed the version information in the resulting binary

char *EMBED__icl_demo_thread_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_thread_component    = "icl_demo_thread ";
char *EMBED__icl_demo_thread_version      = "1.1 ";
char *EMBED__icl_demo_thread_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_thread_filename     = "icl_demo_thread.icl ";
char *EMBED__icl_demo_thread_builddate    = "2011/03/01 ";
char *EMBED__icl_demo_thread_version_end  = "VeRsIoNeNd:ipc";

