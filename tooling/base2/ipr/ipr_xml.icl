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
    name      = "ipr_xml"
    comment   = "iPR XML class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    >

<doc>
    Implements XML objects.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "rwlock" value = "1" />
</inherit>

<import class = "icl_mem" />
<import class = "ipr_bucket" />
<import class = "ipr_xml_attr"/>

<public>
#define IPR_FOR_XML_CHILDREN(child,item)           \
    for (child  = ipr_xml_first_child (item);      \
         child != NULL;                            \
         child  = ipr_xml_next_sibling (&child))   \
        if (ipr_xml_name (child))

#define IPR_FOR_XML_VALUES(child,item)             \
    for (child  = ipr_xml_first_child (item);      \
         child != NULL;                            \
         child  = ipr_xml_next_sibling (&child))   \
        if (!ipr_xml_name (child))

#define IPR_FOR_XML_ATTRIBUTES(attr,item)          \
    for (attr  = ipr_xml_attr_first (item);        \
         attr != NULL;                             \
         attr  = ipr_xml_attr_next (&attr))
</public>

<private>
#include "ipr_bucket.h"
#include "ipr_xmll.h"
</private>

<context>
    ipr_xml_t
        *parent;
    char
        *name,                          //  Item name, allocated string
        *value;                         //  Value node, allocated string
    ipr_xml_list_t
        *children;
    ipr_xml_attr_list_t
        *attrs;
</context>

<method name = "new">
    <argument name = "parent" type = "$(selftype) *">The parent of the new item</argument>
    <argument name = "name" type = "char *">The name of the new item</argument>
    <argument name = "value" type = "char *">The value of the new item</argument>
    if (name)
        self->name  = icl_mem_strdup (name);
    if (value)
        self->value = icl_mem_strdup (value);
    self->children  = ipr_xml_list_new ();
    self->attrs     = ipr_xml_attr_list_new ();
    if (parent)
        ipr_xml_attach_child (parent, self);
</method>

<method name = "destroy">
    icl_mem_free (self->name);
    icl_mem_free (self->value);
    ipr_xml_list_destroy      (&self->children);
    ipr_xml_attr_list_destroy (&self->attrs);
</method>

<method name = "parse file" return = "self">
    <argument name = "path" type = "char *" />
    <argument name = "filename" type = "char *" />
    <declare name = "self" type = "$(selftype) *"/>
    //
    self = NULL;
    if (ipr_xml_load_file (&self, path, filename, FALSE))
        self = self_new (NULL, "", NULL);
</method>

<method name = "parse string" return = "self">
    <argument name = "string" type = "char *" />
    <declare name = "self" type = "$(selftype) *"/>
    //
    self = NULL;
    if (ipr_xml_load_string (&self, string, FALSE))
        self = self_new (NULL, "", NULL);
</method>

<method name = "parse bucket" return = "self">
    <argument name = "bucket" type = "ipr_bucket_t *" />
    <declare name = "self" type = "$(selftype) *"/>
    //
    self = NULL;
    if (bucket && ipr_xml_load_bucket (&self, bucket, FALSE))
        self = self_new (NULL, "", NULL);
</method>

<method name = "attach child">
    <argument name = "parent" type = "$(selftype) *">The new parent of the item</argument>
    <argument name = "self" type = "$(selftype) *"/>
    if (self->parent)
        ipr_xml_list_remove (self);
    self->parent = parent;
    ipr_xml_list_queue (parent->children, self);
</method>

<method name = "attach sibling">
    <argument name = "sibling" type = "$(selftype) *">Item inserted before the sibling</argument>
    <argument name = "self" type = "$(selftype) *">The item</argument>
    <local>
    $(selftype)
        *sibling_link,
        *parent_link;
    </local>
    if (self->parent)
        ipr_xml_list_remove (self);

    sibling_link = ipr_xml_link (sibling);
    self->parent = ipr_xml_parent (&sibling_link);
    ipr_xml_list_relink_before (self, sibling);

    //  A bit messy.  We don't keep a link for our parent.
    parent_link = self->parent;
    ipr_xml_unlink (&parent_link);
</method>

<method name = "detach">
    <argument name = "self" type = "$(selftype) *">The item</argument>
    if (self->parent)
        ipr_xml_list_remove (self);
    self->parent = NULL;
</method>

<method name = "first child" return = "child">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "child" type = "$(selftype) *"/>
    child = ipr_xml_list_first (self->children);
</method>

<method name = "next sibling" return = "sibling">
    <argument name = "pself" type = "$(selftype) **"/>
    <declare name = "sibling" type = "$(selftype) *"/>
    sibling = ipr_xml_list_next (pself);
</method>

<method name = "parent" return = "parent">
    <argument name = "pself" type = "$(selftype) **"/>
    <declare name = "parent" type = "$(selftype) *"/>
    parent = (*pself)->parent;
    ipr_xml_link (parent);
    ipr_xml_unlink (pself);
</method>

<method name = "name" return = "name">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "name" type = "char *"/>
    name = self->name;
</method>

<method name = "value" return = "value">
    <doc>
    Returns the value for a node.  Do not use to get the value for an
    element, use text() for that.
    </doc>
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "value" type = "char *"/>
    value = self->value;
</method>

<method name = "rename">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "name" type = "char *"/>
    icl_mem_strfree (&self->name);
    if (name)
        self->name=icl_mem_strdup (name);
</method>

