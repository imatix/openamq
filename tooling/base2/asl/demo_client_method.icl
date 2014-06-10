<?xml?>
<!--
    *** GENERATED FROM demo_client_proto.asl BY ASL_GEN USING GSL/4 ***

    
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
    name      = "demo_client_method"
    comment   = "Protocol method for client chassis"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class holds a method.  All protocol methods are held in a single
union that can be processed as a single entity.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"   value = "cache" />
    <option name = "nullify" value = "0" />
    <option name = "links"   value = "1" />
</inherit>
<inherit class = "icl_init" />

<import class = "asl" />
<import class = "asl_field" />
<import class = "smt_log" />
<import class = "demo_constants" />
<import class = "demo_client_agent" />
<import class = "demo_content_basic"/>

<context>
    dbyte
        class_id,                       //  ID of current class
        method_id;                      //  ID of current method
    void
        *content;                       //  Content, if any
    char
        *name;                          //  Name, for tracing
    Bool
        sync;                           //  Method is synchronous?

    //  Used internally by servers processing the content
    qbyte
        sequence;                       //  Method sequence counter

    union {
        demo_client_connection_start_t
            connection_start;
        demo_client_connection_start_ok_t
            connection_start_ok;
        demo_client_connection_secure_t
            connection_secure;
        demo_client_connection_secure_ok_t
            connection_secure_ok;
        demo_client_connection_tune_t
            connection_tune;
        demo_client_connection_tune_ok_t
            connection_tune_ok;
        demo_client_connection_open_t
            connection_open;
        demo_client_connection_open_ok_t
            connection_open_ok;
        demo_client_connection_close_t
            connection_close;
        demo_client_connection_close_ok_t
            connection_close_ok;
        demo_client_channel_open_t
            channel_open;
        demo_client_channel_open_ok_t
            channel_open_ok;
        demo_client_channel_flow_t
            channel_flow;
        demo_client_channel_flow_ok_t
            channel_flow_ok;
        demo_client_channel_close_t
            channel_close;
        demo_client_channel_close_ok_t
            channel_close_ok;
        demo_client_exchange_declare_t
            exchange_declare;
        demo_client_exchange_declare_ok_t
            exchange_declare_ok;
        demo_client_exchange_delete_t
            exchange_delete;
        demo_client_exchange_delete_ok_t
            exchange_delete_ok;
        demo_client_queue_declare_t
            queue_declare;
        demo_client_queue_declare_ok_t
            queue_declare_ok;
        demo_client_queue_bind_t
            queue_bind;
        demo_client_queue_bind_ok_t
            queue_bind_ok;
        demo_client_queue_purge_t
            queue_purge;
        demo_client_queue_purge_ok_t
            queue_purge_ok;
        demo_client_queue_delete_t
            queue_delete;
        demo_client_queue_delete_ok_t
            queue_delete_ok;
        demo_client_basic_consume_t
            basic_consume;
        demo_client_basic_consume_ok_t
            basic_consume_ok;
        demo_client_basic_cancel_t
            basic_cancel;
        demo_client_basic_cancel_ok_t
            basic_cancel_ok;
        demo_client_basic_publish_t
            basic_publish;
        demo_client_basic_deliver_t
            basic_deliver;
        demo_client_basic_get_t
            basic_get;
        demo_client_basic_get_ok_t
            basic_get_ok;
        demo_client_basic_get_empty_t
            basic_get_empty;
        demo_client_direct_put_t
            direct_put;
        demo_client_direct_put_ok_t
            direct_put_ok;
        demo_client_direct_get_t
            direct_get;
        demo_client_direct_get_ok_t
            direct_get_ok;
    } payload;
</context>

<public name = "header">
#define DEMO_CLIENT_CONNECTION              10
#define DEMO_CLIENT_CONNECTION_START        10
#define DEMO_CLIENT_CONNECTION_START_OK     11
#define DEMO_CLIENT_CONNECTION_SECURE       20
#define DEMO_CLIENT_CONNECTION_SECURE_OK    21
#define DEMO_CLIENT_CONNECTION_TUNE         30
#define DEMO_CLIENT_CONNECTION_TUNE_OK      31
#define DEMO_CLIENT_CONNECTION_OPEN         40
#define DEMO_CLIENT_CONNECTION_OPEN_OK      41
#define DEMO_CLIENT_CONNECTION_CLOSE        50
#define DEMO_CLIENT_CONNECTION_CLOSE_OK     51

#define DEMO_CLIENT_CHANNEL                 20
#define DEMO_CLIENT_CHANNEL_OPEN            10
#define DEMO_CLIENT_CHANNEL_OPEN_OK         11
#define DEMO_CLIENT_CHANNEL_FLOW            20
#define DEMO_CLIENT_CHANNEL_FLOW_OK         21
#define DEMO_CLIENT_CHANNEL_CLOSE           40
#define DEMO_CLIENT_CHANNEL_CLOSE_OK        41

#define DEMO_CLIENT_EXCHANGE                40
#define DEMO_CLIENT_EXCHANGE_DECLARE        10
#define DEMO_CLIENT_EXCHANGE_DECLARE_OK     11
#define DEMO_CLIENT_EXCHANGE_DELETE         20
#define DEMO_CLIENT_EXCHANGE_DELETE_OK      21

#define DEMO_CLIENT_QUEUE                   50
#define DEMO_CLIENT_QUEUE_DECLARE           10
#define DEMO_CLIENT_QUEUE_DECLARE_OK        11
#define DEMO_CLIENT_QUEUE_BIND              20
#define DEMO_CLIENT_QUEUE_BIND_OK           21
#define DEMO_CLIENT_QUEUE_PURGE             30
#define DEMO_CLIENT_QUEUE_PURGE_OK          31
#define DEMO_CLIENT_QUEUE_DELETE            40
#define DEMO_CLIENT_QUEUE_DELETE_OK         41

#define DEMO_CLIENT_BASIC                   60
#define DEMO_CLIENT_BASIC_CONSUME           10
#define DEMO_CLIENT_BASIC_CONSUME_OK        11
#define DEMO_CLIENT_BASIC_CANCEL            20
#define DEMO_CLIENT_BASIC_CANCEL_OK         21
#define DEMO_CLIENT_BASIC_PUBLISH           30
#define DEMO_CLIENT_BASIC_DELIVER           40
#define DEMO_CLIENT_BASIC_GET               50
#define DEMO_CLIENT_BASIC_GET_OK            51
#define DEMO_CLIENT_BASIC_GET_EMPTY         52

#define DEMO_CLIENT_DIRECT                  61500
#define DEMO_CLIENT_DIRECT_PUT              10
#define DEMO_CLIENT_DIRECT_PUT_OK           11
#define DEMO_CLIENT_DIRECT_GET              20
#define DEMO_CLIENT_DIRECT_GET_OK           21

typedef struct _demo_client_connection_start_t demo_client_connection_start_t;
typedef struct _demo_client_connection_start_ok_t demo_client_connection_start_ok_t;
typedef struct _demo_client_connection_secure_t demo_client_connection_secure_t;
typedef struct _demo_client_connection_secure_ok_t demo_client_connection_secure_ok_t;
typedef struct _demo_client_connection_tune_t demo_client_connection_tune_t;
typedef struct _demo_client_connection_tune_ok_t demo_client_connection_tune_ok_t;
typedef struct _demo_client_connection_open_t demo_client_connection_open_t;
typedef struct _demo_client_connection_open_ok_t demo_client_connection_open_ok_t;
typedef struct _demo_client_connection_close_t demo_client_connection_close_t;
typedef struct _demo_client_connection_close_ok_t demo_client_connection_close_ok_t;
typedef struct _demo_client_channel_open_t demo_client_channel_open_t;
typedef struct _demo_client_channel_open_ok_t demo_client_channel_open_ok_t;
typedef struct _demo_client_channel_flow_t demo_client_channel_flow_t;
typedef struct _demo_client_channel_flow_ok_t demo_client_channel_flow_ok_t;
typedef struct _demo_client_channel_close_t demo_client_channel_close_t;
typedef struct _demo_client_channel_close_ok_t demo_client_channel_close_ok_t;
typedef struct _demo_client_exchange_declare_t demo_client_exchange_declare_t;
typedef struct _demo_client_exchange_declare_ok_t demo_client_exchange_declare_ok_t;
typedef struct _demo_client_exchange_delete_t demo_client_exchange_delete_t;
typedef struct _demo_client_exchange_delete_ok_t demo_client_exchange_delete_ok_t;
typedef struct _demo_client_queue_declare_t demo_client_queue_declare_t;
typedef struct _demo_client_queue_declare_ok_t demo_client_queue_declare_ok_t;
typedef struct _demo_client_queue_bind_t demo_client_queue_bind_t;
typedef struct _demo_client_queue_bind_ok_t demo_client_queue_bind_ok_t;
typedef struct _demo_client_queue_purge_t demo_client_queue_purge_t;
typedef struct _demo_client_queue_purge_ok_t demo_client_queue_purge_ok_t;
typedef struct _demo_client_queue_delete_t demo_client_queue_delete_t;
typedef struct _demo_client_queue_delete_ok_t demo_client_queue_delete_ok_t;
typedef struct _demo_client_basic_consume_t demo_client_basic_consume_t;
typedef struct _demo_client_basic_consume_ok_t demo_client_basic_consume_ok_t;
typedef struct _demo_client_basic_cancel_t demo_client_basic_cancel_t;
typedef struct _demo_client_basic_cancel_ok_t demo_client_basic_cancel_ok_t;
typedef struct _demo_client_basic_publish_t demo_client_basic_publish_t;
typedef struct _demo_client_basic_deliver_t demo_client_basic_deliver_t;
typedef struct _demo_client_basic_get_t demo_client_basic_get_t;
typedef struct _demo_client_basic_get_ok_t demo_client_basic_get_ok_t;
typedef struct _demo_client_basic_get_empty_t demo_client_basic_get_empty_t;
typedef struct _demo_client_direct_put_t demo_client_direct_put_t;
typedef struct _demo_client_direct_put_ok_t demo_client_direct_put_ok_t;
typedef struct _demo_client_direct_get_t demo_client_direct_get_t;
typedef struct _demo_client_direct_get_ok_t demo_client_direct_get_ok_t;
</public>

<public>
//  Start Connection Negotiation

struct _demo_client_connection_start_t {
    byte
        version_major;                  //  protocol major version
    byte
        version_minor;                  //  protocol major version
    icl_longstr_t *
        server_properties;              //  server properties
    icl_longstr_t *
        mechanisms;                     //  available security mechanisms
    icl_longstr_t *
        locales;                        //  available message locales
};

//  Select Security Mechanism And Locale

struct _demo_client_connection_start_ok_t {
    icl_longstr_t *
        client_properties;              //  client properties
    icl_shortstr_t
        mechanism;                      //  selected security mechanism
    icl_longstr_t *
        response;                       //  security response data
    icl_shortstr_t
        locale;                         //  selected message locale
};

//  Security Mechanism Challenge

