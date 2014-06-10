/*---------------------------------------------------------------------------
    ipr_meter.c - ipr_meter component

    The traffic rate meter tracks activity per time segment.
    Generated from ipr_meter.icl by icl_gen using GSL/4.
    
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
#include "ipr_meter.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_meter_t

//  Shorthands for class methods

#define self_count          ipr_meter_count
#define self_mark           ipr_meter_mark
#define self_selftest       ipr_meter_selftest
#define self_terminate      ipr_meter_terminate
#define self_new            ipr_meter_new
#define self_show           ipr_meter_show
#define self_destroy        ipr_meter_destroy
#define self_annihilate     ipr_meter_annihilate
#define self_alloc          ipr_meter_alloc
#define self_free           ipr_meter_free
#define self_cache_initialise  ipr_meter_cache_initialise
#define self_cache_purge    ipr_meter_cache_purge
#define self_cache_terminate  ipr_meter_cache_terminate
#define self_show_animation  ipr_meter_show_animation
#define self_new_in_scope   ipr_meter_new_in_scope

static void
    ipr_meter_annihilate (
ipr_meter_t * ( * self_p )              //  Reference to object reference
);

#define ipr_meter_alloc()               ipr_meter_alloc_ (__FILE__, __LINE__)
static ipr_meter_t *
    ipr_meter_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_meter_free (
ipr_meter_t * self                      //  Object reference
);

static void
    ipr_meter_cache_initialise (
void);

static void
    ipr_meter_cache_purge (
void);

static void
    ipr_meter_cache_terminate (
void);

Bool
    ipr_meter_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_meter_mark

    Type: Component method
    Accepts a ipr_meter_t reference and returns zero in case of success,
    1 in case of errors.
    Returns the current metered rate.  This is calculated by taking the
    difference between the current sample rate and the last measured
    rate and then calculating the average over the history.
    -------------------------------------------------------------------------
 */

int
    ipr_meter_mark (
    ipr_meter_t * self,                 //  Reference to object
    int interval                        //  Interval, in seconds
)
{
uint
    cur_index,
    sample_size;
qbyte
    sample_total;
    int
        rc = 0;                         //  Return code

IPR_METER_ASSERT_SANE (self);

assert (interval);
//
//  Unsigned arithmetic will always give us correct difference
self->current = (self->sample - self->last_sample) / interval;
self->last_sample = self->sample;

//  Ignore meter values of zero
if (self->current) {
    self->history [self->history_index++] = self->current;
    self->history_index = self->history_index % IPR_METER_HISTORY;

    //  Calculate rolling average
    sample_total = 0;
    sample_size  = 0;
    self->maximum = 0;
    for (cur_index = 0; cur_index < IPR_METER_HISTORY; cur_index++) {
        if (self->history [cur_index]) {
            sample_total += self->history [cur_index];
            sample_size++;
            //  Show maximum of last recorded history
            if (self->maximum < self->history [cur_index])
                self->maximum = self->history [cur_index];
        }
    }
    self->average = sample_total / sample_size;
}
rc = (int) self->current;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_meter_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_meter_selftest (
void)
{
ipr_meter_t
    *meter;

meter = ipr_meter_new ();

ipr_meter_count (meter);
ipr_meter_count (meter);
ipr_meter_count (meter);
ipr_meter_count (meter);
ipr_meter_count (meter);
ipr_meter_count (meter);

assert (ipr_meter_mark (meter, 1) == 6);
ipr_meter_destroy (&meter);
}
/*  -------------------------------------------------------------------------
    ipr_meter_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_meter_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_meter_new

    Type: Component method
    Creates and initialises a new ipr_meter_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_meter_t *
    ipr_meter_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_meter_t *
        self = NULL;                    //  Object reference

    self = ipr_meter_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_METER_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_meter_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_meter_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_meter_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_meter_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_meter file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_meter_destroy

    Type: Component method
    Destroys a ipr_meter_t object. Takes the address of a
    ipr_meter_t reference (a pointer to a pointer) and nullifies the
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
    ipr_meter_destroy_ (
    ipr_meter_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_meter_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_meter_annihilate (self_p);
    ipr_meter_free ((ipr_meter_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_meter_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_meter_annihilate (
    ipr_meter_t * ( * self_p )          //  Reference to object reference
)
{

    ipr_meter_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_METER_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    ipr_meter_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_meter_t *
    ipr_meter_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_meter_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_meter_cache_initialise ();

self = (ipr_meter_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_meter_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_meter_free

    Type: Component method
    Freess a ipr_meter_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_meter_free (
    ipr_meter_t * self                  //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_meter_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_meter_t));
        self->object_tag = IPR_METER_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_meter_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_meter_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_meter_t));
icl_system_register (ipr_meter_cache_purge, ipr_meter_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_meter_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_meter_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_meter_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_meter_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_meter_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_meter_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_meter_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_meter_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_meter_new_in_scope_ (
    ipr_meter_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_meter_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_meter_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_meter_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_meter_component          = "ipr_meter ";
char *EMBED__ipr_meter_version            = "1.0 ";
char *EMBED__ipr_meter_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_meter_filename           = "ipr_meter.icl ";
char *EMBED__ipr_meter_builddate          = "2011/03/01 ";
char *EMBED__ipr_meter_version_end        = "VeRsIoNeNd:ipc";

