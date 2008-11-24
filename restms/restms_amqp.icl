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
        *restapi;                       //  RestAPI connection to AMQP
    size_t
        pipe_count;                     //  Number of pipes
    restapi_pipe_table_t
        *pipe_table;                    //  Hashed table of pipes
</context>

<method name = "new">
    self->restapi = restapi_new (
        restms_config_amqp_hostname (restms_config),
        restms_config_amqp_username (restms_config),
        restms_config_amqp_password (restms_config));
    self->pipe_table = restapi_pipe_table_new ();
</method>

<method name = "destroy">
    restapi_destroy (&self->restapi);
    restapi_pipe_table_destroy (&self->pipe_table);
</method>

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
</method>

<method name = "get">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request, resource)) {
        case RESTMS_URI_ROOT:
            s_get_root (self, request, response);
            break;
        case RESTMS_URI_PIPES:
            s_get_pipes (self, request, response);
            break;
        case RESTMS_URI_PIPE:
            s_get_pipe (self, request, response, resource);
            break;
        case RESTMS_URI_SINK:
            s_get_sink (self, request, response, resource);
            break;
        case RESTMS_URI_SELECTOR:
            s_get_selector (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "The GET method is not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "put">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request, resource)) {
        case RESTMS_URI_PIPES:
            s_put_pipes (self, request, response);
            break;
        case RESTMS_URI_PIPE:
            s_put_pipe (self, request, response, resource);
            break;
        case RESTMS_URI_SINK:
            s_put_sink (self, request, response, resource);
            break;
        case RESTMS_URI_SELECTOR:
            s_put_selector (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "The PUT method is not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "post">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request, resource)) {
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "The POST method is not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "delete">
    <action>
    icl_shortstr_t
        resource;

    switch (s_identify_uri (request, resource)) {
        case RESTMS_URI_PIPE:
            s_delete_pipe (self, request, response, resource);
            break;
        case RESTMS_URI_SINK:
            s_delete_sink (self, request, response, resource);
            break;
        case RESTMS_URI_SELECTOR:
            s_delete_selector (self, request, response, resource);
            break;
        default:
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                "The DELETE method is not allowed on this resource");
    }
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "head">
    <action>
    //   restapi_assert_alive (self->restapi);
    //  The AMQP plugin does not implement the HEAD method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
        "The HEAD method is not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "move">
    <action>
    //  The AMQP plugin does not implement the MOVE method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
        "The MOVE method is not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "copy">
    <action>
    //  The AMQP plugin does not implement the COPY method at all
    http_response_set_error (response, HTTP_REPLY_BADREQUEST,
        "The COPY method is not allowed on this resource");
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<private name = "async header">
//  These are the types of URI we recognize for processing
#define RESTMS_URI_ROOT                1
#define RESTMS_URI_PIPES               2
#define RESTMS_URI_PIPE                3
#define RESTMS_URI_SINK                4
#define RESTMS_URI_SELECTOR            5
#define RESTMS_URI_UNKNOWN             6

static int
    s_identify_uri (http_request_t *request, char *resource);
static void
    s_get_root (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_put_pipes (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);
static void
    s_get_pipes (restms_amqp_t *self,
        http_request_t *request, http_response_t *response);

static void
    s_put_pipe (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *pipe_name);
static void
    s_get_pipe (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *pipe_name);
static void
    s_delete_pipe (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *pipe_name);

static void
    s_put_sink (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);
static void
    s_get_sink (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);
static void
    s_delete_sink (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);

static void
    s_put_selector (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);
static void
    s_get_selector (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);
static void
    s_delete_selector (restms_amqp_t *self,
        http_request_t *request, http_response_t *response, char *path);
</private>

<private name = "async footer">
//  Identify a URI type from the supplied request and return the
//  type of the uri and the resource, which is the variable string
//  after the identified part of the path info.  Resource string
//  must be an icl_shortstr_t, or NULL.
//
//  /                                            root
//  /pipe                                        pipe container
//  /pipe/{pipename}                             pipe resource
//  /sink/{sinkpath}                             sink resource
//  /sink/{sinkpath}/{selector}?pipe={pipename}  selector resource

static int
s_identify_uri (
    http_request_t *request,
    char *resource)
{
    int
        rc = RESTMS_URI_UNKNOWN;
    char
        *trailing = NULL;               //  URI following matched part

    if (streq (request->pathinfo, "/"))
        rc = RESTMS_URI_ROOT;
    else
    if (streq (request->pathinfo, "/pipe"))
        rc = RESTMS_URI_PIPES;
    else
    if (ipr_str_prefixed (request->pathinfo, "/pipe/")) {
        trailing = ipr_str_defix (request->pathinfo, "/pipe/");
        //  Pipes are identified by a non-null string that does not include
        //  any of '/', '?', '#', or whitespace.  User-specified pipe URIs
        //  are not validated in any way except for length (limited to 128
        //  chars).
        if (ipr_regexp_eq ("^[^/?#`s]{1,128}$", trailing)) {
            rc = RESTMS_URI_PIPE;
            if (resource)
                icl_shortstr_cpy (resource, trailing);
        }
    }
    else
    if (ipr_str_prefixed (request->pathinfo, "/sink/")) {
        if (resource)
            icl_shortstr_cpy (resource, ipr_str_defix (request->pathinfo, "/sink/"));
        if (strused (ipr_dict_table_headers_search (request->args, "pipe")))
            rc = RESTMS_URI_SELECTOR;
        else
            rc = RESTMS_URI_SINK;
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
        ipr_xml_tree_leaf (tree, "type", "pipe");
        ipr_xml_tree_leaf (tree, "uri", "%spipe", response->root_uri);
      ipr_xml_tree_shut (tree);
      ipr_xml_tree_open (tree, "container");
        ipr_xml_tree_leaf (tree, "type", "sink");
        ipr_xml_tree_leaf (tree, "uri", "%ssink", response->root_uri);
        ipr_xml_tree_leaf (tree, "types", "fanout direct topic headers rotator service");
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create a new pipe and return the URI for that new pipe
//
static void
s_put_pipes (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response)
{
    restapi_pipe_t
        *pipe;

    pipe = restapi_pipe_new (self->pipe_table, self->restapi, NULL);
    self->pipe_count++;
    assert (pipe);
    s_get_pipe (self, request, response, pipe->name);
}

//  Return description of pipe container
//
static void
s_get_pipes (
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
        ipr_xml_tree_leaf (tree, "type", "pipe");
        ipr_xml_tree_leaf (tree, "size", "%d", self->pipe_count);
      ipr_xml_tree_shut (tree);
    http_response_set_from_xml (response, tree);
    ipr_xml_tree_destroy (&tree);
}

//  Create the specified pipe, if it does not already exist
//
static void
s_put_pipe (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *pipe_name)
{
    restapi_pipe_t
        *pipe;

    pipe = restapi_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe == NULL) {
        pipe = restapi_pipe_new (self->pipe_table, self->restapi, NULL);
        self->pipe_count++;
        assert (pipe);
    }
    s_get_pipe (self, request, response, pipe->name);
}

//  Return information about the specified pipe
//
static void
s_get_pipe (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *pipe_name)
{
    restapi_pipe_t
        *pipe;
    ipr_xml_tree_t
        *tree;

    pipe = restapi_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
          ipr_xml_tree_open (tree, "pipe");
            ipr_xml_tree_leaf (tree, "name", pipe_name);
            ipr_xml_tree_leaf (tree, "uri", "%spipe/%s", response->root_uri, pipe_name);
          ipr_xml_tree_shut (tree);
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTFOUND,
            "The requested pipe does not exist");
}

//  Delete the specified pipe, if exists.  This method is idempotent and the
//  pipe does not need to exist.
//
static void
s_delete_pipe (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *pipe_name)
{
    restapi_pipe_t
        *pipe;
    ipr_xml_tree_t
        *tree;
    //
    pipe = restapi_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe) {
        self->pipe_count--;
        restapi_pipe_destroy (&pipe);
    }
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
    char *path)
{
    char
        *type;
    ipr_xml_tree_t
        *tree;

    type = ipr_dict_table_headers_search (request->args, "type");
    if (strused (type)) {
        if (restapi_sink_create (self->restapi, path, type) == 0) {
            tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
            ipr_xml_tree_leaf (tree, "version", "1.0");
            ipr_xml_tree_leaf (tree, "status", "ok");
            http_response_set_from_xml (response, tree);
            ipr_xml_tree_destroy (&tree);
        }
        else
            http_response_set_error (response, HTTP_REPLY_BADREQUEST,
                self->restapi->strerror);
    }
    else
        http_response_set_error (response, HTTP_REPLY_BADREQUEST,
            "A type argument is mandatory for sink creation");
}

//  Return information about the specified sink
//
static void
s_get_sink (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *path)
{
    ipr_xml_tree_t
        *tree;

    if (restapi_sink_query (self->restapi, path) == 0) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
          ipr_xml_tree_open (tree, "sink");
            ipr_xml_tree_leaf (tree, "name", path);
            ipr_xml_tree_leaf (tree, "uri", "%ssink/%s", response->root_uri, path);
            asl_field_list_to_xml (self->restapi->properties, tree);
          ipr_xml_tree_shut (tree);
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTFOUND,
            self->restapi->strerror);
}

//  Delete the specified sink, if exists.  This method
//  is idempotent and the sink does not need to exist.
//
static void
s_delete_sink (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *path)
{
    ipr_xml_tree_t
        *tree;

    if (restapi_sink_delete (self->restapi, path) == 0) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_INTERNALERROR,
            self->restapi->strerror);
}

