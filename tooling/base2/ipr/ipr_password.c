/*---------------------------------------------------------------------------
    ipr_password.c - ipr_password component

    Password tables are held as dictionaries.  Supports passwords in
    either basic or digest format as defined by RFC 2617. ipr_password
    items do not use links.  This class is threadsafe and can be used
    from concurrent threads.
    Generated from ipr_password.icl by icl_gen using GSL/4.
    
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
#include "ipr_password.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_password_t

//  Shorthands for class methods

#define self_new            ipr_password_new
#define self_annihilate     ipr_password_annihilate
#define self_update         ipr_password_update
#define self_assume         ipr_password_assume
#define self_guard_set      ipr_password_guard_set
#define self_guard_get      ipr_password_guard_get
#define self_guard_put      ipr_password_guard_put
#define self_hit            ipr_password_hit
#define self_miss           ipr_password_miss
#define self_selftest       ipr_password_selftest
#define self_remove_from_all_containers  ipr_password_remove_from_all_containers
#define self_show           ipr_password_show
#define self_terminate      ipr_password_terminate
#define self_destroy        ipr_password_destroy
#define self_alloc          ipr_password_alloc
#define self_free           ipr_password_free
#define self_lock           ipr_password_lock
#define self_unlock         ipr_password_unlock
#define self_cache_initialise  ipr_password_cache_initialise
#define self_cache_purge    ipr_password_cache_purge
#define self_cache_terminate  ipr_password_cache_terminate
#define self_show_animation  ipr_password_show_animation
#define self_new_in_scope   ipr_password_new_in_scope

static void
    ipr_password_annihilate (
ipr_password_t * ( * self_p )           //  Reference to object reference
);

#define ipr_password_alloc()            ipr_password_alloc_ (__FILE__, __LINE__)
static ipr_password_t *
    ipr_password_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_password_free (
ipr_password_t * self                   //  Object reference
);

static void
    ipr_password_cache_initialise (
void);

static void
    ipr_password_cache_purge (
void);

static void
    ipr_password_cache_terminate (
void);

Bool
    ipr_password_animating = TRUE;      //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_password_new

    Type: Component method
    Creates and initialises a new ipr_password_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_password_t *
    ipr_password_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_password_table_t * table,       //  Table to insert into
    char * key,                         //  Hash key
    char * hash                         //  Not documented
)
{
    ipr_password_t *
        self = NULL;                    //  Object reference

    self = ipr_password_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_PASSWORD_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->mutex = icl_mutex_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_password_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
//
self->hash = icl_mem_strdup (hash);

//  Basic password keys are user
//  Digest password keys are realm:user
if (strchr (key, ':'))
    self->type = IPR_PASSWD_DIGEST;
else
    self->type = IPR_PASSWD_BASIC;
if (table && self && ipr_password_table_insert (table, key, self))
    ipr_password_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_password_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_password_annihilate (
    ipr_password_t * ( * self_p )       //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    ipr_password_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    IPR_PASSWORD_ASSERT_SANE (self);
    ipr_password_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif

icl_mem_free (self->hash);
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_password_update

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Forces an update/clean of the password, removing any guard it had.
    -------------------------------------------------------------------------
 */

