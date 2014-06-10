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
    name      = "asl_field_list"
    comment   = "Implements the list container for asl_field"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at the field-table level.
    Each field table is represented as a list of fields.  This class
    provides methods to transform field lists to and from aslP binary
    formats, and lookup and operate on field lists.
</doc>

<inherit class = "icl_list_head" >
    <option name = "links"     value = "1" />
    <option name = "prefix"    value = "list" />
    <option name = "childname" value = "asl_field" />
    <option name = "childtype" value = "asl_field_t" />
</inherit>

<import class = "asl_field" />
<option name = "rwlock" value = "0"/>

<context>
</context>

<method name = "new">
    <doc>
    If the field table argument is not null, parses the field table into
    the field list.  If the field table contained invalid data, destroys
    self.
    </doc>
    <argument name = "field table" type = "icl_longstr_t *" pass = "in">Field table</argument>
    //
    if (field_table)
        if (self_parse (self, field_table))
            self_destroy (&self);
</method>

<method name = "destroy">
</method>

<method name = "parse" template = "function">
    <doc>
    Parses a longstr string into a list of fields.  Does not empty the
    list before appending new fields to it.  Returns the number of fields
    parsed, or -1 if the supplied string was invalid.
    </doc>
    <argument name = "field table" type = "icl_longstr_t *">Field table</argument>
    <local>
    byte
        *input,                         //  Current position in string
        *limit;                         //  Limit of string (max + 1)
    </local>
    //
    assert (field_table);

    input = field_table->data;
    limit = input + field_table->cur_size;
    while (input &lt; limit) {
        input = asl_field_parse (self, input, limit);
        if (!input) {
            rc = -1;                    //  Error in fields
            break;
        }
    }
</method>

<method name = "flatten" return = "field table">
    <doc>
    Encodes the field list as a longstr string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "field table" type = "icl_longstr_t *">Field table</declare>
    <local>
    size_t
        flat_size = 0;
    asl_field_t
        *field;                         //  Field object
    </local>
    //
    //  Compute required total size for all fields
    field = self_first (self);
    while (field) {
        flat_size += asl_field_flat_size (field);
        field = self_next (&field);
    }
    //  Flatten all fields into a string
    field_table = icl_longstr_new (NULL, flat_size);
    field = self_first (self);
    while (field) {
        asl_field_flatten (field, field_table);
        field = self_next (&field);
    }
</method>

<method name = "search" return = "field">
    <doc>
    Looks for a field with the specified name, returns the field object
    if found, else NULL.  Field names are case-significant.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *"       >Field name</argument>
    <declare name = "field" type = "asl_field_t *">Field object</declare>
    //
    field = self_first (self);
    while (field) {
        if (streq (field->name, name))
            break;
        field = self_next (&field);
    }
</method>

<method name = "cpy" template = "function">
    <doc>
    Looks for a field with the specified name, if found copies the
    field value into the specified string, else clears the string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "dest" type = "char *"       >Destination shortstr</argument>
    <argument name = "name" type = "char *"       >Field name</argument>
    <local>
    asl_field_t
        *field;
    </local>
    //
    field = asl_field_list_search (self, name);
    if (field) {
        icl_shortstr_cpy (dest, asl_field_string (field));
        asl_field_unlink (&field);
    }
    else
        strclr (dest);
</method>

<method name = "string" return = "value">
    <doc>
    Looks for a field with the specified name, returns the field's string
    value if found, else NULL.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *"       >Field name</argument>
    <declare name = "value" type = "char *">String value to return</declare>
    <local>
    asl_field_t
        *field;
    </local>
    //
    field = self_search (self, name);
    if (field) {
        value = asl_field_string (field);
        asl_field_unlink (&field);
    }
    else
        value = NULL;
</method>

<method name = "integer" return = "value">
    <doc>
    Looks for a field with the specified name, returns the field's integer
    value if found, else zero.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *"       >Field name</argument>
    <declare name = "value" type = "long">Integer value to return</declare>
    <local>
    asl_field_t
        *field;
    </local>
    //
    field = self_search (self, name);
    if (field) {
        value = asl_field_integer (field);
        asl_field_unlink (&field);
    }
    else
        value = 0;
