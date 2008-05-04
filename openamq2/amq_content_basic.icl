<?xml?>
<!--
    *** GENERATED FROM amq.asl BY ASL_GEN USING GSL/4 ***

    
    Copyright (c) 1996-2007 iMatix Corporation
    
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
    name      = "amq_content_basic"
    comment   = "Amq Basic content class"
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
<import class = "asl_content_class" />
<import class = "amq_constants" />

<context>
    dbyte
        class_id,                       //  Content class
        weight;                         //  Content weight
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
        producer_id;                    //  Connection identifier of producer

    //  Used internally by servers processing the content
    Bool
        immediate,                      //  Immediate delivery wanted
        returned;                       //  Content was returned

    //  Content properties
    icl_shortstr_t
        content_type;                   //  MIME content type
    icl_shortstr_t
        content_encoding;               //  MIME content encoding
    icl_longstr_t *
        headers;                        //  Message header field table
    byte
        delivery_mode;                  //  Non-persistent (1) or persistent (2)
    byte
        priority;                       //  The message priority, 0 to 9
    icl_shortstr_t
        correlation_id;                 //  The application correlation identifier
    icl_shortstr_t
        reply_to;                       //  The destination to reply to
    icl_shortstr_t
        expiration;                     //  Message expiration specification
    icl_shortstr_t
        message_id;                     //  The application message identifier
    int64_t
        timestamp;                      //  The message timestamp
    icl_shortstr_t
        type;                           //  The message type name
    icl_shortstr_t
        user_id;                        //  The creating user id
    icl_shortstr_t
        app_id;                         //  The creating application id
    icl_shortstr_t
        sender_id;                      //  Identifier of connection sending the message
</context>

<method name = "new">
    self->class_id    = 60;
    self->weight      = 0;
    self->body_size   = 0;
    self->body_expect = 0;
    self->body_data   = NULL;
    self->returned    = FALSE;
    strclr (self->exchange);
    strclr (self->routing_key);
    strclr (self->producer_id);
    *self->content_type  = 0;
    *self->content_encoding  = 0;
    self->headers     = NULL;
    self->delivery_mode  = 0;
    self->priority    = 0;
    *self->correlation_id  = 0;
    *self->reply_to   = 0;
    *self->expiration  = 0;
    *self->message_id  = 0;
    self->timestamp   = 0;
    *self->type       = 0;
    *self->user_id    = 0;
    *self->app_id     = 0;
    *self->sender_id  = 0;
</method>

<method name = "destroy">
    icl_longstr_destroy (&self->headers);
    if (self->body_data && self->free_fn) {
        (self->free_fn) (self->body_data);
        self->body_data = NULL;
    }
</method>

<method name = "attach" template = "function">
    <doc>
    Attaches this content to an arbitrary parent content list,
    registering all callback handlers for methods to read and
    destroy the content.
    </doc>
    <argument name = "list" type = "asl_content_class_list_t *">List to attach to</argument>
    <local>
    asl_content_class_t
        *class;                         //  Descriptor to own class
    void
        *self_p;
    </local>
    //
    class = asl_content_class_new (&self_p, (icl_destructor_fn *) amq_content_basic_unlink_);
    self  = self_p;
    class->get_body      = amq_content_basic_get_body;
    class->replay_header = amq_content_basic_replay_header;
    class->replay_body   = amq_content_basic_replay_body;
    class->set_reader    = amq_content_basic_set_reader;
    asl_content_class_list_push (list, class);
    asl_content_class_unlink (&class);
</method>