int
    ipr_password_update (
    ipr_password_t * self,              //  Reference to object
    char * hash                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
icl_mem_free (self->hash);
self->hash = icl_mem_strdup (hash);

//  The entry is unguarded until it is re-guarded
self->hits          = 0;
self->hit_quota     = 0;
self->misses        = 0;
self->miss_quota    = 0;
self->expiry        = 0;
self->hit_time      = 0;
self->miss_time     = 0;
self->hit_from  [0] = 0;
self->miss_from [0] = 0;
self->expired   = FALSE;
self->table_head->dirty = TRUE;

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_assume

    Type: Component method
    Create or update entry with specified hash.  Returns reference to entry.
    -------------------------------------------------------------------------
 */

ipr_password_t *
    ipr_password_assume (
    ipr_password_table_t * table,       //  Not documented
    char * key,                         //  Not documented
    char * hash                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    ipr_password_t *
        self = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

//
self = ipr_password_table_search (table, key);
if (self) {
    if (hash && strneq (hash, self->hash))
        ipr_password_update (self, hash);
}
else
    self = ipr_password_new (table, key, hash);
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_password_guard_set

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the guard data for hits, misses, and expiry.  Does not destroy data
    already collected.  Does not change the guard data for expired passwords.
    -------------------------------------------------------------------------
 */

int
    ipr_password_guard_set (
    ipr_password_t * self,              //  Reference to object
    size_t hit_quota,                   //  Not documented
    size_t miss_quota,                  //  Not documented
    size_t time_to_live                 //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
if (!self->expired) {
    if (self->hit_quota == 0)
        self->hit_quota = hit_quota;
    if (self->miss_quota == 0)
        self->miss_quota = miss_quota;
    if (self->expiry == 0 && time_to_live)
        self->expiry = apr_time_now () + (time_to_live * 1000 * 1000);
}

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_guard_get

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the password options from a serialized string, the same format
    as produced by the save method.
    -------------------------------------------------------------------------
 */

int
    ipr_password_guard_get (
    ipr_password_t * self,              //  Reference to object
    char * options                      //  name=value ...
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

ipr_regexp_t
    *regexp;                        //  Regexp for matching each line
char
    *name = NULL,
    *value = NULL,
    *rest =  NULL;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
options = icl_mem_strdup (options);
regexp = ipr_regexp_new ("^`s*([`w-]+)`s*=`s*([^`s]+)(.*)");
while (ipr_regexp_match (regexp, options, &name, &value, &rest) == 3) {
    if (streq (name, "expiry"))
        self->expiry = ipr_time_iso8601_decode (value);
    else
    if (streq (name, "expired"))
        self->expired = atoi (value);
    else
    if (streq (name, "hits"))
        self->hits = atoi (value);
    else
    if (streq (name, "hit-quota"))
        self->hit_quota = atoi (value);
    else
    if (streq (name, "hit-time"))
        self->hit_time = ipr_time_iso8601_decode (value);
    else
    if (streq (name, "hit-from"))
        icl_shortstr_cpy (self->hit_from, value);
    else
    if (streq (name, "misses"))
        self->misses = atoi (value);
    else
    if (streq (name, "miss-quota"))
        self->miss_quota = atoi (value);
    else
    if (streq (name, "miss-time"))
        self->miss_time = ipr_time_iso8601_decode (value);
    else
    if (streq (name, "miss-from"))
        icl_shortstr_cpy (self->miss_from, value);

    icl_mem_strfree (&name);
    icl_mem_strfree (&value);
    icl_mem_strfree (&options);
    options = rest;
    rest = NULL;
    self->table_head->dirty = TRUE;
}
ipr_regexp_destroy (&regexp);
icl_mem_free (options);

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_guard_put

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Writes the password to a text file.  The password is serialised in this
    format: '{key}:{proplist}' where proplist is a pair=value list.
    -------------------------------------------------------------------------
 */

int
    ipr_password_guard_put (
    ipr_password_t * self,              //  Reference to object
    FILE * file                         //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

icl_shortstr_t
    time_str;
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
assert (file);
fprintf (file, "%s:", self->key);
if (self->expiry) {
    ipr_time_iso8601 (self->expiry, ipr_date_format_minute, 0, TRUE, time_str);
    fprintf (file, " expiry=%s", time_str);
}
if (self->expired)
    fprintf (file, " expired=1");
if (self->hits)
    fprintf (file, " hits=%lu", (unsigned long) self->hits);
if (self->hit_quota)
    fprintf (file, " hit-quota=%lu", (unsigned long) self->hit_quota);
if (*self->hit_from)
    fprintf (file, " hit-from=%s", self->hit_from);
if (self->hit_time) {
    ipr_time_iso8601 (self->hit_time, ipr_date_format_minute, 0, TRUE, time_str);
    fprintf (file, " hit-time=%s", time_str);
}
if (self->misses)
    fprintf (file, " misses=%lu", (unsigned long) self->misses);
if (self->miss_quota)
    fprintf (file, " miss-quota=%lu", (unsigned long) self->miss_quota);
if (*self->miss_from)
    fprintf (file, " miss-from=%s", self->miss_from);
if (self->miss_time) {
    ipr_time_iso8601 (self->miss_time, ipr_date_format_minute, 0, TRUE, time_str);
    fprintf (file, " miss-time=%s", time_str);
}
fprintf (file, "\n");

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_hit

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Updates the password for a successful hit and expires the password if
    it ran out of hit quotas, or passed its expiry time.  We do not reset
    the miss quota even after a successful hit, since that would mask any
    ongoing slow dictionary attack.
    -------------------------------------------------------------------------
 */

int
    ipr_password_hit (
    ipr_password_t * self,              //  Reference to object
    char * address                      //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
icl_shortstr_cpy (self->hit_from, address);
self->hit_time = apr_time_now ();
self->hits++;

//  Expire password if the hit quota went down to 1
//  Ignore the hit quota and expiry if they are already zero
if ((self->hit_quota && --self->hit_quota == 0)
||  (self->expiry && apr_time_now () > self->expiry))
    self->expired = TRUE;

self->table_head->dirty = TRUE;

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_miss

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    Updates the password for a failed request and expires the password if
    it ran out of miss quotas, or passed its expiry time.
    -------------------------------------------------------------------------
 */

int
    ipr_password_miss (
    ipr_password_t * self,              //  Reference to object
    char * address                      //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

IPR_PASSWORD_ASSERT_SANE (self);

//
icl_shortstr_cpy (self->miss_from, address);
self->miss_time = apr_time_now ();
self->misses++;

//  Expire password if the miss quota went down to 1
//  Ignore the miss quota and expiry if they are already zero
if ((self->miss_quota && --self->miss_quota == 0)
||  (self->expiry && apr_time_now () > self->expiry))
    self->expired = TRUE;

self->table_head->dirty = TRUE;

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_password_selftest (
void)
{

//  selftest is in ipr_password_table
}
/*  -------------------------------------------------------------------------
    ipr_password_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_password_remove_from_all_containers (
    ipr_password_t * self               //  The itemThe item
)
{

IPR_PASSWORD_ASSERT_SANE (self);
ipr_password_table_remove (self);
ipr_password_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_password_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_password_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_password_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
if (self->list_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_password file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_password_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_password_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_password_destroy

    Type: Component method
    Destroys a ipr_password_t object. Takes the address of a
    ipr_password_t reference (a pointer to a pointer) and nullifies the
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
    ipr_password_destroy_ (
    ipr_password_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_password_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_password_annihilate (self_p);
    ipr_password_free ((ipr_password_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_password_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_password_t *
    ipr_password_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_password_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_password_cache_initialise ();

self = (ipr_password_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_password_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_password_free

    Type: Component method
    Freess a ipr_password_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_password_free (
    ipr_password_t * self               //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_password_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_password_t));
        self->object_tag = IPR_PASSWORD_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_password_lock

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_password_lock (
    ipr_password_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_PASSWORD_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (self->mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_unlock

    Type: Component method
    Accepts a ipr_password_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_password_unlock (
    ipr_password_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_PASSWORD_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (self->mutex);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_password_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_password_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_password_t));
icl_system_register (ipr_password_cache_purge, ipr_password_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_password_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_password_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_password_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_password_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_password_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_password_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_password_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_password_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_password_new_in_scope_ (
    ipr_password_t * * self_p,          //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_password_table_t * table,       //  Table to insert into
    char * key,                         //  Hash key
    char * hash                         //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_password_new_ (file,line,table,key,hash);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_password_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_password_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_password_component       = "ipr_password ";
char *EMBED__ipr_password_version         = "1.0 ";
char *EMBED__ipr_password_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_password_filename        = "ipr_password.icl ";
char *EMBED__ipr_password_builddate       = "2011/03/01 ";
char *EMBED__ipr_password_version_end     = "VeRsIoNeNd:ipc";

