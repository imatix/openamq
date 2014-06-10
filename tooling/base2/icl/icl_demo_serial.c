/*---------------------------------------------------------------------------
    icl_demo_serial.c - icl_demo_serial component

    Generated from icl_demo_serial.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_serial.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_serial_t

//  Shorthands for class methods

#define self_selftest       icl_demo_serial_selftest
#define self_terminate      icl_demo_serial_terminate
#define self_new            icl_demo_serial_new
#define self_show           icl_demo_serial_show
#define self_destroy        icl_demo_serial_destroy
#define self_annihilate     icl_demo_serial_annihilate
#define self_alloc          icl_demo_serial_alloc
#define self_free           icl_demo_serial_free
#define self_read_lock      icl_demo_serial_read_lock
#define self_write_lock     icl_demo_serial_write_lock
#define self_unlock         icl_demo_serial_unlock
#define self_cache_initialise  icl_demo_serial_cache_initialise
#define self_cache_purge    icl_demo_serial_cache_purge
#define self_cache_terminate  icl_demo_serial_cache_terminate
#define self_show_animation  icl_demo_serial_show_animation
#define self_save           icl_demo_serial_save
#define self_load           icl_demo_serial_load
#define self_get_size       icl_demo_serial_get_size
#define self_new_in_scope   icl_demo_serial_new_in_scope

static void
    icl_demo_serial_annihilate (
icl_demo_serial_t * ( * self_p )        //  Reference to object reference
);

#define icl_demo_serial_alloc()         icl_demo_serial_alloc_ (__FILE__, __LINE__)
static icl_demo_serial_t *
    icl_demo_serial_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_serial_free (
icl_demo_serial_t * self                //  Object reference
);

static void
    icl_demo_serial_cache_initialise (
void);

static void
    icl_demo_serial_cache_purge (
void);

static void
    icl_demo_serial_cache_terminate (
void);

Bool
    icl_demo_serial_animating = TRUE;   //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


#define ICL_DEMO_SERIAL_SIZE 14 + strlen (self->item_dest_name) + strlen (self->item_mime_type) + strlen (self->item_encoding) + (self->item_options? self->item_options->cur_size: 0)
/*  -------------------------------------------------------------------------
    icl_demo_serial_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_serial_selftest (
void)
{
icl_demo_serial_t
    *self;

self = icl_demo_serial_new ();
self->size = icl_demo_serial_save (self, self->data, sizeof (self->data));
assert (self->size);
icl_demo_serial_load (self, self->data, self->size);
icl_demo_serial_destroy (&self);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_serial_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_serial_new

    Type: Component method
    Creates and initialises a new icl_demo_serial_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_serial_t *
    icl_demo_serial_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_demo_serial_t *
        self = NULL;                    //  Object reference

    self = icl_demo_serial_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_SERIAL_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_serial_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_serial_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_serial_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_serial file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_serial_destroy

    Type: Component method
    Destroys a icl_demo_serial_t object. Takes the address of a
    icl_demo_serial_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_serial_destroy_ (
    icl_demo_serial_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_serial_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_serial_annihilate (self_p);
    icl_demo_serial_free ((icl_demo_serial_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_serial_annihilate (
    icl_demo_serial_t * ( * self_p )    //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    icl_demo_serial_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    ICL_DEMO_SERIAL_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_longstr_destroy (&self->item_options);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_serial_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_serial_t *
    icl_demo_serial_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_serial_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    icl_demo_serial_cache_initialise ();

self = (icl_demo_serial_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_serial_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_free

    Type: Component method
    Freess a icl_demo_serial_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_serial_free (
    icl_demo_serial_t * self            //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (icl_demo_serial_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_serial_t));
        self->object_tag = ICL_DEMO_SERIAL_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_read_lock

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_serial_read_lock (
    icl_demo_serial_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_SERIAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_write_lock

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_serial_write_lock (
    icl_demo_serial_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_SERIAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_unlock

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_serial_unlock (
    icl_demo_serial_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_SERIAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_serial_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (icl_demo_serial_t));
icl_system_register (icl_demo_serial_cache_purge, icl_demo_serial_cache_terminate);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_serial_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    icl_demo_serial_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_serial_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    icl_demo_serial_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_serial_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_serial_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_save

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the serialised fields to the specified buffer.  Returns amount of
    space used in buffer, in octets.  If the buffer is too small, returns 0.
    Integer data is always saved in network portable format.
    -------------------------------------------------------------------------
 */

