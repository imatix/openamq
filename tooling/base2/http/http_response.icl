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
    name      = "http_response"
    comment   = "HTTP response class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class holds properties and methods for representing an HTTP
    response and turning it into HTML to send back to the client.  The
    response is the primary object exchanged between all components,
    and it holds references to all other key objects.
</doc>
<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>

<import class = "http" />

<context>
    //  todo replace server->content_types with mime mapper
    //  No more need to peek into server object
    http_server_t
        *server;                        //  Parent server object
    http_request_t
        *request;                       //  Tied request object
    uint
        reply_code;                     //  Reply code, if not ok
    icl_shortstr_t
        reply_text;                     //  Reason for error, if any
    ipr_dict_table_t
        *headers;                       //  HTTP response header fields
    ipr_bucket_list_t
        *bucket_list;                   //  Bucket list to return
    char
        *filename;                      //  Filename to return, or
    icl_shortstr_t
        content_type;                   //  Content type for response
    size_t
        content_length;                 //  Content length for response
    icl_shortstr_t
        hostname,                       //  Host name to use
        root_uri;                       //  URI to root path
</context>

<method name = "new">
    <argument name = "server" type = "http_server_t *">Parent server</argument>
    <argument name = "request" type = "http_request_t *">Request context</argument>
    //
    self->server = http_server_link (server);
    self->request = http_request_link (request);
    self->reply_code = http_request_parse (self->request);
    self->headers = ipr_dict_table_new ();
    self->bucket_list = ipr_bucket_list_new ();
    s_format_root_uri (self);
</method>

<method name = "destroy">
    http_server_unlink (&self->server);
    http_request_unlink (&self->request);
    ipr_bucket_list_destroy (&self->bucket_list);
    ipr_dict_table_destroy (&self->headers);
    icl_mem_strfree (&self->filename);
</method>

<method name = "set header" template = "function">
    <doc>
    Sets the value of a specified header in the HTTP response. The header value
    is a printf format, followed by insertion arguments.
    </doc>
    <argument name = "header name" type = "char *">Name of header, case insensitive</argument>
    <argument name = "format" type = "char *">Format specifier</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    <local>
    char
        *string;                        //  Holds formatted value
    int
        str_size = 1024,
        fmt_size;
    </local>
    //
    //  Format a string with the value
    string = icl_mem_alloc (str_size + 1);
    fmt_size = apr_vsnprintf (string, str_size, format, args);
    if (fmt_size >= str_size) {
        //  If formatted value overflowed string, resize and repeat
        icl_mem_free (string);
        str_size = fmt_size;
        string = icl_mem_alloc (str_size + 1);
        fmt_size = apr_vsnprintf (string, str_size, format, args);
    }
    ipr_dict_assume (self->headers, header_name, string);
    icl_mem_free (string);
</method>

<method name = "set from file" template = "function">
    <doc>
    Sets the response to send the file headers back to the browser.  Sets the
    content length, modified date, ETag and content type from the file info.
    For GET methods, prepares to return the file content.
    </doc>
    <argument name = "filename" type = "char *">File to send</argument>
    <local>
    ipr_finfo_t
        *finfo;
    icl_shortstr_t
        mime_date;
    ipr_dict_t
        *content_type;
    char
        *etag = NULL,                   //  Calculated ETag, if any
        *extension;                     //  File extension, if any
    </local>
    //
    //  Set response to return file content iff method is GET
    icl_mem_strfree (&self->filename);
    if (self->request->method == HTTP_METHOD_GET)
        self->filename = icl_mem_strdup (filename);

    //  Calculate Content-Length: and Last-Modified: headers
    finfo = ipr_file_get_info (filename);
    self->content_length = (size_t) finfo->size;
    ipr_time_mime (finfo->mtime, mime_date);
    self_set_header (self, "last-modified", mime_date);

    //  Calculate ETag: header
    etag = ipr_file_etag (filename, http_config_etag (http_config));
    self_set_header (self, "etag", etag);
    icl_mem_free (etag);

    //  Calculate Content type from file extension
    extension = strchr (filename, '.');
    if (!extension)
        extension = "default";
    content_type = ipr_dict_table_search (self->server->content_types, extension);
    if (content_type)
        icl_shortstr_cpy (self->content_type, content_type->value);
    else
        icl_shortstr_cpy (self->content_type,  "*/*");

    ipr_finfo_destroy (&finfo);
</method>

