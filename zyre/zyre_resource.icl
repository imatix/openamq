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
<inherit class = "icl_init" />

<import class = "zyre_classes" />

<!-- Resource portal methods -->
<data>
  <!-- Requests from restms to resource object -->
  <request name = "configure">
    <doc>
    Configure a new resource, the context contains a parsed document. If the
    resource is synchronized with a backend, this is the time to do that.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
    <field name = "table"   type = "ipr_hash_table_t *">Resource hash</field>
    <field name = "backend" type = "zyre_backend_t *">Synchronize with</field>
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
    <field name = "table"   type = "ipr_hash_table_t *">Resource hash</field>
    <field name = "backend" type = "zyre_backend_t *">Synchronize with</field>
  </request>

  <request name = "report">
    <doc>
    Report properties to parent resource.  This method asks a child to
    report its summary properties to the ipr_tree provided by the parent.
    </doc>
    <field name = "context" type = "http_driver_context_t *" />
    <field name = "tree" type = "ipr_tree_t *" />
  </request>

  <!-- Requests from one resource to another resource object -->
  <request name = "attach">
    <doc>
    Attach the sender as a child of the target.  Provides a void * argument
    whose semantics depend on the relationship.  To be used when configuring
    the child.
    </doc>
    <field name = "resource" type = "zyre_resource_t *">Child resource</field>
    <field name = "argument" type = "void *">Argument</field>
  </request>

  <request name = "detach">
    <doc>
    Detach the sender as a child of the target.  Provides a void * argument
    whose semantics depend on the relationship.  To be used when deleting
    the child.
    </doc>
    <field name = "resource" type = "zyre_resource_t *">Child resource</field>
    <field name = "argument" type = "void *">Argument</field>
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
#define RESTMS_RESOURCE_TOP         7
#define RESTMS_RESOURCE_VALID(t)    ((t) > 0 && (t) < RESTMS_RESOURCE_TOP)
</public>

<private name = "header">
#define S_CONFIG_FILE       "zyre_restms.cfg"
//  This counter lets us generate unique resource hashes - we bump it each time
//  we initialise the class, and save the new value in S_CONFIG_FILE
static long int
    s_instance,                         //  Execution instance
    s_current;                          //  Resources created so far
</private>

<context>
    $(selftype)
        *parent;                        //  Parent resource
    uint
        type;                           //  Resource type
    icl_shortstr_t
        name,                           //  Resource name or hash
        path;                           //  Resource path
    Bool
        private;                        //  Not discoverable
    ipr_hash_t
        *hash;                          //  Backlink to hash item
    ipr_looseref_list_t
        *children;                      //  Looseref list of children
    ipr_looseref_t
        *in_parent;                     //  Backlink to parent's list
    apr_time_t
        modified;                       //  Date-Modified value
</context>

<method name = "new">
    <doc>
    Create new resource.  If the resource already exists (path already defined)
    the resource->hash will be null, and the resource should be destroyed by
    the caller.
    </doc>
    <argument name = "parent" type = "zyre_resource_t *">Parent resource, or NULL</argument>
    <argument name = "table" type = "ipr_hash_table_t *">Hash by path</argument>
    <argument name = "type" type = "char *">Resource type name</argument>
    <argument name = "name" type = "char *">Resource name, or empty</argument>
    //
    if (*name) {
        //  If we have a name, clean it up
        char
            *name_ptr;
        icl_shortstr_cpy (self->name, name);
        for (name_ptr = self->name; *name_ptr; name_ptr++)
            if (!isalnum (*name_ptr) && *name_ptr != '.')
                *name_ptr = '-';
        icl_shortstr_fmt (self->path, "/%s/%s", type, self->name);
        self->private = FALSE;          //  Discoverable resource
    }
    else {
        //  Else generate a new unique hash as name
        ipr_str_random (self->name, "ZYRE-AAAAAAAA");
        icl_shortstr_fmt (self->name + strlen (self->name), "-%d%d", s_instance, ++s_current);
        icl_shortstr_fmt (self->path, "/resource/%s", self->name);
        self->private = TRUE;           //  Not discoverable resource
    }
    self->hash = ipr_hash_new (table, self->path, self);
    if (self->hash) {
        self->type      = self_type_value (type);
        self->parent    = parent;
        self->children  = ipr_looseref_list_new ();
        self->modified  = apr_time_now ();
        self_attach_to_parent (self, parent);
    }
    if (zyre_config_restms_debug (zyre_config))
        icl_console_print ("R: - create %s called '%s'", type, self->name);
</method>

