/*---------------------------------------------------------------------------
    ipr_nonce.c - ipr_nonce component

Nonces are held in a hash table.  The nonce value is the key into the
table.  This class generates the nonce value.
    Generated from ipr_nonce.icl by icl_gen using GSL/4.
    
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
#include "ipr_nonce.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_nonce_t

//  Shorthands for class methods

#define self_new            ipr_nonce_new
#define self_assume         ipr_nonce_assume
#define self_expired        ipr_nonce_expired
#define self_annihilate     ipr_nonce_annihilate
#define self_selftest       ipr_nonce_selftest
#define self_remove_from_all_containers  ipr_nonce_remove_from_all_containers
#define self_show           ipr_nonce_show
#define self_terminate      ipr_nonce_terminate
#define self_destroy        ipr_nonce_destroy
#define self_alloc          ipr_nonce_alloc
#define self_free           ipr_nonce_free
#define self_cache_initialise  ipr_nonce_cache_initialise
#define self_cache_purge    ipr_nonce_cache_purge
#define self_cache_terminate  ipr_nonce_cache_terminate
#define self_show_animation  ipr_nonce_show_animation
#define self_new_in_scope   ipr_nonce_new_in_scope

static void
    ipr_nonce_annihilate (
ipr_nonce_t * ( * self_p )              //  Reference to object reference
);

#define ipr_nonce_alloc()               ipr_nonce_alloc_ (__FILE__, __LINE__)
static ipr_nonce_t *
    ipr_nonce_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_nonce_free (
ipr_nonce_t * self                      //  Object reference
);

static void
    ipr_nonce_cache_initialise (
void);

static void
    ipr_nonce_cache_purge (
void);

static void
    ipr_nonce_cache_terminate (
void);

Bool
    ipr_nonce_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_nonce_new

    Type: Component method
    Creates and initialises a new ipr_nonce_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

ipr_nonce_t *
    ipr_nonce_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_nonce_table_t * table,          //  Table to insert into
    char * key,                         //  Hash keyNonce value or NULL
    size_t ttl,                         //  Time to live, seconds
    size_t quota                        //  Lifespan, in hits
)
{
icl_shortstr_t
    nonce_data;
char
    *digest;
    ipr_nonce_t *
        self = NULL;                    //  Object reference

    self = ipr_nonce_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_NONCE_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_nonce_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
//
if (!key) {
    //  nonce is "time:{md5(time:random)}"
    //  This is overkill, a simpler nonce would work as well
    sprintf (nonce_data, "%016llx:", (long long unsigned int) apr_time_now ());
    ipr_str_random (nonce_data + strlen (nonce_data), "Ax16");
    digest = ipr_str_md5 (nonce_data);
    icl_shortstr_fmt (nonce_data + 16, digest);
    icl_mem_free (digest);
    key = nonce_data;
}
self->quota = quota;
self->expiry = apr_time_now () + (ttl * 1000 * 1000);
if (table && self && ipr_nonce_table_insert (table, key, self))
    ipr_nonce_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_assume

    Type: Component method
    Create or update nonce with specified key
    -------------------------------------------------------------------------
 */

ipr_nonce_t *
    ipr_nonce_assume (
    ipr_nonce_table_t * table,          //  Nonce table
    char * key,                         //  Nonce value
    size_t ttl,                         //  Time to live, seconds
    size_t quota                        //  Lifespan, in hits
)
{
ipr_nonce_t
    *nonce;
    ipr_nonce_t *
        self;                           //  Not documented

//
nonce = ipr_nonce_table_search (table, key);
if (!nonce)
    nonce = ipr_nonce_new (table, key, ttl, quota);
return (nonce);

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_expired

    Type: Component method
    Accepts a ipr_nonce_t reference and returns zero in case of success,
    1 in case of errors.
    Tries to consume one request quota from the nonce, and checks the current
    time. If the nonce did not have sufficient quota, or TTL was expired,
    returns TRUE, else returns FALSE.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_expired (
    ipr_nonce_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_NONCE_ASSERT_SANE (self);

//
rc = FALSE;
if (self->quota) {
    self->quota--;
    if (apr_time_now () > self->expiry)
        rc = TRUE;
}
else 
    rc = TRUE;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_annihilate (
    ipr_nonce_t * ( * self_p )          //  Reference to object reference
)
{

    ipr_nonce_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_NONCE_ASSERT_SANE (self);
ipr_nonce_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    ipr_nonce_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_selftest (
void)
{

ipr_nonce_table_t
    *table;
ipr_nonce_t
    *nonce;

table = ipr_nonce_table_new ();
nonce = ipr_nonce_new (table, NULL, 60, 10);
assert (strlen (nonce->key) == 48);
ipr_nonce_table_destroy (&table);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_remove_from_all_containers (
    ipr_nonce_t * self                  //  The item
)
{

IPR_NONCE_ASSERT_SANE (self);
ipr_nonce_table_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_nonce_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_nonce file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_nonce_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_nonce_destroy

    Type: Component method
    Destroys a ipr_nonce_t object. Takes the address of a
    ipr_nonce_t reference (a pointer to a pointer) and nullifies the
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
    ipr_nonce_destroy_ (
    ipr_nonce_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_nonce_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_nonce_annihilate (self_p);
    ipr_nonce_free ((ipr_nonce_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_nonce_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_nonce_t *
    ipr_nonce_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_nonce_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_nonce_cache_initialise ();

self = (ipr_nonce_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_nonce_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_free

    Type: Component method
    Freess a ipr_nonce_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_free (
    ipr_nonce_t * self                  //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_nonce_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_nonce_t));
        self->object_tag = IPR_NONCE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_nonce_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_nonce_t));
icl_system_register (ipr_nonce_cache_purge, ipr_nonce_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_nonce_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_nonce_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_nonce_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_nonce_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_new_in_scope_ (
    ipr_nonce_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_nonce_table_t * table,          //  Table to insert into
    char * key,                         //  Hash keyNonce value or NULL
    size_t ttl,                         //  Time to live, seconds
    size_t quota                        //  Lifespan, in hits
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_nonce_new_ (file,line,table,key,ttl,quota);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_nonce_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_nonce_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_nonce_component          = "ipr_nonce ";
char *EMBED__ipr_nonce_version            = "1.0 ";
char *EMBED__ipr_nonce_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_nonce_filename           = "ipr_nonce.icl ";
char *EMBED__ipr_nonce_builddate          = "2011/03/01 ";
char *EMBED__ipr_nonce_version_end        = "VeRsIoNeNd:ipc";