//  Create or assert specified selector.  This method is idempotent.
//
static void
s_put_selector (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *path)
{
    char
        *pipe;
    ipr_xml_tree_t
        *tree;

    pipe = ipr_dict_table_headers_search (request->args, "pipe");
    assert (strused (pipe));
    if (restapi_selector_create (self->restapi, path, pipe) == 0) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_BADREQUEST,
            self->restapi->strerror);
}

//  Return information about the specified selector
//
static void
s_get_selector (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *path)
{
    char
        *pipe;
    ipr_xml_tree_t
        *tree;

    pipe = ipr_dict_table_headers_search (request->args, "pipe");
    assert (strused (pipe));
    if (restapi_selector_query (self->restapi, path, pipe) == 0) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
          ipr_xml_tree_open (tree, "selector");
            ipr_xml_tree_leaf (tree, "uri", "%ssink/%s", response->root_uri, path);
            asl_field_list_to_xml (self->restapi->properties, tree);
          ipr_xml_tree_shut (tree);
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTFOUND,
            self->restapi->strerror);
}

//  Delete the specified selector, if exists.  This method
//  is idempotent and the selector does not need to exist.
//
static void
s_delete_selector (
    restms_amqp_t *self,
    http_request_t *request,
    http_response_t *response,
    char *path)
{
    char
        *pipe;
    ipr_xml_tree_t
        *tree;

    pipe = ipr_dict_table_headers_search (request->args, "pipe");
    assert (strused (pipe));
    if (restapi_selector_delete (self->restapi, path, pipe) == 0) {
        tree = ipr_xml_tree_new (RESTMS_XML_ROOT);
        ipr_xml_tree_leaf (tree, "version", "1.0");
        ipr_xml_tree_leaf (tree, "status", "ok");
        http_response_set_from_xml (response, tree);
        ipr_xml_tree_destroy (&tree);
    }
    else
        http_response_set_error (response, HTTP_REPLY_INTERNALERROR,
            self->restapi->strerror);
}
</private>

<method name = "selftest" />

</class>
