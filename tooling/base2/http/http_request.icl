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
    name      = "http_request"
    comment   = "HTTP request class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class holds properties and methods for parsing and representing
    an incoming HTTP request.
</doc>
<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "1" />
</inherit>

<import class = "http" />

<context>
    http_server_t
        *server;                        //  Parent server object
    smt_socket_t
        *socket;                        //  Incoming client socket
    ipr_bucket_t
        *header,                        //  Bucket holding header
        *content;                       //  Bucket holding content if any
    char
        *address,                       //  IP address of client
        *request_line,                  //  Full request line
        *request_method,                //  Method text from request line
        *request_uriargs,               //  Full URI from request line
        *request_path,                  //  URI path from request line
        *request_arguments,             //  URI arguments from request line
        *request_fragment,              //  URI fragment from request line
        *request_version;               //  HTTP version from request line
    char
        *path,                          //  Parsed URI path and name
        *pathroot,                      //  Part of path mapped by plugin
        *pathinfo;                      //  Part of path following pathroot
    uint
        method,                         //  Method identifier
        version;                        //  Version identifier
    size_t
        header_length,                  //  Size of header including eoln
        content_length,                 //  Parsed Content-Length any
        content_read;                   //  Amount read so far
    char
        *content_type,                  //  Content-Type header, if any
        *content_file;                  //  Content pipe name, if any
    FILE
        *content_pipe;                  //  Content pipe file, if any
    ipr_dict_table_t
        *headers,                       //  HTTP request header fields
        *args;                          //  Arguments, parsed as table
    Bool
        encrypted,                      //  HTTPS protocol?
        persistent;                     //  Do we keep the connection alive?
</context>

<public>
//  HTTP methods identifiers, non-zero
#define HTTP_METHOD_GET                 1
#define HTTP_METHOD_HEAD                2
#define HTTP_METHOD_POST                3
#define HTTP_METHOD_PUT                 4
#define HTTP_METHOD_DELETE              5
#define HTTP_METHOD_MOVE                6
#define HTTP_METHOD_COPY                7

//  HTTP version identifiers, non-zero
#define HTTP_VERSION_1_0                1
#define HTTP_VERSION_1_1                2

//  HTTP reply codes

#define HTTP_REPLY_OK                   200
#define HTTP_REPLY_CREATED              201
#define HTTP_REPLY_ACCEPTED             202
#define HTTP_REPLY_NOCONTENT            204
#define HTTP_REPLY_PARTIAL              206
#define HTTP_REPLY_MOVED                301
#define HTTP_REPLY_FOUND                302
#define HTTP_REPLY_METHOD               303
#define HTTP_REPLY_NOTMODIFIED          304
#define HTTP_REPLY_BADREQUEST           400
#define HTTP_REPLY_UNAUTHORIZED         401
#define HTTP_REPLY_PAYMENT              402
#define HTTP_REPLY_FORBIDDEN            403
#define HTTP_REPLY_NOTFOUND             404
#define HTTP_REPLY_PRECONDITION         412
#define HTTP_REPLY_TOOLARGE             413
#define HTTP_REPLY_INTERNALERROR        500
#define HTTP_REPLY_NOTIMPLEMENTED       501
#define HTTP_REPLY_OVERLOADED           503
#define HTTP_REPLY_VERSIONUNSUP         505
</public>

<private>
//  These are the methods we allow
static struct {
    char *name;
    uint ident;
} s_valid_methods [] = {
    { "GET",    HTTP_METHOD_GET    },
    { "HEAD",   HTTP_METHOD_HEAD   },
    { "POST",   HTTP_METHOD_POST   },
    { "PUT",    HTTP_METHOD_PUT    },
    { "DELETE", HTTP_METHOD_DELETE },
    { "MOVE",   HTTP_METHOD_MOVE   },
    { "COPY",   HTTP_METHOD_COPY   },
    {  NULL,    0                  }    //  End of table
};

//  These are the versions we allow
static struct {
    char *name;
    uint ident;
} s_valid_versions [] = {
    { "1.0",    HTTP_VERSION_1_0   },
    { "1.1",    HTTP_VERSION_1_1   },
    {  NULL,    0                  }    //  End of table
};
</private>

<!--
    This is an example of a HTTP request header:

    -------------------------------------------------------------------
    POST /cgi-bin/process HTTP/1.0
    If-Modified-Since: Sunday, 05-Nov-95 20:23:42 GMT; length=683
    Connection: Keep-Alive
    User-Agent: Mozilla/2.0GoldB2 (Win95; I)
    Host: www.imatix.com
    Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg

    [Post data ...]
    -------------------------------------------------------------------
 -->

<method name = "new">
    <doc>
    Creates a new HTTP request object.  The caller must provide the
    current socket, and a bucket holding the HTTP request header.
    </doc>
    <argument name = "server" type = "http_server_t *">Parent server</argument>
    <argument name = "socket" type = "smt_socket_t *">Client connection</argument>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket holding header</argument>
    //
    self->server  = http_server_link (server);
    self->socket  = socket;
    self->header  = ipr_bucket_link (bucket);
    self->headers = ipr_dict_table_new ();
    self->args    = ipr_dict_table_new ();
    self->version = HTTP_VERSION_1_0;
