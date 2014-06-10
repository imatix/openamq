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
    name      = "ipr_xml_attr"
    comment   = "iPR console class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    >

<doc>
    Implements XML attributes.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "rwlock" value = "1" />
</inherit>

<import class = "ipr_xml"/>
<import class = "icl_mem" />

<context>
    char
        *name,                          //  Item name, allocated string        
        *value;                         //  Value node, allocated string       
</context>

<method name = "new">
    <argument name = "parent" type = "ipr_xml_t *">The XML item to which the attribute belongs</argument>
    <argument name = "name"  type = "char *">The name of the new item</argument>
    <argument name = "value" type = "char *">The value of the new item</argument>
    if (name)
        self->name  = icl_mem_strdup (name);
    if (value)
        self->value = icl_mem_strdup (value);
    ipr_xml_attr_list_queue (parent->attrs, self);
</method>

<method name = "destroy">
    icl_mem_free (self->name);
    icl_mem_free (self->value);
</method>

<method name = "name" return = "name">
    <argument name = "self"   type = "$(selftype) *"/>
    <declare  name = "name"   type = "char *"/>
    name = self->name;
</method>

<method name = "value" return = "value">
    <argument name = "self"   type = "$(selftype) *"/>
    <declare  name = "value"   type = "char *"/>
    value = self->value;
</method>

<method name = "modify value">
    <inherit name = "wrlock"/>
    <argument name = "self"   type = "$(selftype) *"/>
    <argument name = "value"  type = "char *"/>
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

<method name = "lookup" return = "attr" private = "1">
    <argument name = "item"  type = "ipr_xml_t *">The XML item</argument>
    <argument name = "name"  type = "char *">The attribute name</argument>
    <declare  name = "attr"  type = "ipr_xml_attr_t *">The attribute</declare>
    attr = ipr_xml_attr_list_first (item->attrs);
    while (attr) {
        if (attr->name
        &&  streq (name, attr->name))
            break;
        else
            attr = ipr_xml_attr_list_next (&attr);
    }
</method>

<method name = "get" return = "value">
    <argument name = "item"  type = "ipr_xml_t *">The XML item</argument>
    <argument name = "name"  type = "char *">The attribute name</argument>
    <argument name = "dflt"  type = "char *">The value to return if the attribute is undefined</argument>
    <declare  name = "attr"  type = "ipr_xml_attr_t *">The attribute</declare>
    <declare  name = "value" type = "char *" default = "NULL">The attribute value</declare>
    attr = ipr_xml_attr_lookup (item, name);
    if (attr)
        value = ipr_xml_attr_value (attr);
    if (!value)
        value = dflt;

    ipr_xml_attr_unlink (&attr);
</method>

<method name = "set">
    <argument name = "item"  type = "ipr_xml_t *">The XML item</argument>
    <argument name = "name"  type = "char *">The attribute name</argument>
    <argument name = "value" type = "char *">The attribute value</argument>
    <local>
    ipr_xml_attr_t
        *attr;
    </local>
    attr = ipr_xml_attr_lookup (item, name);
    if (attr)
        ipr_xml_attr_modify_value (attr, value);
    else
        attr = ipr_xml_attr_new (item, name, value);
        
    ipr_xml_attr_unlink (&attr);
</method>

<method name = "first" return = "attr">
    <argument name = "item"  type = "ipr_xml_t *">The XML item</argument>
    <declare  name = "attr"  type = "ipr_xml_attr_t *">The attribute</declare>
    attr = ipr_xml_attr_list_first (item->attrs);
</method>

<method name = "next" return = "next">
    <argument name = "attr"  type = "ipr_xml_attr_t **">The attribute</argument>
    <declare  name = "next"  type = "ipr_xml_attr_t *">The next attribute</declare>
    next = ipr_xml_attr_list_next (attr);
</method>

<method name = "selftest">
</method>

</class>
