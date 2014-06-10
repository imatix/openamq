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
    name      = "ipr_bucket_list"
    comment   = "list of ipr_buckets"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Defines a bucket list.  You can use bucket lists to manage large
    amounts of data in memory without needing to pre-allocate the
    entire amount.
</doc>

<inherit class = "icl_iter_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "ipr_bucket" />
    <option name = "childtype"  value = "ipr_bucket_t" />
</inherit>
<option name = "alloc" value = "cache" />

<import class = "ipr_bucket" />
<import class = "ipr_file" />

<context>
    int64_t
        max_size,                       //  Total allocation
        cur_size;                       //  Current total size
</context>

<public>
#define IRP_BUCKET_DEFAULT_CHUNK    65000
</public>

<method name = "clear" template = "function">
    <doc>
    Removes all the buckets from the list.
    </doc>
    <local>
    $(selfname)_iter_t
        *item;
    </local>
    //
    item = ($(selfname)_iter_t *) self->$(selfname)_iter.next;
    while (item != &self->$(selfname)_iter) {
        $(selfname)_unhook (item);
        $(selfname)_iter_destroy (&item);
        item = ($(selfname)_iter_t *) self->$(selfname)_iter.next;
    }
    self->max_size = 0;
    self->cur_size = 0;
</method>

<method name = "load" template = "function">
    <doc>
    Reads an entire file into the bucket list.  Extends the list if it
    is not empty.  If you want to empty the bucket list first, use the
    ipr_bucket_list_clear method.  The file is read as binary data on
    systems that make such distinctions.  Returns an error if the file
    cannot be found.  If the chunking argument is zero, uses a suitable
    default.
    </doc>
    <argument name = "filename" type = "char *">File to read</argument>
    <argument name = "chunking" type = "size_t">Size of buckets</argument>
    <local>
    FILE
        *file_stream;                   //  Handle to opened file
    ipr_bucket_t
        *bucket;                        //  Allocated bucket
    </local>
    //
    assert (filename);
    if (chunking == 0)
        chunking = IRP_BUCKET_DEFAULT_CHUNK;

    file_stream = fopen (filename, "rb");
    if (file_stream) {
        bucket = ipr_bucket_new (chunking);
        ipr_bucket_load (bucket, file_stream);
        while (bucket->cur_size) {
            self->max_size += bucket->max_size;
            self->cur_size += bucket->cur_size;
            ipr_bucket_list_queue (self, bucket);
            ipr_bucket_unlink (&bucket);
            bucket = ipr_bucket_new (chunking);
            ipr_bucket_load (bucket, file_stream);
        }
        ipr_bucket_unlink (&bucket);
        fclose (file_stream);
    }
    else
        rc = -1;                        //  Failed, cannot read file
</method>

<method name = "save" template = "function">
    <doc>
    Writes a bucket list to a specified file.  The file is written as
    binary data on systems that make such distinctions.
    </doc>
    <argument name = "filename" type = "char *">File to create</argument>
    <local>
    FILE
        *file_stream;                   //  Handle to opened file
    ipr_bucket_list_iter_t
        *iterator;
    </local>
    //
    assert (filename);
    file_stream = fopen (filename, "wb");
    if (file_stream) {
        iterator = ipr_bucket_list_first (self);
        while (iterator) {
            ipr_bucket_save (iterator->item, file_stream);
            iterator = ipr_bucket_list_next (&iterator);
        }
        fclose (file_stream);
    }
    else
        rc = -1;                        //  Failed, cannot write file
</method>

<method name = "selftest">
    <local>
    ipr_bucket_list_t
        *bucket_list_1,
        *bucket_list_2;
    </local>
    //
    bucket_list_1 = ipr_bucket_list_new ();
    bucket_list_2 = ipr_bucket_list_new ();

    ipr_bucket_list_load (bucket_list_1, "ipr_bucket_list.c", 1111);
    ipr_bucket_list_save (bucket_list_1, "ipr_bucket_list.tmp");

    ipr_bucket_list_load (bucket_list_2, "ipr_bucket_list.tmp", 3333);
    assert (bucket_list_1->cur_size == bucket_list_2->cur_size);

    ipr_file_delete ("ipr_bucket_list.tmp");
    ipr_bucket_list_destroy (&bucket_list_1);
    ipr_bucket_list_destroy (&bucket_list_2);
</method>

</class>
