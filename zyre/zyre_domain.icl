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

<inherit class = "zyre_resource_back" />

<context>
    icl_shortstr_t
        name;                           //  Domain name
</context>

<method name = "new">
  <!-- New method is used by portal, does not accept any arguments -->
</method>

<method name = "destroy">
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_XML_ROOT);
    ipr_tree_leaf (tree, "xmlns", "http://www.imatix.com/schema/restms");
    ipr_tree_open (tree, zyre_resource_type_name (portal->type));
    ipr_tree_leaf (tree, "name", self->name);
    ipr_tree_shut (tree);
    zyre_resource_report (portal, context, tree);
    ipr_tree_destroy (&tree);
</method>

<method name = "put">
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The PUT method is not allowed on this resource");
</method>

<method name = "delete">
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The DELETE method is not allowed on this resource");
</method>

<method name = "post">
    http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
        "The POST method is not allowed on this resource");
</method>

<method name = "selftest" />

</class>
