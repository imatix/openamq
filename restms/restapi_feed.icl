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
a stream of messages arriving from the server.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>

<import class = "wireapi" />
<import class = "restapi" />

<context>
    restapi_t
        *restapi;                       //  Parent RestAPI instance
    int
        number;                         //  Our feed number
    amq_content_basic_list_t
        *contents;                      //  Messages for this feed
    ipr_time_t
        time_created,
        time_accessed;
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
        create new feed object
        create new private queue on server
        store feed in restapi structure
            - indexed by number (in an array)
            - reuse array slots when creating new feeds
        rc = restapi_purge_feeds (restapi)
            - purges old feeds
            - timeout is configurable
            - max feeds is configurable
-->

<method name = "new">
    <doc>
    Creates a new feed, within an existing RestAPI session.
    </doc>
    <argument name = "restapi" type = "restapi_t *" />
    //
    self->restapi = restapi_link (restapi);
</method>

<method name = "destroy">
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
    restapi_openamq_start ();
    restapi = restapi_new ("localhost:9000", "guest", "guest");
    assert (restapi);
    
    feed = restapi_feed_new (restapi);
    assert (feed);
    restapi_feed_destroy (&feed);

    restapi_destroy (&restapi);
    restapi_openamq_stop ();
</method>

</class>
