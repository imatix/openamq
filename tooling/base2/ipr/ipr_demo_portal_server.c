/*---------------------------------------------------------------------------
    ipr_demo_portal_server.c - ipr_demo_portal_server component

    Generated from ipr_demo_portal_server.icl by icl_gen using GSL/4.
    
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
#include "ipr_demo_portal_server.h"     //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_demo_portal_server_t

//  Shorthands for class methods

#define self_selftest       ipr_demo_portal_server_selftest
#define self_print          ipr_demo_portal_server_print
#define self_new            ipr_demo_portal_server_new
#define self_annihilate     ipr_demo_portal_server_annihilate
#define self_initialise     ipr_demo_portal_server_initialise
#define self_terminate      ipr_demo_portal_server_terminate
#define self__ipr_demo_portal_new  ipr_demo_portal_server__ipr_demo_portal_new
#define self__ipr_demo_portal_factory  ipr_demo_portal_server__ipr_demo_portal_factory
#define self__ipr_demo_portal_unbind  ipr_demo_portal_server__ipr_demo_portal_unbind
#define self_hello          ipr_demo_portal_server_hello
#define self__ipr_demo_portal_link  ipr_demo_portal_server__ipr_demo_portal_link
#define self__ipr_demo_portal_unlink  ipr_demo_portal_server__ipr_demo_portal_unlink
#define self_show           ipr_demo_portal_server_show
#define self_destroy        ipr_demo_portal_server_destroy
#define self_alloc          ipr_demo_portal_server_alloc
#define self_free           ipr_demo_portal_server_free
#define self_link           ipr_demo_portal_server_link
#define self_unlink         ipr_demo_portal_server_unlink
#define self_show_animation  ipr_demo_portal_server_show_animation
#define self_new_in_scope   ipr_demo_portal_server_new_in_scope

static int
    ipr_demo_portal_server_print (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal,             //  Portal back to client
char * string                           //  Not documented
);

#define ipr_demo_portal_server_annihilate(self)  ipr_demo_portal_server_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_demo_portal_server_annihilate_ (
ipr_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_demo_portal_server_initialise (
void);

static void
    ipr_demo_portal_server_terminate (
void);

static void
    ipr_demo_portal_server__ipr_demo_portal_unbind (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal              //  Portal reference
);

static int
    ipr_demo_portal_server_hello (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal              //  Portal back to client
);

static void *
    ipr_demo_portal_server__ipr_demo_portal_link (
void * object                           //  Reference cast as a void *
);

static void
    ipr_demo_portal_server__ipr_demo_portal_unlink (
void * object_p                         //  Reference pointer cast as a void *
);

#define ipr_demo_portal_server_alloc()  ipr_demo_portal_server_alloc_ (__FILE__, __LINE__)
static ipr_demo_portal_server_t *
    ipr_demo_portal_server_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_demo_portal_server_free(self)  ipr_demo_portal_server_free_ (self, __FILE__, __LINE__)
static void
    ipr_demo_portal_server_free_ (
ipr_demo_portal_server_t * self,        //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

Bool
    ipr_demo_portal_server_animating = TRUE;  //  Animation enabled by default
static Bool
    s_ipr_demo_portal_server_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_ipr_demo_portal_server_mutex  = NULL;
#endif

static ipr_looseref_list_t
    *s_ipr_demo_portal_list;            //  List of portal factories
static Bool
    s_ipr_demo_portal_stopped;          //  Is this portal factory stopped?
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_server_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_print

    Type: Component method
    This is the print incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

static int
    ipr_demo_portal_server_print (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal,         //  Portal back to client
    char * string                       //  Not documented
)
{
ipr_demo_portal_server_t
    *self;
    int
        rc = 0;                         //  Return code

self = (ipr_demo_portal_server_t *) self_v;
icl_console_print ("SERVER PRINT: %s", string);
ipr_demo_portal_response_mogrify (portal, string, NULL, TRUE);
rc = 666;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_new

    Type: Component method
    Creates and initialises a new ipr_demo_portal_server_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_server_t *
    ipr_demo_portal_server_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_demo_portal_server_t *
        self = NULL;                    //  Object reference

if (!s_ipr_demo_portal_server_active)
    self_initialise ();
    self = ipr_demo_portal_server_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_DEMO_PORTAL_SERVER_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_demo_portal_server_show_);
#endif

self->ipr_demo_portal_list = ipr_looseref_list_new ();
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server_annihilate_ (
    ipr_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif

ipr_demo_portal_t
    *ipr_demo_portal;               //  Portal object
    ipr_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif

    IPR_DEMO_PORTAL_SERVER_ASSERT_SANE (self);

//
if (icl_atomic_cas32 ((volatile qbyte *) &self->ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_list))) {
        ipr_demo_portal->server_looseref = NULL;
        ipr_demo_portal_destroy (&ipr_demo_portal);
    }
}
ipr_looseref_list_destroy (&self->ipr_demo_portal_list);

}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_ipr_demo_portal_server_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_ipr_demo_portal_server_mutex)
            s_ipr_demo_portal_server_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_ipr_demo_portal_server_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_ipr_demo_portal_server_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

s_ipr_demo_portal_list = ipr_looseref_list_new ();
            s_ipr_demo_portal_server_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_ipr_demo_portal_server_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server_terminate (
void)
{
ipr_demo_portal_t
    *ipr_demo_portal;               //  Portal factory object

if (s_ipr_demo_portal_server_active) {

//
if (icl_atomic_cas32 ((volatile qbyte *) &s_ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (s_ipr_demo_portal_list))) {
        ipr_demo_portal->server_looseref = NULL;
        ipr_demo_portal_destroy (&ipr_demo_portal);
    }
}
ipr_looseref_list_destroy (&s_ipr_demo_portal_list);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_ipr_demo_portal_server_mutex);
#endif
        s_ipr_demo_portal_server_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server__ipr_demo_portal_new

    Type: Component method
    Creates and initialises a new portal instance.  If the _server argument is
    null, creates a hidden server object that can only be referenced through
    the portal.  This is recommended for most purposes.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_t *
    ipr_demo_portal_server__ipr_demo_portal_new (
    void * self_v,                      //  Untyped server reference
    char * name                         //  Not documented
)
{
ipr_demo_portal_server_t
    *self;
    ipr_demo_portal_t *
        ipr_demo_portal;                //  Not documented

//
//  Grab a link to a new or the provided server object
if (self_v)
    self = self_link ((ipr_demo_portal_server_t *) self_v);
else
    self = self_new ();

ipr_demo_portal = ipr_demo_portal_new (self, self__ipr_demo_portal_link, self__ipr_demo_portal_unlink, name);
ipr_demo_portal->server_looseref = ipr_looseref_queue (
    self->ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));

//  Build virtual methods for server-side methods
ipr_demo_portal->server_unbind = self__ipr_demo_portal_unbind;
ipr_demo_portal->request_hello = self_hello;
ipr_demo_portal->request_print = self_print;
//  Drop link to server object
self_unlink (&self);

    return (ipr_demo_portal);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server__ipr_demo_portal_factory

    Type: Component method
    Creates and initialises a new portal factory instance.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_t *
    ipr_demo_portal_server__ipr_demo_portal_factory (
    char * name                         //  Not documented
)
{
    ipr_demo_portal_t *
        ipr_demo_portal;                //  Not documented

//
if (!s_ipr_demo_portal_server_active)
    self_initialise ();             //  Since we don't call ipr_demo_portal_server_new
ipr_demo_portal = ipr_demo_portal_new (NULL, self__ipr_demo_portal_link, self__ipr_demo_portal_unlink, name);
ipr_demo_portal->server_looseref = ipr_looseref_queue (
    s_ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));

//  Build virtual methods for server-side methods
ipr_demo_portal->server_new = self__ipr_demo_portal_new;

    return (ipr_demo_portal);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server__ipr_demo_portal_unbind

    Type: Component method
    Server-to-portal unbind.
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server__ipr_demo_portal_unbind (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal          //  Portal reference
)
{

//
//  Drop link to portal and remove portal from our list of active portals
if (portal->server_looseref) {
    ipr_looseref_destroy (&portal->server_looseref);
    ipr_demo_portal_unlink (&portal);
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_hello

    Type: Component method
    This is the hello incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

static int
    ipr_demo_portal_server_hello (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal          //  Portal back to client
)
{
ipr_demo_portal_server_t
    *self;
    int
        rc = 0;                         //  Return code

self = (ipr_demo_portal_server_t *) self_v;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server__ipr_demo_portal_link

    Type: Component method
    -------------------------------------------------------------------------
 */

