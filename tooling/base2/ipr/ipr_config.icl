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
    name      = "ipr_config"
    comment   = "Configuration file management"
    version   = "1.1"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>

<import class = "ipr_xml" />
<import class = "ipr_file" />

<context>
    ipr_xml_t
        *xml_root;                      //  Current configuration tree
    ipr_xml_t
        *xml_item;                      //  Current XML item in tree
    Bool
        duplicated,                     //  Duplicated tree?
        located;                        //  Did last locate work?
    char
        *cur_value;                     //  Last-accessed XML item value
</context>

<private>
#include "ipr_xmll.h"
</private>

<method name = "new">
    <doc>
    If a filename is specified, loads the configuration table from the
    XML file (see $(selfname)_load method).
    </doc>
    <argument name = "filename" type = "char *">Name of configuration file</argument>
    <argument name = "required" type = "Bool"  >Complain if file not found</argument>
    //
    //  Empty config tree has just a root item
    self->xml_root = ipr_xml_new (NULL, "root", "");
    if (filename)
        self_load (self, filename, required);
</method>

<method name = "destroy">
    if (self->duplicated)
        ipr_xml_unlink (&self->xml_root);
    else
        ipr_xml_destroy (&self->xml_root);

    ipr_xml_unlink (&self->xml_item);
    icl_mem_free (self->cur_value);
</method>

<method name = "dup" return = "copy">
    <doc>
    Duplicates a configuration tree, used to create a local copy
    for navigation.  Copies the exact tree state.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "copy" type = "$(selftype) *">New instance</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    copy = self_new (NULL, FALSE);
    ipr_xml_destroy (&copy->xml_root);
    copy->xml_root   = ipr_xml_link (self->xml_root);
    copy->xml_item   = ipr_xml_link (self->xml_item);
    copy->located    = self->located;
    copy->duplicated = TRUE;
    if (copy->xml_item)
        copy->cur_value = ipr_xml_text (copy->xml_item);
</method>

<method name = "load" template = "function">
    <doc>
    Loads an XML file into the current configuration tree.  The load method
    merges data in the loaded file with any data already in the tree.  This
    allows configuration file inheritence.  The merge process works by
    matching and merging items at each level in the tree.  Two items with the
    same item name are matched if they both have the same 'name' attribute or
    both have no 'name' attribute, and no other attributes.  The default action
    is to merge items.  Merging means that all attributes of the new item are
    applied to the existing one, and all children of the two items are in turn
    matched and merged.  If the item has the property 'config_meta' set to
    'discrete', matching children are not merged but replaced.  By definition
    a configuration tree cannot have two items with the same name at any given
    level.  Apart from this, configuration trees can have any XML syntax.
    Returns 0 if the configuration file was loaded, else -1.  Searches for the
    XML file on the PATH and reports the full path of the file loaded.
    </doc>
    <argument name = "filename" type = "char *">Name of configuration file</argument>
    <argument name = "required" type = "Bool"  >Complain if file not found</argument>
    <local>
    ipr_xml_t
        *xml_root = NULL;                //  Tree of file we've loaded
    ipr_xml_t
        *child;
    </local>
    //
    rc = ipr_xml_load_file (&xml_root, "PATH", filename, FALSE);
    switch (rc) {
        case IPR_XML_NOERROR:            //  Just a long name for zero :-/
            child = ipr_xml_first_child (self->xml_root);
            icl_console_print ("I: loaded configuration from %s",
                ipr_xml_attr_get (xml_root, "fullname", "missing?"));
            ipr_xml_unlink (&child);

            s_merge_trees (self->xml_root, xml_root);
            ipr_xml_unlink (&self->xml_item);
            self->xml_item = ipr_xml_first_child (self->xml_root);
            break;
        case IPR_XML_FILEERROR:
            if (required) {
                icl_console_print ("$(selfname) E: '%s' not found", filename);
                icl_console_print ("$(selfname) W: %s", ipr_xml_error ());
                rc = -1;
            }
            break;
        case IPR_XML_LOADERROR:
            icl_console_print ("$(selfname) W: syntax error in config file '%s'", filename);
            icl_console_print ("$(selfname) W: %s", ipr_xml_error ());
            rc = -1;
            break;
    }
    ipr_xml_destroy (&xml_root);
</method>

