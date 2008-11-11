<?xml?>
<!--
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
    name      = "restapi_feed"
    comment   = "OpenAMQ RESTful API - feeds"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestAPI feed object.  A feed is a container for
a stream of messages arriving from the server.  Feeds do not support
reference counting.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "restapi" />

<context>
    restapi_t
        *restapi;                       //  Parent RestAPI instance
    icl_shortstr_t
        name;                           //  Our feed name
    icl_shortstr_t
        uri;                            //  Full URI to feed
    amq_content_basic_list_t
        *contents;                      //  Messages for this feed
    apr_time_t
        time_created,
        time_accessed,
        session_timestamp;              //  Private queue timestamp
    size_t
        contents_pending,               //  Messages on queue
        contents_served,                //  Messages delivered
        octets_pending,                 //  Total bytes on queue
        octets_served;                  //  Total bytes delivered
    int
        top_message_id,                 //  Newest message on queue
        cur_message_id;                 //  Last message delivered
</context>

<!--
        rc = restapi_purge_feeds (restapi)
            - purges old feeds
            - timeout is configurable
            - max feeds is configurable
        rc = restapi_ distribute messages
            - take all arrived messages
            - send to feed queues according to consumer tag
-->

<method name = "new">
    <doc>
    Creates a new feed, within an existing RestAPI session.  If the feed
    could not be created, returns NULL.
    </doc>
    <argument name = "restapi" type = "restapi_t *">Parent RestAPI session</argument>
    <argument name = "root uri" type = "char *">Root URI</argument>
    <argument name = "name" type = "char *">Feed name</argument>
    <dismiss argument = "key" value = "self->name">Key is lease name</dismiss>
    <dismiss argument = "table" value = "self->restapi->feed_table">Use table in parent</dismiss>
    <local>
    size_t
        index = 0;
    </local>
    //
    self->restapi = restapi_link (restapi);
    self->restapi->feed_count++;
    if (name)
        icl_shortstr_cpy (self->name, name);
    else {
        ipr_str_random (self->name, "Ax16");
        icl_shortstr_fmt (self->name + strlen (self->name), "%d", ++index);
    }
    icl_shortstr_fmt (self->uri, "%sfeed/%s", root_uri, self->name);
</method>

<method name = "destroy">
    self->restapi->feed_count--;
    amq_content_basic_list_destroy (&self->contents);
    restapi_unlink (&self->restapi);
</method>

<method name = "selftest">
    <local>
    restapi_t
        *restapi;
    restapi_feed_t
        *feed;
    </local>
    icl_console_print ("Starting amq_server...");
    restapi_openamq_start ();
    restapi = restapi_new ("localhost:9000", "guest", "guest");
    assert (restapi);
    feed = restapi_feed_new (restapi, "http://localhost/", NULL);
    icl_console_print ("Feed URI is %s", feed->uri);
    assert (feed);
    restapi_feed_destroy (&feed);
    restapi_destroy (&restapi);
    icl_console_print ("Stopping amq_server...");
    restapi_openamq_stop ();
</method>

</class>
