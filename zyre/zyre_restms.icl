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
    name    = "zyre_restms"
    comment = "HTTP plugin that maps RestMS to AMQP"
    version = "1.0"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a HTTP plugin that maps RestMS requests to AMQP.
    The plugin acts as a portal backend for the Base2/HTTP server.  This
    is an async class that manages a data tree of feeds, pipes, and joins.
    It communicates changes to this tree to the zyre_peering agent, which
    handles all communications with the AMQP server.  zyre_peering provides
    incoming AMQP messages to this agent via the deliver method.

    Table of methods that RestMS defines for each URI type:

                               PUT     GET   DELETE   POST
    RESTMS_URI_ROOT             -       Y       -       -
    RESTMS_URI_PIPES            Y       Y       -       -
    RESTMS_URI_PIPE             Y       Y       Y       -
    RESTMS_URI_FEEDS            -       Y       -       -
    RESTMS_URI_FEED             Y       Y       Y       -
    RESTMS_URI_ADDRESS          -       -       -       Y
    RESTMS_URI_JOIN             Y       Y       Y       -
    RESTMS_URI_MESSAGE          -       Y       Y       -
</doc>

<inherit class = "http_portal_back" />

<import class = "wireapi" />
<import class = "zyre_classes" />

<public name = "header">
#define RESTMS_XML_ROOT     "zyre"
</public>

<context>
    zyre_uri_t
        *uri;                           //  Parsed URI
    http_request_t
        *request;                       //  Current HTTP request
    http_response_t
        *response;                      //  Current HTTP response
    zyre_peering_t
        *peering;                       //  Peering to remote server
    zyre_pipe_table_t
        *pipe_table;                    //  Table of defined pipes
    zyre_feed_table_t
        *feed_table;                    //  Table of defined feeds
    Bool
        connected;                      //  Back-end connection alive?
</context>

<method name = "new">
    self->uri = zyre_uri_new (NULL);
    self->pipe_table = zyre_pipe_table_new ();
    self->feed_table = zyre_feed_table_new ();

    //  Prepare peering to AMQP server
    self->peering = zyre_peering_new (zyre_config_amqp_hostname (zyre_config),
                                      zyre_config_amqp_trace (zyre_config));
    zyre_peering_set_login           (self->peering, "peering");
    zyre_peering_set_status_handler  (self->peering, s_status_handler, self);
    zyre_peering_set_content_handler (self->peering, s_content_handler, self);
    zyre_peering_start               (self->peering);
</method>

<method name = "destroy">
    zyre_pipe_table_destroy (&self->pipe_table);
    zyre_feed_table_destroy (&self->feed_table);
    zyre_peering_destroy (&self->peering);
    zyre_uri_destroy (&self->uri);
</method>

<method name = "announce">
    icl_console_print ("I: initializing AMQP plugin on '%s'", path);
    icl_console_print ("I: connecting to AMQP server at %s",
        zyre_config_amqp_hostname (zyre_config));
</method>

