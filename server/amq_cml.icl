<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "amq_cml"
    comment   = "CML module"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The CML module provides functions to read and write CML commands.
</doc>
<inherit class = "icl_base" />
<import class = "amq_server_classes" />

<method name = "encode" return = "content">
    <doc>
    Formats a CML command using the specified information, returns a
    basic content containing the command.  If the fields argument is
    not null, the fields are included in the command.
    </doc>
    <argument name = "name" type = "char *">Command name</argument>
    <argument name = "object class" type = "char *">Object class name</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <argument name = "status" type = "char *">Status, or null</argument>
    <argument name = "method" type = "char *">Method name, or null</argument>
    <argument name = "fields" type = "asl_field_list_t *">Object fields</argument>
    <declare name = "content" type = "amq_content_basic_t *">Content</declare>
    <local>
    asl_field_t
        *field;
    ipr_xml_t
        *cml_item,                      //  CML item
        *cur_item,                      //  Top level object
        *sub_item,                      //  Field or class within object
        *val_item;                      //  Value of field
    icl_shortstr_t
        strvalue;                       //  Stringified numeric value
    char
        *xml_text;                      //  Serialised XML text
    ipr_bucket_t
        *bucket;
    </local>
    //
    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

    cur_item = ipr_xml_new (cml_item, name, NULL);
    ipr_xml_attr_set (cur_item, "class",  object_class);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%d", object_id));
    
    if (status)
        ipr_xml_attr_set (cur_item, "status", status);
    if (method)
        ipr_xml_attr_set (cur_item, "name", method);

    if (fields) {
        field = asl_field_list_first (fields);
        while (field) {
            sub_item = ipr_xml_new (cur_item, "field", NULL);
            ipr_xml_attr_set (sub_item, "name", field->name);
            val_item = ipr_xml_new (sub_item, NULL, asl_field_string (field));
            ipr_xml_unlink (&val_item);
            ipr_xml_unlink (&sub_item);
            field = asl_field_list_next (&field);
        }
    }
    xml_text = ipr_xml_save_string (cml_item);
    bucket = ipr_bucket_new (strlen (xml_text) + 1);
    ipr_bucket_fill (bucket, xml_text, strlen (xml_text) + 1);
    icl_mem_free (xml_text);

    //  Create a content with our desired reply data
    content = amq_content_basic_new ();
    amq_content_basic_set_content_type (content, "text/xml");
    amq_content_basic_record_body      (content, bucket);

    ipr_bucket_unlink (&bucket);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
</method>

<method name = "selftest">
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_cml_encode ("test-method", "myobject", 123, "ok", NULL, NULL);
    amq_content_basic_unlink (&content);
</method>

</class>

