<?xml?>
<class
    name      = "amq_field"
    comment   = "Field class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Provides functionality to implement the AMQ RFC 006 'Field Table'
data type.  This class provides functions at a per-field level.
</doc>

<inherit class = "ipr_list_item" />

<import class = "ipr" />

<option name = "nullify" value = "0" />

<public name = "header">
#define AMQ_FIELD_TYPE_STRING       'S'
#define AMQ_FIELD_TYPE_INTEGER      'I'
#define AMQ_FIELD_TYPE_DECIMAL      'D'
#define AMQ_FIELD_TYPE_TIME         'T'
#define AMQ_FIELD_NAME_MAX          128
</public>

<private name = "header">
/*  Network byte ordering                                                    */

#define GET_SHORT(host, net) \\
    (host) = (net [0] << 8) + net [1]; \\
    net += 2
#define GET_LONG(host, net) \\
    (host) = (net [0] << 24) + (net [1] << 16) + (net [2] << 8) + net [3]; \\
    net += 4
#define PUT_SHORT(net, host) \\
    net [0] = (byte) ((host >> 8) & 255); \\
    net [1] = (byte) ((host)      & 255); \\
    net += 2
#define PUT_LONG(net, host) \\
    net [0] = (byte) ((host >> 24) & 255); \\
    net [1] = (byte) ((host >> 16) & 255); \\
    net [2] = (byte) ((host >> 8)  & 255); \\
    net [3] = (byte) ((host)       & 255); \\
    net += 4
</private>

<context>
    ipr_shortstr_t
        name;                           /*  Field name                       */
    byte
        type;                           /*  Field type                       */
    byte
        decimals;                       /*  Decimals, if necessary           */
    ipr_longstr_t
        *string;                        /*  Value as string                  */
    int64_t
        integer;                        /*  Value as integer, decimal, time  */
</context>

<method name = "new">
    strclr (self->name);
    self->type     = 0;
    self->decimals = 0;
    self->integer  = 0;
    self->string   = NULL;
</method>

<method name = "destroy">
    ipr_longstr_destroy (&self->string);
</method>

<method name = "new string">
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "char *">Field string value</argument>
    <local>
    amq_field_t
        *self;
    </local>
    assert (strlen (name) <= AMQ_FIELD_NAME_MAX);

    self = self_new ();
    strcpy (self->name, name);
    self->type   = AMQ_FIELD_TYPE_STRING;
    self->string = ipr_longstr_new (value, strlen (value) + 1);
    self->string->data [strlen (value)] = '\\0';
    amq_field_list_queue (list, self);
</method>

<method name = "new integer">
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "int"   >Field integer value</argument>
    <local>
    amq_field_t
        *self;
    </local>
    assert (strlen (name) <= AMQ_FIELD_NAME_MAX);

    self = self_new ();
    strcpy (self->name, name);
    self->type    = AMQ_FIELD_TYPE_INTEGER;
    self->integer = value;
    amq_field_list_queue (list, self);
</method>

<method name = "new decimal">
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "long"  >Field decimal value</argument>
    <argument name = "decimals" type = "int">Number of decimal places</argument>
    <local>
    amq_field_t
        *self;
    </local>
    assert (strlen (name) <= AMQ_FIELD_NAME_MAX);

    self = self_new ();
    strcpy (self->name, name);
    self->type     = AMQ_FIELD_TYPE_DECIMAL;
    self->integer  = value;
    self->decimals = decimals;
    amq_field_list_queue (list, self);
</method>

<method name = "new time">
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "time_t">Field time value</argument>
    <local>
    amq_field_t
        *self;
    </local>
    assert (strlen (name) <= AMQ_FIELD_NAME_MAX);

    self = self_new ();
    strcpy (self->name, name);
    self->type    = AMQ_FIELD_TYPE_TIME;
    self->integer = value;
    amq_field_list_queue (list, self);
</method>