<method name = "record header" template = "function">
    <doc>
    Records a content header, which must be provided as a bucket of data
    received from the network connection and formatted as an ASL content
    header frame.  The bucket size must not include the frame-end octet.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket containing header</argument>
    <local>
    byte
        *data_source,                   //  Pointer into buffer data
        *data_limit;                    //  Limit of buffer data
    size_t
        string_size;
    dbyte
        property_flags = 0;
    char
        *underflowed;                   //  Did we underflow buffer?
    </local>
    //
    //  Check that frame is correctly terminated before decoding it
    assert (bucket);
    data_source = bucket->data;
    data_limit  = data_source + bucket->cur_size;
    underflowed = NULL;

    //  Content header is 14 bytes plus optional properties
    if (data_source &lt;= data_limit - 14) {
        GET_SHORT (self->class_id,    data_source);
        GET_SHORT (self->weight,      data_source);
        GET_LLONG (self->body_expect, data_source);
        GET_SHORT (property_flags,    data_source);
    }
    else
        underflowed = "some mandatory header fields are missing";

    //  Now get content header property fields that are present
    if (property_flags & 1 &lt;&lt; 15) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "content_type field is corrupted";
            else {
                memcpy (self->content_type, data_source, string_size);
                self->content_type[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing content_type field";
    }
    if (property_flags & 1 &lt;&lt; 14) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "content_encoding field is corrupted";
            else {
                memcpy (self->content_encoding, data_source, string_size);
                self->content_encoding[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing content_encoding field";
    }
    if (property_flags & 1 &lt;&lt; 13) {
        if (data_source &lt;= data_limit - 4) {
            GET_LONG (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "headers field is corrupted";
            else {
                self->headers = icl_longstr_new (data_source, string_size);
                data_source += string_size;
            }
        }
        else
            underflowed = "missing headers field";
    }
    if (property_flags & 1 &lt;&lt; 12) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (self->delivery_mode, data_source);
        }
        else
            underflowed = "missing delivery_mode field";
    }
    if (property_flags & 1 &lt;&lt; 11) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (self->priority, data_source);
        }
        else
            underflowed = "missing priority field";
    }
    if (property_flags & 1 &lt;&lt; 10) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "correlation_id field is corrupted";
            else {
                memcpy (self->correlation_id, data_source, string_size);
                self->correlation_id[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing correlation_id field";
    }
    if (property_flags & 1 &lt;&lt; 9) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "reply_to field is corrupted";
            else {
                memcpy (self->reply_to, data_source, string_size);
                self->reply_to[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing reply_to field";
    }
    if (property_flags & 1 &lt;&lt; 8) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "expiration field is corrupted";
            else {
                memcpy (self->expiration, data_source, string_size);
                self->expiration[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing expiration field";
    }
    if (property_flags & 1 &lt;&lt; 7) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "message_id field is corrupted";
            else {
                memcpy (self->message_id, data_source, string_size);
                self->message_id[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing message_id field";
    }
    if (property_flags & 1 &lt;&lt; 6) {
        if (data_source &lt;= data_limit - 8) {
            GET_LLONG (self->timestamp, data_source);
        }
        else
            underflowed = "missing timestamp field";
    }
    if (property_flags & 1 &lt;&lt; 5) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "type field is corrupted";
            else {
                memcpy (self->type, data_source, string_size);
                self->type[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing type field";
    }
    if (property_flags & 1 &lt;&lt; 4) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "user_id field is corrupted";
            else {
                memcpy (self->user_id, data_source, string_size);
                self->user_id[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing user_id field";
    }
    if (property_flags & 1 &lt;&lt; 3) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "app_id field is corrupted";
            else {
                memcpy (self->app_id, data_source, string_size);
                self->app_id[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing app_id field";
    }
    if (property_flags & 1 &lt;&lt; 2) {
        if (data_source &lt;= data_limit - 1) {
            GET_OCTET (string_size, data_source);
            if (data_source + string_size > data_limit)
                underflowed = "sender_id field is corrupted";
            else {
                memcpy (self->sender_id, data_source, string_size);
                self->sender_id[string_size] = 0;
                data_source += string_size;
            }
        }
        else
            underflowed = "missing sender_id field";
    }
    //  Catch bucket underflow
    if (underflowed) {
        icl_console_print ("E: invalid content header - %s", underflowed);
        rc = -1;
    }
</method>

<method name = "record body" template = "function">
    <doc>
    Records a content body, which is a bucket of binary data.  Use this
    method to set the content body to a block of data coming from the
    network, from a file, or from a pipe.  The bucket must not contain
    any frame control data, (i.e. no frame-end octet).  The caller can
    destroy the bucket after this method, which takes possession of it.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket of raw data</argument>
    //
    if (self->body_data && self->free_fn)
        (self->free_fn) (self->body_data);

    self->body_data = icl_mem_alloc (bucket->cur_size);
    self->body_size = bucket->cur_size;
    self->free_fn   = icl_mem_free;
    memcpy (self->body_data, bucket->data, bucket->cur_size);
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
    if (self->body_data && self->free_fn)
        (self->free_fn) (self->body_data);

    //  Now point to provided body
    self->body_data = data;
    self->body_size = size;
    self->free_fn   = free_fn;
</method>

<!-- Accepts void self_v to allow encapsulation via asl_content_class -->
<method name = "get body" return = "rc">
    <doc>
    Copies the content body to the application into a buffer provided by the
    application.  If the content was larger than the buffer limit, returns -1,
    else returns the number of content octets copied.
    </doc>
    <argument name = "self_v"  type = "void *">Object cast as a void *</argument>
    <argument name = "buffer"  type = "byte *">Buffer for message content</argument>
    <argument name = "maxsize" type = "size_t">Maximum size of buffer</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
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
    </local>
    //
    assert (filename);
    file_stream = fopen (filename, "wb");

    if (self->body_data)
        fwrite (self->body_data, (size_t) self->body_size, 1, file_stream);

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

<method name = "set delivery_mode" template = "function">
    <argument name = "delivery_mode" type = "int">Non-persistent (1) or persistent (2)</argument>
    //
    self->delivery_mode = (byte) delivery_mode;
</method>

<method name = "get delivery_mode" return = "delivery_mode">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "delivery_mode" type = "int" />
    //
    delivery_mode = self->delivery_mode;
</method>

<method name = "set priority" template = "function">
    <argument name = "priority" type = "int">The message priority, 0 to 9</argument>
    //
    self->priority = (byte) priority;
</method>

<method name = "get priority" return = "priority">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "priority" type = "int" />
    //
    priority = self->priority;
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

<method name = "set expiration" template = "function">
    <argument name = "expiration" type = "char *">Message expiration specification</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->expiration, ICL_SHORTSTR_MAX, expiration, args);
</method>

<method name = "get expiration" return = "expiration">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "expiration" type = "char *" />
    //
    expiration = self->expiration;
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

<method name = "set timestamp" template = "function">
    <argument name = "timestamp" type = "int64_t">The message timestamp</argument>
    //
    self->timestamp = timestamp;
</method>

<method name = "get timestamp" return = "timestamp">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "timestamp" type = "int64_t" />
    //
    timestamp = self->timestamp;
</method>

<method name = "set type" template = "function">
    <argument name = "type" type = "char *">The message type name</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->type, ICL_SHORTSTR_MAX, type, args);
</method>

<method name = "get type" return = "type">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "type" type = "char *" />
    //
    type = self->type;
</method>

<method name = "set user_id" template = "function">
    <argument name = "user_id" type = "char *">The creating user id</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->user_id, ICL_SHORTSTR_MAX, user_id, args);
</method>

<method name = "get user_id" return = "user_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "user_id" type = "char *" />
    //
    user_id = self->user_id;
</method>

<method name = "set app_id" template = "function">
    <argument name = "app_id" type = "char *">The creating application id</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->app_id, ICL_SHORTSTR_MAX, app_id, args);
</method>

<method name = "get app_id" return = "app_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "app_id" type = "char *" />
    //
    app_id = self->app_id;
</method>

<method name = "set sender_id" template = "function">
    <argument name = "sender_id" type = "char *">Identifier of connection sending the message</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    apr_vsnprintf (self->sender_id, ICL_SHORTSTR_MAX, sender_id, args);
</method>

<method name = "get sender_id" return = "sender_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "sender_id" type = "char *" />
    //
    sender_id = self->sender_id;
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

<!-- Accepts void self_v to allow encapsulation via asl_content_class -->
<method name = "replay header" return = "bucket">
    <doc>
    Replays the content header, providing a bucket that the caller must
    destroy when finished sending.  For orthogonality with the replay_body
    method, the returned bucket does not contain a frame-end octet and
    the returned bucket cur_size does not account for this octet. The
    supplied bucket data has no frame-end; the caller must add this when
    sending a bucket on the wire.  The frame-end octet, while adding some
    measure of robustness against badly-framed frames, is in itself not
    a construct that leads to highly elegant code.
    </doc>
    <argument name = "self_v" type = "void *">Object cast as a void *</argument>
    <declare name = "bucket" type = "ipr_bucket_t *">Bucket to hold header</declare>
    <local>
    $(selftype)
        *self = self_v;
    size_t
        string_size,
        header_size;
    byte
        *data_target;
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
    if (self->delivery_mode) {
        header_size += 1;
        property_flags |= 1 &lt;&lt; 12;
    }
    if (self->priority) {
        header_size += 1;
        property_flags |= 1 &lt;&lt; 11;
    }
    if (strused (self->correlation_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->correlation_id);
        property_flags |= 1 &lt;&lt; 10;
    }
    if (strused (self->reply_to)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->reply_to);
        property_flags |= 1 &lt;&lt; 9;
    }
    if (strused (self->expiration)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->expiration);
        property_flags |= 1 &lt;&lt; 8;
    }
    if (strused (self->message_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->message_id);
        property_flags |= 1 &lt;&lt; 7;
    }
    if (self->timestamp) {
        header_size += 8;
        property_flags |= 1 &lt;&lt; 6;
    }
    if (strused (self->type)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->type);
        property_flags |= 1 &lt;&lt; 5;
    }
    if (strused (self->user_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->user_id);
        property_flags |= 1 &lt;&lt; 4;
    }
    if (strused (self->app_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->app_id);
        property_flags |= 1 &lt;&lt; 3;
    }
    if (strused (self->sender_id)) {
        header_size += ASL_SSTR_HSIZE + strlen (self->sender_id);
        property_flags |= 1 &lt;&lt; 2;
    }
    bucket = ipr_bucket_new (header_size);
    bucket->cur_size = header_size;
    data_target = bucket->data;
    self->weight = 0;

    PUT_SHORT (data_target, self->class_id);
    PUT_SHORT (data_target, self->weight);
    PUT_LLONG (data_target, self->body_size);
    PUT_SHORT (data_target, property_flags);

    //  Now put content header property fields that are present
    if (strused (self->content_type)) {
        PUT_SSTR  (data_target, self->content_type);
    }
    if (strused (self->content_encoding)) {
        PUT_SSTR  (data_target, self->content_encoding);
    }
    if (self->headers && self->headers->cur_size) {
        PUT_TABLE (data_target, self->headers);
    }
    if (self->delivery_mode) {
        PUT_OCTET (data_target, self->delivery_mode);
    }
    if (self->priority) {
        PUT_OCTET (data_target, self->priority);
    }
    if (strused (self->correlation_id)) {
        PUT_SSTR  (data_target, self->correlation_id);
    }
    if (strused (self->reply_to)) {
        PUT_SSTR  (data_target, self->reply_to);
    }
    if (strused (self->expiration)) {
        PUT_SSTR  (data_target, self->expiration);
    }
    if (strused (self->message_id)) {
        PUT_SSTR  (data_target, self->message_id);
    }
    if (self->timestamp) {
        PUT_LLONG (data_target, self->timestamp);
    }
    if (strused (self->type)) {
        PUT_SSTR  (data_target, self->type);
    }
    if (strused (self->user_id)) {
        PUT_SSTR  (data_target, self->user_id);
    }
    if (strused (self->app_id)) {
        PUT_SSTR  (data_target, self->app_id);
    }
    if (strused (self->sender_id)) {
        PUT_SSTR  (data_target, self->sender_id);
    }
    assert ((size_t) (data_target - bucket->data) == header_size);