<method name = "load bucket" template = "function">
    <doc>
    Loads a bucket of XML data into the current configuration tree.  Merges
    data in the loaded data with any data already in the tree, as for the
    load method.  Returns 0 if the data was loaded, else -1.
    </doc>
    <argument name = "bucket" type = "ipr_bucket_t *">Bucket of XML data</argument>
    <local>
    ipr_xml_t
        *xml_root = NULL;                //  Tree of file we've loaded
    </local>
    //
    rc = ipr_xml_load_bucket (&xml_root, bucket, FALSE);
    switch (rc) {
        case IPR_XML_NOERROR:
            s_merge_trees (self->xml_root, xml_root);
            ipr_xml_unlink (&self->xml_item);
            self->xml_item = ipr_xml_first_child (self->xml_root);
            break;
        case IPR_XML_LOADERROR:
            rc = -1;
            break;
    }
    ipr_xml_destroy (&xml_root);
</method>

<method name = "save" template = "function">
    <doc>
    Saves the configuration tree to the specified file.
    </doc>
    <argument name = "filename" type = "char *">Name of configuration file</argument>
    <local>
    ipr_xml_t
        *xml_item;                      //  Current item in our new tree
    </local>
    //
    xml_item = ipr_xml_first_child (self->xml_root);
    ipr_xml_save_file (xml_item, filename);
    ipr_xml_unlink (&xml_item);
</method>

<method name = "locate" template = "function">
    <doc>
    Locates an item in the configuration tree.  We use a simple path
    notation. The path to an item is specified as an absolute path:
    "/item/item..." or a relative path (with no leading slash). The
    last item in the path may be selected by its 'name' attribute.
    You can use ".." in the path to move to the parent item, or "+"
    to move to the first child item.  The name may be null. Having
    located an item you can locate further items relative to it, or
    access the item's value and attributes.  Returns 0 if the item
    was located successfully, -1 if not.  After a locate the located
    property also indicates whether the item was found or not.
    </doc>
    <argument name = "path" type = "char *">Path to item</argument>
    <argument name = "name" type = "char *">Name selector</argument>
    <local>
    char
        *s_path,
        *token_start,
        *token_end;
    int
        token_nbr;
    ipr_xml_t
        *xml_item,
        *xml_child;
    Bool
        have_match;
    </local>
    //
    assert (path);
    if (*path == '/') {
        path++;
        if (self->xml_item)                 //  Remove ref to old item
            ipr_xml_unlink (&self->xml_item);
        self->xml_item = self->xml_root;
        ipr_xml_link (self->xml_item);      //  Add ref to item
    }
    //  Now process path unless XML tree is empty
    if (self->xml_item) {
        xml_item  = self->xml_item;
        ipr_xml_link (xml_item);
        token_nbr = 0;
        s_path = icl_mem_strdup (path);
        token_end = s_path;
        do {
            token_start = token_end;
            token_end   = strchr (token_end, '/');
            if (token_end)
                *token_end = 0;

            if (streq (token_start, "..")) {
                if (xml_item != self->xml_root)
                    xml_item = ipr_xml_parent (&xml_item);
            }
            else
            if (streq (token_start, "+")) {
                xml_child = ipr_xml_first_child (xml_item);
                if (xml_child) {
                    ipr_xml_unlink (&xml_item);
                    xml_item = xml_child;
                }
                else
                    rc = -1;                //  No child items
            }
            else {
                //  Find first child item with name of this token
                have_match = FALSE;
                IPR_FOR_XML_CHILDREN (xml_child, xml_item) {
                    if (streq (ipr_xml_name (xml_child), token_start)) {
                        //  If at end of path, use name selector
                        if ((!token_end) && name) {
                            if (streq (ipr_xml_attr_get (xml_child, "name", ""), name))
                                have_match = TRUE;
                        }
                        else
                            have_match = TRUE;
                    }
                    if (have_match) {
                        ipr_xml_unlink (&xml_item);
                        xml_item = xml_child;
                        break;
                    }
                }
                if (!have_match) {
                    rc = -1;
                    break;              //  No match, quit searching
                }
            }
            if (token_end)
                token_end++;            //  Bump to after slash

        } while (token_end);            //  Until we end the path

        if (rc == 0) {
            ipr_xml_unlink (&self->xml_item);   //  Remove ref to old item
            self->xml_item = xml_item;
        }
        else
            ipr_xml_unlink (&xml_item);

        icl_mem_free (s_path);
    }
    else
        rc = -1;

    self->located = (rc == 0);
