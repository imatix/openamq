/*---------------------------------------------------------------------------
    demo_server_channel.c - demo_server_channel component

    This class implements the ASL demo server channel class.
    Generated from demo_server_channel.icl by smt_object_gen using GSL/4.
    
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
#include "demo_server_channel.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_server_channel_t

//  Shorthands for class methods

#define self_consume        demo_server_channel_consume
#define self_cancel         demo_server_channel_cancel
#define self_initialise     demo_server_channel_initialise
#define self_new            demo_server_channel_new
#define self_annihilate     demo_server_channel_annihilate
#define self_spend          demo_server_channel_spend
#define self_earn           demo_server_channel_earn
#define self_error          demo_server_channel_error
#define self_selftest       demo_server_channel_selftest
#define self_remove_from_all_containers  demo_server_channel_remove_from_all_containers
#define self_show           demo_server_channel_show
#define self_destroy        demo_server_channel_destroy
#define self_unlink         demo_server_channel_unlink
#define self_show_animation  demo_server_channel_show_animation
#define self_free           demo_server_channel_free
#define self_terminate      demo_server_channel_terminate
#define self_alloc          demo_server_channel_alloc
#define self_link           demo_server_channel_link
#define self_cache_initialise  demo_server_channel_cache_initialise
#define self_cache_purge    demo_server_channel_cache_purge
#define self_cache_terminate  demo_server_channel_cache_terminate
#define self_new_in_scope   demo_server_channel_new_in_scope

static void
    demo_server_channel_initialise (
void);

#define demo_server_channel_annihilate(self)  demo_server_channel_annihilate_ (self, __FILE__, __LINE__)
static int
    demo_server_channel_annihilate_ (
demo_server_channel_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_server_channel_alloc()     demo_server_channel_alloc_ (__FILE__, __LINE__)
static demo_server_channel_t *
    demo_server_channel_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_server_channel_cache_initialise (
void);

static void
    demo_server_channel_cache_purge (
void);

static void
    demo_server_channel_cache_terminate (
void);

Bool
    demo_server_channel_animating = TRUE;  //  Animation enabled by default
static Bool
    s_demo_server_channel_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_server_channel_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static volatile qbyte
    s_channel_id_seq_lo,                //  Next channel id
    s_channel_id_seq_hi;                //  High part (64-bits)
/*  -------------------------------------------------------------------------
    demo_server_channel_consume

    Type: Component method
    Accepts a demo_server_channel_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Creates a new channel consumer to specified queue.  Only has an effect
    in Direct Protocol feed access.  Consumers are implemented as leases on
    queues.
    - find queue for consume
    - check queue is exclusive (required for lease)
    - lease may not yet be activated (happens asynchronously)
    - return consume-tag from queue name
    - set queue->feed_on to true
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_consume (
    demo_server_channel_t * self,       //  Reference to object
    demo_server_method_t * method       //  Consume method
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
if (!self->zombie) {

    //

    //

if (self) {
    assert (self->thread);
    if (demo_server_channel_agent_consume (self->thread,method)) {
        //icl_console_print ("Error sending 'consume' method to demo_server_channel agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_cancel

    Type: Component method
    Accepts a demo_server_channel_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Cancels a channel consumer on specified queue.  Only has an effect in
    Direct Protocol feed access.  Consumers are implemented as leases on
    queues.
    - find queue for cancel (consumer-tag is queue name)
    - check queue is exclusive (required for lease)
    - set queue->feed_on to true
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_cancel (
    demo_server_channel_t * self,       //  Reference to object
    demo_server_method_t * method       //  Cancel method
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
if (!self->zombie) {

    //

    //

if (self) {
    assert (self->thread);
    if (demo_server_channel_agent_cancel (self->thread,method)) {
        //icl_console_print ("Error sending 'cancel' method to demo_server_channel agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_channel_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_server_channel_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_server_channel_mutex)
            s_demo_server_channel_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_server_channel_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_server_channel_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

demo_server_channel_agent_init ();

s_channel_id_seq_hi = 0;
s_channel_id_seq_lo = 1;
            s_demo_server_channel_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_server_channel_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_server_channel_new

    Type: Component method
    Creates and initialises a new demo_server_channel_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

demo_server_channel_t *
    demo_server_channel_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_server_channel_table_t * table,   //  Table to insert into
    demo_server_connection_t * connection,   //  Parent connection
    dbyte number                        //  Channel number
)
{
#define key number
qbyte
    seq_lo,
    seq_hi;
    demo_server_channel_t *
        self = NULL;                    //  Object reference

if (!s_demo_server_channel_active)
    self_initialise ();
    self = demo_server_channel_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_SERVER_CHANNEL_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_server_channel_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->thread = demo_server_channel_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->active = TRUE;

//
self->connection = demo_server_connection_link (connection);
self->number = number;

//  Get 64-bit channel id, no wraparound handling
seq_lo = icl_atomic_inc32 (&s_channel_id_seq_lo);
if (seq_lo == 0)
    seq_hi = icl_atomic_inc32 (&s_channel_id_seq_hi);
else
    seq_hi = s_channel_id_seq_hi;
icl_shortstr_fmt (self->id, "%x-%x", seq_hi, seq_lo);
if (table && self && demo_server_channel_table_insert (table, key, self))
    demo_server_channel_destroy (&self);
}

    return (self);
}
#undef key
/*  -------------------------------------------------------------------------
    demo_server_channel_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    demo_server_channel_annihilate_ (
    demo_server_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    int
        history_last;
#endif

    demo_server_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (demo_server_channel_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to demo_server_channel agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_spend

    Type: Component method
    Accepts a demo_server_channel_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_spend (
    demo_server_channel_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
if (!self->zombie) {

icl_atomic_dec32 ((volatile qbyte *) &self->credit);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_earn

    Type: Component method
    Accepts a demo_server_channel_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_earn (
    demo_server_channel_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
if (!self->zombie) {

icl_atomic_inc32 ((volatile qbyte *) &self->credit);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_error

    Type: Component method
    Accepts a demo_server_channel_t reference and returns zero in case of success,
    1 in case of errors.
    Raises a channel exception.
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_error (
    demo_server_channel_t * self,       //  Reference to object
    dbyte reply_code,                   //  Error code
    char * reply_text,                  //  Error text
    dbyte faulting_class_id,            //  Faulting class id
    dbyte faulting_method_id            //  Faulting method id
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
if (!self->zombie) {

//
demo_server_agent_channel_error (
    self->connection->thread, self, reply_code, reply_text,
    faulting_class_id, faulting_method_id);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_server_channel_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_remove_from_all_containers (
    demo_server_channel_t * self        //  The item
)
{

DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
demo_server_channel_table_remove (self);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_server_channel_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_server_channel zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    if (self->history_last > DEMO_SERVER_CHANNEL_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_SERVER_CHANNEL_HISTORY_LENGTH;
        self->history_last %= DEMO_SERVER_CHANNEL_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_SERVER_CHANNEL_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_server_channel>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_server_channel_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_server_channel_destroy_ (
    demo_server_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_server_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

    if (self) {
demo_server_channel_remove_from_all_containers (self);
        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            rc = demo_server_channel_annihilate_ (self_p, file, line);
        else
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
            demo_server_channel_free (self);
        *self_p = NULL;
    }

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_unlink_ (
    demo_server_channel_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    int
        history_last;
#endif
    demo_server_channel_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on demo_server_channel object");
            demo_server_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                demo_server_channel_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                demo_server_channel_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    demo_server_channel_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_server_channel_animating = enabled;

demo_server_channel_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_free

    Type: Component method
    Freess a demo_server_channel_t object.
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_free_ (
    demo_server_channel_t * self,       //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (demo_server_channel_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_server_channel_t));
        self->object_tag = DEMO_SERVER_CHANNEL_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_server_channel_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_terminate (
void)
{

if (s_demo_server_channel_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_server_channel_mutex);
#endif
        s_demo_server_channel_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_server_channel_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_server_channel_t *
    demo_server_channel_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_server_channel_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_server_channel_cache_initialise ();

self = (demo_server_channel_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_server_channel_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_server_channel_t *
    demo_server_channel_link_ (
    demo_server_channel_t * self,       //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
    int
        history_last;
#endif

    if (self) {
        DEMO_SERVER_CHANNEL_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CHANNEL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_SERVER_CHANNEL_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_server_channel_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_server_channel_t));
icl_system_register (demo_server_channel_cache_purge, demo_server_channel_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_server_channel_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_channel_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_server_channel_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_channel_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_server_channel_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_channel_new_in_scope_ (
    demo_server_channel_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_server_channel_table_t * table,   //  Table to insert into
    demo_server_connection_t * connection,   //  Parent connection
    dbyte number                        //  Channel number
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_server_channel_new_ (file,line,table,connection,number);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_server_channel_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_server_channel_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_server_channel_component  = "demo_server_channel ";
char *EMBED__demo_server_channel_version  = "1.0 ";
char *EMBED__demo_server_channel_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_server_channel_filename  = "demo_server_channel.icl ";
char *EMBED__demo_server_channel_builddate  = "2011/03/01 ";
char *EMBED__demo_server_channel_version_end  = "VeRsIoNeNd:ipc";