<method name = "parse" return = "field_end">
    <doc>
    Parses a field definition. Returns the address of the first octet after
    the field definition, or NULL if there was an error parsing the field.
    The limit points to the octet following the last legal octet of the string.
    </doc>
    <argument name = "list"  type = "amq_field_list_t *">Parent field list</argument>
    <argument name = "input" type = "byte *">Field definition structure</argument>
    <argument name = "limit" type = "byte *">End of field-table string</argument>
    <declare name = "field_end" type = "byte *">Calculated end position</declare>
    <local>
    amq_field_t
        *self;
    size_t
        string_size;
    </local>
    assert (list);

    self = self_new ();

    /*  Copy field name                                                      */
    //TODO: validate that the field name is SQL-92 compliant
    string_size = *input;
    input += 1;
    memcpy (self->name, input, string_size);
    self->name [string_size] = 0;
    input += string_size;

    assert (input <= limit);
    self->type = *input;
    input += 1;

    if (self->type == AMQ_FIELD_TYPE_STRING) {
        GET_SHORT (string_size, input);
        self->string = ipr_longstr_new (NULL, string_size + 1);
        memcpy (self->string->data, input, string_size);
        self->string->data [string_size] = '\\0';
        self->string->cur_size = string_size;
        input += string_size;
        field_end = input;
    }
    else
    if (self->type == AMQ_FIELD_TYPE_INTEGER) {
        GET_LONG (self->integer, input);
        field_end = input;
    }
    else
    if (self->type == AMQ_FIELD_TYPE_DECIMAL) {
        self->decimals = *input;
        input += 1;
        GET_LONG (self->integer, input);
        field_end = input;
    }
    else
    if (self->type == AMQ_FIELD_TYPE_TIME) {
        GET_LONG (self->integer, input);
        field_end = input;
    }
    else {
        coprintf ("$(selfname): invalid field type - '%c'", *input);
        field_end = NULL;
    }
    if (field_end == NULL || input > limit) {
        coprintf ("$(selfname): field definition badly formed - rejected");
        field_end = NULL;
        self_destroy (&self);
    }
    else
        amq_field_list_queue (list, self);
</method>

<method name = "set string">
    <doc>
    Formats string value for field.  If the field is not a string, will
    attempt to convert the field value into a string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    assert (self);

    if (self->type != AMQ_FIELD_TYPE_STRING) {
        /*  Add a string value and format the field accordingly              */
        if (!self->string)
            self->string = ipr_longstr_new (NULL, 30);

        if (self->type == AMQ_FIELD_TYPE_INTEGER)
            ipr_longstr_fmt (self->string, "%d", self->integer);
        else
        if (self->type == AMQ_FIELD_TYPE_DECIMAL)
            ipr_longstr_fmt (self->string, "%g",
                (double) self->integer / exp (self->decimals * log (10.0)));
        else
        if (self->type == AMQ_FIELD_TYPE_TIME) {
            struct tm
                *time_struct;
            time_struct = gmtime ((time_t *) &self->integer);
            ipr_longstr_fmt (self->string, "%4d/%02d/%02dT%02d:%02d:%02dZ",
                self->name,
                time_struct->tm_year + 1900,
                time_struct->tm_mon + 1,
                time_struct->tm_mday,
                time_struct->tm_hour,
                time_struct->tm_min,
                time_struct->tm_sec);
        }
    }
</method>

<method name = "integer" return = "value">
    <doc>
    Returns integer value for field.  If the field is not an integer, will
    attempt to convert the field into an integer.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "value" type = "long">Returned integer value</declare>
    assert (self);

    value = 0;
    if (self->type == AMQ_FIELD_TYPE_STRING)
        value = atol (self->string->data);
    else
    if (self->type == AMQ_FIELD_TYPE_INTEGER)
        value = (long) self->integer;
    else
    if (self->type == AMQ_FIELD_TYPE_DECIMAL)
        value = (long) (self->integer / exp (self->decimals * log (10.0)));
    else
    if (self->type == AMQ_FIELD_TYPE_TIME)
        value = (long) self->integer;