struct _demo_client_connection_secure_t {
    icl_longstr_t *
        challenge;                      //  security challenge data
};

//  Security Mechanism Response

struct _demo_client_connection_secure_ok_t {
    icl_longstr_t *
        response;                       //  security response data
};

//  Propose Connection Tuning Parameters

struct _demo_client_connection_tune_t {
    dbyte
        channel_max;                    //  proposed maximum channels
    qbyte
        frame_max;                      //  proposed maximum frame size
    dbyte
        heartbeat;                      //  desired heartbeat delay
};

//  Negotiate Connection Tuning Parameters

struct _demo_client_connection_tune_ok_t {
    dbyte
        channel_max;                    //  negotiated maximum channels
    qbyte
        frame_max;                      //  negotiated maximum frame size
    dbyte
        heartbeat;                      //  desired heartbeat delay
};

//  Open Connection To Virtual Host

struct _demo_client_connection_open_t {
    icl_shortstr_t
        virtual_host;                   //  virtual host name
    icl_shortstr_t
        capabilities;                   //  required capabilities
    Bool
        insist;                         //  insist on connecting to server
};

//  Signal That The Connection Is Ready

struct _demo_client_connection_open_ok_t {
    icl_shortstr_t
        known_hosts;                    //  list of known hosts
};

//  Request A Connection Close

struct _demo_client_connection_close_t {
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
};

//  Confirm A Connection Close

struct _demo_client_connection_close_ok_t {
    int
        filler;                         //  No method-specific fields          
};

//  Open A Channel For Use

struct _demo_client_channel_open_t {
    icl_shortstr_t
        out_of_band;                    //  out-of-band settings
};

//  Signal That The Channel Is Ready

struct _demo_client_channel_open_ok_t {
    icl_longstr_t *
        channel_id;                     //  unique id of the channel
};

//  Enable/Disable Flow From Peer

struct _demo_client_channel_flow_t {
    Bool
        active;                         //  start/stop content frames
};

//  Confirm A Flow Method

struct _demo_client_channel_flow_ok_t {
    Bool
        active;                         //  current flow setting
};

//  Request A Channel Close

struct _demo_client_channel_close_t {
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
};

//  Confirm A Channel Close

struct _demo_client_channel_close_ok_t {
    int
        filler;                         //  No method-specific fields          
};

//  Declare Exchange, Create If Needed

struct _demo_client_exchange_declare_t {
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        type;                           //  exchange class
    Bool
        passive;                        //  do not create exchange
    Bool
        durable;                        //  request a durable exchange
    Bool
        auto_delete;                    //  auto-delete when unused
};

//  Confirms An Exchange Declaration

struct _demo_client_exchange_declare_ok_t {
    int
        filler;                         //  No method-specific fields          
};

//  Delete An Exchange

struct _demo_client_exchange_delete_t {
    icl_shortstr_t
        exchange;                       //  exchange name
    Bool
        if_unused;                      //  delete only if unused
};

//  Confirm Deletion Of An Exchange

struct _demo_client_exchange_delete_ok_t {
    int
        filler;                         //  No method-specific fields          
};

//  Declare Queue, Create If Needed

struct _demo_client_queue_declare_t {
    icl_shortstr_t
        queue;                          //  queue name
    Bool
        passive;                        //  do not create queue
    Bool
        durable;                        //  request a durable queue
    Bool
        exclusive;                      //  request an exclusive queue
    Bool
        auto_delete;                    //  auto-delete queue when unused
};

//  Confirms A Queue Definition

struct _demo_client_queue_declare_ok_t {
    icl_shortstr_t
        queue;                          //  queue name
    icl_longstr_t *
        properties;                     //  queue properties
};

//  Bind Queue To An Exchange

struct _demo_client_queue_bind_t {
    icl_shortstr_t
        queue;                          //  queue name
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_longstr_t *
        arguments;                      //  arguments for binding
};

//  Confirm Bind Successful

struct _demo_client_queue_bind_ok_t {
    int
        filler;                         //  No method-specific fields          
};

//  Purge A Queue

struct _demo_client_queue_purge_t {
    icl_shortstr_t
        queue;                          //  queue name
};

//  Confirms A Queue Purge

struct _demo_client_queue_purge_ok_t {
    qbyte
        message_count;                  //  number of messages purged
};

//  Delete A Queue

struct _demo_client_queue_delete_t {
    icl_shortstr_t
        queue;                          //  queue name
    Bool
        if_unused;                      //  delete only if unused
    Bool
        if_empty;                       //  delete only if empty
};

//  Confirm Deletion Of A Queue

struct _demo_client_queue_delete_ok_t {
    qbyte
        message_count;                  //  number of messages purged
};

//  Start A Queue Consumer

struct _demo_client_basic_consume_t {
    icl_shortstr_t
        queue;                          //  queue name
};

//  Confirm A New Consumer

struct _demo_client_basic_consume_ok_t {
    icl_shortstr_t
        consumer_tag;                   //  consumer tag
};

//  End A Queue Consumer

struct _demo_client_basic_cancel_t {
    icl_shortstr_t
        consumer_tag;                   //  consumer tag
};

//  Confirm A Cancelled Consumer

struct _demo_client_basic_cancel_ok_t {
    icl_shortstr_t
        consumer_tag;                   //  consumer tag
};

//  Publish A Message

struct _demo_client_basic_publish_t {
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        routing_key;                    //  Message routing key
};

//  Notify The Client Of A Consumer Message

struct _demo_client_basic_deliver_t {
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        routing_key;                    //  Message routing key
    int64_t
        delivery_tag;                   //  server-assigned delivery tag
};

//  Direct Access To A Queue

struct _demo_client_basic_get_t {
    icl_shortstr_t
        queue;                          //  queue name
};

//  Provide Client With A Message

struct _demo_client_basic_get_ok_t {
    icl_shortstr_t
        exchange;                       //  exchange name
    icl_shortstr_t
        routing_key;                    //  Message routing key
    qbyte
        message_count;                  //  number of messages pending
};

//  Indicate No Messages Available

struct _demo_client_basic_get_empty_t {
    int
        filler;                         //  No method-specific fields          
};

//  Request A Connection To A Sink

struct _demo_client_direct_put_t {
    icl_shortstr_t
        sink;                           //  Sink name
};

//  Grant Access To The Sink

struct _demo_client_direct_put_ok_t {
    icl_shortstr_t
        lease;                          //  Lease for connection
};

//  Request A Connection To A Feed

struct _demo_client_direct_get_t {
    icl_shortstr_t
        feed;                           //  Feed name
};

//  Grant Access To The Feed

struct _demo_client_direct_get_ok_t {
    icl_shortstr_t
        lease;                          //  Lease for connection
};

</public>

<private>
static ipr_regexp_t
    *s_regexp_connection_virtual_host = NULL;
static ipr_regexp_t
    *s_regexp_exchange_exchange = NULL;
static ipr_regexp_t
    *s_regexp_exchange_type = NULL;
static ipr_regexp_t
    *s_regexp_queue_queue = NULL;
</private>

<method name = "new">
    self->class_id  = 0;
    self->method_id = 0;
    self->name      = NULL;
    self->sync      = FALSE;
    self->content   = NULL;
</method>

<method name = "destroy">
    if (self->class_id == DEMO_CLIENT_CONNECTION) {
        if (self->method_id == DEMO_CLIENT_CONNECTION_START) {
            icl_longstr_destroy (&self->payload.connection_start.server_properties);
            icl_longstr_destroy (&self->payload.connection_start.mechanisms);
            icl_longstr_destroy (&self->payload.connection_start.locales);
        }
        else
        if (self->method_id == DEMO_CLIENT_CONNECTION_START_OK) {
            icl_longstr_destroy (&self->payload.connection_start_ok.client_properties);
            icl_longstr_destroy (&self->payload.connection_start_ok.response);
        }
        else
        if (self->method_id == DEMO_CLIENT_CONNECTION_SECURE) {
            icl_longstr_destroy (&self->payload.connection_secure.challenge);
        }
        else
        if (self->method_id == DEMO_CLIENT_CONNECTION_SECURE_OK) {
            icl_longstr_destroy (&self->payload.connection_secure_ok.response);
        }
    }
    else
    if (self->class_id == DEMO_CLIENT_CHANNEL) {
        if (self->method_id == DEMO_CLIENT_CHANNEL_OPEN_OK) {
            icl_longstr_destroy (&self->payload.channel_open_ok.channel_id);
        }
    }
    else
    if (self->class_id == DEMO_CLIENT_QUEUE) {
        if (self->method_id == DEMO_CLIENT_QUEUE_DECLARE_OK) {
            icl_longstr_destroy (&self->payload.queue_declare_ok.properties);
        }
        else
        if (self->method_id == DEMO_CLIENT_QUEUE_BIND) {
            icl_longstr_destroy (&self->payload.queue_bind.arguments);
        }
    }
    else
    if (self->class_id == DEMO_CLIENT_BASIC) {
        demo_content_basic_t
            *content = (demo_content_basic_t *) self->content;
        demo_content_basic_unlink (&content);
    }
    else
    assert (!self->content);            //  Non-destroyed content = error
</method>

<method name = "initialise">
    s_regexp_connection_virtual_host = ipr_regexp_new ("^[a-zA-Z0-9/-_]+$");
    s_regexp_exchange_exchange = ipr_regexp_new ("^[a-zA-Z0-9-_]+$");
    s_regexp_exchange_type = ipr_regexp_new ("^[a-zA-Z0-9-_]+$");
    s_regexp_queue_queue = ipr_regexp_new ("^[a-zA-Z0-9-_]+$");
</method>

<method name = "terminate">
    ipr_regexp_destroy (&s_regexp_connection_virtual_host);
    ipr_regexp_destroy (&s_regexp_exchange_exchange);
    ipr_regexp_destroy (&s_regexp_exchange_type);
    ipr_regexp_destroy (&s_regexp_queue_queue);
</method>

<method name = "new connection start-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "client_properties"  type = "icl_longstr_t *">client properties</argument>
    <argument name = "mechanism"     type = "char *">selected security mechanism</argument>
    <argument name = "response"      type = "icl_longstr_t *">security response data</argument>
    <argument name = "locale"        type = "char *">selected message locale</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_start_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_start_ok;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_START_OK;
    self->name      = "connection.start_ok";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->mechanism,     mechanism);
    icl_shortstr_cpy (method->locale,        locale);
    if (client_properties)
        method->client_properties  = icl_longstr_dup (client_properties);
    else
        method->client_properties  = NULL;
    if (response)
        method->response    = icl_longstr_dup (response);
    else
        method->response    = NULL;
</method>

<method name = "new connection secure-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "response"      type = "icl_longstr_t *">security response data</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_secure_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_secure_ok;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_SECURE_OK;
    self->name      = "connection.secure_ok";

    //  Copy supplied values to method object
    if (response)
        method->response    = icl_longstr_dup (response);
    else
        method->response    = NULL;
</method>