<method name = "value set">
    <doc>
    Sets the value for a node.  Do not use to set the value for an element,
    but create a child node with no name, and set the value on the child
    node.
    </doc>
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "value" type = "char *"/>
    if (!self->value) {
        if (value)
            self->value = icl_mem_strdup (value);
    }
    else
    if (value) {
        if (strneq (value, self->value)) {
            icl_mem_strfree (&self->value);
            self->value = icl_mem_strdup (value);
        }
    }
    else
        icl_mem_strfree (&self->value);
</method>

<method name = "text" return = "value">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "value" type = "char *" default = "NULL"/>
    <local>
    ipr_xml_t
        *child;
    long
        length;
    char
        *childvalue,
        *target,
        *source;
    </local>
    //
    length = 0;
    IPR_FOR_XML_VALUES (child, self) {
        childvalue = ipr_xml_value (child);
        if (childvalue)
            length += strlen (childvalue);
    }
    if (length > 0) {
        value = icl_mem_alloc (length + 1);
        if (value != NULL) {
            target = value;
            IPR_FOR_XML_VALUES (child, self) {
                source = ipr_xml_value (child);
                if (source) {
                    while (*source)
                       *target++ = *source++;
                }
            }
            *target  = 0;
        }
    }
</method>

<method name = "find item" return = "xml_found">
    <doc>
    Locates a child item according to a path, which is name/name/name.
    Returns the item if found, else null.
    </doc>
    <argument name = "xml_root" type = "$(selftype) *"/>
    <argument name = "p_path" type = "char *"/>
    <declare name = "xml_found" type = "ipr_xml_t *"/>
    <local>
    ipr_xml_t
        *xml_item,
        *xml_child;
    char
        *path,
        *cur_path,
        *next_path;
    </local>
    //
    path = icl_mem_strdup ((byte *) p_path);
    cur_path = path;
    xml_item = xml_root;
    xml_child = NULL;
    xml_found = NULL;

    /*  Traverse the XML Tree, starting at xml_root passed to us, keeping
     *  track of our current position in the path passed to us and in the
     *  XML Tree.  Stop when we have run out of either.
     */
    while (cur_path) {
        next_path = strchr (cur_path, '/');
        if (next_path)
            *next_path++ = 0;

        IPR_FOR_XML_CHILDREN (xml_child, xml_item) {
            if (strcmp (ipr_xml_name (xml_child), cur_path) == 0) {
                if (next_path == NULL) {
                    xml_found = xml_child;
                    break;
                }
                else
                    xml_item = xml_child;
            }
        }
        cur_path = next_path;
    }
    icl_mem_strfree (&path);
</method>

<method name = "next item" return = "sibling">
    <doc>
    Returns the next sibling with the same item name as the provided
    item.  Use to walk through a collection of elements and process all
    those of a specific type.
    </doc>
    <argument name = "pself" type = "$(selftype) **"/>
    <declare name = "sibling" type = "$(selftype) *"/>
    <local>
    char
        *item_name;
    </local>
    //
    assert (pself && *pself);
    item_name = ipr_xml_name (*pself);
    sibling = ipr_xml_list_next (pself);
    while (sibling && strneq (ipr_xml_name (sibling), item_name))
        sibling = ipr_xml_list_next (&sibling);
</method>

<method name = "attr" return = "attr">
    <argument name = "item" type = "ipr_xml_t*"/>
    <argument name = "name" type = "const char*"/>
    <declare name = "attr" type = "ipr_xml_attr_t*" default = "NULL" />
    //
    assert (item);
    assert (name);
    IPR_FOR_XML_ATTRIBUTES (attr, item)
        if (attr-> name? strcmp (attr-> name, name) == 0: FALSE)
            break;
</method>

<method name = "selftest" inherit = "none">
    <local>
    ipr_xml_t
        *item,
        *child;
    char
        *value;
    int
        rc;
    </local>
    //
    item = ipr_xml_new (NULL, "item", NULL);
    value = ipr_xml_attr_get (item, "attr", NULL);
    assert (value == NULL);
    ipr_xml_attr_set (item, "attr", "value");
    value = ipr_xml_attr_get (item, "attr", "");
    assert (streq (value, "value"));

    child = ipr_xml_new (item, "child", NULL);
    ipr_xml_value_set (child, "New value for child");
    child = ipr_xml_parent (&child);
    assert (child == item);
    ipr_xml_unlink (&child);

    child = ipr_xml_first_child (item);
    assert (streq (ipr_xml_value (child), "New value for child"));
    ipr_xml_unlink (&child);
    ipr_xml_destroy (&item);

    ipr_xml_load_string (&item, "&lt;restms&gt;&lt;feed name=\\"test.fanout\\"/&gt;", FALSE);
    ipr_xml_destroy (&item);

    value =
"&lt;message address=\\"address.multi.1\\" message_id=\\"ID1\\"&gt;" //
"    &lt;header name=\\"serial-number\\" value=\\"0001\\" /&gt;" //
"    &lt;content&gt;It's too cold&lt;/content&gt;" //
"&lt;/message&gt;" //
"&lt;message address=\\"address.multi.2\\" message_id=\\"ID2\\"&gt;" //
"    &lt;header name=\\"serial-number\\" value=\\"0002\\" /&gt;" //
"    &lt;content&gt;It's too hot&lt;/content&gt;" //
"&lt;/message&gt;" //
"";
    rc = ipr_xml_load_string (&item, value, FALSE);
    icl_console_print ("RC=%d %s", rc, ipr_xml_error ());
    ipr_xml_destroy (&item);
</method>

</class>
