/*---------------------------------------------------------------------------
    ipr_bucket.c - ipr_bucket component

    Defines a bucket, being a buffer of variable size as chosen at creation
    time.  Buckets can be passed in messages and freed by the recipient.
    Buckets are cached so that heavy new/destroy use is very cheap.  We
    use reference counting so that buckets can be freely passed between
    objects.
    Generated from ipr_bucket.icl by icl_gen using GSL/4.
    
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
#include "ipr_bucket.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_bucket_t

//  Shorthands for class methods

#define self_new            ipr_bucket_new
#define self_annihilate     ipr_bucket_annihilate
#define self_alloc          ipr_bucket_alloc
#define self_free           ipr_bucket_free
#define self_cache_purge    ipr_bucket_cache_purge
#define self_empty          ipr_bucket_empty
#define self_fill           ipr_bucket_fill
#define self_fill_null      ipr_bucket_fill_null
#define self_fill_random    ipr_bucket_fill_random
#define self_fill_repeat    ipr_bucket_fill_repeat
#define self_cat            ipr_bucket_cat
#define self_load           ipr_bucket_load
#define self_save           ipr_bucket_save
#define self_find           ipr_bucket_find
#define self_dump           ipr_bucket_dump
#define self_initialise     ipr_bucket_initialise
#define self_terminate      ipr_bucket_terminate
#define self_increment_used  ipr_bucket_increment_used
#define self_decrement_used  ipr_bucket_decrement_used
#define self_used           ipr_bucket_used
#define self_selftest       ipr_bucket_selftest
#define self_show           ipr_bucket_show
#define self_destroy        ipr_bucket_destroy
#define self_link           ipr_bucket_link
#define self_unlink         ipr_bucket_unlink
#define self_show_animation  ipr_bucket_show_animation
#define self_new_in_scope   ipr_bucket_new_in_scope

#define ipr_bucket_annihilate(self)     ipr_bucket_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_bucket_annihilate_ (
ipr_bucket_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file for call
size_t line                             //  Line numberLine number for call
);

#define ipr_bucket_alloc(size)          ipr_bucket_alloc_ (__FILE__, __LINE__, size)
static ipr_bucket_t *
    ipr_bucket_alloc_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
size_t size                             //  Required length of data
);

#define ipr_bucket_free(self)           ipr_bucket_free_ (self, __FILE__, __LINE__)
static void
    ipr_bucket_free_ (
ipr_bucket_t * self,                    //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_bucket_initialise (
void);

static void
    ipr_bucket_terminate (
void);

Bool
    ipr_bucket_animating = TRUE;        //  Animation enabled by default
static Bool
    s_ipr_bucket_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_ipr_bucket_mutex       = NULL;
#endif

static const size_t
    bucket_size [] = {  0,  256,  512,  1024,  2048,  4096,  8192,  16384,  32768,  65536,  131072,  262144,  524388,  1048576,  2097152  };

#define S_NUM_BUCKET_SIZES 15

#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_class_mutex = NULL;
#endif

static icl_cache_t *
    s_ipr_bucket_cache [S_NUM_BUCKET_SIZES] = {  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL  };
volatile void *
    ipr_bucket_total_used = 0;
/*  -------------------------------------------------------------------------
    ipr_bucket_new

    Type: Component method
    Creates and initialises a new ipr_bucket_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_bucket_t *
    ipr_bucket_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    size_t size                         //  Required length of data
)
{
    ipr_bucket_t *
        self = NULL;                    //  Object reference

if (!s_ipr_bucket_active)
    self_initialise ();
    self = ipr_bucket_alloc_ (file, line, size);
    if (self) {
        self->object_tag   = IPR_BUCKET_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif

    self->mother   = NULL;
    self->cur_size = 0;
    //   self->max_size is set in the allocator.
    self->data     = (byte *) self + sizeof (ipr_bucket_t);
    self_increment_used (self->max_size);

#if defined (DEBUG)
    icl_mem_set_callback (self, ipr_bucket_show_);
#endif
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_annihilate_ (
    ipr_bucket_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file for call
    size_t line                         //  Line numberLine number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    int
        history_last;
#endif

    ipr_bucket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_BUCKET_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_BUCKET_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_BUCKET_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_BUCKET_HISTORY_LENGTH] = self->links;
#endif

    IPR_BUCKET_ASSERT_SANE (self);

//  Only decrement total memory allocated if data belongs to bucket.
if (self->data == (byte *) self + sizeof (ipr_bucket_t))
    self_decrement_used (self->max_size);
if (self->mother)
    ipr_mother_remove (self);

}
/*  -------------------------------------------------------------------------
    ipr_bucket_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_bucket_t *
    ipr_bucket_alloc_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    size_t size                         //  Required length of data
)
{

int
    chop_min = 0,
    chop_max = S_NUM_BUCKET_SIZES,
    chop_mid;
    ipr_bucket_t *
        self = NULL;                    //  Object reference


//  Use a binary chop to work out which cache to use.
while (chop_max > chop_min) {
    chop_mid = (chop_max + chop_min) / 2;
    if (size > bucket_size [chop_mid])
        chop_min = chop_mid + 1;
    else
        chop_max = chop_mid;
}
if (chop_min >= S_NUM_BUCKET_SIZES)
    chop_min = S_NUM_BUCKET_SIZES - 1;

self = icl_mem_cache_alloc_ (s_ipr_bucket_cache [chop_min], file, line);
self->max_size = bucket_size [chop_min];


    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_free

    Type: Component method
    Freess a ipr_bucket_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_free_ (
    ipr_bucket_t * self,                //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_BUCKET_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_BUCKET_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_BUCKET_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_BUCKET_HISTORY_LENGTH] = self->links;
#endif

self->object_tag = 0xDEAD;
icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_bucket_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_cache_purge (
void)
{
int
    cache_index;


for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
    icl_mem_cache_purge (s_ipr_bucket_cache [cache_index]);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_empty

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Empties the bucket.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_empty (
    ipr_bucket_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
self->cur_size = 0;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_fill

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Fill the bucket with data from some place: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_fill (
    ipr_bucket_t * self,                //  Reference to object
    void * data,                        //  Address of data to copy from
    size_t size                         //  Amount of data to copy
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

assert (size <= self->max_size);
memcpy (self->data, data, size);
self->cur_size = size;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_fill_null

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Fill the bucket with binary zero bytes up to the specified size. If
    the requested size is greater than the bucket size, we raise an assertion
    (this is considered a structural error).
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_fill_null (
    ipr_bucket_t * self,                //  Reference to object
    size_t size                         //  Amount of data to copy
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

assert (size <= self->max_size);
memset (self->data, 0, size);
self->cur_size = size;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_fill_random

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Fill the bucket with randomised data of a specified size.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_fill_random (
    ipr_bucket_t * self,                //  Reference to object
    size_t fill_size                    //  Amount of random data to insert
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (fill_size <= self->max_size);
randomize ();
self->cur_size = 0;
while (self->cur_size < fill_size)
    self->data [self->cur_size++] = randomof (256);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_fill_repeat

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Fill the bucket with repeated copies of the specified data, up to the
    specified size.  If the specified size is greater than the bucket size,
    or the source size is zero, we raise an assertion (these are considered
    structural errors).
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_fill_repeat (
    ipr_bucket_t * self,                //  Reference to object
    byte * data,                        //  Address of data to copy from
    size_t size,                        //  Size of source data
    size_t fill_size                    //  Amount of data to fill
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (size);
assert (fill_size <= self->max_size);
self->cur_size = 0;
while (self->cur_size < fill_size) {
    self->data [self->cur_size] = data [self->cur_size % size];
    self->cur_size++;
}
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_cat

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Append data to a bucket, updating its current size accordingly.
    If the size of the data to copy is greater than the bucket size,
    we raise an assertion (this is considered a structural error).
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_cat (
    ipr_bucket_t * self,                //  Reference to object
    void * source,                      //  Address of data to copy from
    size_t size                         //  Amount of data to copy
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (self->cur_size + size <= self->max_size);
memcpy (self->data + self->cur_size, source, size);
self->cur_size += size;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_load

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Fill the bucket with data from a file.  We fill as much data into
    the bucket as possible, either the size of data available in the
    file or the maximum size of the bucket, which ever is smaller.  To
    read a full file into buckets, call this routine repeatedly.  Note
    that the loaded data is concatenated into any existing data.  If
    you want to empty the bucket first, call the empty method.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_load (
    ipr_bucket_t * self,                //  Reference to object
    FILE * file                         //  Open file to read from
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
self->cur_size += fread (
    self->data + self->cur_size, 1, self->max_size - self->cur_size, file);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_save

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the data in the bucket to a file.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_save (
    ipr_bucket_t * self,                //  Reference to object
    FILE * file                         //  Open file to write to
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
if (fwrite (self->data, self->cur_size, 1, file) != 1)
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_find

    Type: Component method
    Searches the bucket for a pattern, using the Boyer-Moore-Horspool-Sunday
    algorithm.  Returns a pointer to the pattern if found within the bucket,
    or NULL if the pattern was not found.
    -------------------------------------------------------------------------
 */

