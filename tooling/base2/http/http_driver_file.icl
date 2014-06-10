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
    name    = "http_driver_file"
    comment = "HTTP file driver module"
    version = "1.0"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    This class implements a driver module to handle normal files.  This
    is an async class that replies by sending methods (to the front end,
    which is the http_server object).
</doc>

<inherit class = "http_driver_module_back" />

<context>
    icl_shortstr_t
        root;                           //  File system root that path maps to
</context>

<method name = "announce">
    icl_shortstr_cpy (self->root, http_config_webroot (http_config));
    smt_log_print (log, "I: X5 file driver loaded at '%s', root is '%s'", portal->path, self->root);
    if (!ipr_file_is_directory (self->root))
        smt_log_print (log, "E:  - directory '%s' does not exist or is unreadable", self->root);
</method>

<method name = "destroy">
    <action>
    </action>
</method>

<method name = "get">
    <action>
    icl_shortstr_t
        disk_path,
        dir_index;

    //  Tracking down http://www.xitami.com/issue:1
    assert (self);
    assert (self->root);
    assert (context);
    assert (context->request);
    assert (context->request->pathinfo);

    //  Calculate real filename for resource
    //  Should become a method, using self->rootdir
    icl_shortstr_fmt (disk_path, "%s/%s", self->root, context->request->pathinfo);
    if (http_config_verbose (http_config))
        smt_log_print (context->debug_log, "I: Mapped pathname=%s", disk_path);

    if (ipr_file_is_directory (disk_path)) {
        if (strlast (disk_path) == '/') {
            char
                *defaults = http_config_defaults (http_config);

            //  URI specifies a directory, look for default file or .httpdir
            while (defaults && *defaults) {
                icl_shortstr_fmt (disk_path, "%s/%s%s",
                    self->root, context->request->pathinfo, defaults);
                if (strchr (disk_path, ' '))
                    *strchr (disk_path, ' ') = 0;
                if (ipr_file_is_readable (disk_path)) {
                    if (http_config_verbose (http_config))
                        smt_log_print (context->debug_log,
                            "I: Found default filename=%s", disk_path);
                    break;
                }
                defaults = strchr (defaults, ' ');
                if (defaults)
                    defaults++;         //  Skip to start of next filename
            }
            //  If we found a defaults file, send it, else look for .httpdir
            if (defaults && *defaults) {
                http_response_set_from_file (context->response, disk_path);
                http_response_set_no_cache  (context->response);
            }
            else {
                icl_shortstr_fmt (dir_index, "%s%s", disk_path, ".httpdir");
                if (ipr_file_exists (dir_index))
                    s_format_directory_listing (self, context, disk_path, dir_index);
                else
                    http_response_set_error (context->response, HTTP_REPLY_FORBIDDEN,
                        "Not allowed to list contents of directory");
            }
        }
        else {
            //  Redirect browser so it asks for the URI ending in /
            icl_shortstr_t
                clean_path;
            icl_shortstr_fmt (clean_path, "%s/", context->request->path);
            http_response_set_redirect (context->response, clean_path);
        }
    }
    else
    if (ipr_file_is_readable (disk_path))
        s_reply_for_file (self, context, disk_path);
    else
    if (!strchr (disk_path, '.'))
        s_try_default_extensions (self, context, disk_path);
    else
        http_response_set_error (context->response, HTTP_REPLY_NOTFOUND,
            "The requested resource does not exist");
    http_driver_context_reply (context);
    </action>
</method>

<method name = "head">
    <action>
    icl_shortstr_t
        disk_path;

    assert (self);
    assert (self->root);
    assert (context);
    assert (context->request);
    assert (context->request->pathinfo);

    //  Calculate real filename for resource
    //  Should become a method, using self->rootdir
    icl_shortstr_fmt (disk_path, "%s/%s", self->root, context->request->pathinfo);
    if (http_config_verbose (http_config))
        smt_log_print (context->debug_log, "I: Mapped filename=%s", disk_path);

    if (ipr_file_is_readable (disk_path))
        s_reply_for_file (self, context, disk_path);
    else
    if (!strchr (disk_path, '.'))
        s_try_default_extensions (self, context, disk_path);
    else
        http_response_set_error (context->response, HTTP_REPLY_NOTFOUND,
            "The requested resource does not exist");
    http_driver_context_reply (context);
    </action>
</method>

<method name = "post">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "POST method is not allowed on this resource");
    </action>
</method>

<method name = "put">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "PUT method is not allowed on this resource");
    </action>
</method>

<method name = "delete">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "DELETE method is not allowed on this resource");
    </action>