</method>

<method name = "print" template = "function">
    <doc>
    Dumps the contents of the field list for debugging purposes.
    </doc>
    <local>
    asl_field_t
        *field;                         //  Field object
    </local>
    //
    field = self_first (self);
    while (field) {
        asl_field_print (field);
        field = self_next (&field);
    }
</method>

<method name = "dump" template = "function">
    <argument name = "dest" type = "char *" />
    <doc>
    Dumps the contents of the field list as a NULL terminated icl_shortstr_t
    of NAME=VALUE pairs separated by spaces.
    </doc>
    <local>
    asl_field_t
        *field;                         //  Field object
    icl_longstr_t
        *string;                        //  Used to build up the final string
    </local>
    //
    //  Create empty longstr to store the result
    string = icl_longstr_new (NULL, ICL_SHORTSTR_MAX + 1);
    string->data [0] = 0;

    //  Dump individual fields of the list
    field = self_first (self);
    while (field) {
        //  Need a copy here since string may get reallocated
        char
            *append = icl_mem_strdup ((char *) string->data);

        icl_longstr_fmt (string, "%s%s=%s ", append, field->name, asl_field_string (field));
        field = self_next (&field);
        icl_mem_strfree (&append);
    }
    string->data [ICL_SHORTSTR_MAX] = 0;
    icl_shortstr_cpy (dest, (char *) string->data);
    icl_longstr_destroy (&string);
</method>

<method name = "to tree" template = "function">
    <argument name = "tree" type = "ipr_tree_t *" />
    <doc>
    Converts the field list to a set of leaves in the provided tree.  
    The tree is built using the ipr_tree class.
    </doc>
    <local>
    asl_field_t
        *field;                         //  Field object
    </local>
    //
    //  Dump individual fields of the list
    field = self_first (self);
    while (field) {
        ipr_tree_leaf (tree, field->name, asl_field_string (field));
        field = self_next (&field);
    }
</method>

<method name = "build" return = "string">
    <doc>
    Builds a flattened field list suitable for sending across a socket
    or writing to a file.  The field list is limited to string values.
    Accepts a series of name, value argument pairs, ending in a null
    name. Returns a icl_longstr, which the caller must destroy after use.
    </doc>
    <argument name = "start"  type = "char *" >First field name</argument>
    <argument name = "argptr" type = "..." >Variable arguments</argument>
    <declare name =  "string" type = "icl_longstr_t *">Flat field list</declare>
    <local>
    char
        *field_name,
        *field_value;
    asl_field_list_t
        *field_list;
    </local>
    //
    field_name = start;
    field_list = self_new (NULL);
    while (field_name) {
        field_value = va_arg (argptr, char *);
        asl_field_new_string (field_list, field_name, field_value);
        field_name  = va_arg (argptr, char *);
    }
    string = asl_field_list_flatten (field_list);
    asl_field_list_destroy (&field_list);
</method>

<method name = "selftest">
    <local>
    asl_field_list_t
        *list;
    icl_longstr_t
        *string1,
        *string2;
    icl_shortstr_t
        shortstr;
    </local>
    //
    list = asl_field_list_new (NULL);
    asl_field_new_string  (list, "testfield0", "Collateral damage");
    asl_field_new_integer (list, "testfield1", 1234567890);
    asl_field_new_decimal (list, "testfield2", 199900, 2);
    asl_field_new_time    (list, "testfield3", time (NULL));
    asl_field_new_string  (list, "testfield4", "S1");
    asl_field_new_string  (list, "testfield5", "P12");
    string1 = asl_field_list_flatten (list);
    asl_field_list_dump (list, shortstr);
    asl_field_list_destroy (&list);

    list = asl_field_list_new (string1);
    string2 = asl_field_list_flatten (list);
    asl_field_list_destroy (&list);

    assert (string1->cur_size == string2->cur_size);
    icl_longstr_destroy (&string1);
    icl_longstr_destroy (&string2);

    //  We can build a flattened field list in one step
    string1 = asl_field_list_build (
        "host",  "Sleeper Service",
        "guest", "My Homework Ate My Dog",
        NULL);
    icl_longstr_destroy (&string1);
</method>

</class>
