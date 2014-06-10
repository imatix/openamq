/*---------------------------------------------------------------------------
    demo_client_channel.c - demo_client_channel component

    This class implements the ASL channel class for the demo client.
    Generated from demo_client_channel.icl by icl_gen using GSL/4.
    
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
#include "demo_client_channel.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_client_channel_t

//  Shorthands for class methods

#define self_new            demo_client_channel_new
#define self_annihilate     demo_client_channel_annihilate
#define self_selftest       demo_client_channel_selftest
#define self_remove_from_all_containers  demo_client_channel_remove_from_all_containers
#define self_show           demo_client_channel_show
#define self_terminate      demo_client_channel_terminate
#define self_destroy        demo_client_channel_destroy
#define self_alloc          demo_client_channel_alloc
#define self_free           demo_client_channel_free
#define self_link           demo_client_channel_link
#define self_unlink         demo_client_channel_unlink
#define self_cache_initialise  demo_client_channel_cache_initialise
#define self_cache_purge    demo_client_channel_cache_purge
#define self_cache_terminate  demo_client_channel_cache_terminate
#define self_show_animation  demo_client_channel_show_animation
#define self_new_in_scope   demo_client_channel_new_in_scope

#define demo_client_channel_annihilate(self)  demo_client_channel_annihilate_ (self, __FILE__, __LINE__)
static void
    demo_client_channel_annihilate_ (
demo_client_channel_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_client_channel_alloc()     demo_client_channel_alloc_ (__FILE__, __LINE__)
static demo_client_channel_t *
    demo_client_channel_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define demo_client_channel_free(self)  demo_client_channel_free_ (self, __FILE__, __LINE__)
static void
    demo_client_channel_free_ (
demo_client_channel_t * self,           //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    demo_client_channel_cache_initialise (
void);

static void
    demo_client_channel_cache_purge (
void);

static void
    demo_client_channel_cache_terminate (
void);

Bool
    demo_client_channel_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    demo_client_channel_new

    Type: Component method
    Creates and initialises a new demo_client_channel_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

demo_client_channel_t *
    demo_client_channel_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_client_channel_table_t * table,   //  Table to insert into
    int key,                            //  Hash key
    smt_method_queue_t * method_queue,   //  Reply queue
    Bool * alive,                       //  Where to push alive status
    dbyte * reply_code,                 //  Where to push reply code
    char * reply_text                   //  Where to push reply text
)
{
    demo_client_channel_t *
        self = NULL;                    //  Object reference

    self = demo_client_channel_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_CLIENT_CHANNEL_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_client_channel_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->active = TRUE;

//
self->method_queue = smt_method_queue_link (method_queue);
self->alive        = alive;
self->reply_code   = reply_code;
self->reply_text   = reply_text;
if (table && self && demo_client_channel_table_insert (table, key, self))
    demo_client_channel_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_channel_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_channel_annihilate_ (
    demo_client_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    int
        history_last;
#endif

    demo_client_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = self->links;
#endif

    DEMO_CLIENT_CHANNEL_ASSERT_SANE (self);
    demo_client_channel_remove_from_all_containers (self);

smt_method_queue_unlink (&self->method_queue);

}
/*  -------------------------------------------------------------------------
    demo_client_channel_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_client_channel_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_remove_from_all_containers (
    demo_client_channel_t * self        //  The item
)
{

DEMO_CLIENT_CHANNEL_ASSERT_SANE (self);
demo_client_channel_table_remove (self);
}
/*  -------------------------------------------------------------------------
    demo_client_channel_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_client_channel_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_client_channel zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    if (self->history_last > DEMO_CLIENT_CHANNEL_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH;
        self->history_last %= DEMO_CLIENT_CHANNEL_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_client_channel>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_client_channel_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    demo_client_channel_destroy

    Type: Component method
    Destroys a demo_client_channel_t object. Takes the address of a
    demo_client_channel_t reference (a pointer to a pointer) and nullifies the
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
    demo_client_channel_destroy_ (
    demo_client_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_client_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        demo_client_channel_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("demo_client_channel", "E: missing link on demo_client_channel object");
        demo_client_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        demo_client_channel_free_ ((demo_client_channel_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_client_channel_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_client_channel_t *
    demo_client_channel_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_client_channel_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_client_channel_cache_initialise ();

self = (demo_client_channel_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_client_channel_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_channel_free

    Type: Component method
    Freess a demo_client_channel_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_client_channel_free_ (
    demo_client_channel_t * self,       //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (demo_client_channel_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_client_channel_t));
        self->object_tag = DEMO_CLIENT_CHANNEL_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_client_channel_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_client_channel_t *
    demo_client_channel_link_ (
    demo_client_channel_t * self,       //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    int
        history_last;
#endif

    if (self) {
        DEMO_CLIENT_CHANNEL_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_channel_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_unlink_ (
    demo_client_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
    int
        history_last;
#endif

    demo_client_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        DEMO_CLIENT_CHANNEL_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("demo_client_channel", "E: missing link on demo_client_channel object");
            demo_client_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_CLIENT_CHANNEL)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_CLIENT_CHANNEL_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            demo_client_channel_annihilate_ (self_p, file, line);
        demo_client_channel_free_ ((demo_client_channel_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_client_channel_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_client_channel_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_client_channel_t));
icl_system_register (demo_client_channel_cache_purge, demo_client_channel_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_client_channel_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_channel_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_client_channel_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_channel_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_client_channel_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_client_channel_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_client_channel_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_channel_new_in_scope_ (
    demo_client_channel_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_client_channel_table_t * table,   //  Table to insert into
    int key,                            //  Hash key
    smt_method_queue_t * method_queue,   //  Reply queue
    Bool * alive,                       //  Where to push alive status
    dbyte * reply_code,                 //  Where to push reply code
    char * reply_text                   //  Where to push reply text
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_client_channel_new_ (file,line,table,key,method_queue,alive,reply_code,reply_text);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_client_channel_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_client_channel_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_client_channel_component  = "demo_client_channel ";
char *EMBED__demo_client_channel_version  = "1.0 ";
char *EMBED__demo_client_channel_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_client_channel_filename  = "demo_client_channel.icl ";
char *EMBED__demo_client_channel_builddate  = "2011/03/01 ";
char *EMBED__demo_client_channel_version_end  = "VeRsIoNeNd:ipc";