</method>

<method name = "destroy">
    //  Unlink shared objects
    ipr_bucket_unlink (&self->header);
    http_server_unlink (&self->server);

    //  External resources
    if (self->content_file)
        ipr_file_delete (self->content_file);

    //  Free strings
    icl_mem_free (self->address);
    icl_mem_free (self->request_line);
    icl_mem_free (self->request_method);
    icl_mem_free (self->request_uriargs);
    icl_mem_free (self->request_path);
    icl_mem_free (self->request_arguments);
    icl_mem_free (self->request_fragment);
    icl_mem_free (self->request_version);
    icl_mem_free (self->path);
    icl_mem_free (self->pathroot);
    icl_mem_free (self->pathinfo);
    icl_mem_free (self->content_file);

    //  Destroy own objects
    ipr_bucket_destroy (&self->content);
    ipr_dict_table_destroy (&self->headers);
    ipr_dict_table_destroy (&self->args);
</method>

<method name = "set address" template = "function">
    <doc>
    Stores the client IP address in the request, for use by other classes.
    Accepts an IP address string in dotted numeric format, "127.0.0.1".
    </doc>
    <argument name = "address" type = "char *" />
    //
    icl_mem_strfree (&self->address);
    self->address = icl_mem_strdup (address);
</method>

<method name = "parse" template = "function">
    <doc>
    Parse the HTTP request header and set the request context accordingly.
    Return zero if ok, non-zero if the request was invalid. After this call,
    the return code is HTTP_REPLY_OK if the request was valid, and an HTTP
    error reply code if not.
    </doc>
    //
    rc = s_parse_request (self, (char *) (self->header->data));
    if (rc == HTTP_REPLY_OK)
        rc = s_parse_headers (self, (char *) (self->header->data));
</method>

<method name = "content parse" template = "function">
    <doc>
    Parse the content headers and content body, if any was already read.
    Sets the request->content_length to the total expected, and content_read
    to the actual read so far.  If content_length > content_read, the caller
    should read again and call the content_append() method to store the
    additional content.  If the content fits into the configured bucket max,
    holds as a single bucket in request->content, else writes as a file in
    request->content_file. Returns number of octets still expected from the
    client socket.
    </doc>
    <local>
    byte
        *content_start;
    </local>
    //
    //  Store content length and type
    self->content_length = atoi (self_get_header (self, "content-length"));
    self->content_type = self_get_header (self, "content-type");

    //  Calculate size of header block
    content_start = ipr_bucket_find (self->header, 0, (byte *) "\\r\\n\\r\\n", 4);
    assert (content_start);
    content_start += 4;
    self->header_length = content_start - self->header->data;

    //  Calculate how much content we already have in the header bucket
    self->content_read = self->header->cur_size - self->header_length;
    if (self->content_length > (size_t) http_config_bucket_max (http_config)) {
        //  Generate random name for pipe file and write what we have
        self->content_file = icl_mem_alloc (ICL_SHORTSTR_MAX + 1);
        ipr_str_random (self->content_file, "POST_9x10.in");
        self->content_pipe = fopen (self->content_file, "w");
        assert (self->content_pipe);
        if (self->content_read)
            assert (fwrite (content_start, self->content_read, 1, self->content_pipe) == 1);
    }
    else
    if (self->content_length > 0) {
        //  Allocate a bucket with room for a binary zero
        //  This makes it safer for imperfect code that parses the bucket
        self->content = ipr_bucket_new (self->content_length + 1);
        ipr_bucket_fill (self->content, content_start, self->content_read);
        self->content->data [self->content->cur_size] = 0;
    }
    rc = self_content_pending (self);
</method>

<method name = "content pending" template = "function">
    <doc>
    Reports how many octets still need to be read from the post body in order
    to complete the content as specified by the content-length header.
    </doc>
    //
    rc = (int) (self->content_length - self->content_read);
</method>

<method name = "content append" template = "function">
    <doc>
    Appends a bucket of data to the request content.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket holding content</argument>
    //
    assert (self->content || self->content_pipe);
    if (self->content) {
        ipr_bucket_cat (self->content, bucket->data, bucket->cur_size);
        self->content->data [self->content->cur_size] = 0;
        self->content_read += bucket->cur_size;
    }
    else {
        assert (fwrite (bucket->data, bucket->cur_size, 1, self->content_pipe) == 1);
        self->content_read += bucket->cur_size;
        if (self->content_read == self->content_length)
            fclose (self->content_pipe);
    }
</method>

<method name = "get header" return = "header value">
    <doc>
    Gets the value of a specified header from the HTTP request.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to request object</argument>
    <argument name = "header name" type = "char *">Name of header, case insensitive</argument>
    <declare name = "header value" type = "char *">Returned value, or ""</declare>
    //
    header_value = ipr_dict_table_headers_search (self->headers, header_name);
