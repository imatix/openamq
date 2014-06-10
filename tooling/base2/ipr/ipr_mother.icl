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
    name      = "ipr_mother"
    comment   = "Bucket buffer class"
    version   = "2.0"
    script    = "icl_gen"
    >
<doc>
    [Todo]
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
    <option name = "links"  value = "1" />
</inherit>

<import class = "ipr_bucket" />

<public>
#define IPR_MOTHER_BUCKET_SIZE 65536
</public>

<context>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    apr_os_thread_t     apr_os_thread;  //  So we can assert no contention
#endif
    uint                slave_count;    //  Number of slave buckets
    size_t              allocated,      //  Total master data allocated
                        filled,         //  Total master data filled
                        claimed;        //  Total master date claimed
    Bool                closed;
    byte                data[IPR_MOTHER_BUCKET_SIZE];
                                        //  The actual data
</context>

<method name = "new">
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    self->apr_os_thread = apr_os_thread_current ();
#endif
    self->slave_count = 0;
    self->allocated   = 0;
    self->filled      = 0;
    self->claimed     = 0;
    self->closed      = FALSE;
</method>

<method name = "allocate" return = "data">
    <argument name = "self" type = "ipr_mother_t *" ref = "1">The mother to extend</argument>
    <argument name = "size" type = "size_t">Required length of data</argument>
    <declare name = "data" type = "byte *">Returned data space</declare>
    <doc>
    Ensures that mother has enough unfilled space to hold 'size' bytes and
    returns a pointer to that space.
    </doc>
    <local>
    ipr_mother_t
        *new_mother;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->apr_os_thread);
#endif
    //  Sanity check
    if (self->allocated - self->claimed + size > IPR_MOTHER_BUCKET_SIZE) {
        icl_system_panic ("ipr_mother", "Attempt to read greater than mother size.");
        abort ();
    }

    //  If not enough data remain, start a new mother
    if (IPR_MOTHER_BUCKET_SIZE - self->allocated < size) {
        new_mother = ipr_mother_new ();
        if (self->filled > self->allocated) {
            memcpy (new_mother->data,
                    self->data + self->allocated,
                    self->filled - self->allocated);
                
            new_mother->filled = self->filled - self->allocated;
            self->filled       = self->allocated;
        }
        self->closed = TRUE;
        ipr_mother_unlink (self_p);
        self = *self_p
             = new_mother;
    }
    data = self->data +  self->allocated;
    self->allocated   += size;
</method>

<method name = "unfilled" return = "remaining" inline = "1">
    <argument name = "self" type = "ipr_mother_t *">The mother object</argument>
    <declare  name = "remaining" type = "size_t">The remaining data size</declare>
    <doc>
    Returns the remaining allocated master data size.
    </doc>
    if (self->closed)
        remaining = self->allocated - self->filled;
    else
        remaining = IPR_MOTHER_BUCKET_SIZE - self->filled;
</method>

<method name = "overhang" return = "overhang" inline = "1">
    <argument name = "self" type = "ipr_mother_t *">The mother object</argument>
    <declare  name = "overhang" type = "size_t">The overhanging data size</declare>
    <doc>
    Returns the number of overhanging bytes, ie the number of bytes in the
    master bucket that haven't been claimed by a slave bucket.
    </doc>
    overhang = self->filled - self->claimed;
</method>

<method name = "claim" return = "slave">
    <argument name = "self" type = "ipr_mother_t *">The mother from which to obtain data</argument>
    <argument name = "size" type = "size_t">Required length of obtained data</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "slave" type = "ipr_bucket_t *" default = "NULL">Returned pointer to slave bucket</declare>
    <doc>
    Creates a bucket pointing to the next data in the mother.  If the mother
    does not have enough data, as defined by its cur_size field, no bucket is
    created.
    </doc>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->apr_os_thread);
#endif
    if (self->filled - self->claimed >= size) {
        assert (self->claimed + size <= self->allocated);
        slave = ipr_bucket_new_ (file, line, 0);
        slave->data = self->data + self->claimed;
        slave->max_size = size;
        slave->cur_size = size;
        self->claimed += size;
        
        icl_atomic_inc32 (&self->slave_count);
        slave->mother = ipr_mother_link (self);
        
    }
</method>

<method name = "fill" template = "function">
    <argument name = "self" type = "ipr_mother_t *">The mother from which to consume data</argument>
    <argument name = "size" type = "size_t">Required length of consumed data</argument>
    <doc>
    Consumes free space in the master bucket.
    </doc>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->apr_os_thread);
#endif
    if (self->closed)
        assert (self->filled + size <= self->allocated);
    else
        assert (self->filled + size <= IPR_MOTHER_BUCKET_SIZE);
    self->filled += size;
</method>

<method name = "remove">
    <argument name = "slave" type = "ipr_bucket_t *">Slave bucket being destroyed</argument>
    icl_atomic_dec32 (&slave->mother->slave_count);
    ipr_mother_unlink (&slave->mother);
</method>

<method name = "selftest">
    <local>
    ipr_mother_t *mother;
    ipr_bucket_t *slave1,
                 *slave2;
    byte         *data;
    int          i;
    </local>
    mother = ipr_mother_new ();
    data = ipr_mother_allocate (&mother, 40000);
    for (i = 0; i < 60000; i++)
        data[i] = (byte) i & 255;
    ipr_mother_fill (mother, 60000);
    slave1 = ipr_mother_claim (mother, 40000);
   
    data = ipr_mother_allocate (&mother, 40000);
    for (i = 0; i < 20000; i++)
        data[20000 + i] = (byte) (60000+i) & 255;
    ipr_mother_fill (mother, 20000);
    slave2 = ipr_mother_claim (mother, 40000);
        
    for (i = 0; i < 40000; i++) {
        assert (slave1->data[i] == (byte) (i & 255));
        assert (slave2->data[i] == ((byte) (40000+i) & 255));
    }
    ipr_bucket_destroy (&slave2);
    ipr_bucket_destroy (&slave1);
    ipr_mother_destroy (&mother);
</method>

</class>
