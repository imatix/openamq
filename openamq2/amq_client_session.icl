<?xml?>
<!--
    Customised to use ZAMQ interface:
    
    int zamq_open    (char *host);
    int zamq_close   (void);
    int zamq_send    (void *content, size_t length);
    int zamq_receive (void *content, size_t max_length, int block);
    
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
    name      = "amq_client_session"
    comment   = "amq_client connection session class"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class provides the session serialisation API.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_client_classes" />
<import class = "amq_constants" />
<import class = "amq_client_connection" />
<import class = "amq_content_basic_list" />
<public name = "header">
#include "zamqapi.h"
</public>

<context>
    amq_client_connection_t
        *connection;                    //  Parent connection object
    dbyte
        channel_nbr;                    //  Session channel number
    Bool
        silent;                         //  Don't print errors
    Bool
        alive;                          //  Is the session alive?
    char
        *error_text;                    //  Last error cause
    Bool
        active;                         //  start/stop content frames
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
    dbyte
        ticket;                         //  access ticket granted by server
    icl_shortstr_t
        queue;                          //  queue name
    qbyte
        message_count;                  //  number of messages in queue
    qbyte
        consumer_count;                 //  number of consumers
    icl_shortstr_t
        consumer_tag;                   //  consumer tag
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        routing_key;                    //  Message routing key
    int64_t
        delivery_tag;                   //  server-assigned delivery tag
    Bool
        redelivered;                    //  message is being redelivered
    icl_shortstr_t
        sender_id;                      //  Sender id
    icl_shortstr_t
        identifier;                     //  staging identifier
    int64_t
        content_size;                   //  message content size

    //  Handling for basic messages
    ipr_bucket_t
        *bucket;
    amq_content_basic_list_t
        *arrived_basic_list;            //  basic messages
    amq_content_basic_list_t
        *returned_basic_list;           //  Returned basic messages
</context>

<method name = "new">
    <argument name = "connection" type = "amq_client_connection_t *" />
    assert (connection);
//  FIXME
//    assert (connection->alive);

    self->connection   = connection;
    self->channel_nbr  = ++connection->channel_nbr;
    self->error_text   = "no error";
    self->arrived_basic_list = amq_content_basic_list_new ();
    self->returned_basic_list = amq_content_basic_list_new ();
    self->bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    self->alive = 1;
       
    if (!self->alive)
        self_destroy (&self);
</method>

<method name = "destroy">
    ipr_bucket_destroy (&self->bucket);
    amq_content_basic_list_destroy (&self->arrived_basic_list);
    amq_content_basic_list_destroy (&self->returned_basic_list);
</method>

<method name = "wait" template = "function">
    <doc>
    Waits for any activity on the session.  Returns 0 if content arrived,
    -1 if the session ended abnormally.  If the timeout is zero, waits
    forever.
    </doc>
    <argument name = "timeout" type = "int">Timeout, in milliseconds</argument>
    //
    if (!amq_content_basic_list_isempty (self->arrived_basic_list))
        return (0);                    //  Return if there is content to process
    
    //  Get at least one content        
    self->bucket->cur_size = zamq_receive (self->bucket->data, self->bucket->max_size, 1);

    //  Process content and get next if any
    while (self->bucket->cur_size) {
        amq_content_basic_t
            *content = amq_content_basic_new ();
            
        amq_content_basic_record_body (content, self->bucket);
        amq_content_basic_list_queue (self->arrived_basic_list, content);
        amq_content_basic_unlink (&content);
        self->bucket->cur_size = zamq_receive (self->bucket->data, self->bucket->max_size, 0);
    }
</method>

<method name = "channel flow" template = "function">
    <argument name = "active" type = "Bool" pass = "in">start/stop content frames</argument>
</method>

<method name = "channel flow-ok" template = "function">
    <argument name = "active" type = "Bool" pass = "in">current flow setting</argument>
</method>

<method name = "exchange declare" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "type" type = "char *" pass = "in">exchange type</argument>
    <argument name = "passive" type = "Bool" pass = "in">do not create exchange</argument>
    <argument name = "durable" type = "Bool" pass = "in">request a durable exchange</argument>
    <argument name = "auto_delete" type = "Bool" pass = "in">auto-delete when unused</argument>
    <argument name = "internal" type = "Bool" pass = "in">create internal exchange</argument>
    <argument name = "arguments" type = "icl_longstr_t *" pass = "in">arguments for declaration</argument>
</method>

<method name = "exchange delete" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "if_unused" type = "Bool" pass = "in">delete only if unused</argument>
</method>

<method name = "exchange delete nowait" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "if_unused" type = "Bool" pass = "in">delete only if unused</argument>
</method>

<method name = "queue declare" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "passive" type = "Bool" pass = "in">do not create queue</argument>
    <argument name = "durable" type = "Bool" pass = "in">request a durable queue</argument>
    <argument name = "exclusive" type = "Bool" pass = "in">request an exclusive queue</argument>
    <argument name = "auto_delete" type = "Bool" pass = "in">auto-delete queue when unused</argument>
    <argument name = "arguments" type = "icl_longstr_t *" pass = "in">arguments for declaration</argument>
</method>

<method name = "queue bind" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "routing_key" type = "char *" pass = "in">message routing key</argument>
    <argument name = "arguments" type = "icl_longstr_t *" pass = "in">arguments for binding</argument>
</method>

<method name = "queue purge" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
</method>

<method name = "queue delete" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "if_unused" type = "Bool" pass = "in">delete only if unused</argument>
    <argument name = "if_empty" type = "Bool" pass = "in">delete only if empty</argument>
</method>

