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
    name      = "zyre_domain"
    comment   = "Zyre domain entity"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class implements the RestMS domain object.
</doc>

<inherit class = "zyre_resource_module_back" />

<context>
    icl_shortstr_t
        name;                           //  Domain name
    zyre_resource_t
        *resource;                      //  Resource wrapper
</context>

<method name = "new">
    <argument name = "name" type = "char *">Pipe name</argument>
    <argument name = "resource" type = "zyre_resource_t *">Resource wrapper</argument>
    //
    icl_shortstr_cpy (self->name, name);
    self->resource = resource;
</method>

<method name = "destroy">
</method>

<method name = "report" template = "function">
    <doc>
    Adds XML description of the object to the provided ipr_tree.
    </doc>
    <argument name = "tree" type = "ipr_tree_t *" />
    //
    assert (tree);
    ipr_tree_open (tree, "domain");
    ipr_tree_leaf (tree, "name", self->name);
    ipr_tree_shut (tree);
</method>

<method name = "modify" template = "function">
    <doc>
    Modify the object from the provided XML element.
    </doc>
    <argument name = "xml_item" type = "ipr_xml_t *" />
    //
    assert (FALSE);                     //  Not possible
</method>

<method name = "selftest" />

</class>