<method name = "new connection tune-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "channel_max"   type = "int">negotiated maximum channels</argument>
    <argument name = "frame_max"     type = "qbyte">negotiated maximum frame size</argument>
    <argument name = "heartbeat"     type = "int">desired heartbeat delay</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_tune_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_tune_ok;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_TUNE_OK;
    self->name      = "connection.tune_ok";

    //  Copy supplied values to method object
    method->channel_max     = (dbyte) channel_max;
    method->frame_max       = frame_max;
    method->heartbeat       = (dbyte) heartbeat;
</method>

<method name = "new connection open" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "virtual_host"  type = "char *">virtual host name</argument>
    <argument name = "capabilities"  type = "char *">required capabilities</argument>
    <argument name = "insist"        type = "Bool">insist on connecting to server</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_open_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_open;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_OPEN;
    self->name      = "connection.open";

    //  Copy supplied values to method object
    method->insist          = insist;
    icl_shortstr_cpy (method->virtual_host,  virtual_host);
    icl_shortstr_cpy (method->capabilities,  capabilities);
</method>

<method name = "new connection close" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "reply_code"    type = "int">reply code from server</argument>
    <argument name = "reply_text"    type = "char *">localised reply text</argument>
    <argument name = "class_id"      type = "int">failing method class</argument>
    <argument name = "method_id"     type = "int">failing method ID</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_close_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_close;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_CLOSE;
    self->name      = "connection.close";

    //  Copy supplied values to method object
    method->reply_code      = (dbyte) reply_code;
    method->class_id        = (dbyte) class_id;
    method->method_id       = (dbyte) method_id;
    icl_shortstr_cpy (method->reply_text,    reply_text);
</method>

<method name = "new connection close-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_connection_close_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.connection_close_ok;
    self->class_id  = DEMO_CLIENT_CONNECTION;
    self->method_id = DEMO_CLIENT_CONNECTION_CLOSE_OK;
    self->name      = "connection.close_ok";

    //  Copy supplied values to method object
</method>

<method name = "new channel open" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "out_of_band"   type = "char *">out-of-band settings</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_channel_open_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.channel_open;
    self->class_id  = DEMO_CLIENT_CHANNEL;
    self->method_id = DEMO_CLIENT_CHANNEL_OPEN;
    self->name      = "channel.open";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->out_of_band,   out_of_band);
</method>

<method name = "new channel flow" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "active"        type = "Bool">start/stop content frames</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_channel_flow_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.channel_flow;
    self->class_id  = DEMO_CLIENT_CHANNEL;
    self->method_id = DEMO_CLIENT_CHANNEL_FLOW;
    self->name      = "channel.flow";

    //  Copy supplied values to method object
    method->active          = active;
</method>

<method name = "new channel flow-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "active"        type = "Bool">current flow setting</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_channel_flow_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.channel_flow_ok;
    self->class_id  = DEMO_CLIENT_CHANNEL;
    self->method_id = DEMO_CLIENT_CHANNEL_FLOW_OK;
    self->name      = "channel.flow_ok";

    //  Copy supplied values to method object
    method->active          = active;
</method>

<method name = "new channel close" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "reply_code"    type = "int">reply code from server</argument>
    <argument name = "reply_text"    type = "char *">localised reply text</argument>
    <argument name = "class_id"      type = "int">failing method class</argument>
    <argument name = "method_id"     type = "int">failing method ID</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_channel_close_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.channel_close;
    self->class_id  = DEMO_CLIENT_CHANNEL;
    self->method_id = DEMO_CLIENT_CHANNEL_CLOSE;
    self->name      = "channel.close";

    //  Copy supplied values to method object
    method->reply_code      = (dbyte) reply_code;
    method->class_id        = (dbyte) class_id;
    method->method_id       = (dbyte) method_id;
    icl_shortstr_cpy (method->reply_text,    reply_text);
</method>

<method name = "new channel close-ok" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_channel_close_ok_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.channel_close_ok;
    self->class_id  = DEMO_CLIENT_CHANNEL;
    self->method_id = DEMO_CLIENT_CHANNEL_CLOSE_OK;
    self->name      = "channel.close_ok";

    //  Copy supplied values to method object
</method>

<method name = "new exchange declare" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "exchange"      type = "char *">exchange name</argument>
    <argument name = "type"          type = "char *">exchange class</argument>
    <argument name = "passive"       type = "Bool">do not create exchange</argument>
    <argument name = "durable"       type = "Bool">request a durable exchange</argument>
    <argument name = "auto_delete"   type = "Bool">auto-delete when unused</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_exchange_declare_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.exchange_declare;
    self->class_id  = DEMO_CLIENT_EXCHANGE;
    self->method_id = DEMO_CLIENT_EXCHANGE_DECLARE;
    self->name      = "exchange.declare";

    //  Copy supplied values to method object
    method->passive         = passive;
    method->durable         = durable;
    method->auto_delete     = auto_delete;
    icl_shortstr_cpy (method->exchange,      exchange);
    icl_shortstr_cpy (method->type,          type);
</method>

<method name = "new exchange delete" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "exchange"      type = "char *">exchange name</argument>
    <argument name = "if_unused"     type = "Bool">delete only if unused</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_exchange_delete_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.exchange_delete;
    self->class_id  = DEMO_CLIENT_EXCHANGE;
    self->method_id = DEMO_CLIENT_EXCHANGE_DELETE;
    self->name      = "exchange.delete";

    //  Copy supplied values to method object
    method->if_unused       = if_unused;
    icl_shortstr_cpy (method->exchange,      exchange);
</method>

<method name = "new queue declare" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <argument name = "passive"       type = "Bool">do not create queue</argument>
    <argument name = "durable"       type = "Bool">request a durable queue</argument>
    <argument name = "exclusive"     type = "Bool">request an exclusive queue</argument>
    <argument name = "auto_delete"   type = "Bool">auto-delete queue when unused</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_queue_declare_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.queue_declare;
    self->class_id  = DEMO_CLIENT_QUEUE;
    self->method_id = DEMO_CLIENT_QUEUE_DECLARE;
    self->name      = "queue.declare";

    //  Copy supplied values to method object
    method->passive         = passive;
    method->durable         = durable;
    method->exclusive       = exclusive;
    method->auto_delete     = auto_delete;
    icl_shortstr_cpy (method->queue,         queue);
</method>

<method name = "new queue bind" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <argument name = "exchange"      type = "char *">exchange name</argument>
    <argument name = "arguments"     type = "icl_longstr_t *">arguments for binding</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_queue_bind_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.queue_bind;
    self->class_id  = DEMO_CLIENT_QUEUE;
    self->method_id = DEMO_CLIENT_QUEUE_BIND;
    self->name      = "queue.bind";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->queue,         queue);
    icl_shortstr_cpy (method->exchange,      exchange);
    if (arguments)
        method->arguments   = icl_longstr_dup (arguments);
    else
        method->arguments   = NULL;
</method>

<method name = "new queue purge" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_queue_purge_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.queue_purge;
    self->class_id  = DEMO_CLIENT_QUEUE;
    self->method_id = DEMO_CLIENT_QUEUE_PURGE;
    self->name      = "queue.purge";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->queue,         queue);
</method>

<method name = "new queue delete" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <argument name = "if_unused"     type = "Bool">delete only if unused</argument>
    <argument name = "if_empty"      type = "Bool">delete only if empty</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_queue_delete_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.queue_delete;
    self->class_id  = DEMO_CLIENT_QUEUE;
    self->method_id = DEMO_CLIENT_QUEUE_DELETE;
    self->name      = "queue.delete";

    //  Copy supplied values to method object
    method->if_unused       = if_unused;
    method->if_empty        = if_empty;
    icl_shortstr_cpy (method->queue,         queue);
</method>

<method name = "new basic consume" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_basic_consume_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.basic_consume;
    self->class_id  = DEMO_CLIENT_BASIC;
    self->method_id = DEMO_CLIENT_BASIC_CONSUME;
    self->name      = "basic.consume";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->queue,         queue);
</method>

<method name = "new basic cancel" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "consumer_tag"  type = "char *">consumer tag</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_basic_cancel_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.basic_cancel;
    self->class_id  = DEMO_CLIENT_BASIC;
    self->method_id = DEMO_CLIENT_BASIC_CANCEL;
    self->name      = "basic.cancel";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->consumer_tag,  consumer_tag);
</method>

<method name = "new basic publish" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "exchange"      type = "char *">exchange name</argument>
    <argument name = "routing_key"   type = "char *">Message routing key</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_basic_publish_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.basic_publish;
    self->class_id  = DEMO_CLIENT_BASIC;
    self->method_id = DEMO_CLIENT_BASIC_PUBLISH;
    self->name      = "basic.publish";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->exchange,      exchange);
    icl_shortstr_cpy (method->routing_key,   routing_key);
</method>

<method name = "new basic get" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "queue"         type = "char *">queue name</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_basic_get_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.basic_get;
    self->class_id  = DEMO_CLIENT_BASIC;
    self->method_id = DEMO_CLIENT_BASIC_GET;
    self->name      = "basic.get";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->queue,         queue);
</method>

<method name = "new direct put" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "sink"          type = "char *">Sink name</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_direct_put_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.direct_put;
    self->class_id  = DEMO_CLIENT_DIRECT;
    self->method_id = DEMO_CLIENT_DIRECT_PUT;
    self->name      = "direct.put";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->sink,          sink);
</method>

<method name = "new direct get" return = "self">
    <doc>
    Creates a new demo_client_method_t object as specified.
    Returns a pointer to the new method object, or NULL if there was an
    error allocating memory or parsing the arguments.
    </doc>
    <argument name = "feed"          type = "char *">Feed name</argument>
    <declare name = "self" type = "$(selftype) *">New method</declare>
    <local>
    demo_client_direct_get_t
        *method;
    </local>
    //
    self   = self_new ();
    method = &self->payload.direct_get;
    self->class_id  = DEMO_CLIENT_DIRECT;
    self->method_id = DEMO_CLIENT_DIRECT_GET;
    self->name      = "direct.get";

    //  Copy supplied values to method object
    icl_shortstr_cpy (method->feed,          feed);
</method>

