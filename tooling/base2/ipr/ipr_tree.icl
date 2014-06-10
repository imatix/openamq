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
    name      = "ipr_tree"
    comment   = "iPR XML builder class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    opaque    = "1"
    >

<doc>
    Implements an XML tree builder - an easy way to construct XML trees
    for delivery to applications.  To load or modify XML trees, use the
    ipr_xml class interface.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "ipr_xml" />
<import class = "ipr_xml_attr" />
<import class = "ipr_bucket" />

<private>
#include "ipr_xmll.h"
</private>

<context>
    ipr_xml_t
        *root_item,                     //  Tree root item
        *cur_branch;                    //  Current branch of tree
</context>

<method name = "new">
    <doc>
    Creates a new tree, which will be empty except for a root item.  To add
    properties to the root item, use "leaf".
    </doc>
    <argument name = "name" type = "char *">The name of the root item</argument>
    //
    self->root_item  = ipr_xml_new (NULL, name, NULL);
    self->cur_branch = ipr_xml_link (self->root_item);
    assert (self->root_item);
</method>

<method name = "destroy">
    ipr_xml_unlink  (&self->cur_branch);
    ipr_xml_destroy (&self->root_item);
</method>

<method name = "open" template = "function">
    <doc>
    Adds a new branch to the current branch, and automatically opens that new
    branch.  New leafs and branches will be added to this branch until you do
    a close.  The normal sequence of operations is open, leaf, leaf... close.
    </doc>
    <argument name = "name" type = "char *">The name of the new branch</argument>
    <local>
    ipr_xml_t
        *new_branch;
    </local>
    //
    assert (self->cur_branch);
    new_branch = ipr_xml_new (self->cur_branch, name, NULL);
    ipr_xml_unlink (&self->cur_branch);
    self->cur_branch = new_branch;
</method>

<method name = "leaf" template = "function">
    <doc>
    Adds a new property (leaf) to the current branch.  A leaf has a name and a
    value.  The value may be a format string, and be followed by arguments.  If
    the name is NULL, the value is used as the branch content value.
    </doc>
    <argument name = "name" type = "char *">The name of the new leaf</argument>
    <argument name = "format" type = "char *">Format specifier</argument>
    <argument name = "args" type = "...">Variable arguments</argument>
    //
    <local>
    char
        *string;                        //  Holds formatted value
    int
        str_size = 1024,
        fmt_size;
    </local>
    //
    assert (self->cur_branch);

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
    if (name)
        ipr_xml_attr_set (self->cur_branch, name, string);
    else
        ipr_xml_value_set (self->cur_branch, string);
    icl_mem_free (string);
</method>

<method name = "shut" template = "function">
    <doc>
    Climbs back along the current branch.  A close must match a previous open.
    Once you have shut a branch there is no way to reopen it.  Doing too many
    shuts will cause an assertion failure.
    </doc>
    //
    assert (self->cur_branch && self->cur_branch != self->root_item);
    self->cur_branch = ipr_xml_parent (&self->cur_branch);
</method>

<method name = "save xml" return = "output">
    <doc>
    Format the XML tree as a user-friendly XML string and return it in a
    fresh icl_longstr_t. The caller should destroy the longstr when finished
    using it. Note that this method does not do meta characters escaping.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to tree</argument>
    <declare name = "output" type = "icl_longstr_t *" />
    //
    output = icl_longstr_new (NULL, 2000);
    icl_longstr_cat (output, "&lt;?xml version=\\"1.0\\"?&gt;\\n");
    s_save_xml_item (output, self->root_item, 0);
</method>

<method name = "save json" return = "output">
    <doc>
    Format the XML tree as a user-friendly JSON string and return it in a
    fresh icl_longstr_t. The caller should destroy the longstr when finished
    using it. Note that this method does not do meta characters escaping.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to tree</argument>
    <declare name = "output" type = "icl_longstr_t *" />
    //
    output = icl_longstr_new (NULL, 2000);
    icl_longstr_cat (output, "{\\n");
    s_save_json_item (output, self->root_item, 0, FALSE, FALSE);
    icl_longstr_cat (output, "\\n}\\n");
</method>

<private name = "header">
void s_save_xml_item  (icl_longstr_t *output, ipr_xml_t *item, size_t indent);
void s_save_json_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent,
    Bool glue_prev, Bool glue_next);
</private>

