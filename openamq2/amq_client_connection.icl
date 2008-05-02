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
    name      = "amq_client_connection"
    comment   = "amq_client connection class"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_constants" />
<import class = "amq_client_session" />
<import class = "amq_content_basic"/>
<public name = "header">
#include "zamqapi.h"
</public>


<context>
    dbyte
        channel_nbr;                    //  Top channel opened so far
    Bool
        silent;                         //  Don't print errors
    Bool
        alive;                          //  Connection is alive?
    Bool
        interrupt;                      //  Application was interrupted?
    Bool
        shutdown;                       //  We were shutdown
    int
        timeout;                        //  Connection timeout, msecs
    char
        *error_text;                    //  Last error cause
    byte
        version_major;                  //  protocol major version
    byte
        version_minor;                  //  protocol major version
    dbyte
        channel_max;                    //  proposed maximum channels
    qbyte
        frame_max;                      //  proposed maximum frame size
    dbyte
        heartbeat;                      //  desired heartbeat delay
    icl_shortstr_t
        known_hosts;                    //  list of known hosts
    icl_shortstr_t
        host;                           //  server to connect to
    dbyte
        reply_code;                     //  reply code from server
    icl_shortstr_t
        reply_text;                     //  localised reply text
    dbyte
        class_id;                       //  failing method class
    dbyte
        method_id;                      //  failing method ID
    icl_shortstr_t
        server_host;                    //  Reported by server
    icl_shortstr_t
        server_port;                    //  Just the port number            
    icl_shortstr_t
        server_product;                
    icl_shortstr_t
        server_version;
    icl_shortstr_t
        server_platform;
    icl_shortstr_t
        server_copyright;
    icl_shortstr_t
        server_information;
    icl_shortstr_t
        server_instance;
</context>

<method name = "new">
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *">Virtual host</argument>
    <argument name = "auth data" type = "icl_longstr_t *">Authentication data</argument>
    <argument name = "instance" type = "char*">Client instance name</argument>
    <argument name = "trace" type = "int">Trace level, 0 - 3</argument>
    <argument name = "timeout" type = "int">Connection timeout, msecs</argument>
    //
    icl_console_print ("I: making ZAMQ wireapi connection...");
    self->error_text = "no error";
    if (zamq_open (host) == 0)
        self->alive = TRUE;
</method>

<method name = "destroy">
    if (self->alive)
        zamq_close ();
</method>

<method name = "auth plain" return = "auth data">
    <doc>
    Formats a new authentication data block for a plain login.
    </doc>
    <argument name = "login"    type = "char *" pass = "in">User login name</argument>
    <argument name = "password" type = "char *" pass = "in">User password</argument>
    <declare name = "auth data" type = "icl_longstr_t *">Authentication data</declare>
    //
    auth_data = ipr_sasl_plain_encode (login, password);
</method>

<method name = "wait" template = "function">
    <argument name = "timeout" type = "int" pass = "in">Timeout, in milliseconds</argument>
    rc = -1;
</method>

<method name = "set silent" template = "function">
    <argument name = "silent" type = "Bool" />
    self->silent = silent;
</method>

<method name = "set timeout" template = "function">
    <argument name = "timeout" type = "Bool" />
    self->timeout = timeout;
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

<method name = "get version_major" return = "version_major">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "version_major" type = "int" />
    version_major = self->version_major;
</method>

<method name = "get version_minor" return = "version_minor">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "version_minor" type = "int" />
    version_minor = self->version_minor;
</method>

<method name = "get channel_max" return = "channel_max">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "channel_max" type = "int" />
    channel_max = self->channel_max;
</method>

<method name = "get frame_max" return = "frame_max">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "frame_max" type = "qbyte" />
    frame_max = self->frame_max;
</method>

<method name = "get heartbeat" return = "heartbeat">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "heartbeat" type = "int" />
    heartbeat = self->heartbeat;
</method>

<method name = "get known_hosts" return = "known_hosts">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "known_hosts" type = "char *" />
    known_hosts = self->known_hosts;
</method>

<method name = "get host" return = "host">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <declare name = "host" type = "char *" />
    host = self->host;
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

<method name = "selftest" />

</class>