<method name = "encode" return = "bucket">
    <doc>
    Encodes a demo_client_method_t object into a memory bucket.
    Allocates a fresh bucket and returns this.  Returns NULL if there
    was an error encoding the method object.  The supplied bucket ends
    in a frame-end octet, for symmetry with the decode method.
    </doc>
    <argument name = "self" type = "$(selftype) *">Method reference</argument>
    <declare name = "bucket" type = "ipr_bucket_t *" default = "NULL">New bucket</declare>
    <local>
    byte
        *data_target = NULL;
    size_t
        string_size = 0;
    byte
        bit_string = 0;
    </local>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    assert (self->class_id);
    assert (self->method_id);

    switch (self->class_id) {
        case DEMO_CLIENT_CONNECTION:
            switch (self->method_id) {
                case DEMO_CLIENT_CONNECTION_START:
                    bucket = ipr_bucket_new (12
                                + (self->payload.connection_start.server_properties?
                                   self->payload.connection_start.server_properties->cur_size: 0)
                                + (self->payload.connection_start.mechanisms?
                                   self->payload.connection_start.mechanisms->cur_size: 0)
                                + (self->payload.connection_start.locales?
                                   self->payload.connection_start.locales->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_OCTET (data_target, self->payload.connection_start.version_major);
                    PUT_OCTET (data_target, self->payload.connection_start.version_minor);
                    PUT_TABLE (data_target, self->payload.connection_start.server_properties);
                    PUT_LSTR  (data_target, self->payload.connection_start.mechanisms);
                    PUT_LSTR  (data_target, self->payload.connection_start.locales);
                    break;
                case DEMO_CLIENT_CONNECTION_START_OK:
                    bucket = ipr_bucket_new (10
                                + (self->payload.connection_start_ok.client_properties?
                                   self->payload.connection_start_ok.client_properties->cur_size: 0)
                                + strlen (self->payload.connection_start_ok.mechanism)
                                + (self->payload.connection_start_ok.response?
                                   self->payload.connection_start_ok.response->cur_size: 0)
                                + strlen (self->payload.connection_start_ok.locale) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_TABLE (data_target, self->payload.connection_start_ok.client_properties);
                    PUT_SSTR  (data_target, self->payload.connection_start_ok.mechanism);
                    PUT_LSTR  (data_target, self->payload.connection_start_ok.response);
                    PUT_SSTR  (data_target, self->payload.connection_start_ok.locale);
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE:
                    bucket = ipr_bucket_new (6
                                + (self->payload.connection_secure.challenge?
                                   self->payload.connection_secure.challenge->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_LSTR  (data_target, self->payload.connection_secure.challenge);
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE_OK:
                    bucket = ipr_bucket_new (6
                                + (self->payload.connection_secure_ok.response?
                                   self->payload.connection_secure_ok.response->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_LSTR  (data_target, self->payload.connection_secure_ok.response);
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE:
                    bucket = ipr_bucket_new (12 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SHORT (data_target, self->payload.connection_tune.channel_max);
                    PUT_LONG  (data_target, self->payload.connection_tune.frame_max);
                    PUT_SHORT (data_target, self->payload.connection_tune.heartbeat);
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE_OK:
                    bucket = ipr_bucket_new (12 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SHORT (data_target, self->payload.connection_tune_ok.channel_max);
                    PUT_LONG  (data_target, self->payload.connection_tune_ok.frame_max);
                    PUT_SHORT (data_target, self->payload.connection_tune_ok.heartbeat);
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN:
                    bucket = ipr_bucket_new (7
                                + strlen (self->payload.connection_open.virtual_host)
                                + strlen (self->payload.connection_open.capabilities) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.connection_open.virtual_host);
                    PUT_SSTR  (data_target, self->payload.connection_open.capabilities);
                    bit_string |= (self->payload.connection_open.insist & 1) &lt;&lt; 0;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN_OK:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.connection_open_ok.known_hosts) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.connection_open_ok.known_hosts);
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE:
                    bucket = ipr_bucket_new (11
                                + strlen (self->payload.connection_close.reply_text) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SHORT (data_target, self->payload.connection_close.reply_code);
                    PUT_SSTR  (data_target, self->payload.connection_close.reply_text);
                    PUT_SHORT (data_target, self->payload.connection_close.class_id);
                    PUT_SHORT (data_target, self->payload.connection_close.method_id);
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE_OK:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
            }
            break;
        case DEMO_CLIENT_CHANNEL:
            switch (self->method_id) {
                case DEMO_CLIENT_CHANNEL_OPEN:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.channel_open.out_of_band) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.channel_open.out_of_band);
                    break;
                case DEMO_CLIENT_CHANNEL_OPEN_OK:
                    bucket = ipr_bucket_new (6
                                + (self->payload.channel_open_ok.channel_id?
                                   self->payload.channel_open_ok.channel_id->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_LSTR  (data_target, self->payload.channel_open_ok.channel_id);
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW:
                    bucket = ipr_bucket_new (5 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    bit_string |= (self->payload.channel_flow.active & 1) &lt;&lt; 0;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW_OK:
                    bucket = ipr_bucket_new (5 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    bit_string |= (self->payload.channel_flow_ok.active & 1) &lt;&lt; 0;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE:
                    bucket = ipr_bucket_new (11
                                + strlen (self->payload.channel_close.reply_text) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SHORT (data_target, self->payload.channel_close.reply_code);
                    PUT_SSTR  (data_target, self->payload.channel_close.reply_text);
                    PUT_SHORT (data_target, self->payload.channel_close.class_id);
                    PUT_SHORT (data_target, self->payload.channel_close.method_id);
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE_OK:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
            }
            break;
        case DEMO_CLIENT_EXCHANGE:
            switch (self->method_id) {
                case DEMO_CLIENT_EXCHANGE_DECLARE:
                    bucket = ipr_bucket_new (7
                                + strlen (self->payload.exchange_declare.exchange)
                                + strlen (self->payload.exchange_declare.type) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.exchange_declare.exchange);
                    PUT_SSTR  (data_target, self->payload.exchange_declare.type);
                    bit_string |= (self->payload.exchange_declare.passive & 1) &lt;&lt; 0;
                    bit_string |= (self->payload.exchange_declare.durable & 1) &lt;&lt; 1;
                    bit_string |= (self->payload.exchange_declare.auto_delete & 1) &lt;&lt; 2;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_EXCHANGE_DECLARE_OK:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE:
                    bucket = ipr_bucket_new (6
                                + strlen (self->payload.exchange_delete.exchange) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.exchange_delete.exchange);
                    bit_string |= (self->payload.exchange_delete.if_unused & 1) &lt;&lt; 0;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE_OK:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
            }
            break;
        case DEMO_CLIENT_QUEUE:
            switch (self->method_id) {
                case DEMO_CLIENT_QUEUE_DECLARE:
                    bucket = ipr_bucket_new (6
                                + strlen (self->payload.queue_declare.queue) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.queue_declare.queue);
                    bit_string |= (self->payload.queue_declare.passive & 1) &lt;&lt; 0;
                    bit_string |= (self->payload.queue_declare.durable & 1) &lt;&lt; 1;
                    bit_string |= (self->payload.queue_declare.exclusive & 1) &lt;&lt; 2;
                    bit_string |= (self->payload.queue_declare.auto_delete & 1) &lt;&lt; 3;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_QUEUE_DECLARE_OK:
                    bucket = ipr_bucket_new (7
                                + strlen (self->payload.queue_declare_ok.queue)
                                + (self->payload.queue_declare_ok.properties?
                                   self->payload.queue_declare_ok.properties->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.queue_declare_ok.queue);
                    PUT_TABLE (data_target, self->payload.queue_declare_ok.properties);
                    break;
                case DEMO_CLIENT_QUEUE_BIND:
                    bucket = ipr_bucket_new (8
                                + strlen (self->payload.queue_bind.queue)
                                + strlen (self->payload.queue_bind.exchange)
                                + (self->payload.queue_bind.arguments?
                                   self->payload.queue_bind.arguments->cur_size: 0) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.queue_bind.queue);
                    PUT_SSTR  (data_target, self->payload.queue_bind.exchange);
                    PUT_TABLE (data_target, self->payload.queue_bind.arguments);
                    break;
                case DEMO_CLIENT_QUEUE_BIND_OK:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
                case DEMO_CLIENT_QUEUE_PURGE:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.queue_purge.queue) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.queue_purge.queue);
                    break;
                case DEMO_CLIENT_QUEUE_PURGE_OK:
                    bucket = ipr_bucket_new (8 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_LONG  (data_target, self->payload.queue_purge_ok.message_count);
                    break;
                case DEMO_CLIENT_QUEUE_DELETE:
                    bucket = ipr_bucket_new (6
                                + strlen (self->payload.queue_delete.queue) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.queue_delete.queue);
                    bit_string |= (self->payload.queue_delete.if_unused & 1) &lt;&lt; 0;
                    bit_string |= (self->payload.queue_delete.if_empty & 1) &lt;&lt; 1;
                    PUT_OCTET (data_target, bit_string);
                    bit_string = 0;
                    break;
                case DEMO_CLIENT_QUEUE_DELETE_OK:
                    bucket = ipr_bucket_new (8 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_LONG  (data_target, self->payload.queue_delete_ok.message_count);
                    break;
            }
            break;
        case DEMO_CLIENT_BASIC:
            switch (self->method_id) {
                case DEMO_CLIENT_BASIC_CONSUME:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.basic_consume.queue) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_consume.queue);
                    break;
                case DEMO_CLIENT_BASIC_CONSUME_OK:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.basic_consume_ok.consumer_tag) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_consume_ok.consumer_tag);
                    break;
                case DEMO_CLIENT_BASIC_CANCEL:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.basic_cancel.consumer_tag) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_cancel.consumer_tag);
                    break;
                case DEMO_CLIENT_BASIC_CANCEL_OK:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.basic_cancel_ok.consumer_tag) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_cancel_ok.consumer_tag);
                    break;
                case DEMO_CLIENT_BASIC_PUBLISH:
                    bucket = ipr_bucket_new (6
                                + strlen (self->payload.basic_publish.exchange)
                                + strlen (self->payload.basic_publish.routing_key) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_publish.exchange);
                    PUT_SSTR  (data_target, self->payload.basic_publish.routing_key);
                    break;
                case DEMO_CLIENT_BASIC_DELIVER:
                    bucket = ipr_bucket_new (14
                                + strlen (self->payload.basic_deliver.exchange)
                                + strlen (self->payload.basic_deliver.routing_key) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_deliver.exchange);
                    PUT_SSTR  (data_target, self->payload.basic_deliver.routing_key);
                    PUT_LLONG (data_target, self->payload.basic_deliver.delivery_tag);
                    break;
                case DEMO_CLIENT_BASIC_GET:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.basic_get.queue) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_get.queue);
                    break;
                case DEMO_CLIENT_BASIC_GET_OK:
                    bucket = ipr_bucket_new (10
                                + strlen (self->payload.basic_get_ok.exchange)
                                + strlen (self->payload.basic_get_ok.routing_key) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.basic_get_ok.exchange);
                    PUT_SSTR  (data_target, self->payload.basic_get_ok.routing_key);
                    PUT_LONG  (data_target, self->payload.basic_get_ok.message_count);
                    break;
                case DEMO_CLIENT_BASIC_GET_EMPTY:
                    bucket = ipr_bucket_new (4 + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    break;
            }
            break;
        case DEMO_CLIENT_DIRECT:
            switch (self->method_id) {
                case DEMO_CLIENT_DIRECT_PUT:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.direct_put.sink) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.direct_put.sink);
                    break;
                case DEMO_CLIENT_DIRECT_PUT_OK:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.direct_put_ok.lease) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.direct_put_ok.lease);
                    break;
                case DEMO_CLIENT_DIRECT_GET:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.direct_get.feed) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.direct_get.feed);
                    break;
                case DEMO_CLIENT_DIRECT_GET_OK:
                    bucket = ipr_bucket_new (5
                                + strlen (self->payload.direct_get_ok.lease) + 1);
                    data_target = bucket->data;
                    PUT_SHORT (data_target, self->class_id);
                    PUT_SHORT (data_target, self->method_id);
                    PUT_SSTR  (data_target, self->payload.direct_get_ok.lease);
                    break;
            }
            break;
    }
    bucket->cur_size = data_target - bucket->data;