</method>

<method name = "move">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "MOVE method is not allowed on this resource");
    </action>
</method>

<method name = "copy">
    <action>
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "COPY method is not allowed on this resource");
    </action>
</method>

<private name = "async header">
//  Insert URI
//  At some stage this would be better abstracted into a class
#define DIR_HEADER_1 \\\\
    "[!DOCTYPE html PUBLIC \\\\"-//W3C//DTD XHTML 1.0 Strict//EN\\\\"\\\\n" \\\\
    "    \\\\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\\\\"]\\\\n" \\\\
    "[html]\\\\n" \\\\
    "[head]\\\\n" \\\\
    "  [title]Directory listing for %s[/title]\\\\n" \\\\
    "  [style type=\\\\"text/css\\\\"]\\\\n" \\\\
    ""
#define DIR_HEADER_2 \\\\
    "  [/style]\\\\n" \\\\
    "  [link rel=\\\\"stylesheet\\\\" href=\\\\"/http.css\\\\" type=\\\\"text/css\\\\"/]\\\\n" \\\\
    "[/head]\\\\n" \\\\
    "[body]\\\\n" \\\\
    "  [div id=\\\\"dir_head\\\\"]Directory listing for %s[/div]\\\\n" \\\\
    "  [div id=\\\\"dir_text\\\\"]\\\\n" \\\\
    ""
#define DIR_HEADER_3 \\\\
    "  [/div]\\\\n" \\\\
    "  [div id=\\\\"dir_main\\\\"]\\\\n" \\\\
    "    [table class=\\\\"dir_table\\\\"]\\\\n" \\\\
    ""
//  Insert class, URI, filename, filename, size, time
#define DIR_ENTRY \\\\
    "[tr class=\\\\"dir_row\\\\"]\\\\n" \\\\
    "  [td class=\\\\"%s\\\\"]&amp;amp;nbsp;[/td]\\\\n" \\\\
    "  [td class=\\\\"dir_name\\\\"][a href=\\\\"%s%s\\\\"]%s[/a][/td]\\\\n" \\\\
    "  [td class=\\\\"dir_size\\\\"]%s[/td]\\\\n" \\\\
    "  [td class=\\\\"dir_time\\\\"]%s[/td]\\\\n" \\\\
    "[/tr]\\\\n" \\\\
    ""
#define DIR_FOOTER \\\\
    "    [/table]\\\\n" \\\\
    "  [/div]\\\\n" \\\\
    "  [div id=\\\\"footer\\\\"]Powered by X5[/div]\\\\n" \\\\
    "[/body]\\\\n" \\\\
    "[/html]\\\\n" \\\\
    ""

static void s_reply_for_file
    ($(selftype) *self, http_driver_context_t *context, char *filename);
static void s_format_directory_listing
    ($(selftype) *self, http_driver_context_t *context, char *disk_path, char *dir_index);
static void s_try_default_extensions
    ($(selftype) *self, http_driver_context_t *context, char *filename);
</private>

<private name = "async footer">
//  We have a file, now check whether we need to return it, or whether
//  the browser already has a cached copy.  If the method is GET, returns
//  the whole file.  If HEAD, returns just the headers.

static void s_reply_for_file (
    $(selftype) *self,
    http_driver_context_t *context,
    char *filename)
{
    char
        *etag = NULL;
    //  ISO times have a resolution of 1 second
    time_t
        if_modified,
        file_time;

    //  Test the 'If-None-Match: "entity-tag"' header
    //  Conditional GET, 304 Not Modified if entry has the same Etag
    //
    etag = ipr_file_etag (filename, http_config_etag (http_config));

    file_time = (time_t) (ipr_file_time (filename) / 1000000);
    if_modified = (time_t) (ipr_time_mime_decode (
        http_request_get_header (context->request, "if-modified-since")) / 1000000);

    if (streq (etag, http_request_get_header (context->request, "if-none-match")))
        http_response_set_error (context->response, HTTP_REPLY_NOTMODIFIED, NULL);
    else
    if (if_modified && file_time <= if_modified)
        http_response_set_error (context->response, HTTP_REPLY_NOTMODIFIED, NULL);
    else
        http_response_set_from_file (context->response, filename);

    icl_mem_free (etag);
}

//  Formats a directory listing into the bucket; sorts the directory
//  according to filename.

