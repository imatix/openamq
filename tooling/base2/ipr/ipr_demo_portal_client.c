/*---------------------------------------------------------------------------
    ipr_demo_portal_client.c - ipr_demo_portal_client component

    Generated from ipr_demo_portal_client.icl by icl_gen using GSL/4.
    
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
#include "ipr_demo_portal_client.h"     //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_demo_portal_client_t

//  Shorthands for class methods

#define self_annihilate     ipr_demo_portal_client_annihilate
#define self_mogrify        ipr_demo_portal_client_mogrify
#define self_selftest       ipr_demo_portal_client_selftest
#define self_new            ipr_demo_portal_client_new
#define self__ipr_demo_portal_bind  ipr_demo_portal_client__ipr_demo_portal_bind
#define self__ipr_demo_portal_unbind  ipr_demo_portal_client__ipr_demo_portal_unbind
#define self_print          ipr_demo_portal_client_print
#define self_terminate      ipr_demo_portal_client_terminate
#define self_show           ipr_demo_portal_client_show
#define self_destroy        ipr_demo_portal_client_destroy
#define self_alloc          ipr_demo_portal_client_alloc
#define self_free           ipr_demo_portal_client_free
#define self_link           ipr_demo_portal_client_link
#define self_unlink         ipr_demo_portal_client_unlink
#define self_show_animation  ipr_demo_portal_client_show_animation
#define self_new_in_scope   ipr_demo_portal_client_new_in_scope

#define ipr_demo_portal_client_annihilate(self)  ipr_demo_portal_client_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_demo_portal_client_annihilate_ (
ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static int
    ipr_demo_portal_client_mogrify (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal,             //  Portal back to server
char * string,                          //  Not documented
ipr_bucket_t * bucket,                  //  Not documented
Bool option                             //  Not documented
);

static void
    ipr_demo_portal_client__ipr_demo_portal_unbind (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal              //  Portal reference
);

static int
    ipr_demo_portal_client_print (
void * self_v,                          //  Self as void pointer
ipr_demo_portal_t * portal,             //  Portal back to server
char * string                           //  Not documented
);

#define ipr_demo_portal_client_alloc()  ipr_demo_portal_client_alloc_ (__FILE__, __LINE__)
static ipr_demo_portal_client_t *
    ipr_demo_portal_client_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_demo_portal_client_free(self)  ipr_demo_portal_client_free_ (self, __FILE__, __LINE__)
static void
    ipr_demo_portal_client_free_ (
ipr_demo_portal_client_t * self,        //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

Bool
    ipr_demo_portal_client_animating = TRUE;  //  Animation enabled by default

/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_client_annihilate_ (
    ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    int
        history_last;
#endif

ipr_demo_portal_t
    *ipr_demo_portal;               //  Portal object
    ipr_demo_portal_client_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = self->links;
#endif

    IPR_DEMO_PORTAL_CLIENT_ASSERT_SANE (self);

//
if (icl_atomic_cas32 ((volatile qbyte *) &self->ipr_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_list))) {
        ipr_demo_portal->client_looseref = NULL;
        ipr_demo_portal_destroy (&ipr_demo_portal);
    }
    ipr_looseref_list_destroy (&self->ipr_demo_portal_list);
    //  Destroy own portal factories
    while ((ipr_demo_portal = (ipr_demo_portal_t *) ipr_looseref_pop (self->ipr_demo_portal_factory_list))) {
        ipr_demo_portal->client_looseref = NULL;
        ipr_demo_portal_destroy (&ipr_demo_portal);
    }
    ipr_looseref_list_destroy (&self->ipr_demo_portal_factory_list);
}

ipr_demo_portal_destroy (&self->portal);

}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_mogrify

    Type: Component method
    This is the mogrify incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

static int
    ipr_demo_portal_client_mogrify (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal,         //  Portal back to server
    char * string,                      //  Not documented
    ipr_bucket_t * bucket,              //  Not documented
    Bool option                         //  Not documented
)
{
ipr_demo_portal_client_t
    *self;
    int
        rc = 0;                         //  Return code

self = (ipr_demo_portal_client_t *) self_v;
icl_console_print ("CLIENT MOGRIFY: %s", string);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_selftest (
void)
{
ipr_demo_portal_client_t
    *client;
ipr_demo_portal_server_t
    *server;
ipr_demo_portal_t
    *portal,
    *factory;
ipr_looseref_t
    *looseref;                      //  Portal reference

//
//  Create an explicit server object and talk to it via a portal
client = ipr_demo_portal_client_new ();
server = ipr_demo_portal_server_new ();
portal = ipr_demo_portal_server__ipr_demo_portal_new (server, "A");
ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
ipr_demo_portal_request_print (portal, "Test explicit server");
ipr_demo_portal_server_destroy (&server);
ipr_demo_portal_client_destroy (&client);
ipr_demo_portal_destroy (&portal);

//
//  Create an implicit server object and talk to it via a portal
client = ipr_demo_portal_client_new ();
portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "B");
ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
assert (ipr_demo_portal_request_print (portal, "Test implicit server") == 666);
ipr_demo_portal_destroy (&portal);
ipr_demo_portal_client_destroy (&client);

//
//  Implicit test but destroy server & portal via client
client = ipr_demo_portal_client_new ();
portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "C");
ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
ipr_demo_portal_request_print (portal, "Test indirect destroy");
ipr_demo_portal_unlink (&portal);
ipr_demo_portal_client_destroy (&client);

//
//  Test portal lookup via client looseref list
client = ipr_demo_portal_client_new ();
portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "D");
ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
ipr_demo_portal_unlink (&portal);
//  This can be done in a different part of the app
looseref = ipr_looseref_list_first (client->ipr_demo_portal_list);
assert (looseref);
portal = (ipr_demo_portal_t *) (looseref->object);
assert (streq (portal->name, "D"));
ipr_demo_portal_request_print (portal, "Test portal lookup");
ipr_demo_portal_client_destroy (&client);

//  Use portal property in client object
client = ipr_demo_portal_client_new ();
client->portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "D");
ipr_demo_portal_client__ipr_demo_portal_bind (client, client->portal);
ipr_demo_portal_request_print (portal, "Using portal property");
ipr_demo_portal_destroy (&client->portal);
ipr_demo_portal_client_destroy (&client);

//  Use a portal factory to create a new server portal
//   - First, create a factory and bind it to ourselves
client = ipr_demo_portal_client_new ();
factory = ipr_demo_portal_server__ipr_demo_portal_factory ("C");
ipr_demo_portal_client__ipr_demo_portal_bind (client, factory);
//   - Next, invoke the factory method to get a new server and portal
portal = ipr_demo_portal_factory (factory, "D");
ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
//   - Use the server portal
ipr_demo_portal_request_print (portal, "portal factory test");
//   - Shut-down the portals
ipr_demo_portal_destroy (&portal);
ipr_demo_portal_destroy (&factory);
ipr_demo_portal_client_destroy (&client);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_new

    Type: Component method
    Creates and initialises a new ipr_demo_portal_client_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_client_t *
    ipr_demo_portal_client_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_demo_portal_client_t *
        self = NULL;                    //  Object reference

    self = ipr_demo_portal_client_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_DEMO_PORTAL_CLIENT_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_demo_portal_client_show_);
#endif

self->ipr_demo_portal_list = ipr_looseref_list_new ();
self->ipr_demo_portal_factory_list = ipr_looseref_list_new ();
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client__ipr_demo_portal_bind

    Type: Component method
    Accepts a ipr_demo_portal_client_t reference and returns zero in case of success,
    1 in case of errors.
    Binds a ipr_demo_portal portal, ready for use. This method is for use
    outside the ipr_demo_portal class.
    -------------------------------------------------------------------------
 */

