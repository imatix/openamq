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
    It communicates changes to this tree to the AMQP backend agent, which
    handles all communications with the AMQP server.  Incoming AMQP messages
    arrive via the backend portal.  All RestMS work is done by a single
    object in this class, so a single thread.  This limits the workload to
    around 300,000 transactions per second, which is the speedlimit of one
    threadlet (SMT thread queue limitation).
</doc>

<!-- Fun with Portals!  -->

<!-- We're a backend for the driver module portal -->
<!-- Here 'backend' means a class that acts as a portal server -->
<inherit class = "http_driver_module_back" />

<!-- We're the front-end for a backend module portal -->
<!-- Here 'backend' means a server that can speak AMQP or such -->
<inherit class = "zyre_backend_front" />

<!-- We're the front-end for a resource portal -->
<!-- That's a class that implements a RestMS resource -->
<inherit class = "zyre_resource_front" />

<public name = "header">
#define RESTMS_ROOT         "restms"
</public>

<context>
    zyre_backend_t
        *backend;                       //  Backend peering to AMQP
    Bool
        connected;                      //  Back-end connection alive?
    smt_log_t
        *log;                           //  Log file for warnings
    ipr_hash_table_t
        *resources;                     //  Table of resource URIs
</context>

<method name = "new">
    <local>
    zyre_resource_t
        *domain,
        *feed;
    </local>
    //
    self->resources = ipr_hash_table_new ();
    self->backend = zyre_backend_amqp__zyre_backend_new (NULL);
    zyre_restms__zyre_backend_bind (self, self->backend);
    zyre_backend_request_start (self->backend);

    //  Create domain resource
    domain = zyre_domain__zyre_resource_new (NULL,
        NULL, self->resources, "domain", "default");
    zyre_restms__zyre_resource_bind (self, domain);

    //  Create default feed
    feed = zyre_feed__zyre_resource_new (NULL,
        domain, self->resources, "feed", "default");
    zyre_restms__zyre_resource_bind (self, feed);
    zyre_resource_request_configure (feed, NULL, self->resources, self->backend);

    zyre_resource_unlink (&feed);
    zyre_resource_unlink (&domain);
</method>

<method name = "destroy">
    <action>
    ipr_hash_table_destroy (&self->resources);
    zyre_backend_unlink (&self->backend);
    smt_log_unlink (&self->log);
    </action>
</method>

<method name = "announce">
    <action>
    self->log = smt_log_link (log);
    smt_log_print (log, "I: Zyre RestMS driver loaded at '%s'", portal->path);
    smt_log_print (log, "I: - peering with AMQP server at '%s'",
        zyre_config_amqp_hostname (zyre_config));
    </action>
</method>

<method name = "get">
    <action>
    zyre_resource_t
        *resource;

    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: GET resource '%s'", context->request->pathinfo);

    if (zyre_restms_check_path (context) == 0) {
        //  Pathinfo is URI key into resource table
        resource = ipr_hash_lookup (self->resources, context->request->pathinfo);
        if (resource) {
            if (zyre_config_restms_debug (zyre_config))
                icl_console_print ("R: - resource is a %s",
                    zyre_resource_type_name (resource->type));
            if (zyre_resource_modified (resource, context->request)) {
                //  Use the Accept content type for our response
                http_response_set_content_type (context->response,
                    http_request_get_header (context->request, "accept"));
                zyre_resource_request_get (resource, context);
                if (!context->replied)
                    http_driver_context_reply_success (context, HTTP_REPLY_OK);
            }
            else
                http_driver_context_reply_error (context, HTTP_REPLY_NOTMODIFIED, NULL);
        }
        else
            http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
                "URI does not match a known resource");
    }
    if (zyre_config_restms_debug (zyre_config) && !context->pending) {
        if (context && context->response)
            icl_console_print ("R: - reply %d %s",
                context->response->reply_code, context->response->reply_text);
        else
            icl_console_print ("R: - ERROR: context=%pp c->response=%pp",
                context, context->response);
    }
    </action>
</method>

<method name = "put">
    <action>
    zyre_resource_t
        *resource;

    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: UPDATE resource '%s'", context->request->pathinfo);

    if (zyre_restms_check_path (context) == 0) {
        //  Pathinfo is URI key into resource table
        resource = ipr_hash_lookup (self->resources, context->request->pathinfo);
        if (resource) {
            if (zyre_config_restms_debug (zyre_config))
                icl_console_print ("R: - resource is a %s",
                    zyre_resource_type_name (resource->type));
            if (zyre_resource_unmodified (resource, context->request))
                http_driver_context_reply_error (context, HTTP_REPLY_PRECONDITION,
                    "resource was modified by another application");
            else {
                zyre_resource_request_put (resource, context);
                if (!context->replied)
                    http_driver_context_reply_success (context, HTTP_REPLY_OK);
            }
        }
        else
            http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
                "URI does not match a known resource");
    }
    if (zyre_config_restms_debug (zyre_config)) {
        if (context && context->response)
            icl_console_print ("R: - reply %d %s",
                context->response->reply_code, context->response->reply_text);
        else
            icl_console_print ("R: - ERROR: context=%pp c->response=%pp",
                context, context->response);
    }
    </action>
