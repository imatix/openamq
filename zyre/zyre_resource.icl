<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This file is licensed under the GPL as follows:

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
    name      = "zyre_resource"
    comment   = "RestMS resource wrapper"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class holds all RestMS resources in a generic form, with a pointer to
the real resource object.  Resources are stored in a hash table where the
key is the resource path.  Resources have a looseref list of all children.
This class is not threadsafe and may be used from one threadlet only.
</doc>

<inherit class = "ipr_portal">
    <option name = "front_end" value = "sync" />
    <option name = "back_end" value = "sync" />
</inherit>

<import class = "zyre_classes" />

<!-- Resource portal methods -->
<data>
  <!-- Requests from restms to resource object -->
  <request name = "configure">
    <doc>
    Configure a new resource, the context contains a parsed document.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </request>

  <request name = "get">
    <doc>
    Retrieve resource.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </request>

  <request name = "put">
    <doc>
    Modify resource as specified.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </request>

  <request name = "delete">
    <doc>
    Delete resource.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </request>

  <request name = "post">
    <doc>
    Create child resource as specified.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </request>

  <request name = "report">
    <doc>
    Report properties to parent resource.  This method asks a child to
    report its summary properties to the ipr_tree provided by the parent.
    </doc>
    <field name = "tree" type = "ipr_tree_t *" />
  </request>

  <!-- Responses back from resource object to restms -->
  <response name = "child add">
    <doc>
    Ask client (restms) to do the child resource creation.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
  </response>
</data>

<public name = "header">
#define RESTMS_RESOURCE_INVALID     0
#define RESTMS_RESOURCE_DOMAIN      1
#define RESTMS_RESOURCE_FEED        2
#define RESTMS_RESOURCE_PIPE        3
#define RESTMS_RESOURCE_JOIN        4
#define RESTMS_RESOURCE_MESSAGE     5
#define RESTMS_RESOURCE_CONTENT     6
#define RESTMS_RESOURCE_VALID(t)    ((t) > 0 && (t) < 6)
</public>

<context>
    $(selftype)
        *parent;                        //  Parent resource
    uint
        type;                           //  Resource type
    ipr_looseref_list_t
        *children;                      //  Looseref list of children
    ipr_hash_t
        *hash;                          //  Backlink to hash item
    ipr_looseref_t
        *in_parent;                     //  Backlink to parent's list
    apr_time_t
        modified;                       //  Date-Modified value
</context>

<method name = "new">
    <argument name = "parent" type = "zyre_resource_t *">Parent resource, or NULL</argument>
    <argument name = "table" type = "ipr_hash_table_t *">Hash by path</argument>
    <argument name = "type" type = "char *">Resource type name</argument>
    <argument name = "path" type = "char *">Resource path</argument>
    //
    self->modified  = apr_time_now ();
    self->type      = zyre_resource_type_value (type);
    self->children  = ipr_looseref_list_new ();
    self->hash      = ipr_hash_new (table, path, self);
    self->in_parent = self_attach_to_parent (self, parent);
</method>

<method name = "destroy">
    ipr_hash_destroy (&self->hash);
    ipr_looseref_list_destroy (&self->children);
</method>

<method name = "attach to parent" return = "looseref">
    <doc>
    Attaches the resource to its parent, if any, and returns the resulting
    looseref object.
    </doc>
    <argument name = "self" type = "zyre_resource_t *" />
    <argument name = "parent" type = "zyre_resource_t *" />
    <declare name = "looseref" type = "ipr_looseref_t *" default = "NULL" />
    //
    if (parent)
        looseref = ipr_looseref_queue (parent->children, self);
</method>

<method name = "detach from parent" template = "function">
    <doc>
    Removes the resource from the parent list if any.  We can't do this in
    the destroy method because that is also called at shutdown, and the
    parent list can already be destroyed then.
    </doc>
    //
    ipr_looseref_destroy (&self->in_parent);
</method>

<method name = "etag" return = "etag">
    <doc>
    Calculates an ETag for the resource and returns this as a fresh
    string, that the caller should free when finished.  The ETag will be
    different for each version of the resource, i.e. it includes the
    resource's modified value.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <declare name = "etag" type = "char *" default = "NULL">ETag to generate</declare>
    //
    etag = ipr_str_format ("%llx-%llx",
        (long long unsigned int) self->modified, self->hash->table_index);
</method>

<method name = "modified" template = "function">
    <doc>
    Returns TRUE if the resource has been modified, based on the If-None-Match
    and If-Modified-Since headers specified in the supplied HTTP request.  If
    the request does not provide either of these headers, returns TRUE.  This
    method is for use in GET methods: 'if modified, 200, else 304'.
    </doc>
    <argument name = "request" type = "http_request_t *">Incoming HTTP request</argument>
    <local>
    char
        *etag = NULL;
    //  ISO times have a resolution of 1 second
    time_t
        if_modified;
    </local>
    //
    assert (request);
    if_modified = ipr_time_mime_decode (
        http_request_get_header (request, "if-modified-since")) / 1000000;

    etag = self_etag (self);
    if (streq (etag, http_request_get_header (request, "if-none-match"))
    || (if_modified && (self->modified / 1000000) == if_modified))
        rc = FALSE;                     //  Not modified
    else
        rc = TRUE;
    icl_mem_free (etag);
</method>

