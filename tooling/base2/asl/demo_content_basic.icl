<?xml?>
<!--
    *** GENERATED FROM demo.asl BY ASL_GEN USING GSL/4 ***

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
    name      = "demo_content_basic"
    comment   = "Demo Basic content class"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >

<inherit class = "icl_object">
    <option name = "alloc"   value = "cache" />
    <option name = "nullify" value = "0" />
    <option name = "links"   value = "1" />
</inherit>
<inherit class = "icl_tracker" />

<import class = "asl" />
<import class = "asl_field" />
<import class = "demo_constants" />

<context>
    dbyte
        class_id,                       //  Content class
        weight;                         //  Content weight
    ipr_bucket_list_t
        *bucket_list;                   //  List of message buckets
    int64_t
        body_size,                      //  Size of content body data
        body_expect;                    //  Expected size of body data
    byte
        *body_data;                     //  Body set by application
    icl_mem_free_fn
        *free_fn;                       //  Function to free appl_body
    icl_shortstr_t
        exchange,                       //  Exchange specified by publish
        routing_key,                    //  Routing key specified by publish
        consumer_tag,                   //  Consumer tag specified by consume
        producer_id;                    //  Connection identifier of producer
    int64_t
        delivery_tag;                   //  Delivery tag from queue

    //  Used internally by servers processing the content
    Bool
        immediate,                      //  Immediate delivery wanted
        redelivered,                    //  Content has been redelivered
        returned;                       //  Content was returned

    //  Content properties
    icl_shortstr_t
        content_type;                   //  MIME content type
    icl_shortstr_t
        content_encoding;               //  MIME content encoding
    icl_longstr_t *
        headers;                        //  Message header field table
    icl_shortstr_t
        reply_to;                       //  The destination to reply to
    icl_shortstr_t
        message_id;                     //  The application message identifier
    icl_shortstr_t
        correlation_id;                 //  The application correlation identifier
</context>

<method name = "new">
    self->class_id    = 60;
    self->weight      = 0;
    self->body_size   = 0;
    self->body_expect = 0;
    self->body_data   = NULL;
    self->immediate   = FALSE;
    self->returned    = FALSE;
    self->bucket_list = NULL;

    strclr (self->exchange);
    strclr (self->routing_key);
    strclr (self->producer_id);
    strclr (self->consumer_tag);
    *self->content_type  = 0;
    *self->content_encoding  = 0;
    self->headers     = NULL;
    *self->reply_to   = 0;
    *self->message_id  = 0;
    *self->correlation_id  = 0;
</method>

<method name = "destroy">
    ipr_bucket_list_destroy (&self->bucket_list);

    icl_longstr_destroy (&self->headers);
    if (self->body_data && self->free_fn) {
        (self->free_fn) (self->body_data);
        self->body_data = NULL;
    }
</method>