<method name = "get">
    <action>
    self->request = request;
    self->response = response;
    switch (zyre_uri_set (self->uri, request->pathinfo)) {
        case RESTMS_URI_ROOT:
            zyre_restms_root_get (self);
            break;
        case RESTMS_URI_PIPES:
            zyre_restms_pipes_get (self);
            break;
        case RESTMS_URI_PIPE:
            zyre_restms_pipe_get (self);
            break;
        case RESTMS_URI_FEEDS:
            zyre_restms_feeds_get (self);
            break;
        case RESTMS_URI_FEED:
            zyre_restms_feed_get (self);
            break;
        case RESTMS_URI_JOIN:
            zyre_restms_join_get (self);
            break;
        case RESTMS_URI_MESSAGE:
            zyre_restms_message_get (self);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "GET method not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "put">
    <action>
    self->request = request;
    self->response = response;
    switch (zyre_uri_set (self->uri, request->pathinfo)) {
        case RESTMS_URI_PIPES:
            zyre_restms_pipes_put (self);
            break;
        case RESTMS_URI_PIPE:
            zyre_restms_pipe_put (self);
            break;
        case RESTMS_URI_FEED:
            zyre_restms_feed_put (self);
            break;
        case RESTMS_URI_JOIN:
            zyre_restms_join_put (self);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "PUT method not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "delete">
    <action>
    self->request = request;
    self->response = response;
    switch (zyre_uri_set (self->uri, request->pathinfo)) {
        case RESTMS_URI_PIPE:
            zyre_restms_pipe_delete (self);
            break;
        case RESTMS_URI_FEED:
            zyre_restms_feed_delete (self);
            break;
        case RESTMS_URI_JOIN:
            zyre_restms_join_delete (self);
            break;
        case RESTMS_URI_MESSAGE:
            zyre_restms_message_delete (self);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "DELETE method not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "post">
    <action>
    self->request = request;
    self->response = response;
    switch (zyre_uri_set (self->uri, request->pathinfo)) {
        case RESTMS_URI_ADDRESS:
            zyre_restms_address_post (self);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "POST method not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "head">
    <action>
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
        "HEAD method not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "move">
    <action>
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
            "MOVE method not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "copy">
    <action>
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
        "COPY method not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<!-- Implementations for each RestMS method -->

<method name = "root get" template = "function">
    <doc>
    Queries the RestMS topology, returns a list of available pipe and feed
    classes.
    </doc>
    <local>
    ipr_xml_tree_t
        *tree;
    </local>
    //
    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "pipe_class");
        ipr_xml_tree_leaf (tree, "name", "pipe");
        ipr_xml_tree_leaf (tree, "uri", "%spipe", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "fanout");
        ipr_xml_tree_leaf (tree, "uri", "%sfanout", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "direct");
        ipr_xml_tree_leaf (tree, "uri", "%sdirect", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "topic");
        ipr_xml_tree_leaf (tree, "uri", "%stopic", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "headers");
        ipr_xml_tree_leaf (tree, "uri", "%sheaders", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "service");
        ipr_xml_tree_leaf (tree, "uri", "%sservice", self->response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", "rotator");
        ipr_xml_tree_leaf (tree, "uri", "%srotator", self->response->root_uri);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
</method>

<method name = "pipes put" template = "function">
    <doc>
    Creates a server-named pipe.  This method returns the name for the new
    pipe.  It is not idempotent, and each invocation will create a new pipe.
    </doc>
    zyre_pipe_t
        *pipe;

    pipe = s_pipe_assert (self, NULL);
    if (pipe) {
        s_report_pipe (self, pipe);
        zyre_pipe_unlink (&pipe);
    }
</method>

<method name = "pipes get" template = "function">
    <doc>
    Queries the pipe class.  This method returns information about the pipe
    class.  It is idempotent and has no side effects.
    </doc>
    <local>
    ipr_xml_tree_t
        *tree;
    </local>
    //
    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "pipe_class");
        ipr_xml_tree_leaf (tree, "name", "pipe");
        ipr_xml_tree_leaf (tree, "size", "%d", self->pipe_table->nbr_items);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
</method>

<method name = "pipe put" template = "function">
    <doc>
    Creates a client-named pipe of the specified class.  The method is 
    idempotent.  If the pipe already exists, asserts that the class is
    accurate.
    </doc>
    zyre_pipe_t
        *pipe;

    pipe = s_pipe_assert (self, self->uri->pipe_name);
    if (pipe) {
        s_report_pipe (self, pipe);
        zyre_pipe_unlink (&pipe);
    }
</method>

<method name = "pipe get" template = "function">
    <doc>
    Queries the specified pipe.  The method returns information about the
    pipe.  It is idempotent and has no side effects.  Asserts that the pipe
    class is accurate.
    </doc>
    zyre_pipe_t
        *pipe;

    pipe = zyre_pipe_table_search (self->pipe_table, self->uri->pipe_name);
    if (!pipe)
        http_response_set_error (self->response, HTTP_REPLY_NOTFOUND,
            "The requested pipe does not exist");
    else
    if (streq (pipe->class, self->uri->pipe_class))
        s_report_pipe (self, pipe);
    else
        http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
            "The requested pipe class is wrong");

    zyre_pipe_unlink (&pipe);
</method>

<method name = "pipe delete" template = "function">
    <doc>
    Deletes the specified pipe.  This method is idempotent.  If the pipe
    exists, asserts that the class is accurate before deleting it.
    </doc>
    zyre_pipe_t
        *pipe;
    ipr_xml_tree_t
        *tree;

    pipe = zyre_pipe_table_search (self->pipe_table, self->uri->pipe_name);
    if (pipe) {
        if (streq (pipe->class, self->uri->pipe_class)) {
            zyre_peering_pipe_delete (self->peering, pipe->name);
            zyre_pipe_destroy (&pipe);
        }
        else {
            rc = -1;                    //  Class mismatch
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The requested pipe does not exist");
            zyre_pipe_unlink (&pipe);
        }
    }
    if (!rc) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (self->response, tree);
        ipr_xml_tree_destroy (&tree);
    }
</method>

<method name = "feeds get" template = "function">
    <doc>
    Queries the feed class.  This method returns the set of feeds defined
    in that class.  It is idempotent and has no side effects.
    </doc>
    ipr_xml_tree_t
        *tree;

    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "feed_class");
        ipr_xml_tree_leaf (tree, "name", self->uri->feed_class);
        zyre_feed_table_apply (self->feed_table, s_report_feed_item, tree);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