<method name = "set from bucket" template = "function">
    <doc>
    Sets the response to send a bucket back to the browser. Updates the content
    length from the bucket and the content type if not null. If you send back
    multiple buckets, the last content type you specify will be used.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket to return</argument>
    <argument name = "content type" type = "char *">Content type</argument>
    //
    ipr_bucket_list_queue (self->bucket_list, bucket);
    self->bucket_list->cur_size += bucket->cur_size;
    self->content_length = (size_t) self->bucket_list->cur_size;
    if (content_type)
        icl_shortstr_cpy (self->content_type, content_type);
</method>

<method name = "set from xml str" template = "function">
    <doc>
    Sets the response to send an XML string back to the browser.  To make it
    easier to code, the string uses [ and ] instead of lt and gt.  Sets the
    content length and content type to text/xml.  Automatically prefixes the
    XML string with an ?xml tag.
    </doc>
    <argument name = "xml string" type = "char *">XML to return</argument>
    <local>
    ipr_bucket_t
        *bucket;                        //  Serialized XML
    char
        *xml_tag = "[?xml version=\\"1.0\\"?]";
    </local>
    //
    bucket = ipr_bucket_new (strlen (xml_tag) + strlen (xml_string) + 1);
    ipr_bucket_cat (bucket, xml_tag, strlen (xml_tag));
    ipr_bucket_cat (bucket, xml_string, strlen (xml_string) + 1);
    ipr_str_subch ((char *) bucket->data, '[', '<');
    ipr_str_subch ((char *) bucket->data, ']', '>');
    http_response_set_from_bucket (self, bucket, "text/xml");
    ipr_bucket_unlink (&bucket);
</method>

<method name = "set no cache" template = "function">
    <doc>
    Sets the response so that the browser does not cache the URI.
    </doc>
    //
    self_set_header (self, "cache-control", "no-cache");
    self_set_header (self, "expires", "0");
</method>

<method name = "set content type" template = "function">
    <doc>
    Sets the response content type.
    </doc>
    <argument name = "content type" type = "char *">Content type</argument>
    //
    icl_shortstr_cpy (self->content_type, content_type);
</method>

<method name = "set redirect" template = "function">
    <doc>
    Redirects the response to the specified URI.  The URI can be a relative
    URI in which case the browser is redirected to the same server and port,
    or an absolute URI on the same or other server.
    </doc>
    <argument name = "new uri" type = "char *">New URI location</argument>
    //
    //  Set Location: header into response headers table
    //  If we have an absolute URI with proto name, use as-is
    if (ipr_regexp_eq ("^[a-zA-Z0-9`.`-`+]+:", new_uri))
        self_set_header (self, "location", new_uri);
    else
        self_set_header (self, "location", "%s%s", self->root_uri, new_uri);

    self_set_error (self, HTTP_REPLY_FOUND, NULL);
</method>

<method name = "set error" template = "function">
    <doc>
    Sets the response reply code.
    </doc>
    <argument name = "reply code" type = "uint">Reply code</argument>
    <argument name = "reply text" type = "char *">Reply text</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    self->reply_code = reply_code;
    if (reply_text)
        apr_vsnprintf (self->reply_text, ICL_SHORTSTR_MAX, reply_text, args);
</method>

<method name = "format reply" template = "function">
    <doc>
    Formats a response header and (in case of errors) body and places
    these at the start of the response bucket list, before any buckets
    the list might contain.
    </doc>
    <local>
    icl_shortstr_t
        mime_date;                      //  Holds formatted date/time
    ipr_bucket_t
        *bucket;                        //  Holds formatted header
    icl_longstr_t
        *prefix;                        //  Holds formatted first line
    </local>
    //
    //  If we have an error, switch off keep-alive and format an error
    //  message as a bucket of dynamic response data.
    if (self->reply_code >= 400) {
        self->request->persistent = FALSE;
        s_format_error_page (self);
    }
    //  Define standard header fields
    ipr_time_mime (apr_time_now (), mime_date);
    self_set_header (self, "server", http_config_server_name (http_config));
    self_set_header (self, "date", mime_date);
    if (strused (self->content_type))
        self_set_header (self, "content-type", self->content_type);
    self_set_header (self, "content-length", "%u", self->content_length);

    //  Set connection header
    if (self->request->persistent) {
        if (self->request->version == HTTP_VERSION_1_0)
            self_set_header (self, "connection", "Keep-Alive");
    }
    else
        if (self->request->version == HTTP_VERSION_1_1)
            self_set_header (self, "connection", "Close");

    //  Format HTTP header block from header fields
    prefix = icl_longstr_new (NULL, 1024);
    icl_longstr_fmt (prefix, "HTTP/%s %d %s",
        self->request->version == HTTP_VERSION_1_0? "1.0": "1.1",
        self->reply_code, self->reply_text);

    if (http_config_verbose (http_config)) {
        icl_console_print ("I: HTTP response ----------------------------------------");
        icl_console_print ("I: %s", (char *) prefix->data);
    }
    icl_longstr_cat (prefix, "\\r\\n");
    bucket = ipr_dict_table_headers_save (
        self->headers, prefix, http_config_verbose (http_config));
    ipr_bucket_list_push (self->bucket_list, bucket);
    ipr_bucket_unlink (&bucket);
    icl_longstr_destroy (&prefix);