<method name = "record header" return = "rc">
    <doc>
    Records a content header, which must be provided as a bucket of data
    received from the network connection and formatted as an ASL content
    header frame.  The bucket size must not include the frame-end octet.
    </doc>
    <argument name = "self" type = "$(selftype) *" default = "NULL" />
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket containing header</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    byte
        *data_ptr,                      //  Pointer into buffer data
        *data_limit;                    //  Limit of buffer data
    size_t
        string_size;
    dbyte
        property_flags = 0;
    </local>
    //
    //  Check that frame is correctly terminated before decoding it
    assert (bucket);
    data_ptr = bucket->data;
    data_limit = data_ptr + bucket->cur_size;

    GET_SHORT (self->class_id,    data_ptr);
    GET_SHORT (self->weight,      data_ptr);
    GET_LLONG (self->body_expect, data_ptr);
    GET_SHORT (property_flags,    data_ptr);

    //  Now get content header property fields that are present
    if (property_flags & 1 &lt;&lt; 15) {
        GET_SSTR (self->content_type, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 14) {
        GET_SSTR (self->content_encoding, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 13) {
        GET_LSTR (self->headers, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 12) {
        GET_SSTR (self->reply_to, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 11) {
        GET_SSTR (self->message_id, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 10) {
        GET_SSTR (self->correlation_id, data_ptr);
    }
    goto finished;

    underflow:
        icl_console_print ("E: invalid content header");
        rc = -1;
    finished:
        //  Return via normal function exit
</method>

<method name = "replay header" return = "bucket">
    <doc>
    Replays the content header, providing a bucket that the caller must
    unlink when finished sending.  For orthogonality with the replay_body
    method, the returned bucket does not contain a frame-end octet and
    the returned bucket cur_size does not account for this octet. The
    supplied bucket data has no frame-end; the caller must add this when
    sending a bucket on the wire.  The frame-end octet, while adding some
    measure of robustness against badly-framed frames, is in itself not
    a construct that leads to highly elegant code.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to content</argument>
    <declare name = "bucket" type = "ipr_bucket_t *">Bucket to hold header</declare>
    <local>
    size_t
        string_size,
        header_size;
    byte
        *data_ptr;
    dbyte
        property_flags = 0;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    //  Content header is 14 bytes plus optional properties
    header_size = 14;
    if (strused (self->content_type)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_type);
        property_flags |= 1 &lt;&lt; 15;
    }
    if (strused (self->content_encoding)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_encoding);
        property_flags |= 1 &lt;&lt; 14;
    }
    if (self->headers && self->headers->cur_size) {
        header_size += ASL_LSTR_HSIZE + self->headers->cur_size;
        property_flags |= 1 &lt;&lt; 13;
    }
    if (strused (self->reply_to)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->reply_to);
        property_flags |= 1 &lt;&lt; 12;
    }
    if (strused (self->message_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->message_id);
        property_flags |= 1 &lt;&lt; 11;
    }
    if (strused (self->correlation_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->correlation_id);
        property_flags |= 1 &lt;&lt; 10;
    }
    bucket = ipr_bucket_new (header_size);
    bucket->cur_size = header_size;
    data_ptr = bucket->data;
    self->weight = 0;

    PUT_SHORT (data_ptr, self->class_id);
    PUT_SHORT (data_ptr, self->weight);
    PUT_LLONG (data_ptr, self->body_size);
    PUT_SHORT (data_ptr, property_flags);

    //  Now put content header property fields that are present
    if (strused (self->content_type))
        PUT_SSTR (data_ptr, self->content_type);
    if (strused (self->content_encoding))
        PUT_SSTR (data_ptr, self->content_encoding);
    if (self->headers && self->headers->cur_size)
        PUT_TABLE (data_ptr, self->headers);
    if (strused (self->reply_to))
        PUT_SSTR (data_ptr, self->reply_to);
    if (strused (self->message_id))
        PUT_SSTR (data_ptr, self->message_id);
    if (strused (self->correlation_id))
        PUT_SSTR (data_ptr, self->correlation_id);
    if ((size_t) (data_ptr - bucket->data) != header_size) {
        icl_console_print ("E: internal error, processed=%d expected=%d",
        (size_t) (data_ptr - bucket->data), header_size);
        assert ((size_t) (data_ptr - bucket->data) == header_size);
    }
</method>

<method name = "record body" template = "function">
    <doc>
    Records a content body, which is a bucket of binary data.  Use this
    method to set the content body to a block of data coming from the
    network, from a file, or from a pipe.  The bucket must not contain
    any frame control data, (i.e. no frame-end octet).  The caller can
    unlink the bucket after this method, which takes possession of it.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket of raw data</argument>
    //
    if (self->body_data) {
        if (self->free_fn)
            (self->free_fn) (self->body_data);
        else
            self->body_data = NULL;
        self->body_size = 0;
    }
    if (!self->bucket_list)
        self->bucket_list = ipr_bucket_list_new ();
    ipr_bucket_list_queue (self->bucket_list, bucket);
    self->body_size += bucket->cur_size;
</method>

<method name = "set reader" template = "function">
    <doc>
    Initialises a new reader. The caller should provide a reader structure
    in thread-safe memory, e.g. on the stack.  We use readers to replay a
    content body into buckets - the reader stores all context needed to
    correctly split and reform the content body into buckets of the desired
    size.  Use the set_reader method to initialise a reader, and then the
    replay_body method to replay buckets of body until there are none left.
    </doc>
    <argument name = "reader" type = "asl_reader_t *">Reader to initialise</argument>
    <argument name = "frame max" type = "size_t" >Maximum frame size</argument>
    <declare name = "rc" type = "int" default = "0" />
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    assert (frame_max >= ASL_FRAME_MIN_SIZE);
    memset (reader, 0, sizeof (*reader));
    if (self->body_data)
        ;
    else
    if (self->bucket_list)
        reader->iterator = ipr_bucket_list_first (self->bucket_list);

    reader->frame_max = frame_max;
</method>

<method name = "replay body" return = "bucket">
    <doc>
    Provides a bucket holding the first frame of body data for the
    content.  If the content had no (more) body, returns a null. The
    caller supplies an initialised reader structure.  Call this method
    in a loop with the same reader structure until the method returns
    a null bucket.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to content</argument>
    <argument name = "reader" type = "asl_reader_t *">Reader to use</argument>
    <declare name = "bucket" type = "ipr_bucket_t *" default = "NULL" />
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    if (self->body_data) {
        size_t
            remainder;                  //  Amount of data remaining

        //  Check if there's any body data remaining
        remainder = (size_t) self->body_size - reader->processed;
        if (remainder > 0) {
            bucket = ipr_bucket_new (0);
            bucket->cur_size = MIN (reader->frame_max, remainder);
            bucket->data     = self->body_data + reader->processed;
            reader->processed += bucket->cur_size;
            reader->count++;
            //  Sanity check
            assert (reader->count < 999999);
        }
    }
    else {
        while (reader->iterator) {
            ipr_bucket_t
                *from_bucket;           //  Bucket we copy from
            size_t
                remainder;              //  Amount of data remaining

            from_bucket = reader->iterator->item;
            assert (from_bucket);

            //  Check if there's any bucket data remaining
            remainder = (size_t) from_bucket->cur_size - reader->processed;
            if (remainder > 0) {
                bucket = ipr_bucket_new (0);
                bucket->cur_size = MIN (reader->frame_max, remainder);
                bucket->data     = from_bucket->data + reader->processed;
                reader->processed += bucket->cur_size;
                reader->count++;
                //  Sanity check
                assert (reader->count < 999999);
                break;                  //  We have a bucket of data
            }
            else {
                //  Get next source bucket, until list ends
                reader->iterator  = ipr_bucket_list_next (&reader->iterator);
                reader->processed = 0;
            }
        }
    }
</method>

<method name = "set body" template = "function">
    <doc>
    Sets the content body to a specified memory address.  The contents
    of the memory block are not copied.  You can specify a function (free()
    or an equivalent) to free the memory block when the message is
    destroyed.
    </doc>
    <argument name = "data" type = "void *">Data for message content</argument>
    <argument name = "size" type = "size_t">Length of message content</argument>
    <argument name = "free fn" type = "icl_mem_free_fn *">Function to free the memory</argument>
    //
    //  Free existing content if any
    if (self->bucket_list)
        ipr_bucket_list_clear (self->bucket_list);
    if (self->body_data && self->free_fn)
        (self->free_fn) (self->body_data);

    //  Now point to provided body
    self->body_data = data;
    self->body_size = size;
    self->free_fn   = free_fn;
</method>

<method name = "get body" template = "function">
    <doc>
    Copies the content body to the application into a buffer provided by the
    application.  If the content was larger than the buffer limit, returns -1,
    else returns the number of content octets copied.
    </doc>
    <argument name = "buffer"  type = "byte *">Buffer for message content</argument>
    <argument name = "maxsize" type = "size_t">Maximum size of buffer</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    ipr_bucket_list_iter_t *
        iterator;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    if (self->body_size &lt;= maxsize) {
        if (self->body_data) {
            memcpy (buffer, self->body_data, (size_t) self->body_size);
            rc = (int) self->body_size;
        }
        else
        if (self->bucket_list) {
            iterator = ipr_bucket_list_first (self->bucket_list);
            while (iterator) {
                memcpy (buffer + rc, iterator->item->data, iterator->item->cur_size);
                rc += iterator->item->cur_size;
                iterator = ipr_bucket_list_next (&iterator);
            }
        }
    }
    else
        rc = -1;                        //  Buffer too small
</method>

<method name = "save body" template = "function">
    <doc>
    Writes the content body to a file as specified by the caller. No header
    properties are saved.
    </doc>
    <argument name = "filename" type = "char *">File to save to</argument>
    //
    <local>
    FILE
        *file_stream = NULL;
    ipr_bucket_list_iter_t *
        iterator;
    </local>
    //
    assert (filename);
    file_stream = fopen (filename, "wb");

    if (self->body_data)
        assert (fwrite (self->body_data, (size_t) self->body_size, 1, file_stream) == 1);
    else {
        iterator = ipr_bucket_list_first (self->bucket_list);
        while (iterator) {
            assert (fwrite (iterator->item->data, (size_t) iterator->item->cur_size, 1,
                file_stream) == 1);
            iterator = ipr_bucket_list_next (&iterator);
        }
    }
    fclose (file_stream);
</method>

<method name = "set routing key" template = "function">
    <doc>
    Set the routing properties for the content. This is an internal
    method that stamps a content with the root exchange and routing key
    used for routing within the server. Calling this method from an
    application has no effect.
    </doc>
    <argument name = "exchange"    type = "char *">Root exchange</argument>
    <argument name = "routing key" type = "char *">Routing key</argument>
    <argument name = "producer id" type = "char *">Producer connection id</argument>
    //
    icl_shortstr_cpy (self->exchange,    exchange);
    icl_shortstr_cpy (self->routing_key, routing_key);
    icl_shortstr_cpy (self->producer_id, producer_id);
</method>

<method name = "get class id" return = "class id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "class id" type = "dbyte" />
    //
    class_id = self->class_id;
</method>

<method name = "get weight" return = "weight">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "weight" type = "dbyte" />
    //
    weight = self->weight;
</method>

<method name = "get body size" return = "body size">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "body size" type = "int64_t" />
    //
    body_size = self->body_size;
</method>

<method name = "get exchange" return = "exchange">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "exchange" type = "char *" />
    //
    exchange = self->exchange;
</method>

<method name = "get routing key" return = "routing key">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "routing key" type = "char *" />
    //
    routing_key = self->routing_key;
</method>

<method name = "get producer id" return = "producer id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "producer id" type = "char *" />
    //
    producer_id = self->producer_id;
</method>

<method name = "set content_type" template = "function">
    <argument name = "content_type" type = "char *">MIME content type</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->content_type, ICL_SHORTSTR_MAX, content_type, args);
</method>

<method name = "get content_type" return = "content_type">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "content_type" type = "char *" />
    //
    content_type = self->content_type;
</method>

<method name = "set content_encoding" template = "function">
    <argument name = "content_encoding" type = "char *">MIME content encoding</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->content_encoding, ICL_SHORTSTR_MAX, content_encoding, args);
