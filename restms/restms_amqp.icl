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
    comment = "Rest/AMQP processing plugin"
    version = "1.0"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a web server plugin to handle Rest/AMQP requests.
</doc>

<inherit class = "http_portal_back" />

<import class = "restapi" />

<public name = "header">
#define RESTMS_XML_ROOT     "amqprest"
</public>

<context>
    restapi_t
        *restapi;                        //  RestAPI connection to AMQP
</context>

<method name = "announce">
    icl_console_print ("I: initializing AMQP/Rest plugin on '%s'", path);
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
#define RESTMS_URI_SINKS               3
#define RESTMS_URI_FEED                4
#define RESTMS_URI_UNKNOWN             5

static int
    s_identify_uri (char *pathinfo, char *resource);
static void
    s_get_root (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_get_feed (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *resource);
static void
    s_put_feeds (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_delete_feed (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *resource);
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
    if (streq (pathinfo, "/sink"))
        rc = RESTMS_URI_SINKS;
    else
    if (ipr_str_prefixed (pathinfo, "/feed/")) {
        trailing = ipr_str_defix (pathinfo, "/feed/");
        //  A valid feed ID is a string of upper-case alphanum characters
        //  The restapi class generates IDs of 16 alphanumeric characters
        if (ipr_regexp_eq ("^[0-9A-Z]{16}$", trailing)) {
            rc = RESTMS_URI_FEED;
            if (resource)
                icl_shortstr_cpy (resource, trailing);
        }
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
    icl_shortstr_t
        location;

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "feed");
        ipr_xml_tree_leaf (tree, "uri", http_response_uri (request, location, "/feed"));
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "sink");
        ipr_xml_tree_leaf (tree, "uri", http_response_uri (request, location, "/sink"));
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create a new feed and return the URI for that new
//  feed resource
//
static void
s_put_feeds (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response)
{
    restapi_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;

    feed = restapi_feed_new (self->restapi);
    if (feed) {
        //  Format URI to feed
        http_response_uri (request, feed->uri, "/feed/");
        icl_shortstr_cat (feed->uri, feed->ident);

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
        http_response_set_error (response, HTTP_REPLY_OVERLOADED);
}

//  Return information about the specified feed resource
//
static void
s_get_feed (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *resource)
{
    restapi_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;

    feed = (restapi_feed_t *) restapi_resource_lookup (self->restapi, resource);
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

//  Delete the specified feed resource, if exists.  This method
//  is idempotent and the feed does not need to exist, but it
//  must be an accurate feed identifier.
//
static void
s_delete_feed (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *resource)
{
    restapi_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;

    feed = (restapi_feed_t *) restapi_resource_lookup (self->restapi, resource);
    if (feed)
        restapi_feed_destroy (&feed);

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}
</private>

<method name = "selftest" />

</class>