</method>

<method name = "feed put" template = "function">
    <doc>
    Creates a feed of the specified class, with the specified name.  The
    method is idempotent (it can be repeated with no further effect).  If
    the feed already exists, asserts that the class is accurate.
    </doc>
    zyre_feed_t
        *feed;

    feed = s_feed_assert (self);
    if (feed) {
        s_report_feed (self, feed);
        zyre_feed_unlink (&feed);
    }
</method>

<method name = "feed get" template = "function">
    <doc>
    Queries the specified feed.  The method returns information about the
    feed.  It is idempotent and has no side effects.  Asserts that the
    class is accurate.
    </doc>
    zyre_feed_t
        *feed;

    feed = zyre_feed_table_search (self->feed_table, self->uri->feed_name);
    if (!feed)
        http_response_set_error (self->response, HTTP_REPLY_NOTFOUND,
            "The requested feed does not exist");
    else
    if (streq (feed->class, self->uri->feed_class))
        s_report_feed (self, feed);
    else
        http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
            "The requested feed class is wrong");
    zyre_feed_unlink (&feed);
</method>

<method name = "feed delete" template = "function">
    <doc>
    Deletes the specified feed.  This method is idempotent.  If the feed
    exists, asserts that the class is accurate before deleting it.
    </doc>
    zyre_feed_t
        *feed;
    ipr_xml_tree_t
        *tree;

    feed = zyre_feed_table_search (self->feed_table, self->uri->feed_name);
    if (feed) {
        if (streq (feed->class, self->uri->feed_class)) {
            zyre_peering_feed_delete (self->peering, feed->name);
            zyre_feed_destroy (&feed);
        }
        else {
            rc = -1;                    //  Class mismatch
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The requested feed class is wrong");
            zyre_feed_unlink (&feed);
        }
    }
    if (!rc) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (self->response, tree);
        ipr_xml_tree_destroy (&tree);
    }
</method>

<method name = "join put" template = "function">
    <doc>
    Creates a join as specified.  The method is idempotent.  The PUT method
    creates the pipe if the pipe does not already exist.  It also creates
    the feed if the feed does not exist, and a feed class is specified.  If
    the feed exists and the URI includes a feed class, the method asserts
    that the class matches.
    </doc>
    zyre_pipe_t
        *pipe;
    zyre_feed_t
        *feed;
    zyre_join_t
        *join;

    pipe = s_pipe_assert (self, self->uri->pipe_name);
    feed = s_feed_assert (self);
    if (pipe && feed) {
        join = s_join_assert (self, pipe, self->uri->address, feed);
        if (join) {
            s_report_join (self, join);
            zyre_join_unlink (&join);
        }
    }
    zyre_feed_unlink (&feed);
    zyre_pipe_unlink (&pipe);