</method>

<method name = "decode" return = "self">
    <doc>
    Decodes a demo_client_method_t object from a memory bucket.
    Allocates a new $(selfname)_t structure and returns this. Returns
    NULL if there was an error decoding the method object.
    </doc>
    <argument name = "bucket"   type = "ipr_bucket_t *">New bucket</argument>
    <argument name = "strerror" type = "char *">Error text, if any</argument>
    <declare name = "self" type = "$(selftype) *">Method reference</declare>
    <local>
    byte
        *data_source,                   //  Pointer into buffer data           
        *data_limit;                    //  Limit of buffer data               
    size_t
        string_size;
    byte
        bit_string = 0;
    </local>
    //
    //  Check that frame is correctly terminated before decoding it            
    assert (bucket);
    self = NULL;
    data_source = bucket->data;
    data_limit  = data_source + bucket->cur_size;

    self = self_new ();
    GET_SHORT (self->class_id,  data_source);
    GET_SHORT (self->method_id, data_source);
    switch (self->class_id) {
        case DEMO_CLIENT_CONNECTION:
            switch (self->method_id) {
                case DEMO_CLIENT_CONNECTION_START:
                    self->name = "connection.start";
                    self->sync = TRUE;
                    GET_OCTET (self->payload.connection_start.version_major, data_source);
                    GET_OCTET (self->payload.connection_start.version_minor, data_source);
                    GET_LSTR (self->payload.connection_start.server_properties, data_source);
                    GET_LSTR (self->payload.connection_start.mechanisms, data_source);
                    GET_LSTR (self->payload.connection_start.locales, data_source);
                    if (self->payload.connection_start.mechanisms == NULL || self->payload.connection_start.mechanisms->cur_size == 0) {
                        strcpy (strerror, "connection.start: mechanisms field is empty");
                        goto exception;
                    }
                    if (self->payload.connection_start.locales == NULL || self->payload.connection_start.locales->cur_size == 0) {
                        strcpy (strerror, "connection.start: locales field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_START_OK:
                    self->name = "connection.start_ok";
                    self->sync = TRUE;
                    GET_LSTR (self->payload.connection_start_ok.client_properties, data_source);
                    GET_SSTR (self->payload.connection_start_ok.mechanism, data_source);
                    GET_LSTR (self->payload.connection_start_ok.response, data_source);
                    GET_SSTR (self->payload.connection_start_ok.locale, data_source);
                    if (*self->payload.connection_start_ok.mechanism == 0) {
                        strcpy (strerror, "connection.start_ok: mechanism field is empty");
                        goto exception;
                    }
                    if (self->payload.connection_start_ok.response == NULL || self->payload.connection_start_ok.response->cur_size == 0) {
                        strcpy (strerror, "connection.start_ok: response field is empty");
                        goto exception;
                    }
                    if (*self->payload.connection_start_ok.locale == 0) {
                        strcpy (strerror, "connection.start_ok: locale field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE:
                    self->name = "connection.secure";
                    self->sync = TRUE;
                    GET_LSTR (self->payload.connection_secure.challenge, data_source);
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE_OK:
                    self->name = "connection.secure_ok";
                    self->sync = TRUE;
                    GET_LSTR (self->payload.connection_secure_ok.response, data_source);
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE:
                    self->name = "connection.tune";
                    self->sync = TRUE;
                    GET_SHORT (self->payload.connection_tune.channel_max, data_source);
                    GET_LONG  (self->payload.connection_tune.frame_max, data_source);
                    GET_SHORT (self->payload.connection_tune.heartbeat, data_source);
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE_OK:
                    self->name = "connection.tune_ok";
                    self->sync = TRUE;
                    GET_SHORT (self->payload.connection_tune_ok.channel_max, data_source);
                    GET_LONG  (self->payload.connection_tune_ok.frame_max, data_source);
                    GET_SHORT (self->payload.connection_tune_ok.heartbeat, data_source);
                    if (self->payload.connection_tune_ok.channel_max == 0) {
                        strcpy (strerror, "connection.tune_ok: channel_max field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN:
                    self->name = "connection.open";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.connection_open.virtual_host, data_source);
                    GET_SSTR (self->payload.connection_open.capabilities, data_source);
                    GET_OCTET (bit_string, data_source);
                    self->payload.connection_open.insist = (bit_string >> 0) & 1;
                    if (!ipr_regexp_match (s_regexp_connection_virtual_host, self->payload.connection_open.virtual_host)) {
                        strcpy (strerror, "connection.open: virtual_host field is invalid");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN_OK:
                    self->name = "connection.open_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.connection_open_ok.known_hosts, data_source);
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE:
                    self->name = "connection.close";
                    self->sync = TRUE;
                    GET_SHORT (self->payload.connection_close.reply_code, data_source);
                    GET_SSTR (self->payload.connection_close.reply_text, data_source);
                    GET_SHORT (self->payload.connection_close.class_id, data_source);
                    GET_SHORT (self->payload.connection_close.method_id, data_source);
                    if (self->payload.connection_close.reply_code == 0) {
                        strcpy (strerror, "connection.close: reply_code field is empty");
                        goto exception;
                    }
                    if (*self->payload.connection_close.reply_text == 0) {
                        strcpy (strerror, "connection.close: reply_text field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE_OK:
                    self->name = "connection.close_ok";
                    self->sync = TRUE;
                    break;
            }
            break;
        case DEMO_CLIENT_CHANNEL:
            switch (self->method_id) {
                case DEMO_CLIENT_CHANNEL_OPEN:
                    self->name = "channel.open";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.channel_open.out_of_band, data_source);
                    break;
                case DEMO_CLIENT_CHANNEL_OPEN_OK:
                    self->name = "channel.open_ok";
                    self->sync = TRUE;
                    GET_LSTR (self->payload.channel_open_ok.channel_id, data_source);
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW:
                    self->name = "channel.flow";
                    self->sync = TRUE;
                    GET_OCTET (bit_string, data_source);
                    self->payload.channel_flow.active = (bit_string >> 0) & 1;
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW_OK:
                    self->name = "channel.flow_ok";
                    self->sync = FALSE;
                    GET_OCTET (bit_string, data_source);
                    self->payload.channel_flow_ok.active = (bit_string >> 0) & 1;
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE:
                    self->name = "channel.close";
                    self->sync = TRUE;
                    GET_SHORT (self->payload.channel_close.reply_code, data_source);
                    GET_SSTR (self->payload.channel_close.reply_text, data_source);
                    GET_SHORT (self->payload.channel_close.class_id, data_source);
                    GET_SHORT (self->payload.channel_close.method_id, data_source);
                    if (self->payload.channel_close.reply_code == 0) {
                        strcpy (strerror, "channel.close: reply_code field is empty");
                        goto exception;
                    }
                    if (*self->payload.channel_close.reply_text == 0) {
                        strcpy (strerror, "channel.close: reply_text field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE_OK:
                    self->name = "channel.close_ok";
                    self->sync = TRUE;
                    break;
            }
            break;
        case DEMO_CLIENT_EXCHANGE:
            switch (self->method_id) {
                case DEMO_CLIENT_EXCHANGE_DECLARE:
                    self->name = "exchange.declare";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.exchange_declare.exchange, data_source);
                    GET_SSTR (self->payload.exchange_declare.type, data_source);
                    GET_OCTET (bit_string, data_source);
                    self->payload.exchange_declare.passive = (bit_string >> 0) & 1;
                    self->payload.exchange_declare.durable = (bit_string >> 1) & 1;
                    self->payload.exchange_declare.auto_delete = (bit_string >> 2) & 1;
                    if (!ipr_regexp_match (s_regexp_exchange_exchange, self->payload.exchange_declare.exchange)) {
                        strcpy (strerror, "exchange.declare: exchange field is invalid");
                        goto exception;
                    }
                    if (!ipr_regexp_match (s_regexp_exchange_type, self->payload.exchange_declare.type)) {
                        strcpy (strerror, "exchange.declare: type field is invalid");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_EXCHANGE_DECLARE_OK:
                    self->name = "exchange.declare_ok";
                    self->sync = TRUE;
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE:
                    self->name = "exchange.delete";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.exchange_delete.exchange, data_source);
                    GET_OCTET (bit_string, data_source);
                    self->payload.exchange_delete.if_unused = (bit_string >> 0) & 1;
                    if (*self->payload.exchange_delete.exchange == 0) {
                        strcpy (strerror, "exchange.delete: exchange field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE_OK:
                    self->name = "exchange.delete_ok";
                    self->sync = TRUE;
                    break;
            }
            break;
        case DEMO_CLIENT_QUEUE:
            switch (self->method_id) {
                case DEMO_CLIENT_QUEUE_DECLARE:
                    self->name = "queue.declare";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.queue_declare.queue, data_source);
                    GET_OCTET (bit_string, data_source);
                    self->payload.queue_declare.passive = (bit_string >> 0) & 1;
                    self->payload.queue_declare.durable = (bit_string >> 1) & 1;
                    self->payload.queue_declare.exclusive = (bit_string >> 2) & 1;
                    self->payload.queue_declare.auto_delete = (bit_string >> 3) & 1;
                    if (!ipr_regexp_match (s_regexp_queue_queue, self->payload.queue_declare.queue)) {
                        strcpy (strerror, "queue.declare: queue field is invalid");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_QUEUE_DECLARE_OK:
                    self->name = "queue.declare_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.queue_declare_ok.queue, data_source);
                    GET_LSTR (self->payload.queue_declare_ok.properties, data_source);
                    if (*self->payload.queue_declare_ok.queue == 0) {
                        strcpy (strerror, "queue.declare_ok: queue field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_QUEUE_BIND:
                    self->name = "queue.bind";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.queue_bind.queue, data_source);
                    GET_SSTR (self->payload.queue_bind.exchange, data_source);
                    GET_LSTR (self->payload.queue_bind.arguments, data_source);
                    if (*self->payload.queue_bind.queue == 0) {
                        strcpy (strerror, "queue.bind: queue field is empty");
                        goto exception;
                    }
                    if (strlen (self->payload.queue_bind.exchange) > 127) {
                        strcpy (strerror, "queue.bind: exchange field exceeds >127 chars");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_QUEUE_BIND_OK:
                    self->name = "queue.bind_ok";
                    self->sync = TRUE;
                    break;
                case DEMO_CLIENT_QUEUE_PURGE:
                    self->name = "queue.purge";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.queue_purge.queue, data_source);
                    if (*self->payload.queue_purge.queue == 0) {
                        strcpy (strerror, "queue.purge: queue field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_QUEUE_PURGE_OK:
                    self->name = "queue.purge_ok";
                    self->sync = TRUE;
                    GET_LONG  (self->payload.queue_purge_ok.message_count, data_source);
                    break;
                case DEMO_CLIENT_QUEUE_DELETE:
                    self->name = "queue.delete";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.queue_delete.queue, data_source);
                    GET_OCTET (bit_string, data_source);
                    self->payload.queue_delete.if_unused = (bit_string >> 0) & 1;
                    self->payload.queue_delete.if_empty = (bit_string >> 1) & 1;
                    if (*self->payload.queue_delete.queue == 0) {
                        strcpy (strerror, "queue.delete: queue field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_QUEUE_DELETE_OK:
                    self->name = "queue.delete_ok";
                    self->sync = TRUE;
                    GET_LONG  (self->payload.queue_delete_ok.message_count, data_source);
                    break;
            }
            break;
        case DEMO_CLIENT_BASIC:
            switch (self->method_id) {
                case DEMO_CLIENT_BASIC_CONSUME:
                    self->name = "basic.consume";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_consume.queue, data_source);
                    if (strlen (self->payload.basic_consume.queue) > 127) {
                        strcpy (strerror, "basic.consume: queue field exceeds >127 chars");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_BASIC_CONSUME_OK:
                    self->name = "basic.consume_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_consume_ok.consumer_tag, data_source);
                    break;
                case DEMO_CLIENT_BASIC_CANCEL:
                    self->name = "basic.cancel";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_cancel.consumer_tag, data_source);
                    break;
                case DEMO_CLIENT_BASIC_CANCEL_OK:
                    self->name = "basic.cancel_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_cancel_ok.consumer_tag, data_source);
                    break;
                case DEMO_CLIENT_BASIC_PUBLISH:
                    self->name = "basic.publish";
                    self->sync = FALSE;
                    GET_SSTR (self->payload.basic_publish.exchange, data_source);
                    GET_SSTR (self->payload.basic_publish.routing_key, data_source);
                    if (*self->payload.basic_publish.exchange == 0) {
                        strcpy (strerror, "basic.publish: exchange field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_BASIC_DELIVER:
                    self->name = "basic.deliver";
                    self->sync = FALSE;
                    GET_SSTR (self->payload.basic_deliver.exchange, data_source);
                    GET_SSTR (self->payload.basic_deliver.routing_key, data_source);
                    GET_LLONG (self->payload.basic_deliver.delivery_tag, data_source);
                    if (strlen (self->payload.basic_deliver.exchange) > 127) {
                        strcpy (strerror, "basic.deliver: exchange field exceeds >127 chars");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET:
                    self->name = "basic.get";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_get.queue, data_source);
                    if (strlen (self->payload.basic_get.queue) > 127) {
                        strcpy (strerror, "basic.get: queue field exceeds >127 chars");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET_OK:
                    self->name = "basic.get_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.basic_get_ok.exchange, data_source);
                    GET_SSTR (self->payload.basic_get_ok.routing_key, data_source);
                    GET_LONG  (self->payload.basic_get_ok.message_count, data_source);
                    if (*self->payload.basic_get_ok.exchange == 0) {
                        strcpy (strerror, "basic.get_ok: exchange field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET_EMPTY:
                    self->name = "basic.get_empty";
                    self->sync = TRUE;
                    break;
            }
            break;
        case DEMO_CLIENT_DIRECT:
            switch (self->method_id) {
                case DEMO_CLIENT_DIRECT_PUT:
                    self->name = "direct.put";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.direct_put.sink, data_source);
                    break;
                case DEMO_CLIENT_DIRECT_PUT_OK:
                    self->name = "direct.put_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.direct_put_ok.lease, data_source);
                    break;
                case DEMO_CLIENT_DIRECT_GET:
                    self->name = "direct.get";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.direct_get.feed, data_source);
                    if (*self->payload.direct_get.feed == 0) {
                        strcpy (strerror, "direct.get: feed field is empty");
                        goto exception;
                    }
                    break;
                case DEMO_CLIENT_DIRECT_GET_OK:
                    self->name = "direct.get_ok";
                    self->sync = TRUE;
                    GET_SSTR (self->payload.direct_get_ok.lease, data_source);
                    break;
            }
            break;
    }
    goto finished;
    underflow:
        icl_shortstr_fmt (strerror, "malformed frame for '%s' method", self->name);
    exception:
        self_destroy (&self);
        return (NULL);
    finished:
        //  Return via normal function exit
</method>

<method name = "dump" template = "function">
    <doc>
    Dumps contents of serialised method for tracing.
    </doc>
    <argument name = "log" type = "smt_log_t *">Log file, if any</argument>
    <argument name = "prefix" type = "char *">Prefix for line</argument>
    //
    switch (self->class_id) {
        case DEMO_CLIENT_CONNECTION:
            switch (self->method_id) {
                case DEMO_CLIENT_CONNECTION_START:
                    if (log) {
                        smt_log_print (log, "%sconnection.start:"\\
                            " version_major=%d version_minor=%d server_properties=%uB mechanisms=%uB locales=%uB", prefix,
                            self->payload.connection_start.version_major,
                            self->payload.connection_start.version_minor,
                            self->payload.connection_start.server_properties?
                                self->payload.connection_start.server_properties->cur_size: 0,
                            self->payload.connection_start.mechanisms?
                                self->payload.connection_start.mechanisms->cur_size: 0,
                            self->payload.connection_start.locales?
                                self->payload.connection_start.locales->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%sconnection.start:"\\
                            " version_major=%d version_minor=%d server_properties=%uB mechanisms=%uB locales=%uB", prefix,
                            self->payload.connection_start.version_major,
                            self->payload.connection_start.version_minor,
                            self->payload.connection_start.server_properties?
                                self->payload.connection_start.server_properties->cur_size: 0,
                            self->payload.connection_start.mechanisms?
                                self->payload.connection_start.mechanisms->cur_size: 0,
                            self->payload.connection_start.locales?
                                self->payload.connection_start.locales->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_START_OK:
                    if (log) {
                        smt_log_print (log, "%sconnection.start-ok:"\\
                            " client_properties=%uB mechanism=\\"%s\\" response=%uB locale=\\"%s\\"", prefix,
                            self->payload.connection_start_ok.client_properties?
                                self->payload.connection_start_ok.client_properties->cur_size: 0,
                            self->payload.connection_start_ok.mechanism,
                            self->payload.connection_start_ok.response?
                                self->payload.connection_start_ok.response->cur_size: 0,
                            self->payload.connection_start_ok.locale);
                    }
                    else {
                        icl_console_print ("%sconnection.start-ok:"\\
                            " client_properties=%uB mechanism=\\"%s\\" response=%uB locale=\\"%s\\"", prefix,
                            self->payload.connection_start_ok.client_properties?
                                self->payload.connection_start_ok.client_properties->cur_size: 0,
                            self->payload.connection_start_ok.mechanism,
                            self->payload.connection_start_ok.response?
                                self->payload.connection_start_ok.response->cur_size: 0,
                            self->payload.connection_start_ok.locale);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE:
                    if (log) {
                        smt_log_print (log, "%sconnection.secure:"\\
                            " challenge=%uB", prefix,
                            self->payload.connection_secure.challenge?
                                self->payload.connection_secure.challenge->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%sconnection.secure:"\\
                            " challenge=%uB", prefix,
                            self->payload.connection_secure.challenge?
                                self->payload.connection_secure.challenge->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_SECURE_OK:
                    if (log) {
                        smt_log_print (log, "%sconnection.secure-ok:"\\
                            " response=%uB", prefix,
                            self->payload.connection_secure_ok.response?
                                self->payload.connection_secure_ok.response->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%sconnection.secure-ok:"\\
                            " response=%uB", prefix,
                            self->payload.connection_secure_ok.response?
                                self->payload.connection_secure_ok.response->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE:
                    if (log) {
                        smt_log_print (log, "%sconnection.tune:"\\
                            " channel_max=%d frame_max=%u heartbeat=%d", prefix,
                            self->payload.connection_tune.channel_max,
                            self->payload.connection_tune.frame_max,
                            self->payload.connection_tune.heartbeat);
                    }
                    else {
                        icl_console_print ("%sconnection.tune:"\\
                            " channel_max=%d frame_max=%u heartbeat=%d", prefix,
                            self->payload.connection_tune.channel_max,
                            self->payload.connection_tune.frame_max,
                            self->payload.connection_tune.heartbeat);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_TUNE_OK:
                    if (log) {
                        smt_log_print (log, "%sconnection.tune-ok:"\\
                            " channel_max=%d frame_max=%u heartbeat=%d", prefix,
                            self->payload.connection_tune_ok.channel_max,
                            self->payload.connection_tune_ok.frame_max,
                            self->payload.connection_tune_ok.heartbeat);
                    }
                    else {
                        icl_console_print ("%sconnection.tune-ok:"\\
                            " channel_max=%d frame_max=%u heartbeat=%d", prefix,
                            self->payload.connection_tune_ok.channel_max,
                            self->payload.connection_tune_ok.frame_max,
                            self->payload.connection_tune_ok.heartbeat);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN:
                    if (log) {
                        smt_log_print (log, "%sconnection.open:"\\
                            " virtual_host=\\"%s\\" capabilities=\\"%s\\" insist=%d", prefix,
                            self->payload.connection_open.virtual_host,
                            self->payload.connection_open.capabilities,
                            self->payload.connection_open.insist);
                    }
                    else {
                        icl_console_print ("%sconnection.open:"\\
                            " virtual_host=\\"%s\\" capabilities=\\"%s\\" insist=%d", prefix,
                            self->payload.connection_open.virtual_host,
                            self->payload.connection_open.capabilities,
                            self->payload.connection_open.insist);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_OPEN_OK:
                    if (log) {
                        smt_log_print (log, "%sconnection.open-ok:"\\
                            " known_hosts=\\"%s\\"", prefix,
                            self->payload.connection_open_ok.known_hosts);
                    }
                    else {
                        icl_console_print ("%sconnection.open-ok:"\\
                            " known_hosts=\\"%s\\"", prefix,
                            self->payload.connection_open_ok.known_hosts);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE:
                    if (log) {
                        smt_log_print (log, "%sconnection.close:"\\
                            " reply_code=%d reply_text=\\"%s\\" class_id=%d method_id=%d", prefix,
                            self->payload.connection_close.reply_code,
                            self->payload.connection_close.reply_text,
                            self->payload.connection_close.class_id,
                            self->payload.connection_close.method_id);
                    }
                    else {
                        icl_console_print ("%sconnection.close:"\\
                            " reply_code=%d reply_text=\\"%s\\" class_id=%d method_id=%d", prefix,
                            self->payload.connection_close.reply_code,
                            self->payload.connection_close.reply_text,
                            self->payload.connection_close.class_id,
                            self->payload.connection_close.method_id);
                    }
                    break;
                case DEMO_CLIENT_CONNECTION_CLOSE_OK:
                    if (log) {
                        smt_log_print (log, "%sconnection.close-ok:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%sconnection.close-ok:"\\
                            "", prefix);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        case DEMO_CLIENT_CHANNEL:
            switch (self->method_id) {
                case DEMO_CLIENT_CHANNEL_OPEN:
                    if (log) {
                        smt_log_print (log, "%schannel.open:"\\
                            " out_of_band=\\"%s\\"", prefix,
                            self->payload.channel_open.out_of_band);
                    }
                    else {
                        icl_console_print ("%schannel.open:"\\
                            " out_of_band=\\"%s\\"", prefix,
                            self->payload.channel_open.out_of_band);
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_OPEN_OK:
                    if (log) {
                        smt_log_print (log, "%schannel.open-ok:"\\
                            " channel_id=%uB", prefix,
                            self->payload.channel_open_ok.channel_id?
                                self->payload.channel_open_ok.channel_id->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%schannel.open-ok:"\\
                            " channel_id=%uB", prefix,
                            self->payload.channel_open_ok.channel_id?
                                self->payload.channel_open_ok.channel_id->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW:
                    if (log) {
                        smt_log_print (log, "%schannel.flow:"\\
                            " active=%d", prefix,
                            self->payload.channel_flow.active);
                    }
                    else {
                        icl_console_print ("%schannel.flow:"\\
                            " active=%d", prefix,
                            self->payload.channel_flow.active);
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_FLOW_OK:
                    if (log) {
                        smt_log_print (log, "%schannel.flow-ok:"\\
                            " active=%d", prefix,
                            self->payload.channel_flow_ok.active);
                    }
                    else {
                        icl_console_print ("%schannel.flow-ok:"\\
                            " active=%d", prefix,
                            self->payload.channel_flow_ok.active);
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE:
                    if (log) {
                        smt_log_print (log, "%schannel.close:"\\
                            " reply_code=%d reply_text=\\"%s\\" class_id=%d method_id=%d", prefix,
                            self->payload.channel_close.reply_code,
                            self->payload.channel_close.reply_text,
                            self->payload.channel_close.class_id,
                            self->payload.channel_close.method_id);
                    }
                    else {
                        icl_console_print ("%schannel.close:"\\
                            " reply_code=%d reply_text=\\"%s\\" class_id=%d method_id=%d", prefix,
                            self->payload.channel_close.reply_code,
                            self->payload.channel_close.reply_text,
                            self->payload.channel_close.class_id,
                            self->payload.channel_close.method_id);
                    }
                    break;
                case DEMO_CLIENT_CHANNEL_CLOSE_OK:
                    if (log) {
                        smt_log_print (log, "%schannel.close-ok:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%schannel.close-ok:"\\
                            "", prefix);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        case DEMO_CLIENT_EXCHANGE:
            switch (self->method_id) {
                case DEMO_CLIENT_EXCHANGE_DECLARE:
                    if (log) {
                        smt_log_print (log, "%sexchange.declare:"\\
                            " exchange=\\"%s\\" type=\\"%s\\" passive=%d durable=%d auto_delete=%d", prefix,
                            self->payload.exchange_declare.exchange,
                            self->payload.exchange_declare.type,
                            self->payload.exchange_declare.passive,
                            self->payload.exchange_declare.durable,
                            self->payload.exchange_declare.auto_delete);
                    }
                    else {
                        icl_console_print ("%sexchange.declare:"\\
                            " exchange=\\"%s\\" type=\\"%s\\" passive=%d durable=%d auto_delete=%d", prefix,
                            self->payload.exchange_declare.exchange,
                            self->payload.exchange_declare.type,
                            self->payload.exchange_declare.passive,
                            self->payload.exchange_declare.durable,
                            self->payload.exchange_declare.auto_delete);
                    }
                    break;
                case DEMO_CLIENT_EXCHANGE_DECLARE_OK:
                    if (log) {
                        smt_log_print (log, "%sexchange.declare-ok:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%sexchange.declare-ok:"\\
                            "", prefix);
                    }
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE:
                    if (log) {
                        smt_log_print (log, "%sexchange.delete:"\\
                            " exchange=\\"%s\\" if_unused=%d", prefix,
                            self->payload.exchange_delete.exchange,
                            self->payload.exchange_delete.if_unused);
                    }
                    else {
                        icl_console_print ("%sexchange.delete:"\\
                            " exchange=\\"%s\\" if_unused=%d", prefix,
                            self->payload.exchange_delete.exchange,
                            self->payload.exchange_delete.if_unused);
                    }
                    break;
                case DEMO_CLIENT_EXCHANGE_DELETE_OK:
                    if (log) {
                        smt_log_print (log, "%sexchange.delete-ok:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%sexchange.delete-ok:"\\
                            "", prefix);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        case DEMO_CLIENT_QUEUE:
            switch (self->method_id) {
                case DEMO_CLIENT_QUEUE_DECLARE:
                    if (log) {
                        smt_log_print (log, "%squeue.declare:"\\
                            " queue=\\"%s\\" passive=%d durable=%d exclusive=%d auto_delete=%d", prefix,
                            self->payload.queue_declare.queue,
                            self->payload.queue_declare.passive,
                            self->payload.queue_declare.durable,
                            self->payload.queue_declare.exclusive,
                            self->payload.queue_declare.auto_delete);
                    }
                    else {
                        icl_console_print ("%squeue.declare:"\\
                            " queue=\\"%s\\" passive=%d durable=%d exclusive=%d auto_delete=%d", prefix,
                            self->payload.queue_declare.queue,
                            self->payload.queue_declare.passive,
                            self->payload.queue_declare.durable,
                            self->payload.queue_declare.exclusive,
                            self->payload.queue_declare.auto_delete);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_DECLARE_OK:
                    if (log) {
                        smt_log_print (log, "%squeue.declare-ok:"\\
                            " queue=\\"%s\\" properties=%uB", prefix,
                            self->payload.queue_declare_ok.queue,
                            self->payload.queue_declare_ok.properties?
                                self->payload.queue_declare_ok.properties->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%squeue.declare-ok:"\\
                            " queue=\\"%s\\" properties=%uB", prefix,
                            self->payload.queue_declare_ok.queue,
                            self->payload.queue_declare_ok.properties?
                                self->payload.queue_declare_ok.properties->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_BIND:
                    if (log) {
                        smt_log_print (log, "%squeue.bind:"\\
                            " queue=\\"%s\\" exchange=\\"%s\\" arguments=%uB", prefix,
                            self->payload.queue_bind.queue,
                            self->payload.queue_bind.exchange,
                            self->payload.queue_bind.arguments?
                                self->payload.queue_bind.arguments->cur_size: 0);
                    }
                    else {
                        icl_console_print ("%squeue.bind:"\\
                            " queue=\\"%s\\" exchange=\\"%s\\" arguments=%uB", prefix,
                            self->payload.queue_bind.queue,
                            self->payload.queue_bind.exchange,
                            self->payload.queue_bind.arguments?
                                self->payload.queue_bind.arguments->cur_size: 0);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_BIND_OK:
                    if (log) {
                        smt_log_print (log, "%squeue.bind-ok:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%squeue.bind-ok:"\\
                            "", prefix);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_PURGE:
                    if (log) {
                        smt_log_print (log, "%squeue.purge:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.queue_purge.queue);
                    }
                    else {
                        icl_console_print ("%squeue.purge:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.queue_purge.queue);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_PURGE_OK:
                    if (log) {
                        smt_log_print (log, "%squeue.purge-ok:"\\
                            " message_count=%u", prefix,
                            self->payload.queue_purge_ok.message_count);
                    }
                    else {
                        icl_console_print ("%squeue.purge-ok:"\\
                            " message_count=%u", prefix,
                            self->payload.queue_purge_ok.message_count);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_DELETE:
                    if (log) {
                        smt_log_print (log, "%squeue.delete:"\\
                            " queue=\\"%s\\" if_unused=%d if_empty=%d", prefix,
                            self->payload.queue_delete.queue,
                            self->payload.queue_delete.if_unused,
                            self->payload.queue_delete.if_empty);
                    }
                    else {
                        icl_console_print ("%squeue.delete:"\\
                            " queue=\\"%s\\" if_unused=%d if_empty=%d", prefix,
                            self->payload.queue_delete.queue,
                            self->payload.queue_delete.if_unused,
                            self->payload.queue_delete.if_empty);
                    }
                    break;
                case DEMO_CLIENT_QUEUE_DELETE_OK:
                    if (log) {
                        smt_log_print (log, "%squeue.delete-ok:"\\
                            " message_count=%u", prefix,
                            self->payload.queue_delete_ok.message_count);
                    }
                    else {
                        icl_console_print ("%squeue.delete-ok:"\\
                            " message_count=%u", prefix,
                            self->payload.queue_delete_ok.message_count);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        case DEMO_CLIENT_BASIC:
            switch (self->method_id) {
                case DEMO_CLIENT_BASIC_CONSUME:
                    if (log) {
                        smt_log_print (log, "%sbasic.consume:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.basic_consume.queue);
                    }
                    else {
                        icl_console_print ("%sbasic.consume:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.basic_consume.queue);
                    }
                    break;
                case DEMO_CLIENT_BASIC_CONSUME_OK:
                    if (log) {
                        smt_log_print (log, "%sbasic.consume-ok:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_consume_ok.consumer_tag);
                    }
                    else {
                        icl_console_print ("%sbasic.consume-ok:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_consume_ok.consumer_tag);
                    }
                    break;
                case DEMO_CLIENT_BASIC_CANCEL:
                    if (log) {
                        smt_log_print (log, "%sbasic.cancel:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_cancel.consumer_tag);
                    }
                    else {
                        icl_console_print ("%sbasic.cancel:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_cancel.consumer_tag);
                    }
                    break;
                case DEMO_CLIENT_BASIC_CANCEL_OK:
                    if (log) {
                        smt_log_print (log, "%sbasic.cancel-ok:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_cancel_ok.consumer_tag);
                    }
                    else {
                        icl_console_print ("%sbasic.cancel-ok:"\\
                            " consumer_tag=\\"%s\\"", prefix,
                            self->payload.basic_cancel_ok.consumer_tag);
                    }
                    break;
                case DEMO_CLIENT_BASIC_PUBLISH:
                    if (log) {
                        smt_log_print (log, "%sbasic.publish:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\"", prefix,
                            self->payload.basic_publish.exchange,
                            self->payload.basic_publish.routing_key);
                    }
                    else {
                        icl_console_print ("%sbasic.publish:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\"", prefix,
                            self->payload.basic_publish.exchange,
                            self->payload.basic_publish.routing_key);
                    }
                    break;
                case DEMO_CLIENT_BASIC_DELIVER:
                    if (log) {
                        smt_log_print (log, "%sbasic.deliver:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\" delivery_tag=%lu", prefix,
                            self->payload.basic_deliver.exchange,
                            self->payload.basic_deliver.routing_key,
                            (long) self->payload.basic_deliver.delivery_tag);
                    }
                    else {
                        icl_console_print ("%sbasic.deliver:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\" delivery_tag=%lu", prefix,
                            self->payload.basic_deliver.exchange,
                            self->payload.basic_deliver.routing_key,
                            (long) self->payload.basic_deliver.delivery_tag);
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET:
                    if (log) {
                        smt_log_print (log, "%sbasic.get:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.basic_get.queue);
                    }
                    else {
                        icl_console_print ("%sbasic.get:"\\
                            " queue=\\"%s\\"", prefix,
                            self->payload.basic_get.queue);
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET_OK:
                    if (log) {
                        smt_log_print (log, "%sbasic.get-ok:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\" message_count=%u", prefix,
                            self->payload.basic_get_ok.exchange,
                            self->payload.basic_get_ok.routing_key,
                            self->payload.basic_get_ok.message_count);
                    }
                    else {
                        icl_console_print ("%sbasic.get-ok:"\\
                            " exchange=\\"%s\\" routing_key=\\"%s\\" message_count=%u", prefix,
                            self->payload.basic_get_ok.exchange,
                            self->payload.basic_get_ok.routing_key,
                            self->payload.basic_get_ok.message_count);
                    }
                    break;
                case DEMO_CLIENT_BASIC_GET_EMPTY:
                    if (log) {
                        smt_log_print (log, "%sbasic.get-empty:"\\
                            "", prefix);
                    }
                    else {
                        icl_console_print ("%sbasic.get-empty:"\\
                            "", prefix);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        case DEMO_CLIENT_DIRECT:
            switch (self->method_id) {
                case DEMO_CLIENT_DIRECT_PUT:
                    if (log) {
                        smt_log_print (log, "%sdirect.put:"\\
                            " sink=\\"%s\\"", prefix,
                            self->payload.direct_put.sink);
                    }
                    else {
                        icl_console_print ("%sdirect.put:"\\
                            " sink=\\"%s\\"", prefix,
                            self->payload.direct_put.sink);
                    }
                    break;
                case DEMO_CLIENT_DIRECT_PUT_OK:
                    if (log) {
                        smt_log_print (log, "%sdirect.put-ok:"\\
                            " lease=\\"%s\\"", prefix,
                            self->payload.direct_put_ok.lease);
                    }
                    else {
                        icl_console_print ("%sdirect.put-ok:"\\
                            " lease=\\"%s\\"", prefix,
                            self->payload.direct_put_ok.lease);
                    }
                    break;
                case DEMO_CLIENT_DIRECT_GET:
                    if (log) {
                        smt_log_print (log, "%sdirect.get:"\\
                            " feed=\\"%s\\"", prefix,
                            self->payload.direct_get.feed);
                    }
                    else {
                        icl_console_print ("%sdirect.get:"\\
                            " feed=\\"%s\\"", prefix,
                            self->payload.direct_get.feed);
                    }
                    break;
                case DEMO_CLIENT_DIRECT_GET_OK:
                    if (log) {
                        smt_log_print (log, "%sdirect.get-ok:"\\
                            " lease=\\"%s\\"", prefix,
                            self->payload.direct_get_ok.lease);
                    }
                    else {
                        icl_console_print ("%sdirect.get-ok:"\\
                            " lease=\\"%s\\"", prefix,
                            self->payload.direct_get_ok.lease);
                    }
                    break;
                default:
                    if (log)
                        smt_log_print (log, "E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
                    else
                        icl_console_print ("E: illegal method (class=%d, method=%d)",
                            self->class_id, self->method_id);
            }
            break;
        default:
            if (log)
                smt_log_print (log, "E: illegal class=%d", self->class_id);
            else
                icl_console_print ("E: illegal class=%d", self->class_id);
    }
</method>

<method name = "execute" template = "function">
    <doc>
    Accepts a demo_client_method_t object and executes the method.
    Returns 0 if the method was executed successfully, non-zero if there
    was an error - e.g. if the method is not allowed for this chassis.
    </doc>
    //
    <argument name = "connection" type = "demo_client_connection_t *">Current connection</argument>
    <argument name = "session" type = "demo_client_session_t *">Current session</argument>
    if (self->class_id  == DEMO_CLIENT_CONNECTION
    &&  self->method_id == DEMO_CLIENT_CONNECTION_START) {
        demo_client_connection_start_t
            *method;
        method = &self->payload.connection_start;
        {
            asl_field_list_t
        *fields;                        //  Decoded responses
    char
        *port;                          //  Port number from host string
    connection->version_major = method->version_major;
    connection->version_minor = method->version_minor;
    icl_longstr_destroy (&connection->server_properties);
    connection->server_properties = icl_longstr_dup (method->server_properties);
    icl_longstr_destroy (&connection->mechanisms);
    connection->mechanisms = icl_longstr_dup (method->mechanisms);
    icl_longstr_destroy (&connection->locales);
    connection->locales = icl_longstr_dup (method->locales);
    //
    fields = asl_field_list_new (method->server_properties);
    if (fields) {
        asl_field_list_cpy (fields, connection->server_host,        "host");
        asl_field_list_cpy (fields, connection->server_instance,    "instance");
        asl_field_list_cpy (fields, connection->server_product,     "product");
        asl_field_list_cpy (fields, connection->server_version,     "version");
        asl_field_list_cpy (fields, connection->server_platform,    "platform");
        asl_field_list_cpy (fields, connection->server_copyright,   "copyright");
        asl_field_list_cpy (fields, connection->server_information, "information");
        asl_field_list_cpy (fields, connection->server_identifier,  "identifier");
        asl_field_list_cpy (fields, connection->id,                 "connection");
        connection->server_direct = asl_field_list_integer (fields, "direct");

        port = strchr (connection->server_host, ':');
        if (port)
            icl_shortstr_cpy (connection->server_port, port + 1);

        asl_field_list_destroy (&fields);
    }
        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CONNECTION
    &&  self->method_id == DEMO_CLIENT_CONNECTION_SECURE) {
        demo_client_connection_secure_t
            *method;
        method = &self->payload.connection_secure;
        {
            icl_longstr_destroy (&connection->challenge);
    connection->challenge = icl_longstr_dup (method->challenge);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CONNECTION
    &&  self->method_id == DEMO_CLIENT_CONNECTION_TUNE) {
        demo_client_connection_tune_t
            *method;
        method = &self->payload.connection_tune;
        {
            connection->channel_max = method->channel_max;
    connection->frame_max = method->frame_max;
    connection->heartbeat = method->heartbeat;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CONNECTION
    &&  self->method_id == DEMO_CLIENT_CONNECTION_OPEN_OK) {
        demo_client_connection_open_ok_t
            *method;
        method = &self->payload.connection_open_ok;
        {
            icl_shortstr_cpy (connection->known_hosts, method->known_hosts);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CONNECTION
    &&  self->method_id == DEMO_CLIENT_CONNECTION_CLOSE) {
        demo_client_connection_close_t
            *method;
        method = &self->payload.connection_close;
        {
            connection->reply_code = method->reply_code;
    icl_shortstr_cpy (connection->reply_text, method->reply_text);
    connection->class_id = method->class_id;
    connection->method_id = method->method_id;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CHANNEL
    &&  self->method_id == DEMO_CLIENT_CHANNEL_OPEN_OK) {
        demo_client_channel_open_ok_t
            *method;
        method = &self->payload.channel_open_ok;
        {
            icl_longstr_destroy (&session->channel_id);
    session->channel_id = icl_longstr_dup (method->channel_id);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CHANNEL
    &&  self->method_id == DEMO_CLIENT_CHANNEL_FLOW) {
        demo_client_channel_flow_t
            *method;
        method = &self->payload.channel_flow;
        {
            session->active = method->active;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CHANNEL
    &&  self->method_id == DEMO_CLIENT_CHANNEL_FLOW_OK) {
        demo_client_channel_flow_ok_t
            *method;
        method = &self->payload.channel_flow_ok;
        {
            session->active = method->active;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_CHANNEL
    &&  self->method_id == DEMO_CLIENT_CHANNEL_CLOSE) {
        demo_client_channel_close_t
            *method;
        method = &self->payload.channel_close;
        {
            session->reply_code = method->reply_code;
    icl_shortstr_cpy (session->reply_text, method->reply_text);
    session->class_id = method->class_id;
    session->method_id = method->method_id;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_QUEUE
    &&  self->method_id == DEMO_CLIENT_QUEUE_DECLARE_OK) {
        demo_client_queue_declare_ok_t
            *method;
        method = &self->payload.queue_declare_ok;
        {
            icl_shortstr_cpy (session->queue, method->queue);
    icl_longstr_destroy (&session->properties);
    session->properties = icl_longstr_dup (method->properties);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_QUEUE
    &&  self->method_id == DEMO_CLIENT_QUEUE_PURGE_OK) {
        demo_client_queue_purge_ok_t
            *method;
        method = &self->payload.queue_purge_ok;
        {
            session->message_count = method->message_count;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_QUEUE
    &&  self->method_id == DEMO_CLIENT_QUEUE_DELETE_OK) {
        demo_client_queue_delete_ok_t
            *method;
        method = &self->payload.queue_delete_ok;
        {
            session->message_count = method->message_count;

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_BASIC
    &&  self->method_id == DEMO_CLIENT_BASIC_CONSUME_OK) {
        demo_client_basic_consume_ok_t
            *method;
        method = &self->payload.basic_consume_ok;
        {
            icl_shortstr_cpy (session->consumer_tag, method->consumer_tag);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_BASIC
    &&  self->method_id == DEMO_CLIENT_BASIC_CANCEL_OK) {
        demo_client_basic_cancel_ok_t
            *method;
        method = &self->payload.basic_cancel_ok;
        {
            icl_shortstr_cpy (session->consumer_tag, method->consumer_tag);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_BASIC
    &&  self->method_id == DEMO_CLIENT_BASIC_DELIVER) {
        demo_client_basic_deliver_t
            *method;
        demo_content_basic_t
            *content;
        method = &self->payload.basic_deliver;
        content = (demo_content_basic_t *) self->content;
        {
            icl_shortstr_cpy (session->exchange, method->exchange);
    icl_shortstr_cpy (session->routing_key, method->routing_key);
    session->delivery_tag = method->delivery_tag;
    demo_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, NULL, 0);
        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_BASIC
    &&  self->method_id == DEMO_CLIENT_BASIC_GET_OK) {
        demo_client_basic_get_ok_t
            *method;
        demo_content_basic_t
            *content;
        method = &self->payload.basic_get_ok;
        content = (demo_content_basic_t *) self->content;
        {
            icl_shortstr_cpy (session->exchange, method->exchange);
    icl_shortstr_cpy (session->routing_key, method->routing_key);
    session->message_count = method->message_count;
    demo_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, NULL, 0);
        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_DIRECT
    &&  self->method_id == DEMO_CLIENT_DIRECT_PUT_OK) {
        demo_client_direct_put_ok_t
            *method;
        method = &self->payload.direct_put_ok;
        {
            icl_shortstr_cpy (session->lease, method->lease);

        }
    }
    else
    if (self->class_id  == DEMO_CLIENT_DIRECT
    &&  self->method_id == DEMO_CLIENT_DIRECT_GET_OK) {
        demo_client_direct_get_ok_t
            *method;
        method = &self->payload.direct_get_ok;
        {
            icl_shortstr_cpy (session->lease, method->lease);

        }
    }
</method>

<method name = "selftest" />

</class>
