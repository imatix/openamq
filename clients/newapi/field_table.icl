<?xml?>
<class
    name      = "field_table"
    comment   = "Simple wrapper for AMQ field table"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object" />

<import class = "asl_field_list" type = "private" />
<import class = "icl_longstr" type = "private" />

<context>
    asl_field_list_t
        *list;
</context>

<method name = "new">
    self->list = asl_field_list_new (NULL);
    assert (self->list);
</method>

<method name = "destroy">
    asl_field_list_destroy (&self->list);
</method>

<method name = "add string" template = "function">
    <doc>
    Adds string value to the field table
    </doc>
    <argument name = "name" type = "char*" const = "1" />
    <argument name = "value" type = "char*" const = "1" />
    <declare name = "str" type = "icl_longstr_t*" />
    str = icl_longstr_new (value, strlen (value));
    assert (str);
    asl_field_new_string (self->list, name, str->data);
    icl_longstr_destroy (&str);
</method>

<method name = "add integer" template = "function">
    <doc>
    Adds integer value to the field table
    </doc>
    <argument name = "name" type = "char*" const = "1" />
    <argument name = "value" type = "int" />
    asl_field_new_integer (self->list, name, value);
</method>

<method name = "get string" template = "function" return = "rc">
    <doc>
    Retrieves string value from the field table
    </doc>
    <argument name = "name" type = "char*" />
    <declare name = "rc" type = "char*" />
    rc = asl_field_list_string (self->list, name);
</method>

<method name = "get integer" template = "function" return = "rc">
    <doc>
    Retrieves integer value from the field table
    </doc>
    <argument name = "name" type = "char*" />
    <declare name = "rc" type = "int" />
    rc = asl_field_list_integer (self->list, name);
</method>

<method name = "selftest">
</method>

</class>