</method>

<method name = "join get" template = "function">
    <doc>
    Queries the specified join.  The method returns information about the join.
    It is idempotent and has no side effects.  The feed, pipe, and join must
    all exist.  If the URI includes a feed class, the method asserts that the
    class is accurate.
    </doc>
    zyre_pipe_t
        *pipe = NULL;
    zyre_feed_t
        *feed = NULL;
    zyre_join_t
        *join = NULL;

    pipe = zyre_pipe_table_search (self->pipe_table, self->uri->pipe_name);
    if (!pipe) {
        http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
            "The join pipe does not exist");
        rc = -1;
    }
    if (!rc) {
        feed = zyre_feed_table_search (self->feed_table, self->uri->feed_name);
        if (feed) {
            if (*self->uri->feed_class && strneq (self->uri->feed_class, feed->class)) {
                http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                    "The requested feed class does not match");
                rc = -1;
            }
        }
        else {
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The join feed does not exist");
            rc = -1;
        }
    }
    if (!rc) {
        join = zyre_join_lookup (pipe, self->uri->address, feed->name);
        if (join)
            s_report_join (self, join);
        else
            http_response_set_error (self->response, HTTP_REPLY_NOTFOUND,
                "The requested join does not exist");
    }
    zyre_join_unlink (&join);
    zyre_feed_unlink (&feed);
    zyre_pipe_unlink (&pipe);
</method>

<method name = "join delete" template = "function">
    <doc>
    Deletes the specified join.  This method is idempotent.  None of the feed,
    pipe, nor join need exist.  If the method includes a feed class and the
    feed and the join exist, the delete method will first check that the class
    is accurate before proceding.
    </doc>
    zyre_pipe_t
        *pipe = NULL;
    zyre_feed_t
        *feed = NULL;
    zyre_join_t
        *join = NULL;
    ipr_xml_tree_t
        *tree;

    pipe = zyre_pipe_table_search (self->pipe_table, self->uri->pipe_name);
    if (pipe) {
        feed = zyre_feed_table_search (self->feed_table, self->uri->feed_name);
        if (feed
        && *self->uri->feed_class && strneq (self->uri->feed_class, feed->class)) {
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The requested feed class does not match");
            rc = -1;
        }
        else {
            join = zyre_join_lookup (pipe, self->uri->address, self->uri->feed_name);
            if (join) {
                zyre_peering_join_delete (
                    self->peering, pipe->name, self->uri->address, self->uri->feed_name);
                zyre_join_destroy (&join);
            }
        }
    }
    zyre_feed_unlink (&feed);
    zyre_pipe_unlink (&pipe);
    if (!rc) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (self->response, tree);
        ipr_xml_tree_destroy (&tree);
    }
</method>

<method name = "address post" template = "function">
    <doc>
    Send the message to the feed, addressed using the specified address 
    string. If the URI specifies a feed class, the feed will be created if 
    it does not exist, and if the feed exists, the POST method will check 
    that the feed class matches (and reply with a PRECONDITION error if not).
    </doc>
    zyre_feed_t
        *feed;
    amq_content_basic_t
        *content;                       //  Content to post

    if (ipr_str_prefixed (self->request->content_type, "multipart"))
        http_response_set_error (self->response, HTTP_REPLY_NOTIMPLEMENTED,
            "Multipart contents not supported");
    else {
        feed = s_feed_assert (self);
        if (feed) {
            content = amq_content_basic_new ();
            amq_content_basic_record_body (content, self->request->content);
            zyre_peering_address_post (
                self->peering, self->uri->address, self->uri->feed_name, content);
            amq_content_basic_unlink (&content);
            zyre_feed_unlink (&feed);
        }
    }