<method name = "unmodified" template = "function">
    <doc>
    Returns TRUE if the resource has not been modified, based on the If-Match
    and If-Unmodified-Since headers specified in the supplied HTTP request.
    If the request does not provide either of these headers, returns TRUE.
    This method is for use in PUT methods: 'if unmodified, 200, else 412'.
    </doc>
    <argument name = "request" type = "http_request_t *">Incoming HTTP request</argument>
    <local>
    char
        *etag = NULL;
    time_t
        if_unmodified;
    </local>
    //
    assert (request);
    if_unmodified = ipr_time_mime_decode (
        http_request_get_header (request, "if-unmodified-since")) / 1000000;

    etag = self_etag (self);
    if (streq (etag, http_request_get_header (request, "if-match"))
    || (if_unmodified && (self->modified / 1000000) == if_unmodified))
        rc = TRUE;                      //  Not modified
    else
        rc = FALSE;
    icl_mem_free (etag);
</method>

<method name = "path" return = "path">
    <doc>
    Build URI path for specified resource, using resource type and Slug header
    value, if any.  Caller frees path after use.
    </doc>
    <argument name = "type" type = "char *">Resource type</argument>
    <argument name = "slug" type = "char *">Slug, or ""</argument>
    <declare name = "path" type = "char *" />
    <local>
    char
        *name_ptr;
    </local>
    //
    path = icl_mem_alloc (ICL_SHORTSTR_MAX + 1);
    if (slug && *slug) {
        for (name_ptr = slug; *name_ptr; name_ptr++)
            if (!isalnum (*name_ptr) && *name_ptr != '.')
                *name_ptr = '-';
        icl_shortstr_fmt (path, "/%s/%s", type, slug);
    }
    else
        //  Grab random key
        //  In fact we should never re-use old hashes
        ipr_str_random (path, "/resource/AAAAAAAA");
</method>

<method name = "type name" return = "name">
    <doc>
    Accepts a resource type and returns the external resource name.
    </doc>
    <argument name = "type" type = "int" />
    <declare name = "name" type = "char *" />
    assert (RESTMS_RESOURCE_VALID (type));
    if (type == RESTMS_RESOURCE_DOMAIN)
        name = "domain";
    else
    if (type == RESTMS_RESOURCE_FEED)
        name = "feed";
    else
    if (type == RESTMS_RESOURCE_PIPE)
        name = "pipe";
    else
    if (type == RESTMS_RESOURCE_JOIN)
        name = "join";
    else
    if (type == RESTMS_RESOURCE_MESSAGE)
        name = "message";
    else
    if (type == RESTMS_RESOURCE_CONTENT)
        name = "content";
    else
        name = "(?)";
</method>

<method name = "type value" return = "type">
    <doc>
    Accepts a resource name and returns the integer resource type.  If the
    name was not valid, returns RESTMS_RESOURCE_INVALID.
    </doc>
    <argument name = "name" type = "char *" />
    <declare name = "type" type = "int" />
    //
    if (streq (name, "domain"))
        type = RESTMS_RESOURCE_DOMAIN;
    else
    if (streq (name, "feed"))
        type = RESTMS_RESOURCE_FEED;
    else
    if (streq (name, "pipe"))
        type = RESTMS_RESOURCE_PIPE;
    else
    if (streq (name, "join"))
        type = RESTMS_RESOURCE_JOIN;
    else
    if (streq (name, "message"))
        type = RESTMS_RESOURCE_MESSAGE;
    else
    if (streq (name, "content"))
        type = RESTMS_RESOURCE_CONTENT;
    else
        type = RESTMS_RESOURCE_INVALID;
</method>

<method name = "report" template = "function">
    <doc>
    Accepts a resource object tree, formats that as a structured document and
    returns it to the browser client.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <argument name = "tree" type = "ipr_tree_t *" />
    <local>
    icl_longstr_t
        *longstr;                       //  Serialized data as string
    ipr_bucket_t
        *bucket;                        //  Serialized data as bucket
    icl_shortstr_t
        content_type;
    char
        *accept;
    icl_shortstr_t
        mime_date;
    char
        *etag;
    </local>
    //
    //  Format content and set Content-Type
    accept = http_request_get_header (context->request, "accept");
    if (ipr_str_prefixed (accept, "application/restms+json")) {
        longstr = ipr_tree_save_json (tree);
        icl_shortstr_fmt (content_type, "application/restms+json;type=");
    }
    else {
        longstr = ipr_tree_save_xml (tree);
        icl_shortstr_fmt (content_type, "application/restms+xml;type=");
    }
    icl_shortstr_cat (content_type, self_type_name (self->type));

    //  Save string in bucket and pass as response bucket
    bucket = ipr_bucket_new (longstr->cur_size);
    ipr_bucket_fill (bucket, longstr->data, longstr->cur_size);
    http_response_set_from_bucket (context->response, bucket, content_type);
    ipr_bucket_unlink (&bucket);
    icl_longstr_destroy (&longstr);

    //  Set Last-Modified and ETag
    ipr_time_mime (self->modified, mime_date);
    etag = zyre_resource_etag (self);
    http_response_set_header (context->response, "last-modified", mime_date);
    http_response_set_header (context->response, "etag", etag);
    icl_mem_free (etag);

    http_driver_context_reply (context);
</method>

<method name = "selftest" />

</class>