static void *
    ipr_demo_portal_server__ipr_demo_portal_link (
    void * object                       //  Reference cast as a void *
)
{

//
object = (void *) ipr_demo_portal_server_link (((ipr_demo_portal_server_t *) object));

    return (object);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server__ipr_demo_portal_unlink

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server__ipr_demo_portal_unlink (
    void * object_p                     //  Reference pointer cast as a void *
)
{

//
ipr_demo_portal_server_unlink (((ipr_demo_portal_server_t **) object_p));
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_server_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_demo_portal_server_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_demo_portal_server zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    if (self->history_last > IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH;
        self->history_last %= IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_demo_portal_server>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_destroy

    Type: Component method
    Destroys a ipr_demo_portal_server_t object. Takes the address of a
    ipr_demo_portal_server_t reference (a pointer to a pointer) and nullifies the
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
    ipr_demo_portal_server_destroy_ (
    ipr_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_demo_portal_server_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_demo_portal_server", "E: missing link on ipr_demo_portal_server object");
        ipr_demo_portal_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_demo_portal_server_free_ ((ipr_demo_portal_server_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_demo_portal_server_t *
    ipr_demo_portal_server_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_demo_portal_server_t *
        self = NULL;                    //  Object reference

self = (ipr_demo_portal_server_t *) icl_mem_alloc_ (sizeof (ipr_demo_portal_server_t), file, line);
if (self)
    memset (self, 0, sizeof (ipr_demo_portal_server_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_free

    Type: Component method
    Freess a ipr_demo_portal_server_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_server_free_ (
    ipr_demo_portal_server_t * self,    //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (ipr_demo_portal_server_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_demo_portal_server_t));
        self->object_tag = IPR_DEMO_PORTAL_SERVER_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_server_t *
    ipr_demo_portal_server_link_ (
    ipr_demo_portal_server_t * self,    //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif

    if (self) {
        IPR_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_server_unlink_ (
    ipr_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif

    ipr_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_demo_portal_server", "E: missing link on ipr_demo_portal_server object");
            ipr_demo_portal_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_SERVER)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_demo_portal_server_annihilate_ (self_p, file, line);
        ipr_demo_portal_server_free_ ((ipr_demo_portal_server_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_server_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_demo_portal_server_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_server_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_server_new_in_scope_ (
    ipr_demo_portal_server_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_demo_portal_server_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_demo_portal_server_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_demo_portal_server_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_demo_portal_server_component  = "ipr_demo_portal_server ";
char *EMBED__ipr_demo_portal_server_version  = "1.1 ";
char *EMBED__ipr_demo_portal_server_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_demo_portal_server_filename  = "ipr_demo_portal_server.icl ";
char *EMBED__ipr_demo_portal_server_builddate  = "2011/03/01 ";
char *EMBED__ipr_demo_portal_server_version_end  = "VeRsIoNeNd:ipc";