</method>

<method name = "next" template = "function">
    <doc>
    Locates the next sibling item in the configuration tree.  This
    will be the next item at the same level as the current item, if
    any.  Returns 0 if there was a next item, -1 if not.
    </doc>
    <local>
    ipr_xml_t
        *xml_item;
    </local>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    xml_item = ipr_xml_link (self->xml_item);
    self->xml_item = ipr_xml_next_sibling (&self->xml_item);
    if (self->xml_item)
        ipr_xml_unlink (&xml_item);
    else {
        self->xml_item = xml_item;
        rc = -1;
    }
    self->located = (rc == 0);
</method>

<method name = "item" return = "item">
    <doc>
    Returns the item name of the current item. You should not modify
    the returned string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "item" type = "char *">Returned name</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    assert (self->xml_item);
    item = ipr_xml_name (self->xml_item);
</method>

<method name = "value" return = "value">
    <doc>
    Returns the value of the current item.  This can include newlines
    and leading and trailing spaces.  You should not modify the returned
    string.  The returned string is valid until you request another item
    value.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "value" type = "char *">Returned value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    assert (self->xml_item);
    if (self->cur_value)
        icl_mem_free (self->cur_value);
    self->cur_value = ipr_xml_text (self->xml_item);
    value = self->cur_value;
</method>

<method name = "get" return = "value">
    <doc>
    Returns the specified attribute, if it exists. The attribute value
    can (but does usually not) include newlines. You should not modify
    the returned string. Returns the default value if the attribute is
    not defined.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *">Attribute name</argument>
    <argument name = "dflt" type = "char *">Default value</argument>
    <declare name = "value" type = "char *">Returned value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    value = ipr_xml_attr_get (self->xml_item, name, dflt);
</method>

<method name = "getn" return = "value">
    <doc>
    Returns the numeric value for the specified attribute, or zero if
    not defined.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *">Attribute name</argument>
    <declare name = "value" type = "long"  >Returned value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    value = atol (ipr_xml_attr_get (self->xml_item, name, "0"));
</method>

<method name = "getp" return = "value">
    <doc>
    Locates a path in the configuration tree and returns an attribute
    of that item.  Combines the $(selfname)_locate and $(selfname)_get
    methods and leaves the current object in the same state as the locate
    method would.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "path" type = "char *">Path to item</argument>
    <argument name = "name" type = "char *">Attribute name</argument>
    <argument name = "dflt" type = "char *">Default value</argument>
    <declare name = "value" type = "char *">Returned value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    self_locate (self, path, NULL);
    value = self_get (self, name, dflt);
</method>

<method name = "put" return = "value">
    <doc>
    Updates or inserts an attribute value at the current position in
    the configuration file.
    </doc>
    <argument name = "self"  type = "$(selftype) *">Reference to object</argument>
    <argument name = "name"  type = "char *">Attribute name</argument>
    <argument name = "value" type = "char *">Attribute value</argument>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    ipr_xml_attr_set (self->xml_item, name, value);
</method>

<method name = "putp" return = "value">
    <doc>
    Locates a path in the configuration tree and updates an attribute
    of that item.  Combines the $(selfname)_locate and $(selfname)_put
    methods and leaves the current object in the same state as the locate
    method would.
    </doc>
    <argument name = "self"  type = "$(selftype) *">Reference to object</argument>
    <argument name = "path"  type = "char *">Path to item</argument>
    <argument name = "name"  type = "char *">Attribute name</argument>
    <argument name = "value" type = "char *">Attribute value</argument>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    //TODO: should create path if it's not already defined
    self_locate (self, path, NULL);
    self_put    (self, name, value);
</method>

<private name = "header">
static void
s_merge_trees (ipr_xml_t *xml_dest, ipr_xml_t *xml_source);
</private>

