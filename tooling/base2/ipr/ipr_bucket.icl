<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name      = "ipr_bucket"
    comment   = "Bucket buffer class"
    version   = "2.0"
    script    = "icl_gen"
    >
<doc>
    Defines a bucket, being a buffer of variable size as chosen at creation
    time.  Buckets can be passed in messages and freed by the recipient.
    Buckets are cached so that heavy new/destroy use is very cheap.  We
    use reference counting so that buckets can be freely passed between
    objects.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"   value = "manual"/>
    <option name = "links"   value = "1"/>
    <option name = "nullify" value = "0"/>
</inherit>
<inherit class = "icl_init" />

<import class = "ipr_mother" />
<import class = "ipr_bucket_list" />

<data>
    <size value = "0"       />
    <size value = "256"     />
    <size value = "512"     />
    <size value = "1024"    />
    <size value = "2048"    />
    <size value = "4096"    />
    <size value = "8192"    />
    <size value = "16384"   />
    <size value = "32768"   />
    <size value = "65536"   />
    <size value = "131072"  />
    <size value = "262144"  />
    <size value = "524388"  />
    <size value = "1048576" />
    <size value = "2097152" />
</data>

<public>
#include "base_apr.h"

#define IPR_BUCKET_MAX_SIZE  2097152
</public>

<private name = "header">
static const size_t
    bucket_size [] = {\
.for class->data.size
                       $(value)$(!last()??','?)\
.endfor
                     };

#define S_NUM_BUCKET_SIZES $(count (class->data.size))

#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_class_mutex = NULL;
#endif

static icl_cache_t *
    s_$(selfname)_cache [S_NUM_BUCKET_SIZES] = {\
.for class->data.size
                       NULL$(!last()??','?)\
.endfor
                       };
</private>

<public name = "header">
extern volatile void *
    ipr_bucket_total_used;
</public>

<private name = "header">
volatile void *
    ipr_bucket_total_used = 0;
</private>

<context>
    ipr_mother_t
        *mother;
    size_t
        cur_size;
    size_t
        max_size;
    byte
        *data;
</context>

<method name = "new">
    <argument name = "size" type = "size_t">Required length of data</argument>
    self->mother   = NULL;
    self->cur_size = 0;
    //   self->max_size is set in the allocator.
    self->data     = (byte *) self + sizeof ($(selftype));
    self_increment_used (self->max_size);

#if defined (DEBUG)
    icl_mem_set_callback (self, $(base)show_);
#endif
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    //  Only decrement total memory allocated if data belongs to bucket.
    if (self->data == (byte *) self + sizeof ($(selftype)))
        self_decrement_used (self->max_size);
    if (self->mother)
        ipr_mother_remove (self);
</method>

<method name = "alloc">
    <argument name = "size" type = "size_t">Required length of data</argument>
    <local>
    int
        chop_min = 0,
        chop_max = S_NUM_BUCKET_SIZES,
        chop_mid;
    </local>
    <header>
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

    self = icl_mem_cache_alloc_ (s_$(selfname)_cache [chop_min], file, line);
    self->max_size = bucket_size [chop_min];
    </header>
</method>

<method name = "free">
    self->object_tag = 0xDEAD;
    icl_mem_free (self);
</method>

<method name = "cache purge" private = "0">
    <local>
    int
        cache_index;
    </local>
    for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
        icl_mem_cache_purge (s_$(selfname)_cache [cache_index]);
</method>

<method name = "empty" template = "function">
    <doc>
    Empties the bucket.
    </doc>
    //
    self->cur_size = 0;
</method>

<method name = "fill" template = "function">
    <doc>
    Fill the bucket with data from some place: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    </doc>
    <argument name = "data" type = "void *">Address of data to copy from</argument>
    <argument name = "size" type = "size_t">Amount of data to copy</argument>
    assert (size <= self->max_size);
    memcpy (self->data, data, size);
    self->cur_size = size;
</method>

<method name = "fill null" template = "function">
    <doc>
    Fill the bucket with binary zero bytes up to the specified size. If
    the requested size is greater than the bucket size, we raise an assertion
    (this is considered a structural error).
    </doc>
    <argument name = "size" type = "size_t">Amount of data to copy</argument>
    assert (size <= self->max_size);
    memset (self->data, 0, size);
    self->cur_size = size;
</method>

<method name = "fill random" template = "function">
    <doc>
    Fill the bucket with randomised data of a specified size.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    </doc>
    <argument name = "fill size" type = "size_t">Amount of random data to insert</argument>
    //
    assert (fill_size <= self->max_size);
    randomize ();
    self->cur_size = 0;
    while (self->cur_size < fill_size)
        self->data [self->cur_size++] = randomof (256);
</method>