<private name = "footer">
void
s_save_xml_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent)
{
    ipr_xml_t
        *child;
    icl_shortstr_t
        blanks;
    ipr_xml_attr_t
        *attr;

    assert (ipr_xml_name (item));
    assert (indent < 256);
    child = ipr_xml_first_child (item);
    icl_longstr_cat (output, "%s&lt;%s", ipr_str_block (blanks, ' ', indent), ipr_xml_name (item));
    IPR_FOR_XML_ATTRIBUTES (attr, item) {
        icl_longstr_cat (output, " %s = \\"%s\\"", ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
    }
    if (child) {
        icl_longstr_cat (output, "&gt;\\n");
        while (child) {
            s_save_xml_item (output, child, indent + 4);
            child = ipr_xml_next_sibling (&child);
        }
        icl_longstr_cat (output, "%s&lt;/%s&gt;\\n", ipr_str_block (blanks, ' ', indent), ipr_xml_name (item));
    }
    else
        icl_longstr_cat (output, " /&gt;\\n");
}

void
s_save_json_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent,
    Bool glue_prev, Bool glue_next)
{
    ipr_xml_t
        *child;                         //  Child items
    icl_shortstr_t
        blanks;                         //  Indentation spacing
    ipr_xml_attr_t
        *attr;
    char
        *comma = "";
    char
        *item_name,
        *prev_name,                     //  Previous child name
        *next_name;                     //  Following child name

    assert (ipr_xml_name (item));
    assert (indent < 256);
    child = ipr_xml_first_child (item);
    ipr_str_block (blanks, ' ', indent);

    if (glue_prev)
        icl_longstr_cat (output, "%s     {", blanks);
    else {
        icl_longstr_cat (output, "%s\\"%s\\":%s",
            blanks, ipr_xml_name (item), indent? "[ ": "");
        if (glue_next)
            icl_longstr_cat (output, "\\n%s    {", blanks);
        else
            icl_longstr_cat (output, "{");
    }

    IPR_FOR_XML_ATTRIBUTES (attr, item) {
        if (ipr_str_numeric (ipr_xml_attr_value (attr)))
            icl_longstr_cat (output, "%s \\"%s\\":%s",
                comma, ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
        else
            icl_longstr_cat (output, "%s \\"%s\\":\\"%s\\"",
                comma, ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
        comma = ",";
    }
    if (child) {
        while (child) {
            icl_longstr_cat (output, "%s\\n", comma);
            item_name = ipr_xml_name (child);
            prev_name = ipr_xml_name ((ipr_xml_t *) child->list_prev);
            next_name = ipr_xml_name ((ipr_xml_t *) child->list_next);
            s_save_json_item (output, child, indent + 4,
                prev_name && streq (item_name, prev_name),
                next_name && streq (item_name, next_name));
            child = ipr_xml_next_sibling (&child);
            comma = ",";
        }
        icl_longstr_cat (output, "\\n%s   }", ipr_str_block (blanks, ' ', indent));
    }
    else
        icl_longstr_cat (output, " }");

    if (indent && !glue_next)
        icl_longstr_cat (output, " ]");
}
</private>

<method name = "selftest" inherit = "none">
    <local>
    ipr_tree_t
        *tree;
    icl_longstr_t
        *longstr;
    </local>
    //
    tree = ipr_tree_new ("restms");
    assert (tree);
    ipr_tree_leaf (tree, "version", "1.0");
    ipr_tree_leaf (tree, "status", "ok");

      ipr_tree_open (tree, "pipe_class");
        ipr_tree_leaf (tree, "name", "name");
        ipr_tree_leaf (tree, "uri", "uri");
        ipr_tree_leaf (tree, "size", "24");
      ipr_tree_shut (tree);

      ipr_tree_open (tree, "feed_class");
        ipr_tree_leaf (tree, "name", "name");
        ipr_tree_leaf (tree, "uri", "uri");
          ipr_tree_open (tree, "feed");
            ipr_tree_leaf (tree, "name", "name1");
            ipr_tree_leaf (tree, "uri", "uri1");
          ipr_tree_shut (tree);
          ipr_tree_open (tree, "feed");
            ipr_tree_leaf (tree, "name", "name2");
            ipr_tree_leaf (tree, "uri", "uri2");
          ipr_tree_shut (tree);
          ipr_tree_open (tree, "feed");
            ipr_tree_leaf (tree, "name", "name3");
            ipr_tree_leaf (tree, "uri", "uri3");
          ipr_tree_shut (tree);
      ipr_tree_shut (tree);

      ipr_tree_open (tree, "pipe");
        ipr_tree_leaf (tree, "name", "name");
        ipr_tree_leaf (tree, "uri", "uri");
        ipr_tree_leaf (tree, "size", "0");
          ipr_tree_open (tree, "join");
            ipr_tree_leaf (tree, "address", "pattern");
            ipr_tree_leaf (tree, "feed", "name");
            ipr_tree_leaf (tree, "uri", "uri");
          ipr_tree_shut (tree);
          ipr_tree_open (tree, "nozzle");
            ipr_tree_leaf (tree, "name", "name");
            ipr_tree_leaf (tree, "size", "123");
          ipr_tree_shut (tree);
      ipr_tree_shut (tree);

      ipr_tree_open (tree, "feed");
        ipr_tree_leaf (tree, "name", "name");
        ipr_tree_leaf (tree, "uri", "uri");
      ipr_tree_shut (tree);

    longstr = ipr_tree_save_xml (tree);
    icl_longstr_destroy (&longstr);

    longstr = ipr_tree_save_json (tree);
    icl_longstr_destroy (&longstr);

    ipr_tree_destroy (&tree);
</method>

</class>