</method>

<!-- Accepts void self_v to allow encapsulation via asl_content_class -->
<method name = "set reader" return = "rc">
    <doc>
    Initialises a new reader. The caller should provide a reader structure
    in thread-safe memory, e.g. on the stack.  We use readers to replay a
    content body into buckets - the reader stores all context needed to
    correctly split and reform the content body into buckets of the desired
    size.  Use the set_reader method to initialise a reader, and then the
    replay_body method to replay buckets of body until there are none left.
    </doc>
    <argument name = "self_v" type = "void *">Object cast as a void *</argument>
    <argument name = "reader" type = "asl_reader_t *">Reader to initialise</argument>
    <argument name = "frame max" type = "size_t" >Maximum frame size</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    assert (frame_max >= ASL_FRAME_MIN_SIZE);
    memset (reader, 0, sizeof (*reader));
    reader->frame_max = frame_max;
</method>

<!-- Accepts void self_v to allow encapsulation via asl_content_class -->
<method name = "replay body" return = "bucket">
    <doc>
    Provides a bucket holding the first frame of body data for the
    content.  If the content had no (more) body, returns a null. The
    caller supplies an initialised reader structure.  Call this method
    in a loop with the same reader structure until the method returns
    a null bucket.
    </doc>
    <argument name = "self_v" type = "void *">Object cast as a void *</argument>
    <argument name = "reader" type = "asl_reader_t *">Reader to use</argument>
    <declare name = "bucket" type = "ipr_bucket_t *">Returned bucket, or null</declare>
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
    //
    bucket = NULL;
    if (self->body_data) {
        size_t
            remainder;                  //  Amount of data remaining

        //  Check if there's any body data remaining
        remainder = (size_t) self->body_size - reader->processed;
        if (remainder > 0) {
            bucket = ipr_bucket_new (0);
            bucket->cur_size = min (reader->frame_max, remainder);
            bucket->data     = self->body_data + reader->processed;
            reader->processed += bucket->cur_size;
            reader->count++;
            //  Sanity check
            assert (reader->count < 999999);
        }
    }
