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
#define RESTMS_XML_ROOT     "restms"
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
    zyre_resource_t
        *factory,
        *resource;

    self->resources = ipr_hash_table_new ();
    self->backend = zyre_backend_amqp__zyre_backend_new (NULL);
    zyre_restms__zyre_backend_bind (self, self->backend);
    zyre_backend_request_start (self->backend);

    //  Create resource factories and default resources
    factory = zyre_domain__zyre_resource_factory (NULL, NULL, NULL, "domain");
    zyre_restms__zyre_resource_bind (self, factory);
    resource = zyre_resource_factory (factory, NULL, self->resources, factory->name, "");
    zyre_restms__zyre_resource_bind (self, resource);
    zyre_resource_unlink (&resource);
    zyre_resource_unlink (&factory);
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
    smt_log_print (log, "I: searching for OpenAMQ server at '%s'",
        zyre_config_amqp_hostname (zyre_config));
    </action>
</method>

<method name = "get">
    <action>
    ipr_hash_t
        *hash;                          //  Hash table entry

    //  Pathinfo is URI key into resource table
    icl_console_print ("GET %s", context->request->pathinfo);
    hash = ipr_hash_table_search (self->resources, context->request->pathinfo);
    if (hash) {
        if (zyre_resource_modified ((zyre_resource_t *) hash->data, context->request))
            zyre_resource_request_get ((zyre_resource_t *) hash->data, context);
        else
            http_driver_context_reply_error (context, HTTP_REPLY_NOTMODIFIED, NULL);

        ipr_hash_unlink (&hash);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
            "The URI does not match a known resource");
    </action>
</method>

<method name = "put">
    <action>
    ipr_hash_t
        *hash;                          //  Hash table entry

    //  Pathinfo is URI key into resource table
    icl_console_print ("PUT %s", context->request->pathinfo);
    hash = ipr_hash_table_search (self->resources, context->request->pathinfo);
    if (hash) {
        if (zyre_resource_unmodified ((zyre_resource_t *) hash->data, context->request))
            http_driver_context_reply_error (context, HTTP_REPLY_PRECONDITION,
                "The resource was modified by another application");
        else
            zyre_resource_request_put ((zyre_resource_t *) hash->data, context);

        ipr_hash_unlink (&hash);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
            "The URI does not match a known resource");
    </action>
</method>

<method name = "delete">
    <action>
    ipr_hash_t
        *hash;                          //  Hash table entry

    //  Pathinfo is URI key into resource table
    icl_console_print ("DELETE %s", context->request->pathinfo);
    hash = ipr_hash_table_search (self->resources, context->request->pathinfo);
    if (hash) {
        if (zyre_resource_unmodified ((zyre_resource_t *) hash->data, context->request))
            http_driver_context_reply_error (context, HTTP_REPLY_PRECONDITION,
                "The resource was modified by another application");
        else
            zyre_resource_request_delete ((zyre_resource_t *) hash->data, context);

        ipr_hash_unlink (&hash);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
            "The URI does not match a known resource");
    </action>
</method>

<method name = "post">
    <action>
    ipr_hash_t
        *hash;                          //  Hash table entry

    //  Pathinfo is URI key into resource table
    icl_console_print ("POST %s", context->request->pathinfo);
    hash = ipr_hash_table_search (self->resources, context->request->pathinfo);
    if (hash) {
        zyre_resource_request_post ((zyre_resource_t *) hash->data, context, self->resources);
        ipr_hash_unlink (&hash);
    }
    else
        http_driver_context_reply_error (context, HTTP_REPLY_NOTFOUND,
            "The URI does not match a known resource");
    </action>
</method>

<method name = "head">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The HEAD method is not allowed on this resource");
    </action>
</method>

<method name = "move">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The MOVE method is not allowed on this resource");
    </action>
</method>

<method name = "copy">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The COPY method is not allowed on this resource");
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
    //  See old code
    </action>
</method>

<method name = "returned">
    <action>
    icl_console_print ("E: RestMS content not routable - abonormal");
    </action>
</method>

<method name = "selftest" />

</class>
