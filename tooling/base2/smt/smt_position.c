/*---------------------------------------------------------------------------
    smt_position.c - smt_position component

    The smt_position class is used to build a stack of 'positions' in an smt
    dialog.  A position consists of a 'place' (identifies thread/event/action)
    and a 'module' (action number within a given 'place').
    Generated from smt_position.icl by icl_gen using GSL/4.
    
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
#include "smt_position.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_position_t

//  Shorthands for class methods

#define self_selftest       smt_position_selftest
#define self_new            smt_position_new
#define self_remove_from_all_containers  smt_position_remove_from_all_containers
#define self_show           smt_position_show
#define self_terminate      smt_position_terminate
#define self_destroy        smt_position_destroy
#define self_annihilate     smt_position_annihilate
#define self_alloc          smt_position_alloc
#define self_free           smt_position_free
#define self_link           smt_position_link
#define self_unlink         smt_position_unlink
#define self_cache_initialise  smt_position_cache_initialise
#define self_cache_purge    smt_position_cache_purge
#define self_cache_terminate  smt_position_cache_terminate
#define self_show_animation  smt_position_show_animation
#define self_new_in_scope   smt_position_new_in_scope

#define smt_position_annihilate(self)   smt_position_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_position_annihilate_ (
smt_position_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_position_alloc()            smt_position_alloc_ (__FILE__, __LINE__)
static smt_position_t *
    smt_position_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_position_free(self)         smt_position_free_ (self, __FILE__, __LINE__)
static void
    smt_position_free_ (
smt_position_t * self,                  //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_position_cache_initialise (
void);

static void
    smt_position_cache_purge (
void);

static void
    smt_position_cache_terminate (
void);

Bool
    smt_position_animating = TRUE;      //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_position_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_position_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_position_new

    Type: Component method
    Creates and initialises a new smt_position_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

smt_position_t *
    smt_position_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_position_t *
        self = NULL;                    //  Object reference

    self = smt_position_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_POSITION_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_position_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_position_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    smt_position_remove_from_all_containers (
    smt_position_t * self               //  The item
)
{

smt_position_list_remove (self);
}
/*  -------------------------------------------------------------------------
    smt_position_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_position_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_position_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_position zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    if (self->history_last > SMT_POSITION_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_POSITION_HISTORY_LENGTH;
        self->history_last %= SMT_POSITION_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_POSITION_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_position>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_position_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_position_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_position_destroy

    Type: Component method
    Destroys a smt_position_t object. Takes the address of a
    smt_position_t reference (a pointer to a pointer) and nullifies the
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
    smt_position_destroy_ (
    smt_position_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_position_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_position_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_position", "E: missing link on smt_position object");
        smt_position_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_position_free_ ((smt_position_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_position_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_position_annihilate_ (
    smt_position_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    int
        history_last;
#endif

    smt_position_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_POSITION_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_POSITION_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_POSITION_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_POSITION_HISTORY_LENGTH] = self->links;
#endif

    SMT_POSITION_ASSERT_SANE (self);
    smt_position_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    smt_position_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_position_t *
    smt_position_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_position_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_position_cache_initialise ();

self = (smt_position_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_position_free

    Type: Component method
    Freess a smt_position_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_position_free_ (
    smt_position_t * self,              //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_POSITION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_POSITION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_POSITION_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_POSITION_HISTORY_LENGTH] = self->links;
#endif

    self->object_tag = SMT_POSITION_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_position_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_position_t *
    smt_position_link_ (
    smt_position_t * self,              //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    int
        history_last;
#endif

    if (self) {
        SMT_POSITION_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_POSITION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_POSITION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_POSITION_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_POSITION_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_position_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_position_unlink_ (
    smt_position_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
    int
        history_last;
#endif

    smt_position_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_POSITION_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_position", "E: missing link on smt_position object");
            smt_position_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_POSITION)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_POSITION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_POSITION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_POSITION_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_POSITION_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_position_annihilate_ (self_p, file, line);
        smt_position_free_ ((smt_position_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_position_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_position_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_position_t));
icl_system_register (smt_position_cache_purge, smt_position_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_position_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_position_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_position_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_position_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_position_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_position_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_position_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_position_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_position_new_in_scope_ (
    smt_position_t * * self_p,          //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_position_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_position_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_position_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_position_component       = "smt_position ";
char *EMBED__smt_position_version         = "1.0 ";
char *EMBED__smt_position_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_position_filename        = "smt_position.icl ";
char *EMBED__smt_position_builddate       = "2011/03/01 ";
char *EMBED__smt_position_version_end     = "VeRsIoNeNd:ipc";