</method>

<method name = "delete">
    <action>
    zyre_resource_t
        *resource;

    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: DELETE resource '%s'", context->request->pathinfo);

    if (zyre_restms_check_path (context) == 0) {
        //  Pathinfo is URI key into resource table
        resource = ipr_hash_lookup (self->resources, context->request->pathinfo);
        if (resource) {
            if (zyre_config_restms_debug (zyre_config))
                icl_console_print ("R: - resource is a %s",
                    zyre_resource_type_name (resource->type));
            if (zyre_resource_unmodified (resource, context->request))
                http_driver_context_reply_error (context, HTTP_REPLY_PRECONDITION,
                    "resource was modified by another application");
            else {
                //  The resource is the portal so we can't destroy it from
                //  within a request, or the request ends up trying to use a
                //  dead portal.  So we do it here, and the DELETE method
                //  just needs to verify the legitimacy of the request.
                zyre_resource_request_delete (resource, context);
                if (!context->failed) {
                    //  Since we don't have a link to the resource, grab one
                    //  Otherwise the destructor gets very unhappy
                    resource = zyre_resource_link (resource);
                    zyre_resource_detach_from_parent (resource);
                    zyre_resource_destroy (&resource);
                    http_driver_context_reply_success (context, HTTP_REPLY_OK);
                }
            }
        }
        else                            //  Idempotent delete -> OK
            http_driver_context_reply_success (context, HTTP_REPLY_OK);
    }
    if (zyre_config_restms_debug (zyre_config)) {
        if (context && context->response)
            icl_console_print ("R: - reply %d %s",
                context->response->reply_code, context->response->reply_text);
        else
            icl_console_print ("R: - ERROR: context=%pp c->response=%pp",
                context, context->response);
    }
    </action>
</method>

<method name = "post">
    <action>
    zyre_resource_t
        *resource;

    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: POST to '%s'", context->request->pathinfo);

    if (zyre_restms_check_path (context) == 0) {
        //  Pathinfo is URI key into resource table
        resource = ipr_hash_lookup (self->resources, context->request->pathinfo);
        if (resource) {
            if (zyre_config_restms_debug (zyre_config))
                icl_console_print ("R: - resource is a %s",
                    zyre_resource_type_name (resource->type));
            zyre_resource_request_post (resource, context, self->resources, self->backend);
            if (!context->replied)
                http_driver_context_reply_success (context, HTTP_REPLY_OK);
        }
        else
            http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
                "URI does not match a known resource");
    }
    if (zyre_config_restms_debug (zyre_config)) {
        if (context && context->response)
            icl_console_print ("R: - reply %d %s",
                context->response->reply_code, context->response->reply_text);
        else
            icl_console_print ("R: - ERROR: context=%pp c->response=%pp",
                context, context->response);
    }
    </action>
</method>

<method name = "head">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "HEAD method is not allowed on this resource");
    </action>
</method>

<method name = "move">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "MOVE method is not allowed on this resource");
    </action>
</method>

<method name = "copy">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
        "COPY method is not allowed on this resource");
    </action>
</method>

<!-- These are responses methods from the zyre_backend portal -->

<method name = "online">
    <action>
    smt_log_print (self->log,
        "I: RestMS handler now peered to OpenAMQ server on %s",
        zyre_config_amqp_hostname (zyre_config));
    self->connected = TRUE;
    </action>
</method>

<method name = "offline">
    <action>
    smt_log_print (self->log,
        "I: RestMS handler unpeered from OpenAMQ server at %s",
        zyre_config_amqp_hostname (zyre_config));
    self->connected = FALSE;
    </action>
</method>

<method name = "arrived">
    <action>
    zyre_resource_t
        *pipe_res = NULL;
    icl_shortstr_t
        pipe_path;
    char
        *pipe_name;
    //
    //  The consumer tag should be in the form prefix:pipe-name
    pipe_name = strchr (method->payload.basic_deliver.consumer_tag, ':');
    if (pipe_name) {
        pipe_name++;                    //  Point to start of pipe name
        icl_shortstr_fmt (pipe_path, "/resource/%s", pipe_name);
        pipe_res = ipr_hash_lookup (self->resources, pipe_path);
    }
    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: - deliver message to pipe '%s' (%d bytes)",
            pipe_path, (int) ((amq_content_basic_t *) method->content)->body_size);

    if (pipe_res) {
        zyre_resource_t
            *message_res,
            *content_res;

        //  Reach through to pipe object to grab message asynclet
        message_res = ((zyre_pipe_t *) (pipe_res->server_object))->asynclet;

        //  Create new content resource as child of message
        content_res = zyre_content__zyre_resource_new (NULL,
            message_res, self->resources, "content", "");
        zyre_restms__zyre_resource_bind (self, content_res);

        //  We provide the new resources with the AMQP method so they can
        //  process & store the AMQP content.  Not really what attach() was
        //  meant for but it's nicer than making a new portal method.
        zyre_resource_request_attach (message_res, NULL, method);
        zyre_resource_request_attach (content_res, NULL, method);
        zyre_resource_unlink (&content_res);

        //  Re-configure the pipe, which creates a new asynclet
        zyre_resource_request_configure (pipe_res, NULL, self->resources, self->backend);

        //  And now re-send the message if the clients is waiting
        zyre_resource_request_get (message_res, NULL);
    }
    else
        smt_log_print (self->log, "W: undeliverable message (tag='%s')",
            method->payload.basic_deliver.consumer_tag);
    </action>
