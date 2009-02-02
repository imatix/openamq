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
    name    = "zyre_digest_amqp"
    comment = "Refresh authentication via Digest-AMQP"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    Refreshes expired or missing passwords using the protocol specified by
    the 8-Digest-AMQP@wiki.amqp.org specification.
</doc>

<!-- We're a backend for the access module portal -->
<!-- Here 'backend' means a class that acts as a portal server -->
<inherit class = "http_access_module_back" />

<!-- And we're the front-end for a backend module portal -->
<!-- Here 'backend' means a server that can speak AMQP or such -->
<inherit class = "zyre_backend_front" />

<import class = "wireapi" />
<import class = "zyre_classes" />

<context>
    http_access_module_t
        *portal;                        //  Portal back to http_server
    zyre_backend_t
        *backend;                       //  Backend peering to AMQP
    Bool
        connected;                      //  AMQP connection alive?
    icl_shortstr_t
        reply_queue;                    //  Queue name for replies
    smt_log_t
        *log;                           //  Log file for warnings
</context>

<method name = "new">
    self->backend = zyre_backend_amqp__zyre_backend_new (NULL);
    zyre_digest_amqp__zyre_backend_bind (self, self->backend);
    zyre_backend_request_start (self->backend);
</method>

<method name = "destroy">
    <action>
    self->portal = NULL;
    zyre_backend_unlink (&self->backend);
    smt_log_unlink (&self->log);
    </action>
</method>

<!-- These are requests coming in via the http_access_module portal -->

<method name = "announce">
    <action>
    //  We  don't link to the portal since the portal wraps this object and
    //  destroys it.  The portal receives responses back from us when we're
    //  invoked asynchronously.  Sorry this is kind of complex... in a portal
    //  request handler, 'portal' is the portal that sent us the request.
    //  In the 'arrived' method we get a request from one portal and send a
    //  response to a different one, namely this one back to http_server:
    self->portal = portal;

    self->log = smt_log_link (log);
    smt_log_print (log, "I: Zyre Digest-AMQP access module loaded");
    </action>
</method>

<method name = "before">
    if ((context->fail_code == IPR_PASSWORD_NOTFOUND
    ||   context->fail_code == IPR_PASSWORD_EXPIRED)
    &&  (context->digest && context->retries == 0)) {
        context->retries++;             //  Do exactly one retry, no more
        zyre_digest_amqp_send_request (self, context->fail_user, context->fail_realm);
        rc = HTTP_ACCESS_RETRY;         //  Server will retry afer 1 second
    }
</method>

<!-- This is an async method for internal use -->

<method name = "send request" template = "async function">
    <doc>
    Sends a Digest-AMQP request to the backend.  The response is handled
    asynchronously by the arrived / returned methods.
    </doc>
    <argument name = "user" type = "char * "/>
    <argument name = "realm" type = "char * "/>
    <possess>
    user = icl_mem_strdup (user);
    realm = icl_mem_strdup (realm);
    </possess>
    <release>
    icl_mem_strfree (&user);
    icl_mem_strfree (&realm);
    </release>
    <action>
    amq_content_basic_t
        *content;                       //  Content to post
    char
        *body;                          //  Formatted message body

    if (self->connected) {
        //  we need a private queue for responses back from server...
        body = ipr_str_format (
            "&amp;lt;digest-amqp version=\\"1.0\\"&amp;gt;"
                "&amp;lt;request"
                " user=\\"%s\\""
                " realm=\\"%s\\""
                " algorithm=\\"MD5\\""
                " reply_to=\\"%s\\""
                " /&amp;gt;"
            "&amp;lt;/digest-amqp&amp;gt;", user, realm, self->reply_queue);

        content = amq_content_basic_new ();
        amq_content_basic_set_body (content, body, strlen (body), icl_mem_free);
        //  Send to default exchange, with service name as routing key
        zyre_backend_request_forward (self->backend,
            NULL, "Digest-AMQP", content, TRUE, TRUE);
        amq_content_basic_unlink (&content);
        smt_log_print (self->log, "I: sent Digest-AMQP request for '%s:%s'", user, realm);
    }
    else
        smt_log_print (self->log, "W: could not send Digest-AMQP request for '%s:%s', not connected",
            user, realm);
    </action>
</method>

<!-- These are responses coming in via the zyre_backend portal -->

<method name = "online">
    <action>
    smt_log_print (self->log,
        "I: Digest-AMQP now peered to OpenAMQ server on %s",
        zyre_config_amqp_hostname (zyre_config));
    self->connected = TRUE;
    icl_shortstr_cpy (self->reply_queue, reply_queue);
    </action>
</method>

<method name = "offline">
    <action>
    smt_log_print (self->log,
        "I: Digest-AMQP unpeered from OpenAMQ server at %s",
        zyre_config_amqp_hostname (zyre_config));
    self->connected = FALSE;
    </action>
</method>

<method name = "arrived">
    <action>
    //  We got a response back from the Digest-AMQP service.  Parse it for
    //  a username, realm, and digest and ok, send those credentials to the
    //  http_server.
    //
    asl_reader_t
        reader;                         //  Body reader
    ipr_bucket_t
        *bucket;                        //  Body bucket
    ipr_xml_t
        *xml_root = NULL,
        *response;
    char
        *user,
        *realm,
        *digest;

    amq_content_basic_set_reader (method->content, &reader, 4096);
    bucket = amq_content_basic_replay_body (method->content, &reader);
    xml_root = ipr_xml_parse_bucket (bucket);
    response = ipr_xml_find_item (xml_root, "/digest-amqp/response");
    if (response) {
        user   = ipr_xml_attr_get (response, "user", NULL);
        realm  = ipr_xml_attr_get (response, "realm", NULL);
        digest = ipr_xml_attr_get (response, "digest", NULL);
        if (user && realm && digest) {
            icl_shortstr_t
                key;
            icl_shortstr_fmt (key, "%s:%s", user, realm);
            http_access_module_response_new_password (self->portal, key, digest, TRUE);
        }
        ipr_xml_unlink (&response);
    }
    ipr_bucket_unlink (&bucket);
    ipr_xml_destroy (&xml_root);
    </action>
</method>

<method name = "returned">
    <action>
    icl_console_print ("I: Digest-AMQP service is not running");
    </action>
</method>

</class>