</method>

<private name = "header">
static int
    s_parse_request ($(selftype) *self, char *data);
static int
    s_parse_headers ($(selftype) *self, char *data);
</private>

<private name = "footer">
//  Parse the Request-Line according to RFC2616 section 5.1. format:
//      Method SP Request-URI SP HTTP-Version CRLF
//  Returns 0 if OK, -1 if there was an error, and sets self->result.
//
static int
s_parse_request ($(selftype) *self, char *data)
{
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    uint
        index;                          //  Index into method table
    int
        rc = HTTP_REPLY_OK;             //  Return code

    regexp = ipr_regexp_new (
        //  We want to match these items from the request line:
        // $1 = request line
        // |$2 = method
        // ||     $3 = uriargs
        // ||     |$4 = pathname
        // ||     ||              $5 = arguments
        // ||     ||              |              $6 = fragment
        // ||     ||              |              |
        // ||     ||              |              |
        // ||     ||              |              |
        // ||     ||              |              |                $7 = version
        // ||     ||              |              |                |
         "^((`w+) (([^?#`s]*)(?:`?([^#`s]*))?(?:#(`s*))?)(?: HTTP/([0-9]`.[0-9]))?)\\r\\n");
    if (ipr_regexp_match (
        regexp,
        data,
        &self->request_line,
        &self->request_method,
        &self->request_uriargs,
        &self->request_path,
        &self->request_arguments,
        &self->request_fragment,
        &self->request_version) == 7)
    {
        char
            *path;                      //  Working copy of path

        ipr_str_upper (self->request_method);
        if (http_config_verbose (http_config)) {
            icl_console_print ("I: HTTP request -----------------------------------------");
            icl_console_print ("I: %s %s HTTP/%s",
                self->request_method, self->request_uriargs, self->request_version);
        }
        //  Clean-up path to remove things like '..'
        path = icl_mem_strdup (self->request_path);
        ipr_http_unescape_uri (path);
        ipr_str_subch (path, '\\\\', '/');
        self->path = ipr_path_resolve (path);
        icl_mem_free (path);            //  Finished with working copy

        //  Load arguments string into request->args
        ipr_dict_table_uri_load (self->args, self->request_arguments);

        //  Check that requested method is valid
        for (index = 0; s_valid_methods [index].name; index++) {
            if (streq (self->request_method, s_valid_methods [index].name)) {
                self->method = s_valid_methods [index].ident;
                break;
            }
        }
        //  Check that requested version is valid
        for (index = 0; s_valid_versions [index].name; index++) {
            if (streq (self->request_version, s_valid_versions [index].name)) {
                self->version = s_valid_versions [index].ident;
                break;
            }
        }
        if (!self->method)
            rc = HTTP_REPLY_BADREQUEST;
        else
        if (!self->version)
            rc = HTTP_REPLY_VERSIONUNSUP;
    }
    else {
        if (http_config_verbose (http_config)) {
            icl_console_print ("I: 400 - BADREQUEST: URI does not parse");
            icl_console_print ("I: %s", data);
        }
        rc = HTTP_REPLY_BADREQUEST;
    }
    ipr_regexp_destroy (&regexp);
    return (rc);
}

//  Parse HTTP headers and collect interesting header values into
//  request context.
//  Returns 0 if OK, -1 if there was an error, and sets self->result.
//
static int
s_parse_headers ($(selftype) *self, char *data)
{
    int
        rc = HTTP_REPLY_OK;             //  Return code
    char
        *host;                          //  Host: header value

    //  Create headers table and insert default value
    ipr_dict_assume (self->headers, "content-length", "0");
    ipr_dict_table_headers_load (self->headers, strchr (data, '\\n') + 1,
        http_config_verbose (http_config));

    //  Load and parse special headers
    //  --------------------------------------------------------------------

    //  RFC 2616 section 14.23 requires Host: header for HTTP/1.1 requests
    host = self_get_header (self, "host");
    if (*host) {
        //  Don't allow ., .., or / characters
        if (streq (host, ".") || streq (host, "..") || strchr (host, '/'))
            rc = HTTP_REPLY_BADREQUEST;
    }
    else
    if (self->version == HTTP_VERSION_1_1) {
        if (http_config_verbose (http_config))
            icl_console_print ("I: 400 - BADREQUEST: Missing Host: header");
        rc = HTTP_REPLY_BADREQUEST;
    }
    //  Set the persistent connection flag
    if (self->version == HTTP_VERSION_1_1)
        //  Use "connection: close" to determine persistent flag
        self->persistent = !ipr_dict_table_matches (self->headers, "connection", "(?i)^close");
    else
        //  Use "connection: keep-alive" to determine persistent flag
        self->persistent = ipr_dict_table_matches (self->headers, "connection", "(?i)^keep-alive");

    return (rc);
}
</private>

<method name = "selftest" />

</class>