</method>

<method name = "selftest">
    <local>
    amq_content_basic_t
        *content;
    icl_longstr_t
        *longstr;
    ipr_bucket_t
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
        count;                          //  Test counter
    </local>

#   define TEST_CHUNK       16000
#   define TEST_DATA_SIZE   TEST_CHUNK*10+1    //  11 buckets

    //  Test 1
    //      - construct content explicitly from body and properties
    //      - replay into header and body buckets
    //
    content = amq_content_basic_new ();
    longstr = icl_longstr_new ("abc", TEST_CHUNK);

    amq_content_basic_set_body (content, "abc", TEST_CHUNK, NULL);
    amq_content_basic_set_content_type (content, "abc");
    amq_content_basic_set_content_encoding (content, "abc");
    amq_content_basic_set_headers (content, longstr);
    amq_content_basic_set_delivery_mode (content, 123);
    amq_content_basic_set_priority (content, 123);
    amq_content_basic_set_correlation_id (content, "abc");
    amq_content_basic_set_reply_to (content, "abc");
    amq_content_basic_set_expiration (content, "abc");
    amq_content_basic_set_message_id (content, "abc");
    amq_content_basic_set_timestamp (content, 123456789L);
    amq_content_basic_set_type (content, "abc");
    amq_content_basic_set_user_id (content, "abc");
    amq_content_basic_set_app_id (content, "abc");
    amq_content_basic_set_sender_id (content, "abc");
    icl_longstr_destroy (&longstr);

    //  We replay the content into a series of buckets
    //  A small content comes out as two buckets - header and body
    //
    amq_content_basic_set_reader (content, &reader, TEST_CHUNK);
    header = amq_content_basic_replay_header (content);
    body   = amq_content_basic_replay_body (content, &reader);
    assert (header);
    assert (body);
    //  Let try it a couple more times...
    assert (amq_content_basic_replay_body (content, &reader) == NULL);
    assert (amq_content_basic_replay_body (content, &reader) == NULL);
    amq_content_basic_unlink (&content);

    //  Test 2
    //      - construct content using header and body buckets
    //      - check that properties are correctly set
    //
    content = amq_content_basic_new ();
    amq_content_basic_record_header (content, header);
    amq_content_basic_record_body  (content, body);
    assert (streq (content->content_type, "abc"));
    assert (streq (content->content_encoding, "abc"));
    assert (content->headers);
    assert (content->headers->cur_size == TEST_CHUNK);
    assert (content->delivery_mode == 123);
    assert (content->priority == 123);
    assert (streq (content->correlation_id, "abc"));
    assert (streq (content->reply_to, "abc"));
    assert (streq (content->expiration, "abc"));
    assert (streq (content->message_id, "abc"));
    assert (content->timestamp == 123456789L);
    assert (streq (content->type, "abc"));
    assert (streq (content->user_id, "abc"));
    assert (streq (content->app_id, "abc"));
    assert (streq (content->sender_id, "abc"));
    amq_content_basic_unlink (&content);

    //  Test 3
    //      - record/replay body of several buckets
    //      - check we can record the same bucket many times
    //
    content = amq_content_basic_new ();
    amq_content_basic_record_header (content, header);
    for (count = 0; count < 10; count++)
        amq_content_basic_record_body (content, body);
    ipr_bucket_unlink (&header);
    ipr_bucket_unlink (&body);

    amq_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = amq_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 10);

    //  Test 4
    //      - replay body into smaller buckets
    //
    amq_content_basic_set_reader (content, &reader, TEST_CHUNK / 2);
    while ((body = amq_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 20);
    amq_content_basic_unlink (&content);

    //  Test 5
    //      - set large content body and replay as buckets
    //      - write the body to a file
    //
    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    memset (test_data, 123, TEST_DATA_SIZE);

    content = amq_content_basic_new ();
    amq_content_basic_set_body (content, test_data, TEST_DATA_SIZE, icl_mem_free);
    amq_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = amq_content_basic_replay_body (content, &reader)))
        ipr_bucket_unlink (&body);
    assert (reader.count == 11);
    amq_content_basic_save_body (content, "amq_content_basic.tst");
    assert (ipr_file_exists ("amq_content_basic.tst"));
    assert (ipr_file_size   ("amq_content_basic.tst") == content->body_size);
    ipr_file_delete ("amq_content_basic.tst");
    amq_content_basic_unlink (&content);

    //  Test 6
    //      - coalesce content into single large buffer
    //      - typically used to return data to application
    //
    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    test_copy = icl_mem_alloc (TEST_DATA_SIZE);
    memset (test_data, 123, TEST_DATA_SIZE);
    memset (test_copy, 0,   TEST_DATA_SIZE);
    processed = 0;

    content = amq_content_basic_new ();
    amq_content_basic_set_body (content, test_data, TEST_DATA_SIZE, icl_mem_free);
    amq_content_basic_set_reader (content, &reader, TEST_CHUNK);
    while ((body = amq_content_basic_replay_body (content, &reader))) {
        memcpy (test_copy + processed, body->data, body->cur_size);
        processed += body->cur_size;
        ipr_bucket_unlink (&body);
    }
    assert (processed == TEST_DATA_SIZE);
    assert (memcmp (test_data, test_copy, TEST_DATA_SIZE) == 0);
    amq_content_basic_unlink (&content);
    icl_mem_free (test_copy);

    //  Test 7
    //      - coalesce content into application buffer
    //      - using get_body

    content = amq_content_basic_new ();
    body = ipr_bucket_new (TEST_CHUNK);
    body->cur_size = TEST_CHUNK;
    memset (body->data, 123, TEST_CHUNK);
    for (count = 0; count < 10; count++)
        amq_content_basic_record_body (content, body);
    ipr_bucket_unlink (&body);

    test_data = icl_mem_alloc (TEST_DATA_SIZE);
    assert (amq_content_basic_get_body (content, test_data, TEST_DATA_SIZE) == TEST_CHUNK * 10);
    amq_content_basic_unlink (&content);
    icl_mem_free (test_data);

    //  Test 8
    //      - empty content
    //
    content = amq_content_basic_new ();
    header = amq_content_basic_replay_header (content);
    amq_content_basic_record_header (content, header);
    ipr_bucket_unlink (&header);
    amq_content_basic_unlink (&content);
</method>

</class>