</method>

<method name = "get content_encoding" return = "content_encoding">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "content_encoding" type = "char *" />
    //
    content_encoding = self->content_encoding;
</method>

<method name = "set headers" template = "function">
    <argument name = "headers" type = "icl_longstr_t *">Message header field table</argument>
    //
    icl_longstr_destroy (&self->headers);
    if (headers)
        self->headers = icl_longstr_dup (headers);
</method>

<method name = "get headers" return = "headers">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "headers" type = "icl_longstr_t *" />
    //
    headers = self->headers;
</method>

<method name = "set reply_to" template = "function">
    <argument name = "reply_to" type = "char *">The destination to reply to</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->reply_to, ICL_SHORTSTR_MAX, reply_to, args);
</method>

<method name = "get reply_to" return = "reply_to">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_to" type = "char *" />
    //
    reply_to = self->reply_to;
</method>

<method name = "set message_id" template = "function">
    <argument name = "message_id" type = "char *">The application message identifier</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->message_id, ICL_SHORTSTR_MAX, message_id, args);
</method>

<method name = "get message_id" return = "message_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "message_id" type = "char *" />
    //
    message_id = self->message_id;
</method>

<method name = "set correlation_id" template = "function">
    <argument name = "correlation_id" type = "char *">The application correlation identifier</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->correlation_id, ICL_SHORTSTR_MAX, correlation_id, args);