<method name = "destroy">
    ipr_hash_destroy (&self->hash);
    if (self->children)
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
        self->in_parent = ipr_looseref_queue (parent->children, self);
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
    resource's modified time and MIME type and the content type.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <argument name = "type" type = "char *" />
    <declare name = "etag" type = "char *" default = "NULL">ETag to generate</declare>
    <local>
    int
        mime_type;                      //  Encode content type into ETag
    </local>
    //
    if (ipr_str_prefixed (type, "application/restms+json"))
        mime_type = 1;
    else
    if (ipr_str_prefixed (type, "application/restms+xml"))
        mime_type = 2;
    else
        mime_type = 3;

    etag = ipr_str_format ("%llx-%llx-%d",
        (long long unsigned int) self->modified,
        (long long unsigned int) self->hash->table_index,
        mime_type);
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

    etag = self_etag (self, http_request_get_header (request, "accept"));
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

    etag = self_etag (self, http_request_get_header (request, "accept"));
    if (streq (etag, http_request_get_header (request, "if-match"))
    || (if_unmodified && (self->modified / 1000000) == if_unmodified))
        rc = TRUE;                      //  Not modified
    else
        rc = FALSE;
    icl_mem_free (etag);
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

<method name = "to document" template = "function">
    <doc>
    Accepts a resource object tree, reports all children, formats tree as a
    structured document and returns it to the browser client.  Destroys the
    tree afterwards.
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <argument name = "p_tree" type = "ipr_tree_t **" />
    <local>
    ipr_looseref_t
        *looseref;
    icl_longstr_t
        *longstr;                       //  Serialized data as string
    ipr_bucket_t
        *bucket;                        //  Serialized data as bucket
    icl_shortstr_t
        mime_date;
    char
        *etag;
    </local>
    //
    looseref = ipr_looseref_list_first (self->children);
    while (looseref) {
        zyre_resource_t
            *resource = (zyre_resource_t *) looseref->object;
        if (!resource->private)
            self_request_report (resource, context, *p_tree);
        looseref = ipr_looseref_list_next (&looseref);
    }
    ipr_tree_shut (*p_tree);
    ipr_tree_leaf (*p_tree, "xmlns", "http://www.restms.org/schema/restms");

    if (ipr_str_prefixed (context->response->content_type, "application/restms+json"))
        longstr = ipr_tree_save_json (*p_tree);
    else
    if (ipr_str_prefixed (context->response->content_type, "application/restms+xml"))
        longstr = ipr_tree_save_xml (*p_tree);
    else {
        icl_shortstr_cpy (context->response->content_type, "text/xml");
        longstr = ipr_tree_save_xml (*p_tree);
    }
    ipr_tree_destroy (p_tree);

    //  Save string in bucket and pass as response bucket
    bucket = ipr_bucket_new (longstr->cur_size);
    ipr_bucket_fill (bucket, longstr->data, longstr->cur_size);
    http_response_set_from_bucket (context->response, bucket, NULL);
    ipr_bucket_unlink (&bucket);
    icl_longstr_destroy (&longstr);

    //  Set Last-Modified and ETag
    ipr_time_mime (self->modified, mime_date);
    etag = self_etag (self, context->response->content_type);
    http_response_set_header (context->response, "last-modified", mime_date);
    http_response_set_header (context->response, "etag", etag);
    icl_mem_free (etag);

    http_driver_context_reply (context);
</method>

<method name = "parse uri" return = "resource">
    <doc>
    Given a resource URI, returns the resource if it exists.  If the URI is
    invalid or the resource does not exist, returns NULL. Resource URIs take
    this form: http://hostname[:port]/restms/{path-info}
    </doc>
    <argument name = "context" type = "http_driver_context_t *" />
    <argument name = "table"   type = "ipr_hash_table_t *" />
    <argument name = "uri"     type = "char *" />
    <declare name = "resource" type = "zyre_resource_t *" default = "NULL" />
    <local>
    char
        *path;
    </local>
    //
    path = ipr_str_defix (uri, context->response->root_uri);
    if (path) {
        path = ipr_str_defix (path, RESTMS_ROOT);
        if (path)
            resource = ipr_hash_lookup (table, path);
    }
</method>

<method name = "initialise">
    <local>
    ipr_xml_t
        *xml_root = NULL,
        *xml_item = NULL;
    icl_shortstr_t
        instance_str;
    </local>
    //
    ipr_xml_load_file (&xml_root, ".", S_CONFIG_FILE, FALSE);
    if (xml_root) {
        xml_item = ipr_xml_first_child (xml_root);
        if (!xml_item)
            xml_item = ipr_xml_new (xml_root, "config", NULL);
    }
    else {
        xml_item = ipr_xml_new (xml_root, "config", NULL);
    }
    s_instance = atoi (ipr_xml_attr_get (xml_item, "instance", "0"));
    s_instance++;
    s_current = 0;
    icl_shortstr_fmt (instance_str, "%d", s_instance);
    ipr_xml_attr_set (xml_item, "instance", instance_str);
    ipr_xml_save_file (xml_item, S_CONFIG_FILE);
    ipr_xml_unlink (&xml_item);
    ipr_xml_destroy (&xml_root);
</method>

<method name = "selftest" />

</class>
