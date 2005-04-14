<?xml?>
<class
    name      = "amq_field_list"
    comment   = "Implements the list container for amq_field"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

Provides functionality to implement the AMQ RFC 006 'Field Table'
data type.  This class provides functions at the field-table level.
Each field table is represented as a list of fields.  This class
provides methods to transform field lists to and from AMQP binary
formats, and lookup and operate on field lists.

<inherit class = "ipr_list_head" />

<import class = "amq_field" />
<option name = "childname" value = "amq_field" />
<option name = "childtype" value = "amq_field_t" />

<context>
    amq_field_t
        amq_field;                   /*  Self starts with child object    */
</context>

<method name = "new">
</method>

<method name = "destroy">
</method>

<method name = "parse" template = "function">
    <doc>
    Parses a longstr string into a list of fields.  Does not empty the
    list before appending new fields to it.  Returns the number of fields
    parsed, or -1 if the supplied string was invalid.
    </doc>
    <argument name = "field table" type = "ipr_longstr_t *">Field table</argument>
    <local>
    byte
        *input,                         /*  Current position in string       */
        *limit;                         /*  Limit of string (max + 1)        */
    </local>
    assert (field_table);

    input = field_table->data;
    limit = input + field_table->cur_size;
    while (input &lt; limit) {
        input = amq_field_parse (self, input, limit);
        if (!input) {
            rc = -1;                    /*  Error in fields                  */
            break;
        }
    }
</method>

<method name = "flatten" return = "field table">
    <doc>
    Encodes the field list as a longstr string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "field table" type = "ipr_longstr_t *">Field table</declare>
    <local>
    size_t
        flat_size = 0;
    amq_field_t
        *field;                         /*  Field object                     */
    </local>

    /*  Compute required total size for all fields                           */
    field = self_first (self);
    while (field) {
        flat_size += amq_field_flat_size (field);
        field = self_next (self, field);
    }
    /*  Flatten all fields into a string                                     */
    field_table = ipr_longstr_new (NULL, flat_size);
    field = self_first (self);
    while (field) {
        amq_field_flatten (field, field_table);
        field = self_next (self, field);
    }
</method>

<method name = "search" return = "field">
    <doc>
    Looks for a field with the specified name, returns the field object
    if found, else NULL.  Field names are case-significant.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "name" type = "char *"       >Field name</argument>
    <declare name = "field" type = "amq_field_t *">Field object</declare>
    field = self_first (self);
    while (field) {
        if (streq (field->name, name))
            return (field);
        field = self_next (self, field);
    }
    return (NULL);
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
    amq_field_t
        *field;
    </local>
    field = self_search (self, name);
    if (field)
        value = amq_field_string (field);
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
    amq_field_t
        *field;
    </local>
    field = self_search (self, name);
    if (field)
        value = amq_field_integer (field);
    else
        value = 0;
</method>

<method name = "print" template = "function">
    <doc>
    Dumps the contents of the field list for debugging purposes.
    </doc>
    <local>
    amq_field_t
        *field;                         /*  Field object                     */
    </local>
    field = self_first (self);
    while (field) {
        amq_field_print (field);
        field = self_next (self, field);
    }
</method>


<method name = "selftest">
    <local>
    amq_field_list_t
        *list;
    ipr_longstr_t
        *string;
    </local>

    list = amq_field_list_new ();
    amq_field_new_string  (list, "testfield0", "Commodity middleware");
    amq_field_new_integer (list, "testfield1", 1234567890);
    amq_field_new_decimal (list, "testfield2", 199900, 2);
    amq_field_new_time    (list, "testfield3", time (NULL));
    amq_field_list_print  (list);

    string = amq_field_list_flatten (list);
    amq_field_list_destroy (&list);

    list = amq_field_list_new ();
    amq_field_list_parse (list, string);

    coprintf (" - should be identical:");
    amq_field_list_print (list);

    amq_field_list_destroy (&list);
    ipr_longstr_destroy (&string);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
