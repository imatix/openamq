<?xml?>
<!--
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
    name      = "zyre_feed"
    comment   = "Zyre feed entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS feed object.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        class,                          //  Feed class
        name;                           //  Feed name
    Bool
        as_queue;                       //  Implemented as AMQP queue?
</context>

<method name = "new">
    <argument name = "class" type = "char *">Feed class</argument>
    <argument name = "name" type = "char *">Feed name</argument>
    <dismiss argument = "key" value = "self->name">Key is feed name</dismiss>
    //
    assert (name);
    icl_shortstr_cpy (self->name, name);
    icl_shortstr_cpy (self->class, class);
    self->as_queue = (streq (class, "service") || streq (class, "rotator"));
</method>

<method name = "selftest">
    <local>
    zyre_feed_t
        *feed;
    zyre_feed_table_t
        *feed_table;                    //  Hashed table of feeds
    </local>
    //
    feed_table = zyre_feed_table_new ();
    feed = zyre_feed_new (feed_table, "direct", "my.test");
    assert (feed);
    zyre_feed_destroy (&feed);
    zyre_feed_table_destroy (&feed_table);
</method>

</class>