</method>

<method name = "returned">
    <action>
    icl_console_print ("E: RestMS content not routable - abonormal");
    </action>
</method>

<!-- These methods are responses from resources
     Everything happens synchronously between restms and its resource tree
     -->
<method name = "child add">
    <doc>
    Creates a resource from an XML document.  Classes that create resources
    in other ways have to do the hard work themselves.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <local>
    char
        *type;                          //  Resource type from XML
    zyre_resource_t
        *resource = NULL;
    </local>
    //
    //  Clean up the name and build resource path
    type = ipr_xml_name (context->xml_item);

    //  We create the resource and the server object instance
    //  The portal that called us is the parent of this new resource
    if (streq (type, "feed"))
        resource = zyre_feed__zyre_resource_new (NULL, portal, self->resources, type,
            ipr_xml_attr_get (context->xml_item, "name", ""));
    else
    if (streq (type, "pipe"))
        //  For pipes we ignore the name attribute
        resource = zyre_pipe__zyre_resource_new (NULL, portal, self->resources, type, "");
    else
    if (streq (type, "join"))
        //  For joins we ignore the name attribute
        resource = zyre_join__zyre_resource_new (NULL, portal, self->resources, type, "");
    else {
        icl_console_print ("E: not implemented: %s", type);
        assert (FALSE);
    }
    zyre_restms__zyre_resource_bind (self, resource);

    //  resource->hash is null if resource already existed with same path
    if (resource->hash) {
        //  Configure action must be safe, all checking has already been done
        zyre_resource_request_configure (resource, context, self->resources, self->backend);
        if (context->failed) {
            icl_console_print ("Configure action may not raise an error");
            assert (FALSE);
        }
        context->response->reply_code = HTTP_REPLY_CREATED;
    }
    else {
        //  Destroy the duplicate resource and grab the real one so we can
        //  report it back to the client application
        zyre_resource_t
            *actual;
        actual = ipr_hash_lookup (self->resources, resource->path);
        assert (actual);
        zyre_resource_destroy (&resource);
        resource = zyre_resource_link (actual);
    }
    if (!context->failed) {
        //  Set Location: header to new or existing resource
        http_response_set_header (context->response, "location",
            "%s%s%s", context->response->root_uri, RESTMS_ROOT, resource->path);

        //  Use the Content-Type of the posted document for our response
        http_response_set_content_type (context->response,
            http_request_get_header (context->request, "content-type"));

        //  Get resource description for client
        zyre_resource_request_get (resource, context);
    }
    //  We drop our link to the resource so that it is automatically
    //  destroyed when this object is destroyed, and we don't need to
    //  do anything further.  If we want to prematurely destroy the
    //  resource we'll need to grab a link back.
    zyre_resource_unlink (&resource);
</method>

<!-- Utility functions - general RestMS parsing & processing -->

<method name = "check path" return = "rc">
    <doc>
    From the current HTTP context, parses the pathinfo and returns an HTTP
    error reply if the pathinfo is not valid.  In that case, returns -1.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <declare name = "rc" type = "int" default = "0" />
    //
    //  path must be start with /{resource-type}/ or /resource/
    if (!ipr_str_prefixed (context->request->pathinfo, "/domain/")
    &&  !ipr_str_prefixed (context->request->pathinfo, "/feed/")
    &&  !ipr_str_prefixed (context->request->pathinfo, "/pipe/")
    &&  !ipr_str_prefixed (context->request->pathinfo, "/resource/")) {
        http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
            "URI path does not refer to a RestMS resource");
        rc = -1;
    }
</method>

<method name = "parse document" return = "rc">
    <doc>
    From the current HTTP context, parses the document and sets the context
    xml_root and xml_item properties.  If the required_type is specified, the
    document must have this type.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <argument name = "required" type = "char *">Required resource type</argument>
    <declare name = "rc" type = "int" default = "0" />
    //
    //  We accept only "application/restms+xml"
    //  JSON support will come later
    if (streq (context->request->content_type, "application/restms+xml"))
        rc = http_driver_context_xml_parse (context, RESTMS_ROOT, required);
    else {
        http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
            "Content-Type must be 'application/restms+xml'");
        rc = -1;
    }
</method>

<method name = "selftest" />

</class>
