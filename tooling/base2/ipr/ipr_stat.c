/*---------------------------------------------------------------------------
    ipr_stat.c - ipr_stat component

    The statistics module is designed to capture information (raw integers)
    and process these into useful information.  The sample size is arbitrarily
    large.  Samples are double-precision floating point.
    Generated from ipr_stat.icl by icl_gen using GSL/4.
    
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
#include "ipr_stat.h"                   //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_stat_t

//  Structure of the ipr_stat object

struct _ipr_stat_t {
dbyte
    object_tag;                     //  Object validity marker
double
    sum,                            //  Sum of all values
    sum2,                           //  Running deviation sum
    min,                            //  Minimum value
    max,                            //  Maximum value
    mean;                           //  Current mean
size_t
    count;                          //  Sample size
};

//  Shorthands for class methods

#define self_new            ipr_stat_new
#define self_record         ipr_stat_record
#define self_min            ipr_stat_min
#define self_max            ipr_stat_max
#define self_count          ipr_stat_count
#define self_sum            ipr_stat_sum
#define self_mean           ipr_stat_mean
#define self_var            ipr_stat_var
#define self_dev            ipr_stat_dev
#define self_selftest       ipr_stat_selftest
#define self_terminate      ipr_stat_terminate
#define self_show           ipr_stat_show
#define self_destroy        ipr_stat_destroy
#define self_annihilate     ipr_stat_annihilate
#define self_alloc          ipr_stat_alloc
#define self_free           ipr_stat_free
#define self_cache_initialise  ipr_stat_cache_initialise
#define self_cache_purge    ipr_stat_cache_purge
#define self_cache_terminate  ipr_stat_cache_terminate
#define self_show_animation  ipr_stat_show_animation
#define self_new_in_scope   ipr_stat_new_in_scope

static void
    ipr_stat_annihilate (
ipr_stat_t * ( * self_p )               //  Reference to object reference
);

#define ipr_stat_alloc()                ipr_stat_alloc_ (__FILE__, __LINE__)
static ipr_stat_t *
    ipr_stat_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_stat_free (
ipr_stat_t * self                       //  Object reference
);

static void
    ipr_stat_cache_initialise (
void);

static void
    ipr_stat_cache_purge (
void);

static void
    ipr_stat_cache_terminate (
void);

Bool
    ipr_stat_animating = TRUE;          //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_stat_new

    Type: Component method
    Creates and initialises a new ipr_stat_t object, returns a
    reference to the created object.
    Initialises a new statistics set.
    -------------------------------------------------------------------------
 */

ipr_stat_t *
    ipr_stat_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_stat_t *
        self = NULL;                    //  Object reference

    self = ipr_stat_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_STAT_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_stat_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_stat_record

    Type: Component method
    Accepts a ipr_stat_t reference and returns zero in case of success,
    1 in case of errors.
    Records a new sample.  The standard deviation is calculated using Knuth's
    algorithm, from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance:

        double mean = 0;
        double S = 0;
        long N;
        for N = 1 to data.length
            double delta = data[N] - mean;
            mean = mean + delta / N;
            S = S + delta * ( data[N] - mean );
        end for
        return S / ( N - 1 ) // the variance
    -------------------------------------------------------------------------
 */

int
    ipr_stat_record (
    ipr_stat_t * self,                  //  Reference to object
    double value                        //  Sample value
)
{
double
    delta;
    int
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);

//
self->sum += value;
self->min  = self->count? MIN (value, self->min): value;
self->max  = MAX (value, self->max);
self->count++;

delta = value - self->mean;
self->mean += delta / self->count;
self->sum2 += delta * (value - self->mean);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_min

    Type: Component method
    Returns the minimum sample value.  If the sample size is zero,
    returns zero.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_min (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = self->min;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_max

    Type: Component method
    Returns the maximum sample value.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_max (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = self->max;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_count

    Type: Component method
    Accepts a ipr_stat_t reference and returns zero in case of success,
    1 in case of errors.
    Returns the number of samples.
    -------------------------------------------------------------------------
 */

int
    ipr_stat_count (
    ipr_stat_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);

//
rc = self->count;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_sum

    Type: Component method
    Returns the sum of all samples.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_sum (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = self->sum;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_mean

    Type: Component method
    Returns the mean of the sample set.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_mean (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = self->mean;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_var

    Type: Component method
    Returns the variance of the sample set.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_var (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = self->sum2 / (self->count - 1);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_dev

    Type: Component method
    Returns the standard deviation of the sample set.
    -------------------------------------------------------------------------
 */

double
    ipr_stat_dev (
    ipr_stat_t * self                   //  Reference to object
)
{
    double
        rc = 0;                         //  Return code

IPR_STAT_ASSERT_SANE (self);
//
rc = sqrt (self->sum2 / (self->count - 1));

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_stat_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_stat_selftest (
void)
{
ipr_stat_t
    *stat;

stat = ipr_stat_new ();

ipr_stat_record (stat, 5);
ipr_stat_record (stat, 7);
ipr_stat_record (stat, 8);
ipr_stat_record (stat, 10);
ipr_stat_record (stat, 10);

assert (ipr_stat_count (stat) == 5);
assert (ipr_stat_sum   (stat) == 40);
assert (ipr_stat_min   (stat) == 5);
assert (ipr_stat_max   (stat) == 10);
assert (ipr_stat_mean  (stat) == 8);
assert (ipr_stat_var   (stat) == 4.5);
assert (ipr_stat_dev   (stat) > 2.12);
assert (ipr_stat_dev   (stat) < 2.13);

ipr_stat_destroy (&stat);
}
/*  -------------------------------------------------------------------------
    ipr_stat_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_stat_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_stat_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_stat_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_stat_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_stat file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_stat_destroy

    Type: Component method
    Destroys a ipr_stat_t object. Takes the address of a
    ipr_stat_t reference (a pointer to a pointer) and nullifies the
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
    ipr_stat_destroy_ (
    ipr_stat_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_stat_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_stat_annihilate (self_p);
    ipr_stat_free ((ipr_stat_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_stat_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_stat_annihilate (
    ipr_stat_t * ( * self_p )           //  Reference to object reference
)
{

    ipr_stat_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_STAT_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    ipr_stat_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_stat_t *
    ipr_stat_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_stat_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_stat_cache_initialise ();

self = (ipr_stat_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_stat_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_stat_free

    Type: Component method
    Freess a ipr_stat_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_stat_free (
    ipr_stat_t * self                   //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_stat_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_stat_t));
        self->object_tag = IPR_STAT_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_stat_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_stat_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_stat_t));
icl_system_register (ipr_stat_cache_purge, ipr_stat_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_stat_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_stat_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_stat_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_stat_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_stat_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_stat_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_stat_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_stat_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_stat_new_in_scope_ (
    ipr_stat_t * * self_p,              //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_stat_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_stat_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_stat_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_stat_component           = "ipr_stat ";
char *EMBED__ipr_stat_version             = "1.0 ";
char *EMBED__ipr_stat_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_stat_filename            = "ipr_stat.icl ";
char *EMBED__ipr_stat_builddate           = "2011/03/01 ";
char *EMBED__ipr_stat_version_end         = "VeRsIoNeNd:ipc";

