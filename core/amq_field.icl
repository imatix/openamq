<?xml?>
<class
    name      = "amq_field"
    comment   = "Field class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<doc>
Fields are passed in commands as field tables.  They hold a value and
are typed.  This class provides the basic field object.  The amq_field_list
class provides methods for organising and using lists of fields.
</doc>

<inherit class = "ipr_list_item" />

<import class = "ipr_classes" />

<public>
#define AMQ_FIELD_TYPE_SHORTSTR     1
#define AMQ_FIELD_TYPE_LONGSTR      2
#define AMQ_FIELD_TYPE_INTEGER      3
#define AMQ_FIELD_TYPE_DECIMAL      4
#define AMQ_FIELD_TYPE_TIME         5
</public>

<context>
    ipr_shortstr_t
        name;                           /*  Field name                       */
    byte
        type;                           /*  Field type                       */
    ipr_shortstr_t
        value_shortstr;                 /*  Value as short string            */
    ipr_longstr_t
        *value_longstr;                 /*  Value as long string             */
    double
        value_decimal;                  /*  Value as decimal                 */
    int64_t
        value_integer;                  /*  Value as integer or time         */
</context>

<method name = "new">
</method>

<method name = "destroy">
</method>

<method name = "parse" template = "function">
    <doc>
    Parses a field definition, which is a null-terminated string
    formatted according to AMQ RFC 006, section 'Field Tables'.
    Returns 0 if ok, -1 if the field was invalid.
    </doc>
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "input" type = "char *">Field name and value</argument>
    <local>
    amq_field_t
        *self;
    char
        *equals,
        *value;
    </local>
    ASSERT (list);

    equals = strchr (input, '=');
    if (equals) {
        self = self_new ();
        /*  TODO: validate that the field name is SQL-92 compliant           */
        *equals = '\0';                 /*  Truncate at the '=' sign         */
        ipr_shortstr_cpy (self->name, input);

        *equals = '=';                  /*  Fix it up again                  */
        value = equals + 1;
        if (*value == 'S')
            rc = s_parse_string_value (self, value);
        else
        if (*value == 'I')
            rc = s_parse_integer_value (self, value);
        else
        if (*value == 'D')
            rc = s_parse_decimal_value (self, value);
        else
        if (*value == 'T')
            rc = s_parse_time_value (self, value);

        if (rc == 0)
            self_queue (list, self);
        else
            self_destroy (&self);
    }
    else
        rc = -1;
</method>

<method name = "express" template = "function" return = "field table">
    <doc>
    Encodes the field list as a longstr string, formatted according
    to AMQ RFC 006 ('Field Tables').
    </doc>
    <declare name = "field table" type = "ipr_longstr_t *">Field table</declare>
    /*
    - go through list and count total size
    - allocate longstring
    - go through list and express each field
    */
</method>

<method name = "selftest">
    <local>
    </local>
</method>

</class>
