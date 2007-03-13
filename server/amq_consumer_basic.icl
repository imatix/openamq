<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name    = "amq_consumer_basic"
    comment = "Basic consumer"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements a Basic consumer.  For now, an empty object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_consumer_t
        *consumer;                      //  Parent consumer
</context>

<method name = "new">
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    //
    self->consumer = consumer;
</method>

<method name = "selftest" />

</class>