</method>

<method name = "get correlation_id" return = "correlation_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "correlation_id" type = "char *" />
    //
    correlation_id = self->correlation_id;
</method>

<method name = "set headers field" template = "function">
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "char *">Field value</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    <local>
	asl_field_list_t
        *field_list;
    icl_longstr_t
        *field_table;
    icl_shortstr_t
        field_value;
    </local>
    //
    apr_vsnprintf (field_value, ICL_SHORTSTR_MAX, value, args);

    field_list = asl_field_list_new (self->headers);
    asl_field_assume (field_list, name, field_value);
    field_table = asl_field_list_flatten (field_list);
    asl_field_list_destroy (&field_list);
    self_set_headers (self, field_table);
    icl_longstr_destroy (&field_table);
</method>

<method name = "wire put" template = "function">
    <doc>
    Puts the content into a bucket provided by the user.  Returns -1 if the
    bucket was too small, zero if the content was successfully serialized.
    Wire data is formatted using X-DMP@wiki.amqp.org specifications.
    Content body is limited to 16M in size.

    [nnnn]                  full encoded size
    [n][exchange]           exchange name
    [n][routing key]        routing key
    [ff][property...]       header flags + properties
    [n]                     options octet
    [nnn][body]             content body
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket to fill</argument>
    <argument name = "options" type = "byte">Options octet</argument>
    <local>
    byte
        *size_ptr,                      //  Offset where we stick chunk size
        *data_ptr;                      //  Current offset writing into data
    size_t
        string_size,
        header_size,
        chunk_size;
    dbyte
        property_flags = 0;
    </local>
    //
    //  We need sufficient space left in the bucket
    if (bucket->max_size - bucket->cur_size >= demo_content_basic_wire_size (self)) {
        //  Allow four octets at start for encoded size
        size_ptr = bucket->data + bucket->cur_size;
        data_ptr = size_ptr + 4;

        //  Store exchange and routing key
        PUT_SSTR (data_ptr, self->exchange);
        PUT_SSTR (data_ptr, self->routing_key);

        //  Store [property flags] as 2 octets
        header_size = 0;
    if (strused (self->content_type)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_type);
        property_flags |= 1 &lt;&lt; 15;
    }
    if (strused (self->content_encoding)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_encoding);
        property_flags |= 1 &lt;&lt; 14;
    }
    if (self->headers && self->headers->cur_size) {
        header_size += ASL_LSTR_HSIZE + self->headers->cur_size;
        property_flags |= 1 &lt;&lt; 13;
    }
    if (strused (self->reply_to)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->reply_to);
        property_flags |= 1 &lt;&lt; 12;
    }
    if (strused (self->message_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->message_id);
        property_flags |= 1 &lt;&lt; 11;
    }
    if (strused (self->correlation_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->correlation_id);
        property_flags |= 1 &lt;&lt; 10;
    }
        PUT_SHORT (data_ptr, property_flags);
    if (strused (self->content_type))
        PUT_SSTR (data_ptr, self->content_type);
    if (strused (self->content_encoding))
        PUT_SSTR (data_ptr, self->content_encoding);
    if (self->headers && self->headers->cur_size)
        PUT_TABLE (data_ptr, self->headers);
    if (strused (self->reply_to))
        PUT_SSTR (data_ptr, self->reply_to);
    if (strused (self->message_id))
        PUT_SSTR (data_ptr, self->message_id);
    if (strused (self->correlation_id))
        PUT_SSTR (data_ptr, self->correlation_id);

        //  Store options octet
        PUT_OCTET (data_ptr, options);

        //  Store 3-byte length and then content body
        data_ptr [0] = (byte) ((self->body_size >> 16) & 255);
        data_ptr [1] = (byte) ((self->body_size >> 8)  & 255);
        data_ptr [2] = (byte) ((self->body_size)       & 255);
        data_ptr += 3;
        demo_content_basic_get_body (self, data_ptr, (size_t) self->body_size);
        data_ptr += self->body_size;

        bucket->cur_size = data_ptr - bucket->data;

        chunk_size = data_ptr - size_ptr - 4;
        assert (chunk_size >= 12);
        PUT_LONG (size_ptr, chunk_size);
        assert (bucket->cur_size <= bucket->max_size);
    }
    else
        rc = -1;                        //  Sorry, we're full