byte *
    ipr_bucket_find (
    ipr_bucket_t * self,                //  Reference to bucket
    size_t offset,                      //  Start searching at this offset
    byte * pattern,                     //  Pattern to look for
    size_t patsize                      //  Size of pattern, in octets
)
{
size_t
    shift [256];                    //  Shift distance for each value
size_t
    byte_nbr,                       //  Distance through pattern
    match_size;                     //  Size of matched part
byte
    *data_start,                    //  Where to start looking
    *match_base = NULL,             //  Base of match of pattern
    *match_ptr = NULL,              //  Point within current match
    *limit = NULL;                  //  Last potiental match point
    byte *
        location;                       //  Location of pattern, or null

//
assert (pattern);

//  Pattern must be smaller or equal in size to string
location = NULL;
data_start = self->data + offset;
if (patsize == 0)              //  Empty patterns match at start
    location = data_start;
else
if (self->cur_size >= patsize) {
    //  The shift table determines how far to shift before trying to match
    //  again, if a match at this point fails.  If the byte after where the
    //  end of our pattern falls is not in our pattern, then we start to
    //  match again after that byte; otherwise we line up the last occurence
    //  of that byte in our pattern under that byte, and try match again.
    //
    for (byte_nbr = 0; byte_nbr < 256; byte_nbr++)
        shift [byte_nbr] = patsize + 1;
    for (byte_nbr = 0; byte_nbr < patsize; byte_nbr++)
        shift [(byte) pattern [byte_nbr]] = patsize - byte_nbr;

    //  Search for the block, each time jumping up by the amount
    //  computed in the shift table
    limit = data_start + (self->cur_size - patsize + 1);
    assert (limit > data_start);

    for (match_base = data_start; match_base < limit && !location; match_base += shift [*(match_base + patsize)]) {
        match_ptr  = match_base;
        match_size = 0;

        //  Compare pattern until it all matches, or we find a difference
        while (*match_ptr++ == pattern [match_size++]) {
            assert (match_size <= patsize && match_ptr == (match_base + match_size));
            //  If we found a match, return the start address
            if (match_size >= patsize) {
                location = match_base;
                break;
            }
        }
    }
}

    return (location);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_dump

    Type: Component method
    Accepts a ipr_bucket_t reference and returns zero in case of success,
    1 in case of errors.
    Prints the contents of a bucket to stderr.  Filters unprintable
    characters.  Does not apply any specific formatting.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_dump (
    ipr_bucket_t * self,                //  Reference to object
    char * prefix                       //  Prefix for each line
)
{
#   define BYTES_PER_LINE   (16)
    static const char
        *hex_digit = "0123456789abcdef";
    size_t
        size = self->cur_size,
        count,
        repeat_count = 0;
    byte
        *ucbuffer = self->data,
        cur_byte;
    char
        last_buffer [(BYTES_PER_LINE * 2) + 1],
        hex_buffer  [(BYTES_PER_LINE * 2) + 1],
        text_buffer [ BYTES_PER_LINE + 1],
        *hex,
        *text;
    int
        remainder;
    int
        rc = 0;                         //  Return code

IPR_BUCKET_ASSERT_SANE (self);
if (!self->zombie) {

//
memset (last_buffer, 0, sizeof (last_buffer));
while (size > 0) {
    hex       = hex_buffer;
    text      = text_buffer;
    count     = BYTES_PER_LINE;
    remainder = BYTES_PER_LINE - size;
    while (size > 0 && count > 0) {
        size--;
        count--;
        cur_byte = *ucbuffer;

        //  Is current byte a printable character?
        *text++ = (cur_byte < 32 || cur_byte > 127) ? '.' : cur_byte;
        *hex++  = hex_digit [cur_byte >> 4];
        *hex++  = hex_digit [cur_byte & 0x0f];
        ucbuffer++;
    }
    while (remainder > 0) {
        *hex++ = ' ';
        *hex++ = ' ';
        remainder--;
    }
    *hex = 0;
    *text = 0;
    if (memcmp (last_buffer, hex_buffer, sizeof (last_buffer)) == 0)
        repeat_count++;
    else {
        if (repeat_count) {
            icl_console_print ("%s[repeated %d times]", prefix, repeat_count);
            repeat_count = 0;
        }
        icl_console_print ("%s%s | %s", prefix, hex_buffer, text_buffer);
        memcpy (last_buffer, hex_buffer, sizeof (last_buffer));
    }
}
if (repeat_count)
    icl_console_print ("%s[repeated %d times]", prefix, repeat_count);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_initialise (
void)
{
int
    cache_index;

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_ipr_bucket_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_ipr_bucket_mutex)
            s_ipr_bucket_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_ipr_bucket_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_ipr_bucket_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

#if (defined (BASE_THREADSAFE))
    s_class_mutex = icl_mutex_new ();
#endif

    for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
        s_ipr_bucket_cache [cache_index] = icl_cache_get (bucket_size [cache_index] + sizeof (ipr_bucket_t));
            s_ipr_bucket_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_ipr_bucket_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    ipr_bucket_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_terminate (
void)
{

if (s_ipr_bucket_active) {

#if (defined (BASE_THREADSAFE))
    icl_mutex_destroy (&s_class_mutex);
#endif
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_ipr_bucket_mutex);
#endif
        s_ipr_bucket_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    ipr_bucket_used

    Type: Component method
    Return the total amount of memory allocated.
    -------------------------------------------------------------------------
 */

size_t
    ipr_bucket_used (
void)
{
    size_t
        used;                           //  The returned value

//  Do this in a loop to prevent race condition giving a messed-up value
//  On 32-bit architecture, could use atomic assignment, but not on 64-bit.
do
    used = (size_t) ipr_bucket_total_used;
while (used != (size_t) ipr_bucket_total_used);

    return (used);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_selftest (
void)
{
ipr_bucket_t
    *bucket;
byte
    *location;

    bucket = ipr_bucket_new (511);
    assert (bucket->max_size == 512);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (512);
    assert (bucket->max_size == 512);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (513);
    assert (bucket->max_size == 1024);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (32767);
    assert (bucket->max_size == 32768);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (32768);
    assert (bucket->max_size == 32768);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (32769);
    assert (bucket->max_size == 65536);
    memset (bucket->data, 0, bucket->max_size);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (2000);
    ipr_bucket_fill_random (bucket, 2000);
    ipr_bucket_destroy (&bucket);

    bucket = ipr_bucket_new (2000);
    ipr_bucket_fill_repeat (bucket, (byte *) "abc", 3, 10);
    assert (memcmp (bucket->data, "abcabcabca", 10) == 0);
    ipr_bucket_destroy (&bucket);

    //  Test find method
#   define TEST_DATA  "   This is a string, a string I say, a string!   "
    bucket = ipr_bucket_new (32000);
    ipr_bucket_cat (bucket, TEST_DATA, strlen (TEST_DATA));
    location = ipr_bucket_find (bucket, 0, (byte *) "string", 6);
    assert (location - bucket->data == 13);
    ipr_bucket_destroy (&bucket);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_bucket_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_bucket zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    if (self->history_last > IPR_BUCKET_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_BUCKET_HISTORY_LENGTH;
        self->history_last %= IPR_BUCKET_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_BUCKET_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_bucket>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_bucket_destroy

    Type: Component method
    Destroys a ipr_bucket_t object. Takes the address of a
    ipr_bucket_t reference (a pointer to a pointer) and nullifies the
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
    ipr_bucket_destroy_ (
    ipr_bucket_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_bucket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_bucket_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_bucket", "E: missing link on ipr_bucket object");
        ipr_bucket_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_bucket_free_ ((ipr_bucket_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_bucket_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_bucket_t *
    ipr_bucket_link_ (
    ipr_bucket_t * self,                //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    int
        history_last;
#endif

    if (self) {
        IPR_BUCKET_ASSERT_SANE (self);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_BUCKET_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_BUCKET_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_BUCKET_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_BUCKET_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_unlink_ (
    ipr_bucket_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    int
        history_last;
#endif

    ipr_bucket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_BUCKET_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_bucket", "E: missing link on ipr_bucket object");
            ipr_bucket_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_BUCKET_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_BUCKET_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_BUCKET_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_BUCKET_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_bucket_annihilate_ (self_p, file, line);
        ipr_bucket_free_ ((ipr_bucket_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_bucket_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_bucket_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_bucket_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_new_in_scope_ (
    ipr_bucket_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    size_t size                         //  Required length of data
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_bucket_new_ (file,line,size);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_bucket_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_bucket_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_bucket_component         = "ipr_bucket ";
char *EMBED__ipr_bucket_version           = "2.0 ";
char *EMBED__ipr_bucket_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_bucket_filename          = "ipr_bucket.icl ";
char *EMBED__ipr_bucket_builddate         = "2011/03/01 ";
char *EMBED__ipr_bucket_version_end       = "VeRsIoNeNd:ipc";