</method>

<private name = "header">
static void
    s_format_root_uri ($(selftype) *self);
static void
    s_format_error_page ($(selftype) *self);
</private>

<private name = "footer">
//  Formats the external URI to the root path /.  This URI is used
//  in any response that needs to refer the client application back
//  to the same service.  Also formats the response->hostname.
//
static void
    s_format_root_uri ($(selftype) *self)
{
    char
        *public_port;                   //  Current client http port

    //  Format proto://
    icl_shortstr_cpy (self->root_uri, self->request->encrypted? "https://": "http://");

    //  Format proto://hostname
    if (*http_request_get_header (self->request, "host"))
        icl_shortstr_cpy (self->hostname, http_request_get_header (self->request, "host"));
    else
        ipr_net_get_hostname (self->hostname);
    icl_shortstr_cat (self->root_uri, self->hostname);

    //  Format proto://hostname:port if necessary
    if (strchr (self->hostname, ':') == NULL) {
        public_port = http_config_public_port (http_config);
        if (*public_port && strneq (public_port, "80")) {
            icl_shortstr_cat (self->root_uri, ":");
            icl_shortstr_cat (self->root_uri, public_port);
        }
    }
    icl_shortstr_cat (self->root_uri, "/");
}

static void
s_format_error_page ($(selftype) *self)
{
    ipr_bucket_t
        *bucket;                        //  Bucket we will format
    char
        *text = "Unknown error";
    icl_shortstr_t
        error_filename;
    FILE
        *error_file;                    //  Handle to error_nnn.html file

    switch (self->reply_code) {
        case HTTP_REPLY_BADREQUEST:
            text = "Badly formed HTTP request: ";
            break;
        case HTTP_REPLY_UNAUTHORIZED:
            text = "Not authorized: ";
            break;
        case HTTP_REPLY_PAYMENT:
            text = "Payment is required: ";
            break;
        case HTTP_REPLY_FORBIDDEN:
            text = "Not authorised: ";
            break;
        case HTTP_REPLY_NOTFOUND:
            text = "Resource not found: ";
            break;
        case HTTP_REPLY_PRECONDITION:
            text = "Precondition failed: ";
            break;
        case HTTP_REPLY_TOOLARGE:
            text = "Request is too large: ";
            break;
        case HTTP_REPLY_INTERNALERROR:
            text = "Internal error: ";
            break;
        case HTTP_REPLY_NOTIMPLEMENTED:
            text = "Not implemented: ";
            break;
        case HTTP_REPLY_OVERLOADED:
            text = "Server overloaded, try again later: ";
            break;
        case HTTP_REPLY_VERSIONUNSUP:
            text = "Unsupported browser version: ";
            break;
    }
    //  Allocated working buffer, fairly arbitrary sizes
    bucket = ipr_bucket_new (65000);

    //  Load contents of error_nnn.html file, if present
    icl_shortstr_fmt (error_filename, "error_%d.html", self->reply_code);
    error_file = fopen (error_filename, "r");
    if (error_file) {
        ipr_bucket_load (bucket, error_file);
        bucket->data [bucket->cur_size++] = 0;
        fclose (error_file);
        http_response_set_from_bucket (self, bucket, "text/html");
    }
    else {
        ipr_bucket_cat (bucket, text, strlen (text));
        if (strused (self->reply_text))
            ipr_bucket_cat (bucket, self->reply_text, strlen (self->reply_text));
        else
            ipr_bucket_cat (bucket, "failed", strlen ("failed"));
        http_response_set_from_bucket (self, bucket, "text/plain");
    }
    //  Add bucket to list of buckets returned to user
    http_response_set_no_cache (self);
    ipr_bucket_unlink (&bucket);
}
</private>

<method name = "selftest" />

</class>