static void
s_format_directory_listing (
    $(selftype) *self,
    http_driver_context_t *context,
    char *disk_path,
    char *dir_index)
{
    ipr_dir_t
        *dir;
    ipr_bucket_t
        *bucket,                        //  Bucket of HTML data
        *style;                         //  http_base.css data
    icl_longstr_t
        *string;
    FILE
        *httpdir;                       //  Handle to .httpdir file
    uint
        finfo_nbr;                      //  Array index for directory

    //  Allocated working buffers, fairly arbitrary sizes
    bucket = ipr_bucket_new (65000);
    string = icl_longstr_new (NULL, 1024);

    //  Format first header block
    icl_longstr_fmt (string, DIR_HEADER_1, context->request->path, context->request->path);
    ipr_bucket_cat (bucket, string->data, string->cur_size);

    //  Add built-in style sheet
    style = http_static_get ("http_base.css");
    assert (bucket);
    ipr_bucket_cat (bucket, style->data, style->cur_size);
    ipr_bucket_destroy (&style);

    icl_longstr_fmt (string, DIR_HEADER_2, context->request->path, context->request->path);
    ipr_bucket_cat (bucket, string->data, string->cur_size);

    //  Load contents of .httpdir file, if possible
    httpdir = fopen (dir_index, "r");
    if (httpdir) {
        ipr_bucket_load (bucket, httpdir);
        fclose (httpdir);
    }
    //  Format last header block
    icl_longstr_fmt (string, DIR_HEADER_3);
    ipr_bucket_cat (bucket, string->data, string->cur_size);

    //  Format directory entries, sorted by name
    dir = ipr_dir_new (disk_path, 'n');

    for (finfo_nbr = 0; finfo_nbr < ipr_dir_index_size (dir); finfo_nbr++) {
        int64_t
            size;
        icl_shortstr_t
            file_size,
            file_time;
        ipr_finfo_t
            *finfo;                     //  File info block

        //  The index returns a sorted list of finfo pointers
        finfo = ipr_dir_index (dir) [finfo_nbr];
        if (finfo->name [0] == '.')
            continue;                   //  Ignore files starting with .

        //  Calculate file size and units
        size = finfo->size;
        if (size < 99999)
            icl_shortstr_fmt (file_size, "%ld", (int) size);
        else {
            size /= 1024;
            if (size < 99999)
                icl_shortstr_fmt (file_size, "%ldKB", (int) size);
            else {
                size /= 1024;
                if (size < 99999)
                    icl_shortstr_fmt (file_size, "%ldMB", (int) size);
                else {
                    size /= 1024;
                    icl_shortstr_fmt (file_size, "%ldGB", (int) size);
                }
            }
        }
        ipr_time_mime (finfo->mtime, file_time);
        icl_longstr_fmt (string, DIR_ENTRY,
            finfo->filetype == APR_DIR? "dir": "file",
            context->request->path, finfo->name, finfo->name, file_size, file_time);
        ipr_bucket_cat (bucket, string->data, string->cur_size);
    }
    ipr_dir_destroy (&dir);
    icl_longstr_fmt (string, DIR_FOOTER);
    ipr_bucket_cat (bucket, string->data, string->cur_size);

    //  Store bucket in bucket list
    bucket->data [bucket->cur_size++] = 0;
    ipr_str_subch ((char *) bucket->data, '[', '<');
    ipr_str_subch ((char *) bucket->data, ']', '>');

    //  Add bucket to list of buckets returned to user
    http_response_set_from_bucket (context->response, bucket, "text/html");
    http_response_set_no_cache (context->response);
    ipr_bucket_unlink (&bucket);
    icl_longstr_destroy (&string);
}

//  Try to find the file using the default types

static void s_try_default_extensions (
    $(selftype) *self,
    http_driver_context_t *context,
    char *filename)
{
    icl_shortstr_t
        fspath_file;
    ipr_token_list_t
        *tokens;
    ipr_token_t
        *token;

    //  The file does not have an extension
    assert (!strchr (filename, '.'));

    tokens = ipr_token_split (http_config_default_types (http_config));
    token = ipr_token_list_first (tokens);
    while (token) {
        icl_shortstr_fmt (fspath_file, "%s%s", filename, token->value);
        if (ipr_file_is_readable (fspath_file)) {
            s_reply_for_file (self, context, fspath_file);
            if (http_config_verbose (http_config))
                smt_log_print (context->debug_log, "I: mapped %s to default file type %s",
                    filename, fspath_file);
            break;
        }
        token = ipr_token_list_next (&token);
    }
    if (token)
        ipr_token_unlink (&token);
    else
        http_response_set_error (context->response, HTTP_REPLY_NOTFOUND,
            "The requested resource does not exist");

    ipr_token_list_destroy (&tokens);
}
</private>

</class>