size_t
    icl_demo_serial_save (
    icl_demo_serial_t * self,           //  Reference to object
    byte * data,                        //  Buffer to save into
    size_t size                         //  Buffer size
)
{
byte
    *dest;
size_t
    string_size;
byte
    bit_string;
    size_t
        rc = 0;                         //  Return codeReturned size

ICL_DEMO_SERIAL_ASSERT_SANE (self);

assert (data);

if (ICL_DEMO_SERIAL_SIZE <= size) {
    dest = data;
    *(dbyte *) dest = htons (self->item_channel_id);
    dest += 2;
    *(dbyte *) dest = htons (self->item_handle_id);
    dest += 2;
    *(dbyte *) dest = htons (self->item_service_type);
    dest += 2;
    *(dbyte *) dest = htons (self->item_confirm_tag);
    dest += 2;
    dest += 1;
    bit_string = 0;
    bit_string |= (self->item_producer & 1) << 0;
    *(byte *) (dest - 1) = bit_string;
    bit_string |= (self->item_consumer & 1) << 1;
    *(byte *) (dest - 1) = bit_string;
    bit_string |= (self->item_browser & 1) << 2;
    *(byte *) (dest - 1) = bit_string;
    bit_string |= (self->item_temporary & 1) << 3;
    *(byte *) (dest - 1) = bit_string;
    string_size = strlen (self->item_dest_name);
    *(byte *) dest = (byte) string_size;
    dest += 1;
    memcpy (dest, self->item_dest_name, string_size);
    dest += string_size;
    string_size = strlen (self->item_mime_type);
    *(byte *) dest = (byte) string_size;
    dest += 1;
    memcpy (dest, self->item_mime_type, string_size);
    dest += string_size;
    string_size = strlen (self->item_encoding);
    *(byte *) dest = (byte) string_size;
    dest += 1;
    memcpy (dest, self->item_encoding, string_size);
    dest += string_size;
    if (self->item_options) {
        assert (self->item_options->cur_size < 0x10000);
        *(dbyte *) dest = htons ((dbyte) self->item_options->cur_size);
        dest += 2;
        memcpy (dest, self->item_options->data, self->item_options->cur_size);
        dest += self->item_options->cur_size;
    }
    else {
        *(dbyte *) dest = 0;
        dest += 2;
    }
    rc = dest - data;               //  Number of octets processed         
}
else {
    icl_console_print ("icl_demo_serial E: object needs %ld bytes, buffer too small at %ld",
        ICL_DEMO_SERIAL_SIZE, size);
    rc = 0;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_load

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    Loads the serialised fields from the supplied buffer.  Returns number of
    octets loaded; zero means the buffer was empty, or invalid.
    -------------------------------------------------------------------------
 */

size_t
    icl_demo_serial_load (
    icl_demo_serial_t * self,           //  Reference to object
    byte * data,                        //  Buffer to load from
    size_t size                         //  Buffer size
)
{
byte
    *src,                           //  Pointer into buffer                
    *max;                           //  Limit of buffer validity           
size_t
    string_size;
Bool
    underflow = FALSE;
byte
    bit_string = 0;
    size_t
        rc = 0;                         //  Return codeReturned size

ICL_DEMO_SERIAL_ASSERT_SANE (self);

assert (data);
src = data;
max = data + size;
if (src < max) {
    self->item_channel_id = ntohs (*(dbyte *) src);
    src += 2;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_handle_id = ntohs (*(dbyte *) src);
    src += 2;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_service_type = ntohs (*(dbyte *) src);
    src += 2;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_confirm_tag = ntohs (*(dbyte *) src);
    src += 2;
}
else
    underflow = TRUE;
if (src < max) {
    bit_string = *(byte *) src;
    src++;
    self->item_producer = (bit_string >> 0) & 1;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_consumer = (bit_string >> 1) & 1;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_browser = (bit_string >> 2) & 1;
}
else
    underflow = TRUE;
if (src < max) {
    self->item_temporary = (bit_string >> 3) & 1;
}
else
    underflow = TRUE;
if (src < max) {
    string_size = *(byte *) src;
    src += 1;
    memcpy (self->item_dest_name, src, string_size);
    self->item_dest_name [string_size] = 0;
    src += string_size;
}
else
    underflow = TRUE;
if (src < max) {
    string_size = *(byte *) src;
    src += 1;
    memcpy (self->item_mime_type, src, string_size);
    self->item_mime_type [string_size] = 0;
    src += string_size;
}
else
    underflow = TRUE;
if (src < max) {
    string_size = *(byte *) src;
    src += 1;
    memcpy (self->item_encoding, src, string_size);
    self->item_encoding [string_size] = 0;
    src += string_size;
}
else
    underflow = TRUE;
if (src < max) {
    string_size = ntohs (*(dbyte *) src);
    src += 2;
    icl_longstr_destroy (&self->item_options);
    self->item_options = icl_longstr_new (src, string_size);
    src += string_size;
}
else
    underflow = TRUE;
if (underflow) {
    icl_console_print ("icl_demo_serial E: Insufficient buffer data provided to decode");
    rc = 0;
}
else
    rc = src - data;                //  Number of octets processed


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_get_size

    Type: Component method
    Accepts a icl_demo_serial_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

size_t
    icl_demo_serial_get_size (
    icl_demo_serial_t * self            //  Reference to object
)
{
    size_t
        rc = 0;                         //  Return code

ICL_DEMO_SERIAL_ASSERT_SANE (self);

rc = ICL_DEMO_SERIAL_SIZE;


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_serial_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_serial_new_in_scope_ (
    icl_demo_serial_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_serial_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_serial_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_serial_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_serial_component    = "icl_demo_serial ";
char *EMBED__icl_demo_serial_version      = "1.0 ";
char *EMBED__icl_demo_serial_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_serial_filename     = "icl_demo_serial.icl ";
char *EMBED__icl_demo_serial_builddate    = "2011/03/01 ";
char *EMBED__icl_demo_serial_version_end  = "VeRsIoNeNd:ipc";