<method name = "fill repeat" template = "function">
    <doc>
    Fill the bucket with repeated copies of the specified data, up to the
    specified size.  If the specified size is greater than the bucket size,
    or the source size is zero, we raise an assertion (these are considered
    structural errors).
    </doc>
    <argument name = "data" type = "byte *">Address of data to copy from</argument>
    <argument name = "size" type = "size_t">Size of source data</argument>
    <argument name = "fill size" type = "size_t">Amount of data to fill</argument>
    //
    assert (size);
    assert (fill_size <= self->max_size);
    self->cur_size = 0;
    while (self->cur_size < fill_size) {
        self->data [self->cur_size] = data [self->cur_size % size];
        self->cur_size++;
    }
</method>

<method name = "cat" template = "function">
    <doc>
    Append data to a bucket, updating its current size accordingly.
    If the size of the data to copy is greater than the bucket size,
    we raise an assertion (this is considered a structural error).
    </doc>
    <argument name = "source" type = "void *">Address of data to copy from</argument>
    <argument name = "size"   type = "size_t">Amount of data to copy</argument>
    //
    assert (self->cur_size + size <= self->max_size);
    memcpy (self->data + self->cur_size, source, size);
    self->cur_size += size;
</method>

<method name = "load" template = "function">
    <doc>
    Fill the bucket with data from a file.  We fill as much data into
    the bucket as possible, either the size of data available in the
    file or the maximum size of the bucket, which ever is smaller.  To
    read a full file into buckets, call this routine repeatedly.  Note
    that the loaded data is concatenated into any existing data.  If
    you want to empty the bucket first, call the empty method.
    </doc>
    <argument name = "file" type = "FILE *">Open file to read from</argument>
    //
    self->cur_size += fread (
        self->data + self->cur_size, 1, self->max_size - self->cur_size, file);
</method>

<method name = "save" template = "function">
    <doc>
    Saves the data in the bucket to a file.
    </doc>
    <argument name = "file" type = "FILE *">Open file to write to</argument>
    //
    if (fwrite (self->data, self->cur_size, 1, file) != 1)
        rc = -1;
</method>

<method name = "find" return = "location">
    <doc>
    Searches the bucket for a pattern, using the Boyer-Moore-Horspool-Sunday
    algorithm.  Returns a pointer to the pattern if found within the bucket,
    or NULL if the pattern was not found.
    </doc>
    <declare name = "location" type = "byte *">Location of pattern, or null</declare>
    <argument name = "self"    type = "$(selftype) *">Reference to bucket</argument>
    <argument name = "offset"  type = "size_t">Start searching at this offset</argument>
    <argument name = "pattern" type = "byte *">Pattern to look for</argument>
    <argument name = "patsize" type = "size_t">Size of pattern, in octets</argument>
    <local>
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
    </local>
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
</method>

<method name = "dump" template = "function">
    <doc>
    Prints the contents of a bucket to stderr.  Filters unprintable
    characters.  Does not apply any specific formatting.
    </doc>
    <argument name = "prefix" type = "char *">Prefix for each line</argument>
    <local>
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
    </local>
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
</method>

<method name = "initialise">
    <local>
    int
        cache_index;
    </local>
#if (defined (BASE_THREADSAFE))
    s_class_mutex = icl_mutex_new ();
#endif

    for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
        s_$(selfname)_cache [cache_index] = icl_cache_get (bucket_size [cache_index] + sizeof ($(selftype)));
</method>

<method name = "terminate">
#if (defined (BASE_THREADSAFE))
    icl_mutex_destroy (&s_class_mutex);
#endif
</method>

<method name = "increment used" inline = "1">
    <doc>
    Increment the record of the total memory allocations.
    
    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    </doc>
    <argument name = "size" type = "size_t">The amount by which to increment the total</argument>
    <local>
    void
        *old_used;
    </local>
    do
        old_used = (void *) ipr_bucket_total_used;
    while (icl_atomic_casptr (&ipr_bucket_total_used, (byte *) old_used + size, old_used) != old_used);
</method>

<method name = "decrement used" inline = "1">
    <doc>
    Decrement the record of the total memory allocations.
    
    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    </doc>
    <argument name = "size" type = "size_t">The amount by which to decrement the total</argument>
    <local>
    void
        *old_used;
    </local>
    do
        old_used = (void *) ipr_bucket_total_used;
    while (icl_atomic_casptr (&ipr_bucket_total_used, (byte *) old_used - size, old_used) != old_used);
</method>

<method name = "used" return = "used">
    <doc>
    Return the total amount of memory allocated.
    </doc>
    <declare name = "used" type = "size_t">The returned value</declare>
    //  Do this in a loop to prevent race condition giving a messed-up value
    //  On 32-bit architecture, could use atomic assignment, but not on 64-bit.
    do
        used = (size_t) ipr_bucket_total_used;
    while (used != (size_t) ipr_bucket_total_used);
</method>

<method name = "selftest">
    <local>
    ipr_bucket_t
        *bucket;
    byte
        *location;
    </local>
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
</method>

</class>