</method>

<method name = "wire size" return = "rc">
    <argument name = "self" type = "$(selftype) *" />
    <declare name = "rc" type = "size_t" />
    <doc>
    Returns the size of the serialized content.
    </doc>
    <local>
    dbyte
        property_flags = 0;
    size_t
        header_size = 0;
    </local>
    //
    header_size = 0;
    if (strused (self->content_type)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_type);
        property_flags |= 1 &lt;&lt; 15;
    }
    if (strused (self->content_encoding)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->content_encoding);
        property_flags |= 1 &lt;&lt; 14;
    }
    if (self->headers && self->headers->cur_size) {
        header_size += ASL_LSTR_HSIZE + self->headers->cur_size;
        property_flags |= 1 &lt;&lt; 13;
    }
    if (strused (self->reply_to)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->reply_to);
        property_flags |= 1 &lt;&lt; 12;
    }
    if (strused (self->message_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->message_id);
        property_flags |= 1 &lt;&lt; 11;
    }
    if (strused (self->correlation_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->correlation_id);
        property_flags |= 1 &lt;&lt; 10;
    }
    rc = 12
        + strlen (self->exchange)
        + strlen (self->routing_key)
        + header_size
        + (size_t) self->body_size;
</method>

<method name = "wire get" return = "content">
    <doc>
    Gets a content from a bucket provided by the user.  Returns NULL if the
    bucket did not contain sufficient data, or a content was unsuccessfully
    de-serialized. Wire data is formatted using 6-DMP@wiki.amqp.org specs.
    Skips heartbeat frames (null contents) transparently.  If the options
    argument is not null, puts the options octet in that byte.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <argument name = "offset" type = "size_t *">Starting, ending offset in bucket</argument>
    <argument name = "options_p" type = "byte *">Options octet address</argument>
    <declare name = "content" type = "$(selftype) *" default = "NULL">New content</declare>
    <local>
    byte
        *body,                          //  Allocated content body blob
        *data_ptr;
    uint
        size;
    byte
        *data_limit;                    //  Limit of buffer data
    size_t
        chunk_size = 0,
        string_size;
    dbyte
        property_flags = 0;
    byte
        options;
    </local>
    //
    data_ptr = bucket->data + *offset;
    data_limit = bucket->data + bucket->cur_size;
    //  Get a non-zero chunk size - zero means heartbeat
    while (chunk_size == 0)
        GET_LONG (chunk_size, data_ptr);
    data_limit = data_ptr + chunk_size;

    //  Guard against malformed data
    if (data_limit > bucket->data + bucket->cur_size) {
        icl_console_print ("E: malformed Direct Mode data - chunk-size=%d at %d",
            chunk_size, (int) (data_ptr - bucket->data));
        ipr_bucket_dump (bucket, "");
        //  For now, we assert and die, should just reject the data
        assert (data_limit <= bucket->data + bucket->cur_size);
    }
    content = demo_content_basic_new ();
    assert (content);
    //  Get exchange and routing key
    GET_SSTR (content->exchange, data_ptr);
    GET_SSTR (content->routing_key, data_ptr);

    //  Now get content header property fields that are present
    GET_SHORT (property_flags, data_ptr);
    if (property_flags & 1 &lt;&lt; 15) {
        GET_SSTR (content->content_type, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 14) {
        GET_SSTR (content->content_encoding, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 13) {
        GET_LSTR (content->headers, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 12) {
        GET_SSTR (content->reply_to, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 11) {
        GET_SSTR (content->message_id, data_ptr);
    }
    if (property_flags & 1 &lt;&lt; 10) {
        GET_SSTR (content->correlation_id, data_ptr);
    }

    //  Get options octet and return to caller
    GET_OCTET (options, data_ptr);
    if (options_p)
        *options_p = options;

    //  Get 3-byte size and point to body
    if (data_ptr + 3 > data_limit)
        goto underflow;
    size = (data_ptr [0] << 16) + (data_ptr [1] << 8) + data_ptr [2];
    data_ptr += 3;
    //  Size must be equal to or less than remaining data
    if (size > (uint) (data_limit - data_ptr))
        goto underflow;

    body = icl_mem_alloc (size);
    memcpy (body, data_ptr, size);
    demo_content_basic_set_body (content, body, size, icl_mem_free);
    data_ptr += size;

    *offset = data_ptr - bucket->data;
    goto finished;
    underflow:
        demo_content_basic_unlink (&content);
    finished:
        //  Return via normal function exit
</method>

<method name = "wire pending" return = "rc">
    <doc>
    Checks whether the supplied bucket holds a complete set of contents or not.
    If complete, returns 0.  Otherwise returns the number of octets that needs to
    be read in order to complete the bucket.  Data should be read into the bucket
    at bucket->cur_size.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <declare name = "rc" type = "int">Pending octets</declare>
    <local>
    byte
        *data_ptr;
    byte
        *data_limit;                    //  Limit of buffer data
    size_t
        chunk_size;
    </local>
    data_ptr   = bucket->data;
    data_limit = bucket->data + bucket->cur_size;
    while (TRUE) {
        GET_LONG (chunk_size, data_ptr);
        data_ptr += chunk_size;
        if (data_ptr > data_limit) {
            rc = data_ptr - data_limit;
            goto finished;
        }
        else
        if (data_ptr == data_limit) {
            rc = 0;                 //  Finished on target
            goto finished;
        }
    }
    underflow:
        //  We get here if we could not get 4 bytes for a header
        rc = 4 - (data_limit - data_ptr);
        assert (rc > 0);
    finished:
        //  Return via normal function exit
</method>

<method name = "chrono set" template = "function">
    <doc>
    Implements the content chronometer, a way of tracking the time spent
    processing contents through an architecture.  The chronometer records
    a series of timestamps and calculates the differences between these as
    a list of deltas, in milliseconds.  To use the chronometer, call the
    chrono_set() method on a new content, then call chrono_add() at each
    measuring point, and finally chrono_get() to return the list of deltas.
    The chronometer uses the correlation-id field (which is not very clean
    but simple and fast).
    </doc>
#   define CHRONO_MARKER "Chrono-"
    //
    //  Reset the chronometer with the current time
    snprintf (self->correlation_id, ICL_SHORTSTR_MAX,
        CHRONO_MARKER "%llx", (long long unsigned int) apr_time_now ());
</method>

<method name = "chrono add" template = "function">
    <doc>
    Appends a delta to the chronometer, if the field has been properly set.
    If the field was not set, does nothing.  Always measures the delta from
    the initial start time so there is no accumulated rounding error.
    </doc>
    <local>
    apr_time_t
        start_time;
    int
        msecs;
    </local>
    //
    if (ipr_str_prefixed (self->correlation_id, CHRONO_MARKER)) {
        start_time = (apr_time_t) (strtoull (self->correlation_id + strlen (CHRONO_MARKER), NULL, 16));
        //  APR times are in usec, we chop down to msec to save bytes
        //  If the delta is above 60 seconds, it's stored as infinity
        msecs = (int) ((apr_time_now () - start_time) / 1000);
        if (msecs > 60000)
            msecs = 0xffff;
        icl_shortstr_fmt (self->correlation_id + strlen (self->correlation_id), " %x", msecs);
    }
</method>

<method name = "chrono get" template = "function">
    <doc>
    Copies, into the provided short string, the list of deltas collected by
    the chronometer for the current content.  If the chronometer was not
    initialized, empties the provided string.  The deltas are formatted as
    decimal millisecond values, separated by spaces.  If the floor is not
    zero, measurements that are below the floor will be discarded.
    </doc>
    <argument name = "deltas" type = "char *">Holds deltas</argument>
    <argument name = "floor" type = "int">Only if final delta GE this</argument>
    <local>
    apr_time_t
        start_time;
    char
        *chrono_ptr,
        *delim = "";
    int
        previous = 0,
        msecs = 0;
    </local>
    //
    strclr (deltas);
    chrono_ptr = self->correlation_id;
    if (ipr_str_prefixed (self->correlation_id, CHRONO_MARKER)) {
        chrono_ptr += strlen (CHRONO_MARKER);
        start_time = (apr_time_t) (strtoull (chrono_ptr, &chrono_ptr, 16));
        while (*chrono_ptr) {
            assert (strlen (deltas) < ICL_SHORTSTR_MAX);
            msecs = strtoul (chrono_ptr, &chrono_ptr, 16);
            if (msecs >= 0xffff)
                msecs = 0;              //  Ignore/truncate overflows

            icl_shortstr_fmt (deltas + strlen (deltas),
                "%s%d", delim, MAX (msecs - previous, 0));
            delim = " ";
            previous = msecs;
        }
        //  Discard deltas if last figure was less than the floor
        if (msecs < floor)
            strclr (deltas);
    }
</method>

<method name = "selftest">
    <local>
    demo_content_basic_t
        *content;
    icl_longstr_t
        *longstr;
    ipr_bucket_t
        *bucket,
        *header,
        *body;
    byte
        *test_data,                     //  Test data
        *test_copy;                     //  Copy of test data
    size_t
        processed;                      //  Amount of data processed
    asl_reader_t
        reader;                         //  Body reader
    int
        test_size,                      //  Test data size
        times,
        count;                          //  Test counter
    size_t
        offset;                         //  Serialization offset
    icl_shortstr_t
        deltas;                         //  Chronometer deltas
    byte
        options;
    </local>

#   define TEST_CHUNK       16000
#   define TEST_DATA_SIZE   TEST_CHUNK*10+1    //  11 buckets

    //  Test 1
    //      - construct content explicitly from body and properties
    //      - replay into header and body buckets
    //
    content = demo_content_basic_new ();
    longstr = icl_longstr_new ("abc", TEST_CHUNK);

    demo_content_basic_set_body (content, "abc", TEST_CHUNK, NULL);
    demo_content_basic_set_content_type (content, "abc");
    demo_content_basic_set_content_encoding (content, "abc");
    demo_content_basic_set_headers (content, longstr);
    demo_content_basic_set_reply_to (content, "abc");
    demo_content_basic_set_message_id (content, "abc");
    demo_content_basic_set_correlation_id (content, "abc");
    icl_longstr_destroy (&longstr);

    //  We replay the content into a series of buckets
    //  A small content comes out as two buckets - header and body
    //
    demo_content_basic_set_reader (content, &reader, TEST_CHUNK);
    header = demo_content_basic_replay_header (content);
    body   = demo_content_basic_replay_body (content, &reader);
    assert (header);
    assert (body);
    //  Let try it a couple more times...
    assert (demo_content_basic_replay_body (content, &reader) == NULL);
    assert (demo_content_basic_replay_body (content, &reader) == NULL);
    demo_content_basic_unlink (&content);

    //  Test 2
    //      - construct content using header and body buckets
    //      - check that properties are correctly set
    //
    content = demo_content_basic_new ();
    demo_content_basic_record_header (content, header);
    demo_content_basic_record_body  (content, body);
    assert (streq (content->content_type, "abc"));
    assert (streq (content->content_encoding, "abc"));
    assert (content->headers);
    assert (content->headers->cur_size == TEST_CHUNK);
    assert (streq (content->reply_to, "abc"));
    assert (streq (content->message_id, "abc"));
    assert (streq (content->correlation_id, "abc"));
    demo_content_basic_unlink (&content);

    //  Test 3
    //      - record/replay body of several buckets
    //      - check we can record the same bucket many times
    //
    content = demo_content_basic_new ();
    demo_content_basic_record_header (content, header);
    for (count = 0; count < 10; count++)
        demo_content_basic_record_body (content, body);
    ipr_bucket_unlink (&header);
    ipr_bucket_unlink (&body);

    demo_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = demo_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 10);

    //  Test 4
    //      - replay body into smaller buckets
    //
    demo_content_basic_set_reader (content, &reader, TEST_CHUNK / 2);
    while ((body = demo_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 20);
    demo_content_basic_unlink (&content);

    //  Test 5
    //      - set large content body and replay as buckets
    //      - write the body to a file
    //
    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    memset (test_data, 0xAA, TEST_DATA_SIZE);

    content = demo_content_basic_new ();
    demo_content_basic_set_body (content, test_data, TEST_DATA_SIZE, icl_mem_free);
    demo_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = demo_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 11);
    demo_content_basic_save_body (content, "demo_content_basic.tst");
    assert (ipr_file_exists ("demo_content_basic.tst"));
    assert (ipr_file_size   ("demo_content_basic.tst") == content->body_size);
    ipr_file_delete ("demo_content_basic.tst");
    demo_content_basic_unlink (&content);

    //  Test 6
    //      - coalesce content into single large buffer
    //      - typically used to return data to application
    //
    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    test_copy = icl_mem_alloc (TEST_DATA_SIZE);
    memset (test_data, 0xAA, TEST_DATA_SIZE);
    memset (test_copy, 0,    TEST_DATA_SIZE);
    processed = 0;

    content = demo_content_basic_new ();
    demo_content_basic_set_body (content, test_data, TEST_DATA_SIZE, icl_mem_free);
    demo_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = demo_content_basic_replay_body (content, &reader))) {
        memcpy (test_copy + processed, body->data, body->cur_size);
        processed += body->cur_size;
        ipr_bucket_unlink (&body);
    }
    assert (processed == TEST_DATA_SIZE);
    assert (memcmp (test_data, test_copy, TEST_DATA_SIZE) == 0);
    demo_content_basic_unlink (&content);
    icl_mem_free (test_copy);

    //  Test 7
    //      - coalesce content into application buffer
    //      - using get_body

    content = demo_content_basic_new ();
    body = ipr_bucket_new (TEST_CHUNK);
    body->cur_size = TEST_CHUNK;
    memset (body->data, 0xAA, TEST_CHUNK);
    for (count = 0; count < 10; count++)
        demo_content_basic_record_body (content, body);
    ipr_bucket_unlink (&body);

    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    assert (demo_content_basic_get_body (content, test_data, TEST_DATA_SIZE) == TEST_CHUNK * 10);
    demo_content_basic_unlink (&content);
    icl_mem_free (test_data);

    //  Test 8
    //      - empty content
    //
    content = demo_content_basic_new ();
    header = demo_content_basic_replay_header (content);
    demo_content_basic_record_header (content, header);
    ipr_bucket_unlink (&header);
    demo_content_basic_unlink (&content);

    //  Test 9
    //      - wire serialization
    //
    //  do 100k contents
    randomize ();
    for (times = 0; times < 1000; times++) {
        processed = 0;
        bucket = ipr_bucket_new ((1024 + 50) * 100);
        for (count = 0; count < 100; count++) {
            test_size = randomof (1023);
            test_size += 1;
            test_data = icl_mem_alloc (test_size);
            memset (test_data, 0xAA, test_size);
            content = demo_content_basic_new ();
            demo_content_basic_set_routing_key (content, "exchange", "routing key", 0);
            demo_content_basic_set_body (content, test_data, test_size, icl_mem_free);
            demo_content_basic_wire_put (content, bucket, 123);
            demo_content_basic_unlink (&content);
            processed++;
        }
        assert (demo_content_basic_wire_pending (bucket) == 0);
        offset = 0;
        content = demo_content_basic_wire_get (bucket, &offset, &options);
        while (content) {
            assert (streq (content->routing_key, "routing key"));
            assert (options == 123);
            demo_content_basic_unlink (&content);
            content = demo_content_basic_wire_get (bucket, &offset, &options);
            processed--;
        }
        assert (processed == 0);
        ipr_bucket_unlink (&bucket);
    }
    //  Test chronometer
    content = demo_content_basic_new ();
    demo_content_basic_chrono_set (content);
    apr_sleep (randomof (5000));
    demo_content_basic_chrono_add (content);
    apr_sleep (randomof (5000));
    demo_content_basic_chrono_add (content);
    apr_sleep (randomof (5000));
    demo_content_basic_chrono_add (content);
    apr_sleep (randomof (5000));
    demo_content_basic_chrono_get (content, deltas, 0);
    if (!isdigit (*deltas)) {
        icl_console_print ("E: deltas wrong: cid=%s dta=%s", content->correlation_id, deltas);
        assert (isdigit (*deltas));
    }
    demo_content_basic_unlink (&content);
</method>

</class>
