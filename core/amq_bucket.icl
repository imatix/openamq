<?xml?>
<class
    name      = "amq_bucket"
    comment   = "Bucket buffer class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >
<doc>
    Defines a bucket, being a buffer of AMQ_BUCKET_MAX bytes.  Buckets can
    be passed in messages and freed by the recipient.  Buckets are cached
    so that heavy new/destroy use is very cheap.
</doc>

<inherit class = "icl_alloc_cache" />
<inherit class = "icl_ref_count"   />
<option name = "nullify" value = "0" />

<public>
#define AMQ_BUCKET_SIZE  32000
</public>

<context>
    size_t cur_size;
    size_t max_size;
    byte   data [AMQ_BUCKET_SIZE];
</context>

<method name = "new">
    self->cur_size = 0;
    self->max_size = AMQ_BUCKET_SIZE;
</method>

<method name = "destroy">
    /*  Since we don't nullify buckets, set all properties to zero now      */
    self->cur_size = 0;
    self->max_size = 0;
</method>

<method name = "fill" template = "function">
    <doc>
    Fill the bucket with data from some place: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    </doc>
    <argument name = "source" type = "void *">Address of data to copy from</argument>
    <argument name = "size"   type = "size_t">Amount of data to copy</argument>
    ASSERT (size <= self->max_size);
    memcpy (self->data, source, size);
    self->cur_size = size;
</method>

<method name = "load" template = "function">
    <doc>
    Fill the bucket with data from a file: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).  If the
    end of the file is reached before the bucket is full, no error is
    reported but the bucket cur_size will be less than the size argument.
    </doc>
    <argument name = "file" type = "FILE *">File pointer of open file to copy from</argument>
    <argument name = "size" type = "size_t">Amount of data to copy</argument>
    ASSERT (size <= self->max_size);
    self->cur_size = fread (self->data, 1, size, file);
</method>

<method name = "save" template = "function">
    <doc>
    Saves the data in the bucket to a file.
    </doc>
    <argument name = "file" type = "FILE *">File pointer of open file to copy to</argument>
    fwrite (self->data, 1, self->cur_size, file);
</method>

<method name = "selftest" />

</class>
