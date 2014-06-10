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
    name      = "http_driver_context"
    comment   = "HTTP driver context class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class holds properties for driver control.  These properties are
    provided to and modified by the various driver modules.  The object is
    for local use by driver modules.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <!-- Passed to async portal back-end -->
    <option name = "links" value = "1" />
</inherit>

<import class = "http" />

<context>
    smt_thread_t
        *thread;                        //  Calling http_agent thread
    http_request_t
        *request;                       //  HTTP request object
    http_response_t
        *response;                      //  HTTP response object
    smt_log_t
        *access_log,                    //  Access log file
        *alert_log,                     //  Alert log file
        *debug_log;                     //  Debug log file
    Bool
        failed,                         //  Did request result in error?
        replied,                        //  Don't sent multiple replies
        pending;                        //  Did void reply
    ipr_xml_t
        *xml_tree,                      //  Parsed XML content, if any
        *xml_root,                      //  Document root (real XML root)
        *xml_item;                      //  First element if any
</context>

<method name = "new">
    <argument name = "thread" type = "smt_thread_t *" />
    <argument name = "server" type = "http_server_t *" />
    <argument name = "request" type = "http_request_t *" />
    <argument name = "response" type = "http_response_t *" />
    //
    assert (request);
    assert (response);
    self->thread = smt_thread_link (thread);
    self->request  = http_request_link (request);
    self->response = http_response_link (response);
    self->alert_log = smt_log_link (server->alert_log);
    self->debug_log = smt_log_link (server->debug_log);
</method>

<method name = "destroy">
    smt_thread_unlink (&self->thread);
</method>

<method name = "free">
    http_request_unlink  (&self->request);
    http_response_unlink (&self->response);
    smt_log_unlink (&self->alert_log);
    smt_log_unlink (&self->debug_log);

    ipr_xml_unlink (&self->xml_item);
    ipr_xml_unlink (&self->xml_root);
    ipr_xml_unlink (&self->xml_tree);
</method>

<method name = "init" template = "function">
    <argument name = "request" type = "http_request_t *" />
    <argument name = "response" type = "http_response_t *" />
    //
    self->replied = FALSE;
    http_request_unlink  (&self->request);
    http_response_unlink (&self->response);
    self->request  = http_request_link (request);
    self->response = http_response_link (response);
</method>

<method name = "reply error" template = "function">
    <doc>
    Sends a reply event back to the http_agent with an error code and text.
    Sets the context->failed property to TRUE.
    </doc>
    <argument name = "reply code" type = "uint">Reply code</argument>
    <argument name = "reply text" type = "char *">Reply text</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    assert (self->response);
    self->response->reply_code = reply_code;
    if (reply_text)
        apr_vsnprintf (self->response->reply_text, ICL_SHORTSTR_MAX, reply_text, args);
    self->failed = TRUE;
    self_reply (self);
</method>

<method name = "reply success" template = "function">
    <doc>
    Sends a reply event back to the http_agent with a 2xx or 3xx reply code.
    Note that we only format reply text bodies for 4xx and 5xx codes.
    </doc>
    <argument name = "reply code" type = "uint">Reply code</argument>
    //
    assert (self->response);
    self->response->reply_code = reply_code;
    self_reply (self);
</method>

<method name = "reply void" template = "function">
    <doc>
    Sets the context->replied status but does not actually send a reply
    method back to the http thread, which will wait.  This lets us do a
    long poll, and reply at some later stage to the same context.  To
    use this, objects should grab a link to the context so it and the http
    thread will hang around.
    </doc>
    //
    assert (!self->replied);
    self->replied = TRUE;
    self->pending = TRUE;
</method>

<method name = "reply" template = "function">
    <doc>
    Sends a reply event back to the http_agent.
    </doc>
    assert (!self->replied);
    http_agent_driver_reply (self->thread);
    self->replied = TRUE;
</method>

<method name = "abort" template = "function">
    <doc>
    Sends an abort event back to the http_agent.
    </doc>
    assert (!self->replied);
    http_agent_driver_abort (self->thread);
    self->replied = TRUE;
</method>

<method name = "xml parse" template = "function">
    <doc>
    Attempts to parse the request content as XML, returns 0 if this was
    successful, else -1.  Applications should check the content-type before
    doing this.  Sets the context->xml_tree, xml_root and xml_item.  If the
    content could not be parsed, signals an error reply to the http_agent.
    If the content was empty, creates a tree with an empty root and empty
    item, if specified.
    </doc>
    <argument name = "root" type = "char *">Default root, or null</argument>
    <argument name = "item" type = "char *">Default item, or null</argument>
    //
    ipr_xml_unlink (&self->xml_item);
    ipr_xml_unlink (&self->xml_root);
    ipr_xml_unlink (&self->xml_tree);

    if (self->request->content_length) {
        if (self->request->content) {
            if (ipr_xml_load_bucket (&self->xml_tree, self->request->content, FALSE) == 0) {
                self->xml_root = ipr_xml_first_child (self->xml_tree);
                if (self->xml_root)
                    self->xml_item = ipr_xml_first_child (self->xml_root);

                //  Check document root and first item
                if (root
                && (self->xml_root == NULL
                ||  strneq (ipr_xml_name (self->xml_root), root)))
                    self_reply_error (self, HTTP_REPLY_BADREQUEST,
                        "document root must be '&lt;%s&gt;'", root);
                else
                if (item
                && (self->xml_item == NULL
                ||  strneq (ipr_xml_name (self->xml_item), item)))
                    self_reply_error (self, HTTP_REPLY_BADREQUEST,
                        "first element must be '&lt;%s&gt;'", item);
            }
            else {
                self_reply_error (self, HTTP_REPLY_BADREQUEST,
                    "XML content is not correctly formed");
                ipr_xml_destroy (&self->xml_tree);
            }
        }
        else
            //  If the content can't fit into a bucket, it's too large
            //  to fit into memory and would probably kill the server...
            self_reply_error (self, HTTP_REPLY_TOOLARGE,
                "document is too large (limit is %), rejected", IPR_BUCKET_MAX_SIZE);
    }
    else {
        //  If we don't have any content, create a default empty tree
        self->xml_tree = ipr_xml_new (NULL, "root", NULL);
        if (root) {
            self->xml_root = ipr_xml_new (self->xml_tree, root, NULL);
            if (item)
                self->xml_item = ipr_xml_new (self->xml_root, item, NULL);
        }
    }
    if (self->failed)
        rc = -1;
</method>

<method name = "selftest" />

</class>