</method>

<method name = "message get" template = "function">
    <doc>
    </doc>
    http_response_set_error (self->response, HTTP_REPLY_NOTIMPLEMENTED,
        "Not yet implemented");
</method>

<method name = "message delete" template = "function">
    <doc>
    </doc>
    http_response_set_error (self->response, HTTP_REPLY_NOTIMPLEMENTED,
        "Not yet implemented");
</method>

<method name = "deliver" template = "async function" async = "1">
    <doc>
    Deliver a message to a pipe.  The chain of execution is as follows.  The 
    AMQP server delivers a message to the zyre_peering object across the 
    peering connection (a Basic.Deliver method).  The zyre_peering object
    passes this to the s_content_handler callback function, still in the
    same thread.  The s_content_handler sends us (zyre_restms) an asynch
    method with the message content, so that the message can be pushed onto
    the appropriate pipe, as defined by the consumer tag.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">Incoming content</argument>
    <argument name = "consumer tag" type = "char *">Consumer tag for routing</argument>
    <action>
    char
        *pipe_name;
    zyre_pipe_t
        *pipe;

    //  The consumer tag should be in the form prefix:pipe-name
    pipe_name = strchr (consumer_tag, ':');
    if (pipe_name) {
        pipe_name++;                    //  Point to start of pipe name
        pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
        if (pipe) {
            zyre_pipe_deliver (pipe, content);
            zyre_pipe_unlink (&pipe);
        }
        else
            icl_console_print ("W: undeliverable message ('%s')", consumer_tag);
    }
    <action>
</method>

<private name = "header">
//  Return XML descriptions on various resources
static void
    s_report_pipe (zyre_restms_t *self, zyre_pipe_t *pipe);
static void
    s_report_feed (zyre_restms_t *self, zyre_feed_t *feed);
static void
    s_report_join (zyre_restms_t *self, zyre_join_t *join);
static void
    s_report_feed_item (zyre_feed_t *feed, void *argument);

//  Create/assert resources
static zyre_pipe_t *
    s_pipe_assert (zyre_restms_t *self, char *pipe_name);
static zyre_feed_t *
    s_feed_assert (zyre_restms_t *self);
static zyre_join_t *
    s_join_assert (zyre_restms_t *self, zyre_pipe_t *pipe, char *address, zyre_feed_t *feed);

//  Callbacks from the zyre_peering agent
static int s_status_handler (
    void *caller, zyre_peering_t *peering, Bool connected);
static int s_content_handler (
    void *caller, zyre_peering_t *peering, zyre_peer_method_t *method);
</private>

<private name = "footer">
static void
s_report_pipe (zyre_restms_t *self, zyre_pipe_t *pipe)
{
    ipr_xml_tree_t
        *tree;

    assert (pipe);
    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "pipe");
        ipr_xml_tree_leaf (tree, "name", pipe->name);
        ipr_xml_tree_leaf (tree, "uri",  pipe->uri);
        ipr_xml_tree_leaf (tree, "messages", amq_content_basic_list_count (pipe->contents));
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
}

