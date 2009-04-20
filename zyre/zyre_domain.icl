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
    Bool
        dynamic;                        //  Dynamic resource?
    icl_shortstr_t
        title;                          //  Title text
</context>

<method name = "new">
    <!-- New method is used by portal, does not accept any arguments -->
    icl_shortstr_cpy (self->title, "Default domain");
    self->dynamic = FALSE;              //  Later, set in configure
</method>

<method name = "destroy">
</method>

<method name = "configure">
    //  If the context is null, configure a default domain
    if (context)
        self->dynamic = TRUE;
    else
        self->dynamic = FALSE;
</method>

<method name = "get">
    <local>
    ipr_tree_t
        *tree;
    </local>
    //
    tree = ipr_tree_new (RESTMS_ROOT);
    ipr_tree_open (tree, "domain");
    ipr_tree_leaf (tree, "name", portal->name);
    ipr_tree_leaf (tree, "title", self->title);
    zyre_resource_to_document (portal, context, &tree);
</method>

<method name = "put">
    if (self->dynamic)
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "may not modify this domain");
    else
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "may not modify this domain");
</method>

<method name = "delete">
    if (self->dynamic || context == NULL)
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "still may not delete this domain");
    else
        http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
            "may not delete this domain");
</method>

<method name = "post">
    if (zyre_restms_parse_document (context, NULL) == 0) {
        if (context->xml_item) {
            if (streq (ipr_xml_name (context->xml_item), "pipe")) {
                if (zyre_pipe_spec_valid (context))
                    zyre_resource_response_child_add (portal, context);
            }
            else
            if (streq (ipr_xml_name (context->xml_item), "feed")) {
                if (zyre_feed_spec_valid (context))
                    zyre_resource_response_child_add (portal, context);
            }
            else
                http_driver_context_reply_error (context, HTTP_REPLY_FORBIDDEN,
                    "may only create new pipe or feed resources here");
        }
        else
            http_driver_context_reply_error (context, HTTP_REPLY_BADREQUEST,
                "XML must specified resource type");
    }
</method>

<method name = "selftest" />

</class>