int
    ipr_demo_portal_client__ipr_demo_portal_bind (
    ipr_demo_portal_client_t * self,    //  Reference to object
    ipr_demo_portal_t * ipr_demo_portal  //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

IPR_DEMO_PORTAL_CLIENT_ASSERT_SANE (self);
if (!self->zombie) {

//
ipr_demo_portal->client_object = self;
if (ipr_demo_portal->server_object)
    ipr_demo_portal->client_looseref = ipr_looseref_queue (
        self->ipr_demo_portal_list, ipr_demo_portal_link (ipr_demo_portal));
else                                //  server_object is null for factories
    ipr_demo_portal->client_looseref = ipr_looseref_queue (
        self->ipr_demo_portal_factory_list, ipr_demo_portal_link (ipr_demo_portal));

//  Build virtual function table for client-side methods
ipr_demo_portal->client_unbind = self__ipr_demo_portal_unbind;
ipr_demo_portal->response_print = self_print;
ipr_demo_portal->response_mogrify = self_mogrify;

//  Signal that portal is ready for use
ipr_demo_portal_ready (ipr_demo_portal);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client__ipr_demo_portal_unbind

    Type: Component method
    Client-to-portal unbind.
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_client__ipr_demo_portal_unbind (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal          //  Portal reference
)
{

//
//  Drop link to portal and remove portal from our list of active portals
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    ipr_demo_portal_unlink (&portal);
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_print

    Type: Component method
    This is the print incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

static int
    ipr_demo_portal_client_print (
    void * self_v,                      //  Self as void pointer
    ipr_demo_portal_t * portal,         //  Portal back to server
    char * string                       //  Not documented
)
{
ipr_demo_portal_client_t
    *self;
    int
        rc = 0;                         //  Return code

self = (ipr_demo_portal_client_t *) self_v;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_demo_portal_client_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_demo_portal_client zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    if (self->history_last > IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH;
        self->history_last %= IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_demo_portal_client>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_destroy

    Type: Component method
    Destroys a ipr_demo_portal_client_t object. Takes the address of a
    ipr_demo_portal_client_t reference (a pointer to a pointer) and nullifies the
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
    ipr_demo_portal_client_destroy_ (
    ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_demo_portal_client_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_demo_portal_client_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_demo_portal_client", "E: missing link on ipr_demo_portal_client object");
        ipr_demo_portal_client_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_demo_portal_client_free_ ((ipr_demo_portal_client_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_demo_portal_client_t *
    ipr_demo_portal_client_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_demo_portal_client_t *
        self = NULL;                    //  Object reference

self = (ipr_demo_portal_client_t *) icl_mem_alloc_ (sizeof (ipr_demo_portal_client_t), file, line);
if (self)
    memset (self, 0, sizeof (ipr_demo_portal_client_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_free

    Type: Component method
    Freess a ipr_demo_portal_client_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_demo_portal_client_free_ (
    ipr_demo_portal_client_t * self,    //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (ipr_demo_portal_client_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_demo_portal_client_t));
        self->object_tag = IPR_DEMO_PORTAL_CLIENT_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_demo_portal_client_t *
    ipr_demo_portal_client_link_ (
    ipr_demo_portal_client_t * self,    //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    int
        history_last;
#endif

    if (self) {
        IPR_DEMO_PORTAL_CLIENT_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_unlink_ (
    ipr_demo_portal_client_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
    int
        history_last;
#endif

    ipr_demo_portal_client_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_DEMO_PORTAL_CLIENT_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_demo_portal_client", "E: missing link on ipr_demo_portal_client object");
            ipr_demo_portal_client_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DEMO_PORTAL_CLIENT)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_DEMO_PORTAL_CLIENT_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_demo_portal_client_annihilate_ (self_p, file, line);
        ipr_demo_portal_client_free_ ((ipr_demo_portal_client_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_demo_portal_client_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_demo_portal_client_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_demo_portal_client_new_in_scope_ (
    ipr_demo_portal_client_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_demo_portal_client_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_demo_portal_client_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_demo_portal_client_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_demo_portal_client_component  = "ipr_demo_portal_client ";
char *EMBED__ipr_demo_portal_client_version  = "1.1 ";
char *EMBED__ipr_demo_portal_client_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_demo_portal_client_filename  = "ipr_demo_portal_client.icl ";
char *EMBED__ipr_demo_portal_client_builddate  = "2011/03/01 ";
char *EMBED__ipr_demo_portal_client_version_end  = "VeRsIoNeNd:ipc";