<private name = "footer">
static void
s_merge_trees (ipr_xml_t *xml_dest, ipr_xml_t *xml_source)
{
    ipr_xml_t
        *xml_item,                      //  Current item in our new tree
        *xml_item_link,                 //  Extra link to xml_item
        *xml_into,                      //  Destination item in config
        *xml_next;                      //  Next item on list
    ipr_xml_attr_t
        *xml_attr,                      //  Attribute of our current item
        *item_attr,                     //  First attribute of target item
        *into_attr;                     //  First attribute of destination item
    char
        *item_name,
        *into_name;
    Bool
        discrete,                       //  Replace children, don't merge
        processed;

    //  Check config_meta options
    discrete = streq (ipr_xml_attr_get (xml_dest, "config_meta", ""), "discrete");

    //  Merge attributes of source into dest
    IPR_FOR_XML_ATTRIBUTES (xml_attr, xml_source)
        ipr_xml_attr_set (xml_dest, ipr_xml_attr_name (xml_attr), ipr_xml_attr_value (xml_attr));

    //  Process each child item of xml_source
    xml_item = ipr_xml_first_child (xml_source);
    while (xml_item) {
        if (ipr_xml_name (xml_item)) {
            processed = FALSE;
            //  a) item is named, look for matching named item
            //  b) item is unnamed, merge only if both have no attributes
            item_name = ipr_xml_attr_get (xml_item, "name", NULL);
            item_attr = ipr_xml_attr_first (xml_item);
            IPR_FOR_XML_CHILDREN (xml_into, xml_dest) {
                if (streq (ipr_xml_name (xml_into), ipr_xml_name (xml_item))) {
                    into_name = ipr_xml_attr_get (xml_into, "name", NULL);
                    into_attr = ipr_xml_attr_first (xml_into);
                    //  Skip config_meta tag, on destination only
                    if (into_attr && streq (ipr_xml_attr_name (into_attr), "config_meta"))
                        into_attr = ipr_xml_attr_next (&into_attr);

                    if ((item_name && into_name && streq (into_name, item_name))
                    || (!item_name && !item_attr && !into_name && !into_attr)) {
                        //  We have a match, so now replace or merge item
                        if (discrete) {
                            xml_item_link = ipr_xml_link (xml_item);
                            ipr_xml_detach (xml_item);
                            ipr_xml_attach_sibling (xml_into, xml_item);
                            ipr_xml_unlink (&xml_item_link); //  Remove the extra link
                            xml_item = ipr_xml_next_sibling (&xml_item);
                            ipr_xml_destroy (&xml_into);
                        }
                        else
                            s_merge_trees (xml_into, xml_item);
                        processed = TRUE;
                    }
                    ipr_xml_attr_unlink (&into_attr);
                    if (processed) {
                        ipr_xml_unlink (&xml_into);
                        break;
                    }
                }
            }
            //  Move current item to destination if needed, and do next
            xml_item_link = ipr_xml_link (xml_item);
            xml_next = ipr_xml_next_sibling (&xml_item_link);
            if (!processed) {
                ipr_xml_detach (xml_item);
                ipr_xml_attach_child (xml_dest, xml_item);
            }
            ipr_xml_attr_unlink (&item_attr);
            ipr_xml_unlink (&xml_item);    //  Remove the extra link
            xml_item = xml_next;
        }
        else
            xml_item = ipr_xml_next_sibling (&xml_item);
    }
}
</private>

<method name = "selftest" export = "none">
    <local>
    ipr_config_t
        *config;
    int
        items;
    </local>
    //
    config = ipr_config_new ("ipr_config_test1.xml", TRUE);
    ipr_config_load (config, "ipr_config_test2.xml", TRUE);
    ipr_config_load (config, "ipr_config_test3.xml", TRUE);
    ipr_config_load (config, "no-such-file.xml",    FALSE);
    ipr_config_save (config, "ipr_config_save.xml");

    //  Test data has four queues, iterate through them
    ipr_config_locate (config, "/config/queues/queue", "");
    for (items = 0; items < 4; items++) {
        assert (config->located);
        ipr_config_next (config);
    }
    assert (config->located == FALSE);

    //  Try some navigation
    ipr_config_locate (config, "/config/general/security/profile", "custom");
    assert (config->located);
    ipr_config_locate (config, "..", "default");
    assert (config->located);

    //  Test config_meta = "discrete", we should see example profile from test3
    ipr_config_locate (config, "/config/profiles/profile", "example");
    assert (config->located);
    ipr_config_locate (config, "limit", NULL);
    for (items = 0; items < 3; items++) {
        assert (config->located);
        ipr_config_next (config);
    }
    assert (config->located == FALSE);

    ipr_config_locate (config, "/config/queues/queue", "");
    ipr_config_locate (config, "/config/queues/queue", "test");
    assert (streq (ipr_config_value (config), "This is a test queue"));
    assert (streq (ipr_config_get (config, "overflow", "?"), "2000"));

    ipr_config_destroy (&config);
</method>

</class>