static void
s_report_feed (zyre_restms_t *self, zyre_feed_t *feed)
{
    ipr_xml_tree_t
        *tree;

    assert (feed);
    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
    s_report_feed_item (feed, tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Adds one feed item to an XML tree

static void
s_report_feed_item (zyre_feed_t *feed, void *argument)
{
    ipr_xml_tree_t
        *tree = argument;

    ipr_xml_tree_open (tree, "feed");
      ipr_xml_tree_leaf (tree, "name", feed->name);
      ipr_xml_tree_leaf (tree, "uri",  feed->uri);
    ipr_xml_tree_shut (tree);
}

static void
s_report_join (zyre_restms_t *self, zyre_join_t *join)
{
    ipr_xml_tree_t
        *tree;

    assert (join);
    tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
    ipr_xml_tree_leaf (tree, "version", "1.0");
    ipr_xml_tree_leaf (tree, "status", "ok");
      ipr_xml_tree_open (tree, "pipe");
        ipr_xml_tree_leaf (tree, "name", join->pipe->name);
        ipr_xml_tree_leaf (tree, "uri",  join->pipe->uri);
          ipr_xml_tree_open (tree, "join");
            ipr_xml_tree_leaf (tree, "address", join->address);
            ipr_xml_tree_leaf (tree, "feed", join->feed_name);
            ipr_xml_tree_leaf (tree, "uri",  join->uri);
          ipr_xml_tree_shut (tree);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (self->response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Name is provided explicitly to allow server-named pipes
static zyre_pipe_t *
s_pipe_assert (zyre_restms_t *self, char *pipe_name)
{
    zyre_pipe_t
        *pipe = NULL;

    //  Lookup pipe if explicitly named, create if not found, or auto-named
    if (pipe_name)
        pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe) {
        if (strneq (pipe->class, self->uri->pipe_class)) {
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The requested pipe class is wrong");
            zyre_pipe_unlink (&pipe);
        }
    }
    else
    if (*self->uri->pipe_class) {
        pipe = zyre_pipe_new (self->pipe_table, self->uri->pipe_class, pipe_name);
        assert (pipe);
        zyre_pipe_set_uri (pipe, "%s%s/%s", self->response->root_uri, pipe->class, pipe->name);
        zyre_peering_pipe_create (self->peering, pipe->class, pipe->name);
    }
    else
        //  Pipe not found, and can't create, since no class specified
        http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
            "The requested pipe does not exist");

    return (pipe);
}

static zyre_feed_t *
s_feed_assert (zyre_restms_t *self)
{
    zyre_feed_t
        *feed;

    feed = zyre_feed_table_search (self->feed_table, self->uri->feed_name);
    if (feed) {
        if (*self->uri->feed_class && strneq (self->uri->feed_class, feed->class)) {
            http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
                "The requested feed class does not match");
            zyre_feed_unlink (&feed);
        }
    }
    else
    if (*self->uri->feed_class) {
        feed = zyre_feed_new (self->feed_table, self->uri->feed_class, self->uri->feed_name);
        assert (feed);
        zyre_feed_set_uri (feed, "%s%s/%s", self->response->root_uri, feed->class, feed->name);
        zyre_peering_feed_create (self->peering, feed->class, feed->name);
    }
    else
        //  Feed not found, and can't create, since no class specified
        http_response_set_error (self->response, HTTP_REPLY_PRECONDITION,
            "The requested feed does not exist");

    return (feed);
}

static zyre_join_t *
s_join_assert (zyre_restms_t *self, zyre_pipe_t *pipe, char *address, zyre_feed_t *feed)
{
    zyre_join_t
        *join;

    join = zyre_join_lookup (pipe, address, feed->name);
    if (join == NULL) {
        join = zyre_join_new (pipe, address, feed->name, feed->class);
        zyre_join_set_uri (join, "%s%s/%s/%s@%s/%s",
            self->response->root_uri,
            pipe->class, pipe->name,
            address,
            feed->name, feed->class);
        zyre_peering_join_create (
            self->peering,
            pipe->name,
            address,
            feed->name,
            feed->class);
    }
    return (join);
}

static int
s_status_handler (void *caller, zyre_peering_t *peering, Bool connected)
{
    zyre_restms_t
        *self = caller;

    self->connected = connected;
    return (0);
}

static int
s_content_handler (void *caller, zyre_peering_t *peering, zyre_peer_method_t *method)
{
    zyre_restms_t
        *self = caller;
    amq_content_basic_t
        *content;

    assert (method->class_id  == ZYRE_PEER_BASIC);
    assert (method->method_id == ZYRE_PEER_BASIC_DELIVER);

    content = (amq_content_basic_t *) method->content;
    zyre_restms_deliver (self, content, method->payload.basic_deliver.consumer_tag);
    return (0);
}
</private>

<method name = "selftest" />

</class>
