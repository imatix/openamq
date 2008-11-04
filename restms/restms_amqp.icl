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
    ipr_bucket_t
        *bucket;                        //  Bucket we will
    icl_longstr_t
        *string;

    restapi_assert_alive (self->restapi);

    //  Get root path returns resource map
    if (streq (request->pathinfo, "/")) {
        //  This can be put into class
        //  http_response_xml (response, format, values);
        //  Allocated working buffers, arbitrary sizes
        bucket = ipr_bucket_new (1024);
        string = icl_longstr_new (NULL, 1024);
        icl_longstr_fmt (string,
            "[?xml version=\\"1.0\\"?]"
            "[amqp]"
                "[resource type=\\"feed\\" href=\\"%s\\"/]"
                "[resource type=\\"sink\\" href=\\"%s\\"/]"
            "[/amqp]",
            "some-feed-uri", "some_sink_uri");
        ipr_bucket_cat (bucket, string->data, string->cur_size);

        //  Store bucket in response bucket list
        bucket->data [bucket->cur_size++] = 0;
        ipr_str_subch ((char *) bucket->data, '[', '<');
        ipr_str_subch ((char *) bucket->data, ']', '>');
        http_response_set_dynamic (response, bucket, "text/xml");
        ipr_bucket_destroy (&bucket);
        icl_longstr_destroy (&string);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);

    http_portal_response_reply (portal, caller, response);

    /*
    if path = "."
        message_get
        HTTP_REPLY_OK or HTTP_NOTFOUND
            -> content body
    else
        resolve path
        if unknown
            HTTP_NOTFOUND
        else
        if selector empty
            sink_query
            HTTP_REPLY_OK or HTTP_NOTFOUND
                -> formatted properties HTML
        else
            selector_create
            message_get
            HTTP_REPLY_OK or HTTP_NOTFOUND
                -> content body
    */
    </action>
</method>

<method name = "head">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "post">
    <action>
    /*
    if path = "."
        HTTP_FORBIDDEN
    else
        ...
    */
    icl_console_print ("AMQP POST: %s::%s", request->path, request->request_arguments);
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "put">
    <action>
    restapi_feed_t
        *feed;
    ipr_bucket_t
        *bucket;                        //  Bucket we will
    icl_longstr_t
        *string;

    if (restapi_assert_alive (self->restapi)) {
        icl_console_print ("E: could not reconnect to %s",
            restms_config_amqp_hostname (restms_config));
        exit (1);
    }
    if (streq (request->path, "feed")) {
        feed = restapi_feed_new (self->restapi);

        //  Allocated working buffers, arbitrary sizes
        bucket = ipr_bucket_new (1024);
        string = icl_longstr_new (NULL, 1024);
        icl_longstr_fmt (string,
            "[?xml version=\\"1.0\\"?][amqp][feed href=\\"%s\\"/][/amqp]",
            "some-feed-uri");
        ipr_bucket_cat (bucket, string->data, string->cur_size);

        //  Store bucket in response bucket list
        bucket->data [bucket->cur_size++] = 0;
        ipr_str_subch ((char *) bucket->data, '[', '<');
        ipr_str_subch ((char *) bucket->data, ']', '>');

        http_response_set_dynamic (response, bucket, "text/html");
        ipr_bucket_destroy (&bucket);
        icl_longstr_destroy (&string);
        restapi_feed_unlink (&feed);
    }
    else
        http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);

    http_portal_response_reply (portal, caller, response);

    /*
    if path = "."
        message_nack
        HTTP_REPLY_OK or HTTP_NOTFOUND
    else
    if type specified
        sink_create
        HTTP_REPLY_OK or HTTP_FORBIDDEN
    else
        selector_create
        HTTP_REPLY_OK or HTTP_BADREQUEST
    */
    </action>
</method>

<method name = "delete">
    <action>
    /*
    if path = "."
        message_ack
        HTTP_REPLY_OK or HTTP_NOTFOUND
    else
        resolve_path
        if unknown
            do nothing
            HTTP_REPLY_OK
        else
        if selector empty
            sink_delete
            HTTP_REPLY_OK or HTTP_FORBIDDEN
        else
            selector_delete
            HTTP_REPLY_OK or HTTP_FORBIDDEN
    */
    icl_console_print ("AMQP DELETE: %s::%s", request->path, request->request_arguments);
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "move">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "copy">
    <action>
    http_response_set_error (response, HTTP_REPLY_NOTIMPLEMENTED);
    http_portal_response_reply (portal, caller, response);
    </action>
</method>

<method name = "selftest" />

</class>

