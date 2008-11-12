<?xml?>
<!--
    Copyright (c) 1996-2008 iMatix Corporation

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
    name    = "restms_amqp"
    comment = "HTTP plugin that maps RestMS to AMQP"
    version = "1.0"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a http plugin that maps RestMS requests to AMQP.
</doc>

<inherit class = "http_portal_back" />

<import class = "restapi" />

<public name = "header">
#define RESTMS_XML_ROOT     "restms"
</public>

<context>
    restapi_t
        *restapi;                        //  RestAPI connection to AMQP
</context>

<method name = "initialise">
    if (strused (restms_config_amqp_autorun (restms_config))) {
        icl_console_print ("Starting AMQP server: %s", restms_config_amqp_autorun (restms_config));
        restapi_openamq_start (restms_config_amqp_autorun (restms_config));
    }
</method>

<method name = "terminate">
    restapi_openamq_stop ();
</method>

<method name = "announce">
    icl_console_print ("I: initializing AMQP plugin on '%s'", path);
    icl_console_print ("I: connecting to AMQP server at %s",
        restms_config_amqp_hostname (restms_config));
    self->restapi = restapi_new (
        restms_config_amqp_hostname (restms_config),
        restms_config_amqp_username (restms_config),
        restms_config_amqp_password (restms_config));
</method>

<method name = "get">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request->pathinfo, resource)) {
        case RESTMS_URI_ROOT:
            s_get_root (self, request, response);
            break;
        case RESTMS_URI_FEEDS:
            s_get_feeds (self, request, response);
            break;
        case RESTMS_URI_FEED:
            s_get_feed (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "put">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request->pathinfo, resource)) {
        case RESTMS_URI_FEEDS:
            s_put_feeds (self, request, response);
            break;
        case RESTMS_URI_FEED:
            s_put_feed (self, request, response, resource);
            break;
        case RESTMS_URI_SINK:
            s_put_sink (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "post">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request->pathinfo, resource)) {
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "delete">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request->pathinfo, resource)) {
        case RESTMS_URI_FEED:
            s_delete_feed (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "head">
    <action>
    //   restapi_assert_alive (self->restapi);
    //  The AMQP plugin does not implement the HEAD method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "move">
    <action>
    //  The AMQP plugin does not implement the MOVE method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "copy">
    <action>
    //  The AMQP plugin does not implement the COPY method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<private name = "async header">
//  These are the types of URI we recognize for processing
#define RESTMS_URI_ROOT                1
#define RESTMS_URI_FEEDS               2
#define RESTMS_URI_FEED                3
#define RESTMS_URI_SINK                4
#define RESTMS_URI_UNKNOWN             5

static int
    s_identify_uri (char *pathinfo, char *resource);
static void
    s_get_root (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_put_feeds (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_get_feeds (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_put_feed (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *feed_name);
static void
    s_get_feed (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *feed_name);
static void
    s_delete_feed (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *feed_name);
static void
    s_put_sink (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *sink_name);
</private>

<private name = "async footer">
//  Identify a URI type from a supplied pathinfo and return the
//  type of the uri and the resource, which is the variable string
//  after the identified part of the path info.  Resource string
//  must be an icl_shortstr_t, or NULL.
//
static int
s_identify_uri (char *pathinfo, char *resource)
{
    int
        rc = RESTMS_URI_UNKNOWN;
    char
        *trailing = NULL;               //  URI following matched part

    if (streq (pathinfo, "/"))
        rc = RESTMS_URI_ROOT;
    else
    if (streq (pathinfo, "/feed"))
        rc = RESTMS_URI_FEEDS;
    else
    if (ipr_str_prefixed (pathinfo, "/feed/")) {
        trailing = ipr_str_defix (pathinfo, "/feed/");
        //  Feeds are identified by a non-null string that does not include
        //  any of '/', '?', '#', or whitespace.  User-specified feed URIs
        //  are not validated in any way except for length (limited to 128
        //  chars).
        if (ipr_regexp_eq ("^[^/?#`s]{1,128}$", trailing)) {
            rc = RESTMS_URI_FEED;
            if (resource)
                icl_shortstr_cpy (resource, trailing);
        }
    }
    else
    if (ipr_str_prefixed (pathinfo, "/sink/")) {
        rc = RESTMS_URI_SINK;
        if (resource)
            icl_shortstr_cpy (resource, ipr_str_defix (pathinfo, "/sink/"));
    }
    return (rc);
}

//  Return root container map
//
static void
s_get_root (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response)
{
    ipr_xml_tree_t
        *tree;

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "feed");
        ipr_xml_tree_leaf (tree, "uri", "%sfeed", response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "sink");
        ipr_xml_tree_leaf (tree, "uri", "%ssink", response->root_uri);
        ipr_xml_tree_leaf (tree, "types", "fanout direct topic headers rotator service");
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create a new feed and return the URI for that new feed
//
static void
s_put_feeds (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response)
{
    restapi_feed_t
        *feed;

    feed = restapi_feed_new (self->restapi, response->root_uri, NULL);
    assert (feed);
    s_get_feed (self, request, response, feed->name);
}

//  Return description of feed container
//
static void
s_get_feeds (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response)
{
    ipr_xml_tree_t
        *tree;

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "feed");
        ipr_xml_tree_leaf (tree, "size", "%d", self->restapi->feed_count);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create the specified feed, if it does not already exist
//
static void
s_put_feed (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *feed_name)
{
    restapi_feed_t
        *feed;

    feed = restapi_feed_table_search (self->restapi->feed_table, feed_name);
    if (feed == NULL) {
        feed = restapi_feed_new (self->restapi, response->root_uri, feed_name);
        assert (feed);
    }
    s_get_feed (self, request, response, feed->name);
}

//  Return information about the specified feed 
//
static void
s_get_feed (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *feed_name)
{
    restapi_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;

    feed = restapi_feed_table_search (self->restapi->feed_table, feed_name);
    if (feed) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
          ipr_xml_tree_open (tree, "feed");
            ipr_xml_tree_leaf (tree, "uri", feed->uri);
          ipr_xml_tree_shut (tree);
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTFOUND);
}

//  Delete the specified feed, if exists.  This method
//  is idempotent and the feed does not need to exist.
//
static void
s_delete_feed (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *feed_name)
{
    restapi_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;
    //
    feed = restapi_feed_table_search (self->restapi->feed_table, feed_name);
    if (feed)
        restapi_feed_destroy (&feed);

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create or assert specified sink.  This method is idempotent.
//
static void
s_put_sink (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *sink_name)
{
    int
        rc;
    ipr_xml_tree_t
        *tree;
    char
        *type;

    type = ipr_dict_table_headers_search (request->args, "type");
    if (strused (type)) {
        rc = restapi_sink_create (self->restapi, sink_name, type);
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        if (rc == 0)
            ipr_xml_tree_leaf (tree, "status", "ok");
        else {
            ipr_xml_tree_leaf (tree, "status", "error");
            ipr_xml_tree_leaf (tree, "reason", self->restapi->strerror);
        }
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_BADREQUEST);
}
</private>

<method name = "selftest" />

</class>
