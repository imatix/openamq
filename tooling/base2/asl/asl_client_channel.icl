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
    name      = "asl_client_channel"
    comment   = "ASL client channel class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    This class implements the ASL client channel class.
</doc>

<inherit class = "asl_channel" />

<context>
    smt_method_queue_t
        *method_queue;                  //  Reply queue for client api
    Bool
        *alive;                         //  Where to push alive status
    dbyte
        *reply_code;                    //  Where to push reply_code
    char
        *reply_text;                    //  Where to push reply_text
</context>

<method name = "new">
    <argument name = "method queue" type = "smt_method_queue_t *">Reply queue</argument>
    <argument name = "alive"        type = "Bool *">Where to push alive status</argument>
    <argument name = "reply code"   type = "dbyte *">Where to push reply code</argument>
    <argument name = "reply text"   type = "char *">Where to push reply text</argument>
    //
    self->method_queue = smt_method_queue_link (method_queue);
    self->alive        = alive;
    self->reply_code   = reply_code;
    self->reply_text   = reply_text;
</method>

<method name = "destroy">
    smt_method_queue_unlink (&self->method_queue);
</method>

</class>