<method name = "queue unbind" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "routing_key" type = "char *" pass = "in">message routing key</argument>
    <argument name = "arguments" type = "icl_longstr_t *" pass = "in">arguments for binding</argument>
</method>

<method name = "basic consume" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "consumer_tag" type = "char *" pass = "in">consumer tag</argument>
    <argument name = "no_local" type = "Bool" pass = "in">do not deliver own messages</argument>
    <argument name = "no_ack" type = "Bool" pass = "in">no acknowledgement needed</argument>
    <argument name = "exclusive" type = "Bool" pass = "in">request exclusive access</argument>
    <argument name = "arguments" type = "icl_longstr_t *" pass = "in">arguments for consume</argument>
</method>

<method name = "basic cancel" template = "function">
    <argument name = "consumer_tag" type = "char *" pass = "in">consumer tag</argument>
</method>

<method name = "basic publish" template = "function">
    <argument name = "content" type = "amq_content_basic_t *" pass = "in">Message content</argument>
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "exchange" type = "char *" pass = "in">exchange name</argument>
    <argument name = "routing_key" type = "char *" pass = "in">Message routing key</argument>
    <argument name = "mandatory" type = "Bool" pass = "in">indicate mandatory routing</argument>
    <argument name = "immediate" type = "Bool" pass = "in">request immediate delivery</argument>
    //
    //  Get content body into a bucket
    self->bucket->cur_size = amq_content_basic_get_body (
        content, self->bucket->data, self->bucket->max_size);
    
    //  Send to ZAMQ layer
    zamq_send (self->bucket->data, self->bucket->cur_size);
</method>

<method name = "basic get" template = "function">
    <argument name = "ticket" type = "int" pass = "in">access ticket granted by server</argument>
    <argument name = "queue" type = "char *" pass = "in">queue name</argument>
    <argument name = "no_ack" type = "Bool" pass = "in">no acknowledgement needed</argument>
    rc = -1;
</method>

<method name = "basic ack" template = "function">
    <argument name = "delivery_tag" type = "int64_t" pass = "in">server-assigned delivery tag</argument>
    <argument name = "multiple" type = "Bool" pass = "in">acknowledge multiple messages</argument>
</method>

<method name = "basic reject" template = "function">
    <argument name = "delivery_tag" type = "int64_t" pass = "in">server-assigned delivery tag</argument>
    <argument name = "requeue" type = "Bool" pass = "in">requeue the message</argument>
</method>

<method name = "get basic arrived count" template = "function">
    rc = amq_content_basic_list_count (self->arrived_basic_list);
</method>

<method name = "basic arrived" return = "content">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <declare name = "content" type = "amq_content_basic_t *" default = "NULL">Message content</declare>
    content = amq_content_basic_list_pop (self->arrived_basic_list);
</method>

<method name = "get basic returned count" template = "function">
    rc = amq_content_basic_list_count (self->returned_basic_list);
</method>

<method name = "basic returned" return = "content">
    <argument name = "self"  type = "$(selftype) *">Session object</argument>
    <declare name = "content" type = "amq_content_basic_t *" default = "NULL">Message content</declare>
    content = amq_content_basic_list_pop (self->returned_basic_list);                                
</method>

<method name = "get silent" return = "silent">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "silent" type = "Bool" />
    silent = self->silent;
</method>

<method name = "get alive" return = "alive">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "alive" type = "Bool" />
    alive = self->alive;
</method>

<method name = "get error text" return = "error text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "error text" type = "char *" />
    error_text = self->error_text;
</method>

<method name = "get active" return = "active">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "active" type = "Bool" />
    active = self->active;
</method>

<method name = "get reply_code" return = "reply_code">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_code" type = "int" />
    reply_code = self->reply_code;
</method>

<method name = "get reply_text" return = "reply_text">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "reply_text" type = "char *" />
    reply_text = self->reply_text;
</method>

<method name = "get class_id" return = "class_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "class_id" type = "int" />
    class_id = self->class_id;
</method>

<method name = "get method_id" return = "method_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "method_id" type = "int" />
    method_id = self->method_id;
</method>

<method name = "get ticket" return = "ticket">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "ticket" type = "int" />
    ticket = self->ticket;
</method>

<method name = "get queue" return = "queue">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "queue" type = "char *" />
    queue = self->queue;
</method>

<method name = "get message_count" return = "message_count">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "message_count" type = "qbyte" />
    message_count = self->message_count;
</method>

<method name = "get consumer_count" return = "consumer_count">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "consumer_count" type = "qbyte" />
    consumer_count = self->consumer_count;
</method>

<method name = "get consumer_tag" return = "consumer_tag">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "consumer_tag" type = "char *" />
    consumer_tag = self->consumer_tag;
</method>

<method name = "get exchange" return = "exchange">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "exchange" type = "char *" />
    exchange = self->exchange;
</method>

<method name = "get routing_key" return = "routing_key">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "routing_key" type = "char *" />
    routing_key = self->routing_key;
</method>

<method name = "get delivery_tag" return = "delivery_tag">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "delivery_tag" type = "int64_t" />
    delivery_tag = self->delivery_tag;
</method>

<method name = "get redelivered" return = "redelivered">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "redelivered" type = "Bool" />
    redelivered = self->redelivered;
</method>

<method name = "get sender_id" return = "sender_id">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "sender_id" type = "char *" />
    sender_id = self->sender_id;
</method>

<method name = "get identifier" return = "identifier">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "identifier" type = "char *" />
    identifier = self->identifier;
</method>

<method name = "get content_size" return = "content_size">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "content_size" type = "int64_t" />
    content_size = self->content_size;
</method>

<method name = "selftest">
</method>

</class>