</method>

<method name = "flatten" template = "function">
    <doc>
    Creates a field definition string from the field. Writes the
    field definition to the provided string.  Returns -1 if the field
    was invalid or there was not enough space to write the field.
    </doc>
    <argument name = "string" type = "ipr_longstr_t *">String to write to</argument>
    <local>
    byte
        *output;
    size_t
        name_length;
    </local>

    if (string->cur_size + self_flat_size (self) <= string->max_size) {
        /*  Set-up pointer to current writing position in string             */
        output = string->data + string->cur_size;

        /*  Write field name                                                 */
        name_length = strlen (self->name);
        *output = (byte) name_length;
        output += 1;
        memcpy (output, self->name, name_length);
        output += name_length;

        /*  Write field type                                                 */
        *(char *) output = self->type;
        output += 1;

        if (self->type == AMQ_FIELD_TYPE_STRING) {
            assert (self->string->cur_size < 0x10000);
            PUT_SHORT (output, self->string->cur_size);
            memcpy (output, self->string->data, self->string->cur_size);
            output += self->string->cur_size;
        }
        else
        if (self->type == AMQ_FIELD_TYPE_INTEGER) {
            PUT_LONG (output, self->integer);
        }
        else
        if (self->type == AMQ_FIELD_TYPE_DECIMAL) {
            *output = self->decimals;
            output += 1;
            PUT_LONG (output, self->integer);
        }
        else
        if (self->type == AMQ_FIELD_TYPE_TIME) {
            PUT_LONG (output, self->integer);
        }
        else
            rc = -1;

        if (rc == 0)
            string->cur_size = output - string->data;
    }
    else
        rc = -1;
</method>

<method name = "flat_size" template = "function">
    <doc>
    Returns the flattened size of the field, in octets.
    </doc>
    /*  All fields have a length-specified name and a 1-octet type           */
    rc = strlen (self->name) + 2;
    if (self->type == AMQ_FIELD_TYPE_STRING)
        rc += self->string->cur_size + 2;
    else
    if (self->type == AMQ_FIELD_TYPE_INTEGER)
        rc += 4;
    else
    if (self->type == AMQ_FIELD_TYPE_DECIMAL)
        rc += 4 + 1;
    else
    if (self->type == AMQ_FIELD_TYPE_TIME)
        rc += 4;
</method>

<method name = "print" template = "function">
    <doc>
    Prints the field format and contents to output (using coprintf).
    </doc>
    if (self->type == AMQ_FIELD_TYPE_STRING)
        coprintf (" - %s - string, length=%d content=%c%c%c%c...",
            self->name,
            self->string->cur_size,
            self->string->data [0],
            self->string->data [1],
            self->string->data [2],
            self->string->data [3]);
    else
    if (self->type == AMQ_FIELD_TYPE_INTEGER)
        coprintf (" - %s - integer, value=%d", self->name, (int) self->integer);
    else
    if (self->type == AMQ_FIELD_TYPE_DECIMAL)
        coprintf (" - %s - decimal, value=%g", self->name,
            (double) self->integer / exp (self->decimals * log (10.0)));
    else
    if (self->type == AMQ_FIELD_TYPE_TIME) {
        struct tm
            *time_struct;
        time_struct = gmtime ((time_t *) &self->integer);
        coprintf (" - %s - time, value=%4d/%02d/%02dT%02d:%02d:%02dZ",
            self->name,
            time_struct->tm_year + 1900,
            time_struct->tm_mon + 1,
            time_struct->tm_mday,
            time_struct->tm_hour,
            time_struct->tm_min,
            time_struct->tm_sec);
    }
    else
        coprintf (" - undefined field type");
</method>

<method name = "selftest">
    <local>
    </local>
</method>

</class>
